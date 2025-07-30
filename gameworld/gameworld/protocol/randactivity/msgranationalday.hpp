#ifndef __MSG_RA_NATIONAL_DAY_HPP__
#define __MSG_RA_NATIONAL_DAY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ranationaldayparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//觅宝坊店 由3000转接
	enum RA_MI_BAO_FANG_DIAN_REQ_TYPE
	{
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_INFO = 0,									//请求信息下发
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_BUY_ITEM = 1,								//请求购买道具 param1:seq
		RA_MI_BAO_FANG_DIAN_REQ_TYPE_OPEN_ACTIVITY = 2,							//打开活动界面
	};

	class SCRAMiBaoFangDianInfo		//!< 3088 随机活动-觅宝坊店信息下发
	{
	public:
		SCRAMiBaoFangDianInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;											//0:不重算 1:重算阶段
		short is_open_today_flag;												//今天是否打开过界面 0:没打开 1：打开过
		int cur_use_open_day;													//使用该天数来确定开服区间
		short today_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];		//每日限购
		short activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];	//活动限购
	};

	class SCRAMiBaoFangDianSingleInfo		//!< 3089 随机活动-觅宝坊店单个信息下发
	{
	public:
		SCRAMiBaoFangDianSingleInfo();
		MessageHeader header;

		short seq;
		short index;
		short today_buy_times;
		short activity_buy_times;
	};





}

#pragma pack(pop)


#endif
