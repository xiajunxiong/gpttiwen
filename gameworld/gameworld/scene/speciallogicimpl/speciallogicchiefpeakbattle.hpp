#ifndef __SPECIAL_LOGIC_CHIEF_PEAK_BATTLE_HPP__
#define __SPECIAL_LOGIC_CHIEF_PEAK_BATTLE_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

class SpecialLogicChiefPeakBattle: public SpecialLogic
{
public:
	SpecialLogicChiefPeakBattle(Scene* scene);
	virtual ~SpecialLogicChiefPeakBattle();

	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy();
	virtual void Update(unsigned long interval, time_t now_second);

	void StartRoundFight(int role_id1, int role_id2);
	void OnFinish(int role_id, bool is_finish = false);
	void OnActivityClose();
private:
	bool m_start_battle;
	bool m_is_battle_finish;

	std::set<int> m_battle_role_id;
};

#endif