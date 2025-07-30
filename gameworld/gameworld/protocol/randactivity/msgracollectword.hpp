#ifndef __MSG_RA_COLLECT_WORD_HPP__
#define __MSG_RA_COLLECT_WORD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/racollectwordparam.hpp"

//////////////////////////////////////////// 2068  随机活动  集字活动  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_COLLECT_WORD_OPERA_TYPE
	{
		RA_COLLECT_WORD_OPERA_TYPE_ROLE_INFO = 0,		// 玩家信息	
		RA_COLLECT_WORD_OPERA_TYPE_EXCHANGE = 1,		// 兑换物品 param_1 : 配置表里的seq
		RA_COLLECT_WORD_OPERA_TYPE_DAY_FREE = 2,		// 领取每日免费礼包
	};


	struct SCRaCollectWordInfo			//<! 3029 集字好礼信息
	{
	public:
		SCRaCollectWordInfo();
		MessageHeader header;

		short day_gift_is_fetch;			//集字有礼每日免费礼包 0:未领取 1:已领取
		short reserve_sh;
		unsigned short exchange_times[RA_COLLECT_WORD_MAX_ENTRY_NUM];
		unsigned short total_exchange_times[RA_COLLECT_WORD_MAX_ENTRY_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_COLLECT_WORD_HPP__


