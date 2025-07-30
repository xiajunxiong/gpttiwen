#ifndef __PETCONFIG_HPP__
#define __PETCONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"
#include "gamedef.h"
#include "servercommon/configcommon.h"
#include <map>
#include <vector>
#include <set>
#include "gameworld/item/itembase.h"

static const int PET_SKILL_NUM = 4;
static const int PET_ID_BEGIN = 50000;
static const int MAX_PET_STUFF_NUM = 8;
static const int MAX_PET_BAG_EXTEND_TIMES = 50;

enum PET_QUALITY_LEVEL
{
	NORMAL_QUALITY_PET,
	SILVER_QUALITY_PET,
	GOLDEN_QUALITY_PET,
};

enum PET_SKILL_EFFECT_ID		// ���＼��Ч��id
{
	FLAME_MAGIC = 1,				// ��ϵħ��
	WATER_MAGIC = 2,				// ˮϵħ��
	EARTH_MAGIC = 3,				// ��ϵħ��
	STORM_MAGIC = 4,				// ��ϵħ��

	STONE_BUFF_SKILLS = 5,			// ʯ��ħ��
	SLEEP_BUFF_SKILLS = 6,			// ��˯ħ��
	DRUNK_BUFF_SKILLS = 7,			// ���ħ��
	TOXIC_BUFF_SKILLS = 8,			// �ж�ħ��
	CHAOS_BUFF_SKILLS = 9,			// ����ħ��
};

enum PET_ATTR_ID				// ��������
{
	MAXHP = 1,						//����ֵ
	MAXMP = 2,						//ħ��ֵ
	ATTACK = 3,						//����
	DEFENSE = 4,					//����
	AGILE = 5,						//����
	SPIRIT = 6,						//����
	REPLY = 7,						//�ظ�
	CRI = 8,						//��ɱ
	TENACITY = 9,					//����
	COUNTERATTACK = 10,				//����
	HIT = 11,						//����
	DODGE = 12,						//����
	ANTI_POISONING = 13,			//���ж�
	ANTI_FOSSILIZATION = 14,		//��ʯ��
	ANTI_LETHARGY = 15,				//����˯
	ANTI_DRUNKEN = 16,				//������
	ANTI_CHAOS = 17,				//������
	MAGIC_ATTACK = 18,				//ħ��
	MAGIC_RESISTANCE = 19,			//ħ��
	PHYSICAL_INJURY = 20,			//�����˺�
	MAGIC_INJURY = 21,				//ħ���˺�
};

enum PET_QUALITY_TYPE
{
	PET_QUALITY_INVALID = 0,
	PET_QUALITY_NORMAL = 1,		//1�����տ�����
	PET_QUALITY_SILVER = 2,		//2������������
	PET_QUALITY_GOLD = 3,		//3�����𿨳���
	PET_QUALITY_RARE = 4,		//4����ϡ�г���
	PET_QUALITY_GOD = 5,		//5�������� (���޲���ϴ��)

	PET_QUALITY_MAX,
};
UNSTD_STATIC_CHECK(MAX_PET_QUALITY_NUM == PET_QUALITY_MAX);

enum CATCH_PET_REWARD_RATE_TYPE
{
	CATCH_PET_REWARD_RATE_TYPE_SHARDS,			// ����ɹ���ȡ��Ƭ����
	CATCH_PET_REWARD_RATE_TYPE_PET,				// ����ɹ���ȡ�������
	CATCH_PET_REWARD_RATE_TYPE_VARIANT_PET,		// ����ɹ�����������
	CATCH_PET_REWARD_RATE_TYPE_MAX,
};


enum PET_SKILL_ADD_ATTR_TYPE
{
	PET_SKILL_ADD_ATTR_TYPE_INVAILD,
	PET_SKILL_ADD_ATTR_TYPE_RECOVERY=1,			//�ӻظ�����
	PET_SKILL_ADD_ATTR_TYPE_2,
	PET_SKILL_ADD_ATTR_TYPE_3,
};

struct PetCfg
{
	PetCfg() : id(0), maxhp(0),
		maxmp(0), gongji(0), fangyu(0), minjie(0), jingshen(0), huifu(0), 
		bisha(0), bisha_dmg(0), jianren(0), fanji(0), mingzhong(0), shanduo(0),
		def_zhongdu(0), def_shihua(0), def_hunshui(0), def_jiuzui(0), def_hunluan(0),
		mogong(0), mokang(0), element_water(0), element_fire(0), element_wind(0),
		element_land(0), race(0), is_long_range_unit(0), seal_card_type(0),
		rate_common(0), rate_silver(0), rate_gold(0), pr_up_on_fail(0), 
		catch_fixed_times(0), catch_reward_rate_count(0), catch_reward_rate_shards(0), 
		catch_reward_rate_pet(0), catch_reward_rate_variant_pet(0),
		mutant_id(0), pet_mutant_times(0), quality(0), shards_item_id(0),
		catch_pet_get_shards_num(0), refined_need_item_id(0), refined_need_item_num(0),
		comprehend_id(0), prototype_pet_id(0), max_passive_skill_num(0),
		magic_pet_extra_attr_type(0), strengthen_cfg_id(0), base_score(0), benediction_id(0), 
		bendiction_attr_add_precent(0), is_normal_attack_pet(false), inborn_type(0), abandonment_cfg_id(0)
	{
		memset(name, 0, sizeof(name));
		memset(growth, 0, sizeof(growth));
		main_attribute_id = 0;

		mutant_consume_item_id = 0;
		mutant_consume_item_num = 0;
	}

	bool HasSkill(int skill_id) const;

	int id;
	GameName name;

	std::vector< PetExclusiveSkill> exclusive_skills;

	std::vector<int>       passive_group_id_vec;		// ��ȡ����ʱ��������츳����
	std::vector<int>	passive_group_id_lowest_vec;	// ��ȡ����ʱ���һ����û�õ����ͷ������
	std::vector<int>	guide_passive_vec;	// ���ֳ����츳
	std::vector<int>	guide_passive_refined_vec;	// ���ֳ����һ��ϴ��ʱ�츳

	int growth[ADD_POINT_TYPE_MAX];

	int maxhp;
	int maxmp;
	int gongji;
	int fangyu;
	int minjie;
	int jingshen;
	int huifu;
	int bisha;
	int bisha_dmg;
	int jianren;
	int fanji;
	int mingzhong;
	int shanduo;
	int def_zhongdu;
	int def_shihua;
	int def_hunshui;
	int def_jiuzui;
	int def_hunluan;
	int mogong;
	int mokang;

	//Ԫ��
	int element_water;
	int element_fire;
	int element_wind;
	int element_land;

	int race;
	int is_long_range_unit;
	
	int seal_card_type;	// ������Ҫ��ʲô������׽
	int rate_common;	// ������� �տ�
	int rate_silver;	// ������� ����
	int rate_gold;		// ������� ��
	int pr_up_on_fail;	// ʧ�ܺ���ʾ����
	int catch_fixed_times; // ��X�αض�ץ��

	int catch_reward_rate_count;			//������ʵĺ�
	int catch_reward_rate_shards;			// ����ɹ���ȡ��Ƭ����
	int catch_reward_rate_pet;			// ����ɹ���ȡ�������
	int catch_reward_rate_variant_pet;		// ����ɹ�����������

	int mutant_id;
	int pet_mutant_times;
	int quality;
	int shards_item_id;
	int catch_pet_get_shards_num;		// ����ɹ���ȡ��Ƭ����
	int refined_need_item_id;
	int refined_need_item_num;
	int comprehend_id;
	int prototype_pet_id;				// ����ĳ����ԭ����ID
	int max_passive_skill_num;

	int magic_pet_extra_attr_type;					// �Ƿ񷨳裨�����ڽ���ս��ʱ���صؼӷ�����

	int strengthen_cfg_id;		//���������ǿ������
	int base_score;				//���������
	int main_attribute_id;

	int mutant_consume_item_id;
	int mutant_consume_item_num;

	int benediction_id;						// ף��ID 0:û��ף��Ч��
	int bendiction_attr_add_precent;		// ��Ϊ����ʱ�ṩ�����Լӳɱ���(�ٷֱ�)(BATTLE_PERCENT_NUM)

	bool is_normal_attack_pet;		// �Ƿ��չ��ͳ���
	int inborn_type;				//�츳����
	int abandonment_cfg_id;			//����id
};


struct PetOtherCfg
{
	PetOtherCfg() : max_pet_bag(0), start_pet_bag(0), extend_add_num(0), 
		max_pet_on(0), min_return_exp(0), rate_return_exp(0), return_id(0), 
		relife_need(0), att_point_level(0), attr_point_init(0), pet_rename_need(0),
		exchange_strengthen_need(0), respec_item(0), common_sealcard(0), 
		silver_sealcard(0), gold_sealcard(0), first_change_min(0), first_change_max(0), first_change_mutant(0)
	{
		common_shards_item_id = 0;
		rare_fragment_item_id = 0;
		refined_mutant_rate = 0;
		skill_stage_1 = 0;
		skill_stage_2 = 0;
		auto_pet_levels = 0;
		reset_level_need_coint = 0;
		broadcast_interval = 0;
		start_broadcast_time = 0;
		world_rank = 0;
		
		for (int i = 0; i < ARRAY_LENGTH(fei_sheng_skill_id); ++i)
		{
			fei_sheng_skill_id[i] = 0;
		}
		fei_sheng_skill_change_need_coin = 0;
		
		for (int i = 0; i < ARRAY_LENGTH(fei_sheng_shu_xing_dan_use_times); ++i)
		{
			fei_sheng_shu_xing_dan_use_times[i] = 0;
		}
	}

	int max_pet_bag;					// ������������
	int start_pet_bag;					// ������ʼ����
	int extend_add_num;					// ÿ����չ��������
	int max_pet_on;					// ������������
	int min_return_exp;				// ����ת����͵ȼ�
	int rate_return_exp;				// ����ת����������
	int return_id;					// ����ת��������Ʒ
	ItemID relife_need;				// �������Ĳ���	
	int att_point_level;					// ÿ��������Ե�
	int attr_point_init;
	int pet_rename_need;				// �޸����ּ۸����ң�
	ItemID exchange_strengthen_need;	// ǿ���ȼ�ת�����Ĳ���
	ItemID respec_item;				// ϴ������

	ItemID common_sealcard;
	ItemID silver_sealcard;
	ItemID gold_sealcard;

	int common_shards_item_id;				//ͨ����Ƭ��Ʒid
	int rare_fragment_item_id;				//ϡ�г���ͨ����Ƭ
	int refined_mutant_rate;					//ϴ��������� 10000

	int skill_stage_1;
	int skill_stage_2;
	int auto_pet_levels;
	int reset_level_need_coint;

	int first_change_min;
	int first_change_max;
	int first_change_mutant;

	int fei_sheng_skill_id[3];
	int fei_sheng_skill_change_need_coin;
	int fei_sheng_shu_xing_dan_use_times[3];

	int broadcast_interval;
	int start_broadcast_time;
	int world_rank;
};

struct PetStrengthenCfg
{
	PetStrengthenCfg() 
	{
		level=0;
		rate=0;
		memset(add_growth, 0, sizeof(add_growth));
		cost_coin = 0;
		memset(add_rate, 0, sizeof(add_rate));

		maxhp = 0;
		maxmp = 0;
		gongji = 0;
		fangyu = 0;
		minjie = 0;
		jingshen = 0;
		huifu = 0;

		exclusive_skill_level = 0;
		exclusive_skill_2_level = 0;
		
		need_fei_sheng_times = 0;
	}

	int level;
	int rate;
	int add_growth[ADD_POINT_TYPE_MAX];
	int cost_coin;
	int add_rate[10];

	int maxhp;
	int maxmp;
	int gongji;
	int fangyu;
	int minjie;
	int jingshen;
	int huifu;

	int exclusive_skill_level;
	int exclusive_skill_2_level;

	int need_fei_sheng_times;
};

struct PetStrengthenBuffItemCfg
{
	int strengthen_buff_level;
	int pet_quality;
	int pet_strengthen_buff_type;
	int grid_num;

	std::vector<int> sp_id_list;
	std::vector<AttrItemCfg> attr_cfg;
};

struct PetStrengthenBuffCfg
{
	int pet_quality;
	int pet_strengthen_buff_type;
	std::map<int, PetStrengthenBuffItemCfg> item_cfg_list;
};

struct PetStrengtheConsumeCfgKey
{
	bool operator<(const PetStrengtheConsumeCfgKey & other)const
	{
		if (pet_quality < other.pet_quality)
			return true;
		if (pet_quality > other.pet_quality)
			return false;

		if (pet_strengthen_level < other.pet_strengthen_level)
			return true;
		if (pet_strengthen_level > other.pet_strengthen_level)
			return false;

		return false;
	}

	int pet_quality;
	int pet_strengthen_level;
};

struct SpecifyPetStrengtheConsumeCfgKey
{
	bool operator<(const SpecifyPetStrengtheConsumeCfgKey & other)const
	{
		if (prototype_id < other.prototype_id)
			return true;
		if (prototype_id > other.prototype_id)
			return false;

		if (pet_strengthen_level < other.pet_strengthen_level)
			return true;
		if (pet_strengthen_level > other.pet_strengthen_level)
			return false;

		return false;
	}

	int prototype_id;
	int pet_strengthen_level;
};

struct PetStrengtheConsumeCfg
{
	struct ItemCfg
	{
		int need_same_pet;
		int need_pet_quality;
		int pet_strengthen_level;
	};

	int prototype_id;
	int pet_quality;
	int pet_strengthen_level;
	int need_coin;
	std::vector<ItemCfg> consume_list;
	
};

struct PetAttrExchangeCfg
{
	PetAttrExchangeCfg()
	{
	}

	struct ItemCfg
	{
		ItemCfg() : attr_id(0), maxhp(0), maxmp(0), gongji(0), fangyu(0), minjie(0), jingshen(0), huifu(0) {}
		
		int attr_id;

		int maxhp;
		int maxmp;
		int gongji;
		int fangyu;
		int minjie;
		int jingshen;
		int huifu;
	};

	std::map<int, ItemCfg> attr_cfg;
};


struct PetExpPillCfg
{
	PetExpPillCfg()
	{
		item_id = 0;
		add_exp = 0;
		money_type = 0;
		price = 0;
	}
	int item_id;
	int add_exp;
	int money_type;
	int price;
};

struct PetExpPillInfo
{
	PetExpPillInfo() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		add_exp = 0;
		count = 0;
	}

	int item_id;
	int add_exp;
	int count;
};

struct PetComprehendPassiveCfg
{
	const static int PET_COMPREHEND_PASSIVE_MAX_RATE = 10000;

	PetComprehendPassiveCfg():need_item_id(0), rate(0), add_value(0), max_value(0)
	{
	}

	bool CanComprehendPassive(const std::set<int> & have_passive_skill) const
	{
		for (int i = 0; i < (int)comprehend_passive_list.size(); ++i)
		{
			if (have_passive_skill.find(comprehend_passive_list[i]) == have_passive_skill.end())
			{
				return true;
			}
		}
		return false;
	}

	int ComprehendOnePassive(const std::set<int> & have_passive_skill) const
	{
		std::vector<int> temp = comprehend_passive_list;
	
		for (std::vector<int>::iterator iter = temp.begin(); iter != temp.end();)
		{
			if (have_passive_skill.find(*iter) != have_passive_skill.end())
			{
				iter = temp.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		if (!temp.empty())
		{
			int r = rand() % (int)temp.size();
			return temp[r];
		}

		return 0;
	}

	int GetComprehendPassiveConsumeItemNum(int passive_num) const;

	int need_item_id;
	std::vector<int> need_item_num;
	int rate;			//ֱ�ӳɹ�����
	int add_value;		//ʧ�ܼӵĽ���ֵ
	int max_value;		//����ɹ�������
	std::vector<int> comprehend_passive_list;
};

enum PET_COMPOSE_SHARE_TYPE
{
	PET_COMPOSE_SHARE_TYPE_NO_2 = -1,		//�޶����� ����ʹ��ͨ����Ƭ (����0����Ϊ�ͻ���Ҫ��0�ֿ���ʾ��ʾ)
	PET_COMPOSE_SHARE_TYPE_NO = 0,		//����ʹ��ͨ����Ƭ
	PET_COMPOSE_SHARE_TYPE_COMMON = 1,	//ʹ����ͨ����ͨ����Ƭ
	PET_COMPOSE_SHARE_TYPE_XIYOU = 2,	//ʹ��ϡ�г���ͨ����Ƭ
	
	PET_COMPOSE_SHARE_TYPE_MAX
};

struct PetComposeCfg
{
	PetComposeCfg()
	{
		need_item_id = 0;
		personal_patch_num = 0;
		need_item_num = 0;
		can_use_common_shards = 0;
		compose_reward_id = 0;
		is_bind = 1;
	}

	int need_item_id;
	int personal_patch_num;
	int need_item_num;
	int can_use_common_shards;
	int compose_reward_id;	
	int is_bind;
	std::vector<ItemConfigData> batch_decompose_rewards;
};

struct PetComposeRewardCfg
{
	const static int MAX_RATE_COUNT = 10000;

	PetComposeRewardCfg()
	{
	}

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
			pet_id = 0;
		}

		int rate;
		int pet_id;
	};

	std::vector<ItemCfg> group_reward;
};

struct PetRandPassiveGroupCfg
{
	const static int MAX_RATE_COUNT = 10000;

	PetRandPassiveGroupCfg()
	{
	}

	struct ItemCfg
	{
		ItemCfg() :rate(0)
		{
			passive_skill_id = 0;
		}

		int rate;
		int passive_skill_id;
	};

	std::vector<ItemCfg> passive_group;
};

struct PetSkillAddCapabilityCfg
{
	PetSkillAddCapabilityCfg()
	{
		pet_level=0;
		low_skill_add=0;
		high_skill_add=0;
	}

	int pet_level;
	int low_skill_add;
	int high_skill_add;
	std::vector<int>super_skill_add;
};

struct PetLockSkillCfg
{
	PetLockSkillCfg()
	{
		pet_pin_zhi = 0;
		need_item_id = 0;
		for (int i = 0; i < ARRAY_LENGTH(need_num); ++i)
		{
			need_num[i] = 0;
		}
	}

	int GetConsume(int times) const
	{
		int index = times - 1;
		if (index < 0 || index >= ARRAY_LENGTH(need_num))
		{
			return 0;
		}

		return need_num[index];
	}

	int pet_pin_zhi;
	int need_item_id;
	int need_num[2];
};

struct PetConvertCfg
{
	PetConvertCfg(): consume_item_id(0), consume_num(0), hearsay(false)
	{}

	int consume_item_id;
	int consume_num;
	ItemConfigData convert_item;
	bool hearsay;
};

struct PetGrowthVersionCfg
{
	PetGrowthVersionCfg() : version(0)
	{
		memset(growth, 0, sizeof(growth));
	}

	int growth[ADD_POINT_TYPE_MAX];
	int version;
};

struct PetLevelCfg
{
	PetLevelCfg()
	{
		level = 0;
		exp = 0;
		is_need_breach = false;
		break_consume_id = 0;
		break_consume_num = 0;
		break_attr_id = 0;
		last_break_attr_id = 0;
	}

	int level;
	int exp;
	bool is_need_breach;
	int break_consume_id;
	int break_consume_num;
	int break_attr_id;
	int last_break_attr_id;
};

struct PetBreakAttrCfg
{
	PetBreakAttrCfg()
	{
		break_attr_id = 0;
	}

	struct AttrItem
	{
		AttrItem()
		{
			attr_id = 0;
			maxhp = 0;
			maxmp = 0;
			gongji = 0;
			fangyu = 0;
			minjie = 0;
			jingshen = 0;
			huifu = 0;
		}

		int attr_id;
		int maxhp;
		int maxmp;
		int gongji;
		int fangyu;
		int minjie;
		int jingshen;
		int huifu;
	};

	int break_attr_id;
	std::map<int, AttrItem> attr_item; 
};

struct PetStrengthenProtectCfg
{
	int pet_quality;
	int strengthen_level;
	int protect_item_id;
	int protect_item_num;
};

struct PetBenedictionSimpleCfg
{
	PetBenedictionSimpleCfg()
	{}

	std::set<int> active_set;
};

struct PetBenedictionDetailCfg
{
	PetBenedictionDetailCfg() : benediction_level(0), benediction_sp_id(0), activate_score(0), sp_score(0)
	{}

	int benediction_level;
	int benediction_sp_id;
	int activate_score;
	int sp_score;
};

struct PetFeiShengAttrCfgKey
{
	PetFeiShengAttrCfgKey(int pet_id,int fei_sheng_task_jie_duan)
	{
		this->pet_id = pet_id;
		this->fei_sheng_task_jie_duan = fei_sheng_task_jie_duan;
	}

	bool operator<(const PetFeiShengAttrCfgKey & other)const
	{
		if (pet_id < other.pet_id)
			return true;
		if (pet_id > other.pet_id)
			return false;

		if (fei_sheng_task_jie_duan < other.fei_sheng_task_jie_duan)
			return true;
		if (fei_sheng_task_jie_duan > other.fei_sheng_task_jie_duan)
			return false;

		return false;
	}

	int pet_id;
	int fei_sheng_task_jie_duan;
};

struct PetFeiShengAttrCfg
{
	int pet_id;
	int fei_sheng_task_jie_duan;
	std::vector<AttrItemCfg> attr_list;
};

struct PetFeiShengTimesAttrCfgKey
{
	PetFeiShengTimesAttrCfgKey(int pet_id, int fei_sheng_task_jie_duan_times)
	{
		this->pet_id = pet_id;
		this->fei_sheng_task_jie_duan_times = fei_sheng_task_jie_duan_times;
	}

	bool operator<(const PetFeiShengTimesAttrCfgKey & other)const
	{
		if (pet_id < other.pet_id)
			return true;
		if (pet_id > other.pet_id)
			return false;

		if (fei_sheng_task_jie_duan_times < other.fei_sheng_task_jie_duan_times)
			return true;
		if (fei_sheng_task_jie_duan_times > other.fei_sheng_task_jie_duan_times)
			return false;

		return false;
	}

	int pet_id;
	int fei_sheng_task_jie_duan_times;
};

struct PetFeiShengTimesAttrCfg
{
	int pet_id;
	int fei_sheng_times;
	std::vector<AttrItemCfg> attr_list;
};

struct PetSkillUpgradeCfgKey
{
	bool operator<(const PetSkillUpgradeCfgKey & other) const
	{
		if (skill_id < other.skill_id)
			return true;
		if (skill_id > other.skill_id)
			return false;

		if (skill_level < other.skill_level)
			return true;
		if (skill_level > other.skill_level)
			return false;

		return false;
	}

	PetSkillUpgradeCfgKey(int skill_id, int skill_level)
	{
		this->skill_id = skill_id;
		this->skill_level = skill_level;
	}

	int skill_id;
	int skill_level;
};

struct PetSkillUpgradeCfg
{
	int skill_id;
	int skill_level;
	int consume_item_id;
	int consume_num;
};

struct MutantPetCfgKey
{
	bool operator<(const MutantPetCfgKey & other) const
	{
		if (pet_mutant_id < other.pet_mutant_id)
			return true;
		if (pet_mutant_id > other.pet_mutant_id)
			return false;

		if (pet_mutant_times < other.pet_mutant_times)
			return true;
		if (pet_mutant_times > other.pet_mutant_times)
			return false;

		return false;
	}

	MutantPetCfgKey(int mutant_id,int mutant_times)
	{
		pet_mutant_id = mutant_id;
		pet_mutant_times = mutant_times;
	}

	int pet_mutant_id;
	int pet_mutant_times;
};

struct PetFeiShengJieDuanTaskCfg
{
	int start_task_id;
	int end_task_id;
};

struct PetFeiShengTaskCfg
{
	int pet_id;
	std::vector<PetFeiShengJieDuanTaskCfg> task_cfg;
};

struct PetFeiShengConditionsItemCfg
{
	int conditions_type;
	int conditions_value;
};


struct PetFeiShengConditionsCfg
{
	int fei_sheng_times;
	std::vector<PetFeiShengConditionsItemCfg> conditions;
};

enum PET_YI_WEN_LU_REWARD_CONDITION_TYPE
{
	PET_YI_WEN_LU_REWARD_CONDITION_GET = 0,
	PET_YI_WEN_LU_REWARD_CONDITION_STR = 1,
	PET_YI_WEN_LU_REWARD_CONDITION_FEI_SHENG = 2,
};

struct PetYiWenLuRewardCfg
{
	int reward_seq;
	int condition;
	int condition_value;
	std::vector<ItemConfigData> rewards;	
};

struct PetYiWenLuCfg
{
	int mutant_id;
	std::map<int, PetYiWenLuRewardCfg> rewards_cfg;
};

struct PetFeiShengSkillUpgradeCfgKey
{
	bool operator<(const PetFeiShengSkillUpgradeCfgKey & other)const
	{
		if (skill_id < other.skill_id)
			return false;
		if (skill_id > other.skill_id)
			return true;

		if (level < other.level)
			return false;
		if (level > other.level)
			return true;

		return false;
	}

	int skill_id;
	int level;
};

struct PetFeiShengSkillUpgradeCfg
{
	int skill_id;
	int level;
	int consume_item_id;
	int consume_item_num;
	int capability;

	std::vector<AttrItemCfg> attr_list;
};

struct PetAbandonmentCfgKey
{
	bool operator<(const PetAbandonmentCfgKey & other)const
	{
		if (abandonment_id < other.abandonment_id)
			return true;
		if (abandonment_id > other.abandonment_id)
			return false;

		if (strengthen_level < other.strengthen_level)
			return true;
		if (strengthen_level > other.strengthen_level)
			return false;

		return false;
	}

	int abandonment_id;
	int strengthen_level;
};

struct PetAbandonmentCfg
{
	int abandonment_id;
	int strengthen_level;

	int abandonment_get_item_id;
	int abandonment_get_item_num;
};

class PetConfig : public ILogicConfig
{
public:
	typedef std::vector<PetGrowthVersionCfg> GrowthVerCfgList;
	typedef std::map<int, GrowthVerCfgList> GrowthVerCfgMap;

public:
	PetConfig();
	virtual ~PetConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetCfg* GetPetCfg(int id);
	const PetComprehendPassiveCfg * GetPetComprehendPassiveCfg(int comprehend_id);
	const PetLevelCfg * GetLevelCfg(int level);
	const PetOtherCfg * GetOtherCfg() { return &m_other_cfg; }
	const PetOtherCfg & GetOtherCfgRef() { return m_other_cfg; }
	const PetStrengthenCfg * GetPetStrengthenCfg(int strengthen_cfg_id, int level);
	const PetStrengthenCfg * GetPetStrengthenCfgByPetId(int pet_id, int str_level);
	const PetStrengthenBuffCfg * GetPetStrengthenBuffCfg(int pet_quality);
	const PetStrengthenBuffItemCfg * GetPetStrengthenBuffCfg(int pet_quality, int level);
	int GetStrRate(int strengthen_cfg_id, int main_level, int consume_level);
	const PetAttrExchangeCfg::ItemCfg* GetAttrExchange(int role_pet_train_skill_level, int attr_id);
	const PetExpPillCfg* GetExpPillCfg(int item_id);
	int RandAttrDecValue();

	const std::map<int, int> & GetPetExPillMap() { return m_pet_exptopill_map; }
	std::vector<ItemConfigData> PutEXPtoConversionPetEXPPill(long long exp);
	std::vector<int> GetRandPassive(const std::vector<int> & passive_group_id_list);			//������list����ÿ����id����ܲ����浽�츳������һ�����˵��츳�б�
	std::set<int> GetPossiblePassiveList(const std::vector<int> & passive_group_id_list, const std::vector<int> & baodi_group_id_list); //�������п��ܵı�������
	int GetOneRandPassive(const std::vector<int> & passive_list);
	const PetComposeCfg* GetPetComposeCfg(int index);
	int GetPetGroupIdByItemId(int item_id);				//����ϳ����� �ɳ�����Ƭ��ȡ������ID
	int GetPetComposeRandPetId(int compose_reward_id);
	bool GetSealPetReward(int  ARG_IN pet_id, ItemConfigData  ARG_OUT & reward_item, bool ARG_OUT  * is_get_pet = NULL);
	int GetVariantPetByPetId(int pet_id);  //��ȡһ���������
	int GetVariantPet(int mutant_id,int mutant_times);  //��ȡһ���������

	const PetSkillAddCapabilityCfg * GetPetSkillAddCapabilityCfg(int pet_level);
	bool GetExtendPrice(int times, ARG_OUT int* price);
	int GetAttrDecNumMax() const { return m_max_attr_dec_value; }

	bool RandPetParam(int pet_id, int str_level, bool is_max, NetValueItemParam& param);
	bool RandPetGrowth(int pet_id, bool is_max, NetValueItemParam & param);

	const PetLockSkillCfg * GetPetLockSkillCfg(int pet_pin_zhi);
	const PetConvertCfg * GetPetConvertCfg(int seq);

	const PetGrowthVersionCfg* GetGrowthVersion(int pet_id, int version);
	int GetPetCurGrowthVersion(int pet_id);

	bool IsStrengthenBroadcast(int pet_id, int str_lv);

	const PetBreakAttrCfg * GetPetBreakAttrCfg(int pet_lv, bool is_break);
	const PetStrengthenProtectCfg * GetPetStrengthenProtectCfg(int pet_quality, int pet_strengthen_level);
	const PetStrengtheConsumeCfg * GetPetStrengtheConsumeCfg(int pet_quality, int pet_strengthen_level);
	const PetStrengtheConsumeCfg * GetSpecifyPetStrengtheConsumeCfg(int prototype_id, int pet_strengthen_level);

	bool IsHasBenediction(int benediction_id);
	bool IsMeetActiveCond(int benediction_id, PetBenedictionSimpleCfg simple_info);
	const PetBenedictionDetailCfg * GetBenedictionDetailCfg(int benediction_id, int score);

	int GetPrototypePetId(int pet_id);
	bool IsSamePrototypePet(int pet_id_1, int pet_id_2);
	bool IsNormalAttackPet(int pet_id);

	const PetFeiShengAttrCfg * GetPetFeiShengAttrCfg(int pet_id, int fei_sheng_task_jie_duan);
	const PetFeiShengTimesAttrCfg * GetPetFeiShengTimesAttrCfg(int pet_id, int fei_sheng_times);
	int GetFeiShengShuXingDanUseTimes(int fei_sheng_times);
	const PetSkillUpgradeCfg * GetPetSkillUpgradeCfg(int src_skill_id, int skill_level);
	int GetPetFeiShengFirstTask(int pet_id,int fei_sheng_task_jie_duan);  //�����׶�
	const PetFeiShengConditionsCfg * GetPetFeiShengConditionsCfg(int fei_sheng_times);  //�����׶�

	//��������¼
	const PetYiWenLuRewardCfg * GetPetYiWenLuRewardCfg(int mutant_id, int reward_type);
	const PetYiWenLuCfg * GetPetYiWenLuCfg(int mutant_id);

	const PetFeiShengSkillUpgradeCfg * GetPetFeiShengSkillUpgradeCfg(int skill_id,int skill_level);

	const ItemID RandPetId()const;

	bool IsInCheckParamUserList(int plat_type, int role_id);
	bool IsPetHasExclusiveSkill(int pet_id, int pet_str_level, int skill_id, int skill_level);

	const PetAbandonmentCfg * GetPetAbandonmentCfg(int abandonment_id, int strengthen_level);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPetLevelCfg(TiXmlElement *RootElement);
	int InitPetStrengthenCfg(TiXmlElement *RootElement);
	int InitAttrExchangeCfg(TiXmlElement *RootElement);
	int InitPetStrengthenBuffCfg(TiXmlElement *RootElement);
	int InitExpPillCfg(TiXmlElement *RootElement);
	int InitAttrDecCfg(TiXmlElement *RootElement);
	int InitPetCfg2(TiXmlElement *RootElement);
	int InitComprehendPassiveCfg(TiXmlElement *RootElement);
	int InitPetComposeCfg(TiXmlElement *RootElement);
	int InitPetComposeRewardCfg(TiXmlElement *RootElement);
	int InitPetRandPassiveGroupCfg(TiXmlElement *RootElement);
	int InitPetSkillAddCapabilityCfg(TiXmlElement *RootElement);
	int InitPetExtendPriceCfg(TiXmlElement *RootElement);
	int InitLockSkillCfg(TiXmlElement * RootElement);
	int InitPetConvertCfg(TiXmlElement * RootElement);
	int InitGrowthVersionCfg(TiXmlElement* RootElement);
	int InitStrengthenBroadcastCfg(TiXmlElement * RootElement);
	int InitBreakAttrCfg(TiXmlElement * RootElement);
	int InitStrengthenProtectCfg(TiXmlElement * RootElement);
	int InitPetStrengtheConsumeCfg(TiXmlElement * RootElement);
	int InitSpecifyPetStrengtheConsumeCfg(TiXmlElement * RootElement);
	int InitPetBenedictionCfg(TiXmlElement * RootElement);
	int InitFeiShengAttrCfg(TiXmlElement * RootElement);
	int InitFeiShengTimesAttrCfg(TiXmlElement * RootElement);
	int InitPetUpgradeSkillCfg(TiXmlElement * RootElement);
	int InitPetFeiShengTaskCfg(TiXmlElement * RootElement);
	int InitPetFeiShengConditionsCfg(TiXmlElement * RootElement);
	int InitPetYiWenLuRewardsCfg(TiXmlElement * RootElement);
	int InitPetFeiShengSkillUpLevelCfg(TiXmlElement * RootElement);
	int InitPetParamCheckUserListCfg(TiXmlElement * RootElement);
	int InitPetBulletinCfg(TiXmlElement * RootElement);
	int InitPetAbandonmentCfg(TiXmlElement * RootElement);

	std::map<int, PetCfg> m_pet_map;
	std::map<int, PetLevelCfg> m_level_map;
	PetOtherCfg m_other_cfg;

	std::map<int,std::vector<PetStrengthenCfg> > m_pet_strengthen_map;
	std::map<int, PetAttrExchangeCfg> m_pet_attr_exchange_map;
	std::map<int, PetStrengthenBuffCfg> m_pet_strengthen_buff_map;
	std::map<int, PetExpPillCfg> m_pet_exp_pill_map;
	std::map<int, int> m_pet_exptopill_map;//����ֵ��Ӧ��Ʒid
	int m_attr_dec_total_rate;
	std::map<int, int> m_addr_dec_map;
	int m_max_attr_dec_value;

	//--------------
	std::map<int, PetComprehendPassiveCfg> m_comprehend_passive_cfg; //����
	std::vector<PetComposeCfg> m_pet_compose_cfg;					//�ϳ�����
	std::map<int, int> m_pet_compose_cfg_by_item_id;				//�ϳ�����  key:item_id second:reward_group(������id)
	std::map<int, PetComposeRewardCfg> m_pet_compose_reward_cfg;	//�ϳɷ��ų��� ���޶һ�Ҳ�ڴ�
	std::map<int, PetRandPassiveGroupCfg> m_pet_passive_group_cfg;	//����츳
	

	std::map<MutantPetCfgKey, std::vector<int> >		m_variant_pet_cfg;		//mutant_id ��Ӧ������Щ����ĳ���id

	std::vector<PetSkillAddCapabilityCfg> m_pet_skill_add_capability_cfg;

	int m_max_extend_times;
	int m_extend_price_list[MAX_PET_BAG_EXTEND_TIMES + 1];

	std::map<int, PetLockSkillCfg> m_lock_skill_cfg;		//����Ʒ�ʴ�����������
	std::vector<PetConvertCfg> m_pet_convert_cfg;			//����һ�(���޶һ�)

	GrowthVerCfgMap m_growth_version_map;

	std::map<int, int> m_strengthen_broadcast_cfg;
	std::map<int, PetBreakAttrCfg> m_break_attr_cfg;

	std::map<int, std::map<int, PetStrengthenProtectCfg> >  m_strengthen_protect_cfg;
	std::map<PetStrengtheConsumeCfgKey, PetStrengtheConsumeCfg>  m_strengthen_consume_cfg;
	std::map<SpecifyPetStrengtheConsumeCfgKey, PetStrengtheConsumeCfg>  m_specify_strengthen_consume_cfg;

	std::map<int, PetBenedictionSimpleCfg> m_benediction_simple_map;					// ����ף��Ч��������Ϣ key:benediction_id
	std::map<int, std::map<int, PetBenedictionDetailCfg> > m_benediction_detail_map;	// ����ף��Ч����ϸ��Ϣ key1:benediction_id key2:benediction_level

	std::map<PetFeiShengAttrCfgKey, PetFeiShengAttrCfg> m_fei_sheng_attr_cfg;
	std::map<PetSkillUpgradeCfgKey, PetSkillUpgradeCfg> m_pet_skill_upgrade_cfg;
	std::map<PetFeiShengTimesAttrCfgKey, PetFeiShengTimesAttrCfg> m_fei_sheng_times_attr_cfg;
	std::map<int, PetFeiShengTaskCfg> m_fei_sheng_task_cfg;
	std::map<int, PetFeiShengConditionsCfg> m_fei_sheng_conditions_cfg;

	//��������¼
	std::map<int, PetYiWenLuCfg> m_yi_wen_lu_rewards_cfg;
	std::map<PetFeiShengSkillUpgradeCfgKey, PetFeiShengSkillUpgradeCfg> m_fei_sheng_skill_up_lv_cfg;


	std::vector<UniqueUserID> m_check_param_user_list;

	//����������id
	std::vector<ItemID> m_pet_bulletin_list;

	std::map<PetAbandonmentCfgKey, PetAbandonmentCfg> m_abandonment_cfg;
};

#endif // __PETCONFIG_HPP__