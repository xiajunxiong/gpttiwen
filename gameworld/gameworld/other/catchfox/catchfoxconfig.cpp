#include "catchfoxconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

CatchFoxFbConfig::CatchFoxFbConfig()
{
}

CatchFoxFbConfig::~CatchFoxFbConfig()
{
	std::vector<CatchFoxMonsterConfig>().swap(m_monster_cfg_vec);
}

bool CatchFoxFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOhterCfg);
	LOAD_CONFIG("find_fox", InitFindFoxConfig);
	LOAD_CONFIG("level_monster_group", InitLevelMonsterGroup);
	
	return true;
}

const CatchFoxMonsterConfig * CatchFoxFbConfig::GetMonsterConfig(int level)
{
	for (std::vector<CatchFoxMonsterConfig>::iterator it = m_monster_cfg_vec.begin(); it != m_monster_cfg_vec.end(); it++)
	{
		if (level >= it->min_level && level <= it->max_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int CatchFoxFbConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "task_id", m_other_cfg.task_id) || m_other_cfg.task_id < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "last_task_id", m_other_cfg.last_task_id) || m_other_cfg.last_task_id < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "npc_id", m_other_cfg.npc_id) || m_other_cfg.npc_id < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "fox_npc_id", m_other_cfg.fox_npc_id) || m_other_cfg.fox_npc_id < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "level", m_other_cfg.level) || m_other_cfg.level < 0)
	{
		return -4;
	}
	
	return 0;
}

int CatchFoxFbConfig::InitFindFoxConfig(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "level", m_catch_fox_cfg.level) || m_catch_fox_cfg.level < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_catch_fox_cfg.scene_id) || m_catch_fox_cfg.scene_id < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "pos_x1", m_catch_fox_cfg.pos_x1) || m_catch_fox_cfg.pos_x1 < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "pos_y1", m_catch_fox_cfg.pos_y1) || m_catch_fox_cfg.pos_y1 < 0)
	{
		return -4;
	}

	return 0;
}

int CatchFoxFbConfig::InitLevelMonsterGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		CatchFoxMonsterConfig node;
		if (!GetSubNodeValue(root_element, "npc_id", node.npc_id) || node.npc_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", node.min_level) || node.min_level < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", node.max_level) || node.max_level < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "monster_group_id", node.monster_group_id) || node.monster_group_id < 0)
		{
			return -4;
		}

		m_monster_cfg_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
