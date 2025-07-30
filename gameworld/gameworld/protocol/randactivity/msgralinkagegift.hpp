#ifndef __MSG_RA_LINKAGE_GIFT_HPP__
#define __MSG_RA_LINKAGE_GIFT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

//////////////////////////////////////////// 2075  ����  ��������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_LINKAGE_GIFT_OPERA_TYPE
	{
		RA_LINKAGE_GIFT_OPERA_TYPE_INFO = 0,			//�·���Ϣ	
		RA_LINKAGE_GIFT_OPERA_TYPE_FETCH = 1,			//��ȡ����
	};


	struct SCRALinkageGiftInfo			//!< 3028	����������Ϣ�·�
	{
		SCRALinkageGiftInfo();
		MessageHeader header;

		short seq;							//��Ӧ����
		short fetch_flag;					//��ȡ��ʶ 0 δ��ɲ�����ȡ,1�Ѵ��δ��ȡ, 2����ȡ
		unsigned int can_receive_timestamp;	//����ȡʱ���
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LINKAGE_GIFT_HPP__
