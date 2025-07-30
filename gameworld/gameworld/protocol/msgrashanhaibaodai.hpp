#ifndef __MSG_RA_SHAN_HAI_BAO_DAI_HPP__
#define __MSG_RA_SHAN_HAI_BAO_DAI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaibaodaiparam.hpp"

//////////////////////////////////////////// 2158  ����  ɽ������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShanHaiBaoDaiInfo						//<! 3433 ��ʱ������Ϣ
	{
	public:
		SCRAShanHaiBaoDaiInfo();
		MessageHeader header;

		int can_buy_seq;
		unsigned short ra_day_buy_times[RA_SHAN_HAI_BAO_DAI_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[RA_SHAN_HAI_BAO_DAI_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_SHAN_HAI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
		int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_BAO_DAI_HPP__
