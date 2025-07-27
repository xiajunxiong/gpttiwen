#ifndef __RA_YUAN_QI_JISHIBU_PARAM_HPP__
#define __RA_YUAN_QI_JISHIBU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 元气记事簿 //////////////////////////////////////////
static const int MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT = 10;


enum RA_YUAN_QI_JISHIBU_TASK_TYPE
{
	RA_YUAN_QI_JISHIBU_TASK_TYPE_LOGIN = 1,						// 每日登陆	
	RA_YUAN_QI_JISHIBU_TASK_TYPE_ACTIVE = 2,					// 活跃达到 X 点
	RA_YUAN_QI_JISHIBU_TASK_TYPE_VITALITY_HELPS = 3,			// 元气助人 X 次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_KONGMINDENG = 4,				// 点亮孔明灯X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_BAOZI_LAILE = 5,				// 包子来了 通关X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_CHENFENG_POLANG = 6,			// 乘风破浪 答题对X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_BOZHONG_YOULI = 7,				// 播种有礼 熟成X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_YUANQI_JINGSU = 8,				// 元气竞速 通关X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_MIAOBI_SHENGHUA = 9,			// 妙笔生花 完成X次
	RA_YUAN_QI_JISHIBU_TASK_TYPE_XUNMENG_HUDIE = 10,			// 寻梦蝴蝶 完成X次

	RA_YUAN_QI_JISHIBU_TASK_TYPE_MAX
};


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAYuanQiJiShiBuParam
{
	RAYuanQiJiShiBuParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		combat = 0;
		is_jinjie_jiangli = 0;
		re_ch = 0;
		now_day_id = 0;

		quanming_reward_fetch_flag = 0;
		jinjie_reward_fetch_flag = 0;

		complete_flag = 0;
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	short combat;							// 获得的劳作值, 不清空
	char is_jinjie_jiangli;					// 是否购买进阶奖励
	char re_ch;
	int now_day_id;

	int quanming_reward_fetch_flag;			// 免费奖励	
	int jinjie_reward_fetch_flag;			// 进阶奖励
	int complete_flag;					// 任务完成情况 按位标记
	unsigned short task_list[MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT];		// 任务进度
};

UNSTD_STATIC_CHECK(sizeof(RAYuanQiJiShiBuParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_YUAN_QI_JISHIBU_PARAM_HPP__
