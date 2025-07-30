#ifndef __MSG_RA_SHEN_SHOU_EN_ZE_HPP__
#define __MSG_RA_SHEN_SHOU_EN_ZE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2208  随机活动  金龙聚宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	class SCRAShenShouEnZeInfo						//<! 5180 神兽恩泽信息
	{
	public:
		SCRAShenShouEnZeInfo();
		MessageHeader header;

		unsigned short ra_day_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];		//20 每种物品每天限购次数
		unsigned short ra_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];			//20 每种物品限购次数
		int draw_times;						//   抽奖次数
		int is_select;					//是否选择了物品
		int select_reward_seq;				//选择物品的seq
	};

	struct RAShenShouEnZeDrawResultItem
	{
		int is_mail;
		int reward_seq;
	};

	struct SCRAShenShouEnZeDrawResult			 //<! 5181 神兽恩泽抽奖结果
	{
	public:
		SCRAShenShouEnZeDrawResult();
		MessageHeader header;

		int draw_times;
		RAShenShouEnZeDrawResultItem reward_seq_list[10];
	};

	struct SCRAShenShouEnZeRecord			 //<! 5182 神兽恩泽抽奖记录
	{
	public:
		SCRAShenShouEnZeRecord();
		MessageHeader header;

		RAShenShouEnZeRecord record_list[SHEN_SHOU_EN_ZE_RECORD_MAX_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
