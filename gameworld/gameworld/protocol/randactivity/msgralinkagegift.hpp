#ifndef __MSG_RA_LINKAGE_GIFT_HPP__
#define __MSG_RA_LINKAGE_GIFT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

//////////////////////////////////////////// 2075  随机活动  联动送礼  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_LINKAGE_GIFT_OPERA_TYPE
	{
		RA_LINKAGE_GIFT_OPERA_TYPE_INFO = 0,			//下发信息	
		RA_LINKAGE_GIFT_OPERA_TYPE_FETCH = 1,			//领取奖励
	};


	struct SCRALinkageGiftInfo			//!< 3028	联动送礼信息下发
	{
		SCRALinkageGiftInfo();
		MessageHeader header;

		short seq;							//对应配置
		short fetch_flag;					//领取标识 0 未达成不可领取,1已达成未领取, 2已领取
		unsigned int can_receive_timestamp;	//可领取时间戳
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LINKAGE_GIFT_HPP__
