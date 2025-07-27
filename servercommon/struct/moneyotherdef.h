#ifndef __MONEY_OTHER_DEF_H__
#define __MONEY_OTHER_DEF_H__

#include "servercommon/servercommon.h"

const static int MAX_MONEY_TYPE_RESERVED = 20;

enum MoneyType
{
	MONEY_TYPE_ALL								= -2,		// 协议用
	MONEY_TYPE_CONVERT_ITEM						= -1,		// 道具兑换时使用
	MONEY_TYPE_GOLD								= 0,		// 元宝
	MONEY_TYPE_IMMORTAL_COIN					= 1,		// 仙币(已弃用)
	MONEY_TYPE_COIN_BIND						= 2,		// 铜币
	MONEY_TYPE_GUILD_CONTRIBUTION				= 3,		// 公会贡献
	MONEY_TYPE_CHIVALROUS						= 4,		// 侠义值
	MONEY_TYPE_COLOSSEUM						= 5,		// 竞技场积分
	MONEY_TYPE_FACE_SCORE						= 6,		// 颜值币
	MONEY_TYPE_SURFACE_COIN						= 7,		// 服装币
	MONEY_TYPE_PLATFORM_BATTLE_CREDITS			= 8,		// 擂台战积分
	MONEY_TYPE_RESERVE							= 9,		// 保留
	MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS		= 10,		// 幻界战场积分
	MONEY_TYPE_LUCK_COIN						= 11,		// 气运值
	MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN			= 12,		// 幻界硬币
	MONEY_TYPE_YUAN_SUI_ZHI_LING				= 13,		// 元灵之髓
	MONEY_TYPE_DUI_ZHANG_GONG_ZI				= 14,		// 队长工资
	MONEY_TYPE_CLOUD_ARENA_COIN					= 15,		// 云端竞技币
	MONEY_TYPE_SILVER_COIN						= 16,		// 银两(灵玉) 
	MONEY_TYPE_FEN_CHEN							= 17,		// 粉尘 (铸灵分解所得)
	MONEY_TYPE_GUESS_COIN						= 18,		// 竞猜币
	MONEY_TYPE_CAN_HUN							= 19,		// 残魂，元魂珠升级资源
	MONEY_TYPE_MAX
};

UNSTD_STATIC_CHECK(MONEY_TYPE_MAX <= MAX_MONEY_TYPE_RESERVED);

struct MoneyOtherParam
{
	MoneyOtherParam() { this->Reset(); }

	void Reset()
	{
		memset(other_currency, 0, sizeof(other_currency));
	}

	long long other_currency[MAX_MONEY_TYPE_RESERVED];
};

typedef char MoneyOtherParamHex[sizeof(MoneyOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MoneyOtherParamHex) < 512);



#endif	//__MONEY_OTHER_DEF_H__