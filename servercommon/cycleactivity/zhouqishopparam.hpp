#ifndef __ZHOU_QI_SHOP_PARAM_HPP__
#define __ZHOU_QI_SHOP_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int ZHOU_QI_SHOP_ITEM_MAX_NUM = 20;				// 情缘商店物品种类上限
static const int ZHOU_QI_SHOP_ITEM_LIMIT_BUY_NUM = 9999;			// 每种道具的限购数量不能超过9999


struct ZhouQiShopItemParam
{
	ZhouQiShopItemParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ver = 0;
		buy_num = 0;
		reserved = 0;
	}

	unsigned short ver;
	unsigned short buy_num;
	int reserved;
};

struct ZhouQiShopParam
{
	ZhouQiShopParam()
	{
		this->Reset();
	}

	void Reset()
	{			
		ArrayCallReset(item_info);
	}

	void ResetShopInfo()
	{
		ArrayCallReset(item_info);
	}

	ZhouQiShopItemParam item_info[ZHOU_QI_SHOP_ITEM_MAX_NUM];
};

typedef char ZhouQiShopParamHex[sizeof(ZhouQiShopParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ZhouQiShopParamHex) < 512);

#pragma pack(pop)
#endif
