#ifndef __MSG_RA_LING_PAI_TIAN_CI_HPP__
#define __MSG_RA_LING_PAI_TIAN_CI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  随机活动  定制宝箱  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRALingPaiTianCiInfo						//<! 3810  令牌天赐
	{
	public:
		SCRALingPaiTianCiInfo();
		MessageHeader header;

		int chong_zhi_num;				//当前活动累计充值金额
		unsigned int fetch_flag;			//领取奖励标志
	};

	struct SCRALingPaiTianCi2Info						//<! 3811  无量心法(令牌天赐2)
	{
	public:
		SCRALingPaiTianCi2Info();
		MessageHeader header;

		int chong_zhi_num;				//当前活动累计充值金额
		unsigned int fetch_flag;			//领取奖励标志
	};

	struct SCRALingPaiTianCi3Info						//<! 3812  赋世魂器(令牌天赐3)
	{
	public:
		SCRALingPaiTianCi3Info();
		MessageHeader header;

		int chong_zhi_num;				//当前活动累计充值金额
		unsigned int fetch_flag;			//领取奖励标志
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
