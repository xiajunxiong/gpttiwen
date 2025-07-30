#ifndef __PARTNER_CONFIG_HPP__
#define __PARTNER_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"
#include "item/itempool.h"

#include <vector>
#include <map>
#include <set>

static const int PARTNER_ELEMET_NUMBER_TOTAL = 10;		// 每个伙伴的元素属性数量总和
static const int PARTNER_ATTR_BKTH_ITEM_TYPE = 2;		// 伙伴属性突破需要道具种类数

// -----------------------------------------------------------------------------------
struct PartnerOtherCfg 
{
	PartnerOtherCfg() : fetter_wash_price(0), synfight_numb_max(0), quality_upgrade_limit(0), quality_segment_limit(0), upgrade_level_limit(0), soar_arms_limit(0)
	{
		reset_level_ret_rate = 0;
		reset_level_need_coint = 0;
	}

	int fetter_wash_price;		// 情缘洗练价格 - 仙币
	int synfight_numb_max;		// 伙伴同时出战数
	int reset_level_ret_rate;
	int reset_level_need_coint;
	int quality_upgrade_limit;	//未飞升伙伴可觉醒品级限制
	int quality_segment_limit;	//未飞升伙伴可觉醒段位限制
	int upgrade_level_limit;	//未飞升伙伴可进阶等级限制
	int soar_arms_limit;		//专武开启觉醒品质限制
};

struct PartnerLevelCfg		// 伙伴基础属性
{
	PartnerLevelCfg()
	{
		comsume_money_type = 0;
		comsume_money_value = 0;
		memset(attr_list, 0, sizeof(attr_list));
		memset(resi_list, 0, sizeof(resi_list));
		memset(amen_list, 0, sizeof(amen_list));
	}

	int comsume_money_type;
	int comsume_money_value;
	Attribute attr_list[PARTNER_ATTR_MAX];		// 属性数值
	Attribute resi_list[PARTNER_RESI_MAX];		// 抵抗数值
	Attribute amen_list[PARTNER_AMEN_MAX];		// 修正数值
};

struct PartnerBasicCfg		// 伙伴基本信息配置
{
	PartnerBasicCfg() : is_valid(false), sex(-1), is_long_range_unit(0), profession(0), race_type(0), fetter_group(0),
		unlock_task(0), activation_level(0), unlock_item(0), need_num(0) , unlock_item_norex(0), quality(FETTER_NONE), 
		crystal_item(0), soar_arms_id(0), soar_task_id(0)
	{
		unlock_item_xun_bao = 0;
		memset(name, 0, sizeof(GameName));
		memset(element, 0, sizeof(element)); 
		memset(skill_list, 0, sizeof(skill_list));
	}

	bool Invalid() const { return !is_valid; }
	
	bool is_valid;					// 配置是否有效
	char sex;						// 性别
	short is_long_range_unit;		// 是否远程单位
	short profession;				// 伙伴职业
	short race_type;				// 种族类型
	short fetter_group;				// 情缘组
	unsigned short unlock_task;		// 解锁任务
	int activation_level;			// 激活时等级
	ItemID unlock_item;				// 解锁道具
	UInt16 need_num;				// 所需数量
	ItemID unlock_item_norex;		// 解锁主动道具
	ItemID unlock_item_xun_bao;		// 寻宝解锁

	short quality;					// 品质类型
	GameName name;					// 伙伴名字
	short element[PARTNER_ELE_MAX];	// 伙伴元素 - 伙伴只有四项基本元素

	ItemID crystal_item;			//伙伴自带的元素水晶id 取消了伙伴自有的元素
	UInt16 skill_list[PARTNER_COMMON_SKILL_MAX];

	int soar_arms_id;				// 飞升武器ID
	int soar_task_id;				// 飞升任务ID
};

struct PartnerUpGradeCfg	// 伙伴进阶信息配置
{
	PartnerUpGradeCfg() : player_lv_need(0), talent_skill_id(0), talent_skill_lv(0), ji_ban_seq(0)
	{}	

	int player_lv_need;			// 需要玩家等级
	int talent_skill_id;		// 天赋技能ID
	int talent_skill_lv;		// 天赋技能等级
	int ji_ban_seq;
};

struct PartnerBreakThCfg		// 属性突破配置
{
	PartnerBreakThCfg() : seq(0), attr_type(0), add_value(0) {}

	short seq;				// 突破顺序

	short attr_type;		// 属性类型
	Attribute add_value;	// 增加的属性值

	ItemPairCon need_item[PARTNER_ATTR_BKTH_ITEM_TYPE];
};

struct PartnerTalentCfg			// 天赋技能配置
{
	PartnerTalentCfg() : effect_type(0), effect_target(0), param1(0), param2(0) {}

	short effect_type;		// 效果类型
	short effect_target;	// 生效目标

	Attribute param1;		// 属性 ID  | 无效参数 |
	Attribute param2;		// 属性加值 | 无效参数 |
};

struct PartnerFetterCfg			// 情缘属性配置
{
	PartnerFetterCfg() : fetter_mark(0), need_partner(0)
	{
		memset(attr_add, 0, sizeof(attr_add));
		memset(resi_add, 0, sizeof(resi_add));
		memset(amen_add, 0, sizeof(amen_add));
	}

	short fetter_mark;		// 情缘标志 - 用于区分两个情缘之间是否同一个情缘
	short need_partner;		// 触发所需的伙伴

	Attribute attr_add[PARTNER_ATTR_MAX];	// 属性加值
	Attribute resi_add[PARTNER_RESI_MAX];	// 抵抗加值
	Attribute amen_add[PARTNER_AMEN_MAX];	// 修正加值
};

struct FettersGroupsCfg			// 情缘分组配置
{
	FettersGroupsCfg() : fetter_id(0), rand_rate(0) {}

	short fetter_id;		// 情缘 ID
	short rand_rate;		// 随机权重
};

struct QualityUpgradeCfg					// 升品配置
{
	enum KeyBaseNum   // key 相乘基数
	{
		KEY_QUALITY_NUM = 100,
		KEY_BASE_NUM = 10000
	};
	QualityUpgradeCfg(): partner_id(0), quality_upgrade_type(0), quality_segment(0), level_limit(0), need_item(0), need_num(0), add_maxhp(0),
		add_maxmp(0), add_gongji(0), add_fangyu(0), add_minjie(0), add_jingshen(0), add_huifu(0)
	{
		memset(add_attr, 0, sizeof(add_attr));
		memset(common_skill_lv_list, 0, sizeof(common_skill_lv_list));
		memset(common_skill_id_list, 0, sizeof(common_skill_id_list));
	}

	int partner_id;							// 伙伴id
	int quality_upgrade_type;				// 升品类型
	int quality_segment;					// 段数
	int level_limit;						// 等级限制
	ItemID need_item;						// 升品道具
	unsigned short need_num;				// 需要道具数量
	unsigned int add_maxhp;					// 增加生命
	unsigned int add_maxmp;					// 增加魔力
	unsigned int add_gongji;				// 增加攻击
	unsigned int add_fangyu;				// 增加防御
	unsigned int add_minjie;				// 增加敏捷
	unsigned int add_jingshen;				// 增加精神
	unsigned int add_huifu;					// 增加回复

	Attribute add_attr[PARTNER_ATTR_MAX];	// 增加属性值的数组
	int common_skill_lv_list[PARTNER_COMMON_SKILL_MAX];
	int common_skill_id_list[PARTNER_COMMON_SKILL_MAX];			//伙伴技能由进阶联系改为品质联系
};

struct PartnerLevelSkillCfg
{
	PartnerLevelSkillCfg() : gift_skill_id(0), gift_skill_level(0)
	{
	//	memset(skill_id_list, 0, sizeof(skill_id_list));
	//	memset(skill_level_list, 0, sizeof(skill_level_list));
	}

	//int skill_id_list[PARTNER_COMMON_SKILL_MAX];
	//int skill_level_list[PARTNER_COMMON_SKILL_MAX];
	int gift_skill_id;
	int gift_skill_level;
};

struct PartnerUpGradeRoleAttrCfg
{
	PartnerUpGradeRoleAttrCfg()
	{
		seq = 0;
		ji_ban_skill_id = 0;
		ji_ban_skill_level = 0;

		for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
		{
			attrs[i] = 0;
		}
	}

	int seq;
	int ji_ban_skill_id;
	int ji_ban_skill_level;
	Attribute attrs[BATTLE_ATTR_MAX];
};

struct PartnerBiographyCfg
{
	PartnerBiographyCfg(): need_upgrade(0), coin(0)
	{}

	int need_upgrade;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct PartnerComposePageCfg
{
	PartnerComposePageCfg() : num(0), reward_group_id(0)
	{}

	int num;
	int reward_group_id;
};

struct PartnerComposeCfg
{
	PartnerComposeCfg() : rate(0)
	{}

	int rate;
	ItemConfigData item;
};

struct PartnerComposeGroupCfg
{
	PartnerComposeGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<PartnerComposeCfg> node_list;
};

struct PartnerSoarCfg
{
	PartnerSoarCfg() : soar_need_level(0), soar_need_score(0), soar_need_quality_upgrade(0), soar_need_quality_segment(0),
		upgrade_level(0), soar_quality_upgrade(0), soar_quality_segment(0), attr_pellet_limit(0), arms_level(0)
	{}

	int soar_need_level;				//飞升所需伙伴等级
	int soar_need_score;				//飞升所需伙伴评分
	int soar_need_quality_upgrade;		//飞升所需伙伴品级
	int soar_need_quality_segment;		//飞升所需伙伴段数
	int upgrade_level;					//飞升后可晋阶
	int soar_quality_upgrade;			//飞升后可觉醒品级
	int soar_quality_segment;			//飞升后可觉醒段数
	int attr_pellet_limit;				//飞升后属性丹上限
	int arms_level;						//飞升后飞升武器可强化阶数
};

static const int MAX_PARTNER_SOAR_ARMS_BASE_NUM = 10000;
struct PartnerSoarArmsCfg
{
	PartnerSoarArmsCfg() : arms_skill_id(0), arms_skill_level(0)
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	
	int arms_skill_id;
	int arms_skill_level;
	std::vector<ItemConfigData> break_consume_list;
	Attribute attr_list[BATTLE_ATTR_MAX];
};

struct PartnerIntensifyCfg
{
	struct AttrData
	{
		int type;
		int param;
	};

	PartnerIntensifyCfg():levels(0), feisheng_num(0), use_coin(0), expend_item_1(0), expend_num_1(0), expend_num_2(0),
		expend_item_3(0), expend_num_3(0) {}

	short levels;
	short feisheng_num;

	int use_coin;

	ItemID expend_item_1;
	short expend_num_1;

	short expend_num_2;

	ItemID expend_item_3;
	short expend_num_3;

	std::vector<AttrData> attr_list;
	bool is_broadcast;
};

// -----------------------------------------------------------------------------------
class PartnerConfig : public ILogicConfig
{
public:
	struct PartnerCfgHead		// 伙伴配置映射 键值结构体
	{
		PartnerCfgHead(int p_id = 0, int p_lv = 0)
		{
			this->partner_id = p_id;	// 伙伴 ID | 天赋 ID
			this->partner_lv = p_lv;	// 伙伴阶级 | 天赋等级 | 传记ID
		}

		int partner_id;
		int partner_lv;

		bool operator < (const PartnerCfgHead & cobj) const
		{
			if (this->partner_id < cobj.partner_id) return true;
			else if (this->partner_id == cobj.partner_id &&
				this->partner_lv < cobj.partner_lv) return true;

			return false;
		}

		bool operator == (const PartnerCfgHead & cobj) const
		{
			return (this->partner_id == cobj.partner_id && this->partner_lv == cobj.partner_lv);
		}
	};

	typedef std::map<PartnerCfgHead, PartnerLevelSkillCfg> LevelSkillCfgMap;

	//--契约仓库IDmap <物品id, 储存位置>
	typedef std::map<ItemID, int> PactItemIDMap;
	typedef std::map<int, ItemID> PactSaveIndexMap;

public:
	PartnerConfig();
	virtual ~PartnerConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	inline int GetPartnerCount() const { return m_partner_num; }
	int GainARandFetter(int partner_id, int f_group_id, int exclude_mark = 0);

	int PartnerActiveCheck(int check_type, int param);
	int GetPartnerQualityByItemId(int item_id);
	const PartnerOtherCfg & GetPartnerOtherCfg() { return p_other; }
	const PartnerLevelCfg * GetPartnerLevelCfg(int partner, int level);
	const PartnerBasicCfg * GetPartnerBasicCfg(int partner_id);
	const PartnerUpGradeCfg * GetCurGradeAttrCfg(int partner_id, int grade_lv);
	const PartnerBreakThCfg * GetCurAttrBreakCfg(int partner_id, int grade_lv, int brk_seq);
	const PartnerTalentCfg * GetPTalentSkillCfg(int talent_id, int talent_lv);
	const PartnerFetterCfg * GetPartnerFetterCfg(int fetter_id);
	const PartnerUpGradeRoleAttrCfg * GetPartnerUpGradeRoleAttrCfg(int ji_ban_seq);
	std::vector<int> GetAllPartnerId();
	const PartnerBiographyCfg * GetPartnerBiographyCfg(int paratner_id, int biography);

	//由[伙伴id]和[伙伴升品类型]唯一确定升品加成数值
	int ___QuaKey__(int _partner_id, int _quality_up_type = QUALITY_TYPE_BLUE_TO_PURPLE, int quality_segment = 1);
	const QualityUpgradeCfg* GetParnterQualityCfg(int _getcfg_key);     //参数类型必须为  ___QuaKey__函数
	int GetCanQualityUpType(int _parttner_quality);
	const PartnerLevelSkillCfg* GetLevelSkillCfg(int partner_id, int parner_level);

	const PartnerComposePageCfg * GetComposePageCfg(int item_id);
	const PartnerComposeCfg * GetRandComposeRewardCfg(int reward_group_id);

	//伙伴--元神契约
	int GetPactSaveIndex(ItemID item_id) const;
	ItemID GetPactItemID(int save_index) const;

	const PartnerSoarCfg * GetPartnerSoarCfg(int soar_frequency);
	const PartnerSoarArmsCfg * GetPartnerSoarArmsCfg(int arms_id, int arms_level);

	//伙伴强化
	const PartnerIntensifyCfg * GetPartnerIntensifyCfg(int partner_id, int star_level);

private:
	int InitPartnerOtherCfg(TiXmlElement *RootElement);
	int InitPartnerBasicCfg(TiXmlElement *RootElement);
	int InitSkillsUnlockCfg(TiXmlElement *RootElement);
	int InitPartnerUpGradeCfg(TiXmlElement *RootElement);
	int InitPartnerBreakThCfg(TiXmlElement *RootElement);
	int InitPartnerAttribute(TiXmlElement *RootElement);
	int InitPartnerTalentCfg(TiXmlElement *RootElement);
	int InitPartnerFetterCfg(TiXmlElement *RootElement);
	int InitFettersGroupsCfg(TiXmlElement *RootElement);
	int InitPartnerQualityUpCfg(TiXmlElement *RootElement);
	int InitPartnerUpGradeRoleAttrCfg(TiXmlElement * RootElement);
	int InitPartnerBiographyCfg(TiXmlElement * RootElement);
	int InitPartnerComposeGroupCfg(TiXmlElement * RootElement);
	int InitPartnerComposePageCfg(TiXmlElement * RootElement);
	int InitPactSaveIndexCfg(TiXmlElement *RootElement);
	int InitPartnerSoarCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitPartnerSoarArmsCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitPartnerIntensifyCfg(TiXmlElement *RootElement, std::string& errormsg);

	int GetSoarArmsKey(int arms_id, int arms_level);
	int CheckFetterGroupsCfg();
	int InitLevelSkillCfgMap();
	PartnerBasicCfg * MutablePartnerBasicCfg(int partner_id);

	PartnerOtherCfg p_other;

	int m_partner_num;
	PartnerBasicCfg m_basic_cfg_list[PARTNER_NUM_MAX];	 // 伙伴基本信息表 下标就是partner_id		
	PartnerLevelCfg m_lv_attr_cfg_list[PARTNER_NUM_MAX][MAX_ROLE_LEVEL + 1];		// 伙伴基础属性表
	int max_upgrade_level;
	std::map<PartnerCfgHead, PartnerUpGradeCfg> m_s_upgrade;				// 等级进阶表
	std::map<PartnerCfgHead, std::vector<PartnerBreakThCfg> > m_v_breakth;	// 属性突破表
	std::map<PartnerCfgHead, PartnerTalentCfg> m_s_talents;					// 天赋表 - 键值是天赋技能容器
	std::map<PartnerCfgHead, PartnerBiographyCfg> m_biography_map;			// 传记配置 key.partner_lv < BIT_COUNT_OF_BYTE

	std::map<int, PartnerFetterCfg> m_s_fetters;							// 情缘表 - 键值是情缘类型 ID
	std::map<int, std::vector<FettersGroupsCfg> > m_v_f_group;				// 情缘分组表 - 键值是分组 ID

	//升品
	std::map<int, QualityUpgradeCfg> m_idqua_to_upg;				//KEY = [partner_id] * 10000 +[quality_upgrade] * 100 + quality_segment  -->   配置信息
	
	LevelSkillCfgMap m_level_skill_cfg_map;

	std::map<int, int> m_basic_cfg_map;							//key:解锁道具ID second:伙伴品质

	std::map<int, PartnerUpGradeRoleAttrCfg> m_upgrade_role_attr_map;	

	std::map<int, PartnerComposePageCfg> m_partner_compose_page_cfg;			//伙伴残页合成 key:道具ID
	std::map<int, PartnerComposeGroupCfg> m_partner_compose_cfg;				//伙伴合成组配置 key :reward_group_id

	//--契约仓库IDmap <物品id, 储存位置>
	PactItemIDMap m_pact_item_id_map;
	PactSaveIndexMap m_pact_save_index_map;

	std::map<int, PartnerSoarCfg> m_partner_soar_cfg;				// 飞升配置 key:soar_frequency
	std::map<int, PartnerSoarArmsCfg> m_partner_soar_arms_cfg;		// 飞升武器配置 key:arms_id * MAX_PARTNER_SOAR_ARMS_BASE_NUM + arms_level

	//伙伴强化
	std::map<int, std::vector<PartnerIntensifyCfg> > m_partner_intensify_cfg;
};

#endif // __PARTNER_CONFIG_HPP__