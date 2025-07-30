#include "dujieconfig.hpp"
#include "checkresourcecenter.hpp"
#include "npc/npcpool.h"
#include "monster/monster_group_pool.hpp"

DuJieConfig::DuJieConfig()
{
	m_fb_count = 0;
	m_task_shan_hai_boss_hard = -10;
}

DuJieConfig::~DuJieConfig()
{
}

bool DuJieConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("base", InitFBCfg);

	return true;
}

const DuJieFbCfg * DuJieConfig::GetFbCfg(int fb_index) const
{
	if (0 > fb_index
		|| m_fb_count <= fb_index
		|| DU_JIE_MAX_FB_NUM <= fb_index)
	{
		return NULL;
	}
	return &m_fb_cfg_[fb_index];
}

int DuJieConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	if (!GetSubNodeValue(root_element, "level_gap", m_other_cfg.level_gap) || m_other_cfg.level_gap <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "help_times", m_other_cfg.day_assist_times))
	{
		return -2;
	}

	{
		TiXmlElement * reward_list_element = root_element->FirstChildElement("help_reward_list");
		if (NULL == reward_list_element)
		{
			return -201;
		}

		TiXmlElement * node_element = reward_list_element->FirstChildElement("help_reward");
		while (NULL != node_element)
		{
			ItemConfigData temp_item;
			if (!temp_item.ReadConfig(node_element))
			{
				return -202;
			}
			m_other_cfg.assist_reward.push_back(temp_item);
			node_element = node_element->NextSiblingElement();
		}
	}

	return 0;
}

int DuJieConfig::InitFBCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	m_fb_count = 0;
	while (NULL != root_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(root_element, "seq", fb_index)
			|| fb_index != m_fb_count
			|| 0 > fb_index
			|| DU_JIE_MAX_FB_NUM <= fb_index)
		{
			return -1;
		}
		m_fb_count++;
		DuJieFbCfg & fb_info = m_fb_cfg_[fb_index];
		fb_info.fb_index = fb_index;

		if (!GetSubNodeValue(root_element, "pre_require", fb_info.front_task_type)
			|| 0 > fb_info.front_task_type
			|| DU_JIE_TASK_TYPE_MAX <= fb_info.front_task_type)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param1", fb_info.front_task_param_1))
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "param2", fb_info.front_task_param_2))
		{
			return -4;
		}

		switch (fb_info.front_task_type)
		{
		case DU_JIE_TASK_TYPE_MONSTER_GROUP:
			{
				if (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(fb_info.front_task_param_1))
				{
					return -4000 - fb_index;
				}
			}
			break;
		case DU_JIE_TASK_TYPE_SHAN_HAI_BOSS:
			{
				if (-10 == m_task_shan_hai_boss_hard)
				{
					m_task_shan_hai_boss_hard = fb_info.front_task_param_2;
				}
			
				if (m_task_shan_hai_boss_hard != fb_info.front_task_param_2)
				{
					return -13;
				}
			}
			break;
		default:
			break;
		}


		if (!GetSubNodeValue(root_element, "scene_id", fb_info.scene_id))
		{
			return -5;
		}
		CheckResourceCenter::Instance().GetSceneCheck()->Add(fb_info.scene_id, __FUNCTION__);

		if (!GetSubNodeValue(root_element, "born_x", fb_info.role_pos.x))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "born_y", fb_info.role_pos.y))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "x", fb_info.boss_pos.x))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "y", fb_info.boss_pos.y))
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "dir_y", fb_info.boss_dir))
		{
			return -10;
		}

		if (!GetSubNodeValue(root_element, "npc_id", fb_info.boss_npc_id))
		{
			return -11;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(fb_info.boss_npc_id);
		if (NULL == npc_cfg)
		{
			return -12;
		}

		{
			TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");
			if (NULL == reward_list_element)
			{
				return -101;
			}

			TiXmlElement * node_element = reward_list_element->FirstChildElement("reward");
			fb_info.reward_num = 0;
			for (int i = 0; i <DuJieFbCfg::MAX_REWARD_NUM; i++)
			{
				if (NULL == node_element)
				{
					break;
				}
				if (!fb_info.reward_list_[i].ReadConfig(node_element))
				{
					return -103;
				}
				fb_info.reward_num++;
				node_element = node_element->NextSiblingElement();
			}
		}
		
		if (!GetSubNodeValue(root_element, "title", fb_info.reward_title_id))
		{
			return -14;
		}
		CheckResourceCenter::Instance().GetTitleCheck()->Add(fb_info.reward_title_id, __FUNCTION__);


		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}


