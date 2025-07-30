#ifndef __MSG_RA_TIAN_JIANG_HAO_LI_HPP__
#define __MSG_RA_TIAN_JIANG_HAO_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"

//////////////////////////////////////////// 2064  随机活动  天降好礼  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRATianJiangHaoLiInfo        // 3023 天降好礼信息
	{
	public:
		SCRATianJiangHaoLiInfo();
		MessageHeader header;

		short is_refresh_box;				// 1:正在刷新宝箱
		short receive_count;
	};

	struct SCRALaoDongHaoLiInfo        // 3074 劳动好礼信息
	{
	public:
		SCRALaoDongHaoLiInfo();
		MessageHeader header;

		short is_refresh_box;				// 1:正在刷新宝箱
		short receive_count;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_TIAN_JIANG_HAO_LI_HPP__
