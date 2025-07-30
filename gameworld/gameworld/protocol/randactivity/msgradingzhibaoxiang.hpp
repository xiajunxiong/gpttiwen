#ifndef __MSG_RA_DING_ZHI_BAO_XIANG_HPP__
#define __MSG_RA_DING_ZHI_BAO_XIANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  ����  ���Ʊ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRADingZhiBaoXiangInfo						//<! 3390 ���Ʊ�����Ϣ
	{
	public:
		SCRADingZhiBaoXiangInfo();
		MessageHeader header;

		unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM> buy_flag;										//�����ʶ 0:δ���� 1:�ѹ���
		int has_free_reward;																		//��ѱ��䣬0-δ��ȡ��1-����ȡ
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
