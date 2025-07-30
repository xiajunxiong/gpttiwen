#ifndef __MSG_RA_TIGER_FIND_SPRING_HPP__
#define __MSG_RA_TIGER_FIND_SPRING_HPP__

#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerFindSpringInfo			//!< 3140 随机活动-寅虎探春
	{
	public:
		SCRATigerFindSpringInfo();
		MessageHeader header;

		short seq;						// 当日刷新地图索引
		short is_receive_reward;		// 当日是否可领取奖励，0--不可领取，1--可领取，2--已领取
	};

}


#pragma pack(pop)

#endif