#ifndef __MSG_RA_SECRET_TREASURE_HPP__
#define __MSG_RA_SECRET_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasecrettreasureparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//由协议 3000
	enum RA_SECRET_TREASURE_REQ_TYPE
	{
		RA_SECRET_TREASURE_REQ_TYPE_INFO = 0,				// 请求信息下发
		RA_SECRET_TREASURE_REQ_TYPE_BUY_ITEM = 1,			// 购买道具 param1:type param2:seq
		RA_SECRET_TREASURE_REQ_TYPE_OPEN_ACTIVITY = 2,		// 打开活动界面
	};

	class SCRASecretTreasureInfo			//!< 3073  运营活动-特惠秘宝信息下发
	{
	public:
		SCRASecretTreasureInfo();
		MessageHeader header;

		short phase;
		short is_open_today_flag;												//今天是否打开过界面 0:没打开 1：打开过
		short today_buy_times[MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM];			//每日限购
		short activity_buy_times[MAX_SECRET_TREASURE_SHOP_ITEM_NUM];			//活动限购
	};

}

#pragma pack(pop)


#endif	//__MSG_RA_SECRET_TREASURE_HPP__