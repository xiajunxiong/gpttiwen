#ifndef __MSG_RA_SHEN_CI_HUA_FU_HPP__
#define __MSG_RA_SHEN_CI_HUA_FU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityshencihuafuparam.hpp"

//////////////////////////////////////////// 2208  随机活动  金龙聚宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	enum SHEN_CI_HUA_FU_INFO_RESON
	{
		SHEN_CI_HUA_FU_INFO_RESON_DEFAULT,
		SHEN_CI_HUA_FU_INFO_RESON_SET,		//设置
		SHEN_CI_HUA_FU_INFO_RESON_EXCHANGE,		//兑换物品
		SHEN_CI_HUA_FU_INFO_RESON_BUY,		//购买
		SHEN_CI_HUA_FU_INFO_RESON_RESET,		//重置
	};

	class SCRAShenCiHuaFuInfo						//<! 5225 信息
	{
	public:
		SCRAShenCiHuaFuInfo();
		MessageHeader header;
	
		unsigned short ra_day_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];		//20 每种物品每天限购次数
		unsigned short ra_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];			//20 每种物品限购次数
		unsigned short ra_day_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 每种物品每天 兑换次数
		unsigned short ra_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 每种物品    兑换次数
		RAShenCiHuaFuItem item_list_select[SHEN_CI_HUA_FU_SELECT_NUM_SELECT];		//3 自选奖励列表
		RAShenCiHuaFuItem item_list_rand[SHEN_CI_HUA_FU_SELECT_NUM_RAND];		//6 非自选奖励列表
		int reason;
	};

	struct SCRAShenCiHuaFuRetItem
	{
		int type;
		int index;
	};

	class SCRAShenCiHuaFuDrawRet						//<! 5226 抽奖返回
	{
	public:
		SCRAShenCiHuaFuDrawRet();
		MessageHeader header;

		int count;
		SCRAShenCiHuaFuRetItem item_list[10];
	};
}

#pragma pack(pop)

#endif 
