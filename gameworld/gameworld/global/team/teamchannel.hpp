#ifndef __TEAM_CHANNEL_HPP__
#define __TEAM_CHANNEL_HPP__

#include "servercommon/teamdef.hpp"

class TeamManager;
class Team;
class Role;
class TeamChannel
{
public:
	struct TeamOrder
	{
		TeamOrder() : team_index(0), is_cross(0), create_time(0u) {}

		friend bool operator< (const TeamOrder& a, const TeamOrder& b)
		{
			if (a.create_time != b.create_time) return a.create_time < b.create_time;
			
			if (a.is_cross != b.is_cross) return a.is_cross < b.is_cross;
			
			return a.team_index < b.team_index;
		}

		int team_index;
		short is_cross;
		unsigned int create_time;
	};

	typedef std::map<TeamOrder, TeamItem> TeamItemMap;

public:
	TeamChannel();
	~TeamChannel();

	void OnTeamCreate(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item);
	void OnTeamCreate(int team_index, bool is_cross, unsigned int create_time, Team* team);
	void OnTeamInfoChange(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item, int team_check_type);
	void OnTeamInfoChange(Team* team);
	void OnTeamDismiss(int team_index, bool is_cross, unsigned int create_time, const TeamItem& team_item);
	void OnTeamDismiss(int team_index, bool is_cross, unsigned int create_time, Team* team);

	void OnTeamEnterSpecialScene(Team* team);
	void OnTeamEnterNormalScene(Team* team);

	void SendTeamList(Role* role);

private:
	const TeamItem* GetTeamItem(int team_index, bool is_cross, unsigned int create_time);
	void SendTeamInfoChange(const TeamItem& team_item, short reason);
	static TeamOrder GetTeamOrder(int team_index, bool is_cross, unsigned int create_time);

	TeamItemMap m_team_map;
};

#endif