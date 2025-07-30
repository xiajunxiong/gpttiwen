#include "serverfirstkillconfig.hpp"
#include "gameworld/gameworld/monster/monster_group_pool.hpp"
#include "servercommon/struct/global/serverfirstkilldef.h"

ServerFirstKillConfig::ServerFirstKillConfig()
{
}

ServerFirstKillConfig::~ServerFirstKillConfig()
{
}

bool ServerFirstKillConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("dexter", InitServerFirstKillCfg);

	return true;
}

const ServerFirstKillCfg * ServerFirstKillConfig::GetServerFirstKillCfg(int monster_group_id)
{
	std::map<int, ServerFirstKillCfg>::const_iterator it = m_server_first_kill.find(monster_group_id);
	if(it != m_server_first_kill.end()) return &it->second;

	return NULL;
}



int ServerFirstKillConfig::InitServerFirstKillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -111;
	}

	while (NULL != root_element)
	{
		ServerFirstKillCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq <= 0 || cfg.seq > TYPE_LEVEL_GROUP_SEQ_MAX_NUM)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "type", cfg.type) || cfg.type <= 0 || cfg.type > FIRST_KILL_TYPE_MAX)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "type_seq", cfg.type_seq) || cfg.type_seq < 0 || cfg.type_seq >= FIRST_KILL_BOSS_TYPE_MAX_NUM)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "level_group", cfg.level_group) || cfg.level_group <= 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "monster_group", cfg.monster_group_id) || NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.monster_group_id))
		{
			return -5;
		}

		if (m_server_first_kill.end() != m_server_first_kill.find(cfg.monster_group_id))
		{
			return -6;
		}
		TiXmlElement * list_node = root_element->FirstChildElement("online_reward_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("online_reward");
			if (NULL != item_node)
			{
				if (!cfg.reward_item.ReadConfig(item_node)) return -100;

				item_node = item_node->NextSiblingElement();
			}
		}
		if (!GetSubNodeValue(root_element, "coin", cfg.coin) || cfg.coin <= 0)
		{
			return -7;
		}

		m_server_first_kill[cfg.monster_group_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}


	return 0;
}
