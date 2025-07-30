#include "reliclootingmathmanager.hpp"
#include "normalmatchlogicimpl.hpp"
#include "servercommon/servercommon.h"
#include "protocol/msgrandactivity.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/gamelog.h"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "servercommon/string/gameworldstr.h"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activity.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "scene/fbmanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/rolemodulemanager.hpp"
#include "global/team/team.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/reliclooting/activityreliclootingconfig.hpp"
#include "global/team/teammanager.hpp"

ActivityRelicLootingMatchManager::ActivityRelicLootingMatchManager(ActivityRelicLooting* activity)
{
	m_activity = activity;
}

ActivityRelicLootingMatchManager::~ActivityRelicLootingMatchManager()
{
	
}

void ActivityRelicLootingMatchManager::Init()
{
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
	if (other_cfg.once_team_players > 0 && other_cfg.once_team_players <= RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM)
	{
		this->NormalMatchMgr::Init(other_cfg.once_team_players, new NormalMatchLogicImpl(), MATCH_BUSINESS_ACTIVITY_RELIC_LOOTING);
	}
}

void ActivityRelicLootingMatchManager::RoleStartMatchReq(Role * role)
{
	Activity *activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);

	if (NULL != activity && activity->IsActivityOpen())
	{
		if (role->InTeam())
		{
			const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
			if (other_cfg.once_team_players <= 0 || other_cfg.once_team_players > RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM)
			{
				role->NoticeNum(errornum::EN_RELICLOOTING_MATH_MEMBER_TOO_MUTCH);
				return;
			}
			if (role->GetMyTeam()->GetMemberCount() > other_cfg.once_team_players)
			{
				role->NoticeNum(errornum::EN_RELICLOOTING_MATH_MEMBER_TOO_MUTCH);
				return;
			}
		}
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			if (role->InTeam())
			{
				TeamManager::Instance().OnTeamSwitchToCrossReq(role);
			}
			else
			{
				//原服不加入匹配，直接把玩家传到跨服
				CrossData_Business business_data;
				business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
				business_data.auto_join_type_param1 = FBManager::FB_TYPE_RELIC_LOOTING;
				business_data.param1 = 0;
				business_data.param2 = 0;
				business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
				role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
				return;
			}
		}
		NormalMatchMgr::RoleStartMatchReq(role);
	}
}

void ActivityRelicLootingMatchManager::OnTeamStartMatch(MatchTeam* mt)
{
	//printf("ActivityRelicLootingMatchManager::OnTeamStartMatch\n");
	this->SendTeamStartMatchNotice(mt);
	// 暂时兼容
	/*Protocol::SCRAJoySeeksFavorsInfo msg;
	msg.is_match = Protocol::SCRAJoySeeksFavorsInfo::MATCH_TYPE_ING;
	
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mt->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}*/
}

void ActivityRelicLootingMatchManager::OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("ActivityRelicLootingMatchManager::OnTeamMatchTeam\n");
	this->SendTeamMatchTeamNotice(mtA, mtB);
}

void ActivityRelicLootingMatchManager::OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("ActivityRelicLootingMatchManager::OnTeamMatchEnemy\n");
	this->SendTeamMatchEnemyNotice(mtA, mtB);
	m_activity->OnMatchEnemy(mtA, mtB);
	// 暂时兼容
	/*Protocol::SCRAJoySeeksFavorsMatchSucc msg;
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtA->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}

	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtB->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}*/
}

void ActivityRelicLootingMatchManager::OnUserLeaveTeam(MatchTeam* mt, int uid)
{
	//printf("ActivityRelicLootingMatchManager::OnUserLeaveTeam\n");
	this->SendUserLeaveTeamNotice(mt, uid);
	/*Protocol::SCRAJoySeeksFavorsInfo msg;
	msg.is_match = Protocol::SCRAJoySeeksFavorsInfo::MATCH_TYPE_NO;
	// 暂时兼容
	//Protocol::SCActivityTeamFightNotice msg;
	World::GetInstWorld()->GetSceneManager()->SendToUser(uid, (const char*)&msg, sizeof(msg));*/
}

