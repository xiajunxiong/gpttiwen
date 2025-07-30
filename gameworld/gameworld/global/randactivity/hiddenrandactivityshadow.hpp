#ifndef __HIDDEN_RAND_ACTIVITY_SHADOW_HPP__
#define __HIDDEN_RAND_ACTIVITY_SHADOW_HPP__

#include "servercommon/servercommon.h"

class RandActivityManager;
struct CrossRandActivityDataParam;
class HiddenRandActivityShadow
{
public:
	typedef std::map<UniqueServerID, RandActivityManager*> RAManagerMap;

public:
	static HiddenRandActivityShadow* Instance();

	RandActivityManager* GetRAManager(const UniqueServerID& usid);
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();

	void OnSyncActivityInfoFromCross(int plat_type, int server_id, int count, CrossRandActivityDataParam * param_list);
private:
	RandActivityManager* CreateRandActivityManager();


	RAManagerMap m_ramgr_map;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(HiddenRandActivityShadow);
};

#endif