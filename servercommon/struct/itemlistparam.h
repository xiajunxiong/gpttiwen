
#ifndef __ITEMLIST_PARAM_H__
#define __ITEMLIST_PARAM_H__

#include "structcommon.h"
#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "common/tlvprotocol.h"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"

class ItemParam;

namespace ItemNamespace
{
	enum ITEM_COLUMN_TYPE
	{
		ITEM_COLUMN_TYPE_MIN = 0,
		ITEM_COLUMN_TYPE_ITEM = 0,							//!< 物品栏 0
		ITEM_COLUMN_TYPE_SUPPLY = 1,						//!< 补给栏 1
		ITEM_COLUMN_TYPE_FRAGMENT = 2,						//!< 碎片栏 2
		ITEM_COLUMN_TYPE_TASK_MATERIAL = 3,					//!< 任务与材料栏 3
		ITEM_COLUMN_TYPE_ILLUSTRATION = 4,					//!< 图鉴栏 4
		ITEM_COLUMN_TYPE_PET_SOUL_EQIOP = 5,				//!< 保留栏1 5 -- 》 魂器背包
		ITEM_COLUMN_TYPE_ACCESSORIES = 6,					//!< 保留栏2 6 -- > 配饰背包
		ITEM_COLUMN_TYPE_NORMAL_COLUMN_END = 7,
		ITEM_COLUMN_TYPE_STORAGE = 7,						//!< 仓库 7
		ITEM_COLUMN_TYPE_TEMPORARY = 8,						//!< 临时背包 8
		ITEM_COLUMN_TYPE_MAX = 9,
	};

	enum ITEM_SUB_TYPE								//!< 主要用于整理物品
	{
		ITEM_SUB_TYPE_TASK_ITEM = 0,				//!< 0 任务道具
		ITEM_SUB_TYPE_WEAPON = 1,					//!< 1 武器
		ITEM_SUB_TYPE_ARMOR = 2,					//!< 2 防具
		ITEM_SUB_TYPE_NORMAL_ITEM = 3,				//!< 3 普通物品
		ITEM_SUB_TYPE_STONE = 4,					//!< 4 矿石
		ITEM_SUB_TYPE_WOOD = 5,						//!< 5 木材
		ITEM_SUB_TYPE_HERB = 6,						//!< 6 花草
		ITEM_SUB_TYPE_FABRIC = 7,					//!< 7 布料
		ITEM_SUB_TYPE_MEDICINE = 8,					//!< 8 药品
		ITEM_SUB_TYPE_GEM = 9,						//!< 9 宝石
		ITEM_SUB_TYPE_KEYS = 10,					//!< 10 钥匙
		ITEM_SUB_TYPE_MAGIC_STONE = 11,				//!< 11 魔石
		ITEM_SUB_TYPE_ELEMENT_CRYSTAL = 12,			//!< 12 元素水晶
		ITEM_SUB_TYPE_SKILL_GRASS = 13,				//!< 13 技能草
		ITEM_SUB_TYPE_FB_CRYSTAL = 14,				//!< 14 副本水晶
		ITEM_SUB_TYPE_ELEMENT_FRAGMENT = 15,		//!< 15 元素水晶碎片
		ITEM_SUB_TYPE_GIFT = 16,					//!< 16 礼包
		ITEM_SUB_TYPE_ATTR_PILL = 17,				//!< 17 属性药
		ITEM_SUB_TYPE_COLLECTION = 18,				//!< 18 图鉴卡
		ITEM_SUB_TYPE_PET_SKILL = 19,				//!< 19 宠物技能书
		ITEM_SUB_TYPE_JEWELRY = 20,					//!< 20 灵饰
		ITEM_SUB_TYPE_SHIZHUANG = 21,				//!< 21 幻化时装
		ITEM_SUB_TYPE_EXP_CRYSTAL = 22,				//!< 22 打卡水晶
		ITEM_SUB_TYPE_MEDAL = 23,					//!< 23 勋章
		ITEM_SUB_TYPE_PARTNER_EQUIPMENT = 24,		//!< 24 伙伴装备
		ITEM_SUB_TYPE_PET_EQUIPMENT = 25,			//!< 25 宠物装备
		ITEM_SUB_TYPE_ADD_MEDAL_SP = 26,			//!< 26 附灵石
		ITEM_SUB_TYPE_APPEARANCE = 27,				//!< 27 颜值物品（头衔、气泡框、尾标）
		ITEM_SUB_TYPE_SKIN = 28,					//!< 28 皮膚（寵物、夥伴）
		ITEM_SUB_TYPE_SHANHAI_KEY = 29,				//!< 29 山海降妖KEY
		ITEM_SUB_TYPE_PARTNER_FRAGMENT = 30,		//!< 30 伙伴碎片
		ITEM_SUB_TYPE_PET_FRAGMENT = 31,			//!< 31 宠物碎片
		ITEM_SUB_TYPE_NO_CHECK_EQUIP = 32,			//!< 32 未鉴定装备
		ITEM_SUB_TYPE_PARTNER_NOREXITEM = 33,		//!< 33 伙伴激活卡
		ITEM_SUB_TYPE_FABAO = 34,					//!< 34 法宝精华
		ITEM_SUB_TYPE_PET_SOUL = 35,				//!< 35 魂器
		ITEM_SUB_TYPE_ZHU_LING = 36,				//!< 36 铸灵
		ITEM_SUB_TYPE_GOD_PRINT = 37,				//!< 37 神印
		ITEM_SUB_TYPE_ATTR_PELLET = 38,				//!< 38 属性丹
		ITEM_SUB_TYPE_SPIRIT_PACT = 39,				//!< 39 元神契约
		ITEM_SUB_TYPE_PEISHI = 40,					//!< 40 配饰
		ITEM_SUB_TYPE_DIANHUASHI = 41,				//!< 41 点化石
		ITEM_SUB_TYPE_FISH = 42,					//!< 42 鱼
		ITEM_SUB_TYPE_PEAK_TOKEN = 43,				//!< 43 天下至尊令
		ITEM_SUB_TYPE_PET_BADGE = 44,				//!< 44 宠物徽章
		ITEM_SUB_TYPE_ZAO_HUA_EQUIP = 45,			//!< 45 造化装备
		ITEM_SUB_TYPE_YUAN_HUN_ZHU = 46,			//!< 46 元魂珠
		ITEM_SUB_TYPE_HOLY_EQUIP = 47,				//!< 47 圣器
		ITEM_SUB_TYPE_RUNE_STONE = 48,				//!< 48 圣器符石
		ITEM_SUB_TYPE_HALLOWS_SKILL = 49,			//!< 49 圣器技能书
		ITEM_SUB_TYPE_FU_YU = 50,					//!< 50 符玉
		ITEM_SUB_TYPE_SMART_MOUNTS_SKILL = 51,		//!< 51 灵骑技能书
		ITEM_SUB_TYPE_HUAN_SHOU_SKILL = 52,			//!< 52 幻兽技能书

		ITEM_SUB_TYPE_MAX,
	};

	static const int ITEM_COLUMN_GRID_NUM_MAX = 300;		// 每个背包栏的最大可放格子数
	static const int ITEM_COLUMN_GRID_UPBOUND = 1000;		// 每个背包栏的上限格子数
	UNSTD_STATIC_CHECK(ITEM_COLUMN_GRID_NUM_MAX <= ITEM_COLUMN_GRID_UPBOUND);

	static const int KNAPSACK_MAX_GRID_NUM = ITEM_COLUMN_GRID_NUM_MAX * ITEM_COLUMN_TYPE_MAX;
	static const int KNAPSACK_MAX_GRID_NUM_RESERVED = ITEM_COLUMN_GRID_UPBOUND * ITEM_COLUMN_TYPE_MAX;

	static const int ITEM_COLUMN_GRID_NUM_UPBOUND_LIST[ITEM_COLUMN_TYPE_MAX] = {
		300, 100, 100, 300, 300, 0, 0, 300, 60
	};

	static const int ITEM_COLUMN_GRID_NUM_LOWBOUND_LIST[ITEM_COLUMN_TYPE_MAX] = {
		60, 100, 100, 60, 60, 0, 0, 60, 60
	};

	static const int ITEM_INDEX_BEGIN = 0;
	static const int ITEM_INDEX_END = 10000;

	static const int MAX_EQUIPMENT_GRID_NUM = 5;											// 最大装备格子数量
	static const int EQUIPMENT_INDEX_BEGIN = 10000;											// 装备起始格子
	static const int EQUIPMENT_INDEX_END = EQUIPMENT_INDEX_BEGIN + MAX_EQUIPMENT_GRID_NUM;	// 装备最大格子

	static const int MAX_JEWELRY_GRID_NUM = 3;												// 最大灵饰格子数量
	static const int JEWELRY_INDEX_BEGIN = 11000;											// 灵饰起始格子
	static const int JEWELRY_INDEX_END = JEWELRY_INDEX_BEGIN + MAX_JEWELRY_GRID_NUM;		// 灵饰最大格子

	static const int PET_INDEX_BEGIN = 12000;												// 宠物背包起始格子index
	static const int MAX_PET_GRID_NUM = 100;												// 最大宠物背包格子数量	
	static const int PET_INDEX_END = PET_INDEX_BEGIN + MAX_PET_GRID_NUM;					// 宠物背包最大格子index

	static const int PET_STORAGE_INDEX_BEGIN = 13000;												// 宠物仓库起始格子index
	static const int MAX_PET_STORAGE_GRID_NUM = 100;												// 最大宠物仓库格子数量
	static const int PET_STORAG_INDEX_END = PET_STORAGE_INDEX_BEGIN + MAX_PET_STORAGE_GRID_NUM;		// 宠物最大格子index

	static const int MAX_HUASHEN_EQUIPEMNET_GRID_NUM = 5;																// 化身装备格子最大数量
	static const int HUASHEN_EQUIPEMNET_INDEX_BEGIN = 14000;															// 化身装备起始格子
	static const int HUASHEN_EQUIPEMNET_INDEX_END = HUASHEN_EQUIPEMNET_INDEX_BEGIN + MAX_HUASHEN_EQUIPEMNET_GRID_NUM;	// 化身装备最大格子

	static const int MAX_HUASHEN_JEWELRY_GRID_NUM = 3;															// 化身灵饰格子最大数量
	static const int HUASHEN_JEWELRY_INDEX_BEGIN = 15000;														// 化身灵饰起始格子
	static const int HUASHEN_JEWELRY_INDEX_END = HUASHEN_JEWELRY_INDEX_BEGIN + MAX_HUASHEN_JEWELRY_GRID_NUM;	// 化身灵饰最大格子

	static const int HUASHEN_ITEM_GRID_NUM_MAX = ITEM_COLUMN_GRID_NUM_MAX + 100;					//化身背包最大可放格子
	static const int HUASHEN_ITEM_INDEX_BEGIN = 16000;												//化身背包起始格子
	static const int HUASHEN_ITEM_INDEX_END = HUASHEN_ITEM_INDEX_BEGIN + HUASHEN_ITEM_GRID_NUM_MAX;	//化身背包最大格子

	static const int MAX_HUASHEN_PET_GRID_NUM = 120;												// 化身宠物背包格子最大数量	
	static const int PET_HUASHEN_INDEX_BEGIN = 17000;												// 化身宠物背包起始格子index
	static const int PET_HUASHEN_INDEX_END = PET_HUASHEN_INDEX_BEGIN + MAX_HUASHEN_PET_GRID_NUM;	// 化身宠物背包最大格子index

	static const int MAX_MEADL_INDEX_NUM = 10;												// 勋章装备格子最大数量	
	static const int MEDAL_INDEX_BEGIN = 18000;												// 勋章装备起始格子index
	static const int MEDAL_INDEX_END = MEDAL_INDEX_BEGIN + MAX_MEADL_INDEX_NUM;				// 勋章装备最大格子index

	static const int MAX_HUASHEN_MEADL_INDEX_NUM = 10;														// 化身勋章装备格子最大数量	
	static const int HUASHEN_MEDAL_INDEX_BEGIN = 19000;														// 化身勋章装备起始格子index
	static const int HUASHEN_MEDAL_INDEX_END = HUASHEN_MEDAL_INDEX_BEGIN + MAX_HUASHEN_MEADL_INDEX_NUM;		// 化身勋章装备最大格子index

	static const int MAX_ACCESSORIES_INDEX_NUM = 5;															// 配饰装备格子最大数量	
	static const int ACCESSORIES_INDEX_BEGIN = 20000;														// 配饰装备起始格子index
	static const int ACCESSORIES_INDEX_END = ACCESSORIES_INDEX_BEGIN + MAX_ACCESSORIES_INDEX_NUM;		// 配饰装备最大格子index

	static const int MAX_ZAO_HUA_EQUIPMENT_GRID_NUM = 8;															// 最大造化装备格子数量
	static const int ZAO_HUA_EQUIPMENT_INDEX_BEGIN = 20010;															// 造化装备起始格子
	static const int ZAO_HUA_EQUIPMENT_INDEX_END = ZAO_HUA_EQUIPMENT_INDEX_BEGIN + MAX_ZAO_HUA_EQUIPMENT_GRID_NUM;	// 造化装备最大格子

	static const int MAX_ZAO_HUA_BAG_NUM = 500;														// 造化背包格子数量
	static const int ZAO_HUA_BAG_INDEX_BEGIN = 20020;												// 造化背包起始格子
	static const int ZAO_HUA_BAG_INDEX_END = ZAO_HUA_BAG_INDEX_BEGIN + MAX_ZAO_HUA_BAG_NUM;			// 造化背包最大格子

	static const int MAX_HOLY_EQUIP_BAG_NUM = 200;														// 圣器背包格子数量
	static const int HOLY_EQUIP_BAG_INDEX_BEGIN = 21000;												// 圣器背包起始格子
	static const int HOLY_EQUIP_BAG_INDEX_END = HOLY_EQUIP_BAG_INDEX_BEGIN + MAX_HOLY_EQUIP_BAG_NUM;	// 圣器背包最大格子

	static const int MAX_RUNE_STONE_BAG_NUM = 500;														// 符石背包格子数量
	static const int RUNE_STONE_BAG_INDEX_BEGIN = 22000;												// 符石背包起始格子
	static const int RUNE_STONE_BAG_INDEX_END = RUNE_STONE_BAG_INDEX_BEGIN + MAX_RUNE_STONE_BAG_NUM;	// 符石背包最大格子

	static const int MAX_FU_YU_GRID_NUM = 150;													// 符玉装备格子数量
	static const int FU_YU_GRID_INDEX_BEGIN = 23000;											// 符玉装备起始格子
	static const int FU_YU_GRID_INDEX_END = FU_YU_GRID_INDEX_BEGIN + MAX_FU_YU_GRID_NUM;		// 符玉装备最大格子

	static const int MAX_HUAN_SHOU_GRID_NUM = 200;														// 幻兽背包格子数量
	static const int HUAN_SHOU_GRID_INDEX_BEGIN = 24000;												// 幻兽背包起始格子
	static const int HUAN_SHOU_GRID_INDEX_END = HUAN_SHOU_GRID_INDEX_BEGIN + MAX_HUAN_SHOU_GRID_NUM;	// 幻兽背包最大格子

	static const int MAX_GRID_NUM = KNAPSACK_MAX_GRID_NUM + MAX_EQUIPMENT_GRID_NUM + MAX_JEWELRY_GRID_NUM + MAX_PET_GRID_NUM + MAX_PET_STORAGE_GRID_NUM
		+ MAX_HUASHEN_EQUIPEMNET_GRID_NUM + MAX_HUASHEN_JEWELRY_GRID_NUM + HUASHEN_ITEM_GRID_NUM_MAX + MAX_HUASHEN_PET_GRID_NUM
		+ MAX_MEADL_INDEX_NUM + MAX_HUASHEN_MEADL_INDEX_NUM + MAX_ACCESSORIES_INDEX_NUM + MAX_ZAO_HUA_EQUIPMENT_GRID_NUM + MAX_ZAO_HUA_BAG_NUM
		+ MAX_HOLY_EQUIP_BAG_NUM + MAX_RUNE_STONE_BAG_NUM + MAX_FU_YU_GRID_NUM + MAX_HUAN_SHOU_GRID_NUM; // 所有物品总格子数

	static const int PET_GRID_MAX_COUNT = MAX_PET_GRID_NUM + MAX_PET_STORAGE_GRID_NUM;						//宠物总共最大格子数量(本尊)
	static const int EQUIPMENT_GRID_MAX_COUNT = MAX_EQUIPMENT_GRID_NUM + MAX_HUASHEN_EQUIPEMNET_GRID_NUM;	//装备总共最大格子数量(本尊+化身)
	static const int JEWELRY_GRID_MAX_COUNT = MAX_JEWELRY_GRID_NUM + MAX_HUASHEN_JEWELRY_GRID_NUM;			//灵饰总共最大格子数量(本尊+化身)
	static const int ACCESSORIES_GRID_MAX_COUNT = MAX_ACCESSORIES_INDEX_NUM;			//配饰总共最大格子数量(本尊)

	static const int MAX_BATCH_CONSUME_ITEM_NUM = 8;										// 批量消耗，单次最大物品种类

	static const int MAX_STORAGE_PAGE_NUM = 5;												// 仓库页数
	static const int STORAGE_GRID_NUM_EACH_PAGE = 60;										// 仓库每页格子数

	UNSTD_STATIC_CHECK(0 + KNAPSACK_MAX_GRID_NUM < EQUIPMENT_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(EQUIPMENT_INDEX_BEGIN + MAX_EQUIPMENT_GRID_NUM < PET_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_INDEX_BEGIN + MAX_PET_GRID_NUM < PET_STORAGE_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_STORAGE_INDEX_BEGIN + MAX_PET_STORAGE_GRID_NUM < HUASHEN_EQUIPEMNET_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_EQUIPEMNET_INDEX_END < HUASHEN_JEWELRY_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_JEWELRY_INDEX_END < HUASHEN_ITEM_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_ITEM_INDEX_END < PET_HUASHEN_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(PET_HUASHEN_INDEX_END < MEDAL_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(MEDAL_INDEX_END < HUASHEN_MEDAL_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HUASHEN_MEDAL_INDEX_BEGIN < ACCESSORIES_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ACCESSORIES_INDEX_BEGIN < ZAO_HUA_EQUIPMENT_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ZAO_HUA_EQUIPMENT_INDEX_BEGIN < ZAO_HUA_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(ZAO_HUA_BAG_INDEX_END < HOLY_EQUIP_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(HOLY_EQUIP_BAG_INDEX_END < RUNE_STONE_BAG_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(RUNE_STONE_BAG_INDEX_END < FU_YU_GRID_INDEX_BEGIN);
	UNSTD_STATIC_CHECK(FU_YU_GRID_INDEX_END < HUAN_SHOU_GRID_INDEX_BEGIN);

	enum ITEM_TRADE_MONEY_TYPE
	{
		ITEM_TRADE_MONEY_TYPE_INVALID = 0,
		ITEM_TRADE_MONEY_TYPE_GOLD = 1,				//!< 1 元宝
		ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN = 2,	//!< 2 仙币
		ITEM_TRADE_MONEY_TYPE_MAX = 3,
	};

	static const int EQUIPMENT_MAX_LEVEL = 13;
	static const int EXCEED_PILE_LIMIT_PUT_NUM = 20;

	static const int VIRTUAL_ITEM_EXP = 65535;

#ifdef _DEBUG
#define TRACK_ITEM_FLAG
#endif
}

struct SuitEffectReCalc//装备套装重算
{
	SuitEffectReCalc() : sp_id(0), num(0) {}

	void Reset()
	{
		sp_id = 0;
		num = 0;
		color_top.clear();
	}

	int sp_id;
	int num;
	std::vector<int> color_top;//对应上面num，各数量最高品质
};

#pragma pack(push, 4)

const static int MAX_SUIT_TYPE = 5;//最大套装类型

static const int REWARD_LIST_ITEM_NUM = 8;

struct ItemConfigData
{
	ItemConfigData() : item_id(0), is_bind(false), num(0) {}
	ItemConfigData(ItemID _item_id, bool _is_bind, int _num) : item_id(_item_id), is_bind(_is_bind), num(_num) {}

	ItemID item_id;
	bool is_bind;
	int num;

	bool ReadConfig(TiXmlElement *element);
	bool ReadConfigNoCheck(TiXmlElement *element);
	bool CheckConfig(std::string& errmsg) const;
};

typedef  char ItemDataWrapperOld[540];

struct ItemDataWrapper
{
	ItemDataWrapper() : item_id(0), is_market_buy_flag(0), reserve_ch(0), num(0), is_bind(0), has_param(0), invalid_time(0), gold_price(0), item_unique_id(0LL)
	{
		memset(param_data, 0, sizeof(param_data));
	}

	ItemDataWrapper(ItemID _item_id, short _num, short _is_bind) : item_id(_item_id), is_market_buy_flag(0), reserve_ch(0), num(_num), is_bind(_is_bind), has_param(0), invalid_time(0), gold_price(0), item_unique_id(0LL)
	{
		memset(param_data, 0, sizeof(param_data));
	}

	ItemDataWrapper(ItemID _item_id, short _num, short _is_bind, short _has_param, NetValueItemParamData _param_data, UInt32 _invalid_time, int _gold_price, long long _item_unique_id, char _is_market_buy_flag)
		: item_id(_item_id), is_market_buy_flag(_is_market_buy_flag), reserve_ch(0), num(_num), is_bind(_is_bind), has_param(_has_param), invalid_time(_invalid_time), gold_price(_gold_price), item_unique_id(_item_unique_id)
	{
		if (NULL != _param_data)
		{
			memcpy(param_data, _param_data, sizeof(NetValueItemParamData));
			has_param = 1;
		}
		else
		{
			memset(param_data, 0, sizeof(NetValueItemParamData));
			has_param = 0;
		}
	}

	static void RevealConfigItemId(int config_item_id, ItemID *item_id, short *is_bind)
	{
		if (NULL == item_id || NULL == is_bind) return;

		*is_bind = short((config_item_id / 100000) != 0);
		*item_id = config_item_id % 100000;
	}

	bool IsEqual(const ItemDataWrapper &item_wrapper, bool ignore_num = false) const
	{
		if (item_id != item_wrapper.item_id || invalid_time != item_wrapper.invalid_time ||
			is_bind != item_wrapper.is_bind || has_param != item_wrapper.has_param ||
			is_market_buy_flag != item_wrapper.is_market_buy_flag) return false;
		if (!ignore_num && item_wrapper.num != num) return false;

		if (!has_param) return true;

		static const int data_size = sizeof(NetValueItemParamData);
		for (int i = 0; i < data_size; ++i)
		{
			if (param_data[i] != item_wrapper.param_data[i])
			{
				return false;
			}
		}

		return true;
	}

	bool CanMerge(const ItemDataWrapper & other) const
	{
		if (this->has_param || other.has_param || this->num <= 0 || other.num <= 0 || this->item_id == 0)
		{
			return false;
		}

		return this->IsEqual(other, true);
	}

	bool Merge(ItemDataWrapper & other)
	{
		if (!this->CanMerge(other))
		{
			return false;
		}

		this->num += other.num;
		other.item_id = 0;
		other.num = 0;

		return true;
	}

	bool Invalid() const { return 0 == item_id || num <= 0; }

	bool Serialize(TLVSerializer &outstream) const
	{
		bool ret = outstream.Push(item_id) && outstream.Push(num) && outstream.Push(is_bind)
			&& outstream.Push(has_param) && outstream.Push(invalid_time) && outstream.Push(gold_price)
			&& outstream.Push(item_unique_id) && outstream.Push(is_market_buy_flag);

		if (!ret) return false;

		if (0 != has_param)
		{
			TLVSerializer itemparam_data;
			itemparam_data.Reset((void*)param_data, sizeof(param_data));
			itemparam_data.MoveCurPos(sizeof(param_data));

			ret = outstream.Push(itemparam_data);

			if (!ret) return false;
		}

		return true;
	}

	bool Unserialize(TLVUnserializer &instream)
	{
		bool ret = instream.Pop(&item_id) && instream.Pop(&num) && instream.Pop(&is_bind)
			&& instream.Pop(&has_param) && instream.Pop(&invalid_time) && instream.Pop(&gold_price)
			&& instream.Pop(&item_unique_id) && instream.Pop(&is_market_buy_flag);

		if (!ret) return false;

		if (0 != has_param)
		{
			static TLVUnserializer itemparam_data;
			if (!instream.Pop(&itemparam_data)) return false;

			if (itemparam_data.Size() <= sizeof(param_data))
			{
				memcpy(param_data, itemparam_data.Ptr(), itemparam_data.Size());
			}
		}
		else
		{
			memset(param_data, 0, sizeof(param_data));
		}

		return true;
	}

	void Reset()
	{
		item_id = 0; is_market_buy_flag = 0; reserve_ch = 0; num = 0; is_bind = 0; has_param = 0; invalid_time = 0; gold_price = 0; item_unique_id = 0;
		memset(param_data, 0, sizeof(param_data));
	}

	ItemID item_id;						//!<  物品ID 
	char is_market_buy_flag;			//!<  是否从市场购买标识 0:否 1:从市场购买
	char reserve_ch;
	int num;							//!<  数量
	short is_bind;						//!<  是否绑定
	short has_param;					//!<  是否有净值
	UInt32 invalid_time;				//!<  过期时间
	int gold_price;						//!<  售价
	NetValueItemParamData param_data;	//!<  char[1024]  物品净值参数，如何解参数要根据物品的类型的不同而不同
	long long item_unique_id;			//!<  物品唯一ID 由系统分配
};

UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) == 1024); //!< \note 当长度大于1024，则要修改数据表中的itemparam的字段长度

static const int MAX_RECORD_ITEM_COUNT = 150;
static const int MAX_RECORD_DUPLICATE_ITEM_COUNT = 10;
struct ItemUseRecordParam
{
	ItemUseRecordParam()
	{
		this->Reset();
	}

	struct Record
	{
		Record() : item_id(0), reserved_sh(0), used_times(0) {}

		ItemID item_id;
		unsigned short reserved_sh;
		int used_times;//次数
	};

	struct DuplicateItemRecord
	{
		DuplicateItemRecord() : used_times(0), reward_time_s(0) {}

		int used_times;//次数
		unsigned int reward_time_s;//时间
	};

	void Reset()
	{
		record_count = 0;
		memset(record_list, 0, sizeof(record_list));
	}

	// 每日清空
	int record_count;
	Record record_list[MAX_RECORD_ITEM_COUNT];

	DuplicateItemRecord duplicate_record_list[MAX_RECORD_DUPLICATE_ITEM_COUNT];
};

struct GemUpgradedata
{
	struct ItemPair
	{
		ItemPair(ItemID _item_id = 0, short _item_num = 0) : item_id(_item_id), item_num(_item_num) {}

		ItemID item_id;
		short item_num;
	};

	GemUpgradedata() : target_gem(0), fin_gem_id(0), fin_need_numb(0), money_type(0), consume_coins(0), list_num(0) {};

	bool IsInvalid() const { return 0 == target_gem; }
	void SetInvalid() { target_gem = 0; }

	ItemID target_gem;		//!< 目标宝石；
	ItemID fin_gem_id;		//!< 最终需要的宝石；若下一个参数不为 0，正常情况下，此ID对应的道具应当为 1 级宝石；
	int fin_need_numb;		//!< 最终需要的宝石数量；
	int money_type;			//!< 货币类型；
	int consume_coins;		//!< 需要的货币；

	int list_num;
	ItemPair own_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];	// 材料列表
};

struct GemUpgradeInfo
{
	GemUpgradeInfo() : equip_posi(0), equip_grid(0), stone_slot(0), compose_times(0) {}

	void SetParam(short _stone_type, short _e_posi, short _e_grid, short _g_slot)
	{
		stone_type = _stone_type;
		equip_posi = _e_posi;
		equip_grid = _e_grid;
		stone_slot = _g_slot;
	}

	short stone_type;
	short equip_posi;
	short equip_grid;
	short stone_slot;
	short compose_times;
	short reserve_sh;

	GemUpgradedata ug_data;
};

class KnapsackOtherParam
{
public:
	KnapsackOtherParam() { this->Reset(); }

	void Reset()
	{
		init_flag = 0;
		is_get_novice_gift = 0;
		reserve_sh = 0;
		reserve_sh2 = 0;
		memset(curr_valid_grid_num_list, 0, sizeof(curr_valid_grid_num_list));
		reserve_ll = 0LL;
		reserve_int = 0;
		clear_temp_column_time = 0u;
		memset(extend_times_list, 0, sizeof(extend_times_list));
		reserve_sh3 = 0;
	}

	char init_flag;
	char is_get_novice_gift;
	short reserve_sh;
	short reserve_sh2;
	short curr_valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];
	unsigned int clear_temp_column_time;
	int reserve_int;
	long long reserve_ll;
	short extend_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];
	short reserve_sh3;
};
typedef char KnapsackOtherParamHex[sizeof(KnapsackOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(ItemNamespace::ITEM_COLUMN_TYPE_MAX == 9);
UNSTD_STATIC_CHECK(sizeof(KnapsackOtherParamHex) < 128);

struct EquipVocLock
{
	static const int EXA_ATTR_NUM_MAX = 6;
	EquipVocLock() : reserve_sh(0)
	{
		this->Reset();
	}

	void Reset()
	{
		memset(is_lock, 0, sizeof(is_lock));
		reserve_sh = 0;
	}

	char is_lock[EXA_ATTR_NUM_MAX];
	short reserve_sh;
};

static const int EQUIPMENT_MAX_INDEX = 5;
static const int JEWELRY_MAX_INDEX = 3;

enum EQUIP_JEWELRY_TYPE
{
	EQUIP_JEWELRY_TYPE_HELMET = 0,		// 头盔 0
	EQUIP_JEWELRY_TYPE_ARMOUR = 1,		// 盔甲 1
	EQUIP_JEWELRY_TYPE_WEAPON = 2,		// 武器 2
	EQUIP_JEWELRY_TYPE_SHIELD = 3,		// 副武器 3
	EQUIP_JEWELRY_TYPE_SHOE = 4,		// 鞋子 4
	EQUIP_JEWELRY_TYPE_GEM_RING = 5,	// 戒指 5
	EQUIP_JEWELRY_TYPE_BRACELET = 6,	// 手镯 6
	EQUIP_JEWELRY_TYPE_PENDANTS = 7,	// 坠饰 7
	EQUIP_JEWELRY_TYPE_MAX,
};

UNSTD_STATIC_CHECK(EQUIP_JEWELRY_TYPE_MAX == EQUIPMENT_MAX_INDEX + JEWELRY_MAX_INDEX);

class EquipmentOtherParam
{
public:
	EquipmentOtherParam() { this->Reset(); }

	void Reset()
	{
		element_crystal_id = 0;
		element_crystal_used_times = 0;
		element_crystal_unique_id = 0;
		huashen_element_crystal_id = 0;
		huashen_element_crystal_used_times = 0;
		huashen_element_crystal_unique_id = 0;
		huashen_element_crystal_is_bind = 0;
		element_crystal_is_market_buy_flag = 0;
		new_suit_flag = 0;
		memset(equipment_jewelry_upgreade, 0, sizeof(equipment_jewelry_upgreade));

		memset(reserve_ll, 0, sizeof(reserve_ll));
		equip_jewelry_cap = 0;

		new_voc_extra_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;

		resonace_open_level = 0;
		memset(equipment_jewelry_strengthen, 0, sizeof(equipment_jewelry_strengthen));
	}
	//本尊
	ItemID element_crystal_id;
	short element_crystal_used_times;

	EquipVocLock m_equipment_lock_list[EQUIPMENT_MAX_INDEX];
	EquipVocLock m_jewelry_lock_list[JEWELRY_MAX_INDEX];

	long long element_crystal_unique_id;

	//化身
	ItemID huashen_element_crystal_id;
	short huashen_element_crystal_used_times;
	long long huashen_element_crystal_unique_id;
	char huashen_element_crystal_is_bind;		//记录化身此时穿戴的元素水晶的绑定情况
	char element_crystal_is_market_buy_flag;	//是否从市场购买标识 0:否 1 : 从市场购买
	short new_suit_flag;							//旧装备检查转移套装

	int equipment_jewelry_upgreade[EQUIP_JEWELRY_TYPE_MAX];

	long long reserve_ll[2];

	int equip_jewelry_cap;						//装备+灵饰评分

	char new_voc_extra_flag;
	char reserve_ch;
	short reserve_sh;

	int resonace_open_level;					// 共鸣等级激活
	int equipment_jewelry_strengthen[EQUIP_JEWELRY_TYPE_MAX];//突破等级
};

#pragma pack(pop)

typedef char EquipmentOtherParamHex[sizeof(EquipmentOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(EquipmentOtherParamHex) < 512);

typedef char NetValueItemParamDataHex[sizeof(NetValueItemParamData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamDataHex) < 8192);

typedef char ItemUseRecordParamHex[sizeof(ItemUseRecordParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ItemUseRecordParamHex) < 4096);

class ItemListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ItemListData
	{
		char change_state_item;
		short index;

		ItemDataWrapper item_wrapper;
	};

	int count;
	ItemListData item_list[ItemNamespace::MAX_GRID_NUM];
};

class QueryItemListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ItemListData
	{
		char change_state_item;
		short index;

		ItemDataWrapper item_wrapper;
	};

	int count;
	ItemListData item_list[200];
};

int ReadItemConfigData(TiXmlElement * data_element, std::string str, ItemConfigData ARG_OUT & item_data, bool need_exist = true);
int ReadItemConfigData2(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData> ARG_OUT & item_data_vec, bool need_exist = true);
int ReadItemConfigDataList(TiXmlElement * data_element, std::string str, std::vector<ItemConfigData> ARG_OUT & item_data_vec, bool need_exist = true);
int ReadCfgListValue(TiXmlElement * data_element, std::string str, std::vector<int> ARG_OUT & vec, bool is_fliter_zero = false);



#endif
