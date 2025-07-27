#include "utaconfig.hpp"

UTAConfig* g_uta_config = NULL;
UTAConfig& UTAConfig::Instance()
{
	if (NULL == g_uta_config)
	{
		g_uta_config = new UTAConfig();
	}
	return *g_uta_config;
}

bool UTAConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("victory_reward", InitWinBoxRewardCfg);
	LOAD_CONFIG("rank_reward", InitRankRewardCfg);
	LOAD_CONFIG("promotion_reward", InitAdvanceRewardCfg);
	LOAD_CONFIG("default_integral", InitInitialScoreCfg);

	return true;
}

bool UTAConfig::Reload(const std::string &configname, std::string *err)
{
	UTAConfig* temp = new UTAConfig();
	if (!temp->Init(configname, err))
	{
		delete temp;
		temp = NULL;
		return false;
	}

	if (NULL != g_uta_config) delete g_uta_config;

	g_uta_config = temp;

	return true;
}

UTAConfig::UTAConfig()
{

}

int UTAConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	if (!GetSubNodeValue(dataElement, "map", m_other_cfg.scene_id))
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "countdown", m_other_cfg.enter_scene_count_down_s))
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "max_rounds", m_other_cfg.max_rounds))
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "win_score", m_other_cfg.win_score))
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "lose_score", m_other_cfg.lose_score))
	{
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "draw_score", m_other_cfg.draw_score))
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "waiver_score", m_other_cfg.give_up_score))
	{
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "need_active", m_other_cfg.need_active))
	{
		return -77;
	}

	{
		TiXmlElement* itemlistElement = dataElement->FirstChildElement("join_reward_list");
		if (NULL != itemlistElement)
		{
			TiXmlElement* itemElement = itemlistElement->FirstChildElement("join_reward");
			while (NULL != itemElement)
			{
				static ItemConfigData item;
				if (!item.ReadConfigNoCheck(itemElement))
				{
					return -8;
				}
				m_other_cfg.qualification_join_rewards.push_back(item);

				itemElement = itemElement->NextSiblingElement();
			}
		}
	}
	
	{
		TiXmlElement* itemlistElement = dataElement->FirstChildElement("lottery_active_list");
		if (NULL != itemlistElement)
		{
			TiXmlElement* itemElement = itemlistElement->FirstChildElement("lottery_active");
			while (NULL != itemElement)
			{
				static ItemConfigData item;
				if (!item.ReadConfigNoCheck(itemElement))
				{
					return -8;
				}
				m_other_cfg.active_rewards.push_back(item);

				itemElement = itemElement->NextSiblingElement();
			}
		}
	}

	if (!GetSubNodeValue(dataElement, "level", m_other_cfg.level_limit) 
		|| m_other_cfg.level_limit < 0 
		|| m_other_cfg.level_limit > MAX_ROLE_LEVEL)
	{
		return -11;
	}

	if (!GetSubNodeValue(dataElement, "icon_keep_time_s", m_other_cfg.icon_keep_time_s))
	{
		return -12;
	}

	return 0;
}

int UTAConfig::InitWinBoxRewardCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_seq = 0;
	int last_win_times = 0;
	while (dataElement)
	{
		UTAWinBoxRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "sn", cfg.seq) 
			|| cfg.seq != last_seq + 1 
			|| cfg.seq < 0 
			|| cfg.seq > 30)
		{
			return -1;
		}
		last_seq = cfg.seq;

		if (!GetSubNodeValue(dataElement, "victory_times", cfg.qualification_win_times) || last_win_times > cfg.qualification_win_times)
		{
			return -2;
		}
		last_win_times = cfg.qualification_win_times;

		TiXmlElement* itemListElement = dataElement->FirstChildElement("reward_list");
		if (NULL != itemListElement)
		{
			TiXmlElement* itemElement = itemListElement->FirstChildElement("reward");
			while (NULL != itemElement)
			{
				ItemConfigData data;
				if (!data.ReadConfigNoCheck(itemElement))
				{
					return -3;;
				}
				cfg.reward_list.push_back(data);
				itemElement = itemElement->NextSiblingElement();
			}
		}

		m_win_box_reward_cfg_list.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int UTAConfig::InitRankRewardCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		UTARankCfg cfg;
		if (!GetSubNodeValue(dataElement, "type", cfg.type) || cfg.type < 0 || cfg.type > 1)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "min_rank", cfg.min_rank))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "max_rank", cfg.max_rank) || cfg.min_rank > cfg.max_rank)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "title_id", cfg.title_id))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "title_item", cfg.title_item_id))
		{
			return -44;
		}

		{
			TiXmlElement* itemlistElement = dataElement->FirstChildElement("reward_list");
			if (NULL != itemlistElement)
			{
				TiXmlElement* itemElement = itemlistElement->FirstChildElement("reward");
				while (NULL != itemElement)
				{
					ItemConfigData item;
					if (!item.ReadConfigNoCheck(itemElement))
					{
						return -5;
					}
					cfg.reward_list.push_back(item);

					itemElement = itemElement->NextSiblingElement();
				}
			}
		}
		
		{
			TiXmlElement* itemlistElement = dataElement->FirstChildElement("service_reward_list");
			if (NULL != itemlistElement)
			{
				TiXmlElement* itemElement = itemlistElement->FirstChildElement("service_reward");
				while (NULL != itemElement)
				{
					ItemConfigData item;
					if (!item.ReadConfigNoCheck(itemElement))
					{
						return -6;
					}
					ItemDataWrapper wrapper;
					wrapper.item_id = item.item_id;
					wrapper.num = item.num;
					wrapper.is_bind = item.is_bind ? 1 : 0;
					cfg.server_reward_list.push_back(wrapper);

					itemElement = itemElement->NextSiblingElement();
				}
			}
		}

		m_rank_reward_cfg_list.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int UTAConfig::InitAdvanceRewardCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		UTAAdvanceRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "type", cfg.type) || cfg.type < 0 || cfg.type > 1)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "stage", cfg.stage))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "stage_name", cfg.stage_name))
		{
			return -3;
		}

		TiXmlElement* itemlistElement = dataElement->FirstChildElement("reward_list");
		if (NULL != itemlistElement)
		{
			TiXmlElement* itemElement = itemlistElement->FirstChildElement("reward");
			while (NULL != itemElement)
			{
				ItemConfigData item;
				if (!item.ReadConfigNoCheck(itemElement))
				{
					return -5;
				}
				cfg.reward_list.push_back(item);

				itemElement = itemElement->NextSiblingElement();
			}
		}

		m_advance_reward_cfg_list.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int UTAConfig::InitInitialScoreCfg(TiXmlElement* RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	m_initial_score_vec.reserve(10);
	int last_rank = 0;
	int last_score = MAX_INT;
	while (dataElement)
	{
		int rank = 0;
		if (!GetSubNodeValue(dataElement, "rank", rank) || rank != last_rank + 1)
		{
			return -1;
		}
		last_rank = rank;

		int score = 0;
		if (!GetSubNodeValue(dataElement, "default_integral", score) || last_score < score)
		{
			return -2;
		}
		last_score = score;

		m_initial_score_vec.push_back(score);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

const UTAWinBoxRewardCfg* UTAConfig::GetWinBoxRewardCfg(int box_seq)
{
	for (size_t i = 0; i < m_win_box_reward_cfg_list.size(); ++i)
	{
		if (box_seq == m_win_box_reward_cfg_list[i].seq)
		{
			return &m_win_box_reward_cfg_list[i];
		}
	}

	return NULL;
}

std::vector<const UTAWinBoxRewardCfg*> UTAConfig::GetAllWinBoxRewardCfg()
{
	std::vector<const UTAWinBoxRewardCfg*> ret_vec;
	ret_vec.reserve(m_win_box_reward_cfg_list.size());

	for (size_t i = 0; i < m_win_box_reward_cfg_list.size(); ++i)
	{
		ret_vec.push_back(&m_win_box_reward_cfg_list[i]);
	}

	return ret_vec;
}

const UTARankCfg* UTAConfig::GetRankCfg(int type, int rank_index)
{
	for (int i = 0; i < m_rank_reward_cfg_list.size(); ++i)
	{
		const UTARankCfg* cfg = &m_rank_reward_cfg_list[i];
		if (type == cfg->type && (rank_index + 1) >= cfg->min_rank && (rank_index + 1) <= cfg->max_rank)
		{
			return cfg;
		}
	}

	return NULL;
}

const UTAAdvanceRewardCfg* UTAConfig::GetAdvanceCfg(int type, int round_idx)
{
	for (int i = 0; i < m_advance_reward_cfg_list.size(); ++i)
	{
		const UTAAdvanceRewardCfg* cfg = &m_advance_reward_cfg_list[i];
		if (type == cfg->type && round_idx == cfg->stage)
		{
			return cfg;
		}
	}

	return NULL;
}

int UTAConfig::GetInitialScore(int world_team_arena_rank) const
{
	if (world_team_arena_rank < 0 || world_team_arena_rank >= m_initial_score_vec.size())
	{
		assert(!m_initial_score_vec.empty());
		return m_initial_score_vec[m_initial_score_vec.size() - 1];
	}
	else
	{
		return m_initial_score_vec[world_team_arena_rank];
	}
}

UTAConfig::~UTAConfig()
{

}

