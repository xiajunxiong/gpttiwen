#ifndef __ACTIVITY_ONLY_FIGHT_HPP__
#define __ACTIVITY_ONLY_FIGHT_HPP__

#include "global/activity/activity.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"

class ActivityOnlyFight : public Activity
{
public:
	ActivityOnlyFight(ActivityManager *activity_manager);
	~ActivityOnlyFight();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void OnUserLogin(Role *role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnConnectCrossServerSucc();
	virtual void SetActivityState(const SyncActivityStatus & as);
	virtual void OnUserLoginOriginGame(int role_id);

	bool IsActivityEnterTime() { return m_is_enter_flag; }
	void GmEnterEndTimeFastforward();
private:
	void  SendEnterInfo(Role * role = NULL);

	unsigned int m_enter_end_timestamp;			//入场时间结束时间戳
	bool m_is_enter_flag;						//是否可以入场
};	

#endif