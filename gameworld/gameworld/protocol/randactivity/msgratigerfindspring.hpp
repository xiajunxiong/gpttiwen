#ifndef __MSG_RA_TIGER_FIND_SPRING_HPP__
#define __MSG_RA_TIGER_FIND_SPRING_HPP__

#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerFindSpringInfo			//!< 3140 ����-����̽��
	{
	public:
		SCRATigerFindSpringInfo();
		MessageHeader header;

		short seq;						// ����ˢ�µ�ͼ����
		short is_receive_reward;		// �����Ƿ����ȡ������0--������ȡ��1--����ȡ��2--����ȡ
	};

}


#pragma pack(pop)

#endif