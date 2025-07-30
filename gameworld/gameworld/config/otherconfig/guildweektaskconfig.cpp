#include "guildweektaskconfig.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

GuildWeekTaskConfig::GuildWeekTaskConfig()
{
}

GuildWeekTaskConfig::~GuildWeekTaskConfig()
{
	std::vector<GWTDayRankReawardCfg>().swap(m_gwt_day_rank_reward_cfg);
	std::vector<GWTWeekRankReawardCfg>().swap(m_gwt_week_rank_reward_cfg);
}

bool GuildWeekTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitGWTOtherInfoCfg);
	LOAD_CONFIG("mission", InitGWTTaskInforCfg);
	LOAD_CONFIG("reward", InitGWTWeekAwardCfg);
	LOAD_CONFIG("gongxian_reward", InitGWTContAwardCfg);
	LOAD_CONFIG("gongxian_reward_day", InitGWTDayContRankRewardCfg);
	LOAD_CONFIG("gongxian_reward_week", InitGWTWeekContRankRewardCfg);

	return true;
}

int GuildWeekTaskConfig::RandGuildWeekTask(int patriarch_level)
{
	std::vector<int> tmp_task_id_vec;
	for (std::map<int, GWTTaskInfor>::iterator task_it = m_gwt_task_infor_map.begin(); task_it != m_gwt_task_infor_map.end(); ++task_it)
	{
		if (patriarch_level >= task_it->second.min_level)
		{
			tmp_task_id_vec.push_back(task_it->first);
		}
	}

	if ((int)tmp_task_id_vec.size() <= 0)
	{
		return 0;
	}

	int rand_idx = rand() % (int)tmp_task_id_vec.size();
	return tmp_task_id_vec[rand_idx];
}

const GWTTaskInfor * GuildWeekTaskConfig::GetGWTTaskInforCfg(int task_id)
{
	std::map<int, GWTTaskInfor>::iterator find_it = m_gwt_task_infor_map.find(task_id);
	if (m_gwt_task_infor_map.end() != find_it) return & find_it->second;

	return NULL;
}

const GWTWeekAward * GuildWeekTaskConfig::GetGWTWeekAwardCfg(int finish_times)
{
	std::map<int, GWTWeekAward>::iterator find_it = m_gwt_week_award_map.find(finish_times);
	if (m_gwt_week_award_map.end() != find_it) return & find_it->second;

	return NULL;
}

const ItemConfigData * GuildWeekTaskConfig::GetContributeAward(int contribution_value)
{
	for (std::map<int, ItemConfigData>::reverse_iterator loop_it = m_gwt_contribute_award_map.rbegin(); loop_it != m_gwt_contribute_award_map.rend(); ++loop_it)
	{
		if (contribution_value >= loop_it->first) return & loop_it->second;
	}

	return NULL;
}

const GWTDayRankReawardCfg * GuildWeekTaskConfig::GetGWTDayRankReawardCfg(int rank)
{
	for (int i = 0; i < (int)m_gwt_day_rank_reward_cfg.size(); ++i)
	{
		GWTDayRankReawardCfg & cfg = m_gwt_day_rank_reward_cfg[i];
		if (cfg.min_rank <= rank && rank <= cfg.max_rank)
		{
			return &cfg;
		}
	}

	return NULL;
}

const GWTWeekRankReawardCfg * GuildWeekTaskConfig::GetGWTWeekRankReawardCfg(int rank)
{
	for (int i = 0; i < (int)m_gwt_week_rank_reward_cfg.size(); ++i)
	{
		GWTWeekRankReawardCfg & cfg = m_gwt_week_rank_reward_cfg[i];
		if (cfg.min_rank <= rank && rank <= cfg.max_rank)
		{
			return &cfg;
		}
	}

	return NULL;
}

int GuildWeekTaskConfig::InitGWTOtherInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	if (!GetSubNodeValue(root_element, "reward_day", m_gwt_other_cfg.reward_day) || m_gwt_other_cfg.reward_day < 0 || m_gwt_other_cfg.reward_day > 6)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "reward_time", m_gwt_other_cfg.reward_time) || !this->IsTimerCfg(m_gwt_other_cfg.reward_time))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "refresh_time", m_gwt_other_cfg.daily_refresh_time) || !this->IsTimerCfg(m_gwt_other_cfg.daily_refresh_time))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "family_coin", m_gwt_other_cfg.day_fin_fund_award) || m_gwt_other_cfg.day_fin_fund_award < 0)
	{
		return -4;
	}

	return 0;
}

int GuildWeekTaskConfig::InitGWTTaskInforCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int task_id = 0; GWTTaskInfor tmp_con;
		if (!GetSubNodeValue(root_element, "mission_id", task_id) || task_id <= 0 || m_gwt_task_infor_map.end() != m_gwt_task_infor_map.find(task_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "mission_type", tmp_con.task_condition) || tmp_con.task_condition <= 0 || tmp_con.task_condition >= GWT_COND_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param_1", tmp_con.param1))
		{
			return -3;
		}

		UNSTD_STATIC_CHECK(7 == GWT_COND_TYPE_MAX)
		switch (tmp_con.task_condition)
		{
		case GWT_COND_KILL_RACE_MONSTER:
			if (tmp_con.param1 < RACE_TYPE_MIN || tmp_con.param1 >= RACE_TYPE_MAX) return -101;
			break;
		case GWT_COND_ITEM_TYPE_GATHERS:
			if (tmp_con.param1 < 0 || tmp_con.param1 >= GATHER_TYPE_MAX) return -201;
			break;
		}

		if (!GetSubNodeValue(root_element, "param_2", tmp_con.need_prog) || tmp_con.need_prog <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "gongxian", tmp_con.contribution_per_times) || tmp_con.contribution_per_times < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "min_level", tmp_con.min_level) || tmp_con.min_level <= 0 || tmp_con.min_level > MAX_ROLE_LEVEL)
		{
			return -6;
		}


		m_gwt_task_infor_map[task_id] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildWeekTaskConfig::InitGWTWeekAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int finish_times = 0; GWTWeekAward tmp_con;
		if (!GetSubNodeValue(root_element, "times", finish_times) || finish_times < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin_award) || tmp_con.coin_award < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.exp_award) || tmp_con.exp_award < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "gongxian", tmp_con.guild_contribution) || tmp_con.guild_contribution < 0)
		{
			return -4;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL != item_node)		// 允许没有道具奖励
		{
			if (!tmp_con.item_award.ReadConfig(item_node))
			{
				return -5;
			}
		}

		m_gwt_week_award_map[finish_times] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildWeekTaskConfig::InitGWTContAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int contribute_value = 0; ItemConfigData reward_item;
		if (!GetSubNodeValue(root_element, "gongxian_limit", contribute_value) || contribute_value < 0)
		{
			return -1;
		}

		if (m_gwt_contribute_award_map.end() != m_gwt_contribute_award_map.find(contribute_value))
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("gongxian_reward");
		if (NULL == item_node) return -20000;

		if (!reward_item.ReadConfig(item_node))
		{
			return -3;
		}

		m_gwt_contribute_award_map[contribute_value] = reward_item;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildWeekTaskConfig::InitGWTDayContRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_role_level = 0;
	while (NULL != root_element)
	{
		GWTDayRankReawardCfg cfg;
		if (!GetSubNodeValue(root_element, "min_rank", cfg.min_rank) || cfg.min_rank != last_max_role_level + 1)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "max_rank", cfg.max_rank) || cfg.max_rank < cfg.min_rank)
		{
			return -__LINE__;
		}

		last_max_role_level = cfg.max_rank;

		if (!GetSubNodeValue(root_element, "gongxian", cfg.guild_gong_xian) || cfg.guild_gong_xian < 0)
		{
			return -__LINE__;
		}

		m_gwt_day_rank_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int GuildWeekTaskConfig::InitGWTWeekContRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_role_level = 0;
	while (NULL != root_element)
	{
		GWTWeekRankReawardCfg cfg;
		if (!GetSubNodeValue(root_element, "min_rank", cfg.min_rank) || cfg.min_rank != last_max_role_level + 1)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "max_rank", cfg.max_rank) || cfg.max_rank < cfg.min_rank)
		{
			return -__LINE__;
		}
		last_max_role_level = cfg.max_rank;

		TiXmlElement * reward_list_element = root_element->FirstChildElement("gongxian_reward_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("gongxian_reward");

			while (reward_element)
			{
				ItemConfigData reward;

				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}

				cfg.reward.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_gwt_week_rank_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

bool GuildWeekTaskConfig::IsTimerCfg(int check_time)
{
	int _hour = check_time / 100;
	int _min = check_time % 100;
	return _hour >= 0 && _hour < 24 && _min >= 0 && _min < 60;
}