#include "partnerrank.hpp"

#include "gamelog.h"
#include "engineadapter.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"
#include "obj/character/role.h"
#include "other/partner/partner.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "servercommon/performancecatch.hpp"
#include "config/logicconfigmanager.hpp"
#include "crossserver/crossserver/protocal/msgcross.h"
#include "global/datasynccheck/datasynccheck.hpp"


PartnerRank::PartnerRank()
	: m_rank_manager(NULL), m_is_all_load_finish(false), m_next_log_timestamp(0)
{
	memset(m_change_flag, 0, sizeof(m_change_flag));
	memset(m_load_flag, 0, sizeof(m_load_flag));

	for (int i = 0; i < PARTNER_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].SetDefaultMaxSize(PET_RANK_MAX_SIZE);
		m_rank_skip_list[i].SetCompare(&PartnerRankItem::RankCompare);

		m_dirty_mark[i].clear();
		m_old_state[i].clear();
	}

	memset(m_last_change_time, 0, sizeof(m_last_change_time));
}

PartnerRank::~PartnerRank()
{
	for (int i = 0; i < PARTNER_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].Clear();
	}
}

void PartnerRank::_SyncPartnerRankInfo(Role * _role, int _rank_type, int _partner_id, bool _is_force)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX || NULL == _role || _role->GetRoleModuleManager()->GetPartner()->IsPartnerInvalid(_partner_id)) return;
	
	static PartnerRankItem rank_item;
	rank_item.Reset();

	this->UpdatePartnerInfo(rank_item, _role, _partner_id);

	switch (_rank_type)
	{
	case PARTNER_RANK_TYPE_CAPABILITY:
	{
		rank_item.rank_type = PARTNER_RANK_TYPE_CAPABILITY;
		rank_item.rank_value = _role->GetRoleModuleManager()->GetPartner()->GetParterCapability(_partner_id);
	}
	break;
	default:
		break;
	}

	this->UpdateRankList(rank_item, _is_force);
}

void PartnerRank::UpdateRankList(PartnerRankItem & rank_item, bool _is_force)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	// 存在则更新 , 不存在则插入
	int rank_type = rank_item.rank_type;
	int uid = rank_item.user_uid;
	bool need_insert = true;
	bool is_error_insert = false;  // 是否是容错插入 , 用于日志查BUG
	bool is_del_index_insert = false;  // 是否是删除索引后添加 , 用于日志查BUG

	static PartnerRankItem check_item;
	int rank_index = -1;
	PartnerIndexMapIt it = m_partner_index_map[rank_type].find(PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
	if (it != m_partner_index_map[rank_type].end())
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
			this->RemoveUserIndex(rank_type, uid, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
			this->RemovePartnerIndex(rank_type, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
		}
	}

	// UPDATE 相关逻辑
	if (!need_insert)
	{
		// 在排行榜上有数据那么需要刷新数据
		SkipList<PartnerRankItem>::Iterator skip_it = m_rank_skip_list[rank_type][rank_index];
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
				this->UpdataUserIndex(rank_type, uid, PartnerRankKey(rank_item.user_uid, rank_item.partner_id), rank_item);
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
					gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList Updata  check_user[%d]  new_user[%d] rank_type:%d  "
						"check_partner_id[%d]  new_partner_id[%d] "
						"old_index:%d   new_index:%d  check_rank_val[%lld]  new_rank_val[%lld] ",
						check_item.user_uid, rank_item.user_uid, rank_type,
						check_item.partner_id, rank_item.partner_id,
						rank_index, new_index, check_item.rank_value, rank_item.rank_value);
				}
			}
			else
			{
				// 记录错误日志 
				gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList[ERROR_2] check_user[%d]  new_user[%d] rank_type:%d  "
					"check_partner_id[%d]  new_partner_id[%d] "
					"old_index:%d   check_rank_val[%lld]  new_rank_val[%lld] ",
					check_item.user_uid, rank_item.user_uid, rank_type,
					check_item.partner_id, rank_item.partner_id,
					rank_index, check_item.rank_value, rank_item.rank_value);

				// 容错
				need_insert = true;
				is_error_insert = true;
				this->RemoveUserIndex(rank_type, uid, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
				this->RemovePartnerIndex(rank_type, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
			}
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList[ERROR_3] check_user[%d]  new_user[%d] rank_type:%d  "
				"check_partner_id[%d]  new_partner_id[%d] "
				"list_size[%d]  old_index:%d   check_rank_val[%lld]  new_rank_val[%lld] ",
				check_item.user_uid, rank_item.user_uid, rank_type,
				check_item.partner_id, rank_item.partner_id, 
				m_rank_skip_list[rank_type].Size(), rank_index, check_item.rank_value, rank_item.rank_value);


			// 容错
			need_insert = true;
			is_error_insert = true;
			this->RemoveUserIndex(rank_type, uid, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
			this->RemovePartnerIndex(rank_type, PartnerRankKey(rank_item.user_uid, rank_item.partner_id));
		}
	}

	// INSERT 相关逻辑
	if (need_insert)
	{
		if (m_rank_skip_list[rank_type].IsFull())
		{
			// 如果表已经满了, 
			SkipList<PartnerRankItem>::Reverse_Iterator r_iter = m_rank_skip_list[rank_type].RBegin();
			if (m_rank_skip_list[rank_type].REnd() != r_iter)
			{
				// 新入数据大于末尾数据
				if (rank_item > *r_iter)
				{
					m_rank_skip_list[rank_type].Erase(*r_iter);
					m_rank_skip_list[rank_type].Insert(rank_item);
					int index = m_rank_skip_list[rank_type].GetIndex(rank_item);
					if (-1 != index)
					{						
						this->AddUserIndex(rank_type, rank_item.user_uid, PartnerRankKey(rank_item.user_uid, rank_item.user_uid), rank_item);
						this->AddPartnerIndex(rank_type, PartnerRankKey(rank_item.user_uid, rank_item.user_uid), rank_item);

						m_change_flag[rank_type] = true;

						// 数据不一定是插入在最后 需要更新插入位到表尾的数据
						for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
						{
							m_dirty_mark[rank_type].insert(i);
						}

						// 插入日志
						gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList Insert_SUCC_1  is_error_insert:%d  is_del_index_insert:%d   new_user[%d] rank_type:%d  "
							"new_partner_id[%d] "
							"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
							is_error_insert, is_del_index_insert, rank_item.user_uid, rank_type,
							rank_item.partner_id,
							m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
					}
					else
					{
						// 错误日志 , 如果出现了需要查逻辑
						gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList[ERROR_4]  is_error_insert:%d  new_user[%d] rank_type:%d  "
							"new_partner_id[%d] "
							"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
							is_error_insert, rank_item.user_uid, rank_type,
							rank_item.partner_id,
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
				this->AddUserIndex(rank_type, rank_item.user_uid, PartnerRankKey(rank_item.user_uid, rank_item.partner_id), rank_item);
				this->AddPartnerIndex(rank_type, PartnerRankKey(rank_item.user_uid, rank_item.partner_id), rank_item);
				m_change_flag[rank_type] = true;

				// 数据不一定是插入在最后 需要更新插入位到表尾的数据
				for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
				{
					m_dirty_mark[rank_type].insert(i);
				}

				// 插入日志
				gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList Insert_SUCC_2  is_error_insert:%d   is_del_index_insert:%d  new_user[%d] rank_type:%d  "
					"new_partner_id[%d] "
					"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
					is_error_insert, is_del_index_insert, rank_item.user_uid, rank_type,
					rank_item.partner_id,
					m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
			else
			{
				// 错误日志 , 如果出现了需要查逻辑
				gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::UpdateRankList[ERROR_5]  is_error_insert:%d  new_user[%d] rank_type:%d  "
					"new_partner_id[%d] "
					"list_size[%d]  rand_index:%d  new_rank_val[%lld]  flexible_int[%d] flexible_ll[%lld]",
					is_error_insert, rank_item.user_uid, rank_type,
					rank_item.partner_id,
					m_rank_skip_list[rank_type].Size(), index, rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
		}
	}
}

void PartnerRank::UpdatePartnerInfo(PartnerRankItem & _rank_item, Role * _role, int _partner_id)
{
	if (NULL == _role || _role->GetRoleModuleManager()->GetPartner()->IsPartnerInvalid(_partner_id)) return;
	
	const NewPartnerBaseInfo* partner_base_info = _role->GetRoleModuleManager()->GetPartner()->GetPartnerBaseInfoById(_partner_id);
	if (NULL == partner_base_info) return;

	if (!_role->GetRoleModuleManager()->GetPartner()->IsPartnerAlreadyActive(_partner_id)) return;

	_rank_item.user_uid = _role->GetUID();
	_rank_item.partner_id = _partner_id;
}

bool PartnerRank::AddUserIndex(int rank_type, int _uid, const PartnerRankKey & partner_rank_key, const PartnerRankItem & _rank_item)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return false;

	UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(_uid));
	if (it != m_user_index_map[rank_type].end())
	{
		static IndexData index_data;
		this->ConstructionIndexData(index_data, _rank_item);

		it->second.insert(std::make_pair(partner_rank_key, index_data));
	}
	else
	{
		std::map<PartnerRankKey, IndexData> temp_map;

		static IndexData index_data;
		this->ConstructionIndexData(index_data, _rank_item);

		temp_map.insert(std::make_pair(partner_rank_key, index_data));
		m_user_index_map[rank_type].insert(std::make_pair(IntToUid(_uid), temp_map));
	}

	return true;
}

bool PartnerRank::AddPartnerIndex(int rank_type, const PartnerRankKey & partner_rank_key, const PartnerRankItem & _rank_item)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return false;

	PartnerIndexMapIt it = m_partner_index_map[rank_type].find(partner_rank_key);
	if (it == m_partner_index_map[rank_type].end())
	{
		static IndexData index_data;
		index_data.Reset();
		this->ConstructionIndexData(index_data, _rank_item);

		m_partner_index_map[rank_type].insert(std::make_pair(partner_rank_key, index_data));
	}

	return true;
}

bool PartnerRank::RemoveUserIndex(int rank_type, int _uid, const PartnerRankKey & _partner_key)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return false;

	UserIndexMapIt it = m_user_index_map[rank_type].find(_uid);
	if (it != m_user_index_map[rank_type].end())
	{
		it->second.erase(_partner_key);

		if (0 == it->second.size())
		{
			m_user_index_map[rank_type].erase(it);
		}
	}

	return true;
}

bool PartnerRank::RemovePartnerIndex(int rank_type, const PartnerRankKey & _partner_key)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return false;

	PartnerIndexMapIt it = m_partner_index_map[rank_type].find(_partner_key);
	if (it != m_partner_index_map[rank_type].end())
	{
		m_partner_index_map[rank_type].erase(it);
	}

	return true;
}

bool PartnerRank::UpdataUserIndex(int rank_type, int _uid, const PartnerRankKey & _partner_key, const PartnerRankItem & _new_rank_item)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return false;
	
	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(IntToUid(_uid));
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		std::map<PartnerRankKey, IndexData>::iterator it = my_index_it->second.find(_partner_key);
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

void PartnerRank::ConstructionCheckItem(PartnerRankItem & _check_item, const IndexData & _index_data)
{
	_check_item.user_uid = _index_data.uid;
	_check_item.rank_type = _index_data.rank_type;
	_check_item.rank_value = _index_data.rank_value;
	_check_item.flexible_int = _index_data.flexible_int;
	_check_item.flexible_ll = _index_data.flexible_ll;
	_check_item.partner_id = _index_data.partner_id;
}

void PartnerRank::ConstructionIndexData(IndexData & _index_data, const PartnerRankItem & _check_item)
{
	_index_data.uid = _check_item.user_uid;
	_index_data.rank_type = _check_item.rank_type;
	_index_data.rank_value = _check_item.rank_value;
	_index_data.flexible_int = _check_item.flexible_int;
	_index_data.flexible_ll = _check_item.flexible_ll;
	_index_data.partner_id = _check_item.partner_id;
}

void PartnerRank::CalcRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return;
	}

	for (int rank_type = 0; rank_type < PARTNER_RANK_TYPE_MAX; ++rank_type)
	{
		if (!m_load_flag[rank_type])
		{
			return;
		}
	}

	m_is_all_load_finish = true;
}

void PartnerRank::GetTopRankList(int rank_type, int max_num, const PartnerRankItem ** out_rank_list, int * out_num)
{		
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX)
	{
		assert(0);
		return;
	}
	if (NULL == out_num || NULL == out_rank_list)
	{
		assert(0);
		return;
	}

	SkipList<PartnerRankItem>::Iterator it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PartnerRankItem>::Iterator it_end = m_rank_skip_list[rank_type].End();
	int num = 0;
	for (; it_begin != it_end && num < max_num; ++it_begin)
	{
		out_rank_list[num++] = &(*it_begin);
	}
	*out_num = num;	
}

void PartnerRank::ClearRank(int rank_type)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return;

	SkipList<PartnerRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PartnerRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();

	for (int i = 0; it_begin != it_end; ++it_begin, ++i)
	{
		m_dirty_mark[rank_type].insert(i);
	}

	m_rank_skip_list[rank_type].Clear();
	m_user_index_map[rank_type].clear();
	m_change_flag[rank_type] = true;
}

const PartnerRankItem * PartnerRank::GetPartnerRankItem(int rank_type, const PartnerRankKey & partner_key)
{
	if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX) return NULL;

	PartnerIndexMapIt it = m_partner_index_map[rank_type].find(partner_key);
	if (it != m_partner_index_map[rank_type].end())
	{
		static PartnerRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		SkipList<PartnerRankItem>::Iterator it_sub = m_rank_skip_list[rank_type].Find(check_item);
		if (m_rank_skip_list[rank_type].End() != it_sub)
		{
			return &(*it_sub);
		}
	}

	return NULL;
}

bool PartnerRank::IsRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return true;
	}

	this->CalcRankLoadFinish();

	return m_is_all_load_finish;
}

void PartnerRank::InitIndex(int _rank_type, bool _is_fault_tolerant, std::set<int> & _check_set)
{
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::InitIndex INVALID RANK TYPE] [rank_type:%d]", (int)_rank_type);
		return;
	}

	SkipList<PartnerRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PartnerRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	for (; it_begin != it_end; ++it_begin)
	{
		int rank_index = m_rank_skip_list[_rank_type].GetIndex(*it_begin);
		if (-1 == rank_index)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::InitUserIndex ERROR] rank_type[%d] [user[%d] "
				"partner_id[%d] partner_capability[%lld]"
				"list_size[%d]  list_size_is_full[%d]]",
				_rank_type, it_begin->user_uid, it_begin->partner_id, it_begin->rank_value,
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
		
		PartnerRankKey key(it_begin->user_uid, it_begin->partner_id);
		this->AddPartnerIndex(_rank_type, key, *it_begin);
		this->AddUserIndex(_rank_type, it_begin->user_uid, key, *it_begin);
	}
}

void PartnerRank::Init(const PartnerRankParam & partnerrank_param, int _rank_type)
{
	int count = 0;
	int max_index = -1;
	bool is_fault_tolerant = false;
	std::set<int >  check_set;

	if (partnerrank_param.count < PARTNER_RANK_MAX_SIZE)
	{
		count = partnerrank_param.count;
	}
	else
	{
		count = PARTNER_RANK_MAX_SIZE;
	}

	for (int i = count - 1; i >= 0; i--)
	{
		int rank_type = partnerrank_param.partner_rank_list[i].rank_type;
		if (rank_type < 0 || rank_type >= PARTNER_RANK_TYPE_MAX)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::Init INVALID RANK TYPE] [rank_type:%d]", (int)rank_type);
			continue;
		}

		int rank_index = partnerrank_param.partner_rank_list[i].index;
		if (rank_index < 0 || rank_index >= PARTNER_RANK_MAX_SIZE)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::Init INVALID RANK INDEX] [rank_index:%d]", (int)rank_index);
			continue;
		}

		if (partnerrank_param.partner_rank_list[i].user_uid <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::Init INVALID USER INFO] [user[%d]]", partnerrank_param.partner_rank_list[i].user_uid);
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
			static PartnerRankItem tmp_data;
			tmp_data.Reset();

			tmp_data.user_uid = partnerrank_param.partner_rank_list[i].user_uid;
			tmp_data.partner_id = partnerrank_param.partner_rank_list[i].partner_id;
			tmp_data.rank_type = partnerrank_param.partner_rank_list[i].rank_type;
			tmp_data.rank_value = partnerrank_param.partner_rank_list[i].rank_value;
			tmp_data.flexible_int = partnerrank_param.partner_rank_list[i].flexible_int;
			tmp_data.flexible_ll = partnerrank_param.partner_rank_list[i].flexible_ll;

			std::pair<SkipList<PartnerRankItem>::Iterator, bool> ret = m_rank_skip_list[rank_type].Insert(tmp_data);
			if (!ret.second)
			{
				// 插入失败说明数据库里出现重复数据 , 或者需要检查 结构的operoperator==
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::Init Insert RANK INFO ERROR] [user[%d] rank_type[%d] "
					"list_size[%d]  list_size_is_full[%d]]",
					(int)partnerrank_param.partner_rank_list[i].user_uid, rank_type,
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

void PartnerRank::Update(time_t now_second)
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

void PartnerRank::OnUserLogin(Role * user)
{
	if (NULL == user) return;

	for (size_t i = 1; i <= PARTNER_NUM_MAX; i++)
	{
		if (!user->GetRoleModuleManager()->GetPartner()->IsPartnerInvalid(i) && user->GetRoleModuleManager()->GetPartner()->IsPartnerAlreadyActive(i))
		{
			continue;
		}

		this->SyncPartnerRankInfo(user, PARTNER_RANK_TYPE_CAPABILITY, i);
	}	
}

void PartnerRank::OnRankLog(time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;	

	m_next_log_timestamp = static_cast<unsigned int>(now_second + EngineAdapter::Instance().NextHourInterval(0, 0));

	//整点记录一次排行榜信息
	for (int i = 0; i < PARTNER_RANK_TYPE_MAX; i++)
	{
		SkipList<PartnerRankItem>::Iterator it_begin = m_rank_skip_list[i].Begin();
		SkipList<PartnerRankItem>::Iterator it_end = m_rank_skip_list[i].End();
		int count = 0;
		for (; it_begin != it_end; ++it_begin)
		{
			gamelog::g_log_rank.printf(LL_INFO, "PartnerRank::OnRankLog rank_type[%d] rank_index[%d] user[%d] "
				" partner_id[%d]  "
				"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
				i, count, it_begin->user_uid,
				it_begin->partner_id,
				it_begin->rank_value, it_begin->flexible_int, it_begin->flexible_ll);

			++count;
		}
	}
}

void PartnerRank::GetChangeRankItemList(PartnerRankParam *partnerrank_param, int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX || NULL == partnerrank_param)
	{
		return;
	}

	if (!m_change_flag[_rank_type])
	{
		partnerrank_param->count = 0;
		return;
	}

	int count = 0;

	std::set < int > ::iterator it_begin = m_dirty_mark[_rank_type].begin();
	std::set < int >::iterator it_end = m_dirty_mark[_rank_type].end();

	for (; it_begin != it_end && count < PARTNER_RANK_MAX_SIZE; ++it_begin)
	{
		if (*it_begin >= (int)m_rank_skip_list[_rank_type].Size())
		{
			// 说明这个数据已经被从排行删了
			partnerrank_param->partner_rank_list[count].change_state = structcommon::CS_DELETE;
			partnerrank_param->partner_rank_list[count].rank_type = _rank_type;
		}
		else
		{
			std::set < int > ::iterator it = m_old_state[_rank_type].find(*it_begin);
			if (m_old_state[_rank_type].end() == it)
			{
				partnerrank_param->partner_rank_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				partnerrank_param->partner_rank_list[count].change_state = structcommon::CS_UPDATE;
			}

			SkipList<PartnerRankItem>::Iterator rank_iter = m_rank_skip_list[_rank_type][*it_begin];
			if (m_rank_skip_list[_rank_type].End() == rank_iter)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PartnerRank::GetChangeRankItemList ERROR] "
					"rank_type[%d]  rank_index[%d]  rank_list_size[%u]",
					_rank_type, *it_begin, m_rank_skip_list[_rank_type].Size());
				continue;
			}

			partnerrank_param->partner_rank_list[count].index = *it_begin;
			partnerrank_param->partner_rank_list[count].user_uid = rank_iter->user_uid;
			partnerrank_param->partner_rank_list[count].partner_id = rank_iter->partner_id;
			partnerrank_param->partner_rank_list[count].rank_type = rank_iter->rank_type;
			partnerrank_param->partner_rank_list[count].rank_value = rank_iter->rank_value;
			partnerrank_param->partner_rank_list[count].flexible_int = rank_iter->flexible_int;
			partnerrank_param->partner_rank_list[count].flexible_ll = rank_iter->flexible_ll;
		}

		++count;
	}

	partnerrank_param->count = count;
}

void PartnerRank::ClearDirtyMark(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_rank_type]) return;

	m_dirty_mark[_rank_type].clear();
	m_old_state[_rank_type].clear();

	SkipList<PartnerRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PartnerRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	int rank_index = 0;
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state[_rank_type].insert(rank_index++);
	}

	m_change_flag[_rank_type] = false;
}

void PartnerRank::SyncPartnerRankInfo(Role * _role, int _rank_type, int _partner_id, bool _is_force)
{
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX || NULL == _role ||
		_role->GetRoleModuleManager()->GetPartner()->IsPartnerInvalid(_partner_id) || !_role->GetRoleModuleManager()->GetPartner()->IsPartnerAlreadyActive(_partner_id))
	{
		gamelog::g_log_rank.printf(LL_INFO, "ParnerRank::SyncParnerRankInfo[ERROR] user[%d  %s]  rank_type:%d  parner_id:%d  is_froce:%d",
			NULL == _role ? 0 : _role->GetUID(), NULL == _role ? "NULL" : _role->GetName(), _rank_type, _partner_id, _is_force);
		return;
	}

	this->_SyncPartnerRankInfo(_role, _rank_type, _partner_id, _is_force);
}

