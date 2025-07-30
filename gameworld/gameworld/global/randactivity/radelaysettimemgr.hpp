#ifndef __RA_DELAY_SET_TIME_MGR_HPP__
#define __RA_DELAY_SET_TIME_MGR_HPP__

#include <map>
#include "servercommon/servercommon.h"
#include "global/commonsave/commonsavemanager.hpp"
#include "config/activityconfig/randactivityopencfg.hpp"

struct RASetTimeItem
{
	RASetTimeItem() : is_force(false), rand_activty_type(0), 
		begin_time(0), end_time(0), open_type(0), delay_to_time(0) {}

	bool is_force;
	int rand_activty_type;
	unsigned int begin_time;
	unsigned int end_time;
	int open_type;
	unsigned int delay_to_time; // 延时到何时设置时间
};

class RandActivityManager;
class RADelaySetTimeMgr : public ICommonSaveLoadSuccCallback
{
public:
	RADelaySetTimeMgr(RandActivityManager* ra_activity_mgr);
	virtual ~RADelaySetTimeMgr();

	void CheckDelaySetTime(unsigned int now_second);
	bool AddDelaySetTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type, unsigned int delay_to_time, const char* caller, bool is_init);
	bool AddDelayForceSetTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type, unsigned int delay_to_time, const char* caller, bool is_init);
	bool AddOpenCfg(const RandActivityOpenDetail& cfg, const char* call_func);

	virtual void Callback();
private:
	void OnCommonSaveLoadSucc();
	void RemoveFromCommonSave(const RASetTimeItem& item);
	void AddToCommonSave(const RASetTimeItem& item);
	CommonSaveKey GetCommonSaveKey(const RASetTimeItem& item);
	void AssembleCommonSaveData(const RASetTimeItem& item, ARG_OUT CommonSaveData& data);

	void CheckRADelaySettimeMap(unsigned int now_second);
	void CheckRADelayForceSettimeMap(unsigned int now_second);
	void CheckRAOpenCfgMap(unsigned int now_second);

	RandActivityManager* m_ra_activity_mgr;

	typedef std::map<long long, RASetTimeItem> RASetTimeItemMap;
	typedef RASetTimeItemMap::iterator RASetTimeItemIt;
	RASetTimeItemMap m_ra_delay_settime_map;			// OnSetActivityTime函数延时设置时间列表
	RASetTimeItemMap m_ra_delay_force_settime_map;		// OnForceSetActivityTime函数延时设置时间列表

	typedef std::map<long long, RandActivityOpenDetail> RAOpenCfgMap;
	RAOpenCfgMap m_ra_open_cfg_map;
};


#endif