#ifndef __MSG_RA_XIAN_SHI_BAO_HE_3_HPP__
#define __MSG_RA_XIAN_SHI_BAO_HE_3_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2211  ����  ��ʱ����3  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoHe3Info						//<! 5140 ��ʱ����3 �ļ����� 
	{
	public:
		SCRAXianShiBaoHe3Info();
		MessageHeader header;

		unsigned short day_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];		//<! ÿ����Ʒÿ���޹�����
		unsigned short buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];			//<! ÿ����Ʒ�޹�����
		int has_free_reward;						//<! ��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
