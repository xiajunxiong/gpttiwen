#include "activityguildhonorbattleconfig.hpp"

ActivityGuildHonorBattleConfig::ActivityGuildHonorBattleConfig()
{
}

ActivityGuildHonorBattleConfig::~ActivityGuildHonorBattleConfig()
{
}

bool ActivityGuildHonorBattleConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("act_reward", InitRewardCfg);
	LOAD_CONFIG("guess_reward", InitGuessCfg);
	LOAD_CONFIG("pos_group", InitBirthPosCfg);
	LOAD_CONFIG("rank_reward", InitRankRewardCfg);

	return true;
}

const ActivityGuildHonorBattleOtherCfg & ActivityGuildHonorBattleConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const ActivityGuildHonorBattleRewardCfg * ActivityGuildHonorBattleConfig::GetActivityReward(int reward_type)
{
	std::map<int, ActivityGuildHonorBattleRewardCfg>::iterator iter = m_reward_cfg.find(reward_type);
	if (iter == m_reward_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const ActivityGuildHonorBattleGuessCfg * ActivityGuildHonorBattleConfig::GetActivityGuildHonorBattleGuessCfg(int type)
{
	std::map<int, ActivityGuildHonorBattleGuessCfg>::iterator iter = m_guess_cfg.find(type);
	if (iter == m_guess_cfg.end())
	{
		return NULL;
	}

	return &iter->second;
}

const ActivityGuildHonorBattleRankRewardCfg * ActivityGuildHonorBattleConfig::GetActivityGuildHonorBattleRankRewardCfg(int rank)
{
	std::map<int, ActivityGuildHonorBattleRankRewardCfg>::iterator iter = m_rank_reward_cfg.find(rank);
	if (iter != m_rank_reward_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

std::vector<int> ActivityGuildHonorBattleConfig::GetGuildHonorRankkbuff(int rank)
{
	const ActivityGuildHonorBattleRankRewardCfg * cfg = this->GetActivityGuildHonorBattleRankRewardCfg(rank);
	if (cfg)
	{
		return cfg->buff_id;
	}

	return std::vector<int>();
}

int ActivityGuildHonorBattleConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_rounds", m_other_cfg.max_battle_rounds))
	{
		return -1;
	}


	if (!GetSubNodeValue(root_element, "map", m_other_cfg.scene_id))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "list_publish_day", m_other_cfg.settlement_list_time_wday) || m_other_cfg.settlement_list_time_wday < 0)
	{
		return -3;
	}

	m_other_cfg.settlement_list_time_wday %= 7;

	int hour_min = 0;
	if (!GetSubNodeValue(root_element, "list_publish_time", hour_min) || hour_min < 0)
	{
		return -4;
	}

	m_other_cfg.settlement_list_time_hour = hour_min / 100;
	m_other_cfg.settlement_list_time_min = hour_min % 100;

	if (m_other_cfg.settlement_list_time_hour >= 24)
	{
		return -5;
	}

	if (m_other_cfg.settlement_list_time_min >= 60)
	{
		return -6;
	}

	TiXmlElement * list_node = root_element->FirstChildElement("integral_record_time");
	if (NULL != list_node)
	{
		TiXmlElement * item_node = list_node->FirstChildElement("node");
		if (NULL == item_node) return -100;

		while (NULL != item_node)
		{
			unsigned int wday = 0;
			if (!GetNodeValue(item_node, wday))
			{
				return -77;
			}

			wday %= 7;

			m_other_cfg.settlement_list_day |= 1 << wday;
			item_node = item_node->NextSiblingElement();
		}
	}

	return 0;
}

int ActivityGuildHonorBattleConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "result_type", reward_type))
		{
			return -1;
		}

		ActivityGuildHonorBattleRewardCfg & cfg = m_reward_cfg[reward_type];
		if (!GetSubNodeValue(root_element, "gongxian", cfg.gong_xian))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "bind_coin", cfg.coin))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "family_coin", cfg.guild_coin))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "title", cfg.title_id))
		{
			return -5;
		}

		TiXmlElement * reward_list_ele = root_element->FirstChildElement("reward_list");
		if (NULL == reward_list_ele)
		{
			return -103;
		}
		TiXmlElement * reward_ele = reward_list_ele->FirstChildElement("reward");
		while (reward_ele != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_ele))
			{
				return -104;
			}
			cfg.items.push_back(reward);
			reward_ele = reward_ele->NextSiblingElement();
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildHonorBattleConfig::InitGuessCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type))
		{
			return -1;
		}

		ActivityGuildHonorBattleGuessCfg & cfg = m_guess_cfg[reward_type];
		if (!GetSubNodeValue(root_element, "right_coin", cfg.right_coin))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "right_family_coin", cfg.right_gong_xian))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "wrong_coin", cfg.wrong_coin))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "wrong_family_coin", cfg.wrong_gong_xian))
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildHonorBattleConfig::InitBirthPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int first_pos_group = -1;
	while (NULL != root_element)
	{
		int pos_group = 0;
		if (!GetSubNodeValue(root_element, "pos_group", pos_group))
		{
			return -1;
		}

		if (first_pos_group == -1)
		{
			first_pos_group = pos_group;
		}

		Posi pos;
		if (!GetSubNodeValue(root_element, "x_pos", pos.x))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "y_pos", pos.y))
		{
			return -3;
		}

		if (first_pos_group == pos_group)
		{
			m_other_cfg.ack_pos.push_back(pos);
		}
		else
		{
			m_other_cfg.def_pos.push_back(pos);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildHonorBattleConfig::InitRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ActivityGuildHonorBattleRankRewardCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.rank))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "winner_coin", cfg.coin) || cfg.coin < 0)
		{
			return -3;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("reward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		for (int i = 0; i < 2; ++i)
		{
			std::string s = STRING_SPRINTF("buff_reward_%d", i + 1);

			int buff_id = 0;
			if (!GetSubNodeValue(root_element, s, buff_id))
			{
				return -__LINE__;
			}

			cfg.buff_id.push_back(buff_id);
		}

		if (!GetSubNodeValue(root_element, "low_winner_coin", cfg.low_winner_coin) || cfg.low_winner_coin < 0)
		{
			return -4;
		}

		TiXmlElement * low_item_list_element = root_element->FirstChildElement("low_reward_list");
		if (NULL != low_item_list_element)
		{
			TiXmlElement * low_item_element = low_item_list_element->FirstChildElement("low_reward");
			while (NULL != low_item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(low_item_element))
				{
					return -__LINE__;
				}

				cfg.low_rewards.push_back(item);
				low_item_element = low_item_element->NextSiblingElement();
			}
		}

		m_rank_reward_cfg[cfg.rank] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

Posi ActivityGuildHonorBattleOtherCfg::GetAckPos() const
{
	if (ack_pos.empty())
	{
		return Posi();
	}

	int r = rand() % ack_pos.size();

	return ack_pos[r];
}

Posi ActivityGuildHonorBattleOtherCfg::GetDefPos() const
{
	if (def_pos.empty())
	{
		return Posi();
	}

	int r = rand() % def_pos.size();

	return def_pos[r];
}

bool ActivityGuildHonorBattleOtherCfg::IsVaildDay(int wday) const
{
	if (wday < 0 || wday >= 7)
	{
		return false;
	}

	return (int)settlement_list_day & (1 << wday);
}

int ActivityGuildHonorBattleOtherCfg::GetLastVaildDay() const
{
	for (int i = 7; i > 0; --i)
	{
		int wday = i % 7;
		if (this->IsVaildDay(wday))
		{
			return wday;
		}
	}

	return 0;
}
