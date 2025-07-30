#include "teammatch.hpp"
#include "team.hpp"
#include "engineadapter.h"
#include <vector>
#include "teammanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/errornum.h"
#include "other/robot/robotmanager.hpp"
#include "obj/character/robot.h"
#include "config/logicconfigmanager.hpp"
#include "other/robot/robotconfig.hpp"
#include "global/team/teamconfig.hpp"
#include "gamelog.h"
#include "global/usercache/usercache.hpp"
#include "other/route/offlinerobotroute.hpp"

TeamMatch& TeamMatch::Instance()
{
	static TeamMatch instance;
	return instance;
}

TeamMatch::TeamMatch() : m_next_match_time(0u)
{

}

TeamMatch::~TeamMatch()
{
	this->Release();
}

void TeamMatch::Update(long long intervals, unsigned int now)
{
	if (now >= m_next_match_time)
	{
		this->CheckExpireMatchUser();
		this->DoSuspendEndRestartMatch();
		this->DoMatch();
		this->DoAutoAddRobot();
		m_next_match_time = now + MATCH_INTERVAL_S;
	}
}

void TeamMatch::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		MatchUserInfo* mui = this->GetMatchUserInfo(role->GetUniqueUserID());
		if (NULL != mui)
		{
			this->RemoveMatchUserInfo(&mui);
		}

		this->OnUserStopCrossMatch(role->GetUniqueUserID());
	}
	else
	{
		UniqueUserID uuid = role->GetUniqueUserID();
		MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
		if (NULL == mui) return;

		if (mui->IsSuspend() && mui->enter_team_index > 0)
		{
			Team* team = TeamManager::Instance().GetTeamByTeamIndex(mui->enter_team_index);
			if (NULL == team)
			{
				role->NoticeNum(errornum::EN_MATCH_CROSS_TEAM_NOT_EXIST);
				role->ReturnToOriginServer();
				this->RemoveMatchUserInfo(&mui);
			}
		}
	}	
}

void TeamMatch::OnUserLogout(Role* role)
{
	UniqueUserID uuid = role->GetUniqueUserID();
	this->OnUserStopMatch(uuid);
}

bool TeamMatch::OnTeamStartMatch(Team* team)
{
	if (NULL == team) return false;

	MatchTeamInfo* mti = this->GetMatchTeamInfo(team->GetTeamIndex());
	if (NULL != mti) return false;

	mti = new MatchTeamInfo();
	mti->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	mti->team = team;
	mti->team_index = team->GetTeamIndex();
	mti->match_status = MATCH_STATUS_MATCHING;
	Role * leader_role = team->GetLeaderRole();
	if (NULL != leader_role)
	{
		mti->creator_guild_id = leader_role->GetGuildID();
	}
	m_matching_team_map.insert(MatchTeamInfoMap::value_type(team->GetTeamIndex(), mti));

	Role* role = team->GetLeaderRole();
	if (NULL != role)
	{
		this->SendMatchInfo(role, 1);

#ifdef _DEBUG
		gamelog::g_log_world.printf(LL_INFO, "OnTeamStartMatch leader[%d,%s] lv[%d] team_type[%d]",
			role->GetUID(), role->GetName(), role->GetLevel(), team->GetTeamType());
#endif

	}

	team->OnTeamStartMatch();

	return true;
}

bool TeamMatch::OnTeamStopMatch(Team* team)
{
	if (NULL == team) return false;

	MatchTeamInfo* mti = this->GetMatchTeamInfo(team->GetTeamIndex());
	if (NULL == mti) return false;

	this->RemoveMatchTeamInfo(&mti);

	Role* role = team->GetLeaderRole();
	if (NULL != role)
	{
		this->SendMatchInfo(role, 0);
	}

	team->OnTeamStopMatch();

	return true;
}

bool TeamMatch::OnUserStartMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create)
{
	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL != mui) return false; 

	mui = new MatchUserInfo();
	mui->uuid = uuid;
	mui->match_status = MATCH_STATUS_MATCHING;
	mui->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	mui->user_level = user_level;
	mui->match_team_type = match_team_type;
	mui->is_cross_matching = false;
	mui->user_guild_id = user_guild_id;
	mui->is_auto_create = is_auto_create;

	m_matching_user_map.insert(MatchUserInfoMap::value_type(uuid, mui));

	return true;
}

bool TeamMatch::OnUserStartCrossMatch(const UniqueUserID& uuid, int user_level, int match_team_type, GuildID user_guild_id, bool is_auto_create)
{
	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL != mui) return false;

#ifdef _DEBUG
	gamelog::g_log_world.printf(LL_INFO, "OnUserStartCrossMatch uid[%d] lv[%d] team_type[%d] guild_id[%d]",
		UidToInt(uuid.user_id), user_level, match_team_type, user_guild_id);
#endif

	mui = new MatchUserInfo();
	mui->uuid = uuid;	
	mui->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	mui->user_level = user_level;
	mui->match_team_type = match_team_type;
	mui->is_cross_matching = true;
	mui->user_guild_id = user_guild_id;
	mui->is_auto_create = is_auto_create;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		//mui->match_status = MATCH_STATUS_SUSPEND;
		//m_suspend_user_map.insert(MatchUserInfoMap::value_type(uuid, mui));
		mui->match_status = MATCH_STATUS_MATCHING;
		m_matching_user_map.insert(MatchUserInfoMap::value_type(uuid, mui));

		static crossgameprotocal::GameHiddenUserStartCrossMatch ghuscm;
		ghuscm.origin_plat_type = uuid.plat_type;
		ghuscm.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		ghuscm.origin_role_id = UidToInt(uuid.user_id);
		ghuscm.role_level = user_level;
		ghuscm.match_team_type = match_team_type;
		ghuscm.role_guild_id = user_guild_id;
		ghuscm.is_auto_create = is_auto_create;
		ghuscm.reserve_ch = 0;
		ghuscm.reserve_sh = 0;

		return InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghuscm, sizeof(ghuscm));
	}
	else
	{
		mui->match_status = MATCH_STATUS_MATCHING;
		m_matching_user_map.insert(MatchUserInfoMap::value_type(uuid, mui));
	
		return true;
	}
}

void TeamMatch::OnUserStopMatch(const UniqueUserID& uuid)
{
	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL == mui) return;

	if (!CrossConfig::Instance().IsHiddenServer() && mui->is_cross_matching)
	{
		this->OnUserStopCrossMatch(uuid);
	}

	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uuid.user_id);
		if (NULL != role)
		{
			this->SendMatchInfo(role, 0);
		}
	}

	this->RemoveMatchUserInfo(&mui);
}

void TeamMatch::OnUserStopCrossMatch(const UniqueUserID& uuid)
{
	static crossgameprotocal::GameHiddenUserStopCrossMatch ghuscm;
	ghuscm.origin_plat_type = uuid.plat_type;
	ghuscm.origin_role_id = UidToInt(uuid.user_id);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghuscm, sizeof(ghuscm));
}

void TeamMatch::OnUserStopLocalMatch(const UniqueUserID& uuid)
{
	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL == mui || mui->IsSuspend()) return;

	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uuid.user_id);
		if (NULL != role)
		{
			this->SendMatchInfo(role, 0);
		}
	}

	this->RemoveMatchUserInfo(&mui);
}

MatchTeamInfo* TeamMatch::GetMatchTeamInfo(int team_index)
{
	MatchTeamInfoMap::iterator it = m_matching_team_map.find(team_index);
	if (it != m_matching_team_map.end())
	{
		return it->second;
	}

	it = m_suspend_team_map.find(team_index);
	if (it != m_suspend_team_map.end())
	{
		return it->second;
	}

	return NULL;
}

MatchUserInfo* TeamMatch::GetMatchUserInfo(const UniqueUserID& uuid)
{
	MatchUserInfoMap::iterator it = m_matching_user_map.find(uuid);
	if (it != m_matching_user_map.end())
	{
		return it->second;
	}

	it = m_suspend_user_map.find(uuid);
	if (it != m_suspend_user_map.end())
	{
		return it->second;
	}

	return NULL;
}

void TeamMatch::RemoveMatchTeamInfo(MatchTeamInfo** mti)
{
	m_matching_team_map.erase((*mti)->team_index);
	m_suspend_team_map.erase((*mti)->team_index);
	delete (*mti);
	(*mti) = NULL;
}

void TeamMatch::RemoveMatchUserInfo(MatchUserInfo** mui)
{
	m_matching_user_map.erase((*mui)->uuid);
	m_suspend_user_map.erase((*mui)->uuid);
	delete (*mui);
	(*mui) = NULL;
}

void TeamMatch::Release()
{
	MatchTeamInfoMap::iterator it = m_matching_team_map.begin();
	for (; it != m_matching_team_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_matching_team_map.clear();

	it = m_suspend_team_map.begin();
	for (; it != m_suspend_team_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_suspend_team_map.clear();
}

bool SortByStartTime(MatchTeamInfo* a, MatchTeamInfo* b)
{
	if (a->start_match_time != b->start_match_time)
	{
		return a->start_match_time < b->start_match_time;
	}
	else
	{
		return a->team_index < b->team_index;
	}
}

void TeamMatch::DoMatch()
{
	std::vector<MatchTeamInfo*> match_team_vec[TEAM_TYPE_MAX];
	std::vector<MatchUserInfo*> match_user_vec[TEAM_TYPE_MAX];
	std::vector<MatchUserInfo*> match_succ_user_vec;
	match_succ_user_vec.reserve(GetMin(20, (int)m_matching_user_map.size() / 2));

	// 先排序
	std::vector<MatchTeamInfo*> match_team_vec_for_sort;
	match_team_vec_for_sort.reserve(m_matching_team_map.size());
	MatchTeamInfoMap::iterator it = m_matching_team_map.begin();
	for (; it != m_matching_team_map.end(); ++it)
	{
		MatchTeamInfo* mti = it->second;
		if (mti->IsSuspend()) continue;

		if (mti->team->IsInFb())
		{
			Role* leader_role = mti->team->GetLeaderRole();
			if (NULL != leader_role && !leader_role->GetScene()->GetSpecialLogic()->CanEnterTeam(NULL, NULL, false))
			{
				continue;
			}
		}

		match_team_vec_for_sort.push_back(it->second);
	}
	std::sort(match_team_vec_for_sort.begin(), match_team_vec_for_sort.end(), SortByStartTime);

	for (size_t i = 0; i < match_team_vec_for_sort.size(); ++i)
	{
		int team_type = match_team_vec_for_sort[i]->team->GetTeamType();
		match_team_vec[team_type].push_back(match_team_vec_for_sort[i]);
	}

	MatchUserInfoMap::iterator it2 = m_matching_user_map.begin();
	for (; it2 != m_matching_user_map.end(); ++it2)
	{
		MatchUserInfo* mui = it2->second;
		if (mui->IsSuspend()) continue;

		int online_status = UserCacheManager::Instance().GetUserOnlineStatus(mui->uuid.user_id);
		if (UserStatusNode::STATUS_TYPE_OFFLINE == online_status)
		{
			continue; // 玩家暂不在线，则不为其匹配
		}

		int match_team_type = mui->match_team_type;
		match_user_vec[match_team_type].push_back(mui);
	}

	for (int team_type = TEAM_TYPE_DEFAULT; team_type < TEAM_TYPE_MAX; ++team_type)
	{
		for (size_t k = 0; k < match_user_vec[team_type].size(); ++k)
		{
			MatchUserInfo* mui = match_user_vec[team_type][k];
			if (mui->IsSuspend()) continue;

			for (size_t i = 0; i < match_team_vec[team_type].size(); ++i)
			{
				MatchTeamInfo* mti = match_team_vec[team_type][i];
				if (mti->IsSuspend() || mti->IsFullForMatch()) continue;

				if (mui->user_level < mti->team->GetLimitLevelLow() ||
					mui->user_level > mti->team->GetLimitLevelHigh())
				{
					continue;
				}

				if (TEAM_TYPE_GUILD_FIGHT == team_type)//家族大乱斗增加匹配逻辑只允许同家族成员
				{
					if (mti->creator_guild_id != mui->user_guild_id)
					{
						continue;
					}
				}

				if (TEAM_TYPE_SHAN_HAI_DENG_HUI == team_type || TEAM_TYPE_KE_JU_EXAM == team_type)
				{
					mti->is_auto_add_robot = false;
				}

				mui->enter_team_index = mti->team->GetTeamIndex();
				mui->match_status = MATCH_STATUS_SUSPEND;
				mui->start_enter_team_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;

				m_matching_user_map.erase(mui->uuid);
				m_suspend_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));
				match_succ_user_vec.push_back(mui);

				mti->AddUserToEnteringList(mui->uuid);
				if (mti->IsFullForMatch())
				{
					mti->is_auto_add_robot = false;
					mti->match_status = MATCH_STATUS_SUSPEND;
					mti->suspend_end_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;
					m_matching_team_map.erase(mti->team_index);
					m_suspend_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
				}
			}

			const int team_type_target_type = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeTargetType(team_type);
			if ((TEAM_TYPE_RESEARCH_TASK == team_type
					|| TEAM_TYPE_TARGET_TYPE_WAN_LING_HUA_BEN == team_type_target_type)
				&& !mui->IsSuspend())
			{
				for (size_t i = 0; i < match_team_vec[team_type].size(); ++i)
				{
					MatchTeamInfo* mti = match_team_vec[team_type][i];
					if (mti->IsSuspend()) continue;

					if (mui->user_level < mti->team->GetLimitLevelLow() ||
						mui->user_level > mti->team->GetLimitLevelHigh())
					{
						continue;
					}

					// 当正在进入该队的玩家列表长度 + 队伍当前人数 >= 队伍最大数量时， 需要尝试踢出一个机器人，如果踢不出去，则认为该次匹配无效并continue
					if (mti->entering_list.size() + mti->team->GetMemberCount() >= TEAM_MEMBER_MAX_NUM)
					{
						int robot_member_index = -1;
						if (!mti->team->HasRobotMember(&robot_member_index))
						{
							continue;
						}

						mti->team->LeaveTeam(robot_member_index);
					}
					
					mui->enter_team_index = mti->team->GetTeamIndex();
					mui->match_status = MATCH_STATUS_SUSPEND;
					mui->start_enter_team_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

					m_matching_user_map.erase(mui->uuid);
					m_suspend_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));
					match_succ_user_vec.push_back(mui);

					mti->entering_list.push_back(mui->uuid);
					if (mti->IsFullForMatch())
					{
						mti->match_status = MATCH_STATUS_SUSPEND;
						mti->suspend_end_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
						m_matching_team_map.erase(mti->team_index);
						m_suspend_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
					}
				}
			}
		}
	}

	for (size_t i = 0; i < match_succ_user_vec.size(); ++i)
	{
		MatchUserInfo* mui = match_succ_user_vec[i];
		this->OnMatchSucc(mui);
	}
}

void TeamMatch::DoAutoAddRobot()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	std::vector<MatchTeamInfo*> match_team_vec[TEAM_TYPE_MAX];
	std::vector<MatchUserInfo*> match_user_vec[TEAM_TYPE_MAX];

	MatchTeamInfoMap::iterator it = m_matching_team_map.begin();
	for (; it != m_matching_team_map.end(); ++it)
	{
		MatchTeamInfo* mti = it->second;
		if (mti->IsSuspend() || mti->IsFullForMatch() || !mti->is_auto_add_robot)
		{
			continue;
		}

		int team_type = mti->team->GetTeamType();
		const TeamTypeRobotCfg* cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(team_type);
		if (NULL == cfg || !cfg->is_run_robot) continue;

		const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(team_type);
		if (NULL == team_type_cfg) continue;
		
		// 队长等级不足目标最低等级，则不给他匹配机器人
		int leader_level = mti->team->GetLeaderLevel();
		if (leader_level < team_type_cfg->low_level_limit) continue;

		if (now < mti->start_match_time + cfg->add_robot_seconds || 
			now < mti->next_add_robot_time)
		{
			continue;
		}

		if (mti->team->GetRobotMemberNum() >= cfg->robot_limit)
		{
			continue;
		}

		match_team_vec[team_type].push_back(mti);
	}

	MatchUserInfoMap::iterator it2 = m_matching_user_map.begin();
	for (; it2 != m_matching_user_map.end(); ++it2)
	{
		MatchUserInfo* mui = it2->second;
		if (mui->IsSuspend()) continue;

		int match_team_type = mui->match_team_type;
		//const TeamTypeRobotCfg* cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(match_team_type);
		//if (NULL == cfg || !cfg->is_user_create_team) continue;
		//
		//if (now < mui->start_match_time + cfg->add_robot_seconds)
		//{
		//	continue;
		//}

		static const int USER_MATCH_AUTO_CREATE_TEAM_TIME_S = 15;
		if (now < mui->start_match_time + USER_MATCH_AUTO_CREATE_TEAM_TIME_S)
		{
			continue;
		}
		const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(mui->match_team_type);
		if (NULL != team_type_cfg && !team_type_cfg->is_creat_team && !mui->is_auto_create)
		{
			continue;
		}

		match_user_vec[match_team_type].push_back(mui);
	}

	for (int team_type = TEAM_TYPE_DEFAULT; team_type < TEAM_TYPE_MAX; ++team_type)
	{
		for (size_t i = 0; i < match_team_vec[team_type].size(); ++i)
		{
			MatchTeamInfo* mti = match_team_vec[team_type][i];
			this->OnAddRobotToTeam(mti);
		}
	}

	for (int team_type = TEAM_TYPE_DEFAULT; team_type < TEAM_TYPE_MAX; ++team_type)
	{
		for (size_t i = 0; i < match_user_vec[team_type].size(); ++i)
		{
			MatchUserInfo* mui = match_user_vec[team_type][i];
			this->OnAddRobotToUser(mui);
		}
	}
}

void TeamMatch::DoSuspendEndRestartMatch()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	{
		MatchTeamInfoMap::iterator it = m_suspend_team_map.begin();
		for (; it != m_suspend_team_map.end();)
		{
			MatchTeamInfo* mti = it->second;
			if (mti->suspend_end_time > 0u && now > mti->suspend_end_time)
			{
				mti->match_status = MATCH_STATUS_MATCHING;
				mti->start_match_time = now;
				mti->suspend_end_time = 0u;
				m_matching_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
				m_suspend_team_map.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
	
	{
		MatchUserInfoMap::iterator it = m_suspend_user_map.begin();
		for (; it != m_suspend_user_map.end();)
		{
			MatchUserInfo* mui = it->second;
			if (mui->start_enter_team_time > 0u && now > mui->start_enter_team_time + 10)
			{
				mui->match_status = MATCH_STATUS_MATCHING;
				mui->start_match_time = now;
				mui->start_enter_team_time = 0u;
				mui->enter_team_index = 0;
				m_matching_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));
				m_suspend_user_map.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}

void TeamMatch::OnMatchSucc(MatchUserInfo* mui)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 非隐藏服 直接拉进队
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(mui->uuid.user_id);
		if (NULL == role)
		{
			// 通知拉进队失败
			this->OnEnterTeamFail(mui->uuid);
			return;
		}

		if (TeamManager::TOR_SUC != TeamManager::Instance().JoinTeamDirectly(role, mui->enter_team_index))
		{
			// 通知拉进队失败
			this->OnEnterTeamFail(mui->uuid);
			return;
		}
	}
	else
	{	
		// 隐藏服 先看是否在隐藏服
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(mui->uuid.user_id);
		if (NULL == role)
		{
			// 不在隐藏服，则去原服拉人
			static crossgameprotocal::HiddenGameTeamMatchSuccJoinTeam hgtmsjt;
			hgtmsjt.cross_team_index = mui->enter_team_index;
			hgtmsjt.origin_plat_type = mui->uuid.plat_type;
			hgtmsjt.origin_server_id = mui->uuid.user_id.db_index;
			hgtmsjt.role_id = UidToInt(mui->uuid.user_id);

			InternalComm::Instance().SendToGameThroughCross(mui->uuid.plat_type, mui->uuid.user_id.db_index, (const char*)&hgtmsjt, sizeof(hgtmsjt));
		}
		else
		{
			// 在隐藏服就直接拉进队
			if (TeamManager::TOR_SUC != TeamManager::Instance().JoinTeamDirectly(role, mui->enter_team_index))
			{
				// 通知拉进队失败
				this->OnEnterTeamFail(mui->uuid);
				return;
			}
		}
		
	}
}

bool TeamMatch::OnAddRobotToTeam(MatchTeamInfo* mti)
{
	if (NULL == mti) return false;

	Team* team = mti->team;
	Role* leader_role = team->GetLeaderRole();
	if (NULL == leader_role) return false;
	if (!leader_role->GetScene()->IsInStaticScene()) return false;

	const TeamTypeRobotCfg* cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(team->GetTeamType());
	if (NULL == cfg) return false;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!cfg->use_real_role_robot
		|| this->IsWaitTooLongForRealRoleRobot(mti, now)
		|| leader_role->GetLevel() < ROBOTCONFIG->GetOtherCfg().match_real_role_robot_level)
	{
		this->AddNormalRobotToTeam(1, team, leader_role, cfg);
		if (mti->team->GetRobotMemberNum() >= cfg->robot_limit)
		{
			this->ResetTryRealRoleRobotTime(mti);
		}
	}
	else
	{
		this->SetTryRealRoleRobotTime(mti, now);
		this->TryAddRealRoleRobotToTeam(1, team, leader_role, cfg);
	}
	
	mti->next_add_robot_time = now + RandomNum(cfg->min_add_robot_interval_s, cfg->max_add_robot_interval_s);

	if (mti->IsFullForMatch())
	{
		mti->is_auto_add_robot = false;
		mti->match_status = MATCH_STATUS_SUSPEND;
		mti->suspend_end_time = now + 10;
		m_matching_team_map.erase(mti->team_index);
		m_suspend_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
	}

	return true;
}

bool TeamMatch::OnAddRobotToUser(MatchUserInfo* mui)
{
	// 目前只创建队伍
	UserID user_id = mui->uuid.user_id;
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return false;
	if (NULL != role->GetMyTeam()) return false;
	if (!role->GetScene()->IsInStaticScene()) return false;
	if (role->GetRoleStatusManager()->IsInBattleStatus()) return false;

	if (mui->is_cross_matching && !CrossConfig::Instance().IsHiddenServer())
	{
		int low_level_limit = 0;
		int high_level_limit = MAX_ROLE_LEVEL;
		const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(mui->match_team_type);
		if (NULL != team_type_cfg)
		{
			low_level_limit = team_type_cfg->low_level_limit;
			high_level_limit = team_type_cfg->high_level_limit;
		}

		CrossData_Business business_data;
		business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_CREATE_CROSS_TEAM;
		business_data.level_limit_high = high_level_limit;
		business_data.level_limit_low = low_level_limit;
		business_data.team_type = mui->match_team_type;
		business_data.is_match = 1;
		business_data.allown_red_join = 0;

		role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
	
		mui->match_status = MATCH_STATUS_SUSPEND;
		mui->start_enter_team_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		m_matching_user_map.erase(mui->uuid);
		m_suspend_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));
		
		return true;
	}

	int low_level_limit = 0;
	int high_level_limit = MAX_ROLE_LEVEL;
	const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(mui->match_team_type);
	if (NULL != team_type_cfg)
	{
		low_level_limit = team_type_cfg->low_level_limit;
		high_level_limit = team_type_cfg->high_level_limit;
	}

	Team* team = TeamManager::Instance().CreateTeam(role, low_level_limit, high_level_limit, mui->match_team_type, true, true);
	if (NULL == team) return false;

	

	/*
	int add_robot_num = 1;
	if (add_robot_num > 0)
	{
		for (int i = 0; i < add_robot_num; ++i)
		{
			int robot_level = ROBOTCONFIG->RandRobotLevelByGroup(cfg->level_group_id, role->GetLevel());
			Robot* robot = RobotManager::Instance().CreateRobotToScene(role->GetScene(), role->GetPos(), robot_level, true, __FUNCTION__, role->GetUserId().db_index);
			if (NULL != robot)
			{
				robot->SetBoundTeamType(team->GetTeamType());
				robot->SetBoundBattleMode(cfg->battle_mode);
				team->AddRobot(robot);
			}
		}
	}
	*/

	MatchTeamInfo* mti = this->GetMatchTeamInfo(team->GetTeamIndex());
	if (NULL == mti)
	{
		this->OnTeamStartMatch(team);
		mti = this->GetMatchTeamInfo(team->GetTeamIndex());
	}

	if (NULL != mti)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		const TeamTypeRobotCfg* cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(mui->match_team_type);
		if (NULL != cfg)
		{
			mti->start_match_time = now - cfg->add_robot_seconds;
			mti->next_add_robot_time = now;
		}
		else
		{
			mti->start_match_time = now;
		}
	}
	return true;
}

void TeamMatch::OnEnterTeamSucc(const UniqueUserID& uuid, int team_index)
{
	MatchTeamInfo* mti = this->GetMatchTeamInfo(team_index);
	if (NULL != mti)
	{
		mti->RemoveUserFromEnteringList(uuid);
	}

	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uuid.user_id);
		if (NULL != role)
		{
			this->SendMatchInfo(role, 0);
		}
	}

	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL != mui)
	{
		this->RemoveMatchUserInfo(&mui);
	}

	if (NULL != mti)
	{
		// 满员且没有机器人情况下，停止匹配
		if (mti->team->GetMemberCount() >= TEAM_MEMBER_MAX_NUM && !mti->team->HasRobotMember(NULL))
		{
			this->OnTeamStopMatch(mti->team);
		}
		else if (mti->IsSuspend() && mti->entering_list.empty())
		{
			mti->match_status = MATCH_STATUS_MATCHING;
			mti->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			m_matching_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
			m_suspend_team_map.erase(mti->team_index);
		}	
	}
}

void TeamMatch::OnEnterTeamFail(const UniqueUserID& uuid, bool continue_match /* = true */)
{
	MatchUserInfo* mui = this->GetMatchUserInfo(uuid);
	if (NULL == mui) return;

	int team_index = mui->enter_team_index;
	MatchTeamInfo* mti = this->GetMatchTeamInfo(team_index);
	if (NULL != mti)
	{
		mti->RemoveUserFromEnteringList(uuid);
	}

	if (continue_match && mui->IsSuspend())
	{
		// 立即重新加入匹配
		mui->match_status = MATCH_STATUS_MATCHING;
		mui->start_enter_team_time = 0u;
		mui->enter_team_index = 0;

		m_suspend_user_map.erase(uuid);
		m_matching_user_map.insert(MatchUserInfoMap::value_type(uuid, mui));
	}
	else
	{
		this->RemoveMatchUserInfo(&mui);
	}

	if (NULL != mti)
	{
		// 满员且没有机器人情况下，停止匹配
		if (mti->team->GetMemberCount() >= MAX_TEAM_MEMBER_NUM && !mti->team->HasRobotMember(NULL))
		{
			this->OnTeamStopMatch(mti->team);
		}
		else if (mti->IsSuspend() && mti->entering_list.empty())
		{
			mti->match_status = MATCH_STATUS_MATCHING;
			mti->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			m_matching_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
			m_suspend_team_map.erase(mti->team_index);
		}
	}
}

void TeamMatch::SendMatchInfo(Role* user, int is_matching)
{
	Protocol::SCTeamMatchInfo info;
	info.is_match = is_matching;
	EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&info, sizeof(info));
}

void TeamMatch::GmAddUserToMatch()
{
	MatchUserInfo* mui = new MatchUserInfo();
	mui->uuid = UniqueUserID(829189376, UserID(1, 1314));
	mui->start_match_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	mui->user_level = 120;
	mui->match_team_type = 10;
	mui->is_cross_matching = true;
	mui->user_guild_id = 0;

	mui->match_status = MATCH_STATUS_MATCHING;
	m_matching_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));
}

void TeamMatch::OnOfflineBattleDataForRobotResp(crossgameprotocal::HiddenGameQueryOfflineBattleDataForRobot* msg)
{
	bool succ = false;

	do
	{
		MatchTeamInfo* team_info = this->GetMatchTeamInfo(msg->team_id);
		if (NULL == team_info || team_info->IsFullForMatch()) break;

		succ = this->OnAddRealRoleRobotToTeam(team_info, msg->uid, msg->rbd);
	} while (0);

	OfflineRobotRoute::NotifyOfflineBattleDataForRobotResult(msg->uid, succ);
}

bool MatchTeamInfo::IsSuspend()
{
	return MATCH_STATUS_SUSPEND == match_status;
}

bool MatchTeamInfo::IsFullForMatch()
{
	if (NULL == team) return false;

	int max_team_member_num = MAX_TEAM_MEMBER_NUM;
	int team_type = team->GetTeamType();
	if (TEAM_TYPE_QINGYUAN_DUIDUIPENG == team_type ||
		TEAM_TYPE_TU_ZI_KUAI_PAO == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_1 == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_2 == team_type ||
		TEAM_TYPE_CONCENTTRIC_KONT_3 == team_type)
	{
		max_team_member_num = 2;			// 这类队伍类型 只要两个人
	}

	int tmp_max_member_num = 0;
	if (LOGIC_CONFIG->GetTeamConfig()->IsTeamTypeLimitMaxMember(team_type, &tmp_max_member_num))
	{
		max_team_member_num = tmp_max_member_num;
	}

	return (team->GetMemberCount() + (int)entering_list.size() >= max_team_member_num);
}

MatchTeamInfo* MatchTeamInfo::Clone()
{
	MatchTeamInfo* mti = new MatchTeamInfo();
	*mti = *this;

	return mti;
}

void MatchTeamInfo::AddUserToEnteringList(const UniqueUserID& uuid)
{
	for (size_t i = 0; i < entering_list.size(); ++i)
	{
		if (uuid == entering_list[i])
		{
			return;
		}
	}
	entering_list.push_back(uuid);
}

void MatchTeamInfo::RemoveUserFromEnteringList(const UniqueUserID& uuid)
{
	for (int i = entering_list.size() - 1; i >= 0; --i)
	{
		if (uuid == entering_list[i])
		{
			entering_list[i] = entering_list[entering_list.size() - 1];
			entering_list.pop_back();
		}
	}
}

bool MatchUserInfo::IsSuspend()
{
	return MATCH_STATUS_SUSPEND == match_status;
}

bool TeamMatch::TryMatchTeamWithRobot(MatchUserInfo* mui)
{
	/*
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(mui->uuid.user_id);
	if (NULL == role)
	{
		return false; 
	}

	MatchTeamInfoMap::iterator it = m_substitude_robot_team_map.begin();
	for (; it != m_substitude_robot_team_map.end();)
	{
		MatchTeamInfo* mti = it->second;
		Team* team = TeamManager::Instance().GetTeamByTeamIndex(mti->team_index);
		int robot_member_index = -1;
		if (NULL == team || !team->HasRobotMember(&robot_member_index))
		{
			delete it->second;
			m_substitude_robot_team_map.erase(it++);
		}
		else if (mui->match_team_type != mti->team->GetTeamType())
		{
			++it;
		}
		else
		{
			team->LeaveTeam(robot_member_index);

			m_matching_user_map.erase(mui->uuid);
			m_suspend_user_map.insert(MatchUserInfoMap::value_type(mui->uuid, mui));

			this->OnMatchSucc(mui);
			this->SendMatchInfo(role, 0);
		
			if (!team->HasRobotMember(NULL))
			{
				delete it->second;
				m_substitude_robot_team_map.erase(it++);
			}

			return true;		
		}
	}
	*/
	return false;
}

void TeamMatch::CheckExpireMatchUser()
{
	std::vector<MatchUserInfo*> expire_list;
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	MatchUserInfoMap::iterator it = m_matching_user_map.begin();
	for (; it != m_matching_user_map.end(); ++it)
	{
		if (NULL == it->second) continue;

		static const int MATCH_OFFLINE_EXPIRE_TIME_S = 3600;
		static const int MATCH_EXPIRE_TIME_S = 3600 * 24;

		MatchUserInfo* mui = it->second;
		if (mui->start_match_time + MATCH_EXPIRE_TIME_S < now)
		{
			expire_list.push_back(mui);
		}
		else if (mui->start_match_time + MATCH_OFFLINE_EXPIRE_TIME_S < now)
		{
			int online_status = UserCacheManager::Instance().GetUserOnlineStatus(mui->uuid.user_id);
			if (UserStatusNode::STATUS_TYPE_OFFLINE == online_status)
			{
				expire_list.push_back(mui);
			}
		}
	}

	for (size_t i = 0; i < expire_list.size(); ++i)
	{
		this->RemoveMatchUserInfo(&expire_list[i]);
	}
}

void TeamMatch::AddNormalRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg)
{
	if (NULL == team || NULL == cfg || NULL == leader_role) return;

	for (int i = 0; i < add_robot_num; ++i)
	{
		int robot_level = ROBOTCONFIG->RandRobotLevelByGroup(cfg->level_group_id, leader_role->GetLevel());
		if (robot_level < team->GetLimitLevelLow())
		{
			robot_level = GetMin(team->GetLimitLevelLow(), leader_role->GetLevel());
		}
		else if (robot_level > team->GetLimitLevelHigh())
		{
			robot_level = GetMin(team->GetLimitLevelHigh(), leader_role->GetLevel());
		}

		int limit_low_level = team->GetLimitLevelLow();
		int limit_high_level = team->GetLimitLevelHigh();
		Robot* robot = RobotManager::Instance().CreateRobotToScene(leader_role->GetScene(), leader_role->GetPos(), robot_level, true, __FUNCTION__, leader_role->GetUserId().db_index, &limit_low_level, &limit_high_level);
		if (NULL != robot)
		{
			robot->SetBoundTeamType(team->GetTeamType());
			robot->SetBoundBattleMode(cfg->battle_mode);
			team->AddRobot(robot);
		}
	}
}

void TeamMatch::TryAddRealRoleRobotToTeam(int add_robot_num, Team* team, Role* leader_role, const TeamTypeRobotCfg* cfg)
{
	if (NULL == team || NULL == cfg || NULL == leader_role) return;

	const RobotOtherCfg& other_cfg = ROBOTCONFIG->GetOtherCfg();
	for (int i = 0; i < add_robot_num; ++i)
	{
		int robot_lv_min = GetMin(team->GetLimitLevelHigh(), leader_role->GetLevel());
		int robot_lv_max = GetMin(team->GetLimitLevelHigh(), leader_role->GetLevel() + 10);
		int best_level_min = GetMax(team->GetLimitLevelLow(), leader_role->GetLevel() + other_cfg.match_level_down);
		int best_level_max = GetMin(team->GetLimitLevelHigh(), leader_role->GetLevel() + other_cfg.match_level_up);
		std::set<int> exclude_uid_set;
		for (int k = 0; k < team->GetMemberCount(); ++k)
		{
			TeamMember* member = team->GetMemberInfoByIndex(k);
			if (NULL == member) continue;

			if (!member->IsRobot())
			{
				exclude_uid_set.insert(member->uid);
			}
			else
			{
				Robot* robot = team->GetMemberRobotByIndex(k);
				if (NULL != robot && robot->GetRealRoleUID() > 0)
				{
					exclude_uid_set.insert(robot->GetRealRoleUID());
				}
			}
		}
		OfflineRobotRoute::RequestAddOfflineRoleRobotToTeam(team->GetTeamIndex(), 
			leader_role->GetLevel(), robot_lv_min, robot_lv_max, 
			exclude_uid_set, best_level_min, best_level_max);
	}
}

bool TeamMatch::OnAddRealRoleRobotToTeam(MatchTeamInfo* mti, int uid, const RoleBattleData& rbd)
{
	Team* team = mti->team;
	Role* leader_role = team->GetLeaderRole();
	if (NULL == leader_role) return false;
	if (!leader_role->GetScene()->IsInStaticScene()) return false;

	if (NULL != team->GetMemberInfoByUID(uid))
	{
		// 分身的主人在队里，就不加机器人进来了
		return false;
	}

	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Robot* robot = team->GetMemberRobotByIndex(i);
		if (NULL != robot && robot->GetRealRoleUID() == uid)
		{
			// 已有一个相同的分身在队里
			return false;
		}
	}

	const TeamTypeRobotCfg* cfg = ROBOTCONFIG->GetAutoTeamRobotCfg(team->GetTeamType());
	if (NULL == cfg) return false;

	RobotParam robot_param;
	if (!ROBOTMANAGER.ConstructRobotParamByRealRoleBattleData(uid, rbd, &robot_param))
	{
		return false;
	}

	Robot* robot = RobotManager::Instance().CreateRobotToScene(leader_role->GetScene(), leader_role->GetPos(), robot_param, __FUNCTION__);
	if (NULL == robot) 
	{
		return false;
	}
	
	robot->SetRealRoleUID(uid);
	robot->SetBoundTeamType(team->GetTeamType());
	robot->SetBoundBattleMode(cfg->battle_mode);
	team->AddRobot(robot);
	
	this->ResetTryRealRoleRobotTime(mti);
	if (mti->IsFullForMatch())
	{
		mti->is_auto_add_robot = false;
		mti->match_status = MATCH_STATUS_SUSPEND;
		mti->suspend_end_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;
		m_matching_team_map.erase(mti->team_index);
		m_suspend_team_map.insert(MatchTeamInfoMap::value_type(mti->team_index, mti));
	}

	return true;
}

bool TeamMatch::IsWaitTooLongForRealRoleRobot(const MatchTeamInfo* mti, unsigned int now) const
{
	// 从开始匹配真人
	return mti->try_real_role_robot_time > 0 && mti->try_real_role_robot_time + 60 < now;
}

void TeamMatch::ResetTryRealRoleRobotTime(MatchTeamInfo* mti)
{
	mti->try_real_role_robot_time = 0u;
}

void TeamMatch::SetTryRealRoleRobotTime(MatchTeamInfo* mti, unsigned int now)
{
	mti->try_real_role_robot_time = now;
}
