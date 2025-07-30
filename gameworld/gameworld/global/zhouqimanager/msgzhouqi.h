#ifndef __MSG_ZHOU_QI_HPP__
#define __MSG_ZHOU_QI_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCZhouQiInfo	//!< 4700
	{
	public:
		SCZhouQiInfo();
		MessageHeader header;

		int zhou_qi_num;
		unsigned int begin_time;
		unsigned int end_time;
	};



}

#pragma pack(pop)

#endif