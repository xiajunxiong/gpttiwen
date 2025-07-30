#include "treasuretaskconfig.hpp"

TreasureTaskConfig::TreasureTaskConfig()
{
}

TreasureTaskConfig::~TreasureTaskConfig()
{
	for (std::map<int, std::vector<TreasureTaskLevelCfg> >::iterator it = m_level_cfg.begin(); it != m_level_cfg.end(); it++)
	{
		std::vector<TreasureTaskLevelCfg>().swap(it->second);
	}
	std::vector<TreasureTaskGroupMap>().swap(m_group_cfg);
	std::vector<TreasureTaskCoordinateCfg>().swap(m_coordomate_cfg);
	std::vector<TreasureTaskRouteMap>().swap(m_route_cfg);
}

bool TreasureTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("turntable", InitLevelCfg);
	LOAD_CONFIG("reward_group", InitGroupCfg);
	LOAD_CONFIG("reward_coordinate", InitCoordomateCfg);
	LOAD_CONFIG("reward_route", InitRouteCfg);

	return true;
}

const std::vector<TreasureTaskLevelCfg>* TreasureTaskConfig::GetTaskLevelVec(int level)
{
	std::map<int, std::vector<TreasureTaskLevelCfg> >::const_iterator level_it = m_level_cfg.upper_bound(level);
	if (level_it == m_level_cfg.begin())
		return NULL;
	level_it--;

	return &level_it->second;
}

const TreasureTaskGroupCfg * TreasureTaskConfig::GetGroupRewardCfg(int group_id)
{
	if (group_id < 0 || group_id >= (int)m_group_cfg.size())
		return NULL;

	int rate = RandomNum(10000);
	std::map<int, TreasureTaskGroupCfg>::iterator it = m_group_cfg[group_id].group_cfg.upper_bound(rate);
	if (it == m_group_cfg[group_id].group_cfg.end())
		return NULL;

	return &it->second;
}

short TreasureTaskConfig::GetCoordomate(short route, short order_id)
{
	if (order_id <= 0 || order_id > (int)m_route_cfg.size() || route <= 0 || route > m_route_num)
		return 0;

	return m_route_cfg[order_id - 1].route[route - 1];
}

int TreasureTaskConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "quantity", m_other_cfg.quantity))
			return -1;

		if (!GetSubNodeValue(root_element, "level", m_other_cfg.level))
			return -2;

		if (!GetSubNodeValue(root_element, "floors", m_other_cfg.floors))
			return -3;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureTaskConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int level_min = 0;
		if (!GetSubNodeValue(root_element, "level_min", level_min) || level_min <= 0)
			return -1;

		TreasureTaskLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq <= 0)
			return -2;

		if (!GetSubNodeValue(root_element, "reward_group", cfg.reward_group) || cfg.reward_group < 0)
			return -3;

		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0)
			return -4;

		if (!GetSubNodeValue(root_element, "get_rate", cfg.get_rate) || cfg.get_rate <= 0)
			return -5;

		m_level_cfg[level_min].push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureTaskConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_group = -1, last_rate = 10000;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id < 0)
			return -1;

		if (group_id != last_group)
		{
			if (last_rate != 10000)
				return -1000;

			last_group = group_id;
			last_rate = 0;

			TreasureTaskGroupMap map;
			m_group_cfg.push_back(map);
		}

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0)
			return -2;

		if (last_rate + rate > 10000)
			return -3;

		last_rate += rate;

		TreasureTaskGroupCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -4;

		if (group_id != 0)
		{
			TiXmlElement * item_element = root_element->FirstChildElement("reward");
			if (NULL != item_element)
			{
				if (!cfg.reward.ReadConfig(item_element))
					return -5;
			}
		}	

		m_group_cfg[group_id].group_cfg[last_rate] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureTaskConfig::InitCoordomateCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TreasureTaskCoordinateCfg cfg;
		if (!GetSubNodeValue(root_element, "scene", cfg.scene_id) || cfg.scene_id <= 0)
			return -1;
		if (!GetSubNodeValue(root_element, "x", cfg.pos.x) || cfg.pos.x <= 0)
			return -2;
		if (!GetSubNodeValue(root_element, "y", cfg.pos.y) || cfg.pos.y <= 0)
			return -3;

		m_coordomate_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureTaskConfig::InitRouteCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	m_route_num = 0;
	while (NULL != root_element)
	{
		TreasureTaskRouteMap map;
		
		short pos_id = 0, i = 1;
		char route[64] = { 0 }; sprintf(route, "route%d", i);
		
		while(GetSubNodeValue(root_element, route, pos_id))
		{
			map.route.push_back(pos_id);

			i++;
			sprintf(route, "route%d", i);
		}

		m_route_num = i - 1;
		m_route_cfg.push_back(map);
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_route_cfg.size() != m_other_cfg.quantity)
		return -1000;

	return 0;
}
