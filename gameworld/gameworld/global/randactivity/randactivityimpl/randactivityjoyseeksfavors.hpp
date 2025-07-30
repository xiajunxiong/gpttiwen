#ifndef __RAND_ACTIVITY_JOY_SEEKS_FAVORS_HPP__
#define __RAND_ACTIVITY_JOY_SEEKS_FAVORS_HPP__
#include <vector>
#include "global/randactivity/randactivity.hpp"
#include "other/match/randactivityjoyseeksfavorsmathmanager.hpp"

class Role;
struct MatchTeam;
class RandactivityJoyseeksfavors : public RandActivity
{
public:
	RandactivityJoyseeksfavors(RandActivityManager *activity_manager);
	~RandactivityJoyseeksfavors();

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnMonthChange();

	void OnMatchReq(Role* role);
	void OnCancelReq(Role* role);

	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);

	void OnMatchEnemy(MatchTeam* mtA, MatchTeam* mtB);
private:
	MatchLogic m_match_logic;

	time_t m_next_match_time;

	std::vector<ActFightGroup> fight_group;

	RandActivityJoyseeksfavorsMatchManager* m_match_mgr;
};

#endif // !__ACTIVITY_TEAM_FIGHT_HPP__
