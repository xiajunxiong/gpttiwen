#include "item/itempool.h"
#include "petbadgeconfig.hpp"

PetBadgeConfig::PetBadgeConfig()
{
}

PetBadgeConfig::~PetBadgeConfig()
{
	for (std::map<int, std::vector<PetBadgeAttrCfg> >::iterator it = m_badge_attr_cfg.begin(); it != m_badge_attr_cfg.end(); it++)
	{
		std::vector<PetBadgeAttrCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<PetBadgeSkillCfg> >::iterator it = m_bagdge_skill_cfg.begin(); it != m_bagdge_skill_cfg.end(); it++)
	{
		std::vector<PetBadgeSkillCfg>().swap(it->second);
	}
}

bool PetBadgeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("pet_badges", InitPetBadgeCfg);
	LOAD_CONFIG("att_group", InitPetBadgeAttrCfg);
	LOAD_CONFIG("skill_group", InitPetBadgeSkillCfg);

	return true;
}

const PetBadgeCfg * PetBadgeConfig::GetPetBadgeCfg(int item_id)
{
	return MapValuesConstPtr(m_badge_cfg, item_id);
}

std::vector<AttrInfo> PetBadgeConfig::GePetBadgeRandAttr(int item_id)
{
	std::vector<AttrInfo> ret;
	const PetBadgeCfg * cfg = this->GetPetBadgeCfg(item_id);
	if (!cfg)
	{
		return ret;
	}

	std::set<int>  exclude_attr_type_list;
	for (int i = 0; i < cfg->attr_group_id.size(); ++i)
	{
		std::vector<PetBadgeAttrCfg> attr_cfgs = this->GePetBadgeAttrCfg(cfg->attr_group_id[i], exclude_attr_type_list);
		if (attr_cfgs.empty())
		{
			continue;
		}
		
		std::random_shuffle(attr_cfgs.begin(), attr_cfgs.end());
		const PetBadgeAttrCfg & attr_cfg = attr_cfgs[0];
		int r = RandomNum(attr_cfg.attr_rand_value_max - attr_cfg.attr_rand_value_min + 1) + attr_cfg.attr_rand_value_min;
		AttrInfo tmp;
		tmp.attr_type = attr_cfg.attr_type;
		tmp.attr_value = r;
		ret.push_back(tmp);		

		exclude_attr_type_list.insert(attr_cfg.attr_type);
	}
	
	return ret;
}

int PetBadgeConfig::GePetBadgeRandSkill(int item_id)
{
	int skill_id = 0;
	const PetBadgeCfg * cfg = this->GetPetBadgeCfg(item_id);
	if (!cfg)
	{
		return skill_id;
	}

	int r = RandomNum(10000);
	if (r < cfg->has_skill_rate)
	{
		const std::vector<PetBadgeSkillCfg> * skill_cfgs = MapValuesConstPtr(m_bagdge_skill_cfg, cfg->rand_skill_group_id);
		if (skill_cfgs && !skill_cfgs->empty())
		{
			int tmp = RandomNum((int)skill_cfgs->size());
			skill_id = (*skill_cfgs)[tmp].skill_id;
		}		
	}

	return skill_id;
}

int PetBadgeConfig::GePetBadgeSkillCapability(int skill_id)
{
	return MapValue(m_skill_capability_cfg,skill_id);
}

std::vector<PetBadgeAttrCfg> PetBadgeConfig::GePetBadgeAttrCfg(int attr_group_id, const std::set<int> & exclude_attr_type_list)
{
	std::vector<PetBadgeAttrCfg> ret;

	const std::vector<PetBadgeAttrCfg> * cfg_ptr = MapValuesConstPtr(m_badge_attr_cfg, attr_group_id);
	if (!cfg_ptr)
	{
		return ret;
	}

	const std::vector<PetBadgeAttrCfg> & cfg = *cfg_ptr;
	for (int i = 0; i < (int)cfg.size(); ++i)
	{
		if (exclude_attr_type_list.find(cfg[i].attr_type) != exclude_attr_type_list.end())
		{
			continue;
		}
		
		ret.push_back(cfg[i]);
	}

	return  ret;
}

int PetBadgeConfig::InitPetBadgeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetBadgeCfg cfg;
		if (!GetSubNodeValue(root_element, "item_id", cfg.item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "next_item_id", cfg.compose_succ_item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "next_level_rate", cfg.compose_succ_rate))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "dec_item_id", cfg.decompose_get_item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "dec_num", cfg.decompose_get_item_num))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "synthesis_item", cfg.compose_need_item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "synthesis_num", cfg.compose_need_item_num))	return -__LINE__;

		for (int i = 0; i < 2; ++i)
		{
			std::string s = STRING_SPRINTF("att_%d", i + 1);
			int t = 0;
			if (!GetSubNodeValue(root_element, s, t))	return -__LINE__;
			cfg.attr_group_id.push_back(t);
		}

		if (!GetSubNodeValue(root_element, "rate", cfg.has_skill_rate))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "skill_group", cfg.rand_skill_group_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "is_broadcast", cfg.is_broadcast))	return -__LINE__;

		m_badge_cfg[cfg.item_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetBadgeConfig::InitPetBadgeAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetBadgeAttrCfg cfg;
		if (!GetSubNodeValue(root_element, "group_id", cfg.attr_group_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "att_type", cfg.attr_type))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "att_num_min", cfg.attr_rand_value_min))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "att_num_max", cfg.attr_rand_value_max))	return -__LINE__;
		if (cfg.attr_rand_value_max < cfg.attr_rand_value_min)return -__LINE__;
		

		m_badge_attr_cfg[cfg.attr_group_id].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetBadgeConfig::InitPetBadgeSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetBadgeSkillCfg cfg;
		if (!GetSubNodeValue(root_element, "group_id", cfg.skill_group_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "skill_id", cfg.skill_id))	return -__LINE__;

		int skill_capability = 0;
		if (!GetSubNodeValue(root_element, "skill_score", skill_capability)) return -__LINE__;

		m_skill_capability_cfg[cfg.skill_id] = skill_capability;
		m_bagdge_skill_cfg[cfg.skill_group_id] .push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
