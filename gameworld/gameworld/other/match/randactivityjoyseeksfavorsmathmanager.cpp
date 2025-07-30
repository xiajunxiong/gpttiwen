#include "randactivityjoyseeksfavorsmathmanager.hpp"
#include "normalmatchlogicimpl.hpp"
#include "servercommon/servercommon.h"
#include "global/randactivity/randactivityimpl/randactivityjoyseeksfavors.hpp"
#include "protocol/msgrandactivity.hpp"
#include "protocol/randactivity/msgrajoyseeksfavors.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/gamelog.h"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "servercommon/string/gameworldstr.h"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjoyseeksfavorsconfig.hpp"

RandActivityJoyseeksfavorsMatchManager::RandActivityJoyseeksfavorsMatchManager(RandactivityJoyseeksfavors* activity)
{
	m_activity = activity;
}

RandActivityJoyseeksfavorsMatchManager::~RandActivityJoyseeksfavorsMatchManager()
{
	
}

void RandActivityJoyseeksfavorsMatchManager::Init()
{
	this->NormalMatchMgr::Init(1, new NormalMatchLogicImpl(), MATCH_BUSINESS_ACTIVITY_JOY_SEEK_PET);
}

void RandActivityJoyseeksfavorsMatchManager::RoleStartMatchReq(Role * role)
{
	//首次请求时载入
	const RandActivityJoySeeksFavorsConfig * config = static_cast<const RandActivityJoySeeksFavorsConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
	if (NULL == config)
	{
		return;
	}

	const JoySeeksFavorsOtherCfg & other_cfg = config->GetOtherCfg();

	time_t now_tamp = EngineAdapter::Instance().Time();
	int next_time_dr1 = EngineAdapter::Instance().NextDayInterval(other_cfg.end_time1 / 100, other_cfg.end_time1 % 100, 0);
	int next_time_dr2 = EngineAdapter::Instance().NextDayInterval(other_cfg.end_time2 / 100, other_cfg.end_time2 % 100, 0);

	time_t end_tamp1 = now_tamp + next_time_dr1;
	time_t end_tamp2 = now_tamp + next_time_dr2;

	if (now_tamp > (end_tamp1 - other_cfg.dr_time) || now_tamp > (end_tamp2 - other_cfg.dr_time))//在可匹配时间内
	{
		NormalMatchMgr::RoleStartMatchReq(role);
		return;
	}

	std::string star1_min;
	std::string end1_min;
	std::string star2_min;
	std::string end2_min;

	star1_min += '0' + (other_cfg.start_time1 % 100) / 10;
	star1_min += '0' + (other_cfg.start_time1 % 100) % 10;

	end1_min += '0' + (other_cfg.end_time1 % 100) / 10;
	end1_min += '0' + (other_cfg.end_time1 % 100) % 10;

	star2_min += '0' + (other_cfg.start_time2 % 100) / 10;
	star2_min += '0' + (other_cfg.start_time2 % 100) % 10;

	end2_min += '0' + (other_cfg.end_time2 % 100) / 10;
	end2_min += '0' + (other_cfg.end_time2 % 100) % 10;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_math_team_fail_content, other_cfg.start_time1 / 100, star1_min.c_str(), other_cfg.end_time1 / 100, end1_min.c_str(),
		other_cfg.start_time2 / 100, star2_min.c_str(), other_cfg.end_time2 / 100, end2_min.c_str());
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
		}
	}
}

void RandActivityJoyseeksfavorsMatchManager::OnTeamStartMatch(MatchTeam* mt)
{
	//printf("RandActivityJoyseeksfavorsMatchManager::OnTeamStartMatch\n");
	//this->SendTeamStartMatchNotice(mt);
	// 暂时兼容
	Protocol::SCRAJoySeeksFavorsInfo msg;
	msg.is_match = Protocol::SCRAJoySeeksFavorsInfo::MATCH_TYPE_ING;
	
	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mt->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}
}

void RandActivityJoyseeksfavorsMatchManager::OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("RandActivityJoyseeksfavorsMatchManager::OnTeamMatchTeam\n");
	//this->SendTeamMatchTeamNotice(mtA, mtB);
}

void RandActivityJoyseeksfavorsMatchManager::OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	//printf("RandActivityJoyseeksfavorsMatchManager::OnTeamMatchEnemy\n");
	//this->SendTeamMatchEnemyNotice(mtA, mtB);
	
	m_activity->OnMatchEnemy(mtA, mtB);


	// 暂时兼容
	Protocol::SCRAJoySeeksFavorsMatchSucc msg;
	for (size_t i = 0; i < mtA->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtA->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}

	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mtB->user_list[i]->uid), (const char*)&msg, sizeof(msg));
	}
}

void RandActivityJoyseeksfavorsMatchManager::OnUserLeaveTeam(MatchTeam* mt, int uid)
{
	//printf("RandActivityJoyseeksfavorsMatchManager::OnUserLeaveTeam\n");
	Protocol::SCRAJoySeeksFavorsInfo msg;
	msg.is_match = Protocol::SCRAJoySeeksFavorsInfo::MATCH_TYPE_NO;
	//this->SendUserLeaveTeamNotice(mt, uid);

	// 暂时兼容
	//Protocol::SCActivityTeamFightNotice msg;
	World::GetInstWorld()->GetSceneManager()->SendToUser(uid, (const char*)&msg, sizeof(msg));
}

