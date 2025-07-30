#ifndef __MSG_RA_XIN_FU_LING_CHONG_HPP__
#define __MSG_RA_XIN_FU_LING_CHONG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2217  ����  �·���� //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_XIN_FU_LING_CHONG_OPERA_TYPE
	{
		RA_XIN_FU_LING_CHONG_OPERA_TYPE_INFO = 0,					//��ȡ��Ϣ
		RA_XIN_FU_LING_CHONG_OPERA_TYPE_BUY_ITEM = 1,				//�������
	};

	struct SCRAXinFuLingChongInfo					//<! 5220 �·������Ϣ�·�
	{
	public:
		SCRAXinFuLingChongInfo();
		MessageHeader header;

		int buy_count;								//�������
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
