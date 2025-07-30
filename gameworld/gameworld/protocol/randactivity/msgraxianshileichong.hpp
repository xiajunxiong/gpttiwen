#ifndef __MSG_RA_XIAN_SHI_LEI_CHONG_HPP__
#define __MSG_RA_XIAN_SHI_LEI_CHONG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raxianshileichongparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//!<3000 随机活动请求操作
	enum RA_XIAN_SHI_LEI_CHONG_REQ_TYPE						//对应 3000协议的 opera_type
	{
		RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_INFO = 0,			//获取活动信息
		RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_GET_REWARD = 1,		//获取奖励  param1:seq[对应奖励区间的seq]
	};

	struct SCRAXianShiLeiChongInfo							//<! 4331 限时累充信息
	{
	public:
		SCRAXianShiLeiChongInfo();
		MessageHeader header;

		int open_day;										// 活动开启时的开服天数
		int total_chong_zhi;								// 活动期间累计充值金额
		int can_fetch_flag;									// 能够领取的标记[按位]
		int flag;											// 已经领取了的标记[按位]
	};
}

#pragma pack(pop)

#endif  
