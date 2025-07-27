#ifndef __PET_DEF_H__
#define __PET_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"

#pragma pack(push, 4)

static const int PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES = 3;	//每次飞升的阶段数

enum PET_EXCLUSIVE_SKILL_TYPE
{
	PET_EXCLUSIVE_SKILL_TYPE_ZHU_DONG,
	PET_EXCLUSIVE_SKILL_TYPE_PASSIVE,
};

enum PET_SKILL_PRIORITY_TYPE
{
	PET_SKILL_PRIORITY_TYPE_LOW = 0,			//低级技能书
	PET_SKILL_PRIORITY_TYPE_HIGH = 1,			//高级技能书
	PET_SKILL_PRIORITY_TYPE_SUPER = 2,			//超级技能书

	PET_SKILL_PRIORITY_TYPE_MAX
};

struct PetExclusiveSkill
{
	PetExclusiveSkill()
	{
		skill_id = 0;


		skill_level = 0;
		type = 0;
	}


	int	skill_id;	
	short skill_level;
	short type;
};

enum PET_REFINED_FLAG_TYPE
{
	PET_REFINED_FLAG_TYPE_QUALIFICATIONS = 1,		//资质
	PET_REFINED_FLAG_TYPE_PASSIVE = 1 << 1,			//被动
};

enum PET_PASSIVE_SOURCE_TYPE
{
	PET_PASSIVE_SOURCE_TYPE_DEFAULT = 0,		//默认
	PET_PASSIVE_SOURCE_TYPE_COMPREHEND = 1,	//领悟来的
};

enum GUIDE_PET_FLAG
{
	GUIDE_PET_FLAG_TRUE = 1,		//是新手宠物
	GUIDE_PET_FLAG_FIRST_REFINE = 1 << 1 ,		//新手宠物第一次洗练
	GUIDE_PET_FLAG_REPLACE_PASSIVE_SKILL = 1 << 2,		//新手指引打书
	GUIDE_PET_FLAG_COMPREHEND = 1 << 3,		//新手宠物领悟技能
};

struct PetPassiveItemInfo
{
	void Reset()
	{
		passive_skill_id = 0;
		passive_source = 0;
		skill_level = 0;
	}

	unsigned short passive_skill_id;
	unsigned char passive_source;		//0是自带，1是领悟的  ,这个字段没用，现在不限制宠物领悟技能个数，限制宠物总的技能个数
	unsigned char skill_level;		
};

const static int PET_PASSIVE_MAX_COUNT = 20;				//宠物最大被动个数
const static int PET_EXCLUSIVE_SKILL_MAX = 2;				//宠物专属技能最大个数
const static int PET_LIAN_YAO_PASSIVE_MAX_COUNT = 8;				//宠物炼妖增加最大被动个数

const static int PET_MAX_COMPREHEND_PASSIVE_NUM = 2;		//宠物最大领悟技能的数量
const static int PET_SKILL_LEVLE_PARAM = 10000;
const static int PET_STRENGTHEN_MAX_LEVEL = 15;			// 宠物最大强化等级
const static int MAX_PET_QUALITY_NUM = 6;				//宠物最高品质 [1,6)

const static int MAX_FIGHT_AND_STAND_BY_PET_NUM = 5;
const static int PET_STRENGTHEN_BUFF_NEED_MIN_NUM = 3;	//宠物强化共鸣所需最小数量
UNSTD_STATIC_CHECK(PET_STRENGTHEN_BUFF_NEED_MIN_NUM >= 1);

const static int PET_ID_MAX_COUNT = 500;  //图鉴记录获取宠物id最大数量

const static int MAX_PET_BENEDICTION__NUM = 3;			// 单个宠物最多可祝福数量
static const int MAX_QUERY_ATTR_NUM = 7;

static const int MAX_PET_TALENT_GROUP_NUM = 20;			// 宠物天赋组数量
static const int MAX_PET_TALENT_GROUP_SKILL_NUM = 3;	// 宠物单个天赋组中技能数量


struct PetTalentInfo
{
	void Reset()
	{
		inborn_grid = 0;
		inborn_level = 0;
		reserve_sh = 0;
	}

	unsigned char inborn_grid;			//对应配置天赋1-3  [1,3]
	unsigned char inborn_level;
	short reserve_sh;
};

enum QUERY_ATTR_TYPE
{
	QUERY_ATTR_TYPE_MAX_HP = 0,			//最大生命值
	QUERY_ATTR_TYPE_MAX_MP = 1,			//最大法力值
	QUERY_ATTR_TYPE_ATTACK = 2,			//攻击
	QUERY_ATTR_TYPE_DEFENSE = 3,		//防御
	QUERY_ATTR_TYPE_AGILITY = 4,		//速度
	QUERY_ATTR_TYPE_MENTAL = 5,			//精神
	QUERY_ATTR_TYPE_RECOVERY = 6,		//回复

	QUERY_ATTR_TYPE_MAX
};

UNSTD_STATIC_CHECK(QUERY_ATTR_TYPE_MAX == MAX_QUERY_ATTR_NUM);

struct QueryAttrList
{
	QueryAttrList() { this->Reset(); }

	void Reset()
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	Attribute attr_list[MAX_QUERY_ATTR_NUM];
};

enum PET_FEI_SHENG_REWARD_TYPE
{
	PET_FEI_SHENG_REWARD_TYPE_ONE = 0,		//一次飞升
	PET_FEI_SHENG_REWARD_TYPE_TWO = 1,		//2次飞升
	PET_FEI_SHENG_REWARD_TYPE_STR_3 = 2,		//强化3
	PET_FEI_SHENG_REWARD_TYPE_STR_5 = 3,		//强化5
	PET_FEI_SHENG_REWARD_TYPE_MAX = 8,		//用的是char 存只能小于8
};



struct PetYiWenLuRewardParam			//哪些种类宠物完成飞升，可以领取奖励。相同变异id为一种
{
	PetYiWenLuRewardParam() { this->Reset(); }

	void Reset()
	{
		mutant_id = 0;
		finish_flag = 0;
		fetch_flag = 0;
	}

	unsigned short mutant_id;
	unsigned char finish_flag;
	unsigned char fetch_flag;
};

struct PetCommonParam
{
	PetCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int i = 0; i < ARRAY_LENGTH(pet_id_record_list); ++i)
		{
			pet_id_record_list_count = 0;
			pet_id_record_list[i] = 0;
		}

		curr_battle_fight_index = 0;
		curr_battle_standby_index1 = 0;
		curr_battle_standby_index2 = 0;
		curr_battle_standby_index3 = 0;
		extend_times = 0;
		extend_grid_num = 0;
		cur_battle_fight_cap = 0;
		curr_battle_standby_index1_cap = 0;
		curr_battle_standby_index2_cap = 0;
		curr_battle_standby_index3_cap = 0;

		curr_battle_standby_index4 = 0;
		curr_battle_standby_index5 = 0;
		curr_battle_standby_index4_cap = 0;
		curr_battle_standby_index5_cap = 0;
		for (int i = 0; i < ARRAY_LENGTH(pet_attr_list); i++)
		{
			pet_attr_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		for (int i = 0; i < ARRAY_LENGTH(benediction_pet_index_list); ++i)
		{
			benediction_pet_index_list[i] = -1;
		}
		god_print_check_flag = 0;

		fei_sheng_pet_uuid = 0;
		fei_sheng_today_is_finish = 0;
		fei_sheng_task_id = 0;
		gm_add_capacity = 0;
		fei_sheng_task_jie_duan = 0;
		ArrayCallReset(yi_wen_lu_rewards);
		reset_fei_sheng_rewards_flag = 0;
		pet_mod_ver = 0;
		is_unlock_lian_yao_furance_flag = 0;
		is_first_time_get_pet = 1;
		reserve_ch = 0;
	}
	void SetCapByFightStatusChange(int posi_status_index, int set_cap)
	{
		switch (posi_status_index)
		{
			case NEW_STANDBY_INDEX1:
			{
				curr_battle_standby_index1_cap = set_cap;
			}
			break;
			case NEW_STANDBY_INDEX2:
			{
				curr_battle_standby_index2_cap = set_cap;
			}
			break;
			case NEW_STANDBY_INDEX3:
			{
				curr_battle_standby_index3_cap = set_cap;
			}
			break;
			case NEW_STANDBY_INDEX4:
			{
				curr_battle_standby_index4_cap = set_cap;
			}
			break;
			case NEW_STANDBY_INDEX5:
			{
				curr_battle_standby_index5_cap = set_cap;
			}
			break;
		}
	}

	int pet_id_record_list_count;
	unsigned short pet_id_record_list[PET_ID_MAX_COUNT];
	int curr_battle_fight_index;
	int curr_battle_standby_index1;
	int curr_battle_standby_index2;
	int curr_battle_standby_index3;
	int extend_times;				//!< 扩展宠物背包的次数
	int extend_grid_num;			//!< 已扩展的格子数
	int cur_battle_fight_cap;		
	int curr_battle_standby_index1_cap;
	int curr_battle_standby_index2_cap;
	int curr_battle_standby_index3_cap;

	int curr_battle_standby_index4;
	int curr_battle_standby_index5;
	int curr_battle_standby_index4_cap;
	int curr_battle_standby_index5_cap;

	QueryAttrList pet_attr_list[NEW_PET_STATUS_MAX];
	long long reserve_ll[2];
	
	short benediction_pet_index_list[ItemNamespace::MAX_PET_GRID_NUM];

	int god_print_check_flag;
	long long fei_sheng_pet_uuid;
	short fei_sheng_today_is_finish;
	short fei_sheng_task_id;
	int gm_add_capacity;
	int fei_sheng_task_jie_duan;

	PetYiWenLuRewardParam yi_wen_lu_rewards[100];
	int reset_fei_sheng_rewards_flag;
	int pet_mod_ver;
	unsigned short is_unlock_lian_yao_furance_flag; //解除炼妖炉子flag
	unsigned char is_first_time_get_pet; 
	unsigned char reserve_ch;
};
typedef char PetCommonParamHex[sizeof(PetCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetCommonParamHex) < 4096);

UNSTD_STATIC_CHECK(ItemNamespace::MAX_PET_GRID_NUM == 100);
UNSTD_STATIC_CHECK(8 == NEW_PET_STATUS_MAX);

//-------------------------宠物皮肤（存库）---------------------------------
const static int PET_SKIN_MAX_NUM = 100;
struct PetSkinParam
{
	PetSkinParam()
	{
		this->Reset();
	}

	void Reset()
	{
		pet_skin_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(pet_skin_list); ++i)
		{
			pet_skin_list[i] = 0;
		}
	}

	int pet_skin_count;
	int pet_skin_list[PET_SKIN_MAX_NUM];
};

typedef char PetSkinParamHex[sizeof(PetSkinParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetSkinParamHex) < 2048);

static const int PET_AUTO_MOVE_SAVE_NUM = 240;
struct PetAutoMove
{
	PetAutoMove() : auto_move_type(0), auto_skill_id(0), auto_skill_lv(0), reserve_sh(0), pet_id(0) { }

	short auto_move_type;
	UInt16 auto_skill_id;
	short auto_skill_lv;
	short reserve_sh;
	int pet_id;
};

struct PetAutoMoveParam
{
	PetAutoMoveParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		pet_count = 0;
		memset(pet_list, 0, sizeof(pet_list));
	}

	long long reserve_ll[4];
	int pet_count;
	PetAutoMove pet_list[PET_AUTO_MOVE_SAVE_NUM];
};

typedef char PetAutoMoveParamHex[sizeof(PetAutoMoveParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetAutoMoveParamHex) < 8192);
const static int MAX_PET_GOD_PRONT_ATTR_NUM = 4;//神印副词条最大数量
struct GodPrintItem
{
	void Reset()
	{
		lock = 0;
		color = 0;
		level = 0;
		exp = 0;
		m_max_exp = 0;
		m_posi = 0;
		union_item_id = 0L;
		m_wear_pet_idx = -1;
		item_id = 0;
		main_attr.attr_type = 0;
		main_attr.attr_value = 0;

		suit_id = 0;
		attr_num = 0;
		for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM; i++)
		{
			add_attr_list[i].attr_type = 0;
			add_attr_list[i].attr_value = 0;
		}
		devour_num = 0;
		devour_attr_list.attr_type = 0;
		devour_attr_list.attr_value = 0;
	}
	bool Invalid() { return item_id == 0; }

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

	const char* GetParamLog()
	{
		static char defalut_log_str[LOG_BASE_LENGTH];
		memset(defalut_log_str, 0, sizeof(defalut_log_str));
		{
			SNPRINTF(defalut_log_str, LOG_BASE_LENGTH, "{PetGodPrintEquipMentParam:{suit_id:%d, lock:[%d],color[%d], level[%d] exp[%d] m_max_exp[%d]"
				"m_posi:[%d], m_wear_pet_idx:[%d] item_id:[%d] main_attr:[%s] add_attr_list[%s] devour_attr_list[%s]}}",
				suit_id, (int)lock, (int)color,
				level, exp, m_max_exp,
				m_posi, m_wear_pet_idx,
				item_id, GetAttrInfoLog(&main_attr, 1),
				GetAttrInfoLog(add_attr_list, attr_num),
				GetAttrInfoLog(&devour_attr_list, 1));
		}
		return defalut_log_str;
	}

	char lock;//是否锁定
	char color;//品质
	short level;//等级
	int exp;
	int m_max_exp;
	int m_wear_pet_idx;
	long long union_item_id;
	short m_posi;//穿戴部位
	ItemID item_id;
	//属性信息
	AttrInfo main_attr;

	short suit_id;//套装id
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_GOD_PRONT_ATTR_NUM];

	int devour_num;
	AttrInfo devour_attr_list;
};

struct OldGodPrintItem
{
	void Reset()
	{
		lock = 0;
		color = 0;
		level = 0;
		exp = 0;
		m_max_exp = 0;
		m_posi = 0;
		union_item_id = 0L;
		m_wear_pet_idx = -1;
		item_id = 0;
		main_attr.attr_type = 0;
		main_attr.attr_value = 0;

		suit_id = 0;
		attr_num = 0;
		for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM; i++)
		{
			add_attr_list[i].attr_type = 0;
			add_attr_list[i].attr_value = 0;
		}
	}
	char lock;//是否锁定
	char color;//品质
	short level;//等级
	int exp;
	int m_max_exp;
	int m_wear_pet_idx;
	long long union_item_id;
	short m_posi;//穿戴部位
	ItemID item_id;
	//属性信息
	AttrInfo main_attr;

	short suit_id;//套装id
	unsigned short attr_num;
	AttrInfo add_attr_list[MAX_PET_GOD_PRONT_ATTR_NUM];
};
const static int MAX_USE_ITEM_NUM = 30;//最大消耗道具数
const static int MAX_PET_GOD_PRONT_ONE_SCHEME_NUM = 5;//神印一套穿戴件数
struct GodPrintWearScheme
{
	GodPrintWearScheme() { this->Reset(); }
	void Reset()
	{
		memset(scheme_name, 0, sizeof(GameName));
		use_pet_idx = -1;
		memset(bag_idx, 0, sizeof(bag_idx));
	}
	GameName scheme_name;
	int use_pet_idx;
	int bag_idx[MAX_PET_GOD_PRONT_ONE_SCHEME_NUM];// 神印背包下标
};
const static int MAX_PET_GOD_PRONT_SCHEME_NUM = 20;//穿戴方案上限
const static int MAX_PET_GOD_PRONT_BAG_NUM = 200;	//神印背包上限
struct GodPrintBagParam
{
	GodPrintBagParam()
	{
		this->Reset();
	}
	void Reset()
	{
		exp = 0;
		today_free_times = 0;
		wish_time = 0;
		for (int i = 0; i < MAX_PET_GOD_PRONT_SCHEME_NUM; i++)
		{
			wear_scheme_list[i].Reset();
		}

		for (int i = 0; i < MAX_PET_GOD_PRONT_BAG_NUM; i++)
		{
			bag_list[i].Reset();
		}
		tranf_flag = 0;
	}

	int GetEmptyBag()
	{
		for (int i = 0; i < MAX_PET_GOD_PRONT_BAG_NUM; i++)
		{
			if (bag_list[i].item_id == 0)
			{
				return i;
			}
		}
		return -1;
	}

	int GetBagEmptyNum() 
	{
		int bag_num = 0;
		for (int i = 0; i < MAX_PET_GOD_PRONT_BAG_NUM; i++)
		{
			if (bag_list[i].item_id == 0)
			{
				bag_num += 1;
			}
		}
		return bag_num;
	}

	long long exp;//存有的经验
	short today_free_times;//今天免费次数
	short wish_time;
	GodPrintWearScheme wear_scheme_list[MAX_PET_GOD_PRONT_SCHEME_NUM];	//预存方案
	GodPrintItem bag_list[MAX_PET_GOD_PRONT_BAG_NUM];				//神印背包
	int tranf_flag;
};
typedef char GodPrintBagParamHex[sizeof(GodPrintBagParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GodPrintBagParamHex) < 28000);
const static int NEW_MAX_PET_GOD_PRONT_BAG_NUM = 500;	//神印背包上限
class GodPrintItemListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct GodPrintItemListData
	{
		char change_state_item;
		short index;

		GodPrintItem item_data;
	};

	int count;
	GodPrintItemListData item_list[NEW_MAX_PET_GOD_PRONT_BAG_NUM];
};

typedef char GodPrintItemHex[sizeof(GodPrintItem) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GodPrintItemHex) < sizeof(NetValueItemParamDataHex));//神印信息必然小于道具净值

struct WishLogData
{
	WishLogData() { this->Reset(); }
	void Reset()
	{
		wish_tamp = 0;
		item_data.Reset();
	}
	unsigned int wish_tamp;
	OldGodPrintItem item_data;
};

struct NewWishLogData
{
	NewWishLogData() { this->Reset(); }
	void Reset()
	{
		wish_tamp = 0;
		item_data.Reset();
	}
	unsigned int wish_tamp;
	GodPrintItem item_data;
};

const static int MAX_GOD_PRINT_WISH_LOG_NUM = 50;//50条日志
struct GodPrintBagWishLogParam
{
	GodPrintBagWishLogParam()
	{
		this->Reset();
	}
	void Reset()
	{
		round_seq = 0;
		log_num = 0;
		for (int i = 0; i < MAX_GOD_PRINT_WISH_LOG_NUM; i++)
		{
			m_log_list[i].Reset();
		}
	}
	int round_seq;
	int log_num;
	WishLogData m_log_list[MAX_GOD_PRINT_WISH_LOG_NUM];				//神印背包
};
typedef char GodPrintBagWishLogParamHex[sizeof(GodPrintBagWishLogParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GodPrintBagWishLogParamHex) < 8000);

struct NewGodPrintBagWishLogParam
{
	NewGodPrintBagWishLogParam()
	{
		this->Reset();
	}
	void Reset()
	{
		tranf_flag = 0;
		log_num = 0;
		for (int i = 0; i < MAX_GOD_PRINT_WISH_LOG_NUM; i++)
		{
			m_log_list[i].Reset();
		}
	}
	void PushBackData(NewWishLogData * new_data)//把所有数据后推一位
	{
		if (new_data == NULL)return;
		if (log_num < MAX_GOD_PRINT_WISH_LOG_NUM && log_num > 0)
		{
			m_log_list[log_num] = m_log_list[log_num - 1];

		}
		for (int i = log_num - 1; i > 0; i--)
		{
			m_log_list[i] = m_log_list[i - 1];
		}

		m_log_list[0] = *new_data;
		if (log_num < MAX_GOD_PRINT_WISH_LOG_NUM)log_num++;
	}
	int tranf_flag;
	int log_num;
	NewWishLogData m_log_list[MAX_GOD_PRINT_WISH_LOG_NUM];				//神印背包
};
typedef char NewGodPrintBagWishLogParamHex[sizeof(NewGodPrintBagWishLogParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewGodPrintBagWishLogParamHex) < 8000);

//宠物强化共鸣

const static int PET_STRENGTHEN_BUFF_TYPE_NUM = 4;		//宠物强化共鸣类型数量
const static int PET_STRENGTHEN_BUFF_GRID_NUM = 9;		//宠物强化共鸣每种类型格子数量

struct PetStrengthenBuffParamItem
{
	PetStrengthenBuffParamItem() { this->Reset(); }

	void Reset()
	{
		strengthen_buff_level = 0;
		ArrayFillZero(grid_pet_uuid);
	}

	int strengthen_buff_level;
	long long grid_pet_uuid[PET_STRENGTHEN_BUFF_GRID_NUM];
};

struct PetStrengthenBuffParam
{
	PetStrengthenBuffParam() { this->Reset(); }

	void Reset()
	{
		ArrayCallReset(strengthen_buff_item_list);
	}

	PetStrengthenBuffParamItem strengthen_buff_item_list[PET_STRENGTHEN_BUFF_TYPE_NUM];
};

typedef char PetStrengthenBuffParamHex[sizeof(PetStrengthenBuffParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetStrengthenBuffParamHex) < 1024);



const static int PET_GAI_MING_ATTR_MAX_NUM = 20;		//宠物改名属性最大条数,小心修改，存库用到
const static int PET_GAI_MING_ATTR_MAX_LEVEL = 10;	//宠物改名属性最大等级

#pragma pack(pop)

#endif
