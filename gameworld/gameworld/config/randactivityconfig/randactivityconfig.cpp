#include "randactivityconfig.hpp"

#include "servercommon/rolecommonsavedef.h"
#include "engineadapter.h"
#include "global/randactivity/randactivitymanager.hpp"

#include "world.h"

RandActivityConfig::RandActivityConfig()
{

}

RandActivityConfig::~RandActivityConfig()
{

}

int RandActivityConfig::GetActOpenDay(RandActivityManager* ramgr, int activity_type, bool can_negative) const
{
	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	int act_real_open_day = GetDayIndex(activity_begin_time, cur_time) + 1; // 活动开启天数，1~N

	if (!can_negative && act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityConfig::GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int act_real_open_day = GetDayIndex(open_server_timestamp, activity_begin_time) + 1; // 活动开启时的开服天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityConfig::GetCurRealOpenDay(RandActivityManager* ramgr) const
{
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int cur_real_open_day = GetDayIndex(open_server_timestamp, cur_time) + 1; // 开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	return cur_real_open_day;
}

int RandActivityConfig::GetOpenDayByTimestamp(RandActivityManager* ramgr, unsigned int timestamp) const
{
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int cur_real_open_day = GetDayIndex(open_server_timestamp, timestamp) + 1; // 活动开启时的开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	return cur_real_open_day;
}

int RandActivityConfig::GetCurRealEndDay(RandActivityManager * ramgr, int activity_type) const
{
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	time_t activity_end_time = ramgr->GetRandActivityEndTime(activity_type);
	int cur_real_end_day = GetDayIndexByEndTime(activity_end_time, cur_time) + 1;		//活动结束天数 1~N

	if (cur_real_end_day <= 0)
	{
		cur_real_end_day = 1;
	}

	return cur_real_end_day;
}

int RandActivityConfig::GetConfigVer() const
{
	return 0;
}
