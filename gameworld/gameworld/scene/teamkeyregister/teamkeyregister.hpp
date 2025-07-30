#ifndef __TEAM_KEY_REGISTER_HPP__
#define __TEAM_KEY_REGISTER_HPP__

#include "servercommon/serverdef.h"
#include "global/team/team.hpp"
#include <map>
class Team;
class TeamKeyRegister
{
public:
	static TeamKeyRegister & Instance();
	void UserLogin(const UserID& user_id);
	void RegisterTeamKey(const UserID &user_id, int scene_id, int scene_key, unsigned int create_time);

	void RegisterTeam(Team * team, int scene_id, int scene_key);//最后队长都掉线了，队伍信息注册到这里
	Team * GetTeamRegister(const UserID &user_id, int *scene_id,int *scene_key);
	void BackTeamToManager(int team_key);//把队伍信息转移回teammanager
	void Update(unsigned long interval, unsigned int now_second);
	void ClearTeamRegisterByRoleLoginHidden(const UserID &user_id);
private:
	TeamKeyRegister();
	~TeamKeyRegister();
	void ClearTeamRegister(int team_key);

	struct TeamKey
	{
		TeamKey() : scene_id(0), scene_key(0), create_time(0) {}
		TeamKey(int _scene_id, int _scene_key, int _create_time) : scene_id(_scene_id), scene_key(_scene_key), create_time(_create_time) {}

		int scene_id;
		int scene_key;
		unsigned int create_time;
		Team* team;
	};
	typedef std::map<UserID, int> UserTeamKeyMap;//队伍成员指向team_key
	typedef std::map<UserID, int>::iterator UserTeamKeyMapIt;
	typedef std::map<int, TeamKey> UserTeamMap;
	typedef std::map<int, TeamKey>::iterator UserTeamMapIt;

	int team_key;
	UserTeamKeyMap m_user_teamkey_map;
	UserTeamMap m_user_team_map;
};

#endif // __TEAM_KEY_REGISTER_HPP__

