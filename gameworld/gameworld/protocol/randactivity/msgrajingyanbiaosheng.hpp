#ifndef __MSG_RA_JING_YAN_BIAO_SHENG_HPP__
#define __MSG_RA_JING_YAN_BIAO_SHENG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2087  ����  �������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAJingYanBiaoShengInfo						//<! 3410 �������
	{
	public:
		SCRAJingYanBiaoShengInfo();
		MessageHeader header;

		int buy_times;			//<! �������
		int day_buy_times;		//<! ÿ�չ������
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
