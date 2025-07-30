#include "advancefbconfig.hpp"
#include "world.h"
#include "gameworld/task/taskpool.h"
#include "item/itempool.h"

AdvanceFbConfig::AdvanceFbConfig()
{
}

AdvanceFbConfig::~AdvanceFbConfig()
{
	std::vector<int>().swap(m_notice_task_cfg);
}

bool AdvanceFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG
	
	LOAD_CONFIG("career", InitAdvFbInfoCfg);
	LOAD_CONFIG("special_npc", InitAdvFbNpcInfoCfg);
	LOAD_CONFIG("boss_drop", InitAdvFbNpcDropCfg);
	LOAD_CONFIG("other", InitOhterCfg);
	/*LOAD_CONFIG("career_other_2", InitAdvFbOthersType0Cfg)
	LOAD_CONFIG("career_other_4", InitAdvFbOthersType1Cfg)
	LOAD_CONFIG("career_2", InitAdvFbMonsterListCfg)
	LOAD_CONFIG("career_4", InitAdvFbAttrChangesCfg)
	LOAD_CONFIG("career_5", InitAdvFbShadowsAttrCfg)*/
	
	return true;
}

/*
const AdvFbMonsterListCfg * AdvanceFbConfig::GetClearListByTier(int tier)
{
	std::map<int, AdvFbMonsterListCfg>::iterator tier_it = m_monster_clear_map.find(tier);
	if (m_monster_clear_map.end() != tier_it) return & tier_it->second;

	return NULL;
}

const AdvFbAttrChangesCfg * AdvanceFbConfig::GetAttrChangeCfg(int fb_type, int tier)
{
	std::map<int, std::map<int, AdvFbAttrChangesCfg> >::iterator type_it = m_attr_change_map.find(fb_type);
	if (m_attr_change_map.end() == type_it) return NULL;

	std::map<int, AdvFbAttrChangesCfg>::iterator tier_it = type_it->second.find(tier);
	if (type_it->second.end() != tier_it) return & tier_it->second;

	return NULL;
}

const AdvFbBossCon * AdvanceFbConfig::GetACFbBoss(int param)
{
	std::map<int, AdvFbBossCon>::iterator param_it = m_ac_boss_map.find(param);
	if (m_ac_boss_map.end() != param_it) return & param_it->second;

	return NULL;
}*/

const AdvFbInfoCfg * AdvanceFbConfig::GetAdvFbInfoCfg(int adv_idx, int level)
{
	std::map<int, std::map<int, AdvFbInfoCfg> >::iterator it = m_fb_info_map.find(adv_idx);
	if (it != m_fb_info_map.end())
	{
		std::map<int, AdvFbInfoCfg>::iterator it2 = it->second.find(level);
		if (it2 != it->second.end())
		{
			return &it2->second;
		}
	}
	return NULL;
}

const AdvanceNpcOption * AdvanceFbConfig::GetNpcOptionCfg(int npcid, int option_index)
{
	if (option_index < 0 || option_index >= NPCChosedTalkInfoCfg::MAX_OPTION_NUM)return NULL;

	std::map<int,AdvanceNpcOptionList >::iterator it = m_npc_option_cfg.find(npcid);
	if (it != m_npc_option_cfg.end())
	{
		return &it->second.info[option_index];
	}
	return NULL;
}

const AdvanceDropCfg * AdvanceFbConfig::GetDropByNPCidCfg(int npc_id)
{
	std::map<int, AdvanceDropCfg>::iterator it = m_drop_cfg.find(npc_id);
	if (it != m_drop_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

bool AdvanceFbConfig::HasNoticeTaskID(int task_id)
{
	for (std::vector<int>::iterator it = m_notice_task_cfg.begin();it != m_notice_task_cfg.end();it++)
	{
		if (*it == task_id)
		{
			return true;
		}
	}
	return false;
}

int AdvanceFbConfig::InitAdvFbInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int adv = 0;
		if (!GetSubNodeValue(root_element, "adv", adv))
		{
			return -1;
		}
		
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level))
		{
			return -2;
		}

		AdvFbInfoCfg &node_cfg = m_fb_info_map[adv][level];
		node_cfg.adv = adv;
		node_cfg.level = level;
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(node_cfg.scene_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "play_type", node_cfg.play_type) || node_cfg.play_type < ADV_TYPE_DEFEAT || node_cfg.play_type >= ADV_TYPE_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		TiXmlElement * item_id_element = root_element->FirstChildElement("item_count");
		if (NULL != item_id_element && item_id_element->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "item_count", node_cfg.item_num) || node_cfg.item_num <= 0)
			{
				return -6;
			}
		}

		TiXmlElement * task_id_element = root_element->FirstChildElement("task_id");
		if (NULL != task_id_element && task_id_element->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "task_id", node_cfg.task_id) || node_cfg.task_id < 0)
			{
				return -7;
			}
		}

		TiXmlElement * drop_monster_list_element = root_element->FirstChildElement("drop_monster");
		if (NULL != drop_monster_list_element)		// 允许为空
		{
			TiXmlElement * node_element = drop_monster_list_element->FirstChildElement("node");
			if (NULL != node_element && node_element->FirstChild())
			{
				if (!GetSubNodeValue(root_element, "drop_num", node_cfg.drop_num) || node_cfg.drop_num <= 0 || node_cfg.drop_num < node_cfg.item_num)
				{
					return -9;
				}
				while (NULL != node_element)
				{
					int monster_id = 0;
					if (!GetNodeValue(node_element, monster_id))
					{
						return -8;
					}

					node_cfg.monster_drop_list.push_back(monster_id);
					node_element = node_element->NextSiblingElement();
				}
			}
		}

		TiXmlElement * npc_id_list0 = root_element->FirstChildElement("transport_id");
		if (NULL != npc_id_list0 && npc_id_list0->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "transport_id", node_cfg.transport_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(node_cfg.transport_id))
			{
				return -10;
			}
		}

		if (!GetSubNodeValue(root_element, "pos_x", node_cfg.transport_pos.x) || node_cfg.transport_pos.x < 0)
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "pos_y", node_cfg.transport_pos.y) || node_cfg.transport_pos.y < 0)
		{
			return -12;
		}

		if (!GetSubNodeValue(root_element, "pos_x1", node_cfg.enter_pos.x) || node_cfg.enter_pos.x < 0)
		{
			return -13;
		}

		if (!GetSubNodeValue(root_element, "pos_y1", node_cfg.enter_pos.y) || node_cfg.enter_pos.y < 0)
		{
			return -14;
		}

		TiXmlElement * npc_id_list1 = root_element->FirstChildElement("boss1_npc_id");
		if (NULL != npc_id_list1 && npc_id_list1->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "boss1_npc_id", node_cfg.boss_npc_id) || NULL == NPCPOOL->GetServerNPCInfo(node_cfg.boss_npc_id))
			{
				return -15;
			}

			if (!GetSubNodeValue(root_element, "boss_x1", node_cfg.boss_pos.x) || node_cfg.boss_pos.x < 0)
			{
				return -16;
			}

			if (!GetSubNodeValue(root_element, "boss_y1", node_cfg.boss_pos.y) || node_cfg.boss_pos.y < 0)
			{
				return -17;
			}

			if (!GetSubNodeValue(root_element, "dir_y1", node_cfg.dir_y1))
			{
				return -18;
			}
		}

		TiXmlElement * npc_id_list2 = root_element->FirstChildElement("boss2_npc_id");
		if (NULL != npc_id_list2 && npc_id_list2->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "boss2_npc_id", node_cfg.boss_npc_id2) || NULL == NPCPOOL->GetServerNPCInfo(node_cfg.boss_npc_id2))
			{
				return -19;
			}

			if (!GetSubNodeValue(root_element, "boss_x2", node_cfg.boss_pos2.x) || node_cfg.boss_pos2.x < 0)
			{
				return -20;
			}

			if (!GetSubNodeValue(root_element, "boss_y2", node_cfg.boss_pos2.y) || node_cfg.boss_pos2.y < 0)
			{
				return -21;
			}

			if (!GetSubNodeValue(root_element, "dir_y2", node_cfg.dir_y2))
			{
				return -22;
			}
		}

		TiXmlElement * npc_element = root_element->FirstChildElement("npc_id");
		if (NULL != npc_element && npc_element->FirstChild())		// 允许为空
		{
			if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(node_cfg.npc_id))
			{
				return -23;
			}

			if (!GetSubNodeValue(root_element, "npc_x", node_cfg.npc_pos.x) || node_cfg.npc_pos.x < 0)
			{
				return -24;
			}

			if (!GetSubNodeValue(root_element, "npc_y", node_cfg.npc_pos.y) || node_cfg.npc_pos.y < 0)
			{
				return -25;
			}

			if (!GetSubNodeValue(root_element, "npc_dir", node_cfg.npc_dir))
			{
				return -26;
			}

			if (!GetSubNodeValue(root_element, "recover_num", node_cfg.recover_num))
			{
				return -27;
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int AdvanceFbConfig::InitAdvFbNpcInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		AdvanceNpcOption node_cfg;

		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id) || NPCPOOL->GetServerNPCInfo(node_cfg.npc_id) == NULL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "operation_id", node_cfg.operation_id) || node_cfg.operation_id <= 0 || node_cfg.operation_id > NPCChosedTalkInfoCfg::MAX_OPTION_NUM)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "is_delete", node_cfg.is_delete))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "operation_type", node_cfg.operation_type))
		{
			return -4;
		}

		TiXmlElement * task_id_element = root_element->FirstChildElement("task_id");
		if (NULL == task_id_element) return -44;
		
		TiXmlElement * node_element = task_id_element->FirstChildElement("node");
		while (node_element != NULL)
		{
			int task_id = 0;
			if (GetNodeValue(node_element, task_id))
			{
				if (NULL == TASKPOOL->GetTaskCfgByID(task_id))
				{
					return -5;
				}			

				node_cfg.task_id_list.push_back(task_id);
			}
			
			node_element = node_element->NextSiblingElement();
		}

		TiXmlElement * item_id_element = root_element->FirstChildElement("online_reward");
		if (NULL != item_id_element && item_id_element->FirstChild())		// 允许为空
		{
			int item_id = 0;
			if (GetSubNodeValue(item_id_element, "item_id", item_id) && item_id != 0)
			{
				node_cfg.item.ReadConfig(item_id_element);
			}
		}
		m_npc_option_cfg[node_cfg.npc_id].info[node_cfg.operation_id - 1] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int AdvanceFbConfig::InitAdvFbNpcDropCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int npc_id;

		if (!GetSubNodeValue(root_element, "npc_id", npc_id) || NPCPOOL->GetServerNPCInfo(npc_id) == NULL)
		{
			return -1;
		}
		AdvanceDropCfg & node_cfg = m_drop_cfg[npc_id];
		node_cfg.npc_id = npc_id;

		TiXmlElement * task_id_element = root_element->FirstChildElement("task_id");
		if (NULL == task_id_element) return -55;

		TiXmlElement * node_element = task_id_element->FirstChildElement("node");
		while (node_element != NULL)
		{
			int task_id = 0;
			if (GetNodeValue(node_element, task_id))
			{
				if (NULL == TASKPOOL->GetTaskCfgByID(task_id))
				{
					return -5;
				}

				node_cfg.task_id_list.push_back(task_id);
			}

			node_element = node_element->NextSiblingElement();
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("online_reward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_id_element = item_list_element->FirstChildElement("online_reward");
			while (item_id_element != NULL)
			{
				ItemConfigData node_item;

				if (!node_item.ReadConfig(item_id_element))
				{
					return -4;
				}
				node_cfg.item_vec.push_back(node_item);
				item_id_element = item_id_element->NextSiblingElement();
			}
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int AdvanceFbConfig::InitOhterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int task_id;

		if (!GetSubNodeValue(root_element, "task_id", task_id) || TASKPOOL->GetTaskCfgByID(task_id) == NULL)
		{
			return -1;
		}
		
		m_notice_task_cfg.push_back(task_id);

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

/*
int AdvanceFbConfig::InitAdvFbOthersType0Cfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int min_num = 0;
	if (!GetSubNodeValue(root_element, "min_group", min_num) || min_num < 0 || min_num > MAX_TEAM_MEMBER_NUM)
	{
		return -1;
	}

	m_adv_fb_other.min_team_num[ADV_TYPE_DEFEAT] = min_num;
	return 0;
}

int AdvanceFbConfig::InitAdvFbOthersType1Cfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int min_num = 0;
	if (!GetSubNodeValue(root_element, "min_group", min_num) || min_num < 0 || min_num > MAX_TEAM_MEMBER_NUM)
	{
		return -1;
	}

	m_adv_fb_other.min_team_num[ADV_TYPE_DEFEAT] = min_num;

	// 保存 BOSS 配置；
	char * boss_reader = new char[10];
	for (int boss_idx = 0; boss_idx < ADV_FB_ATTR_CHANGE_FB_BOSS_NUM; ++boss_idx)
	{
		AdvFbBossCon tmp_con;
		sprintf(boss_reader, "npc_id_%d", boss_idx + 1);
		if (!GetSubNodeValue(root_element, boss_reader, tmp_con.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(tmp_con.npc_id))
		{
			return -(10 + boss_idx);
		}

		sprintf(boss_reader, "x_%d", boss_idx + 1);
		if (!GetSubNodeValue(root_element, boss_reader, tmp_con.posi.x))
		{
			return -(20 + boss_idx);
		}

		sprintf(boss_reader, "y_%d", boss_idx + 1);
		if (!GetSubNodeValue(root_element, boss_reader, tmp_con.posi.y))
		{
			return -(30 + boss_idx);
		}

		m_ac_boss_map[boss_idx + 1] = tmp_con;
	}

	return 0;
}

int AdvanceFbConfig::InitAdvFbMonsterListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	std::set<int> scene_checker;		// 场景ID 不允许相同
	while (NULL != root_element)
	{
		int tier = 0; AdvFbMonsterListCfg tmp_con;
		if (!GetSubNodeValue(root_element, "level", tier))
		{
			return -1;
		}

		if (!m_monster_clear_map.empty() && m_monster_clear_map.rbegin()->first + 1 != tier)	// 必须连续
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "scene_id", tmp_con.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(tmp_con.scene_id))
		{
			return -3;
		}

		if (scene_checker.end() != scene_checker.find(tmp_con.scene_id))
		{
			return -10;
		}

		TiXmlElement * npc_id_list = root_element->FirstChildElement("npc_id_1");
		if (NULL != npc_id_list)		// 允许为空
		{
			TiXmlElement * item_node = npc_id_list->FirstChildElement("node");
			if (NULL == item_node) return -100;

			while (NULL != item_node)
			{
				int npc_id = 0;
				if (!GetNodeValue(item_node, npc_id) || NULL == NPCPOOL->GetServerNPCInfo(npc_id))
				{
					return -4;
				}

				tmp_con.monster_list.push_back(npc_id);
				item_node = item_node->NextSiblingElement();
			}
		}

		scene_checker.insert(tmp_con.scene_id);
		m_monster_clear_map[tier] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int AdvanceFbConfig::InitAdvFbAttrChangesCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int type = 0, tier = 0; AdvFbAttrChangesCfg tmp_con;
		if (!GetSubNodeValue(root_element, "scene_type", type))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level", tier))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "scene_id", tmp_con.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(tmp_con.scene_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "add_att_type", tmp_con.attr_type) || tmp_con.attr_type < 0 || tmp_con.attr_type >= BATTLE_ATTR_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "att_num_percent", tmp_con.attr_change_rate))	// 可以是负数，负数表示属性削减；
		{
			return -5;
		}

		m_attr_change_map[type][tier] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	// 检查层数是否连续
	for (std::map<int, std::map<int, AdvFbAttrChangesCfg> >::iterator type_it = m_attr_change_map.begin(); type_it != m_attr_change_map.end(); ++type_it)
	{
		int tmp_tier = 0;
		for (std::map<int, AdvFbAttrChangesCfg>::iterator tier_it = type_it->second.begin(); tier_it != type_it->second.end(); ++tier_it)
		{
			if (tier_it->first != ++tmp_tier) return -500;
		}
	}

	return 0;
}

int AdvanceFbConfig::InitAdvFbCloneMonsteCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		AdvFbBossCon tmp_con;
		if (!GetSubNodeValue(root_element, "npc_id", tmp_con.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(tmp_con.npc_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "x", tmp_con.posi.x))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "y", tmp_con.posi.y))
		{
			return -3;
		}

		m_monster_clone_list.push_back(tmp_con);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int AdvanceFbConfig::InitAdvFbShadowsAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		AdvFbBossCon & tmp_con = m_clone_info.boss_con;
		if (!GetSubNodeValue(root_element, "scene_id", m_clone_info.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(m_clone_info.scene_id))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "att_num_per", m_clone_info.clone_attr_change_rate))			// 可以是负数，负数表示属性削减；
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "npc_id", tmp_con.npc_id) || NULL == NPCPOOL->GetServerNPCInfo(tmp_con.npc_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "x", tmp_con.posi.x))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "y", tmp_con.posi.y))
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}*/