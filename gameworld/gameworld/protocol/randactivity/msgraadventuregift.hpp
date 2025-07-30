#ifndef __MSG_RA_ADVEMTURE_GIFT_HPP__
#define __MSG_RA_ADVEMTURE_GIFT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raadventuregiftparam.hpp"

//////////////////////////////////////////// 2074  ����  �������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_ADVENTURE_GIFT_OPERA_TYPE
	{
		RA_ADVENTURE_GIFT_OPERA_TYPE_INFO = 0,		//�·���Ϣ	
		RA_ADVENTURE_GIFT_OPERA_TYPE_BUY = 1,		//�������	
	};

	struct SCRaAdventureGiftInfo			//<! 3035 ���������Ϣ
	{
	public:
		SCRaAdventureGiftInfo();
		MessageHeader header;

		int seq;							//��ǰ��������ñ�
		unsigned int end_time;				//��ǰʱ�����end_time�������Ч����end_time==0
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ADVEMTURE_GIFT_HPP__


