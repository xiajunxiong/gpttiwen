#include "teamkeyregister.hpp"
#include "world.h"
#include "scene/scene.h"
#include "global/team/teammanager.hpp"

TeamKeyRegister::TeamKeyRegister()
{

}

TeamKeyRegister::~TeamKeyRegister()
{
	team_key = 1;
}

void TeamKeyRegister::ClearTeamRegister(int team_key)
{
	for (UserTeamKeyMapIt it = m_user_teamkey_map.begin(); it != m_user_teamkey_map.end();)
	{
		if (it->second == team_key)
		{
			m_user_teamkey_map.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

TeamKeyRegister & TeamKeyRegister::Instance()
{
	static TeamKeyRegister tkr;
	return tkr;
}

void TeamKeyRegister::UserLogin(const UserID& user_id)
{
	Role* user = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == user) return;
	UserTeamKeyMapIt it = m_user_teamkey_map.find(user_id);
	if (it == m_user_teamkey_map.end())return;

	this->BackTeamToManager(it->second);
}

void TeamKeyRegister::RegisterTeamKey(const UserID &user_id, int scene_id, int scene_key, unsigned int create_time)
{
	//m_user_teamkey_map[user_id] = TeamKey(scene_id, scene_key, create_time);
}

void TeamKeyRegister::RegisterTeam(Team * team, int scene_id, int scene_key)
{
	if (NULL == team)return;
	int count = 0;
	do
	{
		if (++team_key >= TEAM_INDEX_MAX) { team_key = 1; }
		if (++count > 100) return;

	} while (m_user_team_map.end() != m_user_team_map.find(team_key));
	TeamKey &node = m_user_team_map[team_key];
	node.create_time = (unsigned int)EngineAdapter::Instance().Time() + 300;
	node.scene_id = scene_id;
	node.scene_key = scene_key;
	node.team = new Team(*team);
	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		TeamMember* member = team->GetMemberInfoByIndex(i);
		if (NULL == member) continue;

		if (member->IsRobot())
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL != robot && robot->GetRealRoleUID() > 0)
			{
				// 分身机器人不保留，需要释放
				continue;
			}
		}
		m_user_teamkey_map[IntToUid(member->uid)] = team_key;
	}
}

Team * TeamKeyRegister::GetTeamRegister(const UserID &user_id, int *scene_id, int *scene_key)
{
	UserTeamKeyMapIt it = m_user_teamkey_map.find(user_id);
	if (it != m_user_teamkey_map.end())
	{
		UserTeamMapIt team_it = m_user_team_map.find(it->second);
		if (team_it != m_user_team_map.end())
		{
			if (scene_id != NULL) *scene_id = team_it->second.scene_id;
			if (scene_key != NULL) *scene_key = team_it->second.scene_key;
			return team_it->second.team;
		}
	}
	return NULL;
}

void TeamKeyRegister::BackTeamToManager(int team_key)
{
	UserTeamMapIt team_it = m_user_team_map.find(team_key);
	if (team_it != m_user_team_map.end())
	{
		TeamManager::Instance().TeamBackFromTeamRegister(team_it->second.team);
		for (int i = 0; i < team_it->second.team->GetMemberCount(); ++i)
		{
			TeamMember* member = team_it->second.team->GetMemberInfoByIndex(i);
			if (NULL == member) continue;
			member->uid = 0;
		}
		m_user_team_map.erase(team_it);
		this->ClearTeamRegister(team_key);
	}
}

void TeamKeyRegister::Update(unsigned long interval, unsigned int now_second)
{
	for (UserTeamMapIt it = m_user_team_map.begin();it != m_user_team_map.end();)
	{
		if (it->second.create_time <= now_second)
		{
			for (int i = 0; i < it->second.team->GetMemberCount(); ++i)
			{
				TeamMember* member = it->second.team->GetMemberInfoByIndex(i);
				if (NULL == member) continue;
				member->uid = 0;
			}
			m_user_team_map.erase(it++);
			this->ClearTeamRegister(team_key);
		}
		else
		{
			it++;
		}
	}
}

void TeamKeyRegister::ClearTeamRegisterByRoleLoginHidden(const UserID & user_id)
{
	UserTeamKeyMap::iterator it = m_user_teamkey_map.find(user_id);
	if (it != m_user_teamkey_map.end())
	{
		this->ClearTeamRegister(it->second);
	}
}