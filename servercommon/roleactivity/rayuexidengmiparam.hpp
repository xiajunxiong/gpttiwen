#ifndef __RA_ACTIVITY_YUE_XI_DENG_MI_PARAM_HPP__
#define __RA_ACTIVITY_YUE_XI_DENG_MI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


#pragma pack(push,4)

////////////////////////////////// 月夕灯谜 ///////////////////////////////////////
static const int MAX_RA_YXDM_USED_QUESTION_COUNT = 5;	// 月夕灯谜最大已使用题目

struct RAYueXiDengMiParam
{
	RAYueXiDengMiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cur_question_id = 0;
		today_answer_count = 0;
		memset(used_question, 0, sizeof(used_question));
	}

	void DayReset()
	{
		cur_question_id = 0;
		today_answer_count = 0;
		memset(used_question, 0, sizeof(used_question));
	}


	unsigned int ra_begin_timestamp;
	int cur_question_id;		// 当前题目id
	int today_answer_count;		// 当日答题次数
	int used_question[MAX_RA_YXDM_USED_QUESTION_COUNT];
};
UNSTD_STATIC_CHECK(sizeof(RAYueXiDengMiParam) <= sizeof(RoleActivityRoleData));

////////////////////////////////// 月夕累充 ///////////////////////////////////////
static const int MAX_RA_YUE_XI_LEI_CHONG_BITMAP = 32;

struct RAYueXiLeiChongParam
{
	RAYueXiLeiChongParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		m_total_chongzhi = 0;
		m_receive_reward_flag.Reset();
	}


	unsigned int ra_begin_timestamp;
	int m_total_chongzhi;		// 活动期间总充值金额
	BitMap<MAX_RA_YUE_XI_LEI_CHONG_BITMAP> m_receive_reward_flag;		// 奖励领取flag
};
UNSTD_STATIC_CHECK(sizeof(RAYueXiLeiChongParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)
#endif