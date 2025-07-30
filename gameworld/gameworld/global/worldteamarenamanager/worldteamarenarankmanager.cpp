#include "worldteamarenarankmanager.hpp"
#include "gamelog.h"
#include "internalcomm.h"

#include "servercommon/serverconfig/crossconfig.hpp"

#include "obj/character/role.h"

#include "global/rmibackobjdef.h"
#include "global/cross/crossmanager.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/title/title.hpp"

WorldTeamArenaRankManager & WorldTeamArenaRankManager::Instance()
{
	static WorldTeamArenaRankManager world_team_arena_manager;
	return world_team_arena_manager;
}

void WorldTeamArenaRankManager::Update(unsigned int now_second)
{
	if (now_second >= m_next_save_db_timestamp && this->IsDirty())
	{
		this->Save();
		this->UpdataRankList();		
		m_next_save_db_timestamp = now_second + ACTIVITY_WORLD_TEAM_ARENA_SAVE_DB_TIME;
	}
}

void WorldTeamArenaRankManager::OnServerStart()
{
	this->LoadWorldTeamArenaTeamRankData(0);
}

void WorldTeamArenaRankManager::OnServerStop()
{
	this->Save();
}

bool WorldTeamArenaRankManager::LoadWorldTeamArenaTeamRankData(long long id_from)
{
	RMIInitWorldTeamArenaTeamRankDataBackObjectImpl * impl = new RMIInitWorldTeamArenaTeamRankDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitWorldTeamArenaTeamRankDataAsyn(id_from, impl);
}

void RMIInitWorldTeamArenaTeamRankDataBackObjectImpl::InitWorldTeamArenaTeamRankDataRet(int ret, const WorldTeamArenaTeamRankParam & list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		WorldTeamArenaRankManager & world_team_arena_rank_manager = WorldTeamArenaRankManager::Instance();

		world_team_arena_rank_manager.Init(list_param);
		world_team_arena_rank_manager.LoadWorldTeamArenaTeamRankSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}

void WorldTeamArenaRankManager::Init(const WorldTeamArenaTeamRankParam & param)
{
	for (int i = 0; i < param.count && i < ARRAY_LENGTH(param.team_rank_list); ++i)
	{
		const WorldTeamArenaTeamRankData & rank_data = param.team_rank_list[i];
		if (!rank_data.IsVaild())
		{
			gamelog::g_log_world_team_arena.printf(LL_CRITICAL, "[%s] team[%lld, %s]", __FUNCTION__, rank_data.team_unique_id,
				rank_data.team_name);
			continue;
		}
		m_rank_param[rank_data.team_unique_id] = rank_data;
		m_dirty_mark[rank_data.team_unique_id] = false;
		m_old_state[rank_data.team_unique_id] = true;
	}
	m_load_flag = true;
}

void WorldTeamArenaRankManager::GetInitParam(WorldTeamArenaTeamRankParam * param)
{
	if (!m_load_flag || !this->IsDirty())
	{
		return;
	}

	param->count = 0;
	for (std::map<long long, WorldTeamArenaTeamRankData>::iterator it = m_rank_param.begin(); it != m_rank_param.end() &&
		param->count < ARRAY_LENGTH(param->team_rank_list); it++)
	{
		if (m_dirty_mark[it->first])
		{
			if (!it->second.IsVaild())
			{
				if (m_old_state[it->first]) continue;

				param->team_rank_list[param->count].change_state = structcommon::CS_DELETE;
				param->team_rank_list[param->count].team_unique_id = it->first;
				param->count++;
			}
			else
			{
				param->team_rank_list[param->count] = it->second;
				param->team_rank_list[param->count].change_state = m_old_state[it->first] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->count++;
			}
		}
	}
}

void WorldTeamArenaRankManager::ClearDirtyMark(WorldTeamArenaTeamRankParam * param)
{
	if (!m_change_flag) return;

	for (int i = 0; i < param->count && i < ARRAY_LENGTH(param->team_rank_list); i++)
	{
		WorldTeamArenaTeamRankData & data = param->team_rank_list[i];
		if (structcommon::CS_DELETE != data.change_state)
		{
			m_old_state[data.team_unique_id] = true;
			m_dirty_mark[data.team_unique_id] = false;
		}
		else
		{
			m_dirty_mark.erase(data.team_unique_id);
			m_old_state.erase(data.team_unique_id);
			m_rank_param.erase(data.team_unique_id);
		}
		data.change_state = structcommon::CS_NONE;
	}

	m_change_flag = false;
}

void WorldTeamArenaRankManager::OnRoleInfoChange(Role * role)
{
	if(NULL == role || !CrossConfig::Instance().IsHiddenServer()) return;

	for (std::map<long long, WorldTeamArenaTeamRankData>::iterator it = m_rank_param.begin(); it != m_rank_param.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			if (role->GetUID() == it->second.member_list[i].role_uid)
			{
				WorldTeamArenaSignUpManager::Instance().FillMemberInfo(role, it->second.member_list[i]);
				m_dirty_mark[it->first] = true;
				m_old_state[it->first] = m_old_state[it->first] ? true : false;
				m_change_flag = true;
				return;
			}
		}
	}
}

void WorldTeamArenaRankManager::OnQueryTeamDetailedInfo(Role * role, long long team_unique_id)
{
	if(NULL == role) return;

	std::map<long long, WorldTeamArenaTeamRankData>::const_iterator it = m_rank_param.find(team_unique_id);
	if (it == m_rank_param.end() || !it->second.IsVaild())
	{
		return;
	}
	
	crossgameprotocal::GameCrossWorldTeamArenaQueryRankReq gcwtaqrr;
	gcwtaqrr.server_id = LocalConfig::Instance().GetMergeServerId();
	gcwtaqrr.plat_type = LocalConfig::Instance().GetPlatType();
	gcwtaqrr.team_unqiue_id = it->second.team_unique_id;
	gcwtaqrr.send_uid = role->GetUID();
	gcwtaqrr.notice_type = Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_RANK_END;
	gcwtaqrr.send_type = 0;

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(it->second.member_list) == ARRAY_ITEM_COUNT(gcwtaqrr.rank_user_info));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(gcwtaqrr.rank_user_info[0].rank_type_list) == ARRAY_ITEM_COUNT(ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST));
	for (int i = 0; i < ARRAY_LENGTH(gcwtaqrr.rank_user_info); i++)
	{
		gcwtaqrr.rank_user_info[i].role_id = it->second.member_list[i].role_uid;
		gcwtaqrr.rank_user_info[i].prof = it->second.member_list[i].profession;
		memcpy(gcwtaqrr.rank_user_info[i].rank_type_list, ACTIVITY_WORLD_TEAM_ARENA_RANK_TYPE_LIST, sizeof(gcwtaqrr.rank_user_info[i].rank_type_list));
	}

	InternalComm::Instance().SendToCross((const char *)&gcwtaqrr, sizeof(gcwtaqrr));

}

void WorldTeamArenaRankManager::OnGameRecvCrossQueryRankRet(crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg)
{
	if (NULL == msg) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->send_uid);
	if (NULL == role) return;

	std::map<long long, WorldTeamArenaTeamRankData>::const_iterator it = m_rank_param.find(msg->team_unique_id);
	if (it == m_rank_param.end() || !it->second.IsVaild())
	{
		return;
	}

	Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo info;
	info.notice_type = msg->notice_type;
	info.team_detailed_info.team_unique_id = it->second.team_unique_id;
	memcpy(info.team_detailed_info.team_name, it->second.team_name, sizeof(info.team_detailed_info.team_name));

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(it->second.member_list) == ARRAY_ITEM_COUNT(info.team_detailed_info.member_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->rank_user_info) == ARRAY_ITEM_COUNT(it->second.member_list));
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(msg->rank_user_info[0].rank_type_list) == ARRAY_ITEM_COUNT(info.team_detailed_info.member_list[0].rank_list));
	for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); i++)
	{
		info.team_detailed_info.member_list[i].SetData(it->second.member_list[i]);
		memcpy(info.team_detailed_info.member_list[i].rank_list, msg->rank_user_info[i].rank_type_list, sizeof(info.team_detailed_info.member_list[i].rank_list));
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void WorldTeamArenaRankManager::OnHiddenRecvGameSynRoleInfo(crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg)
{
	if(NULL == msg || !CrossConfig::Instance().IsHiddenServer() || msg->member_info.role_uid <= 0) return;

	for (std::map<long long, WorldTeamArenaTeamRankData>::iterator it = m_rank_param.begin(); it != m_rank_param.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			if (msg->member_info.role_uid == it->second.member_list[i].role_uid)
			{
				it->second.member_list[i] = msg->member_info;
				m_dirty_mark[it->first] = true;
				m_old_state[it->first] = m_old_state[it->first] ? true : false;
				m_change_flag = true;
				return;
			}
		}
	}
}

void WorldTeamArenaRankManager::OnActivityStandy()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	//活动准备则清除上一届的排名(包括此时别的跨服合过来的)
	for (std::map<long long, WorldTeamArenaTeamRankData>::iterator it = m_rank_param.begin(); it != m_rank_param.end(); ++it)
	{
		//回收玩家称号
		for (int i = 0; i < ARRAY_LENGTH(it->second.member_list); ++i)
		{
			this->OnRoleTitleInfoChange(it->second.member_list[i].role_uid, __FUNCTION__, -1, -1);
		}

		it->second.Reset();
		m_dirty_mark[it->first] = true;
		m_old_state[it->first] = false;
		m_change_flag = true;
	}
	if (this->IsDirty())
	{
		m_next_save_db_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
}

void WorldTeamArenaRankManager::OnActivityEnd(std::vector<WorldTeamArenaRankItem> rank_list)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	int hidden_idx = CrossConfig::Instance().GetHiddenServerIDX();
	for (int i = 0; i < (int)rank_list.size(); i++)
	{
		//查找不到报名信息则剔除该战队
		const WorldTeamArenaTeamSignUpData * sign_up_data = WorldTeamArenaSignUpManager::Instance().GetWorldTeamArenaSignUpDataById(rank_list[i].team_unique_id);
		if (NULL == sign_up_data)	
		{
			continue;
		}
		WorldTeamArenaTeamRankData team_data;
		team_data = rank_list[i];
		team_data.hidden_server_idx = hidden_idx;

		for (int i = 0; i < ARRAY_LENGTH(team_data.member_list); i++)
		{
			team_data.member_list[i] = sign_up_data->member_list[i];
		}
		m_rank_param[team_data.team_unique_id] = team_data;
		m_dirty_mark[team_data.team_unique_id] = true;
		m_old_state[team_data.team_unique_id] = false;
		m_change_flag = true;
	}
	if (this->IsDirty())
	{
		m_next_save_db_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
	//信息录入后清空报名列表
	WorldTeamArenaSignUpManager::Instance().OnActivityEnd();
}

void WorldTeamArenaRankManager::OnRoleTitleInfoChange(int role_id, const char * reason, int team_rank, int title_id)
{
	if(NULL == reason) return;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (NULL == role)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameWorldTeamArenaSynRoleTitleInfo hgwtasrti;
			hgwtasrti.role_id = role_id;
			F_STRNCPY(hgwtasrti.reason, reason, sizeof(hgwtasrti.reason));
			hgwtasrti.info.world_team_arena_rank = team_rank;
			hgwtasrti.info.title_id = title_id;
			hgwtasrti.info.timestamp = now_second;

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char*)&hgwtasrti, sizeof(hgwtasrti));
		}
		else
		{
			OfflineRegisterManager::Instance().OnSetWorldTeamArenaInfo(role_id, team_rank, now_second, ((title_id > 0) ? &title_id : NULL));
		}
	}
	else
	{
		role->GetRoleModuleManager()->OnSetWorldTeamArenaInfo(team_rank, now_second, &title_id);
		role->GetRoleModuleManager()->OnSetWorldTeamArenaTimestamp(now_second);
		if (title_id <= 0 || team_rank <= 0)
		{
			role->GetRoleModuleManager()->GetTitle()->CalWorldTeamArenaTitle();
		}
		else
		{
			role->GetRoleModuleManager()->GetTitle()->AddTitle(title_id, reason);
		}
	}
}

void WorldTeamArenaRankManager::OnSendAllRankInfo(Role * role)
{
	if(NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCActivityWorldTeamArenaFinallyRankInfo all_rank_info;
		this->FillAllFinallyRankInfo(all_rank_info, CrossConfig::Instance().GetHiddenServerIDX(), role->GetUID());

		int length = sizeof(all_rank_info) - (ARRAY_LENGTH(all_rank_info.rank_list) - all_rank_info.rank_count) * sizeof(all_rank_info.rank_list[0]);
		EngineAdapter::Instance().NetSend(role->GetNetId(), &all_rank_info, length);
	}
	else
	{
		crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq ghwtafrr;
		ghwtafrr.role_id = role->GetUID();
		ghwtafrr.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghwtafrr, sizeof(ghwtafrr));
	}

}

void WorldTeamArenaRankManager::OnHiddenRecvGameFinallyRankReq(crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq * msg)
{
	if(NULL == msg || !CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCActivityWorldTeamArenaFinallyRankInfo all_rank_info;
	this->FillAllFinallyRankInfo(all_rank_info, msg->hidden_server_idx, msg->role_id);

	int length = sizeof(all_rank_info) - (ARRAY_LENGTH(all_rank_info.rank_list) - all_rank_info.rank_count) * sizeof(all_rank_info.rank_list[0]);
	InternalComm::Instance().SendToGameUserThroughCross(msg->role_id, &all_rank_info, length);
}

void WorldTeamArenaRankManager::GetTopRankTeam(std::map<int, WorldTeamArenaServerTopTeamInfo> & top_team_list)
{
	std::map<int, std::vector<WorldTeamArenaTeamRankData> >::const_iterator it = m_rank_list.begin();
	for (; it != m_rank_list.end(); ++it)
	{
		WorldTeamArenaServerTopTeamInfo team_info;
		for (int i = 0; i < (int)it->second.size() && i < ARRAY_LENGTH(team_info.team_list); ++i)
		{
			team_info.team_list[i].team_unique_id = it->second[i].team_unique_id;
			F_STRNCPY(team_info.team_list[i].team_name, it->second[i].team_name, sizeof(team_info.team_list[i].team_name));
			memcpy(team_info.team_list[i].team_member_list, it->second[i].member_list, sizeof(team_info.team_list[i].team_member_list));
			UNSTD_STATIC_CHECK(sizeof(team_info.team_list[i].team_member_list) == sizeof(it->second[i].member_list));
		}

		top_team_list[it->first] = team_info;
	}
}

void WorldTeamArenaRankManager::CmdRankToSignUpList(int cur_season, unsigned int sign_up_end_timestamp)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	ActivityWorldTeamArena* activity = (ActivityWorldTeamArena*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity)
	{
		return;
	}
	activity->CmdToRankToSignUp(cur_season, sign_up_end_timestamp);
	WorldTeamArenaSignUpManager::Instance().OnCmdRankToSignUpList(m_rank_list);
	this->OnActivityStandy();
}

WorldTeamArenaRankManager::WorldTeamArenaRankManager() : m_change_flag(false), m_load_flag(false), m_next_save_db_timestamp(0)
{
	m_rank_param.clear();
	m_dirty_mark.clear();
	m_old_state.clear();
}

WorldTeamArenaRankManager::~WorldTeamArenaRankManager()
{
}

bool SortRank(const WorldTeamArenaTeamRankData & a, const WorldTeamArenaTeamRankData & b)
{
	if (a.total_score != b.total_score) return a.total_score > b.total_score;

	int our_win_rate = a.fight_times <= 0 ? 0 : a.victory_times * 1000 / a.fight_times;
	int their_win_rate = b.fight_times <= 0 ? 0 : b.victory_times * 1000 / b.fight_times;
	if (our_win_rate != their_win_rate) return our_win_rate > their_win_rate;

	if (a.score_change_time != b.score_change_time) return a.score_change_time < b.score_change_time;
	return false;
}

void WorldTeamArenaRankManager::FillAllFinallyRankInfo(Protocol::SCActivityWorldTeamArenaFinallyRankInfo & all_rank_info, int hidden_server_idx, int rold_id)
{
	all_rank_info.rank_count = 0;
	all_rank_info.my_rank_posi = 0;

	std::map<int, std::vector<WorldTeamArenaTeamRankData> >::const_iterator it = m_rank_list.find(hidden_server_idx);
	if (it == m_rank_list.end())
	{
		return;
	}
	for (int i = 0; i < (int)it->second.size(); ++i)
	{
		for (int k = 0; k < ARRAY_LENGTH(it->second[i].member_list); ++k)
		{
			if (rold_id == it->second[i].member_list[k].role_uid)
			{
				all_rank_info.my_rank_info.SetData(it->second[i]);
				break;
			}
		}

		if (all_rank_info.rank_count < ARRAY_LENGTH(all_rank_info.rank_list))
		{
			all_rank_info.rank_list[all_rank_info.rank_count++].SetData(it->second[i]);
			if (it->second[i].team_unique_id == all_rank_info.my_rank_info.team_unique_id)
			{
				all_rank_info.my_rank_posi = all_rank_info.rank_count;
			}
		}
	}
}

void WorldTeamArenaRankManager::UpdataRankList()
{	
	m_rank_list.clear();
	
	for (std::map<long long, WorldTeamArenaTeamRankData>::const_iterator it = m_rank_param.begin(); it != m_rank_param.end(); ++it)
	{
		if (!it->second.IsVaild())
		{
			continue;
		}
		
		m_rank_list[it->second.hidden_server_idx].push_back(it->second);
	}

	for (int i = 0; i < (int)m_rank_list.size(); i++)
	{
		std::sort(m_rank_list[i].begin(), m_rank_list[i].end(), SortRank);
	}
}

void WorldTeamArenaRankManager::Save(const WorldTeamArenaTeamRankParam * param)
{
	if (!this->IsLoadFinish()) return;

	static WorldTeamArenaTeamRankParam world_team_arena_team_rank_param;
	if (param != NULL)
	{
		world_team_arena_team_rank_param = *param;
	}
	this->GetInitParam(&world_team_arena_team_rank_param);

	RMISaveWorldTeamArenaTeamRankDataBackObjectImpl *backobj = new RMISaveWorldTeamArenaTeamRankDataBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveWorldTeamArenaTeamRankDataAsyn(world_team_arena_team_rank_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&world_team_arena_team_rank_param);
	}
}

void WorldTeamArenaRankManager::LoadWorldTeamArenaTeamRankSucc()
{
	if (this->IsLoadFinish())
	{
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + ACTIVITY_WORLD_TEAM_ARENA_SAVE_DB_TIME);
		this->UpdataRankList();	
	}
}
