#ifndef __RA_BO_ZHONG_YOU_LI_PARAM_HPP__
#define __RA_BO_ZHONG_YOU_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  播种有礼 个人数据  //////////////////////////////////////////////////////
static const int BO_ZHONG_YOU_LI_MAX_PLANT_NUM = 4;		// 最大同时种植数量

struct RABoZhongYouLiParam
{
	RABoZhongYouLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		sowing_count = 0;
		receive_sh = 0;
		next_refresh_timestamp = 0;
		memset(planting_timestamp,0,sizeof(planting_timestamp));
	}

	unsigned int ra_begin_timestamp;

	short sowing_count;		// 剩余可播种次数
	short receive_sh;	
	unsigned int next_refresh_timestamp;	// 浇水下次刷新时间戳
	unsigned int planting_timestamp[BO_ZHONG_YOU_LI_MAX_PLANT_NUM];		// 植物种植时间戳
};
UNSTD_STATIC_CHECK(sizeof(RABoZhongYouLiParam) <= sizeof(RoleActivityRoleData))


////////////////////////////////////////////  元气翻牌 个人数据  //////////////////////////////////////////////////////
static const int MAX_YUAN_QI_FAN_PAI_BITMAP = 32;

struct RAYuanQiFanPaiParam
{
	RAYuanQiFanPaiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		lottery_count = 0;
		is_receive_times.Reset();
		is_fetch_day_reward = 0;
	}

	unsigned int ra_begin_timestamp;

	int lottery_count;				// 翻牌次数
	BitMap<MAX_YUAN_QI_FAN_PAI_BITMAP> is_receive_times;	// 是否已领取翻牌次数奖励
	int is_fetch_day_reward;

};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiFanPaiParam) <= sizeof(RoleActivityRoleData))

////////////////////////////////////////////  包子来了 个人数据  //////////////////////////////////////////////////////

struct RABaoZiLaiLeParam
{
	RABaoZiLaiLeParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		join_count = 0;
	}

	void DayReset()
	{
		join_count = 0;
	}

	unsigned int ra_begin_timestamp;

	int join_count;			// 今日已参与次数
};
UNSTD_STATIC_CHECK(sizeof(RABaoZiLaiLeParam) <= sizeof(RoleActivityRoleData))

////////////////////////////////////////////  妙笔生花 个人数据  //////////////////////////////////////////////////////

struct RAMiaoBiShengHuaParam
{
	RAMiaoBiShengHuaParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		count = 0;
	}

	unsigned int ra_begin_timestamp;
	int count;
};
UNSTD_STATIC_CHECK(sizeof(RAMiaoBiShengHuaParam) <= sizeof(RoleActivityRoleData))

////////////////////////////////////////////  乘风破浪 个人数据  //////////////////////////////////////////////////////
static const int MAX_CHENG_FENG_PO_LANG_QUESTION = 30;

struct RAChengFengPoLangParam
{
	RAChengFengPoLangParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cur_question_id = 0;
		answer_times = 0;
		memset(used_question,0,sizeof(used_question));
	}

	void DayReset()
	{
		answer_times = 0;
		cur_question_id = 0;
		memset(used_question, 0, sizeof(used_question));
	}

	unsigned int ra_begin_timestamp;

	int answer_times;		// 今日已答题数量
	int cur_question_id;	// 当前题目id

	short used_question[MAX_CHENG_FENG_PO_LANG_QUESTION];		// 已使用题目
};
UNSTD_STATIC_CHECK(sizeof(RAChengFengPoLangParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)

#endif
