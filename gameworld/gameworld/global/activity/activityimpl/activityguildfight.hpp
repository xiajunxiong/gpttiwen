#ifndef __ACTIVITY_GUILD_FIGHT_HPP__
#define __ACTIVITY_GUILD_FIGHT_HPP__

#include "global/activity/activity.hpp"

class ActivityGuildFight : public Activity
{
public:
	ActivityGuildFight(ActivityManager *activity_manager);
	~ActivityGuildFight();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void OnUserLogin(Role *role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLoginOriginGame(int role_id);			

	bool IsActivityEnterTime() { return m_is_enter_flag; }
	void GmEnterEndTimeFastforward();
private:
	void ClearLastTitle();
	void SendEnterInfo(Role * role = NULL);

	unsigned int m_enter_end_timestamp;		//入场结束时间戳
	bool m_is_enter_flag;					//是否可以入场
};

#endif