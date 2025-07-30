#ifndef __TEAM_VOTE_HPP__
#define __TEAM_VOTE_HPP__

#include <map>
#include "servercommon/servercommon.h"

enum TEAM_VOTE
{
	TEAM_VOTE_INVALID = 0,
	TEAM_VOTE_NO = 1,
	TEAM_VOTE_YES = 2,
};

class Team;
class Role;
class TeamVote
{
public:
	typedef std::map<int, int> VoteMap;

public:
	TeamVote();
	~TeamVote();

	void Update(unsigned long interval, time_t now_second);
	void SetTeam(Team* team) { m_team = team; }
	void StartVote(Role* campaigner);
	bool IsVoting() const { return m_campaigner_uid > 0; }
	int GetCampainerUID() const { return m_campaigner_uid; }
	void EndVote(bool result);
	void OnRoleVote(Role* role, bool is_yes);

private:
	Team* m_team;
	unsigned int m_vote_end_timestamp;
	int m_campaigner_uid;
	VoteMap m_vote_map;

	int m_yes_num;
	int m_no_num;
};


#endif