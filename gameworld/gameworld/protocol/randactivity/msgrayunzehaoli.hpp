#ifndef __MSG_RA_YUN_ZE_HAO_LI_HPP__
#define __MSG_RA_YUN_ZE_HAO_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"



#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeHaoLiInfo		//!< 3133 随机活动-云泽好礼信息下发
	{
	public:
		SCRAYunZeHaoLiInfo();
		MessageHeader header;

		short buy_times;			// 已购买次数
		short prop_free_num;		// 银锤子数量
	};

}


#pragma pack(pop)

#endif
