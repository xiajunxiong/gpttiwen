#include "fantasyfbconfig.hpp"

FantasyFbConfig::FantasyFbConfig()
{
}

FantasyFbConfig::~FantasyFbConfig()
{
	for (std::map<int, std::vector<FantasyFBSceneCfg> >::iterator it = m_scene_by_level.begin(); it != m_scene_by_level.end(); it++)
	{
		std::vector<FantasyFBSceneCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<FantasyFBRewardGroupCfg> >::iterator it = m_reward_group_cfg.begin(); it != m_reward_group_cfg.end(); it++)
	{
		std::vector<FantasyFBRewardGroupCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<Event_NPC_FB_Cfg> >::iterator it = m_event_npc_cfg.begin(); it != m_event_npc_cfg.end(); it++)
	{
		std::vector<Event_NPC_FB_Cfg>().swap(it->second);
	}

	for (std::map<int, std::vector<FantasyFBFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<FantasyFBFindRewardGroupCfg>().swap(it->second);
	}

	std::vector<FantasyFBFindRewardCfg>().swap(m_fanrwcfg_v);
}

bool FantasyFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("cave_content", InitSceneCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_CONFIG("boss_reward", InitRewardCfg);
	LOAD_CONFIG("gather_group", InitEventNPCCfg);
	LOAD_CONFIG("gather_reward", InitGatherRewardCfg);
	LOAD_CONFIG("reward_find_group", InitFindRewardGroupCfg);
	LOAD_CONFIG("reward_find", InitFindCfg);
	return true;
}


const FantasyFBSceneCfg * FantasyFbConfig::GetFBSceneCfg(int level, std::map<int, int>* seq_by_layer)
{
	if(level <=0 || level > MAX_ROLE_LEVEL) return NULL;
	for (std::map<int, std::vector<FantasyFBSceneCfg> >::const_iterator it = m_scene_by_level.begin(); it != m_scene_by_level.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
		{
			int layer = it->second.size();
			std::vector<FantasyFBSceneCfg>::const_iterator scene_it = it->second.begin();
			for (int i = 1; i <= layer && scene_it != it->second.end(); i++,scene_it++)
			{
				(*seq_by_layer)[i] = scene_it->seq;
			}
			return &(*it->second.begin());
		}
	}

	return NULL;
}

const int FantasyFbConfig::GetSeqByBossid(int _boss_id)
{
	std::map<int, int>::iterator it = m_bossid_to_seq.find(_boss_id);
	if (it != m_bossid_to_seq.end()) return it->second;

	return 0;
}

const FantasyFBRewardGroupCfg * FantasyFbConfig::GetRewardGroupCfg(int reward_id)
{
	std::map<int, std::vector<FantasyFBRewardGroupCfg> >::const_iterator it = m_reward_group_cfg.find(reward_id);
	if (it == m_reward_group_cfg.end()) return NULL;

	int rand_num = rand() % RATE_COUNT_MAX_NUM;
	int weight = 0;
	for (std::vector<FantasyFBRewardGroupCfg>::const_iterator reward_group_it = it->second.begin(); reward_group_it != it->second.end(); reward_group_it++)
	{
		weight += reward_group_it->rate;
		if (weight >= rand_num)
		{
			return &(*reward_group_it);
		}
	}
	return NULL;
}

const FantasyFbKillBossRewardCfg * FantasyFbConfig::GetRewardCfg(int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return NULL;
	for (std::map<int, FantasyFbKillBossRewardCfg>::const_iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
			return &it->second;
	}
	return NULL;
}

int FantasyFbConfig::GetEventNPCFBCfg(int event_group_id, int need_npc_num, std::vector<Event_NPC_FB_Cfg>* node_cfg, std::map<int, std::vector<SpecialLogicFantasyObjInfo> >* obj_info_list)
{
	std::map<int, std::vector<Event_NPC_FB_Cfg> >::const_iterator it = m_event_npc_cfg.find(event_group_id);
	if(it == m_event_npc_cfg.end()) return 0;

	int ret_count = 0;
	int rand_max_num = RATE_COUNT_MAX_NUM;
	int weight = 0;
	std::vector<Event_NPC_FB_Cfg> list;
	for (std::vector<Event_NPC_FB_Cfg>::const_iterator event_npc_it = it->second.begin(); event_npc_it != it->second.end(); event_npc_it++)
	{
		std::map<int, std::vector<SpecialLogicFantasyObjInfo> >::const_iterator obj_it = obj_info_list->find(event_npc_it->npc_type);
		if(obj_it == obj_info_list->end()) 
		{
			list.push_back(*event_npc_it);
			continue;
		}
		for (std::vector<SpecialLogicFantasyObjInfo>::const_iterator obj_info_it = obj_it->second.begin(); obj_info_it != obj_it->second.end(); obj_info_it++)
		{
			if (event_npc_it->npc_id == obj_info_it->npc_id)
			{
				rand_max_num -= event_npc_it->rate;
			}
			else
			{
				list.push_back(*event_npc_it);
			}
		}
	}
	for(int i = 0; i < need_npc_num; i++)
	{ 
		int rand_num = rand() % rand_max_num;
		for (std::vector<Event_NPC_FB_Cfg>::const_iterator event_it = list.begin(); event_it != list.end(); event_it++)
		{
			weight += event_it->rate;
			if (weight >= rand_num)
			{
				(*node_cfg).push_back(*event_it);
				rand_max_num -= event_it->rate;
				ret_count++;
				break;
			}
		}
	}
	return ret_count;
}

const FantasyFBGatherRewardCfg * FantasyFbConfig::GetGatherRewardCfg(int level, int npc_id)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	for (std::map<int, FantasyFBGatherRewardlevelCfg>::const_iterator it = m_gather_reward_cfg.begin(); it != m_gather_reward_cfg.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
		{
			for (int i = 0; i < (int)it->second.node_cfg.size(); i++)
			{
				if(it->second.node_cfg[i].npc_id == npc_id)
					return &it->second.node_cfg[i];
			}
		}
	}
	return NULL;
}

const FantasyFbFindLevelGroupCfg * FantasyFbConfig::GetFindCfg(int level, int find_type)
{
	for (std::map<int, std::map<int, FantasyFbFindLevelGroupCfg> >::iterator it = m_fantasy_find_cfg.begin(); it != m_fantasy_find_cfg.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
		{
			std::map<int, FantasyFbFindLevelGroupCfg>::iterator find_it = it->second.find(find_type);
			if (find_it != it->second.end())
			{
				return &find_it->second;
			}
		}
	}

	return NULL;
}

const FantasyFBFindRewardGroupCfg* FantasyFbConfig::GetFindRewardCfg(int group_id)
{
	if(group_id <= 0) return NULL;			
	std::map<int, std::vector<FantasyFBFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.find(group_id);
	if (it == m_find_reward_group_map.end()) return NULL;

	int rand_num = rand() % RATE_COUNT_MAX_NUM;
	int count = 0;
	for (int i = 0; i < (int)it->second.size(); i++)
	{
		count += it->second[i].rate;
		if (count >= rand_num)
		{
			return &it->second[i];
		}
	}
	return NULL;
}

const int FantasyFbConfig::GetFindType(int _role_level)
{

	for (std::vector<FantasyFBFindRewardCfg>::iterator it = m_fanrwcfg_v.begin(); it != m_fanrwcfg_v.end(); ++it)
	{
		if (_role_level < it->min_level || _role_level > it->max_level) continue;
		return it->find_type;
	}	

	return -1;
}

const FantasyFBSceneCfg * FantasyFbConfig::GetSceneCfgBySeq(int seq)
{
	if(seq <= 0) return NULL;
	std::map<int, FantasyFBSceneCfg>::const_iterator it = m_scene_cfg.find(seq);
	if(it != m_scene_cfg.end()) return &it->second;

	return NULL;
}

int FantasyFbConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id) || npc_id <= 0) return -1;
		m_other_cfg.npc_id = npc_id;

		if (!GetSubNodeValue(root_element, "min_team_num", m_other_cfg.min_team_num) || m_other_cfg.min_team_num <= 0 ||m_other_cfg.min_team_num > TEAM_MEMBER_MAX_NUM) return -2;
		if (!GetSubNodeValue(root_element, "min_level", m_other_cfg.min_level) || m_other_cfg.min_level <= 0 || m_other_cfg.min_level > MAX_ROLE_LEVEL) return -3;
		
		root_element = root_element->NextSiblingElement();	
	}
	return 0;
}

int FantasyFbConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	while (NULL != root_element)
	{
		FantasyFBSceneCfg node_cfg;
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq <= 0) return -1;
		if (!GetSubNodeValue(root_element, "min_level", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level <= 0 || max_level < min_level || 
			max_level > MAX_ROLE_LEVEL) return -3;
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id) || node_cfg.scene_id <= 0) return -4;
		if (!GetSubNodeValue(root_element, "boss_id", node_cfg.boss_id) || node_cfg.boss_id <= 0) return -5;

		if (!GetSubNodeValue(root_element, "x", node_cfg.boss_pos.x) || node_cfg.boss_pos.x <= 0) return -6;
		if (!GetSubNodeValue(root_element, "y", node_cfg.boss_pos.y) || node_cfg.boss_pos.y <= 0) return -7;

		if (!GetSubNodeValue(root_element, "dir_y", node_cfg.monster_dir)) return -8;

		if (!GetSubNodeValue(root_element, "born_x", node_cfg.birth_pos.x) || node_cfg.birth_pos.x <= 0) return -9;
		if (!GetSubNodeValue(root_element, "born_y", node_cfg.birth_pos.y) || node_cfg.birth_pos.y <= 0) return -10;

		if (!GetSubNodeValue(root_element, "door_x", node_cfg.transport_pos.x) || node_cfg.transport_pos.x <= 0) return -11;
		if (!GetSubNodeValue(root_element, "door_y", node_cfg.transport_pos.y) || node_cfg.transport_pos.y <= 0) return -12;

		if (!GetSubNodeValue(root_element, "event_num", node_cfg.event_num) || node_cfg.event_num < 0) return -13;
		if (!GetSubNodeValue(root_element, "event_group_id", node_cfg.event_group_id) || node_cfg.event_group_id <= 0) return -14;
		
		m_scene_cfg[node_cfg.seq] = node_cfg;

		int level_key = min_level * 1000 + max_level;
		m_scene_by_level[level_key].push_back(node_cfg);
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_bossid_to_seq[node_cfg.boss_id] = node_cfg.seq;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int FantasyFbConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		FantasyFBRewardGroupCfg node_cfg;
		static int reward_id = 0;
		if (!GetSubNodeValue(root_element, "reward_id", reward_id) || reward_id <= 0) return -1;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -2;

		TiXmlElement * data_element = root_element->FirstChildElement("reward_item");
		if (NULL != data_element)
		{
			if(!node_cfg.reward.ReadConfig(data_element)) return -100;

			data_element = data_element->NextSiblingElement();
		}

		m_reward_group_cfg[reward_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int FantasyFbConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		FantasyFBRewardCfg node_cfg;
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level <= 0 || max_level < min_level ||max_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0) return -3;
		if (!GetSubNodeValue(root_element, "reward_exp", node_cfg.reward_exp) || node_cfg.reward_exp < 0) return -4;
		if (!GetSubNodeValue(root_element, "boss_id", node_cfg.boss_id) || node_cfg.boss_id < 0) return -5;

		TiXmlElement * data_element = root_element->FirstChildElement("online_reward_list");
		if (NULL != data_element)
		{
			TiXmlElement * node_element = data_element->FirstChildElement("online_reward");
			while(NULL != node_element)
			{ 
				ItemConfigData item;
				if (!item.ReadConfig(node_element)) return -100;

				node_cfg.reward.push_back(item);
				node_element = node_element->NextSiblingElement();
			}
		}

		int level_key = min_level * 1000 + max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		FantasyFbKillBossRewardCfg & cfg = m_reward_cfg[level_key];
		if(cfg.count >= LAYER_MAX_NUM) return -222;
		cfg.node[cfg.count++] = node_cfg;

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, FantasyFbKillBossRewardCfg>::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		if (it->second.count != LAYER_MAX_NUM)
		{
			printf("reload fantasy fb kill boss reward, count is failed, count = %d\n", it->second.count);
			return -333;
		}
	}

	return 0;
}

int FantasyFbConfig::InitEventNPCCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		Event_NPC_FB_Cfg node_cfg;
		if (!GetSubNodeValue(root_element, "event_group_id", node_cfg.event_group_id) || node_cfg.event_group_id <= 0) return -1;
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id) || node_cfg.npc_id <= 0) return -2;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -3;
		if (!GetSubNodeValue(root_element, "npc_type", node_cfg.npc_type) || node_cfg.npc_type < 0) return -4;

		if (node_cfg.npc_type == Event_NPC_FB_Cfg::FANTASYFB_NPC_TYPE_9)
		{
			if (!GetSubNodeValue(root_element, "box_type", node_cfg.box_type) || node_cfg.box_type < 0) return -5;
			if (!GetSubNodeValue(root_element, "consume_item", node_cfg.consume_item) || node_cfg.consume_item <= 0) return -6;
			if (!GetSubNodeValue(root_element, "consume_num", node_cfg.consume_num) || node_cfg.consume_num < 0) return -7;
		}

		m_event_npc_cfg[node_cfg.event_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int FantasyFbConfig::InitGatherRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int min_level = 0;
		int max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level <= 0 || max_level < min_level || max_level > MAX_ROLE_LEVEL) return -2;
		
		FantasyFBGatherRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id) || node_cfg.npc_id <= 0) return -3;
		if (!GetSubNodeValue(root_element, "reward_type", node_cfg.reward_type) || node_cfg.reward_type <= 0) return -4;
		if (!GetSubNodeValue(root_element, "reward", node_cfg.reward_id) || node_cfg.reward_id < 0) return -5;

		int level_key =  min_level * 1000 + max_level;
		m_gather_reward_cfg[level_key].node_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int FantasyFbConfig::InitFindRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	while (NULL != root_element)		// ÔÊÐí¿Õ±í
	{
		int group_id = 0; FantasyFBFindRewardGroupCfg tmp_con;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.rate) || tmp_con.rate <= 0 || tmp_con.rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL == item_node) return -10000;

		if (!tmp_con.reward.ReadConfig(item_node))
		{
			return -3;
		}

		m_find_reward_group_map[group_id].push_back(tmp_con);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<FantasyFBFindRewardGroupCfg> >::iterator group_it = m_find_reward_group_map.begin(); group_it != m_find_reward_group_map.end(); ++group_it)
	{
		int rate_total = 0;
		for (std::vector<FantasyFBFindRewardGroupCfg>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
		{
			rate_total += item_it->rate;
		}

		if (rate_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -500;
	}

	return 0;
}

int FantasyFbConfig::InitFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		FantasyFBFindRewardCfg node_cfg;
		//int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(dataElement, "find_type", node_cfg.find_type) || node_cfg.find_type < RB_TYPE_COIN || node_cfg.find_type > RB_TYPE_GOLD)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(dataElement, "max_level", node_cfg.max_level) || node_cfg.max_level < 0 || node_cfg.max_level > MAX_ROLE_LEVEL || node_cfg.max_level < node_cfg.min_level)
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(dataElement, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "family_gongxian", node_cfg.family_gongxian) || node_cfg.family_gongxian < 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(dataElement, "group_id", node_cfg.group_id) || (node_cfg.group_id > 0 && m_find_reward_group_map.end() == m_find_reward_group_map.find(node_cfg.group_id)) || (node_cfg.group_id < 0 && node_cfg.group_id != -1))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "seq", node_cfg.seq) || node_cfg.seq < 0)
		{
			return -8;
		}

		int level_key = node_cfg.min_level * 1000 + node_cfg.max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);
		
		FantasyFbFindLevelGroupCfg & cfg = m_fantasy_find_cfg[level_key][node_cfg.find_type];
		if(cfg.count >= LAYER_MAX_NUM) return -222;
		cfg.node[cfg.count++] = node_cfg;

		//m_seq_to_fanrwcfg[node_cfg.seq] = node_cfg;
		m_fanrwcfg_v.push_back(node_cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	/*for (std::map<int, std::map<int, FantasyFbFindLevelGroupCfg> >::iterator level_it = m_fantasy_find_cfg.begin(); level_it != m_fantasy_find_cfg.end(); level_it++)
	{
		for (std::map<int, FantasyFbFindLevelGroupCfg>::iterator find_it = level_it->second.begin(); find_it != level_it->second.end(); find_it++)
		{
			if (find_it->second.count != LAYER_MAX_NUM)
			{
				printf("reload fantasy fb find level group, count is failed, count = %d\n", find_it->second.count);
				return -333;
			}
		}
	}
	*/

	return 0;
}
