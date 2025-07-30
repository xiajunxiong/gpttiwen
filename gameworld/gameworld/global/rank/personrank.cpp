#include "personrank.hpp"

#include "rankmanager.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/structcommon.h"

#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "other/event/eventhandler.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/performancecatch.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"
#include "other/pet/pet.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "global/cross/crossuserregister.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/salary/salary.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"
#include "friend/friendmanager.h"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "equipment/equipmentmanager.h"
#include "other/endlesstower/endlesstower.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "global/datasynccheck/datasynccheck.hpp"
#include "other/fallengod/fallengod.hpp"
#include "other/levelcomplement/levelcomplementconfig.hpp"
#include "protocol/msglevelcomplement.h"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/couragechallenge/newcouragechallenge.hpp"

PersonRank::PersonRank()
	: m_rank_manager(NULL), m_next_log_timestamp(0), m_average_level_by_level_complement(0), m_is_all_load_finish(false)
{
	memset(m_load_flag, 0, sizeof(m_load_flag));
	memset(m_change_flag, 0, sizeof(m_change_flag));
	m_average_level_by_level_map.clear();

	for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].SetDefaultMaxSize(PERSON_RANK_MAX_SIZE);
		m_rank_skip_list[i].SetCompare(&PersonRankItem::RankCompare);

		m_dirty_mark[i].clear();
		m_old_state[i].clear();
	}
}

PersonRank::~PersonRank()
{
	for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
	{
		m_rank_skip_list[i].Clear();
	}
}

void PersonRank::Init(const PersonRankParam &personrank_param, int _rank_type)
{
	int count = 0;
	int max_index = -1;
	bool is_fault_tolerant = false;
	std::set<int> check_set;

	if (personrank_param.count < static_cast<int>(PERSON_RANK_MAX_SIZE))
	{
		count = personrank_param.count;
	}
	else
	{
		count = static_cast<int>(PERSON_RANK_MAX_SIZE);
	}

	for (int i = count - 1; i >= 0; i--)
	{
		int rank_type = personrank_param.person_rank_list[i].rank_type;
		if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init INVALID RANK TYPE] [rank_type:%d]", (int)rank_type);
			continue;
		}

		if (rank_type != _rank_type)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init RANK TYPE ERROR] [rank_type:%d]  [_rank_type:%d]", (int)rank_type, _rank_type);
			continue;
		}

		int rank_index = personrank_param.person_rank_list[i].index;
		if (rank_index < 0 || rank_index >= static_cast<int>(PERSON_RANK_MAX_SIZE))
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init INVALID RANK INDEX] [rank_index:%d]", (int)rank_index);
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

		if (personrank_param.person_rank_list[i].role_info.uid <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init INVALID USER INFO] [user[%d %s]]",
				(int)personrank_param.person_rank_list[i].role_info.uid, (const char*)personrank_param.person_rank_list[i].role_info.user_name);
			continue;
		}

		if (rank_type > PERSON_RANK_TYPE_CAPABILITY && rank_type < PERSON_RANK_TYPE_PROF_CAPABILITY_END && (personrank_param.person_rank_list[i].role_info.prof / PROFESSION_BASE) + PERSON_RANK_TYPE_CAPABILITY != rank_type)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init INVALID USER INFO] [user[%d %s] prof[%d] rank_type[%d]]",
				(int)personrank_param.person_rank_list[i].role_info.uid, (const char*)personrank_param.person_rank_list[i].role_info.user_name,
				personrank_param.person_rank_list[i].role_info.prof, rank_type);
			continue;
		}

		// 新的排行榜结构测试
		{
			static PersonRankItem tmp_data;
			tmp_data.Reset();
			tmp_data.role_info = personrank_param.person_rank_list[i].role_info;
			tmp_data.rank_type = personrank_param.person_rank_list[i].rank_type;
			tmp_data.rank_value = personrank_param.person_rank_list[i].rank_value;
			tmp_data.reserve_int = 0;
			tmp_data.flexible_int = personrank_param.person_rank_list[i].flexible_int;
			tmp_data.flexible_ll = personrank_param.person_rank_list[i].flexible_ll;

			std::pair<SkipList<PersonRankItem>::Iterator, bool> ret = m_rank_skip_list[rank_type].Insert(tmp_data);
			if (!ret.second)
			{
				// 插入失败说明数据库里出现重复数据 , 或者需要检查 结构的operoperator==
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::Init Insert RANK INFO ERROR] [user[%d %s] prof[%d] rank_type[%d] "
					"list_size[%d]  list_size_is_full[%d]]",
					(int)personrank_param.person_rank_list[i].role_info.uid, (const char*)personrank_param.person_rank_list[i].role_info.user_name,
					personrank_param.person_rank_list[i].role_info.prof, rank_type,
					m_rank_skip_list[rank_type].Size(), m_rank_skip_list[rank_type].IsFull());
			}
		}
	}

	if (!WorldStatus::Instance().IsResetComCapRank())
	{
		this->ClearRank(PERSON_RANK_TYPE_CAPABILITY);
		WorldStatus::Instance().SetResetComCapRankFlag(true);
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

	this->InitUserIndex(_rank_type, is_fault_tolerant, check_set);
	if (PERSON_RANK_TYPE_LEVEL == _rank_type)
	{
		this->OnSetAverageLevelByLevelComplement();
	}

	m_load_flag[_rank_type] = true;
}

void PersonRank::Update(time_t now_second)
{
	if (!IsRankLoadFinish() || CrossConfig::Instance().IsHiddenServer()) return;

	if (0 == m_next_log_timestamp)
	{
		m_next_log_timestamp = (unsigned int)EngineAdapter::Instance().Time();
	}

	if (now_second > m_next_log_timestamp)
	{
		this->OnRankLog(now_second);
	}

	if (WorldStatus::Instance().GetCommonData().clear_person_rank_flag == 0)
	{
		this->ClearRank(PERSON_RANK_TYPE_ENDLESS_TOWER);
		WorldStatus::Instance().GetCommonData().clear_person_rank_flag = 1;
	}
}

void PersonRank::OnUserLogin(Role *user)
{
	if (NULL == user)
		return;

	this->SyncPersonRankInfo(user, PERSON_RANK_TYPE_LEVEL);
	this->SyncPersonRankInfo(user, PERSON_RANK_TYPE_LEVEL + user->GetAppearanceProf());
	this->SyncPersonRankInfo(user, PERSON_RANK_TYPE_CAPABILITY);
	this->SyncPersonRankInfo(user, PERSON_RANK_TYPE_CAPABILITY + user->GetAppearanceProf());
}

void PersonRank::OnRankLog(time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_next_log_timestamp = static_cast<unsigned int>(now_second + EngineAdapter::Instance().NextHourInterval(0, 0));

	//整点记录一次排行榜信息
	for (int i = 0; i < PERSON_RANK_TYPE_MAX; i++)
	{
		SkipList<PersonRankItem>::Iterator it_begin = m_rank_skip_list[i].Begin();
		SkipList<PersonRankItem>::Iterator it_end = m_rank_skip_list[i].End();
		int count = 0;
		for (; it_begin != it_end; ++it_begin)
		{
			gamelog::g_log_personrank_snapshot.printf(LL_INFO, "PersonRank::OnRankLog  rank_type[%d] rank_index[%d] user[%d,%s] "
				"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
				i, count, it_begin->role_info.uid, it_begin->role_info.user_name,
				it_begin->rank_value, it_begin->flexible_int, it_begin->flexible_ll);

			++count;
		}
	}
}

void PersonRank::OnGameworldAccept()
{
	if (!this->IsRankLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
	{
		this->SyncRankAllDataToCross(i);
	}
	this->OnGameHiddenSynLevelComplementInfo();
}

void PersonRank::GetChangeRankItemList(PersonRankParam *personrank_param, int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX)
	{
		return;
	}

	if (NULL == personrank_param) return;

	if (!m_change_flag[_rank_type])
	{
		personrank_param->count = 0;
		return;
	}

	int count = 0;
	std::set < int > ::iterator it_begin = m_dirty_mark[_rank_type].begin();
	std::set < int >::iterator it_end = m_dirty_mark[_rank_type].end();

	for (; it_begin != it_end && count < static_cast<int>(PERSON_RANK_MAX_SIZE); ++it_begin)
	{
		if (*it_begin >= (int)m_rank_skip_list[_rank_type].Size())
		{
			// 说明这个数据已经被从排行删了
			personrank_param->person_rank_list[count].change_state = structcommon::CS_DELETE;
			personrank_param->person_rank_list[count].index = *it_begin;
			personrank_param->person_rank_list[count].rank_type = _rank_type;
		}
		else
		{
			std::set < int > ::iterator it = m_old_state[_rank_type].find(*it_begin);
			if (m_old_state[_rank_type].end() == it)
			{
				personrank_param->person_rank_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				personrank_param->person_rank_list[count].change_state = structcommon::CS_UPDATE;
			}

			SkipList<PersonRankItem>::Iterator rank_iter = m_rank_skip_list[_rank_type][*it_begin];
			if (m_rank_skip_list[_rank_type].End() == rank_iter)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::GetChangeRankItemList ERROR] "
					"rank_type[%d]  rank_index[%d]  rank_list_size[%u]",
					_rank_type, *it_begin, m_rank_skip_list[_rank_type].Size());
				continue;
			}

			personrank_param->person_rank_list[count].index = *it_begin;
			personrank_param->person_rank_list[count].role_info = rank_iter->role_info;
			personrank_param->person_rank_list[count].rank_type = rank_iter->rank_type;
			personrank_param->person_rank_list[count].rank_value = rank_iter->rank_value;
			personrank_param->person_rank_list[count].record_index = 0;
			personrank_param->person_rank_list[count].flexible_int = rank_iter->flexible_int;
			personrank_param->person_rank_list[count].flexible_ll = rank_iter->flexible_ll;
		}

		++count;
	}

	personrank_param->count = count;
}

void PersonRank::ClearDirtyMark(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_rank_type]) return;

	m_dirty_mark[_rank_type].clear();
	m_old_state[_rank_type].clear();

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	int rank_index = 0;
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state[_rank_type].insert(rank_index++);
	}

	m_change_flag[_rank_type] = false;
}

int PersonRank::GetRankList(int rank_type, int count, PersonRankItem *rank_list)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return 0;
	}

	if (NULL == rank_list || count <= 0 || count > static_cast<int>(PERSON_RANK_MAX_SIZE)) return 0;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return 0;

	int rank_count = 0;

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	int index = 0;
	for (; it_begin != it_end && rank_count < static_cast<int>(PERSON_RANK_MAX_SIZE) && rank_count < count; ++it_begin, ++index)
	{
		rank_list[rank_count].role_info = it_begin->role_info;
		rank_list[rank_count].reserve_int = 0;

		rank_list[rank_count].rank_value = it_begin->rank_value;
		rank_list[rank_count].flexible_int = it_begin->flexible_int;
		rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

		++rank_count;
	}

	return rank_count;
}
void PersonRank::GetRankList(int rank_type, int count, std::vector<int> & rank_uid_list)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	if (count <= 0 || count > static_cast<int>(PERSON_RANK_MAX_SIZE)) return;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	int rank_count = 0;
	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	for (; it_begin != it_end && rank_count < static_cast<int>(PERSON_RANK_MAX_SIZE) && rank_count < count; ++it_begin)
	{
		rank_uid_list.push_back(it_begin->role_info.uid);
		++rank_count;
	}

	return;
}
void PersonRank::OnGetRankList(Role *role, Protocol::CSGetPersonRankList &rank_list_req)
{
	if (!IsRankLoadFinish() || NULL == role)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE == rank_list_req.rank_type ||
			PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK == rank_list_req.rank_type ||
			PERSON_RANK_TYPE_ENDLESS_TOWER == rank_list_req.rank_type)
		{
			crossgameprotocal::HiddenGameGetRankListReq hggrlr;
			hggrlr.role_id = role->GetUID();
			hggrlr.special_type = rank_list_req.special_type;
			hggrlr.rank_type = rank_list_req.rank_type;
			hggrlr.rank_start = rank_list_req.rank_start;
			hggrlr.rank_end = rank_list_req.rank_end;

			InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&hggrlr, sizeof(hggrlr));
			//gamelog::g_log_rank.printf(LL_INFO, "PersonRank::OnGetRankList IsHiddenServer user[%d] rank_type[%d]", role->GetUID(), rank_list_req.rank_type);
			return;
		}

		if (PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY == rank_list_req.rank_type ||
			PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE == rank_list_req.rank_type)
		{
			if (rank_list_req.special_type == Protocol::CSGetPersonRankList::SPECIAL_TYPE_1)
			{
				crossgameprotocal::HiddenGameGetFriendRankListReq hggrlr;
				hggrlr.role_id = role->GetUID();
				hggrlr.rank_type = rank_list_req.rank_type;
				hggrlr.rank_start = rank_list_req.rank_start;
				hggrlr.rank_end = rank_list_req.rank_end;
				hggrlr.friend_count = 0;

				const FriendManager::UserFrinedSet * friend_set = FriendManager::Instance().GetUserFriendSetInfo(role);
				if (NULL != friend_set)
				{
					for (FriendManager::UserFrinedSet::const_iterator it = friend_set->begin(); it != friend_set->end(); it++)
					{
						long long friend_for_index = *it;
						int friend_uid = (friend_for_index << 32) >> 32;

						hggrlr.m_friend_list[hggrlr.friend_count++] = friend_uid;
					}
				}

				InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&hggrlr, sizeof(hggrlr));
				
			}
			else if(rank_list_req.special_type == Protocol::CSGetPersonRankList::SPECIAL_TYPE_2)
			{
				if (role->GetGuildID() <= 0)return;
				crossgameprotocal::HiddenGameGetRankListReq hggrlr;
				hggrlr.role_id = role->GetUID();
				hggrlr.special_type = rank_list_req.special_type;
				hggrlr.rank_type = rank_list_req.rank_type;
				hggrlr.rank_start = rank_list_req.rank_start;
				hggrlr.rank_end = rank_list_req.rank_end;

				InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&hggrlr, sizeof(hggrlr));
			}
			else
			{
				crossgameprotocal::HiddenGameGetRankListReq hggrlr;
				hggrlr.role_id = role->GetUID();
				hggrlr.special_type = rank_list_req.special_type;
				hggrlr.rank_type = rank_list_req.rank_type;
				hggrlr.rank_start = rank_list_req.rank_start;
				hggrlr.rank_end = rank_list_req.rank_end;

				InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&hggrlr, sizeof(hggrlr));
			}
			return;
		}
	}

	int rank_type = rank_list_req.rank_type;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	if (rank_list_req.rank_start < 0 || rank_list_req.rank_end < 0 || rank_list_req.rank_start  > rank_list_req.rank_end)
	{
		return;
	}

	int my_rank_index = -1;
	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(role->GetUserId());
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		static PersonRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, my_index_it->second);
		my_rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
	}

	static Protocol::SCGetPersonRankListAck rank_list_ack;
	rank_list_ack.rank_type = rank_type;
	rank_list_ack.my_rank_index = my_rank_index;
	rank_list_ack.special_type = rank_list_req.special_type;
	rank_list_ack.count = 0;

	int rank_count = 0, target_get_count = rank_list_req.rank_end - rank_list_req.rank_start + 1;
	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type][rank_list_req.rank_start];
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	Guild * m_guild = role->GetGuild();
	for (; it_begin != it_end && rank_count < static_cast<int>(PERSON_RANK_MAX_SIZE) && rank_count < target_get_count; ++it_begin)
	{
		if (rank_list_req.special_type == Protocol::CSGetPersonRankList::SPECIAL_TYPE_1)
		{
			if (role->GetUID() != it_begin->role_info.uid && !FriendManager::Instance().IsFriend(role->GetUID(), it_begin->role_info.uid))
			{
				continue;
			}
		}
		if (rank_list_req.special_type == Protocol::CSGetPersonRankList::SPECIAL_TYPE_2)
		{
			if (NULL == m_guild)continue;
			if (role->GetUID() != it_begin->role_info.uid && !m_guild->IsMember(it_begin->role_info.uid))continue;
		}
		rank_list_ack.rank_list[rank_count].role_info = it_begin->role_info;

		rank_list_ack.rank_list[rank_count].rank_index = rank_count + rank_list_req.rank_start;
		rank_list_ack.rank_list[rank_count].rank_value = it_begin->rank_value;
		rank_list_ack.rank_list[rank_count].flexible_int = it_begin->flexible_int;
		rank_list_ack.rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

		++rank_count;
	}

	rank_list_ack.count = rank_count;
	int sendlen = sizeof(rank_list_ack) - (PERSON_RANK_MAX_SIZE - rank_list_ack.count) * sizeof(rank_list_ack.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&rank_list_ack, sendlen);
}

void PersonRank::OnGetRankTopUser(Role *role, Protocol::CSGetPersonRankTopUser &top_user_req)
{
	// 原逻辑屏蔽了 ,所以这里先屏蔽
	return;

	if (!m_load_flag) return;

	int rank_type = top_user_req.rank_type;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	SkipList<PersonRankItem>::Iterator  it = m_rank_skip_list[rank_type].Begin();
	if (m_rank_skip_list[rank_type].End() == it)
	{
		return;
	}

	// 	static Protocol::SCGetPersonRankTopUser top_user_ack;
	// 	top_user_ack.rank_type = rank_type;
	// 	top_user_ack.user_id = UidToInt(top_user_item.user_id);
	// 	F_STRNCPY(top_user_ack.user_name, top_user_item.user_name, sizeof(top_user_item.user_name));
	// 	top_user_ack.sex = top_user_item.sex;
	// 	top_user_ack.prof = top_user_item.prof;
	// 	top_user_ack.camp = top_user_item.camp;
	// 	top_user_ack.reserved = 0;
	// 	top_user_ack.level = top_user_item.level;
	// 	top_user_ack.rank_value = top_user_item.rank_value;
	// 
	// 	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&top_user_ack, sizeof(top_user_ack));
}

void PersonRank::OnHiddenGameGetRankList(crossgameprotocal::HiddenGameGetRankListReq * req)
{
	if (!IsRankLoadFinish()) return;

	int rank_type = req->rank_type;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	if (req->rank_start < 0 || req->rank_end < 0 || req->rank_start  > req->rank_end)
	{
		return;
	}
	int my_rank_index = -1;
	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(IntToUid(req->role_id));
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		static PersonRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, my_index_it->second);
		my_rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
	}

	static Protocol::SCGetPersonRankListAck rank_list_ack;
	rank_list_ack.rank_type = rank_type;
	rank_list_ack.my_rank_index = my_rank_index;
	rank_list_ack.special_type = req->special_type;
	rank_list_ack.count = 0;

	int rank_count = 0, target_get_count = req->rank_end - req->rank_start + 1;
	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type][req->rank_start];
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	Guild* m_guild = GuildManager::Instance().GetGuild(IntToUid(req->role_id));
	for (; it_begin != it_end && rank_count < static_cast<int>(PERSON_RANK_MAX_SIZE) && rank_count < target_get_count; ++it_begin)
	{
		if (req->special_type == Protocol::CSGetPersonRankList::SPECIAL_TYPE_2)
		{
			if (NULL == m_guild)continue;
			if (!m_guild->IsMember(it_begin->role_info.uid))continue;
		}
		rank_list_ack.rank_list[rank_count].role_info = it_begin->role_info;

		rank_list_ack.rank_list[rank_count].rank_index = rank_count + req->rank_start;
		rank_list_ack.rank_list[rank_count].rank_value = it_begin->rank_value;
		rank_list_ack.rank_list[rank_count].flexible_int = it_begin->flexible_int;
		rank_list_ack.rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

		++rank_count;
	}

	rank_list_ack.count = rank_count;
	int sendlen = sizeof(rank_list_ack) - (PERSON_RANK_MAX_SIZE - rank_list_ack.count) * sizeof(rank_list_ack.rank_list[0]);
	InternalComm::Instance().SendToHiddenUserThroughCross(req->role_id, (const char *)&rank_list_ack, sendlen);
	/*gamelog::g_log_rank.printf(LL_INFO, "PersonRank::OnHiddenGameGetRankList user[%d] rank_type[%d] rank_count[%d]",
		req->role_id, req->rank_type, rank_count);*/
}

void PersonRank::OnHiddenGameGetFriendRankList(crossgameprotocal::HiddenGameGetFriendRankListReq * req)
{
	if (!IsRankLoadFinish()) return;

	int rank_type = req->rank_type;
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	if (req->rank_start < 0 || req->rank_end < 0 || req->rank_start  > req->rank_end)
	{
		return;
	}
	int my_rank_index = -1;
	UserIndexMapIt my_index_it = m_user_index_map[rank_type].find(IntToUid(req->role_id));
	if (m_user_index_map[rank_type].end() != my_index_it)
	{
		static PersonRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, my_index_it->second);
		my_rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
	}

	static Protocol::SCGetPersonRankListAck rank_list_ack;
	rank_list_ack.rank_type = rank_type;
	rank_list_ack.my_rank_index = my_rank_index;
	rank_list_ack.special_type = Protocol::CSGetPersonRankList::SPECIAL_TYPE_1;
	rank_list_ack.count = 0;

	int rank_count = 0, target_get_count = req->rank_end - req->rank_start + 1;
	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type][req->rank_start];
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();

	for (; it_begin != it_end && rank_count < static_cast<int>(PERSON_RANK_MAX_SIZE) && rank_count < target_get_count; ++it_begin)
	{
		for (int friend_idx = 0; friend_idx < req->friend_count && friend_idx < MAX_FRIEND_NUM; friend_idx++)
		{
			if (req->m_friend_list[friend_idx] == it_begin->role_info.uid)
			{
				rank_list_ack.rank_list[rank_count].role_info = it_begin->role_info;

				rank_list_ack.rank_list[rank_count].rank_index = rank_count + req->rank_start;
				rank_list_ack.rank_list[rank_count].rank_value = it_begin->rank_value;
				rank_list_ack.rank_list[rank_count].flexible_int = it_begin->flexible_int;
				rank_list_ack.rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

				++rank_count;
				break;
			}
		}

		if (req->role_id == it_begin->role_info.uid)
		{
			rank_list_ack.rank_list[rank_count].role_info = it_begin->role_info;

			rank_list_ack.rank_list[rank_count].rank_index = rank_count + req->rank_start;
			rank_list_ack.rank_list[rank_count].rank_value = it_begin->rank_value;
			rank_list_ack.rank_list[rank_count].flexible_int = it_begin->flexible_int;
			rank_list_ack.rank_list[rank_count].flexible_ll = it_begin->flexible_ll;

			++rank_count;
		}
	}

	rank_list_ack.count = rank_count;
	int sendlen = sizeof(rank_list_ack) - (PERSON_RANK_MAX_SIZE - rank_list_ack.count) * sizeof(rank_list_ack.rank_list[0]);
	InternalComm::Instance().SendToHiddenUserThroughCross(req->role_id, (const char *)&rank_list_ack, sendlen);
}

void PersonRank::OnBaseInfoChange(Role *_role, int _old_prof, int _cur_prof)
{
	if (NULL == _role)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnBaseInfoChangeByHidden(_role, _old_prof, _cur_prof);
	}
	else
	{
		static RoleBaseInfo info;
		info.Reset();
		this->UpdateBaseInfo(info, _role);
		this->OnBaseInfoChangeByGame(info, _old_prof, _cur_prof);
	}
}

void PersonRank::SyncPersonRankInfo(Role * _role, int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX || NULL == _role)
	{
		return;
	}

	static PersonRankItem rank_item;
	rank_item.Reset();
	this->UpdateBaseInfo(rank_item.role_info, _role);

	switch (_rank_type)
	{
	case PERSON_RANK_TYPE_MEILI_QIXI:
		{
			rank_item.rank_type = _rank_type;
			RoleActivityCharismaticTanabata * activity = (RoleActivityCharismaticTanabata *)_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
			if (NULL != activity)
			{
				rank_item.rank_value = activity->GetMeiLiValue();
			}
			rank_item.flexible_ll = (long long)EngineAdapter::Instance().Time();
		}
		break;
	case PERSON_RANK_TYPE_FALLEN_GOD:
		{
			rank_item.rank_type = _rank_type;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetFallenGod()->GetTopSn();
			rank_item.flexible_ll = _role->GetRoleModuleManager()->GetFallenGod()->GetBattleRound(rank_item.rank_value);
		}
		break;
	case PERSON_RANK_TYPE_ENDLESS_TOWER:
		{
			rank_item.rank_type = _rank_type;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetEndlessTower()->GetBattleSeq();
			rank_item.flexible_ll = (long long)_role->GetRoleModuleManager()->GetEndlessTower()->GetBattleTamp();
		}
		break;
	case PERSON_RANK_TYPE_BIG_DIPPER_1:
	case PERSON_RANK_TYPE_BIG_DIPPER_2:
	case PERSON_RANK_TYPE_BIG_DIPPER_3:
	case PERSON_RANK_TYPE_BIG_DIPPER_4:
	case PERSON_RANK_TYPE_BIG_DIPPER_5:
	case PERSON_RANK_TYPE_BIG_DIPPER_6:
	case PERSON_RANK_TYPE_BIG_DIPPER_7:
	case PERSON_RANK_TYPE_BIG_DIPPER_8:
		{
			rank_item.rank_type = _rank_type;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetBigDipper()->GetBattleRound(_rank_type - PERSON_RANK_TYPE_BIG_DIPPER_1);
			rank_item.flexible_ll = (long long)EngineAdapter::Instance().Time();
		}
		break;
	case PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetNetherWorldPurgatory()->GetRewardNum();
			rank_item.flexible_int = _role->GetRoleModuleManager()->GetNetherWorldPurgatory()->GetChallengeChapter();
			rank_item.flexible_ll = _role->GetRoleModuleManager()->GetNetherWorldPurgatory()->GetChallengeId();
		}
		break;
	case PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetLunHuiWangChuan()->GetPassIDX();
			rank_item.flexible_ll = (long long)EngineAdapter::Instance().Time();
		}
		break;
	case PERSON_RANK_TYPE_CHIVALROUS:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_CHIVALROUS;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetSalary()->GetWeekTotalChivalrous();
			rank_item.flexible_ll = (long long)EngineAdapter::Instance().Time();
		}
		break;
	case PERSON_RANK_TYPE_LEVEL:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_LEVEL;
			rank_item.rank_value = _role->GetLevel();
			rank_item.flexible_ll = _role->GetExp();
		}
		break;
	case 	REASON_RANK_TYPE_PROF_LEVEL_1:
	case 	REASON_RANK_TYPE_PROF_LEVEL_2:
	case 	REASON_RANK_TYPE_PROF_LEVEL_3:
	case 	REASON_RANK_TYPE_PROF_LEVEL_4:
	case 	REASON_RANK_TYPE_PROF_LEVEL_5:
	case 	REASON_RANK_TYPE_PROF_LEVEL_6:
	case 	REASON_RANK_TYPE_PROF_LEVEL_7:
	case 	REASON_RANK_TYPE_PROF_LEVEL_8:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_LEVEL + _role->GetAppearanceProf();
			rank_item.rank_value = _role->GetLevel();
			rank_item.flexible_ll = _role->GetExp();
		}
		break;
	case PERSON_RANK_TYPE_CAPABILITY:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_CAPABILITY;
			rank_item.rank_value = _role->GetCapabilityWithFight();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_1:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_2:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_3:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_4:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_5:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_6:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_7:
	case 	PERSON_RANK_TYPE_PROF_CAPABILITY_8:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_CAPABILITY + _role->GetAppearanceProf();
			rank_item.rank_value = _role->GetCapability();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
	case 	PERSON_RANK_TYPE_HUASHEN_CAPABILITY:				// 化身角色战力榜
		{
			_role->GetRoleModuleManager()->GetHuaShenManager()->GetPersonRankInfo(rank_item.role_info);
			rank_item.rank_type = PERSON_RANK_TYPE_HUASHEN_CAPABILITY;
			rank_item.rank_value = rank_item.role_info.capability;
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
	case 	PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP;
			rank_item.rank_value = _role->GetCapability() + _role->GetRoleModuleManager()->GetPet()->GetPetCapabilityMax();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
	case	PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetDreamNotesChallenge()->GetPassLevel();
			rank_item.flexible_ll = _role->GetRoleModuleManager()->GetDreamNotesChallenge()->GetPassNewLevelTimestamp();
		}
		break;
	case PERSON_RANK_TYPE_ROLE_CAPABILITY:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_ROLE_CAPABILITY;
			rank_item.rank_value = _role->GetCapability();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
		case PERSON_RANK_TYPE_FOUR_MAX_PARTNER:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_FOUR_MAX_PARTNER;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetPartner()->GetMaxFourPartnerCap();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
		case PERSON_RANK_TYPE_EQUIPMENT_CAP:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_EQUIPMENT_CAP;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetEquipmentManager()->GetEquipJewelryCap();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
		case PERSON_RANK_TYPE_PET_MAX_CAPABILITY:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_PET_MAX_CAPABILITY;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetPet()->GetPetCapabilityMax();
			rank_item.flexible_ll = _role->GetUID();
		}
		break;
		case PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE:
		{
			rank_item.rank_type = PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE;
			rank_item.rank_value = _role->GetRoleModuleManager()->GetNewCourageChallenge()->GetAllStarCount();
			rank_item.flexible_int = _role->GetRoleModuleManager()->GetNewCourageChallenge()->GetMaxLayerLevel();
			rank_item.flexible_ll = (long long)EngineAdapter::Instance().Time();
		}
	default:
		break;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnRankInfoChangeByHidden(_role, rank_item);
	}
	else
	{
		this->UpdateRankList(rank_item);
	}
}

void PersonRank::ClearRank(int rank_type)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return;

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	for (int i = 0; it_begin != it_end; ++it_begin, ++i)
	{
		m_dirty_mark[rank_type].insert(i);
	}

	m_rank_skip_list[rank_type].Clear();
	m_user_index_map[rank_type].clear();

	m_change_flag[rank_type] = true;

	this->SyncRankAllDataToCross(rank_type);
	this->OnRankChange(rank_type);
}

int PersonRank::GetPersonRankIndex(int rank_type, const UserID & user_id)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return NULL;

	UserIndexMapIt it = m_user_index_map[rank_type].find(user_id);
	if (it != m_user_index_map[rank_type].end())
	{
		static PersonRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		int tmp_i = m_rank_skip_list[rank_type].GetIndex(check_item);
		if (-1 != tmp_i)
		{
			return tmp_i;
		}
	}

	return -1;
}

const PersonRankItem * PersonRank::GetPersonRankItem(int rank_type, const UserID &user_id)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return NULL;

	UserIndexMapIt it = m_user_index_map[rank_type].find(user_id);
	if (it != m_user_index_map[rank_type].end())
	{
		static PersonRankItem check_item;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		SkipList<PersonRankItem>::Iterator  it_sub = m_rank_skip_list[rank_type].Find(check_item);
		if (m_rank_skip_list[rank_type].End() != it_sub)
		{
			return &(*it_sub);
		}
	}

	return NULL;
}

int PersonRank::GetRankTopUser(int rank_type, int max_user, UserID *user_list)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX || max_user <= 0 || max_user > static_cast<int>(PERSON_RANK_MAX_SIZE) || NULL == user_list) return 0;

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	int count = 0;
	for (; it_begin != it_end && count < max_user; ++it_begin, ++count)
	{
		user_list[count] = IntToUid(it_begin->role_info.uid);
	}

	return count;
}

int PersonRank::GetRankTopItem(int rank_type, int item_count, PersonRankItem *item_list)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX || item_count <= 0 || item_count > static_cast<int>(PERSON_RANK_MAX_SIZE) || NULL == item_list) return 0;

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	int count = 0;
	for (; it_begin != it_end && count < item_count; ++it_begin, ++count)
	{
		item_list[count] = *it_begin;
	}

	return count;
}

bool PersonRank::IsRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return true;
	}

	this->CalcRankLoadFinish();

	return m_is_all_load_finish;
}

void PersonRank::UserResetName(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->UserResetNameByHidden(_role);
	}
	else
	{
		GameName temp_name;
		_role->GetName(temp_name);
		this->UserResetNameByGame(_role->GetUID(), temp_name);
	}
}

void PersonRank::OnTopLevelResetSyn(UserCacheNode * node)
{
	if(NULL == node) return;

	PersonRankItem rank_item;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(node->uid);
		if (NULL == cui)
		{
			return;
		}
		rank_item.role_info.uid = UidToInt(cui->original_user_id);
		node->GetName(rank_item.role_info.user_name);
		std::string temp_str = rank_item.role_info.user_name;
		int index = temp_str.rfind('_');
		if (index > 0 && index < (int)sizeof(GameName))
		{
			rank_item.role_info.user_name[index] = '\0';
		}
	}
	else
	{
		rank_item.role_info.uid = node->uid;
		node->GetName(rank_item.role_info.user_name);
	}
	rank_item.role_info.prof = node->profession;
	rank_item.role_info.role_level = node->level;
	rank_item.role_info.avatar_type = node->avatar_type;
	rank_item.role_info.capability = node->capability;
	rank_item.role_info.exp = 0;
	rank_item.role_info.headshot_id = node->appearance.headshot_id;
	rank_item.role_info.top_level = node->top_level;
	rank_item.role_info.role_fly_flag = node->appearance.fly_flag;
	
	int rank_type_list[2] = { PERSON_RANK_TYPE_LEVEL, PERSON_RANK_TYPE_LEVEL + node->profession / PROFESSION_BASE};
	rank_item.rank_value = node->level;
	rank_item.flexible_ll = 0;

	for (int i = 0; i < ARRAY_LENGTH(rank_type_list); i++)
	{
		rank_item.rank_type = rank_type_list[i];
		if (CrossConfig::Instance().IsHiddenServer())
		{
			const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(node->uid);
			if (NULL == cui)
			{
				return;
			}
			static crossgameprotocal::HiddenGameRoleInfoChange change; change.Reset();
			change.origin_plat_type = cui->original_plat_type;
			change.origin_server_id = cui->original_server_id;
			change.rank_item = rank_item;

			switch (change.rank_item.rank_type)
			{
			case PERSON_RANK_TYPE_LEVEL:
				{
					change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL;
				}
				break;
				case REASON_RANK_TYPE_PROF_LEVEL_1:
				case REASON_RANK_TYPE_PROF_LEVEL_2:
				case REASON_RANK_TYPE_PROF_LEVEL_3:
				case REASON_RANK_TYPE_PROF_LEVEL_4:
				case REASON_RANK_TYPE_PROF_LEVEL_5:
				case REASON_RANK_TYPE_PROF_LEVEL_6:
				case REASON_RANK_TYPE_PROF_LEVEL_7:
				case REASON_RANK_TYPE_PROF_LEVEL_8:
				{
					change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL;
				}
				break;
				default:
				continue;
				break;
			}
			InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
		}
		else
		{
			this->UpdateRankList(rank_item);
		}
	}

}

void PersonRank::SyncRankDataFromHidden(crossgameprotocal::HiddenGameRoleInfoChange * _msg)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == _msg)
	{
		return;
	}

	switch (_msg->sync_info_type)
	{
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_PROF:
		{
			this->OnBaseInfoChangeByGame(_msg->rank_item.role_info, _msg->param_1, _msg->param_2);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_NAME:
		{
			this->UserResetNameByGame(_msg->rank_item.role_info.uid, _msg->rank_item.role_info.user_name);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CAPABILITY:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_DREAM_NOTES:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LUNHUIWANGCHUAN:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CHIVALROUS:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NETHER_REWARD:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_KILL_BOSS:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_ENDLESS_TOWER:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_FALLEN_GOD:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_MEILI_QIXI:
	case crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NEW_COURAGE_CHALLENGE:
		{
			this->UpdateRankList(_msg->rank_item);
		}
		break;
	default:
		break;
	}
}

void PersonRank::OnHiddenRecvGameSynLevelComplementInfo(crossgameprotocal::GameHiddenSynLevelComplementInfo * msg)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == msg) return;

	m_average_level_by_level_map[msg->server_id] = msg->average_level_by_level_complement;

	Protocol::SCNewLevelComplementSpeicalInfo info;
	info.my_server_average_level = msg->average_level_by_level_complement;

	std::vector<int> online_user_list;
	UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
	for (int i = 0; i < (int)online_user_list.size(); i++)
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
		if (NULL == role || msg->server_id != role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId()) continue;

		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

int PersonRank::GetAverageByType(int rank_type, int max_count)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX || max_count <= 0 || max_count > static_cast<int>(PERSON_RANK_MAX_SIZE)) return 0;

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[rank_type].End();
	int count = 0, level_count = 0;
	for (; it_begin != it_end && count < max_count; ++it_begin, ++count)
	{
		level_count += it_begin->role_info.role_level;
	}
	if(0 == count) return 0;

	return ceil(1.0 * level_count / count);
}

int PersonRank::GetAverageByLevelComplement(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return m_average_level_by_level_complement;
	}

	std::map<int, int>::const_iterator it = m_average_level_by_level_map.find(server_id);
	if (it == m_average_level_by_level_map.end())
	{
		return 0;
	}
	
	return it->second;
}

int PersonRank::GetAverageLevel()
{
	int level = 0, count=0;
	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[PERSON_RANK_TYPE_LEVEL].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[PERSON_RANK_TYPE_LEVEL].End();
	for (; it_begin != it_end && count <= 10; it_begin++)
	{
		level += it_begin->role_info.role_level;
		count++;
	}

	if (0 == count)
	{
		return WorldStatus::Instance().GetWorldLevel();
	}

	if ((level / count) <= 0) return 1;
	if ((level / count) > MAX_ROLE_LEVEL) return MAX_ROLE_LEVEL;

	return level / count;
}

void PersonRank::InitUserIndex(int _rank_type, bool _is_fault_tolerant, std::set<int> & _check_set)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::InitUserIndex INVALID RANK TYPE] [rank_type:%d]", (int)_rank_type);
		return;
	}

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	for (; it_begin != it_end; ++it_begin)
	{
		int rank_index = m_rank_skip_list[_rank_type].GetIndex(*it_begin);
		if (-1 == rank_index)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::InitUserIndex ERROR] rank_type[%d] [user[%d %s] prof[%d] "
				"list_size[%d]  list_size_is_full[%d]]",
				_rank_type, it_begin->role_info.uid, it_begin->role_info.user_name,
				it_begin->role_info.prof,
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

		this->AddUserIndex(_rank_type, IntToUid(it_begin->role_info.uid), *it_begin);
	}
}

void PersonRank::SyncRankAllDataToCross(int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossPersonRankInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.rank_type = _rank_type;
	sync_info.is_init = true;
	sync_info.is_init_add = 0;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();

	SkipList<PersonRankItem>::Iterator  it_begin = m_rank_skip_list[_rank_type].Begin();
	SkipList<PersonRankItem>::Iterator  it_end = m_rank_skip_list[_rank_type].End();
	int count = 0;
	for (; it_begin != it_end && count < static_cast<int>(CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT); ++it_begin)
	{
		sync_info.item_list[count++].rank_info = *it_begin;

		if (count >= static_cast<int>(CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT))
		{
			sync_info.count = count;

			int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPersonRankInfo) -
				((CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "PersonRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
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
		int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPersonRankInfo) -
			((CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "PersonRank::SyncRankAllDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

		InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK);
	}

}

void PersonRank::SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, PersonRankItem & _rank_item)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX || CrossConfig::Instance().IsHiddenServer()
		|| _sync_status <= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INVALID ||
		_sync_status >= crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_MAX)
	{
		return;
	}

	static crossgameprotocal::GameCrossSyncCrossPersonRankInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.rank_type = _rank_type;
	sync_info.is_init = false;
	sync_info.last_change_time = (unsigned int)EngineAdapter::Instance().Time();

	sync_info.item_list[0].rank_info = _rank_item;
	sync_info.item_list[0].index = _old_index;
	sync_info.item_list[0].sync_type = _sync_status;


	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossPersonRankInfo) -
		((CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
	gamelog::g_log_debug.printf(LL_INFO, "PersonRank::SyncRankDataToCross rank_type[%d] length[%d]", _rank_type, send_length);
#endif

	InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);

}

void PersonRank::OnBaseInfoChangeByGame(RoleBaseInfo &_role_info, int _old_prof, int _cur_prof)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int rank_type = 0; rank_type < PERSON_RANK_TYPE_MAX; ++rank_type)
	{
		UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(_role_info.uid));
		if (it != m_user_index_map[rank_type].end())
		{
			static PersonRankItem check_item;
			check_item.Reset();
			this->ConstructionCheckItem(check_item, it->second);

			int rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
			if (-1 == rank_index)
			{
				continue;;
			}

			SkipList<PersonRankItem>::Iterator rank_it = m_rank_skip_list[rank_type][rank_index];
			if (m_rank_skip_list[rank_type].End() != rank_it)
			{
				m_change_flag[rank_type] = true;
				if ( ((rank_type == _old_prof / PROFESSION_BASE + PERSON_RANK_TYPE_CAPABILITY) ||
					(rank_type == _old_prof / PROFESSION_BASE + PERSON_RANK_TYPE_LEVEL)) &&
					 _old_prof / PROFESSION_BASE != _cur_prof / PROFESSION_BASE )//玩家转职业
				{
					// 同步删除信息到跨服
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE, rank_index, *rank_it);
					this->RemoveUserIndex(rank_type, IntToUid(_role_info.uid));
					// 数据不一定是插入在最后 需要更新插入位到表尾的数据
					for (int i = rank_index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
					{
						m_dirty_mark[rank_type].insert(i);
					}
					m_rank_skip_list[rank_type].Erase(rank_it);
				}
				else if (((rank_type == _cur_prof / PROFESSION_BASE + PERSON_RANK_TYPE_CAPABILITY) ||
					(rank_type == _cur_prof / PROFESSION_BASE + PERSON_RANK_TYPE_LEVEL)) &&
					 _old_prof / PROFESSION_BASE != _cur_prof / PROFESSION_BASE )//玩家转职业
				{
					// 战斗力榜和等级榜 不做处理  转职后时会重算战斗力和新增等级排行 , 那个地方会新增的
				}
				else
				{
					m_dirty_mark[rank_type].insert(rank_index);
					this->UpdateBaseInfo(rank_it->role_info, _role_info);
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA, rank_index, *rank_it);
				}
			}
			else
			{
				// 这是不应该出现的问题 , 如果出现了需要查逻辑和map相关的调用
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[PersonRank::OnBaseInfoChange Find Rank_info ERROR] [user[%d %s]  old_prof[%d] new_prof[%d] "
					"rank_type[%d]  rank_index[%d]",
					_role_info.uid, _role_info.user_name, _old_prof, _cur_prof,
					rank_type, rank_index);
			}
		}
	}
}

void PersonRank::UserResetNameByGame(int _uid, GameName & _user_name)
{
	for (int rank_type = 0; rank_type < PERSON_RANK_TYPE_MAX; ++rank_type)
	{
		UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(_uid));
		if (it != m_user_index_map[rank_type].end())
		{
			static PersonRankItem check_item;
			check_item.Reset();
			this->ConstructionCheckItem(check_item, it->second);

			int rank_index = m_rank_skip_list[rank_type].GetIndex(check_item);
			if (-1 == rank_index)
			{
				continue;;
			}

			SkipList<PersonRankItem>::Iterator rank_it = m_rank_skip_list[rank_type][rank_index];
			if (m_rank_skip_list[rank_type].End() != rank_it)
			{
				F_STRNCPY(rank_it->role_info.user_name, _user_name, sizeof(GameName));

				// 同步更新数据到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA,
					rank_index, *rank_it);

				m_dirty_mark[rank_type].insert(rank_index);
				m_change_flag[rank_type] = true;
			}
		}
	}
}

void PersonRank::OnBaseInfoChangeByHidden(Role *_role, int _old_prof, int _cur_prof)
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

	static crossgameprotocal::HiddenGameRoleInfoChange change;
	change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_PROF;
	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;
	change.param_1 = _old_prof;
	change.param_2 = _cur_prof;
	this->UpdateBaseInfo(change.rank_item.role_info, _role);

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void PersonRank::OnRankInfoChangeByHidden(Role * _role, PersonRankItem & _rank_item)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGameRoleInfoChange change;
	change.Reset();

	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;
	change.rank_item = _rank_item;

	switch (change.rank_item.rank_type)
	{
	case	PERSON_RANK_TYPE_LEVEL:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL;
		}
		break;
	case REASON_RANK_TYPE_PROF_LEVEL_1:
	case REASON_RANK_TYPE_PROF_LEVEL_2:
	case REASON_RANK_TYPE_PROF_LEVEL_3:
	case REASON_RANK_TYPE_PROF_LEVEL_4:
	case REASON_RANK_TYPE_PROF_LEVEL_5:
	case REASON_RANK_TYPE_PROF_LEVEL_6:
	case REASON_RANK_TYPE_PROF_LEVEL_7:
	case REASON_RANK_TYPE_PROF_LEVEL_8:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL;
		}
		break;
	case PERSON_RANK_TYPE_CAPABILITY:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CAPABILITY;
		}
		break;
	case PERSON_RANK_TYPE_PROF_CAPABILITY_1:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_2:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_3:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_4:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_5:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_6:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_7:
	case PERSON_RANK_TYPE_PROF_CAPABILITY_8:
	case PERSON_RANK_TYPE_ROLE_AND_MAX_PET_CAP:
	case PERSON_RANK_TYPE_ROLE_CAPABILITY:
	case PERSON_RANK_TYPE_FOUR_MAX_PARTNER:
	case PERSON_RANK_TYPE_EQUIPMENT_CAP:
	case PERSON_RANK_TYPE_PET_MAX_CAPABILITY:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CAPABILITY;
		}
		break;
	case 	PERSON_RANK_TYPE_HUASHEN_CAPABILITY:				// 化身角色战力榜
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN;
		}
		break;
	case PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_DREAM_NOTES;
		}
		break;
	case PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LUNHUIWANGCHUAN;
		}
		break;
	case PERSON_RANK_TYPE_CHIVALROUS:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CHIVALROUS;
		}
		break;
	case PERSON_RANK_TYPE_NEHER_WORLD_PURGATORY:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NETHER_REWARD;
		}
		break;
	case PERSON_RANK_TYPE_BIG_DIPPER_1:
	case PERSON_RANK_TYPE_BIG_DIPPER_2:
	case PERSON_RANK_TYPE_BIG_DIPPER_3:
	case PERSON_RANK_TYPE_BIG_DIPPER_4:
	case PERSON_RANK_TYPE_BIG_DIPPER_5:
	case PERSON_RANK_TYPE_BIG_DIPPER_6:
	case PERSON_RANK_TYPE_BIG_DIPPER_7:
	case PERSON_RANK_TYPE_BIG_DIPPER_8:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_KILL_BOSS;
		}
		break;
	case PERSON_RANK_TYPE_ENDLESS_TOWER:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_ENDLESS_TOWER;
		}
		break;
	case PERSON_RANK_TYPE_FALLEN_GOD:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_FALLEN_GOD;
		}
		break;
	case PERSON_RANK_TYPE_MEILI_QIXI:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_MEILI_QIXI;
		}
	break;
	case PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE:
		{
			change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NEW_COURAGE_CHALLENGE;
		}
		break;
	default:
		break;
	}

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void PersonRank::UserResetNameByHidden(Role * _role)
{
	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(_role->GetUserId());
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGameRoleInfoChange change;
	change.sync_info_type = crossgameprotocal::HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_NAME;
	change.origin_plat_type = cui->original_plat_type;
	change.origin_server_id = cui->original_server_id;
	this->UpdateBaseInfo(change.rank_item.role_info, _role);

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

bool PersonRank::IsValidIndex(int rank_type, int index)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX || index < 0)
	{
		return false;
	}

	SkipList<PersonRankItem>::Iterator it = m_rank_skip_list[rank_type][index];
	if (m_rank_skip_list[rank_type].End() == it)
	{
		return false;
	}

	return true;
}

void PersonRank::UpdateRankList(PersonRankItem &rank_item)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (rank_item.rank_type < 0 || rank_item.rank_type >= PERSON_RANK_TYPE_MAX)
		return;

	// 存在则更新 , 不存在则插入
	int rank_type = rank_item.rank_type;
	int uid = rank_item.role_info.uid;
	bool need_insert = true;
	bool is_error_insert = false;  // 是否是容错插入 , 用于日志查BUG
	bool is_del_index_insert = false;  // 是否是删除索引后添加 , 用于日志查BUG

	static PersonRankItem check_item;
	int index = -1;
	UserIndexMapIt it = m_user_index_map[rank_type].find(IntToUid(uid));
	if (it != m_user_index_map[rank_type].end())
	{
		need_insert = false;
		check_item.Reset();
		this->ConstructionCheckItem(check_item, it->second);

		index = m_rank_skip_list[rank_type].GetIndex(check_item);
		if (-1 == index)
		{
			// 因为UPDATE的时候被人把数据挤出了排行榜  , 但是索引里依然存在 
			// 所以如果根据索引无法找到对应的数据时 ,  
			// 所以则清除对应的索引  然后再进行新数据添加
			// 后续由新数据添加来做判断
			need_insert = true;
			is_del_index_insert = true;
			this->RemoveUserIndex(rank_type, IntToUid(uid));
		}
	}

	bool is_change = false;
	// UPDATE 相关逻辑
	if (!need_insert)
	{
		// 当前角色在排行榜上有数据那么需要刷新数据
		SkipList<PersonRankItem>::Iterator skip_it = m_rank_skip_list[rank_type][index];
		if (m_rank_skip_list[rank_type].End() != skip_it)
		{
			// 如果相等则不需要更新 , 玩家上线的时候会重算战斗力 , 此时和排行榜内数据理论上无变化
			if (*skip_it == rank_item)
			{
				return;
			}
			is_change = true;

			// 注意 updata后  原迭代器可能失效 ,如需使用需要重新获取新的迭代器
			m_rank_skip_list[rank_type].UpdateData(*skip_it, rank_item);
			int new_index = m_rank_skip_list[rank_type].GetIndex(rank_item);
			if (-1 != new_index)
			{
				this->ConstructionIndexData(it->second, rank_item);

				m_change_flag[rank_type] = true;

				if (new_index == index)
				{
					// 数据更新了 顺序没变更
					m_dirty_mark[rank_type].insert(new_index);
				}
				else
				{
					// 数据更新了 顺序变更了 , 那么2个index之间的数据顺位都发生了变更
					int start_index = new_index < index ? new_index : index;
					int start_end = new_index > index ? new_index : index;
					for (int i = start_index; i <= start_end; ++i)
					{
						m_dirty_mark[rank_type].insert(i);
					}


					// 名次变更时记录一下日志
					gamelog::g_log_rank.printf(LL_INFO, "PersonRank::UpdateRankList Updata  check_user[%d %s]  new_user[%d %s] rank_type:%d  "
						"old_index:%d   new_index:%d  check_rank_val[%lld]  new_rank_val[%lld] ",
						check_item.role_info.uid, check_item.role_info.user_name, rank_item.role_info.uid, rank_item.role_info.user_name, rank_type,
						index, new_index, check_item.rank_value, rank_item.rank_value);
				}

				// 同步更新信息到跨服
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_UPDATA,
					index, rank_item);
			}
			else
			{
				// 记录错误日志 
				gamelog::g_log_rank.printf(LL_INFO, "PersonRank::UpdateRankList[ERROR_2]  check_user[%d %s]  new_user[%d %s] rank_type:%d  "
					"old_index:%d   check_rank_val[%lld]  new_rank_val[%lld] ",
					check_item.role_info.uid, check_item.role_info.user_name, rank_item.role_info.uid, rank_item.role_info.user_name, rank_type,
					index, check_item.rank_value, rank_item.rank_value);


				// 容错
				need_insert = true;
				is_error_insert = true;
				this->RemoveUserIndex(rank_type, IntToUid(uid));
			}
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_rank.printf(LL_INFO, "PersonRank::UpdateRankList[ERROR_3]  check_user[%d %s]  new_user[%d %s] rank_type:%d  "
				"list_size[%d]  old_index[%d]  check_rank_val[%lld]  new_rank_val[%lld]",
				check_item.role_info.uid, check_item.role_info.user_name, rank_item.role_info.uid, rank_item.role_info.user_name, rank_type,
				m_rank_skip_list[rank_type].Size(), index, check_item.rank_value, rank_item.rank_value);

			// 容错
			need_insert = true;
			is_error_insert = true;
			this->RemoveUserIndex(rank_type, IntToUid(uid));
		}
	}

	// INSERT 相关逻辑
	if (need_insert)
	{
		if (m_rank_skip_list[rank_type].IsFull())
		{
			// 如果表已经满了, 
			SkipList<PersonRankItem>::Reverse_Iterator r_iter = m_rank_skip_list[rank_type].RBegin();
			if (m_rank_skip_list[rank_type].REnd() != r_iter)
			{
				// 新入数据大于末尾数据
				if (rank_item > *r_iter)
				{
					this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_DELETE,
						m_rank_skip_list[rank_type].Size() - 1, *r_iter);
					
					is_change = true;
					m_rank_skip_list[rank_type].Erase(*r_iter);
					m_rank_skip_list[rank_type].Insert(rank_item);
					int index = m_rank_skip_list[rank_type].GetIndex(rank_item);
					if (-1 != index)
					{
						// 同步插入信息到跨服
						this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT,
							-1, rank_item);

						this->AddUserIndex(rank_type, IntToUid(rank_item.role_info.uid), rank_item);
						m_change_flag[rank_type] = true;

						// 数据不一定是插入在最后 需要更新插入位到表尾的数据
						for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
						{
							m_dirty_mark[rank_type].insert(i);
						}

						// 插入日志
						gamelog::g_log_rank.printf(LL_INFO, "[PersonRank::UpdateRankList[Insert_Succ_1] is_del_index_insert:%d  [new_user[%d %s]  rank_type[%d]  rank_index[%d]  list_size[%d] "
							"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
							is_del_index_insert ,rank_item.role_info.uid, rank_item.role_info.user_name, rank_type, index, m_rank_skip_list[rank_type].Size(),
							rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
					}
					else
					{
						// 错误日志 , 如果出现了需要查逻辑
						gamelog::g_log_rank.printf(LL_INFO, "[PersonRank::UpdateRankList[ERROR_4]  [new_user[%d %s]  rank_type[%d]  rank_index[%d]  list_size[%d] "
							"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
							rank_item.role_info.uid, rank_item.role_info.user_name, rank_type, index, m_rank_skip_list[rank_type].Size(),
							rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
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
				this->SyncRankDataToCross(rank_type, crossgameprotocal::SYNC_RANK_TO_CROSS_STATUS_INSERT,
					-1, rank_item);
				
				is_change = true;
				this->AddUserIndex(rank_type, IntToUid(rank_item.role_info.uid), rank_item);
				m_change_flag[rank_type] = true;
				// 数据不一定是插入在最后 需要更新插入位到表尾的数据
				for (int i = index; i < (int)m_rank_skip_list[rank_type].Size(); ++i)
				{
					m_dirty_mark[rank_type].insert(i);
				}

				// 插入日志
				gamelog::g_log_rank.printf(LL_INFO, "[PersonRank::UpdateRankList[Insert_Succ_2]  is_del_index_insert:%d   [new_user[%d %s]  is_error_insert[%d]  rank_type[%d]  rank_index[%d]  list_size[%d] "
					"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
					is_del_index_insert, rank_item.role_info.uid, rank_item.role_info.user_name, is_error_insert, rank_type, index, m_rank_skip_list[rank_type].Size(),
					rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
			else
			{
				// 错误日志 , 如果出现了需要查逻辑
				gamelog::g_log_rank.printf(LL_INFO, "[PersonRank::UpdateRankList[ERROR_5]  [new_user[%d %s]  is_error_insert[%d]  rank_type[%d]  rank_index[%d]  list_size[%d] "
					"rank_value[%lld] flexible_int[%d] flexible_ll[%lld]",
					rank_item.role_info.uid, rank_item.role_info.user_name, is_error_insert, rank_type, index, m_rank_skip_list[rank_type].Size(),
					rank_item.rank_value, rank_item.flexible_int, rank_item.flexible_ll);
			}
		}
	}
	
	if (is_change)
	{
		this->OnRankChange(rank_type);
	}
}

void PersonRank::UpdateBaseInfo(RoleBaseInfo &_role_info, Role * _role)
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
		if (index > 0 && index < (int)sizeof(GameName))
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

void PersonRank::UpdateBaseInfo(RoleBaseInfo &_dest_role_info, const RoleBaseInfo & _src_role_info)
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

bool PersonRank::AddUserIndex(int rank_type, const UserID &user_id, PersonRankItem & _check_item)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return false;

	UserIndexMapIt it = m_user_index_map[rank_type].find(user_id);
	if (it == m_user_index_map[rank_type].end())
	{

		static IndexData index_data;
		index_data.Reset();
		this->ConstructionIndexData(index_data, _check_item);

		m_user_index_map[rank_type].insert(std::make_pair(user_id, index_data));
	}

	return true;
}

bool PersonRank::RemoveUserIndex(int rank_type, const UserID &user_id)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return false;

	m_user_index_map[rank_type].erase(user_id);

	return true;
}

int PersonRank::GetRankTypeSize(int rank_type)
{
	if (rank_type < 0 || rank_type >= PERSON_RANK_TYPE_MAX) return false;

	int rank_type_size = m_rank_skip_list[rank_type].Size();

	return rank_type_size;
}

void PersonRank::OnRankLoadFinish()
{
	if (!this->IsRankLoadFinish()) return;
}


void PersonRank::CalcRankLoadFinish()
{
	if (m_is_all_load_finish)
	{
		return;
	}

	for (int rank_type = 0; rank_type < PERSON_RANK_TYPE_MAX; ++rank_type)
	{
		if (!m_load_flag[rank_type])
		{
			return;
		}
	}

	m_is_all_load_finish = true;
}

void PersonRank::ConstructionCheckItem(PersonRankItem & _check_item, const IndexData & _index_data)
{
	_check_item.role_info.uid = _index_data.uid;
	_check_item.rank_type = _index_data.rank_type;
	_check_item.rank_value = _index_data.rank_value;
	_check_item.flexible_int = _index_data.flexible_int;
	_check_item.flexible_ll = _index_data.flexible_ll;
	_check_item.role_info.top_level = _index_data.top_level;
}

void PersonRank::ConstructionIndexData(IndexData & _index_data, const PersonRankItem & _check_item)
{
	_index_data.uid = _check_item.role_info.uid;
	_index_data.rank_type = _check_item.rank_type;
	_index_data.rank_value = _check_item.rank_value;
	_index_data.flexible_int = _check_item.flexible_int;
	_index_data.flexible_ll = _check_item.flexible_ll;
	_index_data.top_level = _check_item.role_info.top_level;
}

void PersonRank::OnRankChange(int rank_type)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	
	if (PERSON_RANK_TYPE_LEVEL == rank_type)
	{
		int old_average_level_by_level_complement = m_average_level_by_level_complement;
		this->OnSetAverageLevelByLevelComplement();

		if (old_average_level_by_level_complement != m_average_level_by_level_complement)
		{
			this->OnGameHiddenSynLevelComplementInfo();

			Protocol::SCNewLevelComplementSpeicalInfo info;
			info.my_server_average_level = m_average_level_by_level_complement;

			World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
		}
	}
	
}

void PersonRank::OnSetAverageLevelByLevelComplement()
{
	const RoleLevelComplementOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleLevelComplementConfig()->GetOtherCfg();
	m_average_level_by_level_complement = this->GetAverageByType(PERSON_RANK_TYPE_LEVEL, other_cfg.ranking);
}

void PersonRank::OnGameHiddenSynLevelComplementInfo()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSynLevelComplementInfo ghslci;
	ghslci.server_id = LocalConfig::Instance().GetMergeServerId();
	ghslci.average_level_by_level_complement = m_average_level_by_level_complement;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghslci, sizeof(ghslci));
}

bool PersonRank::IsUserOnRank(UserID user_id, int rank_type)
{
	if (rank_type < 0)
	{
		for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
		{
			UserIndexMapIt it = m_user_index_map[i].find(user_id);
			if (it != m_user_index_map[i].end())
			{
				return true;
			}
		}
	}
	else if (rank_type >= 0 && rank_type < PERSON_RANK_TYPE_MAX)
	{
		UserIndexMapIt it = m_user_index_map[rank_type].find(user_id);
		if (it != m_user_index_map[rank_type].end())
		{
			return true;
		}
	}

	return false;
}


