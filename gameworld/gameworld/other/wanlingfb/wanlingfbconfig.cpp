#include "wanlingfbconfig.hpp"
#include "checkresourcecenter.hpp"
#include "npc/npcpool.h"

#include "monster/monster_group_pool.hpp"

WanLingFBConfig::WanLingFBConfig()
{
}

WanLingFBConfig::~WanLingFBConfig()
{
	std::vector<WanLingFB::FB_Cfg>().swap(m_fb_cfg_vec);
}

bool WanLingFBConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("drop_group", InitDropGroupCfg);
	LOAD_CONFIG("fb_info", InitFBCfg);
	LOAD_CONFIG("task", InitTaskCfg);
	LOAD_CONFIG("npc", InitCreatNpcCfg);
	LOAD_CONFIG("reward", InitRewardCfg);

	return true;
}

const WanLingFB::FB_Cfg * WanLingFBConfig::GetFBCfg(const int fb_index) const
{
	if (0 > fb_index
		|| (int)m_fb_cfg_vec.size() <= fb_index)
	{
		return NULL;
	}
	return &m_fb_cfg_vec[fb_index];
}

const WanLingFB::FB_Cfg * WanLingFBConfig::GetFBByLevelCfg(int role_level) const
{
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL) return NULL;

	for (size_t i = m_fb_cfg_vec.size() -1 ; i >= 0; i--)
	{
		if (role_level >= m_fb_cfg_vec[i].open_level)
		{
			return &m_fb_cfg_vec[i];
		}
	}

	return NULL;
}

const WanLingFB::TaskCfg * WanLingFBConfig::GetTaskCfg(const int fb_index, const int task_seq) const
{
	if (0 > fb_index
		|| (int)m_fb_cfg_vec.size() <= fb_index)
	{
		return NULL;
	}
	const WanLingFB::FB_Cfg & fb_cfg = m_fb_cfg_vec[fb_index];
	if (0 >= task_seq
		|| (int)fb_cfg.task_vec.size() < task_seq)
	{
		return NULL;
	}

	return &fb_cfg.task_vec[task_seq - 1];
}

const ItemConfigData * WanLingFBConfig::GetDropItem(const int drop_group_id, const int prof, int &is_broadcast) const
{
	is_broadcast = 0;
	WanLingFB::DropGroupIDMap::const_iterator group_cit = m_drop_group_id_map.find(drop_group_id);
	if (m_drop_group_id_map.end() == group_cit)
	{
		return NULL;
	}

	const WanLingFB::DropGroupCfg & rand_pool = group_cit->second;


	int rand_num = RandomNum(rand_pool.total_power);
	WanLingFB::DropPool::const_iterator item_cit
		= rand_pool.drop_pool.upper_bound(rand_num);
	if (rand_pool.drop_pool.begin() == item_cit)
	{
		return NULL;
	}
	item_cit--;
	int prof_index = prof / PROFESSION_BASE;
	if (0 >= prof_index
		|| (int)item_cit->second.base_prof_drop_.size() <= prof_index)
	{
		return NULL;
	}
	is_broadcast = item_cit->second.is_broadcast;
	return &item_cit->second.base_prof_drop_[prof_index];
}

int WanLingFBConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "fight_reward_times", m_other_cfg.day_battle_reward_time)
		|| 0 > m_other_cfg.day_battle_reward_time)
	{
		return -1;
	}
	if (!GetSubNodeValue(root_element, "reward_times", m_other_cfg.day_pass_reward_time)
		|| 0 > m_other_cfg.day_pass_reward_time)
	{
		return -2;
	}

	return 0;
}

int WanLingFBConfig::InitDropGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int drop_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_id", drop_group_id)
			|| 0 >= drop_group_id)
		{
			return -1;
		}

		int power = 0;
		if (!GetSubNodeValue(data_element, "rate", power)
			|| 0 >= power)
		{
			return -2;
		}

		WanLingFB::DropGroupCfg &drop_group_cfg = m_drop_group_id_map[drop_group_id];
		drop_group_cfg.group_id = drop_group_id;
		WanLingFB::DropCfg &drop_cfg = drop_group_cfg.drop_pool[drop_group_cfg.total_power];
		drop_group_cfg.total_power += power;

		;
		if (!GetSubNodeValue(data_element, "is_broadcast", drop_cfg.is_broadcast)
			|| 0 > drop_cfg.is_broadcast
			|| 1 < drop_cfg.is_broadcast)
		{
			return -2;
		}

		drop_cfg.base_prof_drop_.resize(PROFESSION_TYPE_NUM);

		for (int i = 1; i < PROFESSION_TYPE_NUM; i++)
		{
			std::string reward_str = STRING_SPRINTF("reward_item%d", i);
			
			TiXmlElement * item_element = data_element->FirstChildElement(reward_str.c_str());
			if (NULL != item_element)
			{
				if (!drop_cfg.base_prof_drop_[i].ReadConfig(item_element))
				{
					return -10000 - i - drop_group_id * 100;
				}
			}
		}
		
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int WanLingFBConfig::InitFBCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(data_element, "fb_index", fb_index)
			|| (int)m_fb_cfg_vec.size() != fb_index)
		{
			return -1;
		}

		m_fb_cfg_vec.push_back(WanLingFB::FB_Cfg());
		WanLingFB::FB_Cfg &fb_cfg = m_fb_cfg_vec[fb_index];
		fb_cfg.fb_index = fb_index;
		
		if (!GetSubNodeValue(data_element, "open_level", fb_cfg.open_level))
		{
			return -2;
		}


		if (!GetSubNodeValue(data_element, "scene_id", fb_cfg.scene_id))
		{
			return -3;
		}
		CheckResourceCenter::Instance().GetSceneCheck()->Add(fb_cfg.scene_id, __FUNCTION__);

		if (!GetSubNodeValue(data_element, "pos_x", fb_cfg.role_pos.x))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "pos_y", fb_cfg.role_pos.y))
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "door_x", fb_cfg.leave_pos.x))
		{
			return -6;
		}
		if (!GetSubNodeValue(data_element, "door_y", fb_cfg.leave_pos.y))
		{
			return -7;
		}
		
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int WanLingFBConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(data_element, "fb_index", fb_index)
			|| 0 > fb_index
			|| (int)m_fb_cfg_vec.size() <= fb_index)
		{
			return -1;
		}

		WanLingFB::FB_Cfg &fb_cfg = m_fb_cfg_vec[fb_index];
		int task_seq = 0;
		if (!GetSubNodeValue(data_element, "task_num", task_seq)
			|| (int)fb_cfg.task_vec.size() + 1 != task_seq)
		{
			return -2;
		}
		fb_cfg.task_vec.push_back(WanLingFB::TaskCfg());
		WanLingFB::TaskCfg &task_cfg = fb_cfg.task_vec[task_seq - 1];
		task_cfg.task_seq = task_seq;

		if (!GetSubNodeValue(data_element, "task_type", task_cfg.task_type)
			|| 0 >= task_cfg.task_type
			|| WanLingFB::TASK_TYPE_MAX <= task_cfg.task_type)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "npc_id", task_cfg.target_npc_id))
		{
			return -4;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(task_cfg.target_npc_id);
		if (NULL == npc_cfg)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "npc_action", task_cfg.npc_action))
		{
			return -6;
		}

		UNSTD_STATIC_CHECK(3 == WanLingFB::TASK_TYPE_MAX);
		switch (task_cfg.task_type)
		{
		case WanLingFB::TASK_TYPE_BATTLE:
			{
				if (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(task_cfg.npc_action))
				{
					return -101;
				}

				fb_cfg.final_fight_task_seq = task_cfg.task_seq;
			}
			break;
		case WanLingFB::TASK_TYPE_TALK:
			{

			}
			break;
		default:
			return -100;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int WanLingFBConfig::InitCreatNpcCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(data_element, "fb_id", fb_index)
			|| 0 > fb_index
			|| (int)m_fb_cfg_vec.size() <= fb_index)
		{
			return -1;
		}

		WanLingFB::CreatNPCCfg temp_npc_cfg;
		if (!GetSubNodeValue(data_element, "npc_id", temp_npc_cfg.npc_id))
		{
			return -2;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(temp_npc_cfg.npc_id);
		if (NULL == npc_cfg)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "npc_x", temp_npc_cfg.npc_pos.x))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "npc_y", temp_npc_cfg.npc_pos.y))
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "npc_dir", temp_npc_cfg.npc_dir))
		{
			return -6;
		}

		WanLingFB::FB_Cfg &fb_cfg = m_fb_cfg_vec[fb_index];
		fb_cfg.creat_npc_vec.push_back(temp_npc_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int WanLingFBConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(data_element, "fb_id", fb_index)
			|| 0 > fb_index
			|| (int)m_fb_cfg_vec.size() <= fb_index)
		{
			return -1;
		}
		WanLingFB::FB_Cfg &fb_cfg = m_fb_cfg_vec[fb_index];


		if (!GetSubNodeValue(data_element, "coin_reward", fb_cfg.battle_pass_coin_bind)
			||0 > fb_cfg.battle_pass_coin_bind)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "exp_reward", fb_cfg.pass_exp)
			|| 0 > fb_cfg.pass_exp)
		{
			return -3;
		}

		{
			TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == item_list_element)
			{
				return -101;
			}
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");

			while (NULL != item_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(item_element))
				{
					return -102;
				}

				fb_cfg.pass_reward_vec.push_back(temp_item);
				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement * reward_group_list = data_element->FirstChildElement("drop_group");
			if (NULL == reward_group_list)
			{
				return -500;
			}
			TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
			while (node_element != NULL)
			{
				int reward_group_id = 0;
				if (!GetNodeValue(node_element, reward_group_id)
					|| m_drop_group_id_map.end() == m_drop_group_id_map.find(reward_group_id))
				{
					return -501;
				}

				fb_cfg.pass_drop_grop_id_vec.push_back(reward_group_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

