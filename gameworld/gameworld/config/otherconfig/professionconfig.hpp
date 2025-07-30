#ifndef __PROFESSION_CONFIG_HPP__
#define __PROFESSION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/advancecareerparam.hpp"

#include <map>
#include <set>
#include <vector>

const static int PROFESSION_SKILL_INIT_MAX_NUM = 4;
const static int CHANGE_PRO_ITEM_MAX_NUM = 5;

struct ProfSkillCfg
{
	ProfSkillCfg() : prof_base_type(0), proficient_type(0), skill_id(0), skill_level_limit(0), skill_learn_level(0) {}

	short prof_base_type;
	short proficient_type; // [0:得意技  1:通用技  2:生活技能]
	UInt16 skill_id;
	short skill_level_limit;
	short skill_learn_level;
};

enum ADVANCE_COND_TYPE			// 晋级条件类型
{
	ADVANCE_COND_TYPE_NO = 0,						// 无条件
	ADVANCE_COND_TYPE_MISSION = 1,					// 完成任务				p1 : 任务ID
	ADVANCE_COND_TYPE_XIN_LING_ZHI_JING = 2,		// 通关心灵之镜第X关		p1 : 心魔关卡seq(>=seq)
	ADVANCE_COND_TYPE_MENG_YUAN = 3,				// 通关梦渊第X回			p1 : 副本seq(>=seq)
	ADVANCE_COND_TYPE_GU_MO_ZHI_TA = 4,				// 通关锢魔之塔第X层		p1 : 序列seq(>=seq)
	ADVANCE_COND_TYPE_LUN_HUI_WANG_CHUAN = 5,		// 通关轮回忘川第X层		p1 : 关卡id(>=id)
	ADVANCE_COND_TYPE_SHAN_HAI_BOSS = 6,			// 击败XX难度山海精怪		p1 : 难度类型(该难度任意怪物)
	ADVANCE_COND_TYPE_YUAN_ZHENG_BOSS = 7,			// 击败远征首领：XXXX		p1 : 序列seq p2:boss_index (需要击败指定的怪物) 
	ADVANCE_COND_TYPE_DU_JIE = 8,					// 通关X重劫				p1 : seq(>=seq)
	ADVANCE_COND_TYPE_WU_JIN_ZHI_TA = 9,			// 通关无尽之塔XX层		P1 : seq(>=seq)
	ADVANCE_COND_TYPE_SHANG_HAI_BOSS_SUCC = 10,		// 战胜山海BOSS次数		P1 : 次数(>=cond_param1)
	ADVANCE_COND_TYPE_BIG_DIPPER_SUCC = 11,			// 战胜任意北斗星君次数	P1 : 次数(>=cond_param1)
	ADVANCE_COND_TYPE_NETHER_WORLD_PURGATORY = 12,	// 通关幽冥炼狱第X章Y关	P1 : X章节	P2:关卡ID (P1> COND_P1 OR P1== COND_P1 AND P2>= COND_P2) (该条件不需要完全是该关卡,超过也算)
	ADVANCE_COND_TYPE_YAO_XING_SHI_LIAN = 13,		// 通关妖行试炼第X关		P1 : 关卡seq(>=seq)
	ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC = 14,	// 战胜怪物组			P1 : 怪物组ID(==)
	ADVANCE_COND_TYPE_SHI_SHEN_ZHI_TA = 15,			// 通过弑神之塔第X层		P1 : 序列seq(>=seq)
	ADVANCE_COND_TYPE_MIRAGE_TERRITORY = 16,		// 琉璃蜃境所有区均达X关	P1 : 关卡seq(>=num)
	ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE = 17,		// 通关鸿蒙天界			P1 : 副本ID(==副本ID)

	ADVANCE_COND_TYPE_MAX,
};

struct CareerAdvanceCfg
{
	CareerAdvanceCfg() : profession_id(0), pre_profession(0), order(0), job_seq(0), is_broadcast(0), prof_base(0), need_level(0), pre_task(0), pre_title(0), pre_village(0), pre_relation(0), monster_group(0),
		pre_score(0), need_bind_coin(0), image(0), reward_title(0), max_equip_level(0), max_skill_level(0), all_sp_bonus(0), change_color_coin(0), change_item_id(0), change_item_num(0),
		juanzeng_up_limit(0), shengwangka_up_limit(0)
	{ 
		memset(init_skill_id, 0, sizeof(init_skill_id));
		memset(m_attrs, 0, sizeof(m_attrs));
	}

	int profession_id;				// 职业ID
	int pre_profession;				// 前置职业ID
	int order;						// 阶数
	int job_seq;					// 职业小阶
	int is_broadcast;				// 进阶时是否传闻 0:不传闻
	int prof_base;					// 职业类型
	int need_level;					// 需求等级
	int pre_task;					// 前置任务ID
	int pre_title;					// 前置称号ID
	int pre_village;				// 前置村子ID
	int pre_relation;				// 前置村子关系		[没有使用了]
	int monster_group;				// 关卡怪物组		[没有使用了]
	int cond_type;					// 完成条件			[对应枚举ADVANCE_COND_TYPE]
	int cond_param1;				// 完成条件参数1
	int cond_param2;				// 完成条件参数2
	int pre_score;					// 前置角色评分
	int need_bind_coin;				// 激活铜币消耗
	int image;						// 可选形象
	int reward_title;				// 奖励称号
	int max_equip_level;			// 装备等级上限
	int max_skill_level;			// 技能等级上限

	int init_skill_num;				// 初始技能数量
	int init_skill_id[PROFESSION_SKILL_INIT_MAX_NUM];						//初始技能的列表
	std::set<int> unlock_skill_set;

	JobName job_name;														//该职业阶级称呼
	Attribute m_attrs[BATTLE_ATTR_MAX];										//职业属性
	SuitName  suit_name;													//套装外观名称

	int all_sp_bonus;								// 全体特效加成  特效ID

	int change_color_coin;							// 换色铜币消耗
	int change_item_id;								// 换色材料消耗类型
	int change_item_num;							// 换色材料消耗数量

	int juanzeng_up_limit;							// 装备捐赠上限
	int shengwangka_up_limit;						// 声望卡使用上限
};

struct ProfOtherCfg
{
	ProfOtherCfg() : change_career_cost_min(0), change_career_cost_max(0), change_cost_ratio_per_time(0), ver(0){}

	int change_career_cost_min;
	int change_career_cost_max;
	int change_cost_ratio_per_time;			// 每次耗费增长的倍率
	int ver;								// 职业进阶任务版本号
};

struct CareerAdvanceGiveItemCfg
{
	CareerAdvanceGiveItemCfg()
	{
		role_level_min = 0;
		role_level_max = 0;
	}

	int role_level_min;
	int role_level_max;
	std::vector<ItemConfigData> change_prof_item;				//转职赠送一套装备
};

struct SkillChangeCfg
{
	SkillChangeCfg() : max_level(0)
	{}

	struct SkillChangeLevelCfg
	{
		SkillChangeLevelCfg() : coin_consume_all(0), bind_coin_consume(0), add_exp(0)
		{}
		int coin_consume_all;				//升级所需总铜币	已经叠加了之前的等级所需
		int bind_coin_consume;				//单次升级消耗金币
		int add_exp;						//单次点击升级经验
	};

	int max_level;
	SkillChangeLevelCfg skill_list[MAX_BATTLE_SKILL_LEVEL + 1];
};

struct ProfSkillLearnLevelCfg
{
	int skill_id;
	int learn_level;
};

class Role;
class ProfessionConfig : public ILogicConfig
{
public:
	static const int PROF_HASH_BASE = 100000;
	typedef std::map<int, ProfSkillCfg> ProfSkillCfgMap; //  key = base_prof * PROF_HASH_BASE + skill_id
	typedef std::map<int, std::vector <CareerAdvanceCfg> > CareerAdvanceCfgMap;
	typedef std::map<int, std::vector<ProfSkillLearnLevelCfg> > ProfSkillListMap;

public:
	ProfessionConfig();
	virtual ~ProfessionConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	bool CanLearnSkill(Role* role, UInt16 skill_id);
	int GetMaxLearnSkillLevel(Role* role, UInt16 skill);
	int GetSkillLeanLevel(Role* role, UInt16 skill);

	static int GetHashKey(int prof_base_type, UInt16 skill_id);
	void GetAllTitleByBaseProf(int prof_base, std::vector<int> & out_list);
	std::vector<int> GetUnlockSkillList(int prof_base, int role_level);
	std::vector<int> GetProfSkillList(int prof_base);
	std::set<int> GetProfSkillSet(int prof_base);

	const CareerAdvanceCfg* GetCareerAdvanceCfg(int profession , int _order);
	const std::vector<CareerAdvanceCfg> * GetCareerAdvanceCfgByCondType(int _cond_type);

	int GetCareerAdvanceStageSize(int _profession);
	const CareerAdvanceGiveItemCfg * GetCareerAdvanceGiveItemCfg(int base_prof, int role_level);
	const ProfOtherCfg& GetOtherCfg();
	const ProfSkillCfg* GetProfSkillCfgByHashKey(int hash_key);

	bool IsNeedReturn(int skill_id);
	int GetSkillChangeReturn(int skill_id, int skill_level, int skill_exp);

private:
	int InitProfSkillCfg(TiXmlElement *RootElement);
	int InitCareerAdvanceCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitChangeProfGiveCfg(TiXmlElement * RootElement);
	int InitSkillChangeCfg(TiXmlElement * RootElement);

	ProfSkillCfgMap m_prof_skill_map;
	CareerAdvanceCfgMap m_career_advance_cfg_map;
	CareerAdvanceCfgMap m_advance_cond_cfg_map;		// <cond_type , std::vector<cfg > >

	ProfOtherCfg m_other_cfg;
	std::map<int, std::vector<int> > m_prof_title_vec;		// 职业进阶称号表：键 - 职业基值ID；值 - 称号ID；

	std::map<int, std::vector<CareerAdvanceGiveItemCfg> > m_change_prof_give_item_cfg;
	std::map<int, SkillChangeCfg> m_skill_change_map;		// 技能修改配置 key:技能ID

	ProfSkillListMap m_prof_skilllist_map;
};

#endif