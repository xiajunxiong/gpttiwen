#ifndef __MSG_RA_TIGER_AUSPICIOUS_SNOW_HPP__
#define __MSG_RA_TIGER_AUSPICIOUS_SNOW_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerSpiciousSnowInfo				//!< 3147 ����-��ѩ���� ��Ϣ�·�
	{
	public:
		SCRATigerSpiciousSnowInfo();
		MessageHeader header;

		short times;
		short seq;		// �齱��Ʒseq
	};

}

#pragma pack(pop)

#endif
