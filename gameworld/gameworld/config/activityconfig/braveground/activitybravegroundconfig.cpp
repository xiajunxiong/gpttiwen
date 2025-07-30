#include "global/worldstatus/worldstatus.hpp"

#include "activitybravegroundconfig.hpp"

ActivityBraveGroundConfig::ActivityBraveGroundConfig()
{
}

ActivityBraveGroundConfig::~ActivityBraveGroundConfig()
{
	std::vector<BraveGroundLevelCfg>().swap(m_level_cfg);
	std::vector<BraveGroundRankRewardCfg>().swap(m_rank_reward_cfg);
	for (std::map<int, std::vector<BraveGroundLevelRewardCfg> >::iterator it = m_level_reward_cfg.begin(); it != m_level_reward_cfg.end(); it++)
	{
		std::vector<BraveGroundLevelRewardCfg>().swap(it->second);
	}
}

bool ActivityBraveGroundConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("level_cfg", InitLevelCfg);
	LOAD_CONFIG("npc_group", InitNpcGroupCfg);
	LOAD_CONFIG("npc_list", InitNpcCfg);
	LOAD_CONFIG("rank_reward", InitRankRewardCfg);
	LOAD_CONFIG("born_group", InitPosCfg);
	LOAD_CONFIG("buff_list", InitBuffCfg);
	LOAD_CONFIG("reward_group", InitLevelRewardCfg);
	

	return true;
}

const BraveGroundLevelCfg * ActivityBraveGroundConfig::GetLevelCfg(int level, bool get_pos)
{
	int index = level - 1;
	if (index < 0 || index >= (int)m_level_cfg.size())
	{
		return NULL;
	}

	if (get_pos)
	{
		this->RefreshPos(m_level_cfg[index]);
	}

	return &m_level_cfg[index];
}

const BraveGroundLevelRewardCfg * ActivityBraveGroundConfig::GetLevelRewardCfg(int level, int role_level)
{
	const BraveGroundLevelCfg * level_cfg = this->GetLevelCfg(level);
	if (!level_cfg)
	{
		return NULL;
	}

	const std::vector<BraveGroundLevelRewardCfg> * vec_ptr = MapValuesConstPtr(m_level_reward_cfg, level_cfg->reward_id);
	if (!vec_ptr)
	{
		return NULL;
	}

	const std::vector<BraveGroundLevelRewardCfg> & vec = *vec_ptr;

	for (int i = 0; i < (int)vec.size(); ++i)
	{
		if (vec[i].min_role_level <= role_level && role_level <= vec[i].max_role_level)
		{
			return &vec[i];
		}
	}

	return NULL;
}

const BraveGroundNpcCfg * ActivityBraveGroundConfig::GetRandNpcCfg(int npc_group_id)
{
	std::map<int, BraveGroundNpcGroupCfg > ::iterator iter = m_npc_group_cfg.find(npc_group_id);
	if (iter == m_npc_group_cfg.end())
	{
		return NULL;
	}

	int r = rand() % BraveGroundNpcGroupCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
	{
		if (r < iter->second.group_cfg[i].rate)
		{
			return GetNpcCfg(iter->second.group_cfg[i].npc_id);
		}

		r -= iter->second.group_cfg[i].rate;
	}

	return NULL;
}

const BraveGroundNpcCfg * ActivityBraveGroundConfig::GetNpcCfg(int npc_id)
{
	std::map<int, BraveGroundNpcCfg> ::iterator iter = m_npc_cfg.find(npc_id);
	if (iter!= m_npc_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const BraveGroundOtherCfg & ActivityBraveGroundConfig::GetOtherCfg()
{
	return m_othter_cfg;
}

const BraveGroundRankRewardCfg::ItemCfg * ActivityBraveGroundConfig::GetRankRewardCfg(int rank, int role_level)
{
	for (int i = 0; i < (int)m_rank_reward_cfg.size(); ++i)
	{
		const BraveGroundRankRewardCfg & rank_cfg = m_rank_reward_cfg[i];

		if (rank_cfg.min_role_level <= role_level && role_level<= rank_cfg.max_role_level)
		{
			for (int rank_index = 0; rank_index < (int)rank_cfg.rewards.size(); ++rank_index)
			{
				if (rank_cfg.rewards[rank_index].rank_min <= rank && rank <= rank_cfg.rewards[rank_index].rank_max)
				{
					return &rank_cfg.rewards[rank_index];
				}
			}
		}
	}

	return NULL;
}

int ActivityBraveGroundConfig::GetSaoDangRewardLevel(int max_reached_level, int role_level)
{
	for (std::vector<BraveGroundLevelCfg>::reverse_iterator iter = m_level_cfg.rbegin(); iter != m_level_cfg.rend(); ++iter)
	{
		if (iter->can_sao_dang && max_reached_level >= iter->level && role_level >= iter->role_level)
		{
			return  iter->level;
		}
	}

	return 0;
}

ActivityBraveGroundSaoDangReward ActivityBraveGroundConfig::GetSaoDangLevelReward(int level, int role_level)
{
	ActivityBraveGroundSaoDangReward ret;

	for (std::vector<BraveGroundLevelCfg>::iterator iter = m_level_cfg.begin(); iter != m_level_cfg.end(); ++iter)
	{
		if (level < iter->level)
		{
			break;
		}

		const BraveGroundLevelRewardCfg * cfg = this->GetLevelRewardCfg(iter->level, role_level);
		if (!cfg)
		{
			continue;
		}

		ret.coin += cfg->coin;
		ret.exp += cfg->exp_reward;
		ret.rewards.insert(ret.rewards.end(), cfg->rewards.begin(), cfg->rewards.end());
	}

	::MergeItemConfigData(ret.rewards);

	return ret;
}

int ActivityBraveGroundConfig::GetRandBuffId()
{
	int r = RandomNum(ActivityBraveGroundBuffCfg::MAX_RATE_COUNT);

	for (int i = 0; i < (int)m_buff_cfg.group_cfg.size(); ++i)
	{
		if (r < m_buff_cfg.group_cfg[i].rate)
		{
			return m_buff_cfg.group_cfg[i].buff_id;
		}

		r -= m_buff_cfg.group_cfg[i].rate;
	}

	return 0;
}

int ActivityBraveGroundConfig::GetLastLevel()
{
	return this->GetCfgMaxLevel();   //现在没了世界等级限制，最后一关就是配置的最大关
}

int ActivityBraveGroundConfig::GetCfgMaxLevel()
{
	return (int)m_level_cfg.size();
}

int ActivityBraveGroundConfig::GetMaxSelectLevel(int role_level, int curr_reached_level, int max_reached_level)
{
	int min1= this->GetSaoDangRewardLevel(curr_reached_level, role_level);
	int min2= this->GetSaoDangRewardLevel(max_reached_level, role_level);
	return std::max(min1, min2);
}

int ActivityBraveGroundConfig::GetUseItemLimitTimes(int use_limit_group_id, int use_type)
{
	std::map<int, ActivityBraveGroundUseItemLimitCfg>::iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return MAX_INT;
	}

	std::map<int, int>::iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return MAX_INT;
	}

	return use_limit_iter->second;
}

int ActivityBraveGroundConfig::IsLimitTimesUseType(int use_limit_group_id, int use_type)
{
	std::map<int, ActivityBraveGroundUseItemLimitCfg>::iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return false;
	}

	std::map<int, int>::iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return false;
	}

	return true;
}

int ActivityBraveGroundConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "min_level", m_othter_cfg.need_role_level))
	{
		return -__LINE__;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_othter_cfg.top_role_title); ++i)
	{
		std::string s = STRING_SPRINTF("title_%d", i + 1);
		if (!GetSubNodeValue(root_element, s, m_othter_cfg.top_role_title[i]))
		{
			return -__LINE__;
		}
	}

	return 0;
}

int ActivityBraveGroundConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::set<int> check_repeat_scene_id;

	int next_level = 1;
	int last_need_world_level = 0;
	while (NULL != root_element)
	{
		BraveGroundLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "level", cfg.level) || cfg.level != next_level)
		{
			return -1;
		}
		next_level += 1;

		if (!GetSubNodeValue(root_element, "need_world_level", cfg.need_world_level) || cfg.need_world_level < last_need_world_level)
		{
			return -11;
		}
		last_need_world_level = cfg.need_world_level;

		if (!GetSubNodeValue(root_element, "role_level", cfg.role_level))
		{
			return -111;
		}

		if (!GetSubNodeValue(root_element, "mop_up_node", cfg.can_sao_dang))
		{
			return -112;
		}

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -2;
		}

		if (check_repeat_scene_id.find(cfg.scene_id) != check_repeat_scene_id.end())
		{
			return -3;
		}
		check_repeat_scene_id.insert(cfg.scene_id);

		if (!GetSubNodeValue(root_element, "boss_id", cfg.boss_monster_npc_id))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "boss_x", cfg.boss_pos.x) || cfg.boss_pos.x < 0)
		{
			return -41;
		}

		if (!GetSubNodeValue(root_element, "boss_y", cfg.boss_pos.y) || cfg.boss_pos.y < 0)
		{
			return -42;
		}

		if (!GetSubNodeValue(root_element, "boss_dir", cfg.boss_dir))
		{
			return -43;
		}

		if (!GetSubNodeValue(root_element, "reward_group", cfg.reward_id))
		{
			return -44;
		}

		if (!GetSubNodeValue(root_element, "npc_group_id", cfg.npc_group_id))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "monster_id", cfg.monster_npc_id))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "monster_num", cfg.m_max_monster_num))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "sceneborn_group_id", cfg.sceneborn_group_id))
		{
			return -9;
		}

		TiXmlElement * drop_list_element = root_element->FirstChildElement("drop_list");
		if (drop_list_element != NULL)
		{
			TiXmlElement * node_element = drop_list_element->FirstChildElement("dropid");
			while (node_element)
			{
				int drop_id = 0;
				if (!GetNodeValue(node_element, drop_id))
				{
					return -20001;
				}

				cfg.drop_id_list.push_back(drop_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		m_level_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityBraveGroundConfig::InitNpcGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "npc_group_id", group_id))
		{
			return -1;
		}

		BraveGroundNpcGroupCfg & cfg = m_npc_group_cfg[group_id];
		BraveGroundNpcGroupCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "npc_id", item_cfg.npc_id))
		{
			return -3;
		}


		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_npc_group_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, BraveGroundNpcGroupCfg >::iterator iter = m_npc_group_cfg.begin(); iter != m_npc_group_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count > BraveGroundNpcGroupCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int ActivityBraveGroundConfig::InitNpcCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id))
		{
			return -1;
		}

		BraveGroundNpcCfg & cfg = m_npc_cfg[npc_id];

		cfg.npc_id = npc_id;

		if (!GetSubNodeValue(root_element, "npc_type", cfg.npc_fun_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param_1", cfg.param1))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "param_2", cfg.param2))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "param_3", cfg.param3))
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityBraveGroundConfig::InitRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_rank = 0;

	int last_role_level_min = 0;
	int last_role_level_max = 0;
	while (NULL != root_element)
	{
		int role_level_min = 0;
		if (!GetSubNodeValue(root_element, "level_min", role_level_min))
		{
			return -1666;
		}

		int role_level_max = 0;
		if (!GetSubNodeValue(root_element, "level_max", role_level_max) || role_level_max < role_level_min)
		{
			return -1667;
		}

		if (role_level_min != last_role_level_min || role_level_max != last_role_level_max)
		{
			if (role_level_min <= last_role_level_max)
			{
				return -1668;
			}

			last_role_level_min = role_level_min;
			last_role_level_max = role_level_max;

			last_max_rank = 0;
			m_rank_reward_cfg.push_back(BraveGroundRankRewardCfg());
		}
		
		BraveGroundRankRewardCfg & cfg = m_rank_reward_cfg.back();
		cfg.min_role_level = role_level_min;
		cfg.max_role_level = role_level_max;

		BraveGroundRankRewardCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "min", item_cfg.rank_min) || item_cfg.rank_min < last_max_rank)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max", item_cfg.rank_max) || item_cfg.rank_max < item_cfg.rank_min)
		{
			return -2;
		}
		last_max_rank = item_cfg.rank_max;

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");

		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}
				item_cfg.rewards.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		cfg.rewards.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityBraveGroundConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int, std::set<int> >check_repeat;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "born_group_id", group_id))
		{
			return -1;
		}

		BraveGroundPosCfg& cfg = m_pos_cfg[group_id];
		BraveGroundPosCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "born_x", item_cfg.birth_pos.x))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "born_y", item_cfg.birth_pos.y))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "door_x", item_cfg.transport_pos.x))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "door_y", item_cfg.transport_pos.y))
		{
			return -6;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_pos_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, BraveGroundPosCfg >::iterator iter = m_pos_cfg.begin(); iter != m_pos_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != BraveGroundPosCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int ActivityBraveGroundConfig::InitBuffCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int, std::set<int> >check_repeat;
	while (NULL != root_element)
	{
		ActivityBraveGroundBuffCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "buff_id", item_cfg.buff_id))
		{
			return -__LINE__;
		}

		m_buff_cfg.rate_count += item_cfg.rate;
		m_buff_cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityBraveGroundConfig::InitLevelRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int, std::set<int> >check_repeat;
	while (NULL != root_element)
	{
		BraveGroundLevelRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.reward_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "exp_reward", cfg.exp_reward))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.coin))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "min_level", cfg.min_role_level))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_role_level))
		{
			ret_line;
		}


		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");

		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}

				cfg.rewards.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_level_reward_cfg[cfg.reward_id].push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

void ActivityBraveGroundConfig::RefreshPos(BraveGroundLevelCfg ARG_OUT & cfg)
{
	std::map<int, BraveGroundPosCfg > ::iterator iter = m_pos_cfg.find(cfg.sceneborn_group_id);
	if (iter == m_pos_cfg.end())
	{
		return;
	}

	std::vector<BraveGroundPosCfg::ItemCfg> group_cfg = iter->second.group_cfg;

	int r = rand() % BraveGroundPosCfg::MAX_RATE_COUNT;
	for (int i = 0; i < (int)group_cfg.size(); ++i)
	{
		if (r < group_cfg[i].rate)
		{
			cfg.birth_pos = group_cfg[i].birth_pos;
			cfg.transport_pos = group_cfg[i].transport_pos;
			break;
		}

		r -= group_cfg[i].rate;
	}
}



