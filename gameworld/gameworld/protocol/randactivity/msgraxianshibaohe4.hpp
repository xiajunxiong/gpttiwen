#ifndef __MSG_RA_XIAN_SHI_BAO_HE_4_HPP__
#define __MSG_RA_XIAN_SHI_BAO_HE_4_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2212  ����  ��ʱ����4 (�ļ�����) //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoHe4Info						//<! 5150 ��ʱ����4
	{
	public:
		SCRAXianShiBaoHe4Info();
		MessageHeader header;

		unsigned short day_buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];		//<! ÿ����Ʒÿ���޹�����
		unsigned short buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];			//<! ÿ����Ʒ�޹�����
		int has_free_reward;						//<! ��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
