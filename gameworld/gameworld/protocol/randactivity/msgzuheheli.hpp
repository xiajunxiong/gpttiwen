#ifndef __MSG_RA_ZU_HE_HE_LI_HPP__
#define __MSG_RA_ZU_HE_HE_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  ����  ���Ʊ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAZuHeHeLiInfo						//<! 3630  ��Ϻ���
	{
	public:
		SCRAZuHeHeLiInfo();
		MessageHeader header;

		unsigned short ra_day_buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];			//��С50��ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];				//ÿ����Ʒ�޹�����
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
