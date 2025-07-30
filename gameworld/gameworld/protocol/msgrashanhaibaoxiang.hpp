#ifndef __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__
#define __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rashanhaibaoxiangparam.hpp"

//////////////////////////////////////////// 2157  ����  ɽ������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShanHaiBaoXiangInfo						//<! 3431 ɽ��������Ϣ
	{
	public:
		SCRAShanHaiBaoXiangInfo();
		MessageHeader header;

		unsigned short ra_shan_hai_bao_xiang_day_buy_times[RA_SHAN_HAI_BAO_XIANG_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_shan_hai_bao_xiang_buy_times[RA_SHAN_HAI_BAO_XIANG_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_SHAN_HAI_BAO_XIANG_BUY_MAX_ITEM> buy_flag;										//�����ʶ 0:δ���� 1:�ѹ���
		int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SHAN_HAI_BAO_XIANG_HPP__
