#include "randactivityjoyseeksfavors.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "protocol/msgactivity.hpp"
#include "servercommon/errornum.h"
#include "global/usercache/usercache.hpp"
#include "other/funopen/funopen.hpp"
#include "other/title/title.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/matchmanager/matchdef.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

bool MatchLevel2(MatchTeamData* a, MatchTeamData* b)
{
	if (NULL == a || NULL == b) return false;
	return abs(a->GetTotalLevel() - b->GetTotalLevel()) <= 25;
}

RandactivityJoyseeksfavors::RandactivityJoyseeksfavors(RandActivityManager * activity_manager) : RandActivity(activity_manager, RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS)
{
	m_next_match_time = 0;
	m_match_logic.SetFunc(MatchLevel2);

	m_match_mgr = new RandActivityJoyseeksfavorsMatchManager(this);
	m_match_mgr->Init();
	
}

RandactivityJoyseeksfavors::~RandactivityJoyseeksfavors()
{
	m_match_logic.Reset();

	delete m_match_mgr;
	m_match_mgr = NULL;
}

void RandactivityJoyseeksfavors::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
	case ACTIVITY_STATUS_OPEN:
		{
			m_match_logic.Reset();
		}
		break;

	case ACTIVITY_STATUS_CLOSE://活动结束时，预设对应的下次开启时间
		{
			
		}
		break;
	}
}

void RandactivityJoyseeksfavors::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (ACTIVITY_STATUS_OPEN == m_rand_activity_status)
	{
		m_match_mgr->Update(interval);
	}
}

void RandactivityJoyseeksfavors::OnMonthChange()
{
	//m_role_list.clear();
}

void RandactivityJoyseeksfavors::OnMatchReq(Role * role)
{
	if (ACTIVITY_STATUS_OPEN != m_rand_activity_status)return;
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (!team->IsLeader(role))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return;
		}

		if (team->HasDismissMember())
		{
			role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return;
		}
	}

	m_match_mgr->RoleStartMatchReq(role);
}

void RandactivityJoyseeksfavors::OnCancelReq(Role * role)
{
	if (NULL == role) return;

	m_match_mgr->RoleStopMatchReq(role);
}

void RandactivityJoyseeksfavors::OnUserLogin(Role * role)
{
	RandActivity::OnUserLogin(role);
}

void RandactivityJoyseeksfavors::OnUserLogout(Role * role)
{
	RandActivity::OnUserLogout(role);
	m_match_mgr->RoleStopMatchReq(role);
}

void RandactivityJoyseeksfavors::OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	//TODO 拉进一个队伍里，并开始战斗
	static Role* attacker_role_list[MAX_TEAM_MEMBER_NUM];
	static Role* defender_role_list[MAX_TEAM_MEMBER_NUM];

	int atker_count = 0;
	int defer_count = 0;

	for (unsigned int i = 0; i < mtA->user_list.size() && atker_count < MAX_TEAM_MEMBER_NUM; ++i)
	{
		int role_id = mtA->user_list[i]->uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue; //TODO 做处理， 重新加入队列匹配
		}
		attacker_role_list[atker_count++] = role;
	}

	for (unsigned int i = 0; i < mtB->user_list.size() && defer_count < MAX_TEAM_MEMBER_NUM; ++i)
	{
		int role_id = mtB->user_list[i]->uid;
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL == role)
		{
			continue; //TODO 做处理， 重新加入队列匹配
		}
		defender_role_list[defer_count++] = role;
	}

	BattleManagerLocal::GetInstance().StartSpecialBattleJoyReq(attacker_role_list[0],defender_role_list[0]);
}
