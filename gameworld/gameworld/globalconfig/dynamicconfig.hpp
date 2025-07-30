#ifndef __DYNAMIC_CONFIG_HPP__
#define __DYNAMIC_CONFIG_HPP__

#include <map>
#include <string>

#include "servercommon/activitydef.hpp"

class DynamicConfig
{
public:
	DynamicConfig();
	~DynamicConfig();
	
	bool Init(const std::string &path, std::string *err);

	int GetActivtyNextStatus(int activity_type, int curr_status, int next_open_wday, StatusTimeCfgList *statuestime_list_cfg);
	unsigned int GetActivityNextSwitchTime(int activity_type, int curr_status, int next_open_wday, StatusTimeCfgList *statuestime_list_cfg, bool is_find);
	bool GetActivityNextCfg(int activity_type, int *next_open_wday, StatusTimeCfgList *statuestime_list_cfg, bool *is_find = NULL);
	bool IsActivityOpenDay(int activity_type, int weekday);

	bool GetSpecialActivityTime(int special_activity_type, unsigned int *begin_time, unsigned int *end_time);

	const RandActivityCfg * GetRandActivityCfg(int rand_activity_type);

private:
	ActivityCfg m_activitycfg_list[ACTIVITY_TYPE_MAX];

	SpecialActivityCfg m_special_activitycfg_list[SPECIAL_ACTIVITY_TYPE_MAX];

	RandActivityCfg m_rand_activitycfg_list[RAND_ACTIVITY_TYPE_MAX];
};

#endif // __DYNAMIC_CONFIG_HPP__

