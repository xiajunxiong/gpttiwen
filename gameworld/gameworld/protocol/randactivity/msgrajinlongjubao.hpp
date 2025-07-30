#ifndef __MSG_RA_JIN_LONG_JU_BAO_HPP__
#define __MSG_RA_JIN_LONG_JU_BAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2208  随机活动  金龙聚宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	enum JIN_LONG_JU_BAO_INFO_RESON
	{
		JIN_LONG_JU_BAO_INFO_RESON_DEFAULT,
		JIN_LONG_JU_BAO_INFO_RESON_SET,		//设置
		JIN_LONG_JU_BAO_INFO_RESON_FETCH,		//领取次数奖励
		JIN_LONG_JU_BAO_INFO_RESON_BUY,		//购买
		JIN_LONG_JU_BAO_INFO_RESON_RESET,		//重置
	};

	class SCRAJinLongJuBaoInfo						//<! 5115 金龙聚宝信息
	{
	public:
		SCRAJinLongJuBaoInfo();
		MessageHeader header;

		int reason;
		unsigned short ra_day_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];		//20 每种物品每天限购次数
		unsigned short ra_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];			//20 每种物品限购次数
		int draw_times;						//   抽奖次数
		unsigned int times_reward_fetch_flag;					//   轮数奖励领取标志
		RAJinLongJuBaoItem item_list_select[JIN_LONG_JU_BAO_SELECT_NUM_SELECT];	//4 自选奖励列表
		RAJinLongJuBaoItem item_list_rand[JIN_LONG_JU_BAO_SELECT_NUM_RAND];		//8 非自选奖励列表
		RAJinLongJuBaoRecord record_list[JIN_LONG_JU_BAO_RECORD_NUM];
	};

	struct SCRAJinLongJuBaoDrawRetItem
	{
		unsigned int time;
		int times;		//用来排序
		int type;
		int index;
	};

	class SCRAJinLongJuBaoDrawRet						//<! 5116 金龙聚宝抽奖返回
	{
	public:
		SCRAJinLongJuBaoDrawRet();
		MessageHeader header;

		int count;
		SCRAJinLongJuBaoDrawRetItem item_list[10];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
