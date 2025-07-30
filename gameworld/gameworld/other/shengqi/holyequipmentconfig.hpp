#ifndef __HOLY_EQUIPMENT_CONFIG_HPP__
#define __HOLY_EQUIPMENT_CONFIG_HPP__

#include <map>
#include <vector>
#include <string>

#include "item/itempool.h"
#include "gamelog.h"
#include "item/shengqi/runestoneitem.h"
#include "config/otherconfig/scoresystemconfig.hpp"

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/medaldef.h"
#include "item/medalitem/medalitem.hpp"
#include "obj/character/role.h"

static const int MAX_ROLE_ATTR_NUM = 12;	// 配置可获得宠物属性最大类型
static const int MAX_HOLY_ATTR_NUM = 7;		// 配置可获得圣器属性最大类型

struct HolyAttrItem
{
	HolyAttrItem() :attr_type(0), attr_min(0), attr_max(0)
	{}

	int attr_type;
	int attr_min;
	int attr_max;
};

struct HolyEquipBaseCfg
{
	HolyEquipBaseCfg():shengqi_id(0), item_id(0),shengqi_type(0),shengqi_quality(0),fenjie_exp(0)
	{}

	int shengqi_id;			// 虚拟id
	ItemID item_id;			// 物品id
	int shengqi_type;		// 类型
	int shengqi_quality;	// 品质
	int fenjie_exp;			// 分解经验
	std::vector<int> attr_type_list;		// 属性类型列表
	HolyAttrItem attrs[MAX_HOLY_ATTR_NUM];	// 基础属性随机值组
};

struct HolyEquipTalentCfg
{
	HolyEquipTalentCfg():skill_id(0), skill_class(0), skill_type(0),appear_type(0)
	{}
	static const int MAX_HOLY_EQUIP_TALENT_COUNT = 2;

	enum SKILL_TYPE
	{
		SKILL_TYPE_COMMON = 1,		// 普通天赋
		SKILL_TYPE_STRONGER = 2,	// 高级天赋
		SKILL_TYPE_SUPER = 3,		// 超级天赋
	};

	int skill_id;		// 技能id
	int skill_class;	// 技能组,一个圣器不可以有两个相同技能组的技能
	int skill_type;		// 技能类型,天赋等级,SKILL_TYPE
	int appear_type;	// 出现在的圣器类型,HOLY_EQUIP_TYPE
};

struct HolyEquipInitiativeSkillCfg
{
	HolyEquipInitiativeSkillCfg():combo_level(0),skill_type(0)
	{}

	int combo_level;	// 合技等级,(本质就是不同的技能id)
	int skill_type;		// 技能类型
	std::pair<int, int> skill_pair;		// value1--普通技能id, value2--擅长技能id
	std::set<int> shanchang_shengqi;	// 擅长圣器id
};

struct HolyEquipInitiativeSkillHoleCfg	// 主动技能槽开启配置
{
	HolyEquipInitiativeSkillHoleCfg():unlock_cost(0),num(0)
	{}

	int unlock_cost;	// 开启需要道具id
	int num;			// 开启需要道具数量
	int shengqi_quality;	// 需要圣器品质
};

struct HolyEquipLevelUpAttrCfg
{
	HolyEquipLevelUpAttrCfg():upgrade_exp(0),coin_cost(0), active_max(0)
	{}
	
	int upgrade_exp;	// 所需经验
	int coin_cost;		// 消耗铜币
	int active_max;		// 活跃度上限
	AttrInfo attr_list[BATTLE_ATTR_MAX];	//属性列表 ,根据圣器类型配置获取属性类型, 圣器等级配置决定获得的属性
};

struct HolyEquipLevelUpCfg
{
	HolyEquipLevelUpCfg():shengqi_quality(0)
	{}

	int shengqi_quality;	// 圣器品质
	std::map<int, HolyEquipLevelUpAttrCfg> level_cfg;	// 等级配置,key-圣器等级
};

struct RuneStoneBaseCfg
{
	RuneStoneBaseCfg():item_id(0), fushi_level(0)
	{}

	ItemID item_id;
	int fushi_level;
	HolyAttrItem attrs[MAX_HOLY_ATTR_NUM];
	std::map<int, AttrInfo> role_attr_list;	 // 根据职业获取不同的属性, key-基础职业段数PROF_TYPE
};

struct RuneStoneCfg
{
	RuneStoneCfg():fushi_type(0)
	{}

	int fushi_type;		// 符石类型
	std::vector<RuneStoneBaseCfg> fushi_attr;	// 符石等级属性
};

struct RuneStoneHoleCfg
{
	RuneStoneHoleCfg():shengqi_level(0),unlock_cost(0),consume_num(0)
	{}

	int shengqi_level;		// 需求圣器等级
	ItemID unlock_cost;		// 解锁消耗
	int consume_num;		// 消耗数量
};

struct RuneStoneFusingCfg
{
	RuneStoneFusingCfg():need_item_num(0),cost_coin(0)
	{}

	int need_item_num;	// 符石合成需要消耗的符石数量
	int cost_coin;		// 铜币消耗
};

struct RuneStoneRecastCfg
{
	RuneStoneRecastCfg() :need_item_num(0), cost_coin(0)
	{}

	int need_item_num;	// 符石重铸需要消耗的符石数量
	int cost_coin;		// 铜币消耗
};

struct HolyEquipOtherCfg
{
	HolyEquipOtherCfg():feisheng_type(0), exp_back(0)
	{}
	static const int HOLY_EQUIP_EXP_BACK_PROPORTION = 1000;

	int feisheng_type;		// 角色需要飞升阶段,达到该阶段才能开启系统
	int exp_back;			// 以圣器作为材料时的经验返还比例, 千分比
	ItemConfigData baoxiang1;	// 高级圣器宝箱
	ItemConfigData baoxiang2;	// 超级圣器宝箱
};


// -------------------------------------------------------------------------------------
class HolyEquipMentConfig :public ILogicConfig
{
public:
	HolyEquipMentConfig();
	virtual ~HolyEquipMentConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HolyEquipBaseCfg* GetBaseCfgById(ItemID item_id)const;
	const HolyEquipLevelUpAttrCfg* GetHolyLevelCfg(int shengqi_quality, int level)const;
	int GetLevelBySkillId(int skill_id)const;
	int GetShengQiIdByItemId(ItemID item_id)const;
	int GetSkillIdByShanChangId(int shanchang_id)const;		// return 0,查不到

	bool RandHolyItemAttr(HolyEquipParam & data, ItemID item_id, int shengqi_quality, int shengqi_type, std::string& error);
	int RandHolyItemTaLent(HolyEquipParam& data,int shengqi_quality, int shengqi_type);
	int RandHolyItemSkill(const HolyEquipParam& data, ItemID item_id, int shengqi_type, int skill_quality, int skill_hole);
	bool RandRuneStoneAttr(Role* role, RuneStoneParam& data, int runestone_type, ItemID item_id);
	const HolyEquipOtherCfg& GetOtherCfg()const { return m_other_cfg; }

	const HolyEquipInitiativeSkillHoleCfg* GetSkillHoleCfg(int hole)const;
	const RuneStoneHoleCfg* GetFuShiHoleCfg(int fushi_hole)const;
	const RuneStoneFusingCfg* GetFuShiFusingCfg(int fushi_level);
	const RuneStoneRecastCfg* GetFuShiRecastCfg(int fushi_level);
	const ItemID* GetFuShiIdByTypeLevelCfg(int fushi_type, int fushi_level);
	int GetItemExpCfg(ItemID item_id)const;		// 获取圣器精华可获得的exp
	int GetHolyEquipExp(ItemID item_id)const;	// 获取圣器本身的分解经验
	int GetSkillScoreCfg(int skill_id)const;
	int GetSkillIdByTypeAndItem(int shengqi_type, int lingwu_item)const;
	bool GetSkillLevel(int skill_id, ARG_OUT int* out_skill_id, ARG_OUT int* out_skill_lv) const;

	const AttrInfo* GetAttrBySkill(int skill_id)const;	// 根据主动技能id获取附带的属性
	const AttrInfo* GetRoleAttr(Role* role, ItemID fushi_id, int fushi_type)const;		// 根据符石id,符石类型和职业获取附带属性
private:
	int InitBaseCfg(TiXmlElement * RootElement, std::string& errormsg);				// 圣器类型分页
	int InitTaLentCfg(TiXmlElement * RootElement, std::string& errormsg);			// 天赋技能分页
	int InitSkillCfg(TiXmlElement * RootElement, std::string& errormsg);			// 主动技能分页
	int InitSkillStudyCfg(TiXmlElement * RootElement, std::string& errormsg);		// 技能领悟分页
	int InitSkillHoleCfg(TiXmlElement * RootElement, std::string& errormsg);		// 技能孔开启分页
	int InitHolyLevelUpCfg(TiXmlElement * RootElement, std::string& errormsg);		// 圣器升级分页
	int InitLevelUpItemCfg(TiXmlElement * RootElement, std::string& errormsg);		// 圣器经验丹分页
	int InitRuneStoneCfg(TiXmlElement * RootElement, std::string& errormsg);		// 圣器符石分页
	int InitRuneStoneHoleCfg(TiXmlElement * RootElement, std::string& errormsg);	// 符石孔分页
	int InitRuneStoneFusingCfg(TiXmlElement * RootElement, std::string& errormsg);	// 符石合成分页
	int InitRuneStoneRecastCfg(TiXmlElement * RootElement, std::string& errormsg);	// 符石重铸分页
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);			// 其他分页


	std::vector<HolyEquipBaseCfg> m_shengqi_base_cfg;		//!< 圣器基础属性

	std::vector<HolyEquipTalentCfg> m_talent_cfg;			//!< 天赋技能

	std::vector<HolyEquipInitiativeSkillCfg> m_initiative_skill_cfg;	//!< 主动技能
	std::map<int, std::map<ItemID, int> > m_skill_group_cfg;	//!< 主动技能领悟 key1-圣器类型,key2-消耗道具id
//	std::map<int, ItemID> m_skill_material_cfg;				//!< 圣器要诀领悟组,key-领悟类型,value-对应要消耗的道具id
	std::map<int, HolyEquipInitiativeSkillHoleCfg> m_skill_hole_cfg;	//!< 主动技能孔位开启,key-孔位,0-4
	std::map<int, int> m_skill_find_level_cfg;				//!< 根据技能id查找技能等级,key-主动技能id, value-技能等级
	std::map<int, int> m_skill_shanchang_find_cfg;			//!< 根据擅长技能id查找普通技能id,key-擅长技能id,value-[普通技能id	

	std::vector<HolyEquipLevelUpCfg> m_level_up_cfg;		//!< 圣器等级配置
	std::map<ItemID, int> m_upgrade_consume_cfg;			//!< 圣器精华可以提供的经验,key-物品id,value-提供的经验

	std::vector<RuneStoneCfg> m_rune_stone_cfg;				//!< 符石配置
	std::map<int, RuneStoneHoleCfg> m_runestone_hole_cfg;	//!< 符石孔位开启配置,key-孔位

	std::map<int, RuneStoneFusingCfg> m_fushi_fusing_cfg;	//!< 符石合成配置, key-符石等级
	std::map<int, RuneStoneRecastCfg> m_fushi_recast_cfg;	//!< 符石重铸配置, key-符石等级

	std::map<int, int> m_skill_score_map_cfg;				//!< 技能id评分对照表, key-技能id/天赋id,value-评分
	std::map<int, AttrInfo> m_skill_attr_map_cfg;			//!< 技能id属性对照表, key-技能id/天赋id,value-增加的圣器属性

	HolyEquipOtherCfg m_other_cfg;
};

#endif