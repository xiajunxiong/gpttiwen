#ifndef __MSG_ZHEN_PIN_SHANG_PU_HPP__
#define __MSG_ZHEN_PIN_SHANG_PU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2128  ����  ��Ʒ����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAZhenPinShangPu						//<! 4080 ��Ʒ������Ϣ
	{
	public:
		SCRAZhenPinShangPu();
		MessageHeader header;

		RAZhenPinShangPuBuyTimes day_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		RAZhenPinShangPuBuyTimes buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];				//ÿ����Ʒ�޹�����
	};

	struct SCRAZhenPinShangPuOpenDay						//<! 4081 ��Ʒ������Ϣ
	{
	public:
		SCRAZhenPinShangPuOpenDay();
		MessageHeader header;

		int day;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
