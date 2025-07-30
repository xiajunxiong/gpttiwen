#ifndef __MSG_RA_DAN_BI_FAN_LI_HPP__
#define __MSG_RA_DAN_BI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radanbifanliparam.hpp"

//////////////////////////////////////////// 2223  随机活动  单笔返利  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_DAN_BI_FAN_LI_OPERA_TYPE
	{
		RA_DAN_BI_FAN_LI_OPERA_TYPE_INFO = 0,					//获取信息
		RA_DAN_BI_FAN_LI_OPERA_TYPE_BUY_CHECK = 1,				//购买道具 param1 pool_seq param2:seq
	};

	struct SCRADanBiFanLiInfo			//<! 5325 单笔返利信息下发
	{
	public:
		SCRADanBiFanLiInfo();
		MessageHeader header;

		short today_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];			//当天购买次数
		short activity_buy_times_pool_list[MAX_RA_DAN_BI_FAN_LI_POOL_NUM];		//活动购买次数
	};

	class SCRADanBiFanLiSingleInfo		//!< 5326 单笔返利单条信息下发
	{
	public:
		SCRADanBiFanLiSingleInfo();
		MessageHeader header;

		short reserve_sh;
		short pool_seq;
		short today_buy_times;
		short activity_buy_times;
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_DAN_BI_FAN_LI_HPP__
