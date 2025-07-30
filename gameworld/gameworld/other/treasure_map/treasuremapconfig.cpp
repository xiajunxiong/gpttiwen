#include "treasuremapconfig.h"
#include "servercommon/commondata.hpp"
#include "gamelog.h"
#include <set>

TreasureMapConfig::TreasureMapConfig()
{
}

TreasureMapConfig::~TreasureMapConfig()
{
	for (std::map<int, std::vector<TreasureMapBackItemsCon> >::iterator it = m_tmt_back_item_map.begin(); it != m_tmt_back_item_map.end(); it++)
	{
		std::vector<TreasureMapBackItemsCon>().swap(it->second);
	}

	std::vector<TreasureMapObtainCfg>().swap(m_map_obtain_cfg);
}

bool TreasureMapConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("treasure_map_config", InitTaskCfg);
	LOAD_CONFIG("coordinate_group", InitCoordinateGroupCfg);			//需要放在场景组读取前面
	LOAD_CONFIG("scene_group", InitSceneCfg);		
	LOAD_CONFIG("monster_group", InitMonsterCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("content", InitItemCfg);
	LOAD_CONFIG("item_group", InitRewardGroupCfg);
	LOAD_CONFIG("reward_group", InitBackItemsCfg);
	LOAD_CONFIG("reward_find", InitAwardBackCfg);
	LOAD_CONFIG("treasure_map_get", InitMapObtainCfg);
	
	return true;
}

const TreasureMapTaskCfg * TreasureMapConfig::GetTreasureMapTaskCfg(int task_id)
{
	std::map<int, TreasureMapTaskCfg>::iterator iter = m_task_cfg.find(task_id);
	if (iter == m_task_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const TreasureMapSceneCfg * TreasureMapConfig::GetTreasureMapSceneCfg(int scene_group_id, int seq)
{
	std::map<int, TreasureMapSceneCfg>::iterator iter = m_rand_scene_cfg.find(scene_group_id);
	if (iter == m_rand_scene_cfg.end())
	{
		return NULL;
	}

	if (seq < 0 || seq >= (int)iter->second.group_item_cfg.size())
	{
		return NULL;
	}

	return &iter->second;
}

const TreasureMapMonsterCfg * TreasureMapConfig::GetTreasureMapMonsterCfg(int monster_group_id, int seq)
{
	std::map<int, TreasureMapMonsterCfg>::iterator iter = m_rand_monster_cfg.find(monster_group_id);
	if (iter == m_rand_monster_cfg.end())
	{
		return NULL;
	}

	if (seq < 0 || seq >= (int)iter->second.group_item_cfg.size())
	{
		return NULL;
	}

	return &iter->second;
}

const TreasureMapOtherCfg & TreasureMapConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const TreasureMapItemCfg::ItemCfg * TreasureMapConfig::GetRandItemCfg(int seq, int role_level)
{
	std::map<int, TreasureMapItemCfg>::iterator iter = m_item_cfg.find(seq);
	if (iter != m_item_cfg.end())
	{
		for (int role_level_section_index = 0; role_level_section_index < (int)iter->second.role_level_item_cfg.size(); ++role_level_section_index)
		{
			const TreasureMapItemCfg::RoleLevelItemCfg & role_level_item_cfg = iter->second.role_level_item_cfg[role_level_section_index];

			if (role_level_item_cfg.min_role_level <= role_level && role_level <= role_level_item_cfg.max_role_level)
			{
				const std::vector<TreasureMapItemCfg::ItemCfg> & rand_vector_item_cfg = role_level_item_cfg.group_item_cfg;

				int r = rand() % TreasureMapItemCfg::MAX_RATE_COUNT;

				for (int i = 0; i < (int)rand_vector_item_cfg.size(); ++i)
				{
					if (r < rand_vector_item_cfg[i].rate)
					{
						return  &rand_vector_item_cfg[i];
					}

					r -= rand_vector_item_cfg[i].rate;
				}
				 
				break;
			}
		}
	}

	return NULL;
}

const ItemConfigData * TreasureMapConfig::GetRandReward(int reward_grounp_id)
{
	std::map<int, TreasureMapRewardGroupCfg>::iterator iter = m_reward_group_cfg.find(reward_grounp_id);
	if (iter != m_reward_group_cfg.end())
	{
		int r = rand() % TreasureMapRewardGroupCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			if (r < iter->second.group_item_cfg[i].rate)
			{
				return  &iter->second.group_item_cfg[i].item;
			}

			r -= iter->second.group_item_cfg[i].rate;
		}
	}

	return NULL;
}

const TreasureMapAwardBackCon * TreasureMapConfig::GetRewardBack(int role_level, int back_type)
{
	for (std::map<ParamSection, std::map<int, TreasureMapAwardBackCon> >::iterator level_it = m_tmt_find_back_map.begin(); level_it != m_tmt_find_back_map.end(); ++level_it)
	{
		if (role_level < level_it->first.section_min || role_level > level_it->first.section_max) continue;

		std::map<int, TreasureMapAwardBackCon>::iterator type_it = level_it->second.find(back_type);
		if (level_it->second.end() != type_it) return & type_it->second;
	}

	return NULL;
}

const TreasureMapAwardBackCon * TreasureMapConfig::GetFindType(int _role_level)
{
	for (std::map<ParamSection, std::map<int, TreasureMapAwardBackCon> >::iterator level_it = m_tmt_find_back_map.begin(); level_it != m_tmt_find_back_map.end(); ++level_it)
	{
		if (_role_level < level_it->first.section_min || _role_level > level_it->first.section_max) continue;

		std::map<ParamSection, TreasureMapAwardBackCon>::iterator ito = m_ps_to_tmb.find(level_it->first);
		if (ito != m_ps_to_tmb.end()) return &ito->second;
	}

	return NULL;
}

const ItemConfigData * TreasureMapConfig::RandBackItem(int group_id)
{
	std::map<int, std::vector<TreasureMapBackItemsCon> >::iterator group_it = m_tmt_back_item_map.find(group_id);
	if (m_tmt_back_item_map.end() == group_it) return NULL;

	int rand_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::vector<TreasureMapBackItemsCon>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
	{
		if (rand_rate < item_it->prob) return & item_it->item;
		rand_rate -= item_it->prob;
	}

	return NULL;
}

const ItemConfigData * TreasureMapConfig::GetMapItem(int map_type, int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL || map_type <= 0 || map_type >= TREASURE_MAP_ITEM_RANK_TYPE_MAX) return NULL;

	for (int i = 0; i < (int)m_map_obtain_cfg.size(); i++)
	{
		if (map_type == m_map_obtain_cfg[i].map_type)
		{
			if (m_map_obtain_cfg[i].min_level <= level && m_map_obtain_cfg[i].max_level >= level)
			{
				return &m_map_obtain_cfg[i].item;
			}
		}
	}
	return NULL;
}

const TreasureMapItemCfg::ItemCfg * TreasureMapConfig::GetItemDetailCfg(int seq, int role_level, int index)
{
	std::map<int, TreasureMapItemCfg>::iterator iter = m_item_cfg.find(seq);
	if (iter != m_item_cfg.end())
	{
		for (int role_level_section_index = 0; role_level_section_index < (int)iter->second.role_level_item_cfg.size(); ++role_level_section_index)
		{
			const TreasureMapItemCfg::RoleLevelItemCfg & role_level_item_cfg = iter->second.role_level_item_cfg[role_level_section_index];

			if (role_level_item_cfg.min_role_level <= role_level && role_level <= role_level_item_cfg.max_role_level)
			{
				if (index >= 0 && index < (int)role_level_item_cfg.group_item_cfg.size())
				{
					return &role_level_item_cfg.group_item_cfg[index];
				}
			}
		}
	}
	return NULL;
}

void TreasureMapConfig::GetRandMonsterAndScene(int role_level, int ARG_OUT * task_id, int ARG_OUT * scene_id, int ARG_OUT * monster_id, Posi ARG_OUT * pos)
{
	for (std::map<int, TreasureMapTaskCfg>::iterator iter = m_task_cfg.begin(); iter != m_task_cfg.end(); ++iter)
	{
		if (iter->second.min_level <= role_level && role_level <= iter->second.max_level)
		{
			if (task_id != NULL)
			{
				*task_id = iter->second.task_id;
			}

			if (scene_id != NULL)
			{
				*scene_id = GetRandScene(iter->second.scene_group_id, pos);
			}

			if (monster_id != NULL)
			{
				*monster_id = GetRandMonster(iter->second.monster_group_id);
			}

			break;
		}
	}
}

int TreasureMapConfig::GetRandScene(int scene_group_id, Posi ARG_OUT * pos, const char* reason)
{
	std::map<int, TreasureMapSceneCfg > ::iterator iter = m_rand_scene_cfg.find(scene_group_id);
	if (iter != m_rand_scene_cfg.end())
	{
		int r = rand() % TreasureMapSceneCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			if (r < iter->second.group_item_cfg[i].rate)
			{
				if (NULL != pos)
				{
					*pos = this->GetRandPos(iter->second.group_item_cfg[i].pos_group_id);
				}
				return  iter->second.group_item_cfg[i].scene_id;
			}

			r -= iter->second.group_item_cfg[i].rate;
		}
	}

	return 0;
}

int TreasureMapConfig::GetRandMonster(int monster_group_id)
{
	std::map<int, TreasureMapMonsterCfg > ::iterator iter = m_rand_monster_cfg.find(monster_group_id);
	if (iter != m_rand_monster_cfg.end())
	{
		int r = rand() % TreasureMapMonsterCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			if (r < iter->second.group_item_cfg[i].rate)
			{
				return  iter->second.group_item_cfg[i].monster_id;
			}

			r -= iter->second.group_item_cfg[i].rate;
		}
	}

	return 0;
}

Posi TreasureMapConfig::GetRandPos(int pos_group_id, const char* reason)
{
	std::map<int, TreasureMapCoordinateGroupCfg>::iterator it = m_coordinate_group_cfg.find(pos_group_id);
	if (it != m_coordinate_group_cfg.end())
	{
		int r = rand() % TreasureMapCoordinateGroupCfg::MAX_RATE_COUNT;
		for (int i = 0; i < (int)it->second.group_item_cfg.size(); ++i)
		{
			if (r < it->second.group_item_cfg[i].rate)
			{
				return  it->second.group_item_cfg[i].pos;
			}
			r -= it->second.group_item_cfg[i].rate;
		}
	}
	if (NULL != reason)
	{
		gamelog::g_Log_treasure_map.printf(LL_INFO, "%s->%s error pos_group_id[%d]", reason, __FUNCTION__, pos_group_id);
	}

	return Posi(0, 0);
}

int TreasureMapConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "daily_times", m_other_cfg.limit_get_reward_times))
	{
		return -1;
	}
	if (!GetSubNodeValue(root_element, "max_time", m_other_cfg.max_play_times))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "high_map_id", m_other_cfg.high_map_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.high_map_id))
	{
		return -3;
	}

	return 0;
}

int TreasureMapConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		TreasureMapTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "task_id", cfg.task_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", cfg.min_level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_level))
		{
			return -3;
		}


		if (!GetSubNodeValue(root_element, "scene_group_id", cfg.scene_group_id))
		{
			return -4;
		}


		if (!GetSubNodeValue(root_element, "monster_group_id", cfg.monster_group_id))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "exp", cfg.exp))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.coin))
		{
			return -5;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward");
			if (NULL != item_node)		// 允许奖励道具为空
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node))
				{
					return -106;
				}
				cfg.reward_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}

		if (m_task_cfg.count(cfg.task_id) != 0)
		{
			return -5000;
		}

		m_task_cfg[cfg.task_id] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureMapConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "scene_group_id", group_id))
		{
			return -1;
		}

		TreasureMapSceneCfg & cfg = m_rand_scene_cfg[group_id];
		TreasureMapSceneCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "scene_id", item_cfg.scene_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "pos_group_id", item_cfg.pos_group_id) || m_coordinate_group_cfg.find(item_cfg.pos_group_id) == m_coordinate_group_cfg.end())
		{
			return -4;
		}

		cfg.group_item_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rand_scene_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, TreasureMapSceneCfg >::iterator iter = m_rand_scene_cfg.begin(); iter != m_rand_scene_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			count += iter->second.group_item_cfg[i].rate;
		}

		if (count != TreasureMapSceneCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int TreasureMapConfig::InitMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int,std::set<int> > group_monster_repeat_cheak;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		TreasureMapMonsterCfg & cfg = m_rand_monster_cfg[group_id];
		TreasureMapMonsterCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "monster_id", item_cfg.monster_id))
		{
			return -3;
		}

		if (group_monster_repeat_cheak[group_id].find(item_cfg.monster_id) != group_monster_repeat_cheak[group_id].end())
		{
			return -6222;
		}

		group_monster_repeat_cheak[group_id].insert(item_cfg.monster_id);

		cfg.group_item_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rand_monster_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, TreasureMapMonsterCfg >::iterator iter = m_rand_monster_cfg.begin(); iter != m_rand_monster_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			count += iter->second.group_item_cfg[i].rate;
		}

		if (count != TreasureMapMonsterCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int TreasureMapConfig::InitItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "seq", group_id))
		{
			return -1;
		}

		TreasureMapItemCfg & cfg = m_item_cfg[group_id];

		int min_role_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_role_level) || min_role_level <= 0)
		{
			return -211;
		}

		int max_role_level = 0;
		if (!GetSubNodeValue(root_element, "max_level", max_role_level) || max_role_level < min_role_level)
		{
			return -212;
		}

		int last_max_role_level = 0;
		if (cfg.role_level_item_cfg.empty())
		{
			cfg.role_level_item_cfg.push_back(TreasureMapItemCfg::RoleLevelItemCfg());
			if (min_role_level != last_max_role_level + 1)
			{
				return -213;
			}
		}
		else 
		{
			TreasureMapItemCfg::RoleLevelItemCfg & tmp_role_level_item_cfg = cfg.role_level_item_cfg.back();
			last_max_role_level = tmp_role_level_item_cfg.max_role_level;
			if (tmp_role_level_item_cfg.min_role_level != min_role_level || tmp_role_level_item_cfg.max_role_level != max_role_level)
			{
				cfg.role_level_item_cfg.push_back(TreasureMapItemCfg::RoleLevelItemCfg());
				if (min_role_level != last_max_role_level + 1)
				{
					return -213;
				}
			}
		}

		TreasureMapItemCfg::RoleLevelItemCfg & role_level_item_cfg = cfg.role_level_item_cfg.back();

		role_level_item_cfg.min_role_level = min_role_level;
		role_level_item_cfg.max_role_level = max_role_level;


		TreasureMapItemCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "type", item_cfg.item_type) || item_cfg.item_type < 0 || item_cfg.item_type >= TREASURE_MAP_ITEM_TYPE_MAX)
		{
			return -3;
		}

		if (item_cfg.item_type== TREASURE_MAP_ITEM_TYPE_MONSTER)
		{
			if (!GetSubNodeValue(root_element, "monster_id", item_cfg.monster_id) || item_cfg.monster_id <= 0)
			{
				return -4;
			}
		}

		if (item_cfg.item_type == TREASURE_MAP_ITEM_TYPE_COIN)
		{
			if (!GetSubNodeValue(root_element, "coin", item_cfg.coin) || item_cfg.coin <= 0)
			{
				return -5;
			}
		}

		if (item_cfg.item_type == TREASURE_MAP_ITEM_TYPE_RAND_ITEM)
		{
			if (!GetSubNodeValue(root_element, "item_group_id", item_cfg.reward_grounp_id))
			{
				return -6;
			}
		}

		if (!GetSubNodeValue(root_element, "map_type", item_cfg.map_type))
		{
			return -7;
		}

		if (item_cfg.item_type == TREASURE_MAP_ITEM_TYPE_ITEM)
		{
			TiXmlElement * clear_reward = root_element->FirstChildElement("item");
			if (!item_cfg.item.ReadConfig(clear_reward))
			{
				return -104;
			}
		}
		if (!GetSubNodeValue(root_element, "is_notice", item_cfg.is_notice))
		{
			return -8;
		}

		item_cfg.seq = group_id;
		item_cfg.index = role_level_item_cfg.group_item_cfg.size();
		role_level_item_cfg.group_item_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, TreasureMapItemCfg >::iterator iter = m_item_cfg.begin(); iter != m_item_cfg.end(); ++iter)
	{
		for (int role_level_section_index = 0; role_level_section_index < (int)iter->second.role_level_item_cfg.size(); ++role_level_section_index)
		{
			const TreasureMapItemCfg::RoleLevelItemCfg & role_level_item_cfg = iter->second.role_level_item_cfg[role_level_section_index];
			std::vector<TreasureMapItemCfg::ItemCfg> rand_vector_item_cfg = role_level_item_cfg.group_item_cfg;
			int count = 0;
			for (int i = 0; i < (int)rand_vector_item_cfg.size(); ++i)
			{
				count += rand_vector_item_cfg[i].rate;
			}

			if (count != TreasureMapItemCfg::MAX_RATE_COUNT)
			{
				return -10086;
			}			
		}
	}

	return 0;
}

int TreasureMapConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "item_group_id", group_id))
		{
			return -1;
		}

		TreasureMapRewardGroupCfg & cfg = m_reward_group_cfg[group_id];
		TreasureMapRewardGroupCfg::ItemCfg item_cfg;


		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate))
		{
			return -2;
		}

		TiXmlElement * rand_item = root_element->FirstChildElement("item");
		if (!item_cfg.item.ReadConfig(rand_item))
		{
			return -104;
		}
		

		cfg.group_item_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_item_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, TreasureMapRewardGroupCfg >::iterator iter = m_reward_group_cfg.begin(); iter != m_reward_group_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			count += iter->second.group_item_cfg[i].rate;
		}

		if (count != TreasureMapRewardGroupCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int TreasureMapConfig::InitBackItemsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int group_id = 0; TreasureMapBackItemsCon tmp_con;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.prob) || tmp_con.prob <= 0 || tmp_con.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL == item_node) return -20000;

		if (!tmp_con.item.ReadConfig(item_node))
		{
			return -3;
		}

		std::map<int, std::vector<TreasureMapBackItemsCon> >::iterator group_it = m_tmt_back_item_map.find(group_id);
		if (m_tmt_back_item_map.end() != group_it)
		{
			group_it->second.push_back(tmp_con);
		}
		else
		{
			std::vector<TreasureMapBackItemsCon> tmp_list; tmp_list.push_back(tmp_con);
			m_tmt_back_item_map[group_id] = tmp_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<TreasureMapBackItemsCon> >::iterator group_it = m_tmt_back_item_map.begin(); group_it != m_tmt_back_item_map.end(); ++group_it)
	{
		int prob_total = 0;
		for (std::vector<TreasureMapBackItemsCon>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
		{
			prob_total += item_it->prob;
		}

		if (prob_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -500;
	}

	return 0;
}

int TreasureMapConfig::InitAwardBackCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		ParamSection level_sec; int find_type = 0; TreasureMapAwardBackCon tmp_con;
		if (!GetSubNodeValue(root_element, "min_level", level_sec.section_min) || level_sec.section_min <= 0 || level_sec.section_min > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_sec.section_max) || level_sec.section_max < level_sec.section_min || level_sec.section_max > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "find_type", tmp_con.find_type) || tmp_con.find_type < 0 || tmp_con.find_type >= RB_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.experience) || tmp_con.experience < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin_bind) || tmp_con.coin_bind < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "family_gongxian", tmp_con.family_contribution) || tmp_con.family_contribution < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "group_id", tmp_con.back_item_group_id) || (tmp_con.back_item_group_id >= 0 && m_tmt_back_item_map.end() == m_tmt_back_item_map.find(tmp_con.back_item_group_id)) || (tmp_con.back_item_group_id < 0 && tmp_con.back_item_group_id != -1))
		{
			return -7;
		}
		
		if (!GetSubNodeValue(root_element, "seq", tmp_con.seq) || tmp_con.seq < 0)
		{
			return -8;
		}
		m_ps_to_tmb[level_sec] = tmp_con;
		m_tmt_find_back_map[level_sec][tmp_con.find_type] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureMapConfig::InitMapObtainCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		TreasureMapObtainCfg node_cfg;
		if (!GetSubNodeValue(root_element, "map_type", node_cfg.map_type) || node_cfg.map_type <= 0 || node_cfg.map_type >= TREASURE_MAP_ITEM_RANK_TYPE_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level <= 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}
		
		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL != item_element)
		{
			if(!node_cfg.item.ReadConfig(item_element)) return -100;
		}

		m_map_obtain_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TreasureMapConfig::InitCoordinateGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		TreasureMapCoordinateGroupCfg & cfg = m_coordinate_group_cfg[group_id];
		TreasureMapCoordinateGroupCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}
		
		if (!GetSubNodeValue(root_element, "x", item_cfg.pos.x) || item_cfg.pos.x < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "y", item_cfg.pos.y) || item_cfg.pos.y < 0)
		{
			return -4;
		}

		cfg.group_item_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_coordinate_group_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, TreasureMapCoordinateGroupCfg >::iterator iter = m_coordinate_group_cfg.begin(); iter != m_coordinate_group_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_item_cfg.size(); ++i)
		{
			count += iter->second.group_item_cfg[i].rate;
		}

		if (count != TreasureMapCoordinateGroupCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}
