#include "starchartconfig.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

StarChartConfig::StarChartConfig()
{
}

StarChartConfig::~StarChartConfig()
{
	this->Destroy();
}

bool StarChartConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("role_star", InitRoleStarChartCfg);
	LOAD_CONFIG_2("pet_star", InitPetStarChartCfg);
	LOAD_CONFIG_2("partner_star", InitPartnerStarChartCfg);
	LOAD_CONFIG_2("role_star_condition", InitRoleStarChartConnectCfg);
	LOAD_CONFIG_2("partner_star_condition", InitPartnerStarChartConnectCfg);
	LOAD_CONFIG_2("pet_star_condition", InitPetStarChartConnectCfg);
	LOAD_CONFIG_2("hole_open", InitStarChartSkillCfg);
	LOAD_CONFIG_2("superstar", InitSuperGalaxiesCfg);
	LOAD_CONFIG_2("superstar_condition", InitSuperGalaxiesConnectCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

bool StarChartConfig::ValidStarChartPoint(int star_chart_type, int star_chart_stage, int star_id) const
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return false;

	StarChartTypeMap::const_iterator it = m_star_chart_cfg[star_chart_type].find(star_chart_stage);
	if (it != m_star_chart_cfg[star_chart_type].end())
	{
		std::map<int, std::vector<BaseStarChartCfg*> >::const_iterator node_it = it->second.find(star_id);
		if (node_it != it->second.end())
		{
			return true;
		}
	}

	return false;
}

bool StarChartConfig::ValidSuperGalaxiesPoint(int star_id) const
{
	SuperGalaxiesMap::const_iterator it = m_super_galaxies_cfg.find(star_id);
	if (it != m_super_galaxies_cfg.end()) return true;

	return false;
}

const BaseStarChartCfg * StarChartConfig::GetStarChartCfg(int star_chart_type, int star_chart_stage, int star_id, int star_level) const
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return NULL;

	StarChartTypeMap::const_iterator it = m_star_chart_cfg[star_chart_type].find(star_chart_stage);
	if (it != m_star_chart_cfg[star_chart_type].end())
	{
		std::map<int, std::vector<BaseStarChartCfg*> >::const_iterator node_it = it->second.find(star_id);
		if (node_it != it->second.end())
		{
			for (size_t i = 0; i < node_it->second.size(); i++)
			{
				if (star_level == (*(node_it->second[i])).star_level)
				{
					return node_it->second[i];
				}
			}
		}
	}

	return NULL;
}

const SuperGalaxiesCfg * StarChartConfig::GetSuperGalaxiesCfg(int star_id, int star_level) const
{
	SuperGalaxiesMap::const_iterator it = m_super_galaxies_cfg.find(star_id);
	if (it != m_super_galaxies_cfg.end())
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			if (star_level == it->second[i].star_level)
			{
				return &it->second[i];
			}
		}
	}

	return NULL;
}

const StarChartConnectCfg * StarChartConfig::GetStarChartConnectCfg(int star_chart_type, int star_chart_stage, int star_id)const
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return NULL;

	StarChartConnectMap::const_iterator it = m_star_chart_adjoin_cfg[star_chart_type].find(star_chart_stage);
	if (it != m_star_chart_adjoin_cfg[star_chart_type].end())
	{
		std::map<int, StarChartConnectCfg>::const_iterator node_it = it->second.find(star_id);
		if (node_it != it->second.end())
		{
			return &node_it->second;
		}
	}

	return NULL;
}

const StarChartConnectCfg * StarChartConfig::GetSuperGalaxiesConnectCfg(int star_id)const
{
	SuperGalaxiesConnectMap::const_iterator it = m_super_galaxies_adjoin_cfg.find(star_id);
	if (it != m_super_galaxies_adjoin_cfg.end())
	{
		return &it->second;
	}

	return NULL;
}

const std::set<int>* StarChartConfig::GetSkillHoleOpenCfg(int star_chart_type, int skill_hole)const
{
	if (star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return NULL;

	std::map<int, std::set<int> >::const_iterator it = m_skill_hole_cfg[star_chart_type].find(skill_hole);
	if (it != m_skill_hole_cfg[star_chart_type].end())
	{
		return &it->second;
	}

	return NULL;
}

int StarChartConfig::InitRoleStarChartCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartTypeMap& role_star_chart_map = m_star_chart_cfg[STAR_CHART_TYPE_ROLE];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_id", star_id) || star_id < 0 || star_id >= MAX_STAR_CHART_POINT) return -1;

		RoleStarChartCfg* role_star_chart = new RoleStarChartCfg();

		if (!GetSubNodeValue(data_element, "star_level", role_star_chart->star_level) || role_star_chart->star_level <= 0)
		{
			delete role_star_chart;
			return -2;
		}

		if (!GetSubNodeValue(data_element, "cost_coin", role_star_chart->cost_coin) || role_star_chart->cost_coin < 0)
		{
			delete role_star_chart;
			return -3;
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("cost_item_list");
			if (NULL == item_list_element)
			{
				delete role_star_chart;
				return -100;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("cost_item");
			if (NULL == item_element)
			{
				delete role_star_chart;
				return -101;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete role_star_chart;
					return -4;
				}

				role_star_chart->cost_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				delete role_star_chart;
				return -100;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("reward_item");
			if (NULL == item_element)
			{
				delete role_star_chart;
				return -101;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete role_star_chart;
					return -4;
				}

				role_star_chart->reward_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}
	
		for (int i = 1; i < PROFESSION_TYPE_NUM; i++)
		{
			int prof = i*PROFESSION_BASE;
			AttrInfo attr;

			std::string att_type = "att_type", att = "att";
			if (!GetSubNodeValue(data_element, att_type + STRING_SPRINTF("%d", i), attr.attr_type) || attr.attr_type < BATTLE_ATTR_MIN || attr.attr_type >= BATTLE_ATTR_MAX)
			{
				delete role_star_chart;
				return -5;
			}

			if (!GetSubNodeValue(data_element, att + STRING_SPRINTF("%d", i), attr.attr_value) || attr.attr_value < 0)
			{
				delete role_star_chart;
				return -6;
			}

			role_star_chart->prof_attrs[prof] = attr;
		}

		StarChartTypeMap::iterator it = role_star_chart_map.find(stage);
		if (it == role_star_chart_map.end())
		{
			role_star_chart_map[stage] = std::map<int, std::vector<BaseStarChartCfg*> >();
			it = role_star_chart_map.find(stage);
		}

		std::map<int, std::vector<BaseStarChartCfg*> >::iterator node_it = it->second.find(star_id);
		if (node_it == it->second.end())
		{
			it->second[star_id] = std::vector<BaseStarChartCfg*>();
			node_it = it->second.find(star_id);
		}

		node_it->second.push_back(role_star_chart);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitPetStarChartCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartTypeMap& pet_star_chart_map = m_star_chart_cfg[STAR_CHART_TYPE_PET];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_id", star_id) || star_id < 0 || star_id >= MAX_STAR_CHART_POINT) return -1;

		PetOrPartnerStarChartCfg* pet_star_chart = new PetOrPartnerStarChartCfg();

		if (!GetSubNodeValue(data_element, "star_level", pet_star_chart->star_level) || pet_star_chart->star_level <= 0)
		{
			delete pet_star_chart;
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "cost_coin", pet_star_chart->cost_coin) || pet_star_chart->cost_coin < 0)
		{
			delete pet_star_chart;
			return -__LINE__;
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("cost_item_list");
			if (NULL == item_list_element)
			{
				delete pet_star_chart;
				return -__LINE__;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("cost_item");
			if (NULL == item_element)
			{
				delete pet_star_chart;
				return -__LINE__;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete pet_star_chart;
					return -__LINE__;
				}

				pet_star_chart->cost_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				delete pet_star_chart;
				return -__LINE__;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("reward_item");
			if (NULL == item_element)
			{
				delete pet_star_chart;
				return -__LINE__;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete pet_star_chart;
					return -__LINE__;
				}

				pet_star_chart->reward_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}


		if (!GetSubNodeValue(data_element, "att_type", pet_star_chart->attr.attr_type) || pet_star_chart->attr.attr_type < BATTLE_ATTR_MIN || pet_star_chart->attr.attr_type >= BATTLE_ATTR_MAX)
		{
			delete pet_star_chart;
			return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "att", pet_star_chart->attr.attr_value) || pet_star_chart->attr.attr_value < 0)
		{
			delete pet_star_chart;
			return -__LINE__;
		}

		StarChartTypeMap::iterator it = pet_star_chart_map.find(stage);
		if (it == pet_star_chart_map.end())
		{
			pet_star_chart_map[stage] = std::map<int, std::vector<BaseStarChartCfg*> >();
			it = pet_star_chart_map.find(stage);
		}

		std::map<int, std::vector<BaseStarChartCfg*> >::iterator node_it = it->second.find(star_id);
		if (node_it == it->second.end())
		{
			it->second[star_id] = std::vector<BaseStarChartCfg*>();
			node_it = it->second.find(star_id);
		}

		node_it->second.push_back(pet_star_chart);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitPartnerStarChartCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartTypeMap& partner_star_chart_map = m_star_chart_cfg[STAR_CHART_TYPE_PARTNER];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_id", star_id) || star_id < 0 || star_id >= MAX_STAR_CHART_POINT) return -1;

		PetOrPartnerStarChartCfg* partner_star_chart = new PetOrPartnerStarChartCfg();

		if (!GetSubNodeValue(data_element, "star_level", partner_star_chart->star_level) || partner_star_chart->star_level <= 0)
		{
			delete partner_star_chart;
			return -2;
		}

		if (!GetSubNodeValue(data_element, "cost_coin", partner_star_chart->cost_coin) || partner_star_chart->cost_coin < 0)
		{
			delete partner_star_chart;
			return -3;
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("cost_item_list");
			if (NULL == item_list_element)
			{
				delete partner_star_chart;
				return -100;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("cost_item");
			if (NULL == item_element)
			{
				delete partner_star_chart;
				return -101;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete partner_star_chart;
					return -4;
				}

				partner_star_chart->cost_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				delete partner_star_chart;
				return -100;
			}

			TiXmlElement* item_element = item_list_element->FirstChildElement("reward_item");
			if (NULL == item_element)
			{
				delete partner_star_chart;
				return -101;
			}

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					delete partner_star_chart;
					return -4;
				}

				partner_star_chart->reward_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(data_element, "att_type", partner_star_chart->attr.attr_type) || partner_star_chart->attr.attr_type < BATTLE_ATTR_MIN || partner_star_chart->attr.attr_type >= BATTLE_ATTR_MAX)
		{
			delete partner_star_chart;
			return -5;
		}

		if (!GetSubNodeValue(data_element, "att", partner_star_chart->attr.attr_value) || partner_star_chart->attr.attr_value < 0)
		{
			delete partner_star_chart;
			return -6;
		}

		StarChartTypeMap::iterator it = partner_star_chart_map.find(stage);
		if (it == partner_star_chart_map.end())
		{
			partner_star_chart_map[stage] = std::map<int, std::vector<BaseStarChartCfg*> >();
			it = partner_star_chart_map.find(stage);
		}

		std::map<int, std::vector<BaseStarChartCfg*> >::iterator node_it = it->second.find(star_id);
		if (node_it == it->second.end())
		{
			it->second[star_id] = std::vector<BaseStarChartCfg*>();
			node_it = it->second.find(star_id);
		}

		node_it->second.push_back(partner_star_chart);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitRoleStarChartConnectCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartConnectMap& star_chart_connect_map = m_star_chart_adjoin_cfg[STAR_CHART_TYPE_ROLE];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_type", star_id) || !this->ValidStarChartPoint(STAR_CHART_TYPE_ROLE, stage, star_id))
		{
			errormsg = STRING_SPRINTF("Invalid star_id[%d] or grade[%d]", star_id, stage);
			return -2;
		}

		StarChartConnectCfg connect_cfg;

		if (!GetSubNodeValue(data_element, "condition", connect_cfg.condition_level) || connect_cfg.condition_level < 0) return -3;

		TiXmlElement* adjoin_list_element = data_element->FirstChildElement("adjoin");
		if (NULL == adjoin_list_element) return -100;
		TiXmlElement* node_element = adjoin_list_element->FirstChildElement("node");
		while (node_element)
		{
			int adjoin = -1;
			if (!GetNodeValue(node_element, adjoin) || !this->ValidStarChartPoint(STAR_CHART_TYPE_ROLE, stage, adjoin))
			{
				errormsg = STRING_SPRINTF("Invalid star_chart_point! adjoin[%d]", adjoin);
				return -4;
			}

			if (adjoin != star_id)	// 相邻星位不包括自己
			{
				connect_cfg.adjoin.insert(adjoin);
			}

			node_element = node_element->NextSiblingElement();
		}

		StarChartConnectMap::iterator it = star_chart_connect_map.find(stage);
		if (it == star_chart_connect_map.end())
		{
			star_chart_connect_map[stage] = std::map<int, StarChartConnectCfg>();
			it = star_chart_connect_map.find(stage);
		}

		{
			std::map<int, StarChartConnectCfg>::iterator node_it = it->second.find(star_id);
			if (node_it != it->second.end())
			{
				errormsg = STRING_SPRINTF("repeat star_id[%d]", star_id);
				return -301;
			}
		}

		it->second.insert(std::make_pair(star_id, connect_cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitPartnerStarChartConnectCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartConnectMap& star_chart_connect_map = m_star_chart_adjoin_cfg[STAR_CHART_TYPE_PARTNER];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_type", star_id) || !this->ValidStarChartPoint(STAR_CHART_TYPE_PARTNER, stage, star_id))
		{
			errormsg = STRING_SPRINTF("Invalid star_id[%d]", star_id);
			return -2;
		}

		StarChartConnectCfg connect_cfg;

		if (!GetSubNodeValue(data_element, "condition", connect_cfg.condition_level) || connect_cfg.condition_level < 0) return -3;

		TiXmlElement* adjoin_list_element = data_element->FirstChildElement("adjoin");
		if (NULL == adjoin_list_element) return -100;
		TiXmlElement* node_element = adjoin_list_element->FirstChildElement("node");
		while (node_element)
		{
			int adjoin = -1;
			if (!GetNodeValue(node_element, adjoin) || !this->ValidStarChartPoint(STAR_CHART_TYPE_PARTNER, stage, adjoin))
			{
				errormsg = STRING_SPRINTF("Invalid star_chart_point! adjoin[%d]", adjoin);
				return -4;
			}

			if (adjoin != star_id)	// 相邻星位不包括自己
			{
				connect_cfg.adjoin.insert(adjoin);
			}

			node_element = node_element->NextSiblingElement();
		}

		StarChartConnectMap::iterator it = star_chart_connect_map.find(stage);
		if (it == star_chart_connect_map.end())
		{
			star_chart_connect_map[stage] = std::map<int, StarChartConnectCfg>();
			it = star_chart_connect_map.find(stage);
		}

		{
			std::map<int, StarChartConnectCfg>::iterator node_it = it->second.find(star_id);
			if (node_it != it->second.end())
			{
				errormsg = STRING_SPRINTF("repeat star_id[%d]", star_id);
				return -301;
			}
		}

		it->second.insert(std::make_pair(star_id, connect_cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitPetStarChartConnectCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		StarChartConnectMap& star_chart_connect_map = m_star_chart_adjoin_cfg[STAR_CHART_TYPE_PET];

		int stage = -1;
		if (!GetSubNodeValue(data_element, "grade", stage) || stage < 0 || stage >= MAX_STAR_CHART_STAGE) return -205;

		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_type", star_id) || !this->ValidStarChartPoint(STAR_CHART_TYPE_PET, stage, star_id))
		{
			errormsg = STRING_SPRINTF("Invalid star_id[%d]", star_id);
			return -2;
		}

		StarChartConnectCfg connect_cfg;

		if (!GetSubNodeValue(data_element, "condition", connect_cfg.condition_level) || connect_cfg.condition_level < 0) return -3;

		TiXmlElement* adjoin_list_element = data_element->FirstChildElement("adjoin");
		if (NULL == adjoin_list_element) return -100;
		TiXmlElement* node_element = adjoin_list_element->FirstChildElement("node");
		while (node_element)
		{
			int adjoin = -1;
			if (!GetNodeValue(node_element, adjoin) || !this->ValidStarChartPoint(STAR_CHART_TYPE_PET, stage, adjoin))
			{
				errormsg = STRING_SPRINTF("Invalid star_chart_point! adjoin[%d]", adjoin);
				return -4;
			}

			if (adjoin != star_id)	// 相邻星位不包括自己
			{
				connect_cfg.adjoin.insert(adjoin);
			}

			node_element = node_element->NextSiblingElement();
		}

		StarChartConnectMap::iterator it = star_chart_connect_map.find(stage);
		if (it == star_chart_connect_map.end())
		{
			star_chart_connect_map[stage] = std::map<int, StarChartConnectCfg>();
			it = star_chart_connect_map.find(stage);
		}

		{
			std::map<int, StarChartConnectCfg>::iterator node_it = it->second.find(star_id);
			if (node_it != it->second.end())
			{
				errormsg = STRING_SPRINTF("repeat star_id[%d]", star_id);
				return -301;
			}
		}

		it->second.insert(std::make_pair(star_id, connect_cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitStarChartSkillCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int star_chart_type = -1;
		if (!GetSubNodeValue(data_element, "starmap_tpye", star_chart_type) || star_chart_type < 0 || star_chart_type >= STAR_CHART_TYPE_MAX) return -1;

		std::map<int, std::set<int> >& skill_hole_map = m_skill_hole_cfg[star_chart_type];

		int skill_hole = 0;
		if (!GetSubNodeValue(data_element, "skill_hole", skill_hole) || skill_hole < 0 || skill_hole >= MAX_STAR_CHART_SKILL) return -2;

		TiXmlElement* unlock_star_element = data_element->FirstChildElement("unlock_star");
		if (NULL == unlock_star_element) return -101;

		TiXmlElement* node_element = unlock_star_element->FirstChildElement("node");
		std::set<int> t_unlock_star_set;
		while (node_element)
		{
			int unlock_star = -1;
			if (!GetNodeValue(node_element, unlock_star) || !this->ValidStarChartPoint(star_chart_type, 0, unlock_star))
			{
				errormsg = STRING_SPRINTF("Invalid StarChartPoint unlock_star[%d]", unlock_star);
				return -3;
			}

			t_unlock_star_set.insert(unlock_star);

			node_element = node_element->NextSiblingElement();
		}

		skill_hole_map[skill_hole] = t_unlock_star_set;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitSuperGalaxiesCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_id", star_id) || star_id < 0 || star_id >= MAX_SUPER_GALAXIES_POINT) return -1;

		SuperGalaxiesMap::iterator it = m_super_galaxies_cfg.find(star_id);
		if (it == m_super_galaxies_cfg.end())
		{
			m_super_galaxies_cfg[star_id] = std::vector<SuperGalaxiesCfg>();
			it = m_super_galaxies_cfg.find(star_id);
			if (it == m_super_galaxies_cfg.end()) return -101;
		}

		SuperGalaxiesCfg super_galaxies;

		if (!GetSubNodeValue(data_element, "star_level", super_galaxies.star_level) || super_galaxies.star_level <= 0) return -2;
		if (!GetSubNodeValue(data_element, "cost_coin", super_galaxies.cost_coin) || super_galaxies.cost_coin < 0) return -3;

		{
			TiXmlElement* item_list_element = data_element->FirstChildElement("cost_item_list");
			if (NULL == item_list_element) return -201;
			TiXmlElement* item_element = item_list_element->FirstChildElement("cost_item");
			if (NULL == item_element) return -202;

			while (item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -4;

				super_galaxies.cost_item_list.push_back(item);

				item_element = item_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(data_element, "target", super_galaxies.target) || super_galaxies.target < 0 || super_galaxies.target >= STAR_CHART_TYPE_MAX) return -5;
		if (!GetSubNodeValue(data_element, "star_type", super_galaxies.star_type) || super_galaxies.star_type < 0 || super_galaxies.star_type >= SUPER_GALAXIESCFG_TYPE_MAX) return -6;

		std::pair<int, int> param;
		if (!GetSubNodeValue(data_element, "param1", param.first) || param.first < 0) return -7;
		if (!GetSubNodeValue(data_element, "param2", param.second) || param.second < 0) return -8;

		super_galaxies.param = param;

		it->second.push_back(super_galaxies);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitSuperGalaxiesConnectCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int star_id = -1;
		if (!GetSubNodeValue(data_element, "star_type", star_id) || !this->ValidSuperGalaxiesPoint(star_id))
		{
			errormsg = STRING_SPRINTF("Invalid star_id[%d]", star_id);
			return -1;
		}

		SuperGalaxiesConnectMap::iterator it = m_super_galaxies_adjoin_cfg.find(star_id);
		if (it != m_super_galaxies_adjoin_cfg.end())
		{
			errormsg = STRING_SPRINTF("repeat star_id[%d]", star_id);
			return -101;
		}

		StarChartConnectCfg connect_cfg;

		if (!GetSubNodeValue(data_element, "condition", connect_cfg.condition_level) || connect_cfg.condition_level < 0) return -2;

		TiXmlElement* adjoin_list_element = data_element->FirstChildElement("adjoin");
		if (NULL == adjoin_list_element) return -100;
		TiXmlElement* node_element = adjoin_list_element->FirstChildElement("node");
		while (node_element)
		{
			int adjoin = -1;
			if (!GetNodeValue(node_element, adjoin) || !this->ValidSuperGalaxiesPoint(adjoin))
			{
				errormsg = STRING_SPRINTF("Invalid super_galaxies_point! adjoin[%d]", adjoin);
				return -3;
			}

			if (adjoin != star_id)
			{
				connect_cfg.adjoin.insert(adjoin);
			}
			
			node_element = node_element->NextSiblingElement();
		}

		m_super_galaxies_adjoin_cfg[star_id] = connect_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int StarChartConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "reset_back", m_other_cfg.reset_back) || m_other_cfg.reset_back < 0 || m_other_cfg.reset_back > STAR_CHART_RESET_POINT_RATIO) return -1;
	if (!GetSubNodeValue(data_element, "reset_cion1", m_other_cfg.reset_cion1) || m_other_cfg.reset_cion1 < 0) return -2;
	if (!GetSubNodeValue(data_element, "reset_cion2", m_other_cfg.reset_gold2) || m_other_cfg.reset_gold2 < 0) return -3;

	return 0;
}

void StarChartConfig::Destroy()
{
	for (int i = 0; i < STAR_CHART_TYPE_MAX; i++)
	{
		StarChartTypeMap& star_chart = m_star_chart_cfg[i];

		for (StarChartTypeMap::iterator it = star_chart.begin(); it != star_chart.end(); it++)
		{
			for (std::map<int, std::vector<BaseStarChartCfg*> >::iterator node_it = it->second.begin(); node_it != it->second.end(); node_it++)
			{
				for (size_t j = 0; j < node_it->second.size(); j++)
				{
					if (NULL != node_it->second[j])
					{
						delete node_it->second[j];
						node_it->second[j] = NULL;
					}
				}
			}
		}
	}
}
