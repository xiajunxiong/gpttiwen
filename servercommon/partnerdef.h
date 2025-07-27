#ifndef __PARTNER_DEF_H__
#define __PARTNER_DEF_H__

#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push, 4)

static const int PARTNER_NUM_MAX = 40;			// 最多同时拥有的伙伴数量
static const int PARTNER_SYNFIGHT_NUM_MAX = 5;	// 最大同时出战的伙伴数量
static const int PARTNER_FETTER_COUNT_MAX = 2;	// 每个伙伴所能拥有的羁绊数量 - 不可修改，需要拓展的话另起
static const int PARTNER_COMMON_SKILL_MAX = 3;	// 每个伙伴出战技能的最大数量 - 不可修改，需要拓展的话另起

static const int PARTNER_ERROR_SEQ = -1;		// 伙伴数据游标错误号
static const int PARTNER_ID_START = 1;
static const int PARTNER_HOLE_PER_LEVEL = 10;	// 命卡每阶段等级间隔

static const int PARTNER_SKIN_ONCE = 12;				// 设一个伙伴最多有12个皮肤		4的倍数
UNSTD_STATIC_CHECK(PARTNER_SKIN_ONCE <= 12);
static const int PARTNER_SKIN_MAX = PARTNER_SKIN_ONCE * PARTNER_NUM_MAX;		// 伙伴皮肤总和最大值
static const int MOVE_BYTE_BASE = 0x00000001;			// 位移基数

UNSTD_STATIC_CHECK(PARTNER_NUM_MAX <= 64)				//!< 伙伴列表常量在拓展时不要超过 64，否则数据库方面会出问题

static const int MYSTERY_STORE_MAX_SEQ = 12;			// 商店最大商品数

static const int PARTNER_QUALITY_SEGMENT_MAX_NUM = 5;	// 伙伴最高段数

static const int MAX_PARTNER_ARMS_LEVEL = 20;			// 伙伴专属武器最高等级

// 伙伴--元神契约 最大星级 1--n
static const int PARTNER_SPIRIT_PACT_MAX_STAR = 5;
// 伙伴--元神契约 仓库格子数 = 最大伙伴数 * 最大星级
static const int PARTNER_SPIRIT_PACT_MAX_GRID = PARTNER_NUM_MAX * PARTNER_SPIRIT_PACT_MAX_STAR;	

// 伙伴--元神契约 最大契约数量
static const int PARTNER_SPIRIT_PACT_MAX = 3;
// 伙伴--元神契约 最大被契约数量
static const int PARTNER_SPIRIT_BE_PACT_MAX = 2;

// 清空水晶元素
#define CLEAR_PARTNER_CRYSTAL(attr_list)								\
do																		\
{																		\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_NULL;		\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_NULL;			\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_NULL;			\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_NULL;		\
} while (0);

// 添加水晶元素
#define PARTNER_CRYSTAL_PUT(cyrstal_id,attr_list)							\
do																			\
{																			\
switch (cyrstal_id)															\
{																			\
case PARTNER_CRYSTAL_ID_WATER:												\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_PURE;			\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_FIRE:												\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_PURE;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WIND:												\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_PURE;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_EARTH:												\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_PURE;			\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WATER_AND_FIRE:										\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_EARTH_AND_WATER:									\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_WATER] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_WIND_AND_EARTH:										\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_EARTH] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
																			\
case PARTNER_CRYSTAL_ID_FIRE_AND_WIND:										\
	attr_list[BATTLE_ATTR_ELEMENT_FIRE] = PARTNER_CRYSTAL_MIX;				\
	attr_list[BATTLE_ATTR_ELEMENT_WIND] = PARTNER_CRYSTAL_MIX;				\
	break;																	\
}																			\
} while (0)



// 评分 =  生命 / 24 + 魔力 / 20 + 攻击 / 4 + 防御 / 4 + 敏捷 / 4 + 回复 / 2.4 + 精神 / 2.4
enum PARTNER_ELEMENT_TYPE		// 伙伴元素 - 只有 水火气土 四种 
{
	PARTNER_ELE_WATER = 0,		// 水元素
	PARTNER_ELE_FLAME = 1,		// 火元素
	PARTNER_ELE_STORM = 2,		// 气元素
	PARTNER_ELE_EARTH = 3,		// 土元素

	PARTNER_ELE_MAX
};

enum PARTNER_CRYSTAL_TYPE		// 伙伴的元素水晶
{
	PARTNER_CRYSTAL_WATER				= 0,			// 水的水晶
	PARTNER_CRYSTAL_FIRE				= 1,			// 火的水晶
	PARTNER_CRYSTAL_WIND				= 2,			// 风的水晶
	PARTNER_CRYSTAL_EARTH				= 3,			// 地的水晶

	PARTNER_CRYSTAL_WATER_AND_FIRE		= 4,			// 水火的水晶
	PARTNER_CRYSTAL_EARTH_AND_WATER		= 5,			// 地水的水晶
	PARTNER_CRYSTAL_WIND_AND_EARTH		= 6,			// 风地的水晶
	PARTNER_CRYSTAL_FIRE_AND_WIND		= 7,			// 火风的水晶

	PARTNER_CRYSTAL_TYPE_MAX
};

enum PARTNER_CRYSTAL_ITEMID		// 伙伴元素水晶的id
{
	PARTNER_CRYSTAL_ID_START			= 40000,

	PARTNER_CRYSTAL_ID_WATER			= PARTNER_CRYSTAL_ID_START,					// 水的水晶_id
	PARTNER_CRYSTAL_ID_FIRE				= 40001,									// 火的水晶_id
	PARTNER_CRYSTAL_ID_WIND				= 40002,									// 风的水晶_id
	PARTNER_CRYSTAL_ID_EARTH			= 40003,									// 地的水晶_id

	PARTNER_CRYSTAL_ID_WATER_AND_FIRE	= 40004,									// 水火的水晶_id
	PARTNER_CRYSTAL_ID_EARTH_AND_WATER	= 40005,									// 地水的水晶_id
	PARTNER_CRYSTAL_ID_WIND_AND_EARTH	= 40006,									// 风地的水晶_id
	PARTNER_CRYSTAL_ID_FIRE_AND_WIND	= 40007,									// 火风的水晶_id

	PARTNER_CRYSTAL_ID_END
};

enum PARTNER_CRYSTAL_VALUE	// 伙伴元素水晶数值  纯 = 10  杂 = 5  不相关元素 = 0
{
	PARTNER_CRYSTAL_NULL		= 0,
	PARTNER_CRYSTAL_MIX			= 5,
	PARTNER_CRYSTAL_PURE		= 10
};

enum PARTNER_SKILL_TYPE
{
	PARTNER_SKILL_COMM_1 = 0,		// 普通技能 1
	PARTNER_SKILL_COMM_2 = 1,		// 普通技能 2
	PARTNER_SKILL_COMM_3 = 2,		// 普通技能 3
	PARTNER_SKILL_TALENT = 3,		// 天赋技能

	PARTNER_SKILL_MAX
};

enum TALENT_SKILL_EFFECT_TYPE
{
	TALENT_SKILL_TYPE_INVALID = 0,				// 无效的天赋技能类型

	TALENT_PLAYER_ATTR_VALUE_INCR = 1,			// 增加固定属性
	TALENT_PLAYER_WITHOUT_ANY_EFF = 2,			// 没有任何效果
	TALENT_PLAYER_PASSIVE_SKILL = 3,            // 被动技能

	TALENT_SKILL_TYPE_MAX
};

enum TALENT_SKILL_TARGET_TYPE
{
	TALENT_TARGET_PLAYER = 0,		// 以玩家作为目标
	TALENT_TARGET_MYSELF = 1,		// 以自身作为目标

	TALENT_TARGET_MAX
};

enum PARTNER_ATTR		//!< 伙伴属性类型
{
	PARTNER_ATTR_MHP = 0,	// 生命
	PARTNER_ATTR_MMP = 1,	// 魔力
	PARTNER_ATTR_ATK = 2,	// 攻击
	PARTNER_ATTR_DEF = 3,	// 防御
	PARTNER_ATTR_AGI = 4,	// 敏捷
	PARTNER_ATTR_REC = 5,	// 回复
	PARTNER_ATTR_SPI = 6,	// 精神

	PARTNER_ATTR_MAX
};

static const float MARK_SCALE_VALUE_LIST[PARTNER_ATTR_MAX] = { 24.0f, 20.0f, 4.0f, 4.0f, 4.0f, 2.4f, 2.4f };

//伙伴 基础/契约 属性类型->战斗属性 对应
static const int  PARTNER_ATTR_BATTLE_HAS[PARTNER_ATTR_MAX] = {
	BATTLE_ATTR_MAX_HP,		// 6 最大血量
	BATTLE_ATTR_MAX_MP,		// 8 最大魔量
	BATTLE_ATTR_ATTACK,		// 0 攻击 
	BATTLE_ATTR_DEFENSE,	// 1 防御 
	BATTLE_ATTR_AGILITY,	// 2 敏捷 
	BATTLE_ATTR_RECOVERY,	// 3 回复
	BATTLE_ATTR_MENTAL,		// 4 精神
};
UNSTD_STATIC_CHECK(PARTNER_ATTR_MAX == ARRAY_ITEM_COUNT(PARTNER_ATTR_BATTLE_HAS));

enum PARTNER_RESI		// 伙伴抵抗类型
{
	PARTNER_RESI_TOXIC = 0,		// 剧毒抵抗
	PARTNER_RESI_SLEEP = 1,		// 催眠抵抗
	PARTNER_RESI_STONE = 2,		// 石化抵抗
	PARTNER_RESI_CHAOS = 3,		// 混乱抵抗
	PARTNER_RESI_DRUNK = 4,		// 醉酒抵抗

	PARTNER_RESI_MAX
};

enum PARTNER_AMEND		// 伙伴修正类型
{
	PARTNER_AMEN_CRI = 0,		// 必杀修正
	PARTNER_AMEN_HIT = 1,		// 命中修正
	PARTNER_AMEN_EVA = 2,		// 闪避修正
	PARTNER_AMEN_FBK = 3,		// 反击修正
	PARTNER_AMEN_MAK = 4,		// 魔攻修正
	PARTNER_AMEN_MRS = 5,		// 魔抗修正
	PARTNER_AMEN_BISHA_DMG = 6,		// 必杀伤害
	PARTNER_AMEN_TEN = 7,		// 坚韧修正	

	PARTNER_AMEN_MAX
};

enum PARTNER_PROFESSION
{
	PARTNER_PROF_NEWCOMER = 0,			// 新手
	PARTNER_PROF_PUGILIST = 1,			// 格斗士
	PARTNER_PROF_SWORDMAN = 2,			// 剑士
	PARTNER_PROF_ROMAHAWK = 3,			// 战斧斗士
	PARTNER_PROF_HORSEMAN = 4,			// 骑士
	PARTNER_PROF_MARKSMAN = 5,			// 弓箭手
	PARTNER_PROF_MAGICIAN = 6,			// 魔法师
	PARTNER_PROF_MINISTER = 7,			// 传教士
	PARTNER_PROF_SORCERER = 8,			// 巫师

	PARTNER_PROF_MAX
};

enum FETTER_QUALITY		// 羁绊品质枚举
{
	FETTER_NONE = 0,	// 非法品质

	FETTER_WHITES = 1,	// 白
	FETTER_GRASSY = 2,	// 绿
	FETTER_BLUE	  = 3,	// 蓝
	FETTER_PURPLE = 4,	// 紫
	FETTER_ORANGE = 5,	// 橙
	PETTER_RED    = 6,  // 红

	FETTER_MAX
};

enum PartnerQualityT		// 伙伴品质
{
	PQT_MIN = 0,

	PARTNERQUALITYT_WHITES		= 1,	// 白
	PARTNERQUALITYT_GRASSY		= 2,	// 绿
	PARTNERQUALITYT_BLUE		= 3,	// 蓝
	PARTNERQUALITYT_PURPLE		= 4,	// 紫
	PARTNERQUALITYT_ORANGE		= 5,	// 橙
	PARTNERQUALITYT_RED			= 6,	// 红
	PARTNERQUALITYT_GOLD		= 7,	// 金

	PQT_MAX
};

enum QUALITY_UPGRADE_TYPE  //升品类型
{
	QUALITY_TYPE_MIN					= 1,

	QUALITY_TYPE_BLUE_TO_PURPLE			= 1,			//!< 蓝  --> 紫
	QUALITY_TYPE_PURPLE_TO_ORANGE		= 2,			//!< 紫  --> 橙
	QUALITY_TYPE_ORANGE_TO_RED			= 3,			//!< 橙  --> 红
	QUALITY_TYPE_RED_TO_GOLD			= 4,			//!< 红  --> 金
	QUALITY_TYPE_GOLD					= 5,			//!< 金色

	QUALITY_TYPE_MAX,
};

enum OTHER_PARAM_TYPE
{
	LOCK_STATE_LOCKED = 0,		// 解锁状态 - 锁定
	LOCK_STATE_UNLOCK = 1,		// 解锁状态 - 解锁

	BATTLE_STATE_AWAIT = 0,		// 战斗状态 - 待机
	BATTLE_STATE_FIGHT = 1,		// 战斗状态 - 出战

	UNLOCK_TYPE_TASK = 0,		// 解锁原因 - 任务
	UNLOCK_TYPE_ITEM = 1,		// 解锁原因 - 道具
	UNLOCK_TYPE_ITEM_NOREX = 2,	// 解锁原因 - 主动道具

	MAX_TYPE_ONLY_CHP = 0,		// 只刷满当前 HP
	MAX_TYPE_ONLY_CMP = 1,		// 只刷满当前 MP
	MAX_TYPE_BOTH_TWO = 2,		// 当前 HP 和 MP 都刷满

	POP_UP_WINDOWS_NONE = 1,	// 弹窗操作 - 小进阶 - 无需弹窗
	POP_UP_WINDOWS_TRUE = 2,	// 弹窗操作 - 大进阶 - 需要弹窗

	NOT_ACTIVE = 0,				// 未激活
	HAS_ACTIVE = 1,				// 激活

	SUB_ATTR = 0,				// 减少属性
	ADD_ATTR = 1,				// 增加属性
};

enum PARTNER_NATAL_HOROSCOPE_TYPE
{
	PE_TYPE_SKILY = 0,		// 天之命格
	PE_TYPE_EARTH = 1,		// 地之命格
	PE_TYPE_HUMAN = 2,		// 人之命格

	PE_TYPE_MAX
};

enum PARTNER_EQ_SUIT_EFFECT_TYPE
{
	PE_EFFECT_INVALID = 0,			// 无效属性；
	PE_EFFECT_LIFE_STEAL = 1,		// 生命窃取；	窃取比(%)
	PE_EFFECT_PSY_DMG_AD = 2,		// 物伤附加；	附加值		附加比(%)
	PE_EFFECT_MGC_DMG_AD = 3,		// 法伤附加；	附加值		附加比(%)

	PE_EFFECT_MAX
};

struct PartnerEquipment
{
	PartnerEquipment() { this->Reset(); }

	void Reset()
	{
		partner_equip_id = 0;
		unlock_level = MAX_ROLE_LEVEL;
		attr_type = 0;
		attr_value = 0;
	}

	void ResetByPutOff()
	{
		partner_equip_id = 0;
		attr_type = 0;
		attr_value = 0;
	}

	ItemID partner_equip_id;
	UInt16 unlock_level;
	int attr_type;
	int attr_value;
};

struct PartnerCon			// 伙伴属性容器 —— 协议格式，存库格式
{
	PartnerCon() { this->Reset(); }

	void Reset()
	{
		partner_id = 0;
		is_unlocked = 0;
		fight_state = 0;

		upgrade_lv = 0;
		attr_stage = 0;

		memset(skill_list, 0, sizeof(skill_list));
		memset(curr_fetter, 0, sizeof(curr_fetter));
		memset(wash_fetter, 0, sizeof(wash_fetter));

		for (int type = 0; type < PE_TYPE_MAX; ++type)
		{
			equip_list[type].Reset();
		}
	}

	// 列表 - 4
	unsigned short partner_id;					// 伙伴 ID - [0, 65535]
	char is_unlocked;							// 解锁状态：0 - 锁定；1 - 解锁
	char fight_state;							// 战斗状态：0 - 待机；1 - 出战
	// 进阶 - 2
	unsigned char upgrade_lv;					// 进阶等级 - [0, 255](我就不信策划能给伙伴配到 255 阶)
	char attr_stage;							// 属性已经突破的阶段，[0, maxBreakStage]，进阶成功后，此数据会被重置为 0
	
	short skill_list[PARTNER_COMMON_SKILL_MAX];	// 技能启用禁用 0为禁用 1为启用

	// 情缘 - 4
	unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];	// 当前情缘
	unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];	// 洗练情缘

	// 装备 - 12 * 3 = 36
	PartnerEquipment equip_list[PE_TYPE_MAX];	// 三才命格
};

struct PartnerSql	// 存库容器，只存库 —— 存库格式
{
	PartnerSql() { this->Reset(); }

	void Reset()
	{
		cur_hp = 0;
		cur_mp = 0;
		max_hp = 0;
		max_mp = 0;
	}

	// 当前生命值和魔法值 - 8
	Attribute cur_hp;							// 当前生命值
	Attribute cur_mp;							// 当前魔法值
	Attribute max_hp;							// 最大生命值
	Attribute max_mp;							// 最大魔法值
};

struct PartnerQuality			// 伙伴升品容器
{
	PartnerQuality()
	{
		this->Reset();
	}
	void Reset()
	{
		partner_id = 0;
		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_up = 0;
	}
	int partner_id;						// 伙伴ID
	int partner_quality_type;			// 升品类型
	int old_quality;					// 原来的品质
	int new_quality;					// 新升级的品质
	int is_up;							// 是否晋升过  保留
};

struct PartnerCrytal		// 装备元素水晶
{
	PartnerCrytal()
	{
		this->Reset();
	}
	void Reset()
	{
		current_crytal_id = 0;
		use_time = 0;
		memset(att_list, 0, sizeof(att_list));
	}
	ItemID current_crytal_id;					// 元素水晶id
	short use_time;								// 剩余使用次数
	int att_list[PARTNER_ELE_MAX];				// 元素属性值 依次为[水、火、风、地]
};

struct QualitiedAtt				// 保存升品后的品质
{
	QualitiedAtt()
	{
		this->Reset();
	}

	void Reset()
	{
		partner_id = 0;
		memset(att, 0, sizeof(att));
	}

	int partner_id;
	int att[PARTNER_ATTR_MAX];
};

struct PartnerUseElementCrytalFightInfo		// 水晶下发
{
	PartnerUseElementCrytalFightInfo()
	{
		this->Reset();
	}
	void Reset()
	{
		partner_id = 0;
		crytal_id = 0;
		use_time = 0;
	}

	int partner_id;								// 伙伴id
	int crytal_id;								// 装备的水晶id
	int use_time;								// 剩余使用次数
};

struct PartnerSkinInfo
{
	enum SkinUseStatus
	{
		UNACTIVE	 = 0,
		ACTIVE		 = 1,
		PUT_ON		 = 2,
		PUT_OFF		 = 3,
	};
	PartnerSkinInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		partner_id = 0;
		size = 0;
		memset(skin_id_list, 0, sizeof(skin_id_list));
		memset(skin_use, UNACTIVE, sizeof(skin_use));
	}

	short skin_id_list[PARTNER_SKIN_ONCE];		// 拥有的皮肤id  [已激活的]
	char skin_use[PARTNER_SKIN_ONCE];			// 0 - 未激活  1 - 激活  2 - 穿戴  3- 未穿戴
	int partner_id;
	int size;									// 拥有皮肤的个数
};

struct StoreNatalCard
{
	StoreNatalCard() { this->Reset(); }
	void Reset()
	{
		buy_count = 0;
		group_id = 0;
		seq = -1;
		attr_type = -1;
		attr_value = 0;
	}
	char buy_count;
	char group_id;
	short seq;
	int attr_type;
	int attr_value;
};


/****************************** 伙伴基本信息数据结构**************************************/
struct PartnerBaseInfo
{
	enum ReverNum
	{
		LONG_LONG_REVER_MAX = 6,			// 预留保留字长度
	};

	enum InitBitUse							// 初始化使用位 一个字节  如都使用过了，不可扩大，另起
	{
		INIT_BIT_MIN	= 0,

		INIT_BIT_0		= INIT_BIT_MIN,					// 已使用 [初始化： 技能  觉醒  元素水晶]
		INIT_BIT_1		=	1,
		INIT_BIT_2		=	2,
		INIT_BIT_3		=	3,
		INIT_BIT_4		=	4,
		INIT_BIT_5		=	5,
		INIT_BIT_6		=	6,
		INIT_BIT_7		=	7,

		INIT_BIT_MAX,
	};

	PartnerBaseInfo() { this->Reset(); }

	void Reset()
	{
		partner_list.Reset();
		partner_data.Reset();
		partner_crytal.Reset();

		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_init = 0;
		skin_active_status = 0;
		cur_skin_id = 0;
		capability = 0;

		battle_pos = 0;				
		quality_segment = 0;
		is_set_partner_quality_segment_flag = 0;
		biography_open_flag = 0;
		level = 1;
		biography_fetch_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;

		for (int i = 0;i < LONG_LONG_REVER_MAX;++i)
		{
			reserved_ll[i] = 0;
		}
	}

	bool IsInvalid() const { return (partner_list.partner_id <= 0) || (partner_list.partner_id >= PARTNER_NUM_MAX); }
	unsigned short GetPartnerId()const {return partner_list.partner_id;}

	bool IsInitBitUse(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		if ((is_init & (MOVE_BYTE_BASE << bit)) != (MOVE_BYTE_BASE << bit)) return false;

		return true;
	}
	bool InitBitSet(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		
		is_init |= (MOVE_BYTE_BASE << bit);

		return true;
	}

	void InitBitClear(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return;

		is_init &= ~(MOVE_BYTE_BASE << bit);

	}

	void InitBitClearAll()
	{
		is_init &= (~is_init);
	}

	PartnerCon partner_list;
	PartnerSql partner_data;

	char partner_quality_type;			// 升品类型  
	char old_quality;					// 原来的品质
	char new_quality;					// 新升级的品质
	unsigned char is_init;				// 该伙伴是否初始化(二进制) 防止新增功能时重新初始化

	PartnerCrytal partner_crytal;

	short skin_active_status;						// 皮肤激活状态  skin_status |= (1 << 皮肤seq(配置skin_seq)) 二进制  
	short cur_skin_id;								// 当前穿戴皮肤id

	int capability;									// 战斗力

	char battle_pos;								// 战斗 0:后排  1:前排
	char quality_segment;							// 段数
	char is_set_partner_quality_segment_flag;		// 伙伴觉醒修改段位是否设置标识  4:已设置
	char biography_open_flag;						// 二进制运算标识(传记) 0:没打开过 1:打开过
	int level;
	char biography_fetch_flag;						// 二进制运算标识(传记) 0:未领取 1:已领取
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	long long reserved_ll[LONG_LONG_REVER_MAX];		// 保留
};

struct PartnerParam
{
	/*************** **********************/
	PartnerParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < PARTNER_NUM_MAX;++i)
		{
			partner_base_info[i].Reset();
		}
		memset(synfight_list, 0, sizeof(synfight_list));

		store_next_auto_refresh_time = 0;
		for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; ++i)
		{
			store_natal_card[i].Reset();
		}
		new_partner_unlock_flag = 0;
		is_recover_item_flag = 0;
		is_refresh_store_flag = 0;
		tranf_id = 1;

		store_refresh_num = 0;
		store_ver = 0;
		next_free_refresh_time = 0u;

		partner_skill_tree_back_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	PartnerBaseInfo partner_base_info[PARTNER_NUM_MAX]; // partnet_id 作为索引  [0] 都是空
	int synfight_list[PARTNER_SYNFIGHT_NUM_MAX];

	unsigned int	store_next_auto_refresh_time;
	StoreNatalCard  store_natal_card[MYSTERY_STORE_MAX_SEQ];		//命格卡商店刷新内容

	char new_partner_unlock_flag;
	char is_recover_item_flag;			// 回收灵液 3:已回收
	char is_refresh_store_flag;			// 4：已刷新 （神秘商店）
	char tranf_id;

	short store_refresh_num;			// 神秘商店刷新次数(每日刷新)
	short store_ver;					// 版本号 版本不对则刷新
	unsigned int next_free_refresh_time;// 下次免费刷新时间戳

	char partner_skill_tree_back_flag;
	char reserve_ch;
	short reserve_sh;
};

typedef char PartnerParamHex[sizeof(PartnerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerParamHex) < 14000);

/******************************新伙伴基本信息数据结构***********************************/
struct PartnerNatalCardVoc
{
	int attr_type;
	int attr_value;
};

const static int MAX_PARTNER_NATAL_CARD_VOC_NUM = 4;//词条最大数
struct NewStoreNatalCard
{
	NewStoreNatalCard() { this->Reset(); }
	void Reset()
	{
		buy_count = 0;
		group_id = 0;
		seq = -1;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}
	char buy_count;
	char group_id;
	short seq;
	
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};

struct NewPartnerEquipment
{
	NewPartnerEquipment() { this->Reset(); }

	void Reset()
	{
		partner_equip_id = 0;
		unlock_level = MAX_ROLE_LEVEL;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}

	void ResetByPutOff()
	{
		partner_equip_id = 0;
		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			m_voc_list[i].attr_type = 0;
			m_voc_list[i].attr_value = 0;
		}
	}
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
	ItemID partner_equip_id;
	UInt16 unlock_level;
	
	PartnerNatalCardVoc m_voc_list[MAX_PARTNER_NATAL_CARD_VOC_NUM];
};

struct NewPartnerCon			// 伙伴属性容器 —— 协议格式，存库格式
{
	NewPartnerCon() { this->Reset(); }

	void Reset()
	{
		partner_id = 0;
		is_unlocked = 0;
		fight_state = 0;

		upgrade_lv = 0;
		attr_stage = 0;

		memset(skill_list, 0, sizeof(skill_list));
		memset(curr_fetter, 0, sizeof(curr_fetter));
		memset(wash_fetter, 0, sizeof(wash_fetter));

		for (int type = 0; type < PE_TYPE_MAX; ++type)
		{
			equip_list[type].Reset();
		}
	}

	// 列表 - 4
	unsigned short partner_id;					// 伙伴 ID - [0, 65535]
	char is_unlocked;							// 解锁状态：0 - 锁定；1 - 解锁
	char fight_state;							// 战斗状态：0 - 待机；1 - 出战
												// 进阶 - 2
	unsigned char upgrade_lv;					// 进阶等级 - [0, 255](我就不信策划能给伙伴配到 255 阶)
	char attr_stage;							// 属性已经突破的阶段，[0, maxBreakStage]，进阶成功后，此数据会被重置为 0

	short skill_list[PARTNER_COMMON_SKILL_MAX];	// 技能启用禁用 0为禁用 1为启用

												// 情缘 - 4
	unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];	// 当前情缘
	unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];	// 洗练情缘

															// 装备 - 12 * 3 = 36
	NewPartnerEquipment equip_list[PE_TYPE_MAX];	// 三才命格
};

struct NewPartnerBaseInfo
{
	enum ReverNum
	{
		LONG_LONG_REVER_MAX = 5,			// 预留保留字长度
	};

	enum InitBitUse							// 初始化使用位 一个字节  如都使用过了，不可扩大，另起
	{
		INIT_BIT_MIN = 0,

		INIT_BIT_0 = INIT_BIT_MIN,					// 已使用 [初始化： 技能  觉醒  元素水晶]
		INIT_BIT_1 = 1,
		INIT_BIT_2 = 2,
		INIT_BIT_3 = 3,
		INIT_BIT_4 = 4,
		INIT_BIT_5 = 5,
		INIT_BIT_6 = 6,
		INIT_BIT_7 = 7,

		INIT_BIT_MAX,
	};

	NewPartnerBaseInfo() { this->Reset(); }

	void Reset()
	{
		partner_list.Reset();
		partner_data.Reset();
		partner_crytal.Reset();

		partner_quality_type = 0;
		old_quality = 0;
		new_quality = 0;
		is_init = 0;
		skin_active_status = 0;
		cur_skin_id = 0;
		capability = 0;

		battle_pos = 0;
		quality_segment = 0;
		is_set_partner_quality_segment_flag = 0;
		biography_open_flag = 0;
		level = 1;
		biography_fetch_flag = 0;
		intensify_level = 0;
		soar_count = 0;

		memset(put_on_pact_id_list, 0, sizeof(put_on_pact_id_list));
		reserve_sh_1 = 0;

		reserve_sh = 0;
		soar_arms_level = 0;

		for (int i = 0; i < LONG_LONG_REVER_MAX; ++i)
		{
			reserved_ll[i] = 0;
		}
	}

	bool IsInvalid() const { return (partner_list.partner_id <= 0) || (partner_list.partner_id >= PARTNER_NUM_MAX); }
	unsigned short GetPartnerId()const { return partner_list.partner_id; }

	bool IsInitBitUse(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;
		if ((is_init & (MOVE_BYTE_BASE << bit)) != (MOVE_BYTE_BASE << bit)) return false;

		return true;
	}
	bool InitBitSet(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return false;

		is_init |= (MOVE_BYTE_BASE << bit);

		return true;
	}

	void InitBitClear(InitBitUse bit)
	{
		if (bit < INIT_BIT_MIN || bit >= INIT_BIT_MAX) return;

		is_init &= ~(MOVE_BYTE_BASE << bit);

	}

	void InitBitClearAll()
	{
		is_init &= (~is_init);
	}

	NewPartnerCon partner_list;
	PartnerSql partner_data;

	char partner_quality_type;			// 升品类型  
	char old_quality;					// 原来的品质
	char new_quality;					// 新升级的品质
	unsigned char is_init;				// 该伙伴是否初始化(二进制) 防止新增功能时重新初始化

	PartnerCrytal partner_crytal;

	short skin_active_status;						// 皮肤激活状态  skin_status |= (1 << 皮肤seq(配置skin_seq)) 二进制  
	short cur_skin_id;								// 当前穿戴皮肤id

	int capability;									// 战斗力

	char battle_pos;								// 战斗 0:后排  1:前排
	char quality_segment;							// 段数
	char is_set_partner_quality_segment_flag;		// 伙伴觉醒修改段位是否设置标识  4:已设置
	char biography_open_flag;						// 二进制运算标识(传记) 0:没打开过 1:打开过
	int level;
	char biography_fetch_flag;						// 二进制运算标识(传记) 0:未领取 1:已领取
	char intensify_level;							// 强化等级	
	short soar_count;								// 伙伴飞升次数

	//元神契约物品id[装备index]
	UNSTD_STATIC_CHECK(3 == PARTNER_SPIRIT_PACT_MAX);
	ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
	short reserve_sh_1;

	short reserve_sh;
	short soar_arms_level;							// 伙伴飞升武器阶数


	long long reserved_ll[LONG_LONG_REVER_MAX];		// 保留
};

struct NewPartnerParam
{
	/*************** **********************/
	NewPartnerParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < PARTNER_NUM_MAX; ++i)
		{
			partner_base_info[i].Reset();
		}
		memset(synfight_list, 0, sizeof(synfight_list));

		store_next_auto_refresh_time = 0;
		for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; ++i)
		{
			store_natal_card[i].Reset();
		}
		new_partner_unlock_flag = 0;
		is_recover_item_flag = 0;
		is_refresh_store_flag = 0;
		reserve_ch = 0;

		store_refresh_num = 0;
		store_ver = 0;
		next_free_refresh_time = 0u;
	}

	NewPartnerBaseInfo partner_base_info[PARTNER_NUM_MAX]; // partnet_id 作为索引  [0] 都是空
	int synfight_list[PARTNER_SYNFIGHT_NUM_MAX];

	unsigned int	store_next_auto_refresh_time;
	NewStoreNatalCard  store_natal_card[MYSTERY_STORE_MAX_SEQ];		//命格卡商店刷新内容

	char new_partner_unlock_flag;
	char is_recover_item_flag;			// 回收灵液 3:已回收
	char is_refresh_store_flag;			// 4：已刷新 （神秘商店）
	char reserve_ch;

	short store_refresh_num;			// 神秘商店刷新次数(每日刷新)
	short store_ver;					// 版本号 版本不对则刷新
	unsigned int next_free_refresh_time;// 下次免费刷新时间戳
};
typedef char NewPartnerParamHex[sizeof(NewPartnerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewPartnerParamHex) < 20000);

const static int MAX_PARTNER_GIFT_TREE_SKILL_NUM = 15;//天赋树最大技能数
const static int MAX_PARTNER_GIFT_TREE_LEVEL = 10;//天赋树最大等级

struct PartnerGiftTree
{
	PartnerGiftTree() { this->Reset(); }
	void Reset() 
	{
		memset(gift_tree, 0, sizeof(gift_tree));
	}

	int gift_tree[MAX_PARTNER_GIFT_TREE_SKILL_NUM];
};

struct PartnerGiftTreeParam
{
	PartnerGiftTreeParam() { this->Reset(); }
	void Reset() 
	{
		for (int i = 0; i < PARTNER_NUM_MAX; i++)
		{
			partner_gift_tree_list[i].Reset();
		}
	}

	PartnerGiftTree partner_gift_tree_list[PARTNER_NUM_MAX];
};
typedef char PartnerGiftTreeParamHex[sizeof(PartnerGiftTreeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerGiftTreeParamHex) < 5000);

//伙伴--元神契约仓库
struct PartnerSpiritPactParam
{
	PartnerSpiritPactParam() { this->Reset(); }
	void Reset()
	{
		reserve_ll = 0;
		memset(grid_list, 0, sizeof(grid_list));
	}

	struct PactGridInfo
	{
		PactGridInfo()
		{
			this->Reset();
		}
		void Reset()
		{
			item_num = 0;
			reserve_int = 0;
		}
		int item_num;
		int reserve_int;
	};

	long long reserve_ll;
	//在伙伴表里配置了物品id与的储存位置的关系
	PactGridInfo grid_list[PARTNER_SPIRIT_PACT_MAX_GRID];
};
typedef char PartnerSpiritPactParamHex[sizeof(PartnerSpiritPactParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PartnerSpiritPactParamHex) < 4096);

struct PartnerPro			// 伙伴属性容器 —— 协议格式
{
	PartnerPro() { this->Reset(); }

	void Reset()
	{
		base_info.Reset();
		capability = 0;
		quality_segment = 0;
		quality = 0;
		intensify_level = 0;
		task_finish = 0;
		//attr_count = 0;
		crytal_id = 0;
		use_time = 0;
		skin_id = 0;
		level = 0;
		//memset(attr_pair, 0, sizeof(attr_pair));
		memset(attr_pair, 0, sizeof(attr_pair));
		biography_open_flag = 0;
		biography_fetch_flag = 0;
		soar_count = 0;

		memset(put_on_pact_id_list, 0, sizeof(put_on_pact_id_list));
		reserve_sh_1 = 0;

		memset(attach_base_attr_list, 0, sizeof(attach_base_attr_list));
		soar_arms_level = 0;
		rank = -1;
	}

	NewPartnerCon base_info;

	int capability;				// 战斗力

	char quality_segment;		// 段数
	char task_finish;			// 是否完成升阶所需任务
	char quality;				// 品质
	char intensify_level;		// 强化等级
	int attr_pair[BATTLE_ATTR_MAX];
	int crytal_id;				// 装备元素水晶id
	int use_time;				// 元素水晶剩余使用次数
	int skin_id;				// 穿戴的皮肤id, 0表示未穿戴 有就发id
	int level;
	char biography_open_flag;	// 二进制运算标识(传记) 0:没打开过 1:打开过s
	char biography_fetch_flag;	// 二进制运算标识(传记) 0:未领取 1:已领取
	short soar_count;			// 伙伴飞升次数

	//元神契约物品id[装备index]
	ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
	short reserve_sh_1;

	//该伙伴用于给其契约目标增加的属性
	Attribute attach_base_attr_list[PARTNER_ATTR_MAX];

	short soar_arms_level;		// 伙伴飞升武器阶数
	short rank;					// 伙伴本服排名（从0开始，-1为未上榜）
};

struct PartnerQueryPro			//伙伴查询协议
{
	PartnerQueryPro() { this->Reset(); }

	//此处index的安全由上层逻辑保证[取消seq的查找方式，改为partner_id]
	void SetData(const NewPartnerParam & param, int partner_id_)
	{
		partner_id = partner_id_;
		upgrade_lv = param.partner_base_info[partner_id].partner_list.upgrade_lv;
		quality_lv = param.partner_base_info[partner_id].new_quality;
		capability = param.partner_base_info[partner_id].capability;
		skin_id = param.partner_base_info[partner_id].cur_skin_id;
		quality_segment = param.partner_base_info[partner_id].quality_segment;
		intensify_level = param.partner_base_info[partner_id].intensify_level;
		level = param.partner_base_info[partner_id].level;
		soar_count = param.partner_base_info[partner_id].soar_count;
		soar_arms_level = param.partner_base_info[partner_id].soar_arms_level;
	}

	void Reset()
	{
		partner_id = 0;
		upgrade_lv = 0;
		quality_lv = 0;
		capability = 0;
		skin_id = 0;
		quality_segment = 0;
		intensify_level = 0;
		level = 0;
		soar_count = 0;
		soar_arms_level = 0;
		rank = -1;
	}

	unsigned short partner_id;					// 伙伴 ID - [0, 65535]
	unsigned char upgrade_lv;					// 进阶等级 - [0, 255](我就不信策划能给伙伴配到 255 阶)
	unsigned char quality_lv;					// 保留对齐
	int capability;								// 伙伴评分
	short skin_id;								// 穿戴的皮肤id， 0为未穿戴
	char quality_segment;						// 段数
	char intensify_level;						// 强化等级
	short level;								// 伙伴等级
	short soar_count;							// 伙伴飞升次数
	short soar_arms_level;						// 伙伴飞升武器阶数
	short rank;									// 伙伴本服排名，从0开始，-1为未上榜
};

#pragma pack(pop)
#endif