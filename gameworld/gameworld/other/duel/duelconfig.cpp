#include "item/itempool.h"
#include "duelconfig.hpp"

DuelConfig::DuelConfig()
{
}

DuelConfig::~DuelConfig()
{
	std::vector<DuelKillCfg>().swap(m_kill_cfg);
}

bool DuelConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("scene_id", InitMapCfg);
	LOAD_CONFIG("kill_value", InitKillCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const DuelMapCfg * DuelConfig::GetMapCfg(int scene_id) const
{
	std::map<int, DuelMapCfg>::const_iterator it = m_map_cfg.find(scene_id);
	if (it == m_map_cfg.end()) return NULL;

	return &it->second;
}

const DuelKillCfg * DuelConfig::GetKillCfg(int kill_vaule) const
{
	for (std::vector<DuelKillCfg>::const_iterator it = m_kill_cfg.begin(); it != m_kill_cfg.end(); ++it)
	{
		if (it->min_value <= kill_vaule && kill_vaule <= it->max_value)
		{
			return &(*it);
		}
	}

	return NULL;
}

const bool DuelConfig::SceneCanDuel(int scene_id) const
{
	std::map<int, DuelMapCfg>::const_iterator it = m_map_cfg.find(scene_id);
	if (it == m_map_cfg.end())
	{
		return false;
	}

	return true;
}

int DuelConfig::InitMapCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id) || scene_id <= 0)
		{
			return -1;
		}
		if (m_map_cfg.find(scene_id) != m_map_cfg.end())
		{
			return -99;
		}

		DuelMapCfg & cfg = m_map_cfg[scene_id];

		if (!GetSubNodeValue(root_element, "revive_map", cfg.revive_map) || cfg.revive_map < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "revive_point_x", cfg.pos.x) || cfg.pos.x <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "revive_point_y", cfg.pos.y) || cfg.pos.y <= 0)
		{
			return -4;
		}
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int DuelConfig::InitKillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		DuelKillCfg cfg;

		if (!GetSubNodeValue(root_element, "kill_level", cfg.kill_level) || cfg.kill_level < 0 || cfg.kill_level != (int)m_kill_cfg.size())
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "kill_value_kill_value", cfg.min_value) || cfg.min_value <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "kill_value_max", cfg.max_value))
		{
			return -3;
		}
		if (-1 == cfg.max_value)
		{
			cfg.max_value = MAX_INT;
		}

		if (!GetSubNodeValue(root_element, "titles_id", cfg.title_id) || cfg.title_id <= 0 || cfg.title_id >= MAX_UINT16)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "attr_down", cfg.attr_down) || cfg.attr_down < 0)
		{
			return -5;
		}

		m_kill_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int DuelConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		DuelOtherCfg & cfg = m_other_cfg;

		if (!GetSubNodeValue(root_element, "kill_value_num", cfg.kill_value_base) || cfg.kill_value_base < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "kill_value_multiple", cfg.kill_value_multiple) || cfg.kill_value_multiple < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "reduce_kill_balue_time", cfg.reduce_kill_value_time) || cfg.reduce_kill_value_time <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "reduce_exp", cfg.reduce_exp) || cfg.reduce_exp < 0 || cfg.reduce_exp > 100)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "reduce_", cfg.reduce_coin) || cfg.reduce_coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "item_reduce_kill_value", cfg.item_reduce_kill_value) || cfg.item_reduce_kill_value < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "item_reduce_times", cfg.item_reduce_daily_times) || cfg.item_reduce_daily_times < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "death_down", cfg.loss_reduce_kill_value) || cfg.loss_reduce_kill_value < 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "duel_taken", cfg.duel_item_id) ||  NULL == ITEMPOOL->GetItem(cfg.duel_item_id))
		{
			return -9;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
