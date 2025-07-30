#ifndef __MSG_RA_PARTNER_EXCHANGE_HPP__
#define __MSG_RA_PARTNER_EXCHANGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"

//////////////////////////////////////////// 2070  随机活动  伙伴兑换  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_PARTNER_EXCHANGE_OPERA_TYPE
	{
		RA_PARTNER_EXCHANGE_OPERA_TYPE_GET_INFO = 0,		//获取信息
		RA_PARTNER_EXCHANGE_OPERA_TYPE_EXCHANGE = 1,		//领取每日奖励
	};

	class SCRAActivityPartnerExchangeInfo			//!< 1233: 伙伴兑换活动下发
	{
	public:
		SCRAActivityPartnerExchangeInfo();
		MessageHeader header;

		unsigned short ra_partner_exchange_times[MAX_RA_PARTNER_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_PARTNER_EXCHANGE_HPP__
