#ifndef __MSG_RA_TIGER_AUSPICIOUS_SNOW_HPP__
#define __MSG_RA_TIGER_AUSPICIOUS_SNOW_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerSpiciousSnowInfo				//!< 3147 随机活动-瑞雪丰年 信息下发
	{
	public:
		SCRATigerSpiciousSnowInfo();
		MessageHeader header;

		short times;
		short seq;		// 抽奖物品seq
	};

}

#pragma pack(pop)

#endif
