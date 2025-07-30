#ifndef __DIAN_HUA_SHI_CONFIG_HPP__
#define __DIAN_HUA_SHI_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "item/dianhuashiitem/dianhuashiitem.hpp"

struct DianHuaShiOtherCfg
{
	DianHuaShiOtherCfg() {}
	int open_condition;
	int	open_level;
	int	fumo_id;
	int	xifu_id;
	int	dianhua_id;
};

struct DianHuaSpecialEffectDetailCfg
{
	DianHuaSpecialEffectDetailCfg() :type(0), effect_level(0), bonus_type(0), bonus_attributes(0), equipmen_num(0), set_skills(0), xifu_consume(0) {}
	int type;
	int	effect_level;
	int	bonus_type;
	int	bonus_attributes;
	int	equipmen_num;
	int	set_skills;
	int xifu_consume;
	std::vector<ItemConfigData> resolve_vec;
};

struct DianHuaEffectsProbabilityCfg
{
	DianHuaEffectsProbabilityCfg() : seq(0), dianhua_probability(0) {}
	int seq;
	int dianhua_probability;
};

struct DianHuaLevelProbabilityCfg
{
	DianHuaLevelProbabilityCfg() : effect_level(0), new_effect_level(0), effect_probability(0) {}
	int effect_level;
	int new_effect_level;
	int effect_probability;
};

struct DianHuaLevelProbabilityListCfg
{
	DianHuaLevelProbabilityListCfg() : effect_level(0), effect_pro_level_weight(0) {}
	int effect_level;
	int effect_pro_level_weight;
	std::vector<DianHuaLevelProbabilityCfg> level_vec;
};

struct DianHuaSkillLevelCfg
{
	DianHuaSkillLevelCfg() :all_effect_level(0), skill_level(0) {}
	int all_effect_level;
	int skill_level;
};

// -----------------------------------------------------------------------------------
class DianHuaShiConfig : public ILogicConfig
{
public:
	DianHuaShiConfig();
	virtual ~DianHuaShiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const DianHuaShiOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const DianHuaSpecialEffectDetailCfg * GetEffectDetailCfg(int effect_id, int level);
	const DianHuaEffectsProbabilityCfg * RandEffectProCfg();
	const DianHuaLevelProbabilityCfg * RandEffectLevelProCfg(int effect_level);
	const DianHuaSkillLevelCfg * GetSkillLevelCfg(int all_skill_level);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitEffectDetailCfg(TiXmlElement * RootElement);
	int InitEffectProCfg(TiXmlElement * RootElement);
	int InitEffectLevelProCfg(TiXmlElement * RootElement);
	int InitEffectSkillLevelCfg(TiXmlElement * RootElement);

	DianHuaShiOtherCfg m_other_cfg;
	std::vector<DianHuaSpecialEffectDetailCfg> m_skill_vec;
	int effect_pro_weight;
	std::vector<DianHuaEffectsProbabilityCfg> m_effect_pro_vec;
	
	std::map<int, DianHuaLevelProbabilityListCfg> m_level_pro_vec;

	std::vector<DianHuaSkillLevelCfg> m_all_skill_level_vec;
};

#endif