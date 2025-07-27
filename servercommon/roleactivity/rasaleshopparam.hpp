#ifndef __RA_SALE_SHOP_PARAM_HPP__
#define __RA_SALE_SHOP_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2059 随机活动 - 特卖商店  //////////////////////////////////////////
static const int MAX_SALE_SHOP_TYPE_SEQ_NUM = 24;

enum SALE_SHOP_ITEM_TYPE					//商店道具类型
{
	SALE_SHOP_ITEM_TYPE_RECOMMEND = 0,		//推荐
	SALE_SHOP_ITEM_TYPE_MOUNT,				//坐骑
	SALE_SHOP_ITEM_TYPE_SURFACE,			//时装
	SALE_SHOP_ITEM_TYPE_ARMS,				//武器
	SALE_SHOP_ITEM_TYPE_PET_GIFT,			//宠物
	SALE_SHOP_ITEM_TYPE_EQUIPMENT_GIFT,		//装备礼包

	SALE_SHOP_ITEM_TYPE_MAX
};

enum SALE_SHOP_LIMIT_TYPE
{
	SALE_SHOP_LIMIT_TYPE_BEGIN = 0,			//无效
	SALE_SHOP_LIMIT_TYPE_BUY_COUNT = 1,		//总限兑
	SALE_SHOP_LIMIT_TYPE_TODAY = 2,			//每日限兑

	SALE_SHOP_LIMIT_TYPE_MAX,
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASaleShopParam
{
	RASaleShopParam() { this->Reset(); }
	void Reset()
	{
		ra_sale_shop_begin_timestamp = 0;
		memset(ra_sale_shop_buy_count, 0, sizeof(ra_sale_shop_buy_count));
	}

	unsigned int ra_sale_shop_begin_timestamp;
	short ra_sale_shop_buy_count[SALE_SHOP_ITEM_TYPE_MAX][MAX_SALE_SHOP_TYPE_SEQ_NUM];      //玩家各种类型中的各个道具购买数量

};

UNSTD_STATIC_CHECK(sizeof(RASaleShopParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SALE_SHOP_PARAM_HPP__
