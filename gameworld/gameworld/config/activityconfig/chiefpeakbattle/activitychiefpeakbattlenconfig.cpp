#include "activitychiefpeakbattlenconfig.hpp"

ActivityChiefPeakBattleConfig::ActivityChiefPeakBattleConfig()
{
}

ActivityChiefPeakBattleConfig::~ActivityChiefPeakBattleConfig()
{
	std::vector<ChiefPeakBattleReadySceneCfg>().swap(m_ready_scene_cfg);
	std::vector<ChiefPeakBattleBetCfg>().swap(m_bet_cfg);
	std::vector<ChiefPeakBattleRankRewardCfg>().swap(m_rank_cfg);
	std::vector<ActivityChiefPeakBattleBattleRewardCfg>().swap(m_battle_reward_cfg);
}

bool ActivityChiefPeakBattleConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("waiting_room", InitSceneCfg);
	LOAD_CONFIG("bet_reward", InitBetCfg);
	LOAD_CONFIG("rank_reward", InitRandRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("battle_reward", InitBattleRewardCfg);

	return true;
}

const ChiefPeakBattleRankRewardCfg * ActivityChiefPeakBattleConfig::GetChiefPeakBattleRewardCfg(int round)
{
	for (int i = (int)m_rank_cfg.size() - 1; i >= 0; --i)
	{
		if (round >= m_rank_cfg[i].round)
		{
			return &m_rank_cfg[i];
		}
	}

	return NULL;
}

const ChiefPeakBattleBetCfg * ActivityChiefPeakBattleConfig::GetChiefPeakBattleBetCfg(int bet_seq)
{
	if (bet_seq < 0 || bet_seq >= (int)m_bet_cfg.size())
	{
		return NULL;
	}

	return &m_bet_cfg[bet_seq];
}

int ActivityChiefPeakBattleConfig::GetReadySceneId(int round)
{
	int index = round - 1;

	if (index < 0 || index >= (int)m_ready_scene_cfg.size())
	{
		return 0;
	}

	return m_ready_scene_cfg[index].scene_id;
}

const ChiefPeakBattleOtherCfg & ActivityChiefPeakBattleConfig::GetOhterCfg()
{
	return m_other_cfg;
}

const ActivityChiefPeakBattleBattleRewardCfg * ActivityChiefPeakBattleConfig::GetPerBattleRewardCfg(int role_level)
{
	for (int i = 0; i < (int)m_battle_reward_cfg.size(); ++i)
	{
		ActivityChiefPeakBattleBattleRewardCfg & cfg = m_battle_reward_cfg[i];
		if (cfg.min_role_level <= role_level && role_level <= cfg.max_role_level)
		{
			return &cfg;
		}
	}

	return NULL;
}

int ActivityChiefPeakBattleConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ChiefPeakBattleReadySceneCfg cfg;
		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id))
		{
			return -1;
		}

		m_ready_scene_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_ready_scene_cfg.size() == 0)
	{
		return -889;
	}

	return 0;
}

int ActivityChiefPeakBattleConfig::InitBetCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		ChiefPeakBattleBetCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.bet_seq) || cfg.bet_seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "bet_coin", cfg.bet_coin) || cfg.bet_coin <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "reward_coin", cfg.win_coin) || cfg.win_coin <= 0)
		{
			return -3;
		}

		m_bet_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityChiefPeakBattleConfig::InitRandRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_round = 1;
	while (NULL != root_element)
	{
		ChiefPeakBattleRankRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.round) || cfg.round != next_round)
		{
			return -1;
		}
		next_round += 1;

		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "winner_coin", cfg.coin) || cfg.coin < 0)
		{
			return -3;
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
				cfg.reward_list.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_rank_cfg.push_back(cfg);
		if (cfg.title_id > 0)
		{
			m_chief_peak_battle_rank_title_set.insert(cfg.title_id);
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityChiefPeakBattleConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_round", m_other_cfg.max_round) || m_other_cfg.max_round <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "countdown", m_other_cfg.first_ready_time) || m_other_cfg.first_ready_time <= 0)
	{
		return -2;
	}


	return 0;
}

int ActivityChiefPeakBattleConfig::InitBattleRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_role_level = 0;
	while (NULL != root_element)
	{
		ActivityChiefPeakBattleBattleRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "min_level", cfg.min_role_level) || cfg.min_role_level <= last_max_role_level)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "max_level", cfg.max_role_level) || cfg.max_role_level < cfg.min_role_level)
		{
			return -__LINE__;
		}

		last_max_role_level = cfg.max_role_level;

		if (!GetSubNodeValue(root_element, "winner_coin", cfg.winner_coin) || cfg.winner_coin < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "winner_exp", cfg.winner_exp) || cfg.winner_exp < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "loser_coin", cfg.loser_coin) || cfg.loser_coin < 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "loser_exp", cfg.loser_exp) || cfg.loser_exp < 0)
		{
			return -__LINE__;
		}


		m_battle_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
