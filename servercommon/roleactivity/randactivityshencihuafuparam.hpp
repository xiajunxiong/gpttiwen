#ifndef __RA_SHEN_CI_HUA_FU_PARAM_HPP__
#define __RA_SHEN_CI_HUA_FU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#pragma pack(push, 4)
//////////////////////////////////////////// 2217 随机活动 - 神赐华福 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const static int SHEN_CI_HUA_FU_SELECT_NUM_SELECT = 3;		//自选奖励数量
const static int SHEN_CI_HUA_FU_SELECT_NUM_RAND = 6;		//非自选奖励数量
const static int SHEN_CI_HUA_FU_MAX_BUY_ITEM = 20;		//
const static int SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM = 26;		//

struct RAShenCiHuaFuItem
{
	RAShenCiHuaFuItem() { this->Reset(); }
	void Reset()
	{
		is_get = 0;
		is_select = 0;
		reserved_sh = 0;
		reward_group_id = 0;
		reward_seq = 0;
	}

	unsigned char is_get;
	unsigned char is_select;
	unsigned short reserved_sh;
	int reward_group_id;
	int reward_seq;
};


struct RAShenCiHuaFuParam
{
	RAShenCiHuaFuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		ArrayFillZero(ra_day_exchange_times);
		ArrayFillZero(ra_exchange_times);
		ArrayCallReset(item_list_select);
		ArrayCallReset(item_list_rand);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];		//20 每种物品每天限购次数
	unsigned short ra_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];			//20 每种物品限购次数
	unsigned short ra_day_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 每种物品每天 兑换次数
	unsigned short ra_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 每种物品    兑换次数
	RAShenCiHuaFuItem item_list_select[SHEN_CI_HUA_FU_SELECT_NUM_SELECT];		//3 自选奖励列表
	RAShenCiHuaFuItem item_list_rand[SHEN_CI_HUA_FU_SELECT_NUM_RAND];		//6 非自选奖励列表
};

UNSTD_STATIC_CHECK(sizeof(RAShenCiHuaFuParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	
