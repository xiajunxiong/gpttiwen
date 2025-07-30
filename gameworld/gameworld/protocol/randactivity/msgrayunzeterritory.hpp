#ifndef __MSG_RA_YUN_ZE_TERRITORY_HPP__
#define __MSG_RA_YUN_ZE_TERRITORY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzeterritoryparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{


	class SCRAYunZeTerritoryInfo		//!< 3114 随机活动-云泽画境信息下发
	{
	public:
		SCRAYunZeTerritoryInfo();
		MessageHeader header;

		int linght_up_length;
		int light_up[PICTURE_FRAGMENTS_NUM];
		int times;
	};

}

#pragma pack(pop)

#endif