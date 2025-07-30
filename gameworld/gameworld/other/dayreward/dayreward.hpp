#ifndef __DAY_REWARD_HPP__
#define __DAY_REWARD_HPP__

#include "servercommon/dayrewarddef.hpp"

class RoleModuleManager;
class DayReward
{
public:
	DayReward();
	~DayReward();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const RoleDayRewardParam & param);
	void GetInitParam(RoleDayRewardParam* param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();
	void Update(unsigned int now_second);
	void OnRoleLogin();
	void OnRoleLevelUp(int cur_level);

	int OnFetchOnlineReward(int index);
	int OnFetchSignReward(int day);
	int OnFetchLevelReward(int seq);
	int OnFindReward(int type, int count, int consume_type);
	int OnFetchDayFood(int seq);
	int OnFetchOnlineGift(int seq);	//领取在线礼包
	void OnFindRewardCancelNotify();

	void OnUnlockPetStrengthenReward(int quality, int str_level);
	void OnFetchPetStrengthenReward(int seq);

	int OnOverFolowEnergyFindReward();

	void SendAllInfo();
	void SendFindRewardAndNotifysInfo();
	void SendOnlineRewardInfo();
	void SendSignRewardInfo();
	void SendLevelRewardInfo();
	void SendFindRewardInfo();
	void SendFindNotifysInfo();
	void SendDayFoodInfo();
	void SendPetStrengthenInfo();
	void SendOnlineGiftInfo();
private:
	void OnGuildAnswerFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp);			
	bool OnGuildAnswerFindReward(int consume_type, int count);
	void OnGuildFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp);
	bool OnGuildFightFindReward(int consume_type, int count);
	void OnOnlyFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp);
	bool OnOnlyFightFindReward(int consume_type, int count);
	void OnTeamFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp);
	bool OnTeamFightFindReward(int consume_type, int count);
	void OnSilkRoadsFindDataReset(unsigned int can_find_end_timestamp);
	bool OnSilkRoadsFindReward(int consume_type, int count);

	void OnCheckOnlineGiftOpen();

	int month_zero_open_day;				//当月1号0点计算的开服天数
	RoleModuleManager *m_role_module_mgr;

	RoleDayRewardParam m_param;			
};


#endif // !__DAY_REWARD_HPP__
