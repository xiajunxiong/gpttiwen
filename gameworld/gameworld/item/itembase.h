#ifndef __ITEMBASE_H__
#define __ITEMBASE_H__

#include <string>
#include <map>

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/petdef.h"
#include "servercommon/medaldef.h"
#include "itempool.h"
#include "servercommon/partnerdef.h"
#include "servercommon/roleattrpelletdef.hpp"

const static int OLD_EQUIP_STONE_MOUNT_MAX = 2;			// 装备(灵饰)宝石镶嵌最大数量
UNSTD_STATIC_CHECK(2 == OLD_EQUIP_STONE_MOUNT_MAX);		// 影响数据库，谨慎修改

const static int NEW_EQUIP_STONE_MOUNT_MAX = 4;			// 装备(灵饰)宝石镶嵌最大数量
UNSTD_STATIC_CHECK(4 == NEW_EQUIP_STONE_MOUNT_MAX);		// 影响数据库，谨慎修改
static const int MAX_NEW_STONE_MODIFY_VER_NUM = 1;

const static int EQUIP_JEWELRY_ATTR_NUM = 6;		// 装备灵饰强化精工最大种类
UNSTD_STATIC_CHECK(6 == EQUIP_JEWELRY_ATTR_NUM);		// 影响数据库，谨慎修改

const static int EQUIP_ZHU_LING_SLOT_NUM = 5;			// 装备(灵饰) 铸灵槽位数量 , 影响数据库，谨慎修改

class Role;
struct ItemDataWrapper;
struct ItemGridData;

const static int MAX_ACCESSORIES_BASE_VOC_NUM = 2;//基础配饰词条数

enum ACCESSORIES_SUIT_TYPE
{
	ACCESSORIES_SUIT_TYPE_MIN = 0,
	ACCESSORIES_SUIT_TYPE_1,				// 蓄气
	ACCESSORIES_SUIT_TYPE_2,				// 剑意
	ACCESSORIES_SUIT_TYPE_3,				// 相魔
	ACCESSORIES_SUIT_TYPE_4,				// 无双
	ACCESSORIES_SUIT_TYPE_5,				// 射日
	ACCESSORIES_SUIT_TYPE_6,				// 法魂
	ACCESSORIES_SUIT_TYPE_7,				// 仁心
	ACCESSORIES_SUIT_TYPE_8,				// 巫毒
	ACCESSORIES_SUIT_TYPE_9,				// 怒气
	ACCESSORIES_SUIT_TYPE_10,				// 两仪
	ACCESSORIES_SUIT_TYPE_11,				// 狂怒
	ACCESSORIES_SUIT_TYPE_12,				// 金刚
	ACCESSORIES_SUIT_TYPE_13,				// 普度
	ACCESSORIES_SUIT_TYPE_14,				// 雷音
	ACCESSORIES_SUIT_TYPE_15,				// 圣医
	ACCESSORIES_SUIT_TYPE_16,				// 咒师

	ACCESSORIES_SUIT_TYPE_MAX,
};

enum ITEM_PARAM_TYPE
{
	ITEM_PARAM_TYPE_INVAILD,
	ITEM_PARAM_TYPE_OPTIOAN_GIFT,			//自选礼包
	ITEM_PARAM_TYPE_OPTIOAN_GIFT_2,		//自选礼包2| 自选礼包（大）
};

enum ITEM_SCORE_LEVEL
{
	ITEM_SCORE_LEVEL_S = 3,
	ITEM_SCORE_LEVEL_A = 2,
	ITEM_SCORE_LEVEL_B = 1,
	ITEM_SCORE_LEVEL_DEF = 0,
};

enum ITEM_BIG_TYPE
{
	I_TYPE_INVALID = -1,							// -1 无效

	I_TYPE_EXPENSE = 0,								// 0 消耗类 能直接使用
	I_TYPE_OTHER,									// 1 被动使用类 type value 最好不要直接使用
	I_TYPE_GIFT,									// 2 礼包类
	I_TYPE_MEDAL_DISCARD,							// 3 勋章(没用了)
	I_TYPE_SPECIAL_PET,								// 4 特殊宠物（入背包时会自动变成加入宠物背包，并确定宠物属性）
	I_TYPE_CONVERT_ITEM,							// 5 放入背包会根据职业转换物品id
	I_TYPE_ZHU_LING,								// 6 铸灵
	I_TYPE_PARTNER_PACT,							// 7 伙伴--元神契约	
	I_TYPE_HUAN_SHOU_SKILL,							// 8 幻兽技能书

	I_TYPE_NET_VALUE_ITEM_BEGIN = 100,				// 带有净值的物品类型
	I_TYPE_EQUIPMENT = I_TYPE_NET_VALUE_ITEM_BEGIN,	// 100 装备
	I_TYPE_PET,										// 101 宠物
	I_TYPE_ELEMENT_CRYSTAL,							// 102 元素水晶
	I_TYPE_JEWELRY,									// 103 灵饰
	I_TYPE_EXP_CRYSTAL,								// 104 打卡水晶
	I_TYPE_TREASURE_MAP,							// 105 藏宝图
	I_TYPE_PARTNER_NATAL_CARD,						// 106 伙伴命格卡
	I_TYPE_PET_EQUIPMENT,							// 107 宠物装备
	I_TYPE_MEDAL,									// 108 勋章
	I_TYPE_PET_SOUL_EQUIP,							// 109 宠物魂器
	I_TYPE_GOD_PRINT,								// 110 神印
	I_TYPE_ACCESSORIES,								// 111 配饰
	I_TYPE_DIAN_HUA_SHI,							// 112 点化石
	I_TYPE_PET_BADGE,								// 113 宠物徽章
	I_TYPE_PEAK_TOKEN,								// 114 天下至尊令
	I_TYPE_ZAO_HUA_EQUIP,							// 115 造化装备
	I_TYPE_HOLY_EQUIP,								// 116 圣器
	I_TYPE_RUNE_STONE,								// 117 符石
	I_TYPE_YUAN_HUN_ZHU,							// 118 元魂珠
	I_TYPE_FU_YU,									// 119 符玉

	I_TYPE_NET_VALUE_ITEM_END						// 净值物品类型结束
};
static const int NET_VALLUE_ITEM_COUNT = I_TYPE_NET_VALUE_ITEM_END - I_TYPE_NET_VALUE_ITEM_BEGIN;

enum BAGS_BELONG	// 虚拟背包类型
{
	B_BELONG_KNAPSACK = 0,						// 人物背包
	B_BELONG_PET_BAG = 1,						// 宠物背包
	B_BELONG_PET_GOD_PRINT = 2,					// 神印背包
	B_BELONG_PARTNER_SPIRIT_PACT = 3,			// 伙伴--元神契约
	B_BELONG_ZAO_HUA_BAG = 4,					// 造化背包
	B_BELONG_HOLY_EQUIP_BAG = 5,				// 圣器背包
	B_BELONG_RUNE_STONE_BAG = 6,				// 符石背包
	B_BELONG_HUAN_SHOU_BAG = 7,					// 幻兽背包(净值道具不能放)

	B_BELONG_TYPE_MAX
};

union NetValueItemParam;
class ItemBase
{
public:
	static const UInt16 INVALID_ITEM_ID = 0;
	static const int MAX_EQUIP_LEVEL = 200;			// 最大装备等级

	enum ITEM_COLOR
	{
		I_COLOR_INVALID = 0,

		I_COLOR_WHITE = 1,								// 白
		I_COLOR_GREEN = 2,								// 绿
		I_COLOR_BLUE = 3,								// 蓝
		I_COLOR_PURPLE = 4,								// 紫
		I_COLOR_ORANGE = 5,								// 橙
		I_COLOR_RED = 6,								// 红
		I_COLOR_GOD_COLOR_BEGAIN = 7,
		I_COLOR_STAR = I_COLOR_GOD_COLOR_BEGAIN,		//星耀
		I_COLOR_GOD = 8,								//神装
		I_COLOR_GOD_COLOR_END = I_COLOR_GOD,
		I_COLOR_GOLD = 9,								// 金
		I_COLOR_NIELLO = 10,							// 黑金

		I_COLOR_MAX,
	};

	enum NOT_DISCARD_REASON							// 不可丢弃理由
	{
		NDR_DEFAULT,
		NDR_INLAY,									// 有镶嵌
	};

	ItemBase(short item_type);
	virtual ~ItemBase();

	inline short GetItemType() const { return m_item_type; }
	inline short GetItemSubType() const { return m_item_sub_type; }
	inline UInt16 GetItemId() const { return m_item_id; }
	inline const char * GetItemName() const { return m_item_name; }
	inline short GetColor() const { return m_color; }
	inline short GetLimitLevel() const { return m_limit_level; }
	inline short GetLimitProf() const { return m_limit_prof; }
	inline short GetPileLimit() const { return m_pile_limit; }
	inline short GetItemColumn() const { return m_item_column; }
	inline short GetBagsBelong() const { return m_bags_belong; }

	virtual bool HasInvalidTime() const;
	inline UInt32 GetTimeLength() const { return m_time_length; }
	virtual UInt32 CalInvalidTime() const;

	bool IsRecord() const { return m_is_record; }
	bool IsBroadcast() const { return m_is_broadcast; }
	bool IsBind() const { return m_is_bind; }

	virtual bool Init(const char* path, std::string *err) = 0;
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const = 0;

	virtual int GetSellPrice(NetValueItemParam* param) const { return m_sell_price; }
	virtual bool CanDiscard(NetValueItemParam* param, int *reason) const;
	virtual bool CanSell(NetValueItemParam* param) const { return this->GetSellPrice(param) > 0; }

	bool IsNeedReCalSmeltEntryNum() const;	//判断是否需要重算熔炼词条数量

	static NetValueItemParam * CreateParam();
	static NetValueItemParam * CreateParamFromStruct(const NetValueItemParam& param_data);
	static NetValueItemParam * CreateParamFromParamData(const NetValueItemParamData param_data);
	bool HasRandomNetValue(NetValueItemParam* param_data) const;
	void SetRandomNetValueFlag(NetValueItemParam* param_data) const;

	UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT); // 这里只是提醒，新增净值物品记得要考虑重写这个函数
	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const {} // 净值物品需重写随机净值的方法（是否需要随机，由外部调用者判断）

	static bool CanTrade(const ItemGridData *item_data);
	bool CanTrade() const;
	bool CanMerge() const;
	inline int GetTradeMoneyType() const { return m_item_trade_money_type; }
	inline bool IsTradeNeedAnnouncement() const { return m_trade_need_announcement; }
	std::vector<int> GetTradeMinPrice(const ItemDataWrapper* wrapper) const;
	std::vector<int> GetTradeMaxPrice(const ItemDataWrapper* wrapper) const;
	std::vector<int> GetTradeRecommendPrice(const ItemDataWrapper* wrapper) const;
	inline short GetTradeSearchType() const { return m_trade_search_type; }
	inline int GetTradeNumPerGroup() const { return m_trade_num_per_group; }
	inline bool IsLimitDrop() const { return m_is_limit_drop; }
	inline bool HasDropCoolDown() const { return m_drop_cooldown_s > 0; }
	inline int GetDropCoolDown_S() const { return m_drop_cooldown_s; }
	inline bool IsNetValueItem() const { return m_item_type >= I_TYPE_NET_VALUE_ITEM_BEGIN && m_item_type < I_TYPE_NET_VALUE_ITEM_END; }

	static bool CheckTradePrice(const std::vector<int>& min_price_vec, const std::vector<int>& max_price_vec, const std::vector<int>& recommend_price_vec);
protected:
	const short m_item_type;
	ItemID m_item_id;								// 物品id
	short m_color;									// 颜色
	bool m_is_bind;									// 初始是否绑定
	bool m_can_discard;								// 是否可丢弃
	bool m_is_broadcast;							// 是否广播
	int m_sell_price;								// 出售价格
	bool m_is_record;								// 是否记录到日志中：掉落、使用
	UInt32 m_time_length;							// 有效时长（获得物品起算）
	UInt32 m_invalid_time;							// 失效时间（特定时间点）
	short m_limit_prof;								// 职业限制
	short m_limit_level;							// 等级限制
	short m_pile_limit;								// 堆叠上限
	ItemName m_item_name;
	short m_item_column;							// 物品属于哪个栏
	short m_bags_belong;							// 所属背包类型
	short m_item_sub_type;							//!< 物品子类型 \see ITEM_SUB_TYPE

	short m_item_trade_money_type;					//!< 交易行参数：交易货币类型 \see ITEM_TRADE_MONEY_TYPE
	bool m_trade_need_announcement;					//!< 交易行参数：是否需要公示
	std::vector<int> m_item_trade_min_price;		//!< 交易行参数：底价
	std::vector<int> m_item_trade_max_price;		//!< 交易行参数：最高价
	std::vector<int> m_item_trade_recommend_price;	//!< 交易行参数：推荐价格
	short m_trade_search_type;						//!< 交易行参数：搜索类型
	int m_trade_num_per_group;						//!< 交易行参数：每组物品的数量( >1 就是要捆绑销售）
	bool m_is_limit_drop;							//!< 是否限制掉落[0:否 1:是] (若为是，则无法从掉落中获取该物品，除非被某些条件所重新开放）
	int m_drop_cooldown_s;							//!< 掉落冷却（秒）
};

#pragma pack(push, 4)

struct ZhuLinParam
{
	void Reset()
	{
		zhu_ling_item_id = 0;
		reserved_sh = 0;
	}

	ItemID zhu_ling_item_id;
	unsigned short reserved_sh;
};

static const int PET_BADGE_ITEM_ATTR_NUM = 2;

struct PetBadgeItemAttr
{
	unsigned short attr_type;
	unsigned short attr_value;
};

struct PetBadgeItemParam
{
	void Reset()
	{
		has_random_net_value = 0;
		reserved_ch = 0;
		pet_badgei_item_id = 0;
		item_uuid = 0;
		skill_id = 0;
		ArrayFillZero(attrs);
		ArrayFillZero(reserved_ch2);
	}

	unsigned char  has_random_net_value;
	unsigned char reserved_ch;
	unsigned short pet_badgei_item_id;
	long long item_uuid;
	int skill_id;
	AttrInfo attrs[PET_BADGE_ITEM_ATTR_NUM];
	unsigned char reserved_ch2[16];
};

struct EquipmentParam // 作为NetValueItemParam的一个成员，该结构体不需要构造和reset, 成员默认值为0，但不能单独被使用，必须与NetValueItemParam联用
{
	static const int ATTR_NUM_MAX = 6;
	static const int EXA_ATTR_NUM_MAX = 6;
	UNSTD_STATIC_CHECK(BATTLE_WEAPON_ATTR_NUM == ATTR_NUM_MAX);
	UNSTD_STATIC_CHECK(EQUIP_JEWELRY_ATTR_NUM == ATTR_NUM_MAX);
	int IsSameExtraInc(short attr_type)
	{
		for (int i = 0; i < EXA_ATTR_NUM_MAX; i++)
		{
			if (attr_extra_inc[i].attr_type == attr_type && attr_extra_inc[i].attr_value > 0)return i;
		}
		return -1;
	}

	int HasVacancyStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] == 0)return i;
		}
		return NEW_EQUIP_STONE_MOUNT_MAX;
	}

	bool HasStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] != 0)return true;
		}
		return false;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num, short ultimate_attr_list[ATTR_NUM_MAX])
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,ultimate_value:%d],",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)ultimate_attr_list[i]);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	const char* GetAttrVocInfoLog(AttrVocInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,unlock:%d],", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)attr_list[i].un_lock);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool HasZhuLing() const
	{
		for (int i = 0; i < ARRAY_LENGTH(zhu_ling_info); ++i)
		{
			if (zhu_ling_info[i].zhu_ling_item_id)
			{
				return true;
			}
		}

		return false;
	}
	bool CheckModifyVersion();

	char has_random_net_value;					//!< 是否已随机净值
	char is_treasure;							//!< 珍品评级
	short attr_num;								//!< 属性条数
	AttrInfo attr_list[ATTR_NUM_MAX];			//!< 属性信息数组（数组长度为6）

	ItemID old_stone[OLD_EQUIP_STONE_MOUNT_MAX];	//!< 镶嵌的宝石ItemID  --> 废弃

	GameName creator_name;						//!< 制作者名称  GameName长度64

	int sp_id;									//!< 特效id   0为没有特效

	int inc_num;									//!< 属性额外提升条数
	AttrVocInfo attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< 属性额外提升列表

	AttrVocInfo refresh_attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< 属性额外提升列表-洗练保留
	UNSTD_STATIC_CHECK(6 == EXA_ATTR_NUM_MAX);

	//强化精工
	short ultimate_attr_list[ATTR_NUM_MAX];			//!< 强化精工属性信息数组（数组长度为6）

	short suit_effect_id;							//!< 套装效果
	short is_strong_create;							//!< 是否强化打造

	int strengthen_level;							//!< 强化等级

	ZhuLinParam zhu_ling_info[EQUIP_ZHU_LING_SLOT_NUM];  //5个，铸灵孔位

	char modify_ver;									//!< 宝石迁移标记
	char suit_effect_change_flag;
	short new_suit_effect_id;
	ItemID new_stone[NEW_EQUIP_STONE_MOUNT_MAX];		//!< 镶嵌的宝石ItemID
	short reserve_sh_list[6];							//!< 预留一些,别的功能如需使用请从尾部开始
	//装备点化效果--现有
	int effect_id;
	int effect_level;
	//装备点化效果--未保留
	int refresh_effect_id;
	int refresh_effect_level;

	int star_level;
};

const static int MAX_PET_EQUIPMENT_SKILL_NUM = 2;//宠物装备携带技能数最大值

struct PetEquipMentParam			// 宠物装备的参数
{
	void Reset()
	{
		has_random_net_value = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
	}
	char has_random_net_value;
	char reserve_ch;
	short reserve_sh;
	//宠物装备
	unsigned short pet_equipment_id;
	unsigned short equipment_skill_num;
	short equipment_skill_list[MAX_PET_EQUIPMENT_SKILL_NUM];
	short equipment_skill_refresh_list[MAX_PET_EQUIPMENT_SKILL_NUM];//宠物洗练技能保存
};
UNSTD_STATIC_CHECK(MAX_PET_EQUIPMENT_SKILL_NUM == 2);

const static int MAX_PET_SOUL_EQUIPMENT_ATTR_NUM = 5;//魂器词条最大数量

struct PetSoulEquipMentParam			// 宠物魂器的参数
{
	void Reset()
	{
		has_random_net_value = 0;
		is_lock = 0;
		sp_id = 0;

		refine = 0;
		level = 0;
		steps = 0;

		pet_equipment_id = 0;
		attr_num = 0;
		memset(add_attr_list, 0, sizeof(add_attr_list));
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	char has_random_net_value;
	char is_lock;
	short sp_id;							//特效
	//宠物魂器

	int exp;
	int m_max_exp;

	int refine;				// 精炼
	short level;			// 等级
	short steps;			// 突破

	unsigned short pet_equipment_id;
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM];
};

struct PetGodPrintEquipMentParam			// 宠物神印的参数
{
	void Reset()
	{
		has_random_net_value = 0;
		reserve_ch = 0;
		reserve_sh = 0;

		god_print_data.Reset();
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	char has_random_net_value;
	char reserve_ch;
	short reserve_sh;

	GodPrintItem god_print_data;
};

enum PET_SOURCE_TYPE
{
	PET_SOURCE_TYPE_DEFAULT,
	PET_SOURCE_TYPE_TRANSACTION,
};

const static int MAX_PET_MODIFY_VER = 2;

const static int MAX_PET_SOUL_EQUIP_LIST = 2;//魂器装备数
struct PetSoulEquip
{
	void Reset()
	{
		item_unique_id = 0;
		item_id = 0;
		is_bind = 0;
		is_lock = 0;

		exp = 0;
		m_max_exp = 0;
		m_level = 0;
		m_gread = 0;
		m_refine = 0;

		attr_num = 0;
		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			add_attr_list[i].attr_type = 0;
			add_attr_list[i].attr_value = 0;
		}

		sp_id = 0;
	}
	bool Invalid() const
	{
		return item_unique_id == 0;
	}
	const char* GetAttrInfoLog(const AttrInfo* attr_list, int attr_num) const
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	long long item_unique_id;
	ItemID item_id;
	char is_bind;
	char is_lock;

	int exp;
	int m_max_exp;
	short m_level;//等级
	short m_gread;//等阶
	int m_refine;//精炼

	short sp_id;
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM];
};

UNSTD_STATIC_CHECK(sizeof(PetSoulEquip) % 4 == 0);//结构对齐

enum GOD_PRINT_INDEX_TYPE
{
	GOD_PRINT_INDEX_TYPE_0 = 0,
	GOD_PRINT_INDEX_TYPE_1 = 1,
	GOD_PRINT_INDEX_TYPE_2 = 2,
	GOD_PRINT_INDEX_TYPE_3 = 3,
	GOD_PRINT_INDEX_TYPE_4 = 4,
	GOD_PRINT_INDEX_TYPE_MAX,
};
UNSTD_STATIC_CHECK(GOD_PRINT_INDEX_TYPE_MAX == 5);//修改需要看看是否要转移数据字段

enum PET_REPLACE_SKILL_INDEX_TYPE
{
	PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT = 0,
	PET_REPLACE_SKILL_INDEX_TYPE_LIAN_YAO = 1,
};


struct PetParam			// 宠物的参数,改动同时修改擂台站的。
{
	void Reset()
	{
		pet_id = 0;
		memset(name, 0, sizeof(name));
		str_level = 0;
		level = 0;
		exp = 0;
		add_exp = 0;
		hp = 0;
		mp = 0;

		reserve_int = 0;

		pet_growth_version = 0;
		auto_add_points_flag = 0;
		reserve_sh = 0;

		reserve_int1 = 0;
		memset(use_attr_pellet_list, 0, sizeof(use_attr_pellet_list));

		memset(growth, 0, sizeof(growth));
		memset(new_growth, 0, sizeof(new_growth));
		comprehend_gift_num = 0;
		comprehend_passive_value = 0;
		lock = 0;
		refined_flag = 0;
		memset(attr, 0, sizeof(attr));
		max_hp = 0;
		max_mp = 0;
		pet_auto_move_type = 0;
		pet_auto_skill_id = 0;
		pet_auto_skill_level = 0;
		guide_pet_flag = 0;
		pet_equipment_is_bind = 0;
		equipment_item_unique_id = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
		memset(equipment_skill_refresh_list, 0, sizeof(equipment_skill_refresh_list));
		skin_id = 0;

		modify_ver = 0;
		is_up_level_break = 0;
		source = 0;
		has_refresh_points = 0;
		for (int i = 0; i < PET_PASSIVE_MAX_COUNT; i++)
		{
			passive_list[i].Reset();
			new_passive_list[i].Reset();
		}

		replace_skill_id = 0;
		replace_index = 0;
		replace_type = PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT;
		fei_sheng_times = 0;
		fei_sheng_task_jie_duan = 0;

		m_soul_equip_1.Reset();

		memset(god_print_list, 0, sizeof(god_print_list));
		fei_sheng_select_skill_id = 0;
		fei_sheng_select_skill_level = 0;

		pet_bage_item.Reset();

		m_soul_equip_2.Reset();
		lian_yao_pet_id = 0;
		lian_yao_pet_str_lv = 0;
		lian_yao_reserved_sh = 0;
		ArrayCallReset(lian_yao_passive_list);
		ArrayCallReset(talent_list);
		pet_gai_ming_level = 0;
		pet_gai_ming_exp = 0;
		ArrayFillZero(pet_gai_ming_attr_level);
	}
	void ClearGift()
	{
		for (int i = 0; i < ARRAY_LENGTH(passive_list); ++i)
		{
			passive_list[i].passive_skill_id = 0;
			passive_list[i].passive_source = 0;
			passive_list[i].skill_level = 0;
		}
	}

	void ClearNewGift()
	{
		for (int i = 0; i < ARRAY_LENGTH(new_passive_list); ++i)
		{
			new_passive_list[i].passive_skill_id = 0;
			new_passive_list[i].passive_source = 0;
			new_passive_list[i].skill_level = 0;
		}
	}

	void ClearNewGrowth()
	{
		for (int i = 0; i < ARRAY_LENGTH(new_growth); ++i)
		{
			new_growth[i] = 0;
		}
	}

	const char* GetPassiveInfoLog(const PetPassiveItemInfo* passive_list, int passive_num) const
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < passive_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[skill_id:%d,source:%d],", (int)passive_list[i].passive_skill_id, (int)passive_list[i].passive_source);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool GetPetEquip(PetEquipMentParam * pet_equipment)
	{
		if (NULL == pet_equipment)return false;
		pet_equipment->has_random_net_value = 1;
		pet_equipment->pet_equipment_id = pet_equipment_id;
		pet_equipment->equipment_skill_num = equipment_skill_num;
		for (int i = 0; i < MAX_PET_EQUIPMENT_SKILL_NUM && i < equipment_skill_num; i++)
		{
			pet_equipment->equipment_skill_list[i] = equipment_skill_list[i];
			pet_equipment->equipment_skill_refresh_list[i] = equipment_skill_refresh_list[i];
		}
		return true;
	}
	bool SetPetEquip(PetEquipMentParam * pet_equipment)
	{
		if (NULL == pet_equipment)return false;

		pet_equipment_id = pet_equipment->pet_equipment_id;
		equipment_skill_num = pet_equipment->equipment_skill_num;
		for (int i = 0; i < MAX_PET_EQUIPMENT_SKILL_NUM && i < equipment_skill_num; i++)
		{
			equipment_skill_list[i] = pet_equipment->equipment_skill_list[i];
			equipment_skill_refresh_list[i] = pet_equipment->equipment_skill_refresh_list[i];
		}

		return true;
	}

	bool GetPetSoulEquip(PetSoulEquipMentParam * pet_equipment,int pet_soul_equipment_index)
	{
		if (NULL == pet_equipment)return false;
		PetSoulEquip * soul_equip = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip)return false;
		

		pet_equipment->has_random_net_value = 1;

		pet_equipment->is_lock = soul_equip->is_lock;
		pet_equipment->level = soul_equip->m_level;
		pet_equipment->exp = soul_equip->exp;
		pet_equipment->m_max_exp = soul_equip->m_max_exp;
		pet_equipment->refine = soul_equip->m_refine;
		pet_equipment->steps = soul_equip->m_gread;
		pet_equipment->sp_id = soul_equip->sp_id;

		pet_equipment->attr_num = soul_equip->attr_num;

		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			pet_equipment->add_attr_list[i].attr_type = soul_equip->add_attr_list[i].attr_type;
			pet_equipment->add_attr_list[i].attr_value = soul_equip->add_attr_list[i].attr_value;
		}
		return true;
	}
	bool SetPetSoulEquip(PetSoulEquipMentParam * pet_equipment, int pet_soul_equipment_index)
	{
		if (NULL == pet_equipment)return false;
		PetSoulEquip * soul_equip_ptr = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (NULL == soul_equip_ptr)return false;

		PetSoulEquip & soul_equip = *soul_equip_ptr;

		soul_equip.is_lock = pet_equipment->is_lock;
		soul_equip.m_level = pet_equipment->level;
		soul_equip.exp = pet_equipment->exp;
		soul_equip.m_max_exp = pet_equipment->m_max_exp;
		soul_equip.m_refine = pet_equipment->refine;
		soul_equip.m_gread = pet_equipment->steps;
		soul_equip.sp_id = pet_equipment->sp_id;

		soul_equip.attr_num = pet_equipment->attr_num;

		for (int i = 0; i < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; i++)
		{
			soul_equip.add_attr_list[i].attr_type = pet_equipment->add_attr_list[i].attr_type;
			soul_equip.add_attr_list[i].attr_value = pet_equipment->add_attr_list[i].attr_value;
		}

		return true;
	}

	PetSoulEquip * GetPetSoulEquipInPetParam(int pet_soul_equipment_index, bool check_lian_yao = true)
	{
		if (pet_soul_equipment_index == 0)
		{
			return &m_soul_equip_1;
		}

		if (pet_soul_equipment_index == 1)
		{		//只有炼妖后才会增加格子
			if (!check_lian_yao || lian_yao_pet_id > 0)
			{
				return &m_soul_equip_2;
			}
		}

		return NULL;
	}

	const PetSoulEquip * GetPetSoulEquipInPetParamConst(int pet_soul_equipment_index, bool check_lian_yao = true) const
	{
		if (pet_soul_equipment_index == 0)
		{
			return &m_soul_equip_1;
		}

		if (pet_soul_equipment_index == 1)
		{
			//只有炼妖后才会增加格子
			if (!check_lian_yao || lian_yao_pet_id > 0)
			{
				return &m_soul_equip_2;
			}
		}

		return NULL;
	}

	void ClearPetEquipment()
	{
		pet_equipment_is_bind = 0;
		equipment_item_unique_id = 0;
		pet_equipment_id = 0;
		equipment_skill_num = 0;
		memset(equipment_skill_list, 0, sizeof(equipment_skill_list));
		memset(equipment_skill_refresh_list, 0, sizeof(equipment_skill_refresh_list));
	}
	void ClearPetSoulEquipment(int pet_soul_equipment_index)
	{
		PetSoulEquip * soul_equip_ptr = this->GetPetSoulEquipInPetParam(pet_soul_equipment_index);
		if (soul_equip_ptr)
		{
			soul_equip_ptr->Reset();
		}		
	}

	bool HasGodPrint()
	{
		for (int i = 0; i < GOD_PRINT_INDEX_TYPE_MAX; i++)
		{
			if (god_print_list[i] > 0)
			{
				return true;
			}
		}
		return false;
	}
	
	bool IsHasPetTalent() const;

	bool CheckGrowthVersion(long long pet_item_unique_id, const char* caller_func);
	bool CheckModifyVersion();

	int GetPetPassiveSkillAndLianYaoPassiveSkillNum();
	std::string ToString(long long pet_uuid) const;
	std::string GetPetBaseInfoString(long long pet_uuid) const;

	int				pet_id;
	GameName		name;
	// 强化等级
	unsigned short	str_level;
	// 等级&经验
	unsigned short	level;
	long long		exp;
	long long       add_exp;		// 战斗结束宠物增加的经验

	// 当前的血量与蓝量
	int				hp;
	int				mp;

	int	reserve_int;

	char pet_growth_version;
	unsigned char auto_add_points_flag;		//自动加点标志，bit
	short reserve_sh;

	int reserve_int1;
	short use_attr_pellet_list[MAX_ATTR_PELLET_TYPE_NUM];

	// 资质
	unsigned short	growth[ADD_POINT_TYPE_MAX];
	// 洗练出来资质
	unsigned short	new_growth[ADD_POINT_TYPE_MAX];

	int comprehend_gift_num;		//没用了，现在不限制领悟技能数量了，领悟技能的个数，每个宠物只能领悟2个技能
	int comprehend_passive_value;		//领悟技能的值

	//0为默认没有上锁  1 为上锁了
	unsigned char		lock;
	unsigned char		refined_flag;		//洗练了什么标志
	// 已分配属性点
	unsigned short	attr[ADD_POINT_TYPE_MAX];

	// 最大的血量与蓝量
	int				max_hp;
	int				max_mp;

	short pet_auto_move_type;
	short pet_auto_skill_id;
	short pet_auto_skill_level;
	char guide_pet_flag;
	char pet_equipment_is_bind;			//此处是给化身宠物装备记录（化身宠物穿戴不影响绑定，以本尊为准）

	//宠物装备
	long long equipment_item_unique_id;
	unsigned short pet_equipment_id;
	unsigned short equipment_skill_num;
	short equipment_skill_list[MAX_PET_EQUIPMENT_SKILL_NUM];
	short equipment_skill_refresh_list[MAX_PET_EQUIPMENT_SKILL_NUM];//宠物洗练技能保存
	int skin_id;

	char modify_ver;
	char is_up_level_break;			//当前等级是否突破了
	unsigned char source;			//来源 PET_SOURCE_TYPE, 自动加点功能用到，通过购买的宠物，不自动加点
	unsigned char has_refresh_points;		//每一只宠物都有一次免费洗点的机会，洗点之后就不自动加点

	// 天赋列表
	PetPassiveItemInfo	passive_list[PET_PASSIVE_MAX_COUNT];
	// 洗练出来天赋列表
	PetPassiveItemInfo	new_passive_list[PET_PASSIVE_MAX_COUNT];

	int replace_skill_id;
	unsigned char replace_index;
	unsigned char replace_type;		//0是普通index，1是炼妖index

	unsigned char fei_sheng_times;			//飞升次数
	unsigned char fei_sheng_task_jie_duan;			//飞升任务阶段 

	PetSoulEquip m_soul_equip_1;			
	int god_print_list[GOD_PRINT_INDEX_TYPE_MAX];//神印背包下表+1
	int fei_sheng_select_skill_id;
	int fei_sheng_select_skill_level;
	PetBadgeItemParam pet_bage_item;

	PetSoulEquip m_soul_equip_2;//魂器格子2 ,扩展的//只有炼妖后才会增加格子
	int lian_yao_pet_id;		//炼妖合并过来的宠物id
	unsigned short lian_yao_pet_str_lv;		//炼妖合并过来的宠物强化等级
	unsigned short lian_yao_reserved_sh;		
	PetPassiveItemInfo	lian_yao_passive_list[PET_LIAN_YAO_PASSIVE_MAX_COUNT]; //8,炼妖增加的技能
	PetTalentInfo talent_list[MAX_PET_TALENT_GROUP_NUM];	//宠物天赋列表
	unsigned short pet_gai_ming_level;		//改命等级
	unsigned short pet_gai_ming_exp;		//改命当前等级经验
	unsigned char pet_gai_ming_attr_level[PET_GAI_MING_ATTR_MAX_NUM];	//16，改命属性等级
};
UNSTD_STATIC_CHECK(5 == ADD_POINT_TYPE_MAX);
UNSTD_STATIC_CHECK(sizeof(PetBadgeItemParam) == 40);

struct ElementCrystalParam		// 元素水晶
{
	short used_times;
	short has_set_net_value;
};

struct JewelryParam			//!< 灵饰
{
	static const int ATTR_NUM_MAX = 6;
	static const int EXA_ATTR_NUM_MAX = 6;
	UNSTD_STATIC_CHECK(BATTLE_WEAPON_ATTR_NUM == ATTR_NUM_MAX);
	UNSTD_STATIC_CHECK(EQUIP_JEWELRY_ATTR_NUM == ATTR_NUM_MAX);
	int IsSameExtraInc(short attr_type)
	{
		for (int i = 0; i < EXA_ATTR_NUM_MAX; i++)
		{
			if (attr_extra_inc[i].attr_type == attr_type && attr_extra_inc[i].attr_value > 0)return i;
		}
		return -1;
	}

	int HasVacancyStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] == 0)return i;
		}
		return NEW_EQUIP_STONE_MOUNT_MAX;
	}

	bool HasStone()
	{
		for (int i = 0; i < NEW_EQUIP_STONE_MOUNT_MAX; i++)
		{
			if (new_stone[i] != 0)return true;
		}
		return false;
	}

	bool HasZhuLing() const
	{
		for (int i = 0; i < ARRAY_LENGTH(zhu_ling_info); ++i)
		{
			if (zhu_ling_info[i].zhu_ling_item_id)
			{
				return true;
			}
		}

		return false;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num, short ultimate_attr_list[ATTR_NUM_MAX])
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,ultimate_value:%d],",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)ultimate_attr_list[i]);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	const char* GetAttrVocInfoLog(AttrVocInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d,unlock:%d],", (int)attr_list[i].attr_type, (int)attr_list[i].attr_value, (int)attr_list[i].un_lock);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	bool CheckModifyVersion();

	char has_random_net_value;					//!< 是否已随机净值
	char is_treasure;							//!< 是否珍品
	short attr_num;								//!< 属性条数
	AttrInfo attr_list[ATTR_NUM_MAX];			//!< 属性信息数组（数组长度为6）

	GameName creator_name;						//!< 制作者名称  GameName长度32

	int inc_num;									//!< 属性额外提升条数
	AttrVocInfo attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< 属性额外提升列表

	AttrVocInfo refresh_attr_extra_inc[EXA_ATTR_NUM_MAX];	//!< 属性额外提升列表-洗练保留
	ItemID old_stone[OLD_EQUIP_STONE_MOUNT_MAX];		//!< 镶嵌的宝石ItemID
	UNSTD_STATIC_CHECK(6 == EXA_ATTR_NUM_MAX);

	//强化精工
	short ultimate_attr_list[ATTR_NUM_MAX];			//!< 强化精工属性信息数组（数组长度为6）

	short suit_effect_id;							//!< 套装效果
	short is_strong_create;							//!< 是否强化打造

	int strengthen_level;							//!< 强化等级

	ZhuLinParam zhu_ling_info[EQUIP_ZHU_LING_SLOT_NUM];  //5个，铸灵孔位

	char modify_ver;									//!< 宝石迁移标记
	char suit_effect_change_flag;
	short new_suit_effect_id;
	ItemID new_stone[NEW_EQUIP_STONE_MOUNT_MAX];		//!< 镶嵌的宝石ItemID
	short reserve_sh_list[6];							//!< 预留一些,别的功能如需使用请从尾部开始

	int star_level;
};

UNSTD_STATIC_CHECK(sizeof(JewelryParam::old_stone) == sizeof(EquipmentParam::old_stone));//装备和灵饰的宝石孔必须一致
UNSTD_STATIC_CHECK(sizeof(JewelryParam::new_stone) == sizeof(EquipmentParam::new_stone));//装备和灵饰的宝石孔必须一致

struct ExpCrystalParam
{
	short remain_times;			//!< 剩余次数
	short is_turn_on;			//!< 是否开启
};

struct TreasureMapItemParam
{
	short is_set_rand;
	char is_active_map;			//仅限星辰宝图	0:未激活 1:已激活
	char reserve_ch;
	int scene_id;
	int pos_x;
	int pos_y;
	int monster_id;
	long long client_need_uid;			//客户端要求增加的id，服务端没用
	int is_rand_map;				// 是否已经开图(为了旧图也可以使用)
	int param_type;					// 参数类型 TREASIRE_MAP_PARAM_TYPE
	int param1;
	int param2;
	int param3;
};

struct PartnerNatalCardParam		//!< 伙伴命格卡
{
	int has_net_value;
	int attr_type;
	int attr_value;
};

UNSTD_STATIC_CHECK(sizeof(PartnerNatalCardVoc) == 8);//必须和PartnerNatalCardParam 下面的sizeof（attr_type + attr_value） 值大小一致，这个结构修改需要舍弃道具净值重新写一个

struct NewPartnerNatalCardParam		//!< 伙伴命格卡
{
	const char* GetAttrInfoLog(PartnerNatalCardVoc* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d]",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	int has_net_value;
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};


struct MedalItemParam		//!< 勋章物品净值
{
	bool CheckModifyVersion();
	void Reset()
	{
		has_net_value = 0;
		attr_num = 0;
		sp_id = 0;
		memset(attrs, 0, sizeof(attrs));
		medal_move_ver = 0;
		grade = 0;
		special_medal_gread = 0;
		second_sp_id = 0;
	}

	short has_net_value;
	short attr_num;									//!< 没用了
	int sp_id;							//特效id
	MedalItemAttrInfo attrs[MAX_MEDAL_MAX_ATTR_NUM];		//!< 没用了

	short medal_move_ver;
	short grade;

	short special_medal_gread;		// 天赐令牌升华
	short second_sp_id;				// 副特效
};

const static int MAX_ACCESSORIES_VOC_NUM = 7;//配饰副词条数
struct AccessoriesParam		//!< 配饰
{
	const char* GetAttrVocInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d],", (int)voc_list[i].attr_type, (int)voc_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}
	int has_net_value;

	AttrInfo voc_list[MAX_ACCESSORIES_VOC_NUM];

	int strengthen_level;
};

struct DianHuaShiParam
{
	const static int MAX_DIAN_HUA_SHI_LEVEL = 40;// 点化石最大特效等级
	int has_net_value;
	int effect_id;
	int effect_level;
};

//天下至尊令
struct PeakTokenParam
{
	int has_net_value;
	int used_times;		//已使用次数
};

const static int YUAN_HUN_ZHU_MAX_ATTR_NUM = 3;  //元魂珠3个属性

struct YuanHunZhuParam
{
	std::string ToString() const;

	void Reset()
	{
		has_net_value = 0;
		yuan_hun_zhu_item_id = 0;
		ArrayFillZero(attr_seq);
		ArrayFillZero(attrs);
	}

	int has_net_value;
	ItemID yuan_hun_zhu_item_id;
	unsigned short attr_seq[YUAN_HUN_ZHU_MAX_ATTR_NUM];		//配置里属性组 seq属性
	AttrInfo attrs[YUAN_HUN_ZHU_MAX_ATTR_NUM];
};

const static int ZAO_HUA_EQUIP_YUAN_HUN_ZHU_MAX_NUM = 4;

struct ZaoHuaEquipParam
{
	int has_net_value;	

	int ling_li_value;
	int hun_li_value;
	YuanHunZhuParam yuan_hun_zhu[ZAO_HUA_EQUIP_YUAN_HUN_ZHU_MAX_NUM];
};

// 符石
static const int MAX_RUNE_STONE_VOC_NUM = 3;
struct RuneStoneParam
{
	void Reset()
	{
		has_net_value = 0;
		item_id = 0; 
		attr_num = 0;
		memset(voc_list, 0, sizeof(voc_list));
		role_attr.attr_type = 0;
		role_attr.attr_value = 0;
	}

	const char* GetAttrInfoLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d]",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	std::string ToString() const;

	int has_net_value;		//!< 是否已随机净值

	ItemID item_id;			//!< 符石id
	short attr_num;			//!< 属性条数
	AttrInfo voc_list[MAX_RUNE_STONE_VOC_NUM];	// 符石附带的可增加给圣器的基础属性
	AttrInfo role_attr;		//!< 符石附带的可增加给角色的属性
};

// 圣器
static const int MAX_HOLY_EQUIP_BASE_VOC_NUM = 7;	// 圣装基础词条
static const int MAX_HOLY_EQUIP_EXTRA_VOC_NUM = 3;	// 额外随机词条
static const int MAX_RUNE_STONE_NUM = 5;			// 最大镶嵌符石数量
static const int MAX_ACTIVE_SKILLS_NUM = 5;			// 主动技能最大数量
static const int MAX_TALENT_SKILLS_NUM = 2;			// 天赋技能最大数量

struct BattleHallowInfo;
struct HolyEquipParam
{
	enum HOLY_EQUIP_STATUS
	{
		HOLY_EQUIP_STATUS_REST = 0,			//!< 休息
		HOLY_EQUIP_STATUS_STANDBY = 1,		//!< 待机
		HOLY_EQUIP_STATUS_FIGHTING = 2,		//!< 出战
	};

	enum SKILL_HOLE_STATUS		// 主动技能与符石孔共用
	{
		SKILL_HOLE_STATUS_LOCK = -1,	 // 未解锁
		SKILL_HOLE_STATUS_UNLOCK = 0,	 // 已解锁
		SKILL_HOLE_STATUS_HAVE_VALUE = 1,// 符石孔位有符石
	};

	void Reset()
	{
		has_net_value = 0;
		level = 0;
		exp = 0;
		score = 0;
		active_max = 0;
		holy_status = 0;
		valid_extra_attr_num = 0;
		memset(m_extra_attr_list, 0, sizeof(m_extra_attr_list));
		memset(reserve_int, 0, sizeof(reserve_int));
		pet_unique_id = 0LL;
		pet_id = 0;
		memset(total_attr_list, 0, sizeof(total_attr_list));
		memset(base_attr_list, 0, sizeof(base_attr_list));
		memset(unsaved_skill_id, 0, sizeof(unsaved_skill_id));
		memset(active_skills, SKILL_HOLE_STATUS_LOCK, sizeof(active_skills));
		memset(talent_skills, 0, sizeof(talent_skills));
		memset(rune_stone_hole, SKILL_HOLE_STATUS_LOCK, sizeof(rune_stone_hole));
		reserve_ch = 0;
		reserve_sh = 0;

		for (int i = 0; i < ARRAY_LENGTH(rune_stone_list); i++)
		{
			rune_stone_list[i].Reset();
		}
	}

	const char* GetBaseAttrLog(AttrInfo* attr_list, int attr_num)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_num; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d]",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	char* GetAttrInfoLog(AttrInfo* attr_list, int attr_count)
	{
		static const int BUFFER_LEN = 512;
		static char buffer[BUFFER_LEN];

		memset(buffer, 0, sizeof(buffer));
		int cur_len = 0;
		for (int i = 0; i < attr_count; ++i)
		{
			int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[type:%d,value:%d]",
				(int)attr_list[i].attr_type, (int)attr_list[i].attr_value);
			if (len <= 0)
			{
				break;
			}
			cur_len += len;
		}

		return buffer;
	}

	void GetBattleHallowAttr(BattleHallowInfo& info) const;
	std::string AttrToString(const AttrInfo* attr_list, int attr_count) const;
	std::string ToString() const;

	int has_net_value;		//!< 是否已随机净值

	unsigned int level;			//!< 圣器等级
	unsigned int exp;			//!< 经验, 作为材料时100%保留
	unsigned int score;			//!< 圣器评分
	unsigned int active_max;	//!< 圣器当前等级活跃度上限

	short holy_status;			//!< 圣器状态

	short valid_extra_attr_num;				//!< m_extra_attr_list有效条数
	AttrInfo m_extra_attr_list[MAX_HOLY_EQUIP_EXTRA_VOC_NUM];	//!< 随圣器升级更新的加到宠物的属性
	int reserve_int[2];
	long long pet_unique_id;	//!< 绑定的宠物唯一id
	unsigned int pet_id;		//!< 圣佑宠物id

	AttrInfo total_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];		//!< 圣器属性汇总

	AttrInfo base_attr_list[MAX_HOLY_EQUIP_BASE_VOC_NUM];		//!< 圣器基础属性

	unsigned short unsaved_skill_id[2];			//!< 学习了但还未决定是否替换的技能信息, [0]-技能id, [1]-主动技能索引(如果有技能id这里才合法)
	int active_skills[MAX_ACTIVE_SKILLS_NUM];	//!< 主动技能 初始化为-1,解锁之后变成0(或许永远不会变成0) \see SKILL_HOLE_STATUS
	int talent_skills[MAX_TALENT_SKILLS_NUM];				//!< 天赋技能

	char rune_stone_hole[MAX_RUNE_STONE_NUM];	//!< 符石孔位开启状态,\see SKILL_HOLE_STATUS
	char reserve_ch;
	short reserve_sh;
	RuneStoneParam rune_stone_list[MAX_RUNE_STONE_NUM];		//!< 符石
};
UNSTD_STATIC_CHECK(sizeof(RuneStoneParam) == 24)
UNSTD_STATIC_CHECK(sizeof(HolyEquipParam) == 272)

// 符玉
static const int MAX_FU_YU_ATTR_NUM = 3;
struct FuYuParam
{
	void Reset()
	{
		has_net_value = 0;
		expired_timestamp = 0;
		sp_id = 0;
		attr_num = 0;
		extra_sp_id = 0;
		memset(base_attr_list, 0, sizeof(base_attr_list));
	}

	std::string ToString() const;

	int has_net_value;		//!< 是否已随机净值

	uint32_t expired_timestamp;	// 特效保留到期时间戳

	int sp_id;		// 特效id

	int attr_num;
	AttrInfo base_attr_list[MAX_FU_YU_ATTR_NUM];	//符玉附加基础属性

	int extra_sp_id;	// 额外特效id,(升华)
};


#pragma pack(pop)

union NetValueItemParam
{
	NetValueItemParam() // union可带有构造函数，但其成员不能有自定义的构造函数
	{
		this->Reset();
	}

	void GetData(NetValueItemParamData out_data) const
	{
		if (NULL == out_data) return;

		memcpy(out_data, data, sizeof(NetValueItemParamData));
	}

	void SetData(const NetValueItemParamData in_data)
	{
		if (NULL == in_data) return;

		memcpy(data, in_data, sizeof(NetValueItemParamData));
	}

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	const char* GetParamLog(int item_type)
	{
		static char defalut_log_str[LOG_BASE_LENGTH];
		memset(defalut_log_str, 0, sizeof(defalut_log_str));

		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", fu_yu_param.ToString().c_str());
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", yuan_hun_zhu_param.ToString().c_str());
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", rune_stone_param.ToString().c_str());
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{%s}", holy_equip_param.ToString().c_str());
				
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ZaoHuaEquipParam (has_net_value:%d) , ling_li_value[%d] hun_li_value[%d] yuanhunzhu[%s]}", 
					zao_hua_param.has_net_value, zao_hua_param.ling_li_value, zao_hua_param.hun_li_value, zao_hua_param.yuan_hun_zhu->ToString().c_str());
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PeakTokenParam {has_net_value:%d, used_times[%d]}}",
					peak_token_param.has_net_value,
					peak_token_param.used_times);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				if (ARRAY_LENGTH(pet_badge_item_param.attrs) >= 2)
				{
					SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetBadgeItemParam {has_net_value:%d, skill_id[%d] arrt_type_1:[%d]  arrt_value_1:[%d] arrt_type_2:[%d]  arrt_value_2:[%d] }}",
						pet_badge_item_param.has_random_net_value,
						pet_badge_item_param.skill_id,
						pet_badge_item_param.attrs[0].attr_type, pet_badge_item_param.attrs[0].attr_value,
						pet_badge_item_param.attrs[1].attr_type, pet_badge_item_param.attrs[1].attr_value
					);
				}
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{DianHuaShiParam {has_net_value:%d, sp_id[%d] sp_level[%d]}}",
					dian_hua_shi_param.has_net_value,
					dian_hua_shi_param.effect_id, dian_hua_shi_param.effect_level);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{AccessoriesParam {has_net_value:%d, voc_list[%s], strengthen_level:[%d]}}",
					accessoriess_param.has_net_value,
					accessoriess_param.GetAttrVocInfoLog(accessoriess_param.voc_list, MAX_ACCESSORIES_VOC_NUM),
					accessoriess_param.strengthen_level);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetGodPrintEquipMentParam:{suit_id:%d, lock:[%d],color[%d], level[%d] exp[%d] m_max_exp[%d]"
					"m_posi:[%d], m_wear_pet_idx:[%d] item_id:[%d] main_attr:[%s] add_attr_list[%s]}}",
					god_print_param.god_print_data.suit_id, (int)god_print_param.god_print_data.lock, (int)god_print_param.god_print_data.color,
					god_print_param.god_print_data.level, god_print_param.god_print_data.exp, god_print_param.god_print_data.m_max_exp,
					god_print_param.god_print_data.m_posi, god_print_param.god_print_data.m_wear_pet_idx,
					god_print_param.god_print_data.item_id, god_print_param.GetAttrInfoLog(&god_print_param.god_print_data.main_attr, 1),
					god_print_param.GetAttrInfoLog(god_print_param.god_print_data.add_attr_list, god_print_param.god_print_data.attr_num));
			}
			break;
		case I_TYPE_TREASURE_MAP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{TreasureMapItemParam:{scene_id:%d, pos_x:%d, pos_y:%d}}",
					treasure_map_param.scene_id, treasure_map_param.pos_x, treasure_map_param.pos_y);
			}
			break;

		case I_TYPE_PARTNER_NATAL_CARD:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PartnerNatalCardParam:{%s}}",
					partner_natal_card_param.GetAttrInfoLog(partner_natal_card_param.m_voc_list, MAX_PARTNER_NATAL_CARD_VOC_NUM));
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ElementCrystalParam:{used_times:%d}}",
					element_crystal_param.used_times);
			}
			break;

		case I_TYPE_EQUIPMENT:
			{
				UNSTD_STATIC_CHECK(OLD_EQUIP_STONE_MOUNT_MAX == 2);
				UNSTD_STATIC_CHECK(NEW_EQUIP_STONE_MOUNT_MAX == 4);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{EquipmentParam:{is_treasure:%d, creator:%s, sp_id:%d, "
					"new_stone:[%d,%d,%d,%d], attr:[%s], extra_attr:[%s] suit_effect_id:[%d] is_strong_create:[%d] strengthen_level[%d]}}",
					equip_param.is_treasure, equip_param.creator_name, equip_param.sp_id,
					equip_param.new_stone[0], equip_param.new_stone[1], equip_param.new_stone[2], equip_param.new_stone[3],
					equip_param.GetAttrInfoLog(equip_param.attr_list, equip_param.attr_num, equip_param.ultimate_attr_list),
					equip_param.GetAttrVocInfoLog(equip_param.attr_extra_inc, equip_param.inc_num),
					(int)equip_param.new_suit_effect_id, (int)equip_param.is_strong_create, equip_param.strengthen_level);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				UNSTD_STATIC_CHECK(OLD_EQUIP_STONE_MOUNT_MAX == 2);
				UNSTD_STATIC_CHECK(NEW_EQUIP_STONE_MOUNT_MAX == 4);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{JewelryParam:{is_treasure:%d, creator:%s,  attr:[%s],"
					"extra_attr:[%s] new_stone:[%d,%d,%d,%d] suit_effect_id:[%d] is_strong_create:[%d] strengthen_level[%d]}}",
					jewelry_param.is_treasure, jewelry_param.creator_name,
					jewelry_param.GetAttrInfoLog(jewelry_param.attr_list, jewelry_param.attr_num, jewelry_param.ultimate_attr_list),
					jewelry_param.GetAttrVocInfoLog(jewelry_param.attr_extra_inc, jewelry_param.inc_num),
					jewelry_param.new_stone[0], jewelry_param.new_stone[1], jewelry_param.new_stone[2], jewelry_param.new_stone[3],
					(int)jewelry_param.new_suit_effect_id, (int)jewelry_param.is_strong_create, jewelry_param.strengthen_level);
			}
			break;

		case I_TYPE_PET:
			{
				UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetParam:{pet_id:%d, name:%s, str_level:%d, level:%d, exp:%lld, add_exp:%lld, "
					"maxhp:%d, maxmp:%d, pet_growth_version[%d], passive_list:{%s}, growth_list:[%d,%d,%d,%d,%d], comprehend_num:%d, comprehend_value:%d, "				
					"attr:[%d,%d,%d,%d,%d] "
					"m_soul_equip_1[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s], "
					"m_soul_equip_2[itemid:%d is_bind:%d is_lock:%d unique_id:%lld exp:%d max_exp:%d level:%d gread:%d refine:%d sp_id:%d attr[%s]"
					"lian_yao_pet_id:%d lian_yao_pet_str_lv:%d feisheng_times:%d feisheng_skill_id:%d feisheng_skill_lv:%d"
					"}}", pet_param.pet_id, pet_param.name, (int)pet_param.str_level, (int)pet_param.level,
					pet_param.exp, pet_param.add_exp, pet_param.max_hp, pet_param.max_mp, (int)pet_param.pet_growth_version,
					pet_param.GetPassiveInfoLog(pet_param.passive_list, PET_PASSIVE_MAX_COUNT),
					pet_param.growth[0], pet_param.growth[1], pet_param.growth[2], pet_param.growth[3], pet_param.growth[4],
					pet_param.comprehend_gift_num, pet_param.comprehend_passive_value,
					pet_param.attr[0], pet_param.attr[1], pet_param.attr[2], pet_param.attr[3], pet_param.attr[4],
					pet_param.m_soul_equip_1.item_id, pet_param.m_soul_equip_1.is_bind, pet_param.m_soul_equip_1.is_lock, pet_param.m_soul_equip_1.item_unique_id,
					pet_param.m_soul_equip_1.exp, pet_param.m_soul_equip_1.m_max_exp, pet_param.m_soul_equip_1.m_level, pet_param.m_soul_equip_1.m_gread,
					pet_param.m_soul_equip_1.m_refine, pet_param.m_soul_equip_1.sp_id, pet_param.m_soul_equip_1.GetAttrInfoLog(pet_param.m_soul_equip_1.add_attr_list, pet_param.m_soul_equip_1.attr_num),					
					pet_param.m_soul_equip_2.item_id, pet_param.m_soul_equip_2.is_bind, pet_param.m_soul_equip_2.is_lock, pet_param.m_soul_equip_2.item_unique_id,
					pet_param.m_soul_equip_2.exp, pet_param.m_soul_equip_2.m_max_exp, pet_param.m_soul_equip_2.m_level, pet_param.m_soul_equip_2.m_gread,
					pet_param.m_soul_equip_2.m_refine, pet_param.m_soul_equip_2.sp_id, pet_param.m_soul_equip_2.GetAttrInfoLog(pet_param.m_soul_equip_2.add_attr_list, pet_param.m_soul_equip_2.attr_num),
					pet_param.lian_yao_pet_id, pet_param.lian_yao_pet_str_lv, 
					pet_param.fei_sheng_times, pet_param.fei_sheng_select_skill_id, pet_param.fei_sheng_select_skill_level 
				);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{ExpCrystalParam:{remain_times:%d, is_turn_on:%d}}",
					exp_crystal_param.remain_times, exp_crystal_param.is_turn_on);
			}
			break;

		case I_TYPE_PET_EQUIPMENT:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetEquipMentParam:{has_random_net_value:%d,reserve_sh:%d reserve_ch:%d item_id:%d, skill_num:%d, skill_1:%d, skill_2:%d re_skill_1:%d, re_skill_2:%d}}",
					(int)pet_equipment_param.has_random_net_value, pet_equipment_param.reserve_sh, (int)pet_equipment_param.reserve_ch, pet_equipment_param.pet_equipment_id,
					pet_equipment_param.equipment_skill_num, pet_equipment_param.equipment_skill_list[0], pet_equipment_param.equipment_skill_list[1],
					pet_equipment_param.equipment_skill_refresh_list[0], pet_equipment_param.equipment_skill_refresh_list[1]);
			}
			break;
		case I_TYPE_MEDAL:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{MedalItemParam:{sp_id:%d, grade:%d second_sp_id:%d second_grade:%d}}",
					medal_param.sp_id, medal_param.grade, medal_param.second_sp_id, medal_param.special_medal_gread);
			}
			break;

		case I_TYPE_PET_SOUL_EQUIP:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetSoulEquipParam:{sp_id:%d, refine:%d, level:%d, steps:%d, exp[%d], m_max_exp[%d], attr_info[%s]}}", pet_soul_equipment_param.sp_id, pet_soul_equipment_param.refine,
					pet_soul_equipment_param.level, pet_soul_equipment_param.steps, pet_soul_equipment_param.exp, pet_soul_equipment_param.m_max_exp,
					pet_soul_equipment_param.GetAttrInfoLog(pet_soul_equipment_param.add_attr_list, pet_soul_equipment_param.attr_num));
			}
			break;

		default:
			{
				SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{NotNetValueItem}");
			}
			break;
		}

		return defalut_log_str;
	}

	static const char* GetParamLog(ItemID item_id, const NetValueItemParamData param_data)
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			return "NotValidItem";
		}

		if (NULL == param_data)
		{
			return "{NULL}";
		}

		static NetValueItemParam param;
		param.Reset();
		param.SetData(param_data);

		return param.GetParamLog(item->GetItemType());
	}

	static int GetDataLengthByItemType(int item_type)
	{
		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		int length = 0;
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				length = sizeof(FuYuParam);
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				length = sizeof(YuanHunZhuParam);
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				length = sizeof(RuneStoneParam);
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				length = sizeof(HolyEquipParam);
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				length = sizeof(ZaoHuaEquipParam);
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				length = sizeof(PeakTokenParam);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				length = sizeof(PetBadgeItemParam);
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				length = sizeof(DianHuaShiParam);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				length = sizeof(AccessoriesParam);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				length = sizeof(PetGodPrintEquipMentParam);
			}
			break;
		case I_TYPE_EQUIPMENT:
			{
				length = sizeof(EquipmentParam);
			}
			break;

		case I_TYPE_PET:
			{
				length = sizeof(PetParam);
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				length = sizeof(ElementCrystalParam);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				length = sizeof(JewelryParam);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				length = sizeof(ExpCrystalParam);
			}
			break;

		case I_TYPE_TREASURE_MAP:
			{
				length = sizeof(TreasureMapItemParam);
			}
			break;

		case I_TYPE_PARTNER_NATAL_CARD:
			{
				length = sizeof(NewPartnerNatalCardParam);
			}
			break;

		case I_TYPE_PET_EQUIPMENT:
			{
				length = sizeof(PetEquipMentParam);
			}
			break;

		case I_TYPE_MEDAL:
			{
				length = sizeof(MedalItemParam);
			}
			break;
		case I_TYPE_PET_SOUL_EQUIP:
			{
				length = sizeof(PetSoulEquipMentParam);
			}
			break;

		default:
			{
				length = 0;
			}
			break;
		}

		return length;
	}

	int GetDataByItemType(int item_type, NetValueItemParamData & out_data) const
	{
		UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
		int length = 0;
		switch (item_type)
		{
		case I_TYPE_FU_YU:
			{
				memcpy(out_data, &fu_yu_param, sizeof(fu_yu_param));
				length = sizeof(fu_yu_param);
			}
			break;
		case I_TYPE_YUAN_HUN_ZHU:
			{
				memcpy(out_data, &yuan_hun_zhu_param, sizeof(yuan_hun_zhu_param));
				length = sizeof(yuan_hun_zhu_param);
			}
			break;
		case I_TYPE_RUNE_STONE:
			{
				memcpy(out_data, &rune_stone_param, sizeof(rune_stone_param));
				length = sizeof(rune_stone_param);
			}
			break;
		case I_TYPE_HOLY_EQUIP:
			{
				memcpy(out_data, &holy_equip_param, sizeof(holy_equip_param));
				length = sizeof(holy_equip_param);
			}
			break;
		case I_TYPE_ZAO_HUA_EQUIP:
			{
				memcpy(out_data, &zao_hua_param, sizeof(zao_hua_param));
				length = sizeof(zao_hua_param);
			}
			break;
		case I_TYPE_PEAK_TOKEN:
			{
				memcpy(out_data, &peak_token_param, sizeof(peak_token_param));
				length = sizeof(peak_token_param);
			}
			break;
		case I_TYPE_PET_BADGE:
			{
				memcpy(out_data, &pet_badge_item_param, sizeof(pet_badge_item_param));
				length = sizeof(pet_badge_item_param);
			}
			break;
		case I_TYPE_DIAN_HUA_SHI:
			{
				memcpy(out_data, &dian_hua_shi_param, sizeof(dian_hua_shi_param));
				length = sizeof(dian_hua_shi_param);
			}
			break;
		case I_TYPE_ACCESSORIES:
			{
				memcpy(out_data, &accessoriess_param, sizeof(accessoriess_param));
				length = sizeof(accessoriess_param);
			}
			break;
		case I_TYPE_GOD_PRINT:
			{
				memcpy(out_data, &god_print_param, sizeof(PetGodPrintEquipMentParam));
				length = sizeof(PetGodPrintEquipMentParam);
			}
			break;
		case I_TYPE_PET_SOUL_EQUIP:
			{
				memcpy(out_data, &pet_soul_equipment_param, sizeof(PetSoulEquipMentParam));
				length = sizeof(PetSoulEquipMentParam);
			}
			break;
		case I_TYPE_EQUIPMENT:
			{
				memcpy(out_data, &equip_param, sizeof(EquipmentParam));
				length = sizeof(EquipmentParam);
			}
			break;
		case I_TYPE_PET:
			{
				memcpy(out_data, &pet_param, sizeof(PetParam));
				length = sizeof(PetParam);
			}
			break;

		case I_TYPE_ELEMENT_CRYSTAL:
			{
				memcpy(out_data, &element_crystal_param, sizeof(ElementCrystalParam));
				length = sizeof(ElementCrystalParam);
			}
			break;

		case I_TYPE_JEWELRY:
			{
				memcpy(out_data, &jewelry_param, sizeof(JewelryParam));
				length = sizeof(JewelryParam);
			}
			break;

		case I_TYPE_EXP_CRYSTAL:
			{
				memcpy(out_data, &exp_crystal_param, sizeof(ExpCrystalParam));
				length = sizeof(ExpCrystalParam);
			}
			break;

		case I_TYPE_TREASURE_MAP:
			{
				memcpy(out_data, &treasure_map_param, sizeof(TreasureMapItemParam));
				length = sizeof(TreasureMapItemParam);
			}
			break;
		case I_TYPE_PARTNER_NATAL_CARD:
			{
				memcpy(out_data, &partner_natal_card_param, sizeof(NewPartnerNatalCardParam));
				length = sizeof(NewPartnerNatalCardParam);
			}
			break;
		case I_TYPE_PET_EQUIPMENT:
			{
				memcpy(out_data, &pet_equipment_param, sizeof(PetEquipMentParam));
				length = sizeof(PetEquipMentParam);
			}
			break;
		case I_TYPE_MEDAL:
			{
				memcpy(out_data, &medal_param, sizeof(medal_param));
				length = sizeof(medal_param);
			}
			break;
		default:
			{
				length = 0;
			}
			break;
		}

		return length;
	}

	void* operator new(size_t c);
	void operator delete(void* m);

	NetValueItemParamData data;
	EquipmentParam equip_param;
	PetParam pet_param;
	ElementCrystalParam element_crystal_param;
	JewelryParam jewelry_param;
	ExpCrystalParam exp_crystal_param;
	TreasureMapItemParam treasure_map_param;
	NewPartnerNatalCardParam partner_natal_card_param;
	PetEquipMentParam pet_equipment_param;
	MedalItemParam medal_param;
	PetSoulEquipMentParam pet_soul_equipment_param;
	PetGodPrintEquipMentParam god_print_param;
	AccessoriesParam accessoriess_param;
	DianHuaShiParam dian_hua_shi_param;
	PetBadgeItemParam pet_badge_item_param;
	PeakTokenParam peak_token_param;
	ZaoHuaEquipParam zao_hua_param;
	HolyEquipParam holy_equip_param;
	RuneStoneParam rune_stone_param;
	YuanHunZhuParam yuan_hun_zhu_param;
	FuYuParam fu_yu_param;
};

#pragma pack(push, 4)
struct PetPro
{
	PetPro() { this->Reset(); }

	void Reset()
	{
		pet_unique_id = 0;
		index = -1;
		pet_state = -1;
		is_bind = 0;
		capability = 0;
		pet_param.Reset();
	}

	unsigned long long pet_unique_id;
	short index;				// 序号
	char pet_state;				// 宠物状态  -1无状态  0 - 跟随  1 - 出战  (2 - 4) 待机宠物(1-3)
	char is_bind;				// 是否绑定
	int capability;				// 评分

	PetParam pet_param;
};

struct EquipmentPro
{
	short index;						//!<  序号
	ItemID item_id;						//!<  物品ID 
	short num;							//!<  数量
	short is_bind;						//!<  是否绑定
	long long item_unique_id;			//!<  道具唯一ID

	EquipmentParam param;				//!<  装备净值数据
};

struct AccessoriesPro
{
	short index;						//!<  序号
	ItemID item_id;						//!<  物品ID 
	short num;							//!<  数量
	short is_bind;						//!<  是否绑定
	long long item_unique_id;			//!<  道具唯一ID

	AccessoriesParam param;
};

struct ZaoHuaPro
{
	short index;						//!<  序号
	ItemID item_id;						//!<  物品ID 
	short num;							//!<  数量
	short is_bind;						//!<  是否绑定
	long long item_unique_id;			//!<  道具唯一ID

	ZaoHuaEquipParam param;
};

struct HolyEquipPro
{
	HolyEquipPro() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		reserve_sh = 0;
		param.Reset();
	}

	ItemID item_id;						//!<  物品ID 
	short reserve_sh;

	HolyEquipParam param;
};

struct JewelryPro		//!< 灵饰 信息
{
	JewelryPro() { this->Reset(); }

	void Reset()
	{
		jewelry_type = 0;
		jewelry_item = 0;
		item_unique_id = 0;

		j_param.has_random_net_value = 0;
		j_param.is_treasure = 0;
		j_param.attr_num = 0;
		is_bind = 0;

		memset(j_param.attr_list, 0, sizeof(j_param.attr_list));
		memset(j_param.creator_name, 0, sizeof(GameName));
		j_param.inc_num = 0;
		memset(j_param.attr_extra_inc, 0, sizeof(j_param.attr_extra_inc));
	}

	short jewelry_type;			//!< 灵饰 类型
	ItemID jewelry_item;		//!< 道具ID
	long long item_unique_id;	//!<  道具唯一ID

	JewelryParam j_param;	//!< 净值数据
	int is_bind;				// 是否绑定
};

struct MedalPro
{
	MedalPro() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		index = -1;
		num = 0;
		is_bind = 0;
		medal_unique_id = 0;
		param.Reset();
	}
	ItemID item_id;
	short index;
	short num;
	short is_bind;
	long long medal_unique_id;

	MedalItemParam param;
};


struct GodPrintListPro
{
	GodPrintListPro() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < MAX_PET_GOD_PRONT_ONE_SCHEME_NUM; i++)
		{
			god_print_list[i].Reset();
		}
	}
	PetGodPrintEquipMentParam god_print_list[MAX_PET_GOD_PRONT_ONE_SCHEME_NUM];	
};

struct InfoType
{
	ItemID			item_id;			//!< 物品id
	short			column;				//!< 物品在哪个背包栏中 [0,9)
	short			index;				//!< 物品在背包中的序号
	char			is_market_buy_flag;	//!< 是否从市场购买标识 0:否 1:从市场购买
	char			reserve_ch;
	int				num;				//!< 物品数量
	char			is_bind;			//!< 是否绑定  1为绑定 0为非绑
	char			has_param;			//!< 是否有净值参数 有为1，无为0   \note 净值就是像装备那样有自己的数值，每件物品都需要看成不一样的
	short			param_length;		//!< 净值参数的数据长度 （当has_param为1时可读）
	UInt32		    invalid_time;		//!< 过期时间
	long long       item_unique_id;		//!< 道具唯一ID
	NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
};

struct InfoType2
{
	ItemID			item_id;			//!< 物品id
	short			index;				//!< 物品在背包中的序号
	char			is_market_buy_flag;	//!< 是否从市场购买标识 0:否 1:从市场购买
	char			reserve_ch;
	short			reserve_sh;
	int				num;				//!< 物品数量
	char			is_bind;			//!< 是否绑定  1为绑定 0为非绑
	char			has_param;			//!< 是否有净值参数 有为1，无为0   \note 净值就是像装备那样有自己的数值，每件物品都需要看成不一样的
	short			param_length;		//!< 净值参数的数据长度 （当has_param为1时可读）
	UInt32		    invalid_time;		//!< 过期时间
	long long       item_unique_id;		//!< 道具唯一ID
	NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
};

#pragma pack(pop)
// PS: 注意，NetValueItemParam中的data的长度必须是最大的，否则将会出现不可知的数据问题
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) % 4 == 0);

UNSTD_STATIC_CHECK(20 == NET_VALLUE_ITEM_COUNT);
UNSTD_STATIC_CHECK(sizeof(RuneStoneParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(HolyEquipParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ZaoHuaEquipParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(EquipmentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ElementCrystalParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(JewelryParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ExpCrystalParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(TreasureMapItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PartnerNatalCardParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(MedalItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(NewPartnerNatalCardParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetSoulEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(ZhuLinParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetGodPrintEquipMentParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PetBadgeItemParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(PeakTokenParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(YuanHunZhuParam) <= sizeof(NetValueItemParamData));
UNSTD_STATIC_CHECK(sizeof(FuYuParam) <= sizeof(NetValueItemParamData));

typedef char GodPrintListProHex[sizeof(GodPrintListPro) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GodPrintListProHex) < 1024);

#endif