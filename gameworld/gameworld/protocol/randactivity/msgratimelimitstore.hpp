#ifndef __MSG_RA_TIME_LIMIT_STORE_HPP__
#define __MSG_RA_TIME_LIMIT_STORE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratimelimitstoreparam.hpp"

//////////////////////////////////////////// 2049 随机活动  限时商店  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_TIME_LIMIT_STORE_OPERA_TYPE
	{
		RA_TIME_LIMIT_STORE_OPERA_TYPE_GET_INFO = 0,			// 获取信息
		RA_TIME_LIMIT_STORE_OPERA_TYPE_BUY = 1,					// 购买 param_1: 购买的商品序号  param_2: 购买的数量 					
	};

	class SCRandActivityTimeLimitStoreInfo						//!<3001  获取信息
	{
	public:
		SCRandActivityTimeLimitStoreInfo();
		MessageHeader header;

		unsigned short ra_time_limit_store_buy_count[RA_TIME_LIMIT_STORE_MAX_NUM];		//!< RA_TIME_LIMIT_STORE_MAX_NUM:12
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TIME_LIMIT_STORE_HPP__
