#ifndef __SPECIAL_LOGIC_CHIEF_ELECTION_HPP__
#define __SPECIAL_LOGIC_CHIEF_ELECTION_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

class SpecialLogicChiefElection : public SpecialLogic
{
public:
	SpecialLogicChiefElection(Scene* scene);
	virtual ~SpecialLogicChiefElection();

	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanChallengeOthers(Role * role, Role * target);
	virtual bool CanJoinOtherActivity() { return false; }			//!< 是否允许加入其它活动

	void StartRoundFight(int role_id1, int role_id2, int which_prof_match);
	void OnFinish(int role_id, bool is_finish = false);
	void OnActivityClose();
private:
	bool m_start_battle;
	bool m_is_battle_finish;
	int m_which_prof_match;

	std::set<int> m_battle_role_id;
};

#endif