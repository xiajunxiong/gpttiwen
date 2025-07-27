#ifndef __WORKSHOP_DEF_H__
#define __WORKSHOP_DEF_H__

#include "servercommon/struct/battle/battle_def.hpp"
#include "gameworld/gameworld/item/itembase.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int SPECIAL_RECIPE_TOTAL_NUM = 80 * BIT_COUNT_OF_LONG_64;
static const int PET_SKILL_BOOK_NEED_ITEM_TYPE_NUM = 10;
const static int INTJEWELRY_SUCC_MAX_LIST = 5 + 1;//灵饰品质组最大颜色 5
const static int INTJEWELRY_SUCC_MAX_GROUP = 30 + 1;//灵饰品质组最大组数
const static int MAX_GOD_EQUIP_MAX_LEVEL = 86;//神装最大进阶等级

enum WORKSHOP_OTHER_PARAM
{
	WORKSHOP_CRAFT_NONE = 0,	// 是否强化打造 - 否
	WORKSHOP_CRAFT_TRUE = 1,	// 是否强化打造 - 是

	WORKSHOP_RETURN_NONE = 0,	// 合成失败时，材料是否返回 - 否
	WORKSHOP_RETURN_TRUE = 1,	// 合成失败时，材料是否返还 - 是

	WORKSHOP_EQUIP_POSI_EQUIP = 0,	// 装备所属位置 - 装备着
	WORKSHOP_EQUIP_POSI_INBAG = 1,	// 装备所属位置 - 在背包

	WORKSHOP_STONE_TYPE_EQUIP = 0,		// 装备
	WORKSHOP_STONE_TYPE_JEWELRY = 1,	// 灵饰
};

enum WORKSHOP_JEWELRY_REFOEGE_LIST
{
	JEWELRY_REFORGE_MATE_1 = 0,
	JEWELRY_REFORGE_MATE_2 = 1,
	JEWELRY_REFORGE_RESULT = 2,

	JEWELTY_REFORGE_LIST_MAX
};

enum WORKSHOP_EXCHANGE_TYPE
{
	WORKSHOP_EXCHANGE_PET_SKILL_BOOK = 0,		// 宠物技能书
	WORKSHOP_EXCHANGE_PET_SEALS_CARD = 1,		// 宠物封印卡
	WORKSHOP_DISCOMPOSE_FABAO = 2,				// 法宝碎片分解
	WORKSHOP_SYNTHESIS_KEY= 3,					// 银钥匙、金钥匙合成
	WORKSHOP_SYNTHESIS_SUPER_MARROW  = 4,		// 超级洗髓露合成

	WORKSHOP_EXCHANGE_TYPE_MAX
};

// 0-4,装备，5-7，灵饰
enum ULTIMATE_EQUIP_TYPE_E
{
	ULTIMATE_EQUIP_TYPE_E_HELMET = 0,		// 头盔
	ULTIMATE_EQUIP_TYPE_E_ARMOUR,			// 盔甲
	ULTIMATE_EQUIP_TYPE_E_WEAPON,			// 武器
	ULTIMATE_EQUIP_TYPE_E_SHIELD,			// 盾牌
	ULTIMATE_EQUIP_TYPE_E_SHOE,				// 鞋子
	ULTIMATE_EQUIP_TYPE_J_RING,				// 戒指
	ULTIMATE_EQUIP_TYPE_J_BRACELET,			// 手镯
	ULTIMATE_EQUIP_TYPE_J_PENDANTS,			// 坠饰
	ULTIMATE_EQUIP_TYPE_MAX,
};

struct GridPair
{
	GridPair() : grid_idx(0), item_num(0) {}

	UInt16 grid_idx;
	UInt16 item_num;
};

struct JewelryProReserve		//!< 灵饰 信息 在此定义是为了防止之后通用灵饰结构体增长导致保存错误
{
	JewelryProReserve() { this->Reset(); }

	void Reset()
	{
		jewelry_type = 0;
		jewelry_item = 0;

		memset(reserve_ch, 0, sizeof(reserve_ch));
		/*j_param.has_random_net_value = 0;
		j_param.is_treasure = 0;
		j_param.attr_num = 0;

		memset(j_param.attr_list, 0, sizeof(j_param.attr_list));
		memset(j_param.creator_name, 0, sizeof(GameName));
		j_param.inc_num = 0;
		memset(j_param.attr_extra_inc, 0, sizeof(j_param.attr_extra_inc));*/
	}

	short jewelry_type;			//!< 灵饰 类型
	ItemID jewelry_item;		//!< 道具ID

	char reserve_ch[192];
	//JewelryParam j_param;	//!< 净值数据
};

struct WorkShopParam
{
	WorkShopParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < JEWELTY_REFORGE_LIST_MAX; ++i)
		{
			jewelry_reforge_list[i].Reset();
		}
		sp_recipe_active_flag.Reset();
		memset(jewelry_create_group, 0, sizeof(jewelry_create_group));
		memset(reserve_longlong, 0, sizeof(reserve_longlong));
		god_equip_flag = 0;
		sp_recipe_created_flag.Reset();
		version_god_equip = 0;
		god_equip_change_flag = 0;
		check_god_equip_base_voc_flag = 0;
		reserve_sh_2 = 0;
	}

	JewelryProReserve jewelry_reforge_list[JEWELTY_REFORGE_LIST_MAX];		//该字段已弃用 
	BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_recipe_active_flag;		// 特殊配方激活列表
	int jewelry_create_group[INTJEWELRY_SUCC_MAX_GROUP];
	int reserve_sh[JEWELTY_REFORGE_LIST_MAX];
	long long reserve_longlong[JEWELTY_REFORGE_LIST_MAX];
	int god_equip_flag;//神装制造标记
	BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_recipe_created_flag;	// 特殊配方已制造列表
	int version_god_equip;//神装版本
	char god_equip_change_flag;
	char check_god_equip_base_voc_flag;
	char reserve_sh_2;
};

UNSTD_STATIC_CHECK(INTJEWELRY_SUCC_MAX_LIST == 6);//上线以后，有修改有转移数据
typedef char WorkShopParamHex[sizeof(WorkShopParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(WorkShopParamHex) < 4096);

#pragma pack(pop)
#endif