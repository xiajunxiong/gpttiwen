#ifndef __MSG_RA_XIAN_SHI_BAO_DAI_2_HPP__
#define __MSG_RA_XIAN_SHI_BAO_DAI_2_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2163  ����  ��ʱ����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAXianShiBaoDai2Info						//<! 3479 ��ʱ������Ϣ
	{
	public:
		SCRAXianShiBaoDai2Info();
		MessageHeader header;

		int can_buy_seq;
		unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
		int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
