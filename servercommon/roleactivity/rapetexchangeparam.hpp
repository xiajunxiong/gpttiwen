#ifndef __RA_PET_EXCHANGE_PARAM_HPP__
#define __RA_PET_EXCHANGE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2072 随机活动 - 宠物兑换  //////////////////////////////////////////

const static int MAX_RA_PET_NUM = 20;			// 最多兑换种类
const static int MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM = 10;	//商店道具数量

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAPetExchangeParam
{
	RAPetExchangeParam() { this->Reset(); }
	void Reset()
	{
		ra_pet_exchange_begin_timestamp = 0;
		memset(ra_pet_exchange_times, 0, sizeof(ra_pet_exchange_times));
		memset(ra_pet_exchange_day_times, 0, sizeof(ra_pet_exchange_day_times));
		memset(ra_shop_buy_times, 0, sizeof(ra_shop_buy_times));
	}

	unsigned int ra_pet_exchange_begin_timestamp;
	unsigned short ra_pet_exchange_times[MAX_RA_PET_NUM];			//活动已兑换次数
	unsigned short ra_pet_exchange_day_times[MAX_RA_PET_NUM];		//每日已兑换次数
	int ra_shop_buy_times[MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM];		//商店已购买次数(每日与活动共用)
};

UNSTD_STATIC_CHECK(sizeof(RAPetExchangeParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_PET_EXCHANGE_PARAM_HPP__
