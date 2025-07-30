#include "yingyuanyixianqianconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

YYYXQConfig::YYYXQConfig()
{
}

YYYXQConfig::~YYYXQConfig()
{
}

bool YYYXQConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOhterCfg);
	
	return true;
}

int YYYXQConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "yjcnpc_ID", m_other_cfg.yjcnpc_ID) || m_other_cfg.yjcnpc_ID < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "scene_ID", m_other_cfg.scene_ID) || m_other_cfg.scene_ID < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "x", m_other_cfg.x) || m_other_cfg.x < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "y", m_other_cfg.y) || m_other_cfg.y < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "dir_y", m_other_cfg.dir_y) || m_other_cfg.dir_y < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "npc_id_1", m_other_cfg.npc_id_1) || m_other_cfg.npc_id_1 < 0)
	{
		return -6;
	}
	
	return 0;
}
