#ifndef __RA_WANLING_ZHULI_PARAM_HPP__
#define __RA_WANLING_ZHULI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 万灵筑礼 //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAWanLingZhuLiParam
{
	RAWanLingZhuLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		login_day = 0;
		fetch_reward_mark = 0;
		day_reward_mark = 0;
		is_add_day = 0;
		is_buy_jiagou = 0;
	}

	unsigned int ra_begin_timestamp;

	short login_day;
	short fetch_reward_mark;				// 奖励领取标记

	char day_reward_mark;					// 每日领取标记
	char is_add_day;
	char is_buy_jiagou;						// 是否加购
	char re_ch;
};

UNSTD_STATIC_CHECK(sizeof(RAWanLingZhuLiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_WANLING_ZHULI_PARAM_HPP__
