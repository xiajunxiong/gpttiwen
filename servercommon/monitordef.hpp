#ifndef __MONITORDEF_HPP__
#define __MONITORDEF_HPP__

#include "servercommon/servercommon.h"

enum MONITOR_TYPE
{
	MONITOR_TYPE_DAY_GOLD_GET,								// 每日该玩家元宝进账 (非充值手段)
	MONITOR_TYPE_DAY_GOLD_CONSUME,							// 每日该玩家元宝消耗
	MONITOR_TYPE_DAY_COINBIND_GET,							// 每日该玩家铜币进账
	MONITOR_TYPE_DAY_COINBIND_CONSUME,						// 每日该玩家铜币消耗
	MONITOR_TYPE_DAY_IMMORTALCOIN_GET,						// 每日该玩家仙币进账
	MONITOR_TYPE_DAY_IMMORTALCOIN_CONSUME,					// 每日该玩家仙币消耗
	MONITOR_TYPE_FETCH_ATTACHMENT_NUM,						// 领取邮件
	MONITOR_TYPE_MAX,
};

static const int MONITOR_LOG_INTERVAL = 1800;				// 监控日志间隔

#pragma pack(push, 4)

struct MonitorParam
{
	MonitorParam() { this->Reset(); }

	void Reset()
	{
		next_monitor_time = 0;
		memset(monitor_num_list, 0, sizeof(monitor_num_list));
	}

	unsigned int next_monitor_time;
	int monitor_num_list[MONITOR_TYPE_MAX];
};

typedef char MonitorParamHex[sizeof(MonitorParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MonitorParamHex) < 256);

#pragma pack(pop)

#endif	// __MONITORDEF_HPP__
