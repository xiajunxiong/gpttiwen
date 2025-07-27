#ifndef __RA_CHONG_JI_FAN_LI_PARAM_HPP__
#define __RA_CHONG_JI_FAN_LI_PARAM_HPP__


#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

////////////////////////////////////////////  宠技返利 个人数据  //////////////////////////////////////////////////////

static const int MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM = 64;

struct RAChongJiFanLiParam
{
	RAChongJiFanLiParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_day_meet_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		meet_cond_day_num = 0;
		day_chongzhi_num = 0;
		reward_fetch_flag.Reset();
	}

	void OnDayChange()
	{
		is_day_meet_flag = 0;
		day_chongzhi_num = 0;
	}

	unsigned int ra_begin_timestamp;
	long long reserve_ll[3];

	char is_day_meet_flag;			//当天是否已经满足条件过 0:还未 1:已满足
	char reserve_ch;
	short reserve_sh;
	int meet_cond_day_num;			//满足完成条件天数
	int day_chongzhi_num;
	BitMap<MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM> reward_fetch_flag;

};
UNSTD_STATIC_CHECK(sizeof(RAChongJiFanLiParam) % 4 == 0);
UNSTD_STATIC_CHECK(sizeof(RAChongJiFanLiParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)
#endif