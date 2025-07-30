#include "randactivitygodtokenconfig.hpp"

RandActivityGodTokenConfig::RandActivityGodTokenConfig()
{
	memset(m_time_cfg, 0, sizeof(m_time_cfg));
}

RandActivityGodTokenConfig::~RandActivityGodTokenConfig()
{
	for (std::map<int, std::vector<RAGodTokenLevelSectionCfg> >::iterator it = m_level_cfg.begin(); it != m_level_cfg.end(); it++)
	{
		std::vector<RAGodTokenLevelSectionCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<RAGodTokenExchangeCfg> >::iterator it = m_exchange_cfg.begin(); it != m_exchange_cfg.end(); it++)
	{
		std::vector<RAGodTokenExchangeCfg>().swap(it->second);
	}
}

bool RandActivityGodTokenConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitRAGodTokenOtherCfg);
	LOAD_RA_CONFIG("dailytask", InitRAGodTokenTaskCfg);
	LOAD_RA_CONFIG("grade_reward", InitRAGodTokenLevelCfg);
	LOAD_RA_CONFIG("itemexchange", InitRAGodTokenExchangeCfg);
	LOAD_RA_CONFIG("reset_time", InitRAGodTokenTimeCfg);
	LOAD_RA_CONFIG("weeklytasks", InitRAGodTokenWeekTaskCfg);
	LOAD_RA_CONFIG("surplus_reward", InitRAGodTokenSurplusCfg);

	return true;
}

const RAGodTokenTaskCfg * RandActivityGodTokenConfig::GetTaskCfg(int time_seq, int level, int type) const
{
	if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return NULL;

	for (int i = 0; i < GOD_TOKEN_LEVEL_SEGMENT; ++i)
	{
		if (m_daily_task_cfg[time_seq][i].level_min <= level && level <= m_daily_task_cfg[time_seq][i].level_max)
		{
			std::vector<RAGodTokenTaskCfg>::const_iterator it = m_daily_task_cfg[time_seq][i].task_list.begin();
			for (; it != m_daily_task_cfg[time_seq][i].task_list.end(); ++it)
			{
				if (it->type == type)
				{
					return &(*it);
				}
			}
		}
	}

	return NULL;
}

const RAGodTokenLevelCfg * RandActivityGodTokenConfig::GetLevelCfg(int time_seq, int seq, int level) const
{
	if(seq < 0) return NULL;

	std::map<int, std::vector<RAGodTokenLevelSectionCfg> >::const_iterator it = m_level_cfg.find(time_seq);
	if (m_level_cfg.end() == it || it->second.empty())
	{
		return NULL;
	}

	for (int i = 0; i < (int)it->second.size(); ++i)
	{
		if (it->second[i].open_level <= level && level <= it->second[i].end_level && seq < (int)it->second[i].level_vec.size())
		{
			return &it->second[i].level_vec[seq];
		}
	}

	return NULL;
}

const RAGodTokenExchangeCfg * RandActivityGodTokenConfig::GetExchangeCfg(int time_seq, int seq) const
{
	std::map<int, std::vector<RAGodTokenExchangeCfg> >::const_iterator it = m_exchange_cfg.find(time_seq);
	if (m_exchange_cfg.end() == it)
	{
		return NULL;
	}

	std::vector<RAGodTokenExchangeCfg>::const_iterator vit = it->second.begin();
	for (; vit != it->second.end(); ++vit)
	{
		if (seq == vit->seq)
		{
			return &(*vit);
		}
	}

	return NULL;
}

const RAGodTokenTaskCfg * RandActivityGodTokenConfig::GetWeekTaskCfg(int time_seq, int level, int type) const
{
	if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return NULL;

	for (int i = 0; i < GOD_TOKEN_LEVEL_SEGMENT; ++i)
	{
		if (m_weekly_task_cfg[time_seq][i].level_min <= level && level <= m_weekly_task_cfg[time_seq][i].level_max)
		{
			std::vector<RAGodTokenTaskCfg>::const_iterator it = m_weekly_task_cfg[time_seq][i].task_list.begin();
			for (; it != m_weekly_task_cfg[time_seq][i].task_list.end(); ++it)
			{
				if (it->type == type)
				{
					return &(*it);
				}
			}
		}
	}

	return NULL;
}

const unsigned int RandActivityGodTokenConfig::GetTimeCfg(int seq) const
{
	if (0 > seq || seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return 0;

	return m_time_cfg[seq];
}

const int RandActivityGodTokenConfig::GetTimeSeqCfg(unsigned int timestamp) const
{
	int ret_seq = 0;
	for (int i = 0; i < GOD_TOKEN_TIME_SEQ_MAXNUM; ++i)
	{
		if(0 == m_time_cfg[i]) continue;

		if (m_time_cfg[i] <= timestamp)
		{
			ret_seq = i;
		}
		else break;
	}
	return ret_seq;
}

const int RandActivityGodTokenConfig::GetMinOpenLevel(int time_seq) const
{
	if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return 0;

	int ret_level = INT_MAX;
	for (int i = 0; i < GOD_TOKEN_LEVEL_SEGMENT; ++i)
	{
		if (ret_level > m_daily_task_cfg[time_seq][i].level_min && 0 < m_daily_task_cfg[time_seq][i].level_min)
		{
			ret_level = m_daily_task_cfg[time_seq][i].level_min;
		}
	}

	return ret_level;
}

const int RandActivityGodTokenConfig::GetExchangeCfgLen(int time_seq) const
{
	std::map<int, std::vector<RAGodTokenExchangeCfg> >::const_iterator it = m_exchange_cfg.find(time_seq);
	if (m_exchange_cfg.end() == it)
	{
		return 0;
	}

	return (int)it->second.size();
}

void RandActivityGodTokenConfig::RandDailyTask(int time_seq, int level, char* type_list)const
{
	if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return;

	for (int i = 0; i < GOD_TOKEN_LEVEL_SEGMENT; ++i)
	{
		if (m_daily_task_cfg[time_seq][i].level_min <= level && level <= m_daily_task_cfg[time_seq][i].level_max)
		{
			//不在随机 改为每天固定六个
			/*std::vector<RAGodTokenTaskCfg> cfg;
			cfg.assign(m_daily_task_cfg[time_seq][i].task_list.begin(), m_daily_task_cfg[time_seq][i].task_list.end());
			std::random_shuffle(cfg.begin(), cfg.end());*/

			int count = 0;
			std::vector<RAGodTokenTaskCfg>::const_iterator it = m_daily_task_cfg[time_seq][i].task_list.begin();
			for (; it != m_daily_task_cfg[time_seq][i].task_list.end() && count < RA_GOD_TOKEN_DAILY_TASK_NUM; ++it)
			{
				type_list[count++] = (char)it->type;
			}
			return;
		}
	}
}

void RandActivityGodTokenConfig::RandWeeklyTask(int time_seq, int level, char* type_list)const
{
	if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return;

	for (int i = 0; i < GOD_TOKEN_LEVEL_SEGMENT; ++i)
	{
		if (m_weekly_task_cfg[time_seq][i].level_min <= level && level <= m_weekly_task_cfg[time_seq][i].level_max)
		{
			std::vector<RAGodTokenTaskCfg> cfg;
			cfg.assign(m_weekly_task_cfg[time_seq][i].task_list.begin(), m_weekly_task_cfg[time_seq][i].task_list.end());
			std::random_shuffle(cfg.begin(), cfg.end());

			int count = 0;
			for (std::vector<RAGodTokenTaskCfg>::const_iterator it = cfg.begin(); it != cfg.end() && count < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++it)
			{
				type_list[count++] = (char)it->type;
			}
			return;
		}
	}
}

int RandActivityGodTokenConfig::InitRAGodTokenOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RAGodTokenOtherCfg &cfg = m_other_cfg;

		if (!GetSubNodeValue(data_element, "buy_exp", cfg.buy_exp) || cfg.buy_exp <= 0) return -1;
		if (!GetSubNodeValue(data_element, "buy_senior", cfg.buy_token) || cfg.buy_token <= 0) return -2;
		if (!GetSubNodeValue(data_element, "grade_time", cfg.max_leve) || cfg.max_leve <= 0) return -3;
		if (!GetSubNodeValue(data_element, "retrieve_item", cfg.reclaim_item) || !ITEMPOOL->GetItem(cfg.reclaim_item)) return -4;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenTaskCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int level_idx = -1;
	int old_time_seq = -1;
	int old_min_level = 0, old_max_level = 0;

	while (NULL != data_element)
	{
		bool time_seq_change = false;
		int time_seq, min_level, max_level = 0;
		if (!GetSubNodeValue(data_element, "time_seq", time_seq) || time_seq < 0) return -1;
		if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return -100;
		if (old_time_seq != time_seq)
		{
			old_time_seq = time_seq;
			level_idx = 0;
			time_seq_change = true;
		}

		if (!GetSubNodeValue(data_element, "grade_start", min_level) || min_level <= 0) return -2;
		if (!GetSubNodeValue(data_element, "grade_end", max_level) || max_level < 0) return -3;
		if (0 == max_level) max_level = INT_MAX;

		bool level_change = false;
		if (old_min_level != min_level || old_max_level != max_level)
		{
			old_min_level = min_level;
			old_max_level = max_level;
			level_change = true;
		}
		if (!time_seq_change && level_change)
		{
			level_idx++;
		}

		if (0 > level_idx || level_idx >= GOD_TOKEN_LEVEL_SEGMENT) return -200;

		RAGodTokenTaskListCfg & node = m_daily_task_cfg[time_seq][level_idx];
		RAGodTokenTaskCfg cfg;

		node.level_min = min_level;
		node.level_max = max_level;
		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0) return -4;
		if (!GetSubNodeValue(data_element, "type", cfg.type) || cfg.type < 0) return -5;
		if (!GetSubNodeValue(data_element, "parameter", cfg.param) || cfg.param < 0) return -6;
		if (!GetSubNodeValue(data_element, "open_exp", cfg.get_exp) || cfg.get_exp < 0) return -7;

		node.task_list.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenLevelCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int time_seq = 0;
		if (!GetSubNodeValue(data_element, "time_seq", time_seq) || time_seq < 0) return -1;

		int open_level = 0, end_level = 0;
		if (!GetSubNodeValue(data_element, "open_level", open_level) || open_level < 0) return -50;
		if (!GetSubNodeValue(data_element, "end_level", end_level) || end_level < open_level || end_level > MAX_ROLE_LEVEL) return -51;

		std::vector<RAGodTokenLevelSectionCfg> & l_cfg = m_level_cfg[time_seq];

		if (l_cfg.empty())
		{
			l_cfg.push_back(RAGodTokenLevelSectionCfg());
		}
		else
		{
			int pre_index = (int)l_cfg.size() - 1;
			if (open_level != l_cfg[pre_index].open_level ||
				end_level != l_cfg[pre_index].end_level)
			{
				if (open_level < l_cfg[pre_index].end_level)
				{
					return -666;
				}
				l_cfg.push_back(RAGodTokenLevelSectionCfg());
			}
		}
		
		RAGodTokenLevelSectionCfg & node_cfg = l_cfg[(int)l_cfg.size() - 1];
		node_cfg.open_level = open_level;
		node_cfg.end_level = end_level;

		RAGodTokenLevelCfg cfg;

		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0 || cfg.seq != (int)node_cfg.level_vec.size()) return -2;
		if (!GetSubNodeValue(data_element, "open_exp", cfg.need_exp) || cfg.need_exp <= 0) return -3;
		if (!GetSubNodeValue(data_element, "open_grade", cfg.open_level) || cfg.open_level <= 0) return -4;
		if (!GetSubNodeValue(data_element, "senior_gold_num", cfg.senior_gold) || cfg.senior_gold < 0) return -5;

		TiXmlElement * item_element = data_element->FirstChildElement("ordinary_item");
		if (NULL != item_element)
		{
			if (!cfg.common_item.ReadConfig(item_element)) return -100;
		}

		TiXmlElement * reward_list_element = data_element->FirstChildElement("senior_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("senior_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -500;
				}
				cfg.senior_item.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		node_cfg.level_vec.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenExchangeCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int time_seq = 0;
		if (!GetSubNodeValue(data_element, "time_seq", time_seq) || time_seq < 0) return -1;

		std::vector<RAGodTokenExchangeCfg> & e_cfg = m_exchange_cfg[time_seq];
		RAGodTokenExchangeCfg cfg;

		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0) return -2;
		if (!GetSubNodeValue(data_element, "buy_everyday", cfg.daily_exchange) || cfg.daily_exchange < 0) return -3;
		if (!GetSubNodeValue(data_element, "buy_reset", cfg.all_exchange) || cfg.all_exchange < 0) return -4;
		if (!GetSubNodeValue(data_element, "open_level", cfg.open_level) || cfg.open_level < 0) return -5;
		if (!GetSubNodeValue(data_element, "open_token", cfg.open_token) || cfg.open_token < 0) return -6;
		if (!GetSubNodeValue(data_element, "end_token", cfg.end_token) || cfg.end_token < 0) return -7;
		if (!GetSubNodeValue(data_element, "end_level", cfg.end_level) || cfg.end_level < 0) return -8;
		if (cfg.end_level <= 0)
		{
			cfg.end_level = INT_MAX;
		}

		TiXmlElement * item_element = data_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		TiXmlElement * use_item_element = data_element->FirstChildElement("item_use");
		if (NULL != use_item_element)
		{
			if (!cfg.consume_item.ReadConfig(use_item_element)) return -200;
		}

		e_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenTimeCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (0 > seq || seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return -100;

		if (!GetSubNodeValue(data_element, "reset_time", m_time_cfg[seq]) || m_time_cfg[seq] <= 0) return -2;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenWeekTaskCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int level_idx = -1;
	int old_time_seq = -1;
	int old_min_level = 0, old_max_level = 0;

	while (NULL != data_element)
	{
		bool time_seq_change = false;
		int time_seq, min_level, max_level = 0;
		if (!GetSubNodeValue(data_element, "time_seq", time_seq) || time_seq < 0) return -1;
		if (0 > time_seq || time_seq >= GOD_TOKEN_TIME_SEQ_MAXNUM) return -100;
		if (old_time_seq != time_seq)
		{
			old_time_seq = time_seq;
			level_idx = 0;
			time_seq_change = true;
		}

		if (!GetSubNodeValue(data_element, "grade_start", min_level) || min_level <= 0) return -2;
		if (!GetSubNodeValue(data_element, "grade_end", max_level) || max_level < 0) return -3;
		if (0 == max_level) max_level = INT_MAX;

		bool level_change = false;
		if (old_min_level != min_level || old_max_level != max_level)
		{
			old_min_level = min_level;
			old_max_level = max_level;
			level_change = true;
		}
		if (!time_seq_change && level_change)
		{
			level_idx++;
		}

		if (0 > level_idx || level_idx >= GOD_TOKEN_LEVEL_SEGMENT) return -200;

		RAGodTokenTaskListCfg & node = m_weekly_task_cfg[time_seq][level_idx];
		RAGodTokenTaskCfg cfg;

		node.level_min = min_level;
		node.level_max = max_level;
		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0) return -4;
		if (!GetSubNodeValue(data_element, "type", cfg.type) || cfg.type < 0) return -5;
		if (!GetSubNodeValue(data_element, "parameter", cfg.param) || cfg.param < 0) return -6;
		if (!GetSubNodeValue(data_element, "open_exp", cfg.get_exp) || cfg.get_exp < 0) return -7;

		node.task_list.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityGodTokenConfig::InitRAGodTokenSurplusCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RAGodTokenSurplusCfg &cfg = m_surplus_cfg;

		if (!GetSubNodeValue(data_element, "open_exp", cfg.open_exp) || cfg.open_exp <= 0) return -1;

		TiXmlElement * common_element = data_element->FirstChildElement("ordinary_item_list");
		if (common_element == NULL)
		{
			return -1000;
		}
		TiXmlElement * reward_element = common_element->FirstChildElement("ordinary_item");
		while (reward_element)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -100;
			}
			cfg.common_item.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		TiXmlElement * senior_element = data_element->FirstChildElement("senior_item_list");
		if (senior_element == NULL)
		{
			return -2000;
		}
		TiXmlElement * senior_item_element = senior_element->FirstChildElement("senior_item");
		while (senior_item_element)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(senior_item_element))
			{
				return -200;
			}
			cfg.senior_item.push_back(reward);
			senior_item_element = senior_item_element->NextSiblingElement();
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
