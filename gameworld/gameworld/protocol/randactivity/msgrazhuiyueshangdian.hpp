#ifndef __MSG_RA_ZHUI_YUE_SHANG_DIAN_HPP__
#define __MSG_RA_ZHUI_YUE_SHANG_DIAN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityzhuiyueshangdianparam.hpp"

//////////////////////////////////////////// 2208  随机活动  金龙聚宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	class SCRAZhuiYueShangDianInfo						//<! 5330 信息
	{
	public:
		SCRAZhuiYueShangDianInfo();
		MessageHeader header;
	
		unsigned short ra_day_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];		//20 每种物品每天限购次数
		unsigned short ra_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];			//20 每种物品限购次数
		RAZhuiYueShangDianItem item_list_select[ZHUI_YUE_SHANG_DIAN_SELECT_NUM_SELECT];		//3 自选奖励列表
	};

	class SCRAZhuiYueShangDianDrawRet						//<! 5331 抽奖返回
	{
	public:
		SCRAZhuiYueShangDianDrawRet();
		MessageHeader header;

		int count;
		int item_list[10];		//-1 是大奖，其他是格子index
	};

	class SCRAZhuiYueShangDianDrawRecrod						//<! 5332 抽奖记录
	{
	public:
		SCRAZhuiYueShangDianDrawRecrod();
		MessageHeader header;

		int count;
		RAZhuiYueShangDianDrawRecord record_list[ZHUI_YUE_SHANG_DIAN_RECORD_NUM];		//-1 是大奖，其他是格子index
	};
}

#pragma pack(pop)

#endif 
