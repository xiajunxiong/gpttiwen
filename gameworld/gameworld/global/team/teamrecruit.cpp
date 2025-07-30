#include "teamrecruit.hpp"
#include "protocol/msgteam.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/team/team.hpp"
#include "obj/character/role.h"
#include "global/team/teammanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/team/teamconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolemodulemanager.hpp"
#include "other/researchtask/researchtask.h"
#include "other/wanlingfb/wanlingfb.hpp"
#include "internalcomm.h"
#include "gamelog.h"

TeamRecruit::TeamRecruit() : m_next_check_timestamp(0)
{

}

void TeamRecruit::AddRoleRecruit(Role* role, int team_id, int team_type,
	const char* leader_name, int limit_lv_low, int limit_lv_high,
	int cur_member_num, int max_member_num, bool is_from_cross)
{
	int role_id = role->GetUID();
	RoleRecruitInfo* rri = this->MutableRoleRecruitInfo(role_id);
	if (NULL == rri)
	{
		RoleRecruitInfo info;
		info.role_id = role_id;
		info.team_id = 0;

		m_role_recruit_map.insert(RoleRecruitMap::value_type(role_id, info));
		rri = this->MutableRoleRecruitInfo(role_id);
		if (NULL == rri) return;
	}

	if (rri->team_id > 0)
	{
#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "AddRoleRecruit leader[%s] is_cross[%s] be_recruit_role[%d,%s] being recruit now",
			leader_name, is_from_cross ? "T" : "F", role->GetUID(), role->GetName());
#endif

		return;
	}
	if (this->CheckTeamTypeRefuse(rri, team_type))
	{
#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "AddRoleRecruit leader[%s] is_cross[%s] be_recruit_role[%d,%s] refuse_team_type[%d]",
			leader_name, is_from_cross ? "T" : "F", role->GetUID(), role->GetName(), team_type);
#endif

		return;
	}
	rri->team_id = team_id;
	F_STRNCPY(rri->leader_name, leader_name, sizeof(rri->leader_name));
	rri->team_type = team_type;
	rri->level_limit_low = limit_lv_low;
	rri->level_limit_high = limit_lv_high;
	rri->cur_member_num = cur_member_num;
	rri->max_member_num = max_member_num;
	rri->is_cross_team = is_from_cross;
	
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	rri->auto_reset_timestamp = now + 
		LOGIC_CONFIG->GetTeamConfig()->GetOtherCfg().ignore_team_recruit_s + 5; //客户端计时再+5秒
	
	this->SendRoleRecruitInfo(role, rri);


#ifdef _DEBUG
	gamelog::g_log_debug.printf(LL_INFO, "AddRoleRecruit leader[%s] is_cross[%s] be_recruit_role[%d,%s]",
		leader_name, is_from_cross ? "T" : "F", role->GetUID(), role->GetName());
#endif
}

bool TeamRecruit::CheckTeamTypeRefuse(RoleRecruitInfo* rri, int team_type)
{
	std::map<int, unsigned int>::iterator it = rri->refuse_map.find(team_type);
	if (it != rri->refuse_map.end())
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (now < it->second)
		{
			return true;
		}
		else
		{
			rri->refuse_map.erase(team_type);
		}
	}

	return false;
}

void TeamRecruit::OnRoleLogout(int role_id)
{
	this->RemoveRoleRecruit(role_id);
}

void TeamRecruit::CheckAutoRefuse(unsigned int now)
{
	RoleRecruitMap::iterator it = m_role_recruit_map.begin();
	for (; it != m_role_recruit_map.end(); ++it)
	{
		if (it->second.auto_reset_timestamp > 0u &&
			now > it->second.auto_reset_timestamp)
		{
#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "CheckAutoRefuse【AUTO_REFUSE】 be_recruit_role[%d] leader_name[%s] is_from_cross[%s]",
				it->first, it->second.leader_name, it->second.is_cross_team ? "T" : "F");
#endif

			this->ResetCurRecruitInfo(&it->second);
		}
	}
}

void TeamRecruit::CheckErase()
{
	std::vector<int> uid_list;
	uid_list.reserve(4);

	RoleRecruitMap::iterator it = m_role_recruit_map.begin();
	for (; it != m_role_recruit_map.end(); ++it)
	{
		if (it->second.team_id == 0
			&& it->second.refuse_map.empty())
		{
			uid_list.push_back(it->first);
		}
	}

	for (size_t i = 0; i < uid_list.size(); ++i)
	{
		this->RemoveRoleRecruit(uid_list[i]);
	}
}

RoleRecruitInfo* TeamRecruit::MutableRoleRecruitInfo(int role_id)
{
	RoleRecruitMap::iterator it = m_role_recruit_map.find(role_id);
	if (it != m_role_recruit_map.end())
	{
		return &it->second;
	}
	return NULL;
}

void TeamRecruit::SendRoleRecruitInfo(Role* role, const RoleRecruitInfo* rri)
{
	Protocol::SCTeamRoleRecruitInfo msg;
	msg.cur_member_num = rri->cur_member_num;
	F_STRNCPY(msg.leader_name, rri->leader_name, sizeof(msg.leader_name));
	msg.level_high = rri->level_limit_high;
	msg.level_low = rri->level_limit_low;
	msg.max_member_num = rri->max_member_num;
	msg.team_id = rri->team_id;
	msg.team_type = rri->team_type;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
}

void TeamRecruit::ResetCurRecruitInfo(RoleRecruitInfo* rri)
{
	rri->team_id = 0;
	rri->cur_member_num = 0;
	memset(rri->leader_name, 0, sizeof(rri->leader_name));
	rri->level_limit_high = 0;
	rri->level_limit_low = 0;
	rri->max_member_num = 0;
	rri->team_type = 0;
	rri->auto_reset_timestamp = 0u;
	rri->is_cross_team = false;
}

TeamRecruit::~TeamRecruit()
{

}

TeamRecruit& TeamRecruit::Instance()
{
	static TeamRecruit instance;
	return instance;
}

void TeamRecruit::Update(unsigned int now)
{
	if (now > m_next_check_timestamp)
	{
		this->CheckAutoRefuse(now);
		this->CheckErase();
		m_next_check_timestamp = now;
	}
}

struct RecruitCondition
{
	unsigned int now;
	int limit_lv_low;
	int limit_lv_high;
};

void TeamRecruit::RemoveRoleRecruit(int role_id)
{
	m_role_recruit_map.erase(role_id);
}

bool TeamRecruit::CheckRoleRemainTimesEnough(Role* role, int team_type, int team_type_target_type)
{
	if (TEAM_TYPE_RESEARCH_TASK == team_type)
	{
		if (role->GetRoleModuleManager()->GetResearchTask()
			->HasFinishAllToday())
		{
			return false;
		}
	}
	else if (TEAM_TYPE_WILD_BOSS == team_type)
	{
		if (!role->HasWildBossRewardTimes())
		{
			return false;
		}
	}
	else if (TEAM_TYPE_TARGET_TYPE_ELITE_TRAIL == team_type_target_type)
	{
		if (!role->GetRoleModuleManager()->GetRoleCrystalFb()
			->HasUseTimes())
		{
			return false;
		}
	}
	else if (TEAM_TYPE_TARGET_TYPE_WAN_LING_HUA_BEN == team_type_target_type)
	{
		if (!role->GetRoleModuleManager()->GetRoleWanLingFB()
			->HasRewardTimes())
		{
			return false;
		}
	}
	else if (TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS == team_type_target_type
		|| TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HARD == team_type_target_type
		|| TEAM_TYPE_TARGET_TYPE_SHANHAI_BOSS_HELL == team_type_target_type)
	{
		if (!role->GetRoleModuleManager()->HasRemainShanHaiBossDayTimes())
		{
			return false;
		}
	}
	
	return true;
}

bool TeamRecruitUserFilter(const UserCacheNode &user_node, void **func_param)
{
	RecruitCondition* rlr = (RecruitCondition*)func_param;
	return user_node.last_online_timestamp + 60 * 10 > rlr->now
		&& user_node.GetLevel() >= rlr->limit_lv_low
		&& user_node.GetLevel() <= rlr->limit_lv_high;
}

void TeamRecruit::OnTeamRecruit(int team_id, int team_type, 
	const char* leader_name, int limit_lv_low, int limit_lv_high, 
	int cur_member_num, int max_member_num, bool is_from_cross)
{
	const TeamTypeCfg* team_type_cfg = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeCfg(team_type);
	if (NULL == team_type_cfg || !team_type_cfg->is_recruit_team) return;

#ifdef _DEBUG
	static const int ONCE_OPER_NUM = 1;	// 用于测试是否会重复发邮件
#else
	static const int ONCE_OPER_NUM = 1024;
#endif
	UserID user_list[ONCE_OPER_NUM];
	int from = 0;

	RecruitCondition rlr;
	rlr.now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	rlr.limit_lv_low = limit_lv_low;
	rlr.limit_lv_high = limit_lv_high;

	int cnt = 100;
	while (--cnt)
	{
		int count = UserCacheManager::Instance().GetUserIDList(from, ONCE_OPER_NUM, user_list, TeamRecruitUserFilter, (void**)&rlr);
		if (count <= 0) break;

		for (int i = 0; i < count; ++i)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_list[i]);
			if (NULL == role || role->InTeam()) continue;
			
			if (!this->CheckRoleRemainTimesEnough(role,
				team_type_cfg->team_type,
				team_type_cfg->team_type_target_type))
			{
#ifdef _DEBUG
				gamelog::g_log_debug.printf(LL_INFO, "OnTeamRecruit leader[%s] is_cross[%s] be_recruit_role[%d,%s] times_not_enough",
					leader_name, is_from_cross ? "T" : "F", role->GetUID(), role->GetName());
#endif
				continue;
			}

			this->AddRoleRecruit(role, team_id, team_type, leader_name,
				limit_lv_low, limit_lv_high, cur_member_num, 
				max_member_num, is_from_cross);
		}
	}
}

void TeamRecruit::OnRoleAnswerRecruit(Role* role, Protocol::CSTeamRoleRecruitAnswer* req)
{
	RoleRecruitInfo* rri = this->MutableRoleRecruitInfo(role->GetUID());
	if (NULL == rri) return;

	if (Protocol::CSTeamRoleRecruitAnswer::ACCEPT == req->answer_type)
	{
		TeamManager::Instance().JoinTeam(role, rri->team_id, rri->is_cross_team);
	
#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "OnRoleAnswerRecruit【ACCEPT】 be_recruit_role[%d,%s] leader_name[%s] is_from_cross[%s]",
			role->GetUID(), role->GetName(), rri->leader_name, rri->is_cross_team ? "T" : "F");
#endif

		this->ResetCurRecruitInfo(rri);
	}
	else
	{
		if (Protocol::CSTeamRoleRecruitAnswer::REFUSE_MANUALLY 
			== req->answer_type)
		{
			unsigned int now = EngineAdapter::Instance().Time();
			int refuse_s = LOGIC_CONFIG->GetTeamConfig()->GetOtherCfg().refuse_team_recruit_time_s;
			rri->refuse_map.insert(
				std::map<int, unsigned int>::value_type(
					rri->team_type, now + refuse_s
				));

#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "OnRoleAnswerRecruit【AUTO_REFUSE】 be_recruit_role[%d,%s] leader_name[%s] is_from_cross[%s]",
				role->GetUID(), role->GetName(), rri->leader_name, rri->is_cross_team ? "T" : "F");
#endif
		}
		else
		{
#ifdef _DEBUG
			gamelog::g_log_debug.printf(LL_INFO, "OnRoleAnswerRecruit【REFUSE_MANUALLY】 be_recruit_role[%d,%s] leader_name[%s] is_from_cross[%s]",
				role->GetUID(), role->GetName(), rri->leader_name, rri->is_cross_team ? "T" : "F");
#endif
		}

		this->ResetCurRecruitInfo(rri);
	}
}

void TeamRecruit::SyncTeamRoleRecruitToOrigin(int team_id, int team_type, const char* leader_name, int limit_lv_low, int limit_lv_high, int cur_member_num, int max_member_num)
{
	crossgameprotocal::HiddenGameTeamRoleRecruitSync msg;
	msg.team_id = team_id;
	msg.team_type = team_type;
	F_STRNCPY(msg.leader_name, leader_name, sizeof(msg.leader_name));
	msg.limit_lv_low = limit_lv_low;
	msg.limit_lv_high = limit_lv_high;
	msg.cur_member_num = cur_member_num;
	msg.max_member_num = max_member_num;

	InternalComm::Instance().SendToAllGameThroughCross(&msg, sizeof(msg));
}

