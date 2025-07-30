#include "lingyaojiuzhuconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

LingYaoJiuZhuFbConfig::LingYaoJiuZhuFbConfig()
{
}

LingYaoJiuZhuFbConfig::~LingYaoJiuZhuFbConfig()
{
}

bool LingYaoJiuZhuFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOhterCfg);
	
	return true;
}

int LingYaoJiuZhuFbConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "special_task_id", m_other_cfg.special_task_id) || m_other_cfg.special_task_id < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "last_task_id", m_other_cfg.last_task_id) || m_other_cfg.last_task_id < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "special_talk", m_other_cfg.special_talk) || m_other_cfg.special_talk < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "restart_talk", m_other_cfg.restart_talk) || m_other_cfg.restart_talk < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "task_item_id", m_other_cfg.task_item_id) || m_other_cfg.task_item_id < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "yjcnpc_ID", m_other_cfg.yjcnpc_ID) || m_other_cfg.yjcnpc_ID < 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "scene_ID", m_other_cfg.scene_ID) || m_other_cfg.scene_ID < 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "x", m_other_cfg.x) || m_other_cfg.x < 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "y", m_other_cfg.y) || m_other_cfg.y < 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "dir_y", m_other_cfg.dir_y) || m_other_cfg.dir_y < 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "npc_id_1", m_other_cfg.npc_id_1) || m_other_cfg.npc_id_1 < 0)
	{
		return -11;
	}

	if (!GetSubNodeValue(root_element, "special_bubble_1", m_other_cfg.special_bubble_1) || m_other_cfg.special_bubble_1 < 0)
	{
		return -12;
	}
	
	return 0;
}