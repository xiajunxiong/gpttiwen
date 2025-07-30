#include "petrank.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"
#include "obj/character/role.h"
#include "other/pet/pet.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "servercommon/performancecatch.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "crossserver/crossserver/protocal/msgcross.h"
#include "global/datasynccheck/datasynccheck.hpp"

PetRank::PetRank()
	: m_rank_manager(NULL), m_is_all_load_finish(false), m_next_log_timestamp(0)
{
	memset(m_change_flag, 0, sizeof(m_change_flag));
	memset(m_load_flag, 0, sizeof(m_load_flag));

	for (int i = 0; i < PET_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].SetDefaultMaxSize(PET_RANK_MAX_SIZE);
		m_rank_skip_list[i].SetCompare(&PetRankItem::RankCompare);

		m_dirty_mark[i].clear();
		m_old_state[i].clear();
	}

	memset(m_last_change_time, 0, sizeof(m_last_change_time));
}

PetRank::~PetRank()
{
	for (int i = 0; i < PET_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].Clear();
	}
}

void PetRank::Init(const PetRankParam &petrank_param, int _rank_type)
{
	int count = 0;
	int max_index = -1;
	bool is_fault_tolerant = false;
	std::set<int >  check_set;

	if (petrank_param.count < PET_RANK_MAX_SIZE)
	{
		count = petrank_param.count;
	}
	else
	{
		count = PET_RANK_MAX_SIZE;
	}

	for (int i = count - 1; i >= 0; i--)
	{
		int rank_type = petrank_param.pet_rank_list[i].rank_type;
		if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::Init INVALID RANK TYPE] [rank_type:%d]", (int)rank_type);
			continue;
		}

		int rank_index = petrank_param.pet_rank_list[i].index;
		if (rank_index < 0 || rank_index >= PET_RANK_MAX_SIZE)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::Init INVALID RANK INDEX] [rank_index:%d]", (int)rank_index);
			continue;
		}

		if (petrank_param.pet_rank_list[i].role_info.uid <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::Init INVALID USER INFO] [user[%d %s]]",
				(int)petrank_param.pet_rank_list[i].role_info.uid, (const char*)petrank_param.pet_rank_list[i].role_info.user_name);
			continue;
		}

		{
			// 旧数据重复数据处理 ,sql 删除重复数据后, 会导致数据库内的数据 
			// 排名大于数据总量 所以需要把这个部分从数据库内删掉 , 并更新剩下的数据 
			if (rank_index > max_index)
			{
				max_index = rank_index;
			}
			check_set.insert(rank_index);
		}

		// 新的排行榜结构测试
		{
			static PetRankItem tmp_data;
			tmp_data.Reset();
			tmp_data.master_info = petrank_param.pet_rank_list[i].role_info;
			tmp_data.rank_type = petrank_param.pet_rank_list[i].rank_type;
			tmp_data.rank_value = petrank_param.pet_rank_list[i].rank_value;
			tmp_data.reserve_int = 0;
			tmp_data.flexible_int = petrank_param.pet_rank_list[i].flexible_int;
			tmp_data.flexible_ll = petrank_param.pet_rank_list[i].flexible_ll;
			tmp_data.pet_param = petrank_param.pet_rank_list[i].pet_info;
			tmp_data.pet_attr = petrank_param.pet_rank_list[i].pet_attr;
			tmp_data.pet_god_print = petrank_param.pet_rank_list[i].pet_god_print;

			tmp_data.pet_param.pet_param.CheckModifyVersion();

			std::pair<SkipList<PetRankItem>::Iterator, bool> ret = m_rank_skip_list[rank_type].Insert(tmp_data);
			if (!ret.second)
			{
				// 插入失败说明数据库里出现重复数据 , 或者需要检查 结构的operoperator==
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::Init Insert RANK INFO ERROR] [user[%d %s] prof[%d] rank_type[%d] "
					"list_size[%d]  list_size_is_full[%d]]",
					(int)petrank_param.pet_rank_list[i].role_info.uid, (const char*)petrank_param.pet_rank_list[i].role_info.user_name,
					petrank_param.pet_rank_list[i].role_info.prof, rank_type,
					m_rank_skip_list[rank_type].Size(), m_rank_skip_list[rank_type].IsFull());
			}
		}
	}

	{
		// 旧数据重复数据处理
		if (max_index != (int)m_rank_skip_list[_rank_type].Size())
		{
			is_fault_tolerant = true;
			std::set<int>::iterator it_begin = check_set.begin();
			std::set<int>::iterator it_end = check_set.end();
			for (; it_begin != it_end; ++it_begin)
			{
				m_dirty_mark[_rank_type].insert(*it_begin);
			}
			m_change_flag[_rank_type] = true;
		}
	}

	this->InitIndex(_rank_type, is_fault_tolerant, check_set);

	m_load_flag[_rank_type] = true;
}

void PetRank::Update(time_t now_second)
{
	if (!IsRankLoadFinish() || CrossConfig::Instance().IsHiddenServer()) return;

	if (0 == m_next_log_timestamp)
	{
		m_next_log_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}

	if (now_second > m_next_log_timestamp)
	{
		this->OnRankLog(now_second);
	}
}

void PetRank::OnRankLog(time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_next_log_timestamp = static_cast<unsigned int>(now_second + EngineAdapter::Instance().NextHourInterval(0, 0));

	//整点记录一次排行榜信息
	for (int i = 0; i < PET_RANK_TYPE_MAX; i++)
	{
		SkipList<PetRankItem>::Iterator it_begin = m_rank_skip_list[i].Begin();
		SkipList<PetRankItem>::Iterator it_end = m_rank_skip_list[i].End();
		int count = 0;
		for (; it_begin != it_end; ++it_begin)
		{
			gamelog::g_log_rank.printf(LL_INFO, "PetRank::OnRankLog rank_type[%d] rank_index[%d] user[%d,%s] "
				" pet[%llu %s %d]  "
				"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
				i, count, it_begin->master_info.uid, it_begin->master_info.user_name, 
				it_begin->pet_param.pet_unique_id, it_begin->pet_param.pet_param.name, it_begin->pet_param.capability,
				it_begin->rank_value, it_begin->flexible_int, it_begin->flexible_ll);

			++count;
		}
	}
}

void PetRank::OnGameworldAccept()
{
	if (!this->IsRankLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < PET_RANK_TYPE_MAX; ++i)
	{
		this->SyncRankAllDataToCross(i);
	}
}

void PetRank::GetChangeRankItemList(PetRankParam *petrank_param, int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || NULL == petrank_param)
	{
		return;
	}

	if (!m_change_flag[_rank_type])
	{
		petrank_param->count = 0;
		return;
	}

	int count = 0;

	std::set < int > ::iterator it_begin = m_dirty_mark[_rank_type].begin();
	std::set < int >::iterator it_end = m_dirty_mark[_rank_type].end();

	for (; it_begin != it_end && count < PET_RANK_MAX_SIZE; ++it_begin)
	{
		if (*it_begin >= (int)m_rank_skip_list[_rank_type].Size())
		{
			// 说明这个数据已经被从排行删了
			petrank_param->pet_rank_list[count].change_state = structcommon::CS_DELETE;
			petrank_param->pet_rank_list[count].index = *it_begin;
			petrank_param->pet_rank_list[count].rank_type = _rank_type;
		}
		else
		{
			std::set < int > ::iterator it = m_old_state[_rank_type].find(*it_begin);
			if (m_old_state[_rank_type].end() == it)
			{
				petrank_param->pet_rank_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				petrank_param->pet_rank_list[count].change_state = structcommon::CS_UPDATE;
			}

			SkipList<PetRankItem>::Iterator rank_iter = m_rank_skip_list[_rank_type][*it_begin];
			if (m_rank_skip_list[_rank_type].End() == rank_iter)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::GetChangeRankItemList ERROR] "
					"rank_type[%d]  rank_index[%d]  rank_list_size[%u]",
					_rank_type, *it_begin, m_rank_skip_list[_rank_type].Size());
				continue;
			}

			petrank_param->pet_rank_list[count].index = *it_begin;
			petrank_param->pet_rank_list[count].role_info = rank_iter->master_info;
			petrank_param->pet_rank_list[count].rank_type = rank_iter->rank_type;
			petrank_param->pet_rank_list[count].rank_value = rank_iter->rank_value;
			petrank_param->pet_rank_list[count].record_index = 0;
			petrank_param->pet_rank_list[count].flexible_int = rank_iter->flexible_int;
			petrank_param->pet_rank_list[count].flexible_ll = rank_iter->flexible_ll;
			petrank_param->pet_rank_list[count].str_buff_level = rank_iter->str_buff_level;
			petrank_param->pet_rank_list[count].pet_info = rank_iter->pet_param;
			petrank_param->pet_rank_list[count].pet_attr = rank_iter->pet_attr;
			petrank_param->pet_rank_list[count].pet_god_print = rank_iter->pet_god_print;
		}

		++count;
	}

	petrank_param->count = count;
}

void PetRank::ClearDirtyMark(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_rank_type]) return;

	m_dirty_mark[_rank_type].clear();
	m_old_state[_rank_type].clear();

	SkipList<PetRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PetRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	int rank_index = 0;
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state[_rank_type].insert(rank_index++);
	}

	m_change_flag[_rank_type] = false;
}

void PetRank::OnGetRankList(Role *role, int rank_type, int from_index, int to_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (!IsRankLoadFinish()) return;
	if (NULL == role) return;

	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return;
	if (from_index < 0 || to_index < 0 || from_index  > to_index)
	{
		return;
	}

	int my_rank_index = -1;
	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(role->GetUserId());
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		std::map<PetRankKey, IndexData> ::iterator index_it_begin = my_index_it->second.begin();
		std::map<PetRankKey, IndexData> ::iterator index_it_end = my_index_it->second.end();

		int temp_index = MAX_INT;
		for (; index_it_begin != index_it_end; ++index_it_begin)
		{
			static PetRankItem check_item;
			check_item.Reset();
			this->ConstructionCheckItem(check_item, index_it_begin->second);
			int tmp_i = m_rank_skip_list[rank_type].GetIndex(check_item);
			if (tmp_i < temp_index && -1 != tmp_i)
			{
				temp_index = tmp_i;
			}
			my_rank_index = temp_index;
		}
	}

	if (MAX_INT == my_rank_index)
	{
		my_rank_index = -1;
	}

	static Protocol::SCGetPetRankListAck rank_list_ack;
	rank_list_ack.last_snapshot_time = static_cast<unsigned int>(m_last_change_time[rank_type]);
	rank_list_ack.rank_type = rank_type;
	rank_list_ack.my_rank_index = my_rank_index;
	rank_list_ack.count = 0;

	int rank_count = 0, target_get_count = to_index - from_index + 1;
	SkipList<PetRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type][from_index];
	SkipList<PetRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	for (; it_begin != it_end &&rank_count < PET_RANK_MAX_SIZE && rank_count < target_get_count; ++it_begin)
	{
		//	rank_list_ack.rank_list[rank_count].role_info = it_begin->master_info;
		//	rank_list_ack.rank_list[rank_count].pet_info = it_begin->pet_param;
		//	rank_list_ack.rank_list[rank_count].pet_attrs = it_begin->pet_attr;

		this->GetSimpleInfo(&rank_list_ack.rank_list[rank_count], *it_begin);

		rank_list_ack.rank_list[rank_count].rank_index = rank_count + from_index;
		rank_list_ack.rank_list[rank_count].rank_value = it_begin->rank_value;
		rank_list_ack.rank_list[rank_count].flexible_int = it_begin->flexible_int;
		rank_list_ack.rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

		++rank_count;
	}

	rank_list_ack.count = rank_count;
	int sendlen = sizeof(rank_list_ack) - (PET_RANK_MAX_SIZE - rank_list_ack.count) * sizeof(rank_list_ack.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&rank_list_ack, sendlen);
}

void PetRank::OnGetRankDetailedInfo(Role * role, int target_uid, int rank_type, unsigned long long pet_unique_item_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (!IsRankLoadFinish()) return;
	if (NULL == role) return;

	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return;

	const PetRankItem * pet_rank_item = this->GetPetRankItem(rank_type, PetRankKey(target_uid, pet_unique_item_id));
	if(NULL == pet_rank_item) return;

	Protocol::SCRankPetDetailedInfo info;
	info.str_buff_level = pet_rank_item->str_buff_level;
	info.pet_info = pet_rank_item->pet_param;
	info.pet_attrs = pet_rank_item->pet_attr;
	info.pet_god_print = pet_rank_item->pet_god_print;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void PetRank::OnRoleInfoChange(Role *_role)
{
	if (NULL == _role)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnRoleInfoChangeByHidden(_role);
	}
	else
	{
		static RoleBaseInfo info;
		info.Reset();
		this->UpdateBaseInfo(info, _role);
		this->OnRoleInfoChangeByGame(info);
	}
}

void PetRank::OnPetRemove(Role * _role, unsigned long long _pet_unique_id)
{
	if (NULL == _role)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnPetRemoveByHidden(_role, _pet_unique_id);
	}
	else
	{
		static RoleBaseInfo info;
		info.Reset();
		this->UpdateBaseInfo(info, _role);
		this->OnPetRemoveByGame(info, _pet_unique_id);
	}
}

void PetRank::SyncPetRankInfo(Role * _role, int _rank_type, int _index , bool _is_force)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || NULL == _role || _index < 0 || _index >= ItemNamespace::MAX_PET_GRID_NUM)
	{
		gamelog::g_log_rank.printf(LL_INFO, "PetRank::SyncPetRankInfo[ERROR] user[%d  %s]  rank_type:%d  index:%d  is_froce:%d",
			NULL == _role ? 0 : _role->GetUID() , NULL == _role ? "NULL" : _role->GetName() , _rank_type , _index , _is_force );
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SyncPetRankInfoByHidden(_role, _rank_type, _index , _is_force);
	}
	else
	{
		this->SyncPetRankInfoByGame(_role, _rank_type, _index , _is_force);
	}
}

void PetRank::ClearRank(int rank_type)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return;

	SkipList<PetRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PetRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();

	for (int i = 0; it_begin != it_end; ++it_begin, ++i)
	{
		m_dirty_mark[rank_type].insert(i);
	}

	m_rank_skip_list[rank_type].Clear();
	m_user_index_map[rank_type].clear();
	m_change_flag[rank_type] = true;

	this->SyncRankAllDataToCross(rank_type);

}

const PetRankItem * PetRank::GetPetRankItem(int rank_type, const PetRankKey &pet_key)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return NULL;

	PetIndexMapIt it = m_pet_index_map[rank_type].find(pet_key);
	if (it != m_pet_index_map[rank_type].end())
	{
		static PetRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		SkipList<PetRankItem>::Iterator  it_sub = m_rank_skip_list[rank_type].Find(check_item);
		if (m_rank_skip_list[rank_type].End() != it_sub)
		{
			return &(*it_sub);
		}
	}

	return NULL;
}

bool PetRank::IsRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return true;
	}

	this->CalcRankLoadFinish();

	return m_is_all_load_finish;
}

void PetRank::SyncRankDataFromHidden(crossgameprotocal::HiddenGamePetInfoChange * _msg)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == _msg)
	{
		return;
	}

	switch (_msg->sync_info_type)
	{
	case crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_ROLE_INFO:
		{
			this->OnRoleInfoChangeByGame(_msg->rank_item.master_info);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_INDEX:
		{
			// 该条目已经作废
			return;
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_REMOVE:
		{
			unsigned long long pet_unique_id = ((unsigned long long)(_msg->param_1) << 32) | ((unsigned long long)(_msg->param_2) & 0xFFFFFFFF);
			this->OnPetRemoveByGame(_msg->rank_item.master_info, pet_unique_id);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_CAPABILITY:
	case crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN:
		{
			this->UpdateRankList(_msg->rank_item , 1 == _msg->param_1);
		}
		break;
	default:
		break;
	}
}

void PetRank::GetTopRankList(int rank_type, int max_num, const PetRankItem** out_rank_list, int* out_num)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX)
	{
		assert(0);
		return;
	}
	if (NULL == out_num || NULL == out_rank_list)
	{
		assert(0);
		return;
	}

	SkipList<PetRankItem>::Iterator it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PetRankItem>::Iterator it_end = m_rank_skip_list[rank_type].End();
	int num = 0;
	for (; it_begin != it_end && num < max_num; ++it_begin)
	{
		out_rank_list[num++] = &(*it_begin);
	}
	*out_num = num;
}

void PetRank::InitIndex(int _rank_type, bool _is_fault_tolerant, std::set<int> & _check_set)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::InitIndex INVALID RANK TYPE] [rank_type:%d]", (int)_rank_type);
		return;
	}

	SkipList<PetRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PetRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	for (; it_begin != it_end; ++it_begin)
	{
		int rank_index = m_rank_skip_list[_rank_type].GetIndex(*it_begin);
		if (-1 == rank_index)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::InitUserIndex ERROR] rank_type[%d] [user[%d %s] "
				"pet_unique_id[%lld]  pet_index[%d]  pet_capability[%d]"
				"list_size[%d]  list_size_is_full[%d]]",
				_rank_type, it_begin->master_info.uid, it_begin->master_info.user_name,
				it_begin->pet_param.pet_unique_id, it_begin->pet_param.index, it_begin->pet_param.capability,
				m_rank_skip_list[_rank_type].Size(), m_rank_skip_list[_rank_type].IsFull());
			continue;
		}

		if (_is_fault_tolerant)
		{
			std::set<int>::iterator it = _check_set.find(rank_index);
			if (_check_set.end() != it)
			{
				m_old_state[_rank_type].insert(rank_index);
			}

			m_dirty_mark[_rank_type].insert(rank_index);
		}
		else
		{
			m_old_state[_rank_type].insert(rank_index);
		}

		{
			// 宠物资质检查
			int pet_growth_version = it_begin->pet_param.pet_param.pet_growth_version;
			int pet_id = it_begin->pet_param.pet_param.pet_id;
			if (pet_growth_version < LOGIC_CONFIG->GetPetConfig()->GetPetCurGrowthVersion(pet_id))
			{
				// 刷新一下宠物的资质
				if (it_begin->pet_param.pet_param.CheckGrowthVersion(0, "PetRank::InitIndex"))
				{
					m_dirty_mark[_rank_type].insert(rank_index);

					gamelog::g_log_rank.printf(LL_WARNING, "PetRank::InitIndex, \
						old_version[%d]  new_version[%d] \
						pet_id[%d] unique_id[%lld] pet_growth_version[%d] owner[%d,%s] \
						rank_type[%d] rank_index[%d]",
						pet_growth_version, it_begin->pet_param.pet_param.pet_growth_version,
						pet_id, it_begin->pet_param.pet_unique_id,
						pet_growth_version, it_begin->master_info.uid,
						it_begin->master_info.user_name, _rank_type, rank_index);
				}
			}
		}

		PetRankKey key(it_begin->master_info.uid, it_begin->pet_param.pet_unique_id);
		this->AddPetIndex(_rank_type, key, *it_begin);
		this->AddUserIndex(_rank_type, it_begin->master_info.uid, key, *it_begin);
	}
}

void PetRank::SyncRankAllDataToCross(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossPetRankInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init_add = 0;
	sync_info.reserve_sh = 0;
	sync_info.rank_type = _rank_type;
	sync_info.is_init = true;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();

	SkipList<PetRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PetRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	int count = 0;
	for (; it_begin != it_end && count < CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT; ++it_begin)
	{
		sync_info.item_list[count++].rank_info = *it_begin;

		if (count >= CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT)
		{
			sync_info.count = count;

			int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPetRankInfo) -
				((CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));
#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "PetRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

			InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
			DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK);
			count = 0;
			sync_info.is_init_add = 1;
		}
	}

	if (count > 0)
	{
		sync_info.count = count;

		int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPetRankInfo) -
			((CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "PetRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

		InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK);
	}
}

void PetRank::SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, PetRankItem & _rank_item)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer()
		|| _sync_status <= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INVALID ||
		_sync_status >= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_MAX)
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossPetRankInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init_add = 0;
	sync_info.reserve_sh = 0;
	sync_info.rank_type = _rank_type;
	sync_info.is_init = false;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();

	sync_info.item_list[0].rank_info = _rank_item;
	sync_info.item_list[0].index = _old_index;
	sync_info.item_list[0].sync_type = _sync_status;


	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPetRankInfo) -
		((CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
	gamelog::g_log_debug.printf(LL_INFO, "PetRank::SyncRankDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

	InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);

}

void PetRank::OnRoleInfoChangeByGame(const RoleBaseInfo &_role_info)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int rank_type = 0; rank_type < PET_RANK_TYPE_MAX; ++rank_type)
	{
		UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(_role_info.uid));
		if (it != m_user_index_map[rank_type].end())
		{
			std::map<PetRankKey, IndexData>  ::iterator  it_set_begin = it->second.begin();
			std::map<PetRankKey, IndexData> ::iterator  it_set_end = it->second.end();
			for (; it_set_begin != it_set_end; ++it_set_begin)
			{
				static PetRankItem check_item;
				check_item.Reset();
				this->ConstructionCheckItem(check_item, it_set_begin->second);

				int rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
				if (-1 == rank_index)
				{
					continue;
				}

				SkipList<PetRankItem>::Iterator rank_it = m_rank_skip_list[rank_type][rank_index];
				if (m_rank_skip_list[rank_type].End() != rank_it)
				{
					m_change_flag[rank_type] = true;
					m_dirty_mark[rank_type].insert(rank_index);
					this->UpdateBaseInfo(rank_it->master_info, _role_info);
					// 同步更新信息到跨服
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA, rank_index, *rank_it);

					m_last_change_time[rank_type] = (unsigned int)EngineAdapter::Instance().Time();
				}
				else
				{
					// 这是不应该出现的问题 , 如果出现了需要查逻辑和map相关的调用
					gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::OnRoleInfoChangeByGame Find Rank_info ERROR] [user[%d %s] "
						"rank_type[%d]  rank_index[%d]",
						_role_info.uid, _role_info.user_name,
						rank_type, rank_index);
				}
			}
		}
	}
}

void PetRank::OnPetRemoveByGame(const RoleBaseInfo & _role_info, unsigned long long _pet_unique_id)
{
	for (int rank_type = 0; rank_type < PET_RANK_TYPE_MAX; ++rank_type)
	{
		PetIndexMapIt it = m_pet_index_map[rank_type].find(PetRankKey(_role_info.uid, _pet_unique_id));
		if (it != m_pet_index_map[rank_type].end())
		{
			static PetRankItem check_item;
			this->ConstructionCheckItem(check_item, it->second);

			int rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
			if (-1 == rank_index)
			{
				continue;
			}

			SkipList<PetRankItem>::Iterator rank_it = m_rank_skip_list[rank_type][rank_index];
			if (m_rank_skip_list[rank_type].End() != rank_it)
			{
				// 同步更新信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE, rank_index, *rank_it);

				// 重新设置索引
				this->RemoveUserIndex(rank_type, _role_info.uid, PetRankKey(_role_info.uid, _pet_unique_id));
				this->RemovePetIndex(rank_type, PetRankKey(_role_info.uid, _pet_unique_id));

				// 数据不一定是插入在最后 需要更新插入位到表尾的数据
				for (int i = rank_index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
				{
					m_dirty_mark[rank_type].insert(i);
				}

				m_rank_skip_list[rank_type].Erase(rank_it);
				m_change_flag[rank_type] = true;
				m_last_change_time[rank_type] = (unsigned int)EngineAdapter::Instance().Time();
			}
			else
			{
				// 这是不应该出现的问题 , 如果出现了需要查逻辑和map相关的调用
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PetRank::OnPetRemoveByGame Find Rank_info ERROR] [user[%d %s] "
					"rank_type[%d]  rank_index[%d]",
					_role_info.uid, _role_info.user_name,
					rank_type, rank_index);
			}
		}
	}
}

void PetRank::SyncPetRankInfoByGame(Role * _role, int _rank_type, int _index ,bool _is_force)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || NULL == _role || _index < 0 || _index >= ItemNamespace::MAX_PET_GRID_NUM)
	{
		return;
	}

	static PetRankItem rank_item;
	rank_item.Reset();

	this->UpdateBaseInfo(rank_item.master_info, _role);
	this->UpdatePetInfo(rank_item, _role, _index);

	switch (_rank_type)
	{
	case PET_RANK_TYPE_CAPABILITY:
		{
			rank_item.rank_type = PET_RANK_TYPE_CAPABILITY;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetPet()->GetPetCapability_1(_index);
		}
		break;
	case PET_RANK_TYPE_HUASHEN_CAPABILITY:
		{
			_role->GetRoleModuleManager()->GetHuaShenManager()->GetPetRankInfo(_index, rank_item.master_info, rank_item.pet_param, rank_item.pet_attr);
			rank_item.rank_type = PET_RANK_TYPE_HUASHEN_CAPABILITY;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetHuaShenManager()->GetPetCapability(_index);
		}
		break;
	default:
		break;
	}

	this->UpdateRankList(rank_item , _is_force);
}

void PetRank::OnRoleInfoChangeByHidden(Role * _role)
{
	if (NULL == _role)
	{
		return;
	}

	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGamePetInfoChange change;
	change.sync_info_type = crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_ROLE_INFO;
	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;

	this->UpdateBaseInfo(change.rank_item.master_info, _role);

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void PetRank::OnPetRemoveByHidden(Role * _role, unsigned long long _pet_unique_id)
{
	if (NULL == _role )
	{
		return;
	}

	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGamePetInfoChange change;
	change.sync_info_type = crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_REMOVE;
	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;
	change.param_1 = (int)((_pet_unique_id >> 32) & 0xFFFFFFFF) ;
	change.param_2 = (int)((_pet_unique_id << 32 ) >> 32);

	this->UpdateBaseInfo(change.rank_item.master_info, _role);

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void PetRank::SyncPetRankInfoByHidden(Role * _role, int _rank_type, int _index , bool _is_force )
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || NULL == _role || _index < 0 || _index >= ItemNamespace::MAX_PET_GRID_NUM)
	{
		return;
	}

	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGamePetInfoChange change;
	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;
	change.param_1 = _is_force;

	this->UpdateBaseInfo(change.rank_item.master_info, _role);
	this->UpdatePetInfo(change.rank_item, _role, _index);

	switch (_rank_type)
	{
	case PET_RANK_TYPE_CAPABILITY:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_CAPABILITY;
			change.rank_item.rank_type = PET_RANK_TYPE_CAPABILITY;
			change.rank_item.rank_value = _role->GetRoleModuleManager()->GetPet()->GetPetCapability_1(_index);
		}
		break;
	case PET_RANK_TYPE_HUASHEN_CAPABILITY:
		{
			_role->GetRoleModuleManager()->GetHuaShenManager()->GetPetRankInfo(_index, change.rank_item.master_info, change.rank_item.pet_param, change.rank_item.pet_attr);
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN;
			change.rank_item.rank_type = PET_RANK_TYPE_HUASHEN_CAPABILITY;
			change.rank_item.rank_value = _role->GetRoleModuleManager()->GetHuaShenManager()->GetPetCapability(_index);
		}
		break;
	default:
		break;
	}

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void PetRank::UpdateRankList(PetRankItem &rank_item, bool _is_force )
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	// 存在则更新 , 不存在则插入
	int rank_type = rank_item.rank_type;
	int uid = rank_item.master_info.uid;
	bool need_insert = true;
	bool is_error_insert = false;  // 是否是容错插入 , 用于日志查BUG
	bool is_del_index_insert = false;  // 是否是删除索引后添加 , 用于日志查BUG

	static PetRankItem check_item;
	int rank_index = -1;
	PetIndexMapIt it = m_pet_index_map[rank_type].find(PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
	if (it != m_pet_index_map[rank_type].end())
	{
		need_insert = false;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
		if (-1 == rank_index)
		{
			// 因为UPDATE的时候被人把数据挤出了排行榜  , 但是索引里依然存在 
			// 所以如果根据索引无法找到对应的数据时 ,  
			// 所以则清除对应的索引  然后再进行新数据添加
			// 后续由新数据添加来做判断
			need_insert = true;
			is_del_index_insert = true;
			this->RemoveUserIndex(rank_type, uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
			this->RemovePetIndex(rank_type, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
		}
	}

	// UPDATE 相关逻辑
	if (!need_insert)
	{
		// 在排行榜上有数据那么需要刷新数据
		SkipList<PetRankItem>::Iterator skip_it = m_rank_skip_list[rank_type][rank_index];
		if (m_rank_skip_list[rank_type].End() != skip_it)
		{
			// 如果相等则不需要更新 , 玩家上线的时候会重算战斗力 , 此时和排行榜内数据理论上无变化
			if (*skip_it == rank_item && !_is_force)
			{
				return;
			}

			// 注意 updata后  原迭代器可能失效 ,如需使用需要重新获取新的迭代器
			m_rank_skip_list[rank_type].UpdateData(*skip_it, rank_item);
			int new_index = m_rank_skip_list[rank_type].GetIndex(rank_item);
			if (-1 != new_index)
			{
				// 需要更新索引
				this->UpdataUserIndex(rank_type, uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id), rank_item);
				this->ConstructionIndexData(it->second, rank_item);

				m_change_flag[rank_type] = true;

				if (new_index == rank_index)
				{
					// 数据更新了 顺序没变更
					m_dirty_mark[rank_type].insert(new_index);
				}
				else
				{
					// 数据更新了 顺序变更了 , 那么2个index之间的数据顺位都发生了变更
					int start_index = new_index < rank_index ? new_index : rank_index;
					int start_end = new_index > rank_index ? new_index : rank_index;
					for (int i = start_index; i <= start_end; ++i)
					{
						m_dirty_mark[rank_type].insert(i);
					}

					// 名次变更时记录一下日志
					gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList Updata  check_user[%d %s]  new_user[%d %s] rank_type:%d  "
						"check_pet_info[%llu %s]  new_pet_info[%llu %s] "
						"old_index:%d   new_index:%d  check_rank_val[%lld]  new_rank_val[%lld] ",
						check_item.master_info.uid, check_item.master_info.user_name, rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
						check_item.pet_param.pet_unique_id, check_item.pet_param.pet_param.name, rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
						rank_index, new_index, check_item.rank_value, rank_item.rank_value);
				}

				// 同步更新信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA, rank_index, rank_item);
			}
			else
			{
				// 记录错误日志 
				gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList[ERROR_2] check_user[%d %s]  new_user[%d %s] rank_type:%d  "
					"check_pet_info[%llu %s]  new_pet_info[%llu %s] "
					"old_index:%d   check_rank_val[%lld]  new_rank_val[%lld] ",
					check_item.master_info.uid, check_item.master_info.user_name, rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
					check_item.pet_param.pet_unique_id, check_item.pet_param.pet_param.name, rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
					rank_index, check_item.rank_value, rank_item.rank_value);

				// 容错
				need_insert = true;
				is_error_insert = true;
				this->RemoveUserIndex(rank_type, uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
				this->RemovePetIndex(rank_type, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
			}
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList[ERROR_3] check_user[%d %s]  new_user[%d %s] rank_type:%d  "
				"check_pet_info[%llu %s]  new_pet_info[%llu %s] "
				"list_size[%d]  old_index:%d   check_rank_val[%lld]  new_rank_val[%lld] ",
				check_item.master_info.uid, check_item.master_info.user_name, rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
				check_item.pet_param.pet_unique_id, check_item.pet_param.pet_param.name, rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
				m_rank_skip_list[rank_type].Size(), rank_index, check_item.rank_value, rank_item.rank_value);


			// 容错
			need_insert = true;
			is_error_insert = true;
			this->RemoveUserIndex(rank_type, uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
			this->RemovePetIndex(rank_type, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id));
		}
	}

	// INSERT 相关逻辑
	if (need_insert)
	{
		if (m_rank_skip_list[rank_type].IsFull())
		{
			// 如果表已经满了, 
			SkipList<PetRankItem>::Reverse_Iterator r_iter = m_rank_skip_list[rank_type].RBegin();
			if (m_rank_skip_list[rank_type].REnd() != r_iter)
			{
				// 新入数据大于末尾数据
				if (rank_item > *r_iter)
				{
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE,
						m_rank_skip_list[rank_type].Size() - 1, *r_iter);

					m_rank_skip_list[rank_type].Erase(*r_iter);
					m_rank_skip_list[rank_type].Insert(rank_item);
					int index = m_rank_skip_list[rank_type].GetIndex(rank_item);
					if (-1 != index)
					{
						// 同步插入信息到跨服
						this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT, -1, rank_item);

						this->AddUserIndex(rank_type, rank_item.master_info.uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id), rank_item);
						this->AddPetIndex(rank_type, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id), rank_item);

						m_change_flag[rank_type] = true;

						// 数据不一定是插入在最后 需要更新插入位到表尾的数据
						for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
						{
							m_dirty_mark[rank_type].insert(i);
						}

						// 插入日志
						gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList Insert_SUCC_1  is_error_insert:%d  is_del_index_insert:%d   new_user[%d %s] rank_type:%d  "
							"new_pet_info[%llu %s] "
							"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
							is_error_insert , is_del_index_insert , rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
							rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
							m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value , rank_item.flexible_int, rank_item.flexible_ll);
					}
					else
					{
						// 错误日志 , 如果出现了需要查逻辑
						gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList[ERROR_4]  is_error_insert:%d  new_user[%d %s] rank_type:%d  "
							"new_pet_info[%llu %s] "
							"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
							is_error_insert ,rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
							rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
							m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
					}
				}
			}
		}
		else
		{
			// 没满直接插入
			m_rank_skip_list[rank_type].Insert(rank_item);
			int index = m_rank_skip_list[rank_type].GetIndex(rank_item);
			if (-1 != index)
			{
				// 同步插入信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT, -1, rank_item);

				this->AddUserIndex(rank_type, rank_item.master_info.uid, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id), rank_item);
				this->AddPetIndex(rank_type, PetRankKey(rank_item.master_info.uid, rank_item.pet_param.pet_unique_id), rank_item);
				m_change_flag[rank_type] = true;

				// 数据不一定是插入在最后 需要更新插入位到表尾的数据
				for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
				{
					m_dirty_mark[rank_type].insert(i);
				}

				// 插入日志
				gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList Insert_SUCC_2  is_error_insert:%d   is_del_index_insert:%d  new_user[%d %s] rank_type:%d  "
					"new_pet_info[%llu %s] "
					"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
					is_error_insert, is_del_index_insert, rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
					rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
					m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
			else
			{
				// 错误日志 , 如果出现了需要查逻辑
				gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdateRankList[ERROR_5]  is_error_insert:%d  new_user[%d %s] rank_type:%d  "
					"new_pet_info[%llu %s] "
					"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
					is_error_insert, rank_item.master_info.uid, rank_item.master_info.user_name, rank_type,
					rank_item.pet_param.pet_unique_id, rank_item.pet_param.pet_param.name,
					m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
		}
	}
}

void PetRank::UpdateBaseInfo(RoleBaseInfo &_dest_role_info, const RoleBaseInfo & _src_role_info)
{
	_dest_role_info.uid = _src_role_info.uid;
	STRNCPY(_dest_role_info.user_name, _src_role_info.user_name, sizeof(GameName));

	_dest_role_info.prof = _src_role_info.prof;
	_dest_role_info.role_level = _src_role_info.role_level;
	_dest_role_info.avatar_type = _src_role_info.avatar_type;
	_dest_role_info.capability = _src_role_info.capability;
	_dest_role_info.exp = _src_role_info.exp;
	_dest_role_info.headshot_id = _src_role_info.headshot_id;
	_dest_role_info.top_level = _src_role_info.top_level;
	_dest_role_info.role_fly_flag = _src_role_info.role_fly_flag;
}

void PetRank::UpdateBaseInfo(RoleBaseInfo &_role_info, Role * _role)
{
	if (NULL == _role)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
		if (NULL == cui)
		{
			return;
		}
		_role_info.uid = UidToInt(cui->original_user_id);
		_role->GetName(_role_info.user_name);
		std::string temp_str = _role_info.user_name;
		int index = temp_str.rfind('_');
		if (index > 0 && index < static_cast<int>(sizeof(GameName)))
		{
			_role_info.user_name[index] = '\0';
		}
	}
	else
	{
		_role_info.uid = _role->GetUID();
		_role->GetName(_role_info.user_name);
	}

	_role_info.prof = _role->GetProfession();
	_role_info.role_level = _role->GetLevel();
	_role_info.avatar_type = _role->GetAvatarType();
	_role_info.capability = _role->GetCapability();
	_role_info.exp = _role->GetExp();
	_role_info.headshot_id = _role->GetHeadshotID();
	_role_info.top_level = _role->GetTopLevel();
	_role_info.role_fly_flag = _role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
}

void PetRank::UpdatePetInfo(PetRankItem & _rank_item, Role * _role, int _pet_index)
{
	if (NULL == _role || _pet_index < 0 || _pet_index >= ItemNamespace::MAX_PET_GRID_NUM + ItemNamespace::MAX_PET_STORAGE_GRID_NUM)
	{
		return;
	}

	const PetParam *pet_param = NULL;
	const ItemGridData * temp_grid_data = NULL;
	AttributeList attr_list;
	int ret = 0;
	int index = _pet_index;
	int str_buff_level = 0;
	GodPrintListPro pet_god_print;

	// 如果大于背包序号，可认为是仓库内的
	// 谨慎拓展，序号会错乱，排行榜会因此出现重复
	UNSTD_STATIC_CHECK(ItemNamespace::MAX_PET_GRID_NUM == 100);
	if (index >= ItemNamespace::MAX_PET_GRID_NUM)
	{
		pet_param = _role->GetRoleModuleManager()->GetPet()->GetPetParamFromStorage(index - ItemNamespace::MAX_PET_GRID_NUM);
		ret = _role->GetRoleModuleManager()->GetPet()->GetBagAttributeList(index, attr_list);
		temp_grid_data = _role->GetRoleModuleManager()->GetPet()->GetPetGridDataFromStorage(index);
	}
	else
	{
		pet_param = _role->GetRoleModuleManager()->GetPet()->GetPetParam(index);
		ret = _role->GetRoleModuleManager()->GetPet()->GetAttributeList(index, attr_list);
		temp_grid_data = _role->GetRoleModuleManager()->GetPet()->GetPetGridData(index);
		str_buff_level = _role->GetRoleModuleManager()->GetPet()->GetStrBuffLevel(index);
		_role->GetRoleModuleManager()->GetPet()->GetPetGodPrintList(index, &pet_god_print);
	}

	if (NULL == pet_param || NULL == temp_grid_data)
	{
		return;
	}

	if (ret != 0)
	{
		gamelog::g_log_rank.printf(LL_INFO, "PetRank::UpdatePetInfo no attr_list index [%d]", index);
	}

	_rank_item.pet_param.pet_unique_id = temp_grid_data->item_unique_id;
	_rank_item.pet_param.index = index;
	_rank_item.pet_param.pet_param = *pet_param;
	_rank_item.pet_attr = attr_list;
	_rank_item.str_buff_level = str_buff_level;
	_rank_item.pet_god_print = pet_god_print;
}

bool PetRank::AddUserIndex(int rank_type, int _uid, const PetRankKey &pet_rank_key, const PetRankItem & _rank_item)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return false;

	UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(_uid));
	if (it != m_user_index_map[rank_type].end())
	{
		static IndexData index_data;
		this->ConstructionIndexData(index_data, _rank_item);

		it->second.insert(std::make_pair(pet_rank_key, index_data));
	}
	else
	{
		std::map<PetRankKey, IndexData> temp_map;

		static IndexData index_data;
		this->ConstructionIndexData(index_data, _rank_item);

		temp_map.insert(std::make_pair(pet_rank_key, index_data));
		m_user_index_map[rank_type].insert(std::make_pair(IntToUid(_uid), temp_map));
	}

	return true;
}

bool PetRank::AddPetIndex(int rank_type, const PetRankKey &pet_rank_key, const PetRankItem & _rank_item)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return false;

	PetIndexMapIt it = m_pet_index_map[rank_type].find(pet_rank_key);
	if (it == m_pet_index_map[rank_type].end())
	{
		static IndexData index_data;
		index_data.Reset();
		this->ConstructionIndexData(index_data, _rank_item);

		m_pet_index_map[rank_type].insert(std::make_pair(pet_rank_key, index_data));
	}


	return true;
}

bool PetRank::RemoveUserIndex(int rank_type, int _uid, const PetRankKey & _pet_key)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return false;

	UserIndexMapIt it = m_user_index_map[rank_type].find(_uid);
	if (it != m_user_index_map[rank_type].end())
	{
		it->second.erase(_pet_key);

		if (0 == it->second.size())
		{
			m_user_index_map[rank_type].erase(it);
		}
	}

	return true;
}

bool PetRank::RemovePetIndex(int rank_type, const PetRankKey &_pet_key)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return false;

	PetIndexMapIt it = m_pet_index_map[rank_type].find(_pet_key);
	if (it != m_pet_index_map[rank_type].end())
	{
		m_pet_index_map[rank_type].erase(it);
	}

	return true;
}

bool PetRank::UpdataUserIndex(int rank_type, int _uid, const PetRankKey &_pet_key, const PetRankItem & _new_rank_item)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX)
	{
		return false;
	}

	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(IntToUid(_uid));
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		std::map< PetRankKey, IndexData>::iterator it = my_index_it->second.find(_pet_key);
		if (my_index_it->second.end() != it)
		{
			static IndexData index_data;
			index_data.Reset();
			this->ConstructionIndexData(index_data, _new_rank_item);
			it->second = index_data;
		}
	}

	return true;
}

int PetRank::GetRankTypeSize(int rank_type)
{
	if (rank_type < 0 || rank_type >= PET_RANK_TYPE_MAX) return false;

	int rank_type_size = m_rank_skip_list[rank_type].Size();

	return rank_type_size;
}

void PetRank::CalcRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return;
	}

	for (int rank_type = 0; rank_type < PET_RANK_TYPE_MAX; ++rank_type)
	{
		if (!m_load_flag[rank_type])
		{
			return;
		}
	}

	m_is_all_load_finish = true;
}

void PetRank::ConstructionCheckItem(PetRankItem & _check_item, const IndexData & _index_data)
{
	_check_item.master_info.uid = _index_data.uid;
	_check_item.rank_type = _index_data.rank_type;
	_check_item.rank_value = _index_data.rank_value;
	_check_item.flexible_int = _index_data.flexible_int;
	_check_item.flexible_ll = _index_data.flexible_ll;
	_check_item.pet_param.pet_unique_id = _index_data.pet_unique_id;
}

void PetRank::ConstructionIndexData(IndexData & _index_data, const PetRankItem & _check_item)
{
	_index_data.uid = _check_item.master_info.uid;
	_index_data.rank_type = _check_item.rank_type;
	_index_data.rank_value = _check_item.rank_value;
	_index_data.flexible_int = _check_item.flexible_int;
	_index_data.flexible_ll = _check_item.flexible_ll;
	_index_data.pet_unique_id = _check_item.pet_param.pet_unique_id;
}

void PetRank::GetSimpleInfo(Protocol::SCGetPetRankListAck::PetRankListItem * pet_rank_info, const PetRankItem & pet_rank_item)
{
	if(NULL == pet_rank_info) return;

	pet_rank_info->role_info.role_id = pet_rank_item.master_info.uid;
	pet_rank_info->role_info.prof = pet_rank_item.master_info.prof;
	pet_rank_info->role_info.avatar_type = pet_rank_item.master_info.avatar_type;
	pet_rank_info->role_info.headshot_id = pet_rank_item.master_info.headshot_id;
	pet_rank_info->role_info.top_level = pet_rank_item.master_info.top_level;
	pet_rank_info->role_info.role_fly_flag = pet_rank_item.master_info.role_fly_flag;
	F_STRNCPY(pet_rank_info->role_info.role_name, pet_rank_item.master_info.user_name, sizeof(pet_rank_info->role_info.role_name));
	
	pet_rank_info->pet_info.pet_id = pet_rank_item.pet_param.pet_param.pet_id;
	pet_rank_info->pet_info.str_level = pet_rank_item.pet_param.pet_param.str_level;
	pet_rank_info->pet_info.pet_level = pet_rank_item.pet_param.pet_param.level;
	pet_rank_info->pet_info.skin_id = pet_rank_item.pet_param.pet_param.skin_id;
	pet_rank_info->pet_info.pet_unique_item_id = pet_rank_item.pet_param.pet_unique_id;
	pet_rank_info->pet_info.fei_sheng_times = pet_rank_item.pet_param.pet_param.fei_sheng_times;
	pet_rank_info->pet_info.lian_yao_pet_id = pet_rank_item.pet_param.pet_param.lian_yao_pet_id;
	F_STRNCPY(pet_rank_info->pet_info.pet_name, pet_rank_item.pet_param.pet_param.name, sizeof(pet_rank_info->pet_info.pet_name));
}
