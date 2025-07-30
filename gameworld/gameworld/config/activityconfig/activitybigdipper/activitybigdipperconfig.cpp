#include "activitybigdipperconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "engineadapter.h"

ActivityBigDipperConfig::ActivityBigDipperConfig()
{
}

ActivityBigDipperConfig::~ActivityBigDipperConfig()
{
	std::vector<BigDipperCfg*>().swap(rand_monst_cfg);
	std::vector<BigDipperCfg*>().swap(rand_boss_cfg);
	std::vector<BigDipperCfg>().swap(m_monster_vec);
}

bool ActivityBigDipperConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("star_man_position", InitBaseCfg);
	LOAD_CONFIG("other", InitOhterCfg);

	return true;
}

const BigDipperCfg * ActivityBigDipperConfig::GetBaseCfg(int sn)
{
	std::vector<BigDipperCfg>::iterator it = m_monster_vec.begin();
	for (; it != m_monster_vec.end(); it++)
	{
		if (it->sn == sn)
		{
			return &(*it);
		}
	}
	return NULL;
}

void ActivityBigDipperConfig::GetRandMonsterCfg(int monster_num, std::vector<BigDipperCfg>* rand_list)
{
	if (monster_num <= 0)return;
	if (rand_list == NULL)return;
	if (monster_num > (short)rand_monst_cfg.size())return;


	std::vector<BigDipperCfg *> new_vec;
	for (std::vector<BigDipperCfg *>::iterator it = rand_monst_cfg.begin(); it != rand_monst_cfg.end(); it++)
	{
		new_vec.push_back(*it);
	}

	for (int idx = 0; idx < monster_num && new_vec.size() > 0; idx++)
	{
		if (new_vec.size() <= 0)break;
		int rand_idx = rand() % new_vec.size();
		rand_list->push_back(*new_vec[rand_idx]);
		new_vec.erase(new_vec.begin() + rand_idx);
	}

	for (std::vector<BigDipperCfg *>::iterator it = rand_boss_cfg.begin(); it != rand_boss_cfg.end(); it++)
	{
		rand_list->push_back(*(*it));
	}
}

int ActivityBigDipperConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		BigDipperCfg node_cfg;

		if (!GetSubNodeValue(dataElement, "sn", node_cfg.sn) || node_cfg.sn < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "npc_seq", node_cfg.npc_seq) || node_cfg.npc_seq < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "monster_guoup", node_cfg.monster_guoup) || node_cfg.monster_guoup < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "type", node_cfg.type) || node_cfg.type < 0)
		{
			return -4;
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
					return -5;
				}
				node_cfg.open_time_list.push_back(open_time);
				node_Element = node_Element->NextSiblingElement();
			}
		}

		TiXmlElement* first_reward_Element = dataElement->FirstChildElement("item_list_first_list");
		if (NULL != first_reward_Element)
		{
			TiXmlElement* item_Element = first_reward_Element->FirstChildElement("item_list_first");
			while (item_Element != NULL)
			{
				ItemConfigData reward_cfg;
				if (!reward_cfg.ReadConfig(item_Element))
				{
					return -6;
				}
				node_cfg.first_reward.push_back(reward_cfg);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		char path[30];
		for (int i = 0; i < MAX_BIG_DIPPER_MONSTER_REWARD_INDEX; i++)
		{
			memset(path, 0, sizeof(path));
			sprintf(path, "item_list_%d_list", i + 1);
			TiXmlElement* item_list_Element = dataElement->FirstChildElement(path);
			if (NULL != item_list_Element)
			{
				memset(path, 0, sizeof(path));
				sprintf(path, "item_list_%d", i + 1);
				bool has_stage = false;
				TiXmlElement* item_Element = item_list_Element->FirstChildElement(path);
				while (item_Element != NULL)
				{
					ItemConfigData reward_cfg;
					if (!reward_cfg.ReadConfig(item_Element))
					{
						return -7;
					}
					node_cfg.reward_list[i].push_back(reward_cfg);
					item_Element = item_Element->NextSiblingElement();
					has_stage = true;
				}
				if(has_stage)node_cfg.monster_stage_reward_num++;
			}
		}

		if (!GetSubNodeValue(dataElement, "help_times", node_cfg.help_times) || node_cfg.help_times < 0)
		{
			return -8;
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

		m_monster_vec.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<BigDipperCfg>::iterator it = m_monster_vec.begin(); it != m_monster_vec.end(); it++)
	{
		if (it->type == BigDipperCfg::MONSTER_TYPE_0)
		{
			rand_monst_cfg.push_back(&(*it));
		}
		else if (it->type == BigDipperCfg::MONSTER_TYPE_1)
		{
			rand_boss_cfg.push_back(&(*it));
		}
	}

	return 0;
}

int ActivityBigDipperConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "open_num", other_cfg.open_num) || other_cfg.open_num <= 0)
	{
		return -1;
	}

	return 0;
}
