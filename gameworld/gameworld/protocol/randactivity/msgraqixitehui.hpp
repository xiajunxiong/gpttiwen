#ifndef __MSG_RA_QI_XI_TE_HUI_HPP__
#define __MSG_RA_QI_XI_TE_HUI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raqixitehuiparam.hpp"

//////////////////////////////////////////// 2209  随机活动  七夕特惠  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_QI_XI_TE_HUI_OPERA_TYPE
	{
		RA_QI_XI_TE_HUI_OPERA_TYPE_INFO = 0,					//获取信息
		RA_QI_XI_TE_HUI_OPERA_TYPE_BUY_ITEM = 1,				//购买道具 param1 购买seq
		RA_QI_XI_TE_HUI_OPERA_TYPE_CANCEL_RED_DOT = 2,			//取消红点(仅限初始打开界面)
		RA_QI_XI_TE_HUI_OPERA_TYPE_FETCH_PHASE_REWARD = 3,		//领取阶段奖励 param1:seq
	};

	struct SCRAQiXiTeHuiInfo			//<! 3187 七夕特惠信息
	{
	public:
		SCRAQiXiTeHuiInfo();
		MessageHeader header;

		char is_init_open_interface_flag;									//是否初始打开过界面 1:已打开
		char reserve_ch;
		short activity_buy_count;											//活动期间购买的总次数
		BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//阶段奖励领取标识
		short today_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//每日限购
		short activity_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//活动限购
	};

	class SCRAQiXiTeHuiSingleInfo		//!< 3188 随机活动-七夕特惠单个信息下发
	{
	public:
		SCRAQiXiTeHuiSingleInfo();
		MessageHeader header;

		short activity_buy_count;
		short seq;
		short today_buy_times;
		short activity_buy_times;
	};

	struct SCRAQiXiTeHuiOtherInfo			//<! 3189 七夕特惠其它信息
	{
	public:
		SCRAQiXiTeHuiOtherInfo();
		MessageHeader header;

		char is_init_open_interface_flag;									//是否初始打开过界面 1:已打开
		char reserve_ch;
		short reserve_sh;
		BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//阶段奖励领取标识
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_QI_XI_TE_HUI_HPP__
