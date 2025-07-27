#ifndef __RA_LUCKY_BAG_PARAM_HPP__
#define __RA_LUCKY_BAG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2062 随机活动 - 福袋  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct LuckyBagBatchBuyItem
{
	LuckyBagBatchBuyItem() { this->Reset(); }
	void Reset()
	{
		end_time = 0;
		last_send_reward_time = 0;
	}


	unsigned int end_time;
	unsigned int last_send_reward_time;	
};

struct RALuckyBagParam
{
	RALuckyBagParam() { this->Reset(); }
	void Reset()
	{
		ra_lucky_bag_begin_timestamp = 0;
		ra_lucky_bag_buy_flag = 0;
		ra_lucky_bag_is_fetch_reward = 0;

		batch_buy_info.Reset();
	}

	unsigned int ra_lucky_bag_begin_timestamp;
	int ra_lucky_bag_buy_flag;				//福袋每天买的挡位
	int ra_lucky_bag_is_fetch_reward;		//是否领取每日奖励
	LuckyBagBatchBuyItem batch_buy_info;  //一键购买多日日礼包后，首日奖励立即发放（弹出奖励获取框），次日及之后每日0点通过邮件发放奖励
};

UNSTD_STATIC_CHECK(sizeof(RALuckyBagParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_BAG_PARAM_HPP__
