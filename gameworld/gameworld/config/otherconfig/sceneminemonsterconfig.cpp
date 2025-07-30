#include "sceneminemonsterconfig.hpp"
#include "gameworld/gameworld/checkresourcecenter.hpp"
#include "config/sceneconfig.h"

SceneMineMonsterConfig::SceneMineMonsterConfig()
{
}

SceneMineMonsterConfig::~SceneMineMonsterConfig()
{
}

SceneMineMonsterConfig* g_mine_monster_cfg = NULL;
SceneMineMonsterConfig& SceneMineMonsterConfig::Instance()
{
	if (NULL == g_mine_monster_cfg)
	{
		g_mine_monster_cfg = new SceneMineMonsterConfig();
	}

	return *g_mine_monster_cfg;
}

bool SceneMineMonsterConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("mine_monsters", InitSceneMineMonster);
	LOAD_CONFIG("activity_limit", InitActiveLimit);

	return true;
}

bool SceneMineMonsterConfig::Reload(const std::string& configname, std::string *err)
{
	SceneMineMonsterConfig *temp = new SceneMineMonsterConfig();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_mine_monster_cfg) delete g_mine_monster_cfg;

	g_mine_monster_cfg = temp;

	return true;
}

const SceneMineMonsterCfg * SceneMineMonsterConfig::GetSceneMineMonsterCfg(int scene_id)
{
	std::map<int, SceneMineMonsterCfg>::iterator iter = m_scene_id_to_scene_mine_cfg.find(scene_id);
	if (iter == m_scene_id_to_scene_mine_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

bool SceneMineMonsterConfig::IsMeetActive(int role_level, int active_num)
{
	for (std::map<int, int>::const_iterator it = m_active_limit_cfg.begin(); it != m_active_limit_cfg.end(); ++it)
	{
		if ((it->first % 1000) <= role_level && role_level <= (it->first / 1000) && active_num >= it->second)
		{
			return true;
		}
	}

	return false;
}

int SceneMineMonsterConfig::InitSceneMineMonster(TiXmlElement * RootElement)
{
	TiXmlElement* root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id))
		{
			return -1;
		}

		if (m_scene_id_to_scene_mine_cfg.count(scene_id) != 0)
		{
			return -12341;
		}

		SceneMineMonsterCfg & cfg = m_scene_id_to_scene_mine_cfg[scene_id];

		if (!GetSubNodeValue(root_element, "mine_monster_count_min", cfg.mine_monster_count_min) || cfg.mine_monster_count_min < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "mine_monster_count_max", cfg.mine_monster_count_max) || cfg.mine_monster_count_max < cfg.mine_monster_count_min)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "mine_interval_min", cfg.mine_interval_min_s) || cfg.mine_interval_min_s < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "mine_interval_max", cfg.mine_interval_max_s) || cfg.mine_interval_max_s < cfg.mine_interval_min_s)
		{
			return -5;
		}

		if (GetSubNodeValue(root_element, "mine_monster_level_min", cfg.mine_monster_level_min))
		{
			if (cfg.mine_monster_level_min < 0 || cfg.mine_monster_level_min > MONSTER_MAX_LEVEL)
			{
				return -6;
			}
		}

		if (GetSubNodeValue(root_element, "mine_monster_level_max", cfg.mine_monster_level_max))
		{
			if (cfg.mine_monster_level_max < cfg.mine_monster_level_min)
			{
				return -7;
			}
		}

		TiXmlElement * mine_monsters_list_element = root_element->FirstChildElement("mine_monsters");
		if (NULL != mine_monsters_list_element)
		{
			TiXmlElement *node_element = mine_monsters_list_element->FirstChildElement("node");
			while (node_element)
			{
				int monster_id = 0;				
				if (!GetNodeValue(node_element, monster_id))
				{
					return -299;
				}

				CheckResourceCenter::Instance().GetMonsterCheck()->Add(monster_id, __FUNCTION__);

				cfg.mine_monsters.push_back(monster_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (cfg.mine_monsters.size() > 0)
		{
			if (0 == cfg.mine_monster_count_min || 0 == cfg.mine_monster_count_max)
			{
				return -100;
			}

			if (0 == cfg.mine_interval_min_s || 0 == cfg.mine_interval_max_s)
			{
				return -101;
			}
		}

		TiXmlElement * mine_monsters_drop_list_element = root_element->FirstChildElement("drop_id");
		if (NULL != mine_monsters_drop_list_element)
		{
			TiXmlElement *node_element = mine_monsters_drop_list_element->FirstChildElement("dropid");
			while (node_element)
			{
				int drop_id = 0;
				if (!GetNodeValue(node_element, drop_id))
				{
					return -399;
				}

				if (drop_id > 0)
				{
					cfg.drop_list.push_back(drop_id);
					if (static_cast<int>(cfg.drop_list.size()) > MINE_MONSTERS_DROP_NUM)
					{
						return -398;
					}
				}

				node_element = node_element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(root_element, "prestige_id", cfg.prestige_id) || cfg.prestige_id < 0)
		{
			return -10;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SceneMineMonsterConfig::InitActiveLimit(TiXmlElement * RootElement)
{
	TiXmlElement* root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	int last_max_level = 0;
	while (root_element)
	{
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_level) || min_level <= last_max_level)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		last_max_level = max_level;

		int bind_need_active = 0;
		if (!GetSubNodeValue(root_element, "bind_need_active", bind_need_active) || bind_need_active < 0)
		{
			return -3;
		}
		int level_key = min_level + max_level * 1000;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_active_limit_cfg[level_key] = bind_need_active;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
