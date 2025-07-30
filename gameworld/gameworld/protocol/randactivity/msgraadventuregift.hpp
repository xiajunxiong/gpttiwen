#ifndef __MSG_RA_ADVEMTURE_GIFT_HPP__
#define __MSG_RA_ADVEMTURE_GIFT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raadventuregiftparam.hpp"

//////////////////////////////////////////// 2074  随机活动  奇遇礼包  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_ADVENTURE_GIFT_OPERA_TYPE
	{
		RA_ADVENTURE_GIFT_OPERA_TYPE_INFO = 0,		//下发信息	
		RA_ADVENTURE_GIFT_OPERA_TYPE_BUY = 1,		//购买礼包	
	};

	struct SCRaAdventureGiftInfo			//<! 3035 奇遇礼包信息
	{
	public:
		SCRaAdventureGiftInfo();
		MessageHeader header;

		int seq;							//当前礼包，配置表
		unsigned int end_time;				//当前时间大于end_time礼包就无效，或end_time==0
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ADVEMTURE_GIFT_HPP__


