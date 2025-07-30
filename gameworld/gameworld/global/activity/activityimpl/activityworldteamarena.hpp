#ifndef __ACTIVITY_WORLD_TEAM_ARENA_HPP__
#define __ACTIVITY_WORLD_TEAM_ARENA_HPP__

#include "global/activity/activity.hpp"

class ActivityWorldTeamArena : public Activity
{
public:
	ActivityWorldTeamArena(ActivityManager *activity_manager);
	~ActivityWorldTeamArena();

	virtual bool IsNeedSave();
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void OnUserLogin(Role *role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnCalcNextSwtichTimeByClose();
	virtual void SetActivityState(const SyncActivityStatus & as);
	virtual void OnUserLoginOriginGame(int role_id);
	virtual void OnConnectCrossServerSucc();

	void OnSetActivityInfo(ActivityWorldTeamArenaParam data);
	void OnHiddenSynActivityInfoToGame(int server_id = 0);

	void GmEnterEndTimeFastforward();
	void GmSetSignUpTime(int day);
	void CmdToRankToSignUp(int season, unsigned int sign_up_end_timestamp);
	void CmdToAdjTime(unsigned int sign_up_start_timestamp, unsigned int sign_up_end_timestamp, unsigned int next_status_switch_timestamp);
	bool IsActivityEnterTime() { return m_is_enter_flag; }
	bool IsCanSignUp();

	void SendActivityInfo(Role * role = NULL);
private:
	void SendEnterInfo(Role * role = NULL);
	virtual void SendActivtyNotify(int notify_status);

	unsigned int m_next_switch_season_timestamp;	//下一次切换赛季时间戳
	unsigned int m_enter_end_timestamp;				//入场时间结束时间戳
	bool m_is_enter_flag;							//是否可以入场

	ActivityWorldTeamArenaParam m_param;
};

#endif