#ifndef __MSG_RA_CAI_SHEN_JU_BAO_HPP__
#define __MSG_RA_CAI_SHEN_JU_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2062  ����  ����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRACaiShenJuBaoInfo						//<! 3380 ����۱���Ϣ
	{
	public:
		SCRACaiShenJuBaoInfo();
		MessageHeader header;

		unsigned short ra_cai_shen_ju_bao_buy_day_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_cai_shen_ju_bao_buy_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];				//ÿ����Ʒ�޹�����
		BitMap<RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM> buy_flag;										//�����ʶ  0:δ���� 1:�ѹ���
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
