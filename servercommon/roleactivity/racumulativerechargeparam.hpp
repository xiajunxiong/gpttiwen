#ifndef __RA_CUMULATIVE_RECHARGE_PARAM_HPP__
#define __RA_CUMULATIVE_RECHARGE_PARAM_HPP__

#include "servercommon/servercommon.h"

//////////////////////////////////////////// 2100 随机活动 - 累计充值  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RACumulativeChargeParam
{
	RACumulativeChargeParam() { this->Reset(); }

	void Reset()
	{
		ra_cumulative_charge_begin_timestamp = 0;
		has_get_reward = 0;
		acc_price = 0;
		is_received = 0;
	}

	unsigned int ra_cumulative_charge_begin_timestamp;
	int has_get_reward;
	long long acc_price;
	int is_received;
};

UNSTD_STATIC_CHECK(sizeof(RACumulativeChargeParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_GIVE_SECRET_KEY_PARAM_HPP__
