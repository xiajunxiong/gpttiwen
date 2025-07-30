#ifndef __MSG_RA_TIGER_TEASE_NEW_YEAR_HPP__
#define __MSG_RA_TIGER_TEASE_NEW_YEAR_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerteasenewyearparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerTeaseNewYearInfo				//!< 3141 ����-�������´�	
	{
	public:
		SCRATigerTeaseNewYearInfo();
		MessageHeader header;

		short times;
		short is_win;		// ʤ��,0-��ͨ�·�,��ʤ����1-ʤ����2-ʧ��
	};

}


#pragma pack(pop)

#endif