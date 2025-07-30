#include "fallengodconfig.hpp"
#include "config/logicconfigmanager.hpp"

FallenGodConfig::FallenGodConfig()
{
	
}

FallenGodConfig::~FallenGodConfig()
{
	std::vector<FallenGodBossCfg>().swap(m_boss_vec);
}

bool FallenGodConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOhterCfg);
	LOAD_CONFIG("battle_list", InitBaseCfg);

	return true;
}

const FallenGodBossCfg * FallenGodConfig::GetBaseCfg(int sn)
{
	for (std::vector<FallenGodBossCfg>::iterator it = m_boss_vec.begin(); it != m_boss_vec.end(); it++)
	{
		if (it->sn == sn)return &(*it);
	}
	return NULL;
}

int FallenGodConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		FallenGodBossCfg node_cfg;

		if (!GetSubNodeValue(dataElement, "sn", node_cfg.sn) || node_cfg.sn < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "monster_guoup", node_cfg.monster_guoup) || node_cfg.monster_guoup < 0)
		{
			return -2;
		}

		TiXmlElement* open_time_list_Element = dataElement->FirstChildElement("open_time");
		if (NULL != open_time_list_Element)
		{
			TiXmlElement* node_Element = open_time_list_Element->FirstChildElement("node");
			while (NULL != node_Element)
			{
				short open_time = -1;
				if (!GetNodeValue(node_Element, open_time) || open_time < 0 || open_time >= 7)
				{
					return -3;
				}
				node_cfg.open_time_list.push_back(open_time);
				node_Element = node_Element->NextSiblingElement();
			}
		}

		TiXmlElement* first_reward_Element = dataElement->FirstChildElement("item_list_1_list");
		if (NULL != first_reward_Element)
		{
			TiXmlElement* item_Element = first_reward_Element->FirstChildElement("item_list_1");
			while (item_Element != NULL)
			{
				ItemConfigData reward_cfg;
				if (!reward_cfg.ReadConfig(item_Element))
				{
					return -4;
				}
				node_cfg.reward_list.push_back(reward_cfg);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(dataElement, "help_times", node_cfg.help_times) || node_cfg.help_times < 0)
		{
			return -5;
		}

		TiXmlElement* help_reward_Element = dataElement->FirstChildElement("help_reward_list");
		if (NULL != help_reward_Element)
		{
			TiXmlElement* item_Element = help_reward_Element->FirstChildElement("help_reward");
			while (item_Element != NULL)
			{
				ItemConfigData reward_cfg;
				if (!reward_cfg.ReadConfig(item_Element))
				{
					return -6;
				}
				node_cfg.help_reward.push_back(reward_cfg);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(dataElement, "pass_sn", node_cfg.pass_sn) || node_cfg.pass_sn < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "pass_type", node_cfg.pass_type) || node_cfg.pass_type < FallenGodBossCfg::PASS_TYPE_0 || node_cfg.pass_type > FallenGodBossCfg::PASS_TYPE_1)
		{
			return -3;
		}

		m_boss_vec.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int FallenGodConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "challenge_num", m_other_cfg.challenge_num) || m_other_cfg.challenge_num < 0)return -1;
	if (!GetSubNodeValue(dataElement, "title_id", m_other_cfg.title_id))return -2;
	if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id))return -3;
	if (!GetSubNodeValue(dataElement, "reward_rank", m_other_cfg.reward_rank))return -4;
	if (!GetSubNodeValue(dataElement, "last_times", m_other_cfg.last_times))return -5;
	dataElement = dataElement->NextSiblingElement();

	return 0;
}
