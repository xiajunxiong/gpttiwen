#ifndef __RAND_ACTIVITY_MANAGER_HPP__
#define __RAND_ACTIVITY_MANAGER_HPP__

#include "servercommon/activitydef.hpp"

class Role;
class ActivityDataListParam;
class RandActivity;
class RandActivityConsumeCarousel;
class RandAcitivityExpelMonster;
class RandActivityAnimalRace;
class RandActivityAdapter;
class RADelaySetTimeMgr;
class RandActivityManager
{
public:
	friend class RandActivityAdapter;
	friend class HiddenRandActivityShadow;

public:
	void InitActivity(int activity_type, const ActivityData& data);
	void GetChangedActivityData(ActivityDataListParam* list_param);
	void GetActivityData(ActivityDataListParam* list_param);
	void ClearDirtyMark();

	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();

	void OnUserLogin(Role *user);
	void OnUserLogout(Role *user);

	bool IsRandActivityOpen(int rand_activity_type);
	unsigned int GetRandActivityBeginTime(int rand_activity_type);
	unsigned int GetRandActivityEndTime(int rand_activity_type);
	bool IsInRandActivityOpenTime(int rand_activity_type, unsigned int timestamp);

	void OnSetActivityTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int limit_day, int open_type);
	void OnForceSetActivityTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type);
	void OnForceToNextState(int rand_activity_type);
	
	inline void SetUniqueServerID(const UniqueServerID& usid) { m_usid = usid; }
	inline const UniqueServerID& GetUniqueServerID() const { return m_usid; }

	RandActivityConsumeCarousel * GetRAconsumeCarousel();
	RandAcitivityExpelMonster * GetRAExpelMonster();
	RandActivityAnimalRace * GetRAAnimalRace();
	RandActivity * ForceGetRandActivity(int activity_type);
	void SendActivityDataToCross();
	void OnSyncActivityInfoFromCross(int count, CrossRandActivityDataParam * param_list);

	void OnReloadActivityConfig();
private:
	RandActivityManager();
	~RandActivityManager();

	static RandActivityManager* Instance();

	void CalActivityConfigOpenTime(bool is_from_init, const char* call_func);

	RandActivity *m_rand_activity_list[RAND_ACTIVITY_TYPE_MAX];

	bool m_is_first;

	UniqueServerID m_usid;

	RADelaySetTimeMgr* m_delay_set_time_mgr;
};

#endif // __RAND_ACTIVITY_MANAGER_HPP__

