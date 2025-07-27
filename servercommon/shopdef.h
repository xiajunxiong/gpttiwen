#ifndef __SHOP_DEF_H__
#define __SHOP_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int SHOP_ITEM_MAX_COUNT = 256;
static const int GOLD_SHOP_ITEM_MAX_COUNT = 128;
UNSTD_STATIC_CHECK(128 == GOLD_SHOP_ITEM_MAX_COUNT)		// 影响数据库，慎改

struct ShopParam
{
	ShopParam() { this->Reset(); }

	void Reset()
	{
		memset(shop_buy_count, 0, sizeof(shop_buy_count));
		memset(gold_shop_buy_count, 0, sizeof(gold_shop_buy_count));
		active_flag.Reset();
	}

	unsigned char shop_buy_count[SHOP_ITEM_MAX_COUNT];
	unsigned char gold_shop_buy_count[GOLD_SHOP_ITEM_MAX_COUNT];
	BitMap<GOLD_SHOP_ITEM_MAX_COUNT> active_flag;		// 元宝商城的红点通知
};

typedef char ShopParamHex[sizeof(ShopParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ShopParamHex) < 1024);
#pragma pack(pop)

#endif // !__SHOP_DEF_H__