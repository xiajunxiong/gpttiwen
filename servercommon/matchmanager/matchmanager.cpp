#include "matchmanager.hpp"
#include "matchdef.hpp"
#include "servercommon/servercommon.h"
#include "matchengine.hpp"

MatchManager::MatchManager() : m_engine(NULL), m_has_init(false)
{
	
}


MatchManager::~MatchManager()
{
	if (NULL != m_engine)
	{
		delete m_engine;
		return;
	}
}

void MatchManager::Init(MatchEngine* match_engine, MatchManagerInitParam& param)
{
	if (m_has_init)
	{
		return;
	}

	m_has_init = true;
	m_engine = match_engine;

	MatchEngineInitParam engine_param;
	engine_param.full_team_match_interval_ms = param.full_team_match_interval_ms;
	engine_param.business_type = param.business_type;
	engine_param.unfull_team_match_interval_ms = param.unfull_team_match_interval_ms;
	engine_param.team_member_num = param.team_member_num;
	engine_param.logic_ptr = param.logic_ptr;

	m_engine->Init(this, engine_param);
}

bool MatchManager::OnUserStartMatchReq(int user_num, int logic_type, MatchUser** user_list)
{
	if (!m_has_init) return false;
	
	return m_engine->OnUserStartMatchReq(user_num, logic_type, user_list);
}

bool MatchManager::OnUserStopMatchReq(int user_num, int* uid_list)
{
	if (!m_has_init) return false;

	return m_engine->OnUserStopMatchReq(user_num, uid_list);
}

void MatchManager::Update(unsigned long interval_ms)
{
	if (!m_has_init)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	m_engine->Update(interval_ms);
}

void MatchManager::Reset()
{
	if (!m_has_init) return;

	m_engine->Reset();
}

void MatchManager::OnTeamStartMatch(MatchTeam* mt)
{
	printf("OnTeamStartMatch\n");
}

void MatchManager::OnTeamMatchTeam(MatchTeam* mtA, MatchTeam* mtB)
{
	printf("OnTeamMatchTeam\n");
}

void MatchManager::OnTeamMatchEnemy(MatchTeam* mtA, MatchTeam* mtB)
{
	printf("OnTeamMatchEnemy\n");
}

void MatchManager::OnUserLeaveTeam(MatchTeam* mt, int uid)
{
	printf("OnUserLeaveTeam\n");
}
