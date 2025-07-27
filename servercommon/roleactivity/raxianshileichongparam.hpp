#ifndef __RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__
#define __RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2169 随机活动 - 限时累充  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAXianShiLeiChongParam
{
	RAXianShiLeiChongParam() { this->Reset(); }
	void Reset()
	{
		last_timestamp = 0;
		can_fetch_flag = 0;
		flag = 0;
		total_gold = 0;
	}

	unsigned int last_timestamp;		// 上一次参与活动的时间戳
	int can_fetch_flag;					// 能领取的标记
	int flag;							// 已领取标记
	int total_gold;						// 累计充值的金额
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiLeiChongParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	//__RA_XIAN_SHI_LEI_CHONG_PARAM_HPP__
