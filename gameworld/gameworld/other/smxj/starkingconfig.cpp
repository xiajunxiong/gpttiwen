#include "starkingconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

StarKingFbConfig::StarKingFbConfig()
{
}

StarKingFbConfig::~StarKingFbConfig()
{
}

bool StarKingFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOhterCfg);
	LOAD_CONFIG("smhj_fb", InitConfig);
	
	return true;
}

int StarKingFbConfig::InitOhterCfg(TiXmlElement * RootElement)
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

	if (!GetSubNodeValue(root_element, "level", m_other_cfg.level) || m_other_cfg.level < 0)
	{
		return -4;
	}
	
	return 0;
}

int StarKingFbConfig::InitConfig(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "scene_id", m_star_king_cfg.scene_id) || m_star_king_cfg.scene_id < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "npc_id", m_star_king_cfg.npc_id) || m_star_king_cfg.npc_id < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "pos_x1", m_star_king_cfg.pos_x1) || m_star_king_cfg.pos_x1 < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "pos_y1", m_star_king_cfg.pos_y1) || m_star_king_cfg.pos_y1 < 0)
	{
		return -4;
	}

	return 0;
}
