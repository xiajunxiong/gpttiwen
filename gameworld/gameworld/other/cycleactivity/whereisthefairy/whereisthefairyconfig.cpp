#include "whereisthefairyconfig.hpp"

#include "npc/npcpool.h"
#include "world.h"

WhereIsTheFairyConfig::WhereIsTheFairyConfig()
{
}

WhereIsTheFairyConfig::~WhereIsTheFairyConfig()
{
	std::vector<WhereIsTheFairySceneCfg>().swap(m_scene_cfg);
}

bool WhereIsTheFairyConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("coordinate_group", InitSceneCfg);
	LOAD_CONFIG_2("play_config", InitBaseCfg);
	LOAD_CONFIG_2("group_id", InitRewardCfg);

	return true;
}

bool WhereIsTheFairyConfig::GetRandSceneCfg(unsigned int refresh_time, WhereIsTheFairySceneCfg & out_cfg) const
{
	if(refresh_time <= 0 || m_scene_cfg.empty()) return false;

	//此处使用随机种子是为了原服与跨服一致
	unsigned int seed = refresh_time % 13131;
	std::vector<WhereIsTheFairySceneCfg> scene_cfg(m_scene_cfg);

	RandomShuffle(&scene_cfg[0], 0, (int)scene_cfg.size(), seed);
	out_cfg = scene_cfg[0];
	return true;
}

int WhereIsTheFairyConfig::InitBaseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "npc_id", m_base_cfg.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(m_base_cfg.npc_id))
	{
		errormsg = STRING_SPRINTF("[npc_id=%d] error, is not find data?", m_base_cfg.npc_id);
		return -1;
	}

	if (!GetSubNodeValue(data_element, "live_time", m_base_cfg.live_time) || m_base_cfg.live_time <= 0 || m_base_cfg.live_time > SECOND_PER_DAY)
	{
		errormsg = STRING_SPRINTF("[npc_id=%d] error, [live_time:%d] is not find data or <= 0 or > %dd?", m_base_cfg.npc_id, m_base_cfg.live_time, SECOND_PER_DAY);
		return -2;
	}

	return 0;
}

int WhereIsTheFairyConfig::InitSceneCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	
	while (NULL != data_element)
	{
		WhereIsTheFairySceneCfg node_cfg;
		if (!GetSubNodeValue(data_element, "x", node_cfg.npc_pos.x) || node_cfg.npc_pos.x < 0)
		{
			errormsg = STRING_SPRINTF("[x:%d] error, is not find data or < 0?", node_cfg.npc_pos.x);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "y", node_cfg.npc_pos.y) || node_cfg.npc_pos.y < 0)
		{
			errormsg = STRING_SPRINTF("[x:%d] error, [y:%d] is not find data or < 0?", node_cfg.npc_pos.x, node_cfg.npc_pos.y);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "scene_id", node_cfg.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(node_cfg.scene_id))
		{
			errormsg = STRING_SPRINTF("[x:%d, y:%d] error, [scene_id:%d] is not find data or <= 0 or not has scene?", node_cfg.npc_pos.x, node_cfg.npc_pos.y, node_cfg.scene_id);
			return -3;
		}
		if (!GetSubNodeValue(data_element, "dir", node_cfg.npc_dir) || node_cfg.npc_dir < 0)
		{
			errormsg = STRING_SPRINTF("[x:%d, y:%d] error, [dir:%d] is not find data or < 0?", node_cfg.npc_pos.x, node_cfg.npc_pos.y, node_cfg.npc_dir);
			return -4;
		}

		m_scene_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	if (m_scene_cfg.empty())
	{
		errormsg = STRING_SPRINTF("InitSceneCfg error, [coordinate_group] is empty?");
		return -888;
	}

	return 0;
}

int WhereIsTheFairyConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	TiXmlElement * list_element = data_element->FirstChildElement("reward_list");
	if (NULL == list_element)
	{
		return -1000;
	}

	TiXmlElement * rewrad_element = list_element->FirstChildElement("reward");
	if (NULL == rewrad_element)
	{
		return -1000;
	}
	while (NULL != rewrad_element)
	{
		ItemConfigData item;
		if (!item.ReadConfig(rewrad_element))
		{
			return -100;
		}
		
		m_reward_cfg.reward_list.push_back(item);
		rewrad_element = rewrad_element->NextSiblingElement();
	}

	return 0;
}
