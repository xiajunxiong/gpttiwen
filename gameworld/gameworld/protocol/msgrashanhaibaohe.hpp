#ifndef __MSG_RA_SHAN_HAI_BAO_HE_HPP__
#define __MSG_RA_SHAN_HAI_BAO_HE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaibaoheparam.hpp"

//////////////////////////////////////////// 2158  ����  ɽ������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShanHaiBaoHeInfo						//<! 3432 ɽ��������Ϣ
	{
	public:
		SCRAShanHaiBaoHeInfo();
		MessageHeader header;

		unsigned short day_buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short buy_times[RA_SHAN_HAI_BAO_HE_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_SHAN_HAI_BAO_HE_BUY_MAX_ITEM> buy_flag;					//�����ʶ 0:δ���� 1:�ѹ���
		int has_free_reward;												//��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_BAO_HE_HPP__
