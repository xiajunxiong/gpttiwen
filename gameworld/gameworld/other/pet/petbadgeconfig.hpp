#ifndef __PET_BADGE_CONFIG_HPP__
#define __PET_BADGE_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"
#include "gamedef.h"
#include "servercommon/configcommon.h"
#include <map>
#include <vector>
#include <set>
#include "gameworld/item/itembase.h"

struct PetBadgeCfg
{
	int item_id;
	int compose_succ_item_id;
	int compose_succ_rate;
	int decompose_get_item_id;
	int decompose_get_item_num;
	int compose_need_item_id;
	int compose_need_item_num;
	std::vector<int> attr_group_id;
	int has_skill_rate;
	int rand_skill_group_id;
	bool is_broadcast;
};

struct PetBadgeAttrCfg
{
	int attr_group_id;
	int attr_type;
	int attr_rand_value_min;
	int attr_rand_value_max;
};

struct PetBadgeSkillCfg
{
	int skill_group_id;
	int skill_id;
};

class PetBadgeConfig : public ILogicConfig
{
public:
	PetBadgeConfig();
	virtual ~PetBadgeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetBadgeCfg * GetPetBadgeCfg(int item_id);
	std::vector<AttrInfo > GePetBadgeRandAttr(int item_id); 
	int GePetBadgeRandSkill(int item_id);
	int GePetBadgeSkillCapability(int skill_id);
private:
	std::vector<PetBadgeAttrCfg> GePetBadgeAttrCfg(int attr_group_id, const std::set<int> & exclude_attr_type_list);
private:
	int InitPetBadgeCfg(TiXmlElement *RootElement);
	int InitPetBadgeAttrCfg(TiXmlElement * RootElement);
	int InitPetBadgeSkillCfg(TiXmlElement * RootElement);


	std::map<int, PetBadgeCfg> m_badge_cfg;
	std::map<int, std::vector<PetBadgeAttrCfg> > m_badge_attr_cfg;
	std::map<int, std::vector<PetBadgeSkillCfg> > m_bagdge_skill_cfg;
	std::map<int, int > m_skill_capability_cfg;
};

#endif // __PETCONFIG_HPP__