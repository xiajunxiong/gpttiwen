#ifndef __MSG_RA_CAI_SHEN_JU_BAO_HPP__
#define __MSG_RA_CAI_SHEN_JU_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2062  随机活动  福袋  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRACaiShenJuBaoInfo						//<! 3380 财神聚宝信息
	{
	public:
		SCRACaiShenJuBaoInfo();
		MessageHeader header;

		unsigned short ra_cai_shen_ju_bao_buy_day_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];			//每种物品每天限购次数
		unsigned short ra_cai_shen_ju_bao_buy_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];				//每种物品限购次数
		BitMap<RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM> buy_flag;										//购买标识  0:未购买 1:已购买
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
