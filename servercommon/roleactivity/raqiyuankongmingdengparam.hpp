#ifndef __RA_QIYUAN_KONGMINGDENG_PARAM_HPP__
#define __RA_QIYUAN_KONGMINGDENG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 祈愿孔明灯 //////////////////////////////////////////
static const int MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT = 10;


enum RA_QIYUAN_KONGMINGDENG_TASK_TYPE
{
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_LOGIN = 1,						// 每日登陆	
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_ACTIVE = 2,					// 活跃达到 X 点
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_VITALITY_HELPS = 3,			// 元气助人 X 次

	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_MAX
};


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAQiYuanKongMingDengParam
{
	RAQiYuanKongMingDengParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		huozhong = 0;
		now_day_id = 0;

		complete_flag = 0;
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	int huozhong;				// 获得的火种数量, 不清空
	int now_day_id;
	
	int complete_flag;					// 任务完成情况 按位标记
	unsigned short task_list[MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT];		// 任务进度
};

UNSTD_STATIC_CHECK(sizeof(RAQiYuanKongMingDengParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_QIYUAN_KONGMINGDENG_PARAM_HPP__
