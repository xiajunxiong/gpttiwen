#ifndef __MSG_RA_YUN_ZE_CATCH_ELVES_HPP__
#define __MSG_RA_YUN_ZE_CATCH_ELVES_HPP__

#include "servercommon/userprotocal/msgheader.h"



#pragma pack(push, 4)

static const int MAX_ELVES_NUM = 10;			// 精灵最大数量

namespace Protocol
{

	class SCRAYunZeCatchElvesGetInfo			//!< 3135 随机活动-精灵大抓捕抓捕信息下发
	{
	public:
		SCRAYunZeCatchElvesGetInfo();
		MessageHeader header;

		int catch_elves_num[MAX_ELVES_NUM + 1];   //只读elves_num长度，下标0--是否抓到特殊精灵标志位，精灵id与下标相同
	};


}


#pragma pack(pop)

#endif
