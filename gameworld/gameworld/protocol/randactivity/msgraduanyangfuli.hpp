#ifndef __MSG_RA_DUAN_YANG_FU_LI_HPP__
#define __MSG_RA_DUAN_YANG_FU_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2207  ����  ������ѡ  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRADuanYangFuLiInfo						//<! 5110 ����������Ϣ
	{
	public:
		SCRADuanYangFuLiInfo();
		MessageHeader header;

		RADuanYangFuLiItem item_list[DUAN_YANG_FU_LI_BUY_ITEM_MAX_NUM];		//20
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
