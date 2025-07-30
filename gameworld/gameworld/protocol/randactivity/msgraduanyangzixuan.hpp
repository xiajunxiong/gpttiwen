#ifndef __MSG_RA_DUAN_YANG_ZI_XUAN_HPP__
#define __MSG_RA_DUAN_YANG_ZI_XUAN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2200  随机活动  端阳自选  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRADuanYanZiXuanInfo						//<! 5100 端阳自选信息
	{
	public:
		SCRADuanYanZiXuanInfo();
		MessageHeader header;

		int is_buy_reward;			//是否购买过奖励
		unsigned int fetch_reward_flag;
		unsigned char select_reward_seq[DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM];		//32 领取标志奖励seq
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
