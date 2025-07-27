#ifndef __ROLE_CONVERT_SHOP_PARAM_HPP__
#define __ROLE_CONVERT_SHOP_PARAM_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

enum CONVERT_TYPE
{
	CONVERT_TYPE_DAILY_LIMIT_BUY = 0,	//!< 0 每日限购
	CONVERT_TYPE_AMAZING_SHOP = 1,		//!< 1 神奇杂货
	CONVERT_TYPE_JIANGHU_PRESTIGE = 2,	//!< 2 江湖声望
	CONVERT_TYPE_COLOSSEUM = 3,			//!< 3 竞技商店
	CONVERT_TYPE_GOLD_SHOP = 4,			//!< 4 元宝商店（商城）
	CONVERT_TYPE_SURFACE_SHOP = 5,		//!< 5 时装商店 （暂已不用）
	CONVERT_TYPE_PLATFORM_SHOP = 6,		//!< 6 擂台商店
	CONVERT_TYPE_LUCKY_BAG = 7,			//!< 7 福袋币商店
	CONVERT_TYPE_HUAN_JIE_SHOP = 8,		//!< 8 幻界商店
	CONVERT_TYPE_DUI_ZHANG_SHOP = 9,	//!< 9 队长商店,福利商店
	CONVERT_TYPE_CLOUD_ARENA = 10,		//!< 10 云端竞技商店
	CONVERT_TYPE_ZHU_LING = 11,		//!< 11 铸灵兑换商店
	CONVERT_TYPE_GUESS_COIN = 12,		//!< 12  竞猜币商店
	CONVERT_TYPE_YUAN_LING_BAI_HUO = 13,	//!< 13  元灵百货
	CONVERT_TYPE_MAX,
};

enum CONVERT_LIMIT_TYPE
{
	CONVERT_LIMIT_TYPE_NONE = 0,		//!< 0 不限购
	CONVERT_LIMIT_TYPE_DAILY = 1,		//!< 1 每日限购
	CONVERT_LIMIT_TYPE_WEEKLY = 2,		//!< 2 每周限购
	CONVERT_LIMIT_TYPE_MONTHLY = 3,		//!< 3 每月限购
	CONVERT_LIMIT_TYPE_FOREVER = 4,		//!< 4 永久限购
	CONVERT_LIMIT_TYPE_HUAN_JIE = 5,	//!< 5 幻界赛季重置
	CONVERT_LIMIT_TYPE_CLOUD_ARENA_SEASON = 6, //!< 6 云端竞技赛季重置
	CONVERT_LIMIT_TYPE_MAX,
};

enum CONVERT_REQUIRE_TYPE
{
	CONVERT_REQUIRE_TYPE_NONE = 0,				//!< 0 任何人有购买资格
	CONVERT_REQUIRE_TYPE_ROLE_LEVEL = 1,		//!< 1 按人物等级
	CONVERT_REQUIRE_TYPE_VILLAGE_PRESTIGE = 2,	//!< 2 按人物村庄声望
	CONVERT_REQUIRE_TYPE_COLOSSEUM = 3,			//!< 3 竞技场
	CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_DAN_ID = 4, //!< 4 云端竞技赛季最高段位
	CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_RANK = 5,	//!< 5 云端竞技赛季最高排名
	CONVERT_REQUIRE_TYPE_HUAN_JIE_SCORE = 6,	//!< 6 幻界商店最高积分

	CONVERT_REQUIRE_TYPE_MAX,
};

static const int CONVERT_TYPE_MAX_ITEM_COUNT = 100;

struct ConvertItemRecord
{
	ConvertItemRecord() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		convert_times = 0;
		version = 0;
		open_day = 0;
	}

	ItemID item_id;
	short convert_times;			//!< 兑换了多少次
	short version;
	short open_day;
};

struct ConvertTypeInfo
{
	ConvertTypeInfo() { this->Reset(); }

	void Reset()
	{
		memset(reserve_int, 0, sizeof(reserve_int));
		for (int i = 0; i < CONVERT_TYPE_MAX_ITEM_COUNT; ++i)
		{
			record_list[i].Reset();
		}
	}

	int reserve_int[4];		//!< 保留字
	ConvertItemRecord record_list[CONVERT_TYPE_MAX_ITEM_COUNT];
};

struct RoleConvertShopParam
{
	RoleConvertShopParam() { this->Reset(); }

	void Reset()
	{
		memset(reserve_int, 0, sizeof(reserve_int));
		for (int i = 0; i < CONVERT_TYPE_MAX; ++i)
		{
			convert_list[i].Reset();
		}
	}

	int reserve_int[4];		//!< 保留字
	ConvertTypeInfo convert_list[CONVERT_TYPE_MAX];
};

typedef char RoleConvertShopParamHex[sizeof(RoleConvertShopParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleConvertShopParamHex) < 22900);

#pragma pack(pop)

#endif