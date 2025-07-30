#ifndef __MSG_RA_ONE_YUAN_GOU_HPP__
#define __MSG_RA_ONE_YUAN_GOU_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2114  ����  һԪ��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAOneYuanGouInfo						//<! 3890  һԪ����Ϣ
	{
	public:
		SCRAOneYuanGouInfo();
		MessageHeader header;

		int has_chong_zhi;
		int buy_times;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
