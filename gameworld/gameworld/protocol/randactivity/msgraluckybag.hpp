#ifndef __MSG_RA_LUCKY_BAG_HPP__
#define __MSG_RA_LUCKY_BAG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckybagparam.hpp"

//////////////////////////////////////////// 2062  随机活动  福袋  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_LUCKY_BAG_OPERA_TYPE
	{
		RA_LUCKY_BAG_OPERA_TYPE_GET_INFO = 0,				//获取信息
		RA_LUCKY_BAG_OPERA_TYPE_FETCH_DAY_REWARD = 1,		//领取每日奖励
	};

	struct SCRALuckyBagInfo						//<! 3020 福袋信息
	{
	public:
		SCRALuckyBagInfo();
		MessageHeader header;

		int ra_lucky_bag_buy_flag;				//福袋每天买的挡位
		int ra_lucky_bag_is_fetch_reward;		//是否领取每日奖励
		unsigned int batch_buy_end_time;		//一键购买结束时间
		unsigned int batch_buy_last_time;		//一键购买上次领取奖励时间
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
