#ifndef __MSG_RA_YUAN_QI_YAN_HUO_HPP__
#define __MSG_RA_YUAN_QI_YAN_HUO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2195  随机活动  元气烟火  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAYuanQiYanHuo		//<! 4800 元气烟火信息
	{
	public:
		SCRAYuanQiYanHuo();
		MessageHeader header;

		int is_buy;
		int is_fetch_free_reward;
		unsigned int get_reward_flag;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
