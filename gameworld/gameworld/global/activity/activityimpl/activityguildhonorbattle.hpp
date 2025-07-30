#ifndef __ACTIVITY_GUILD_HONOR_BATTLE_HPP__
#define __ACTIVITY_GUILD_HONOR_BATTLE_HPP__

#include "global/activity/activity.hpp"
#include <vector>
#include <map>
#include <set>

class Role;

class ActivityGuildHonorBattle : public Activity
{
public:
	ActivityGuildHonorBattle(ActivityManager *activity_manager);
	~ActivityGuildHonorBattle();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLogin(Role *role);
	void GoToBattleScene(Role * role);

	void GmOperate(int type);
private:
	void OnActivityClose();
};

#endif