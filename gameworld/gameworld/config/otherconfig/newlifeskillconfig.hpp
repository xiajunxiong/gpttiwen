#ifndef __NEW_LIFE_SKILL_CONFIG_HPP__
#define __NEW_LIFE_SKILL_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <set>

struct NewLifeSkillOtherCfg
{
	NewLifeSkillOtherCfg() : occupation_double_times(0), food_time(0), shift_card_time(0)
	{}

	int occupation_double_times;			//药师和厨师每日生成双倍次数
	int food_time;							//美食属性时间
	int shift_card_time;					//变身卡持续时间
};

struct NewLifeSkillLevelCfg
{
	static const int MAX_DOUBLE_RATE_COUNT_NUM = 1000;
	NewLifeSkillLevelCfg() : up_expend(0), double_rate(0), prof_double_rate(0), up_expend_medicine(0), up_expend_cook(0)
	{}

	int up_expend;				//技能升级消耗
	int double_rate;			//采集双倍概率(千分比)
	int prof_double_rate;		//职业制作双倍概率(千分比)
	int up_expend_medicine;		//药师职业升级消耗
	int up_expend_cook;			//厨师职业升级消耗
};

struct NewLifeSkillPropsCfg
{
	static const int MAX_ATTR_NUM = 3;
	NewLifeSkillPropsCfg() : type(0), expend_point(0), increase_point(0), skill_id(0), use_skill_need_point(0), attr_count(0)
	{
		for (int i = 0; i < ARRAY_LENGTH(attr_list); i++)
		{
			attr_list[i].attr_type = 0;
			attr_list[i].attr_value = 0;
		}
	}

	int type;							//NEW_LIFE_SKILL_PROPS_TYPE_BEGIN_MAX
	int expend_point;					//单次战斗消耗幻化点
	int increase_point;					//单卡增加幻化点
	int skill_id;						//技能ID
	int use_skill_need_point;			//使用技能消耗幻化点
	int attr_count;						
	AttrInfo attr_list[MAX_ATTR_NUM];	//属性列表
};

struct NewLifeSkillRecipeCfg
{
	static const int MAX_NEED_ITEM_COUNT = 2;
	NewLifeSkillRecipeCfg() : new_life_skill_type_limit(0), new_life_skill_level_limit(0), need_item_count(0), 
		expend_active(0), compose_item_id(0)
	{}

	int new_life_skill_type_limit;
	int new_life_skill_level_limit;
	std::set<int> role_prof_set;
	int need_item_count;
	ItemConfigData need_item_list[MAX_NEED_ITEM_COUNT];
	int expend_active;
	int compose_item_id;			//合成道具
};

struct NewLifeSkillRecoveryCfg
{
	NewLifeSkillRecoveryCfg() : mail_virtual_type(0), price(0)
	{}

	int mail_virtual_type;
	int price;			//一点幻化点返回该数值 X(幻化点) *price
};

class NewLifeSkillConfig : public ILogicConfig
{
public:
	NewLifeSkillConfig();
	virtual ~NewLifeSkillConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const NewLifeSkillOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const NewLifeSkillLevelCfg * GetNewLifeSkillLevelCfg(int skill_level);
	const NewLifeSkillRecipeCfg * GetRecipeCfg(int sn);
	const NewLifeSkillPropsCfg * GetPropsCfg(int seq);

	bool IsNewLifeProf(int new_life_skill_type);
	const std::vector<int>& GetSystemFoodList() const { return m_system_food_list; }
	
	const NewLifeSkillRecoveryCfg * GetRecoveryCfg(int change_card_seq) const;

private:
	
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitRecipeCfg(TiXmlElement *RootElement);
	int InitPropsCfg(TiXmlElement *RootElement);
	int InitSystemFoodCfg(TiXmlElement* RootElement);
	int InitRecoveryCfg(TiXmlElement* RootElement);

	NewLifeSkillOtherCfg m_other_cfg;
	std::map<int, NewLifeSkillLevelCfg> m_new_life_skill_level_cfg;		//生活技能等级配置 key:skill_level
	std::map<int, NewLifeSkillRecipeCfg> m_recipe_cfg;					//配方配置 key:sn
	std::map<int, NewLifeSkillPropsCfg> m_props_cfg;					//道具配置 key:seq
	std::vector<int> m_system_food_list;								//竞技用食物列表
	std::map<int, NewLifeSkillRecoveryCfg> m_recovery_map;				//变身卡返还(幻化点返还)
};


#endif