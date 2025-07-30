#include "activityteamfightmatchmanager.hpp"
#include "servercommon/servercommon.h"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "activityteamfightmatchlogicimpl.hpp"

#include "protocol/msgactivity.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

ActivityTeamFightMatchManager::ActivityTeamFightMatchManager(ActivityTeamFight* activity)
{
	m_activity = activity;
}

ActivityTeamFightMatchManager::~ActivityTeamFightMatchManager()
{
	
}

void ActivityTeamFightMatchManager::Init()
{
#ifdef _DEBUG
	this->NormalMatchMgr::Init(1, new ActivityTeamFightMatchLogicImpl(), MATCH_BUSINESS_ACTIVITY_TEAM_FIGHT);
#else
	this->NormalMatchMgr::Init(TUAN_DUI_JING_JI_MAX_MEMBER_NUM, new ActivityTeamFightMatchLogicImpl(), MATCH_BUSINESS_ACTIVITY_TEAM_FIGHT);
#endif
}

void ActivityTeamFightMatchManager::OnTeamStartMatch(MatchTeam* mt)
{
	//printf("ActivityTeamFightMatchManager::OnTeamStartMatch\n");
	this->SendTeamStartMatchNotice(mt);

	// ‘› ±ºÊ»›
	//Protocol::SCActivityTeamFightNotice msg;
	//msg.is_match = Protocol::SCActivityTeamFightNotice::MATCH_TYPE_ING;
	
	//for (size_t i = 0; i < mt->user_list.size(); ++i)
	//{
	//	World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mt->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	//}
}

void ActivityTeamFightMatchManager::OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("ActivityTeamFightMatchManager::OnTeamMatchTeam\n");
	this->SendTeamMatchTeamNotice(mtA, mtB);
}

void ActivityTeamFightMatchManager::OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("ActivityTeamFightMatchManager::OnTeamMatchEnemy\n");
	this->SendTeamMatchEnemyNotice(mtA, mtB);
	
	m_activity->OnMatchEnemy(mtA, mtB);


	// ‘› ±ºÊ»›
	//Protocol::SCActivityTeamFightMatchSucc msg;
	//for (size_t i = 0; i < mtA->user_list.size(); ++i)
	//{
	//	World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtA->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	//}

	//for (size_t i = 0; i < mtB->user_list.size(); ++i)
	//{
	//	World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtB->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	//}
}

void ActivityTeamFightMatchManager::OnUserLeaveTeam(MatchTeam* mt, int uid)
{
	//printf("ActivityTeamFightMatchManager::OnUserLeaveTeam\n");
	this->SendUserLeaveTeamNotice(mt, uid);

	// ‘› ±ºÊ»›
	//Protocol::SCActivityTeamFightNotice msg;
	//msg.is_match = Protocol::SCActivityTeamFightNotice::MATCH_TYPE_NO;
	//World::GetInstWorld()->GetSceneManager()->SendToUser(uid, (const char*)&msg, sizeof(msg));
}

