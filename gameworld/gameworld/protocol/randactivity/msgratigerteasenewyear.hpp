#ifndef __MSG_RA_TIGER_TEASE_NEW_YEAR_HPP__
#define __MSG_RA_TIGER_TEASE_NEW_YEAR_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerteasenewyearparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerTeaseNewYearInfo				//!< 3141 随机活动-寅虎逗新春	
	{
	public:
		SCRATigerTeaseNewYearInfo();
		MessageHeader header;

		short times;
		short is_win;		// 胜负,0-普通下发,无胜负，1-胜利，2-失败
	};

}


#pragma pack(pop)

#endif