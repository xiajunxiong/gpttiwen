#include "randactivitygodbeastadventconfig.hpp"
#include "item/itempool.h"
#include "gameworld/engineadapter.h"

#include "servercommon/maildef.hpp"

RandActivityYiShouLinShiConfig::RandActivityYiShouLinShiConfig()
{
}

RandActivityYiShouLinShiConfig::~RandActivityYiShouLinShiConfig()
{
	std::vector<YSLSTaskSectionCfg>().swap(m_task_section_cfg);
	std::vector<YSLSExchangeSectionCfg>().swap(m_exchange_section_cfg);
	std::vector<YSLSFbSectionCfg>().swap(m_fb_cfg);
	std::vector<YSLSBuySectionCfg>().swap(m_buy_cfg);
	std::vector<YSLSCycleCfg>().swap(m_cycle_time_cfg);
}

bool RandActivityYiShouLinShiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("task_configure", InitTaskCfg);
	LOAD_RA_CONFIG("exchange_configure", InitExchangeCfg);
	LOAD_RA_CONFIG("secret_area_configure", InitFbCfg);
	LOAD_RA_CONFIG("buy_configure", InitBuyCfg);
	LOAD_RA_CONFIG("recycle_configure", InitRecycleCfg);
	LOAD_RA_CONFIG("times_configure", InitCycleCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityYiShouLinShiConfig::GetTaskParamCfg(int task_type) const
{
	std::map<int, int>::const_iterator it = m_task_param_cfg.find(task_type);
	if (m_task_param_cfg.end() != it) return it->second;

	return -1;
}

const YSLSTaskCfg * RandActivityYiShouLinShiConfig::GetTaskCfg(RandActivityManager * ramgr, int seq, int task_group) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);

	for (int i = 0; i < (int)m_task_section_cfg.size(); ++i)
	{
		if (m_task_section_cfg[i].section_start <= act_real_open_day && m_task_section_cfg[i].section_end >= act_real_open_day)
		{
			for (size_t j = 0; j < m_task_section_cfg[i].task_group_cfg.size(); j++)
			{
				if (m_task_section_cfg[i].task_group_cfg[j].task_group == task_group)
				{
					std::map<int, YSLSTaskCfg>::const_iterator it = m_task_section_cfg[i].task_group_cfg[j].task_cfg.find(seq);
					if (it != m_task_section_cfg[i].task_group_cfg[j].task_cfg.end()) return &it->second;
				}
			}
		}
	}
	return NULL;
}

const YSLSTaskRewardCfg * RandActivityYiShouLinShiConfig::GetTaskRewardCfg(RandActivityManager* ramgr, int seq, int task_group) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);

	for (int i = 0; i < (int)m_task_section_cfg.size(); ++i)
	{
		if (m_task_section_cfg[i].section_start <= act_real_open_day && m_task_section_cfg[i].section_end >= act_real_open_day)
		{
			for (size_t j = 0; j < m_task_section_cfg[i].task_group_cfg.size(); j++)
			{
				if (m_task_section_cfg[i].task_group_cfg[j].task_group == task_group)
				{
					std::map<int, YSLSTaskRewardCfg>::const_iterator it = m_task_section_cfg[i].task_group_cfg[j].reward_cfg.find(seq);
					if (it != m_task_section_cfg[i].task_group_cfg[j].reward_cfg.end()) return &it->second;
				}
			}
		}
	}
	return NULL;
}

const YSLSExchangeCfg * RandActivityYiShouLinShiConfig::GetExchangeCfg(int exchange_group, int seq) const
{
	for (int i = 0; i < (int)m_exchange_section_cfg.size(); ++i)
	{
		if (m_exchange_section_cfg[i].exchange_group == exchange_group)
		{
			std::map<int, YSLSExchangeCfg>::const_iterator it = m_exchange_section_cfg[i].exchange_cfg.find(seq);
			if (it != m_exchange_section_cfg[i].exchange_cfg.end()) return &it->second;
		}
	}
	return NULL;
}

int RandActivityYiShouLinShiConfig::GetFbType(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return -100;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);

	for (int i = 0; i < (int)m_fb_cfg.size(); ++i)
	{
		if (m_fb_cfg[i].section_start <= act_real_open_day && m_fb_cfg[i].section_end >= act_real_open_day)
		{
			return m_fb_cfg[i].secret_area_type;
		}
	}

	return -1;
}

int RandActivityYiShouLinShiConfig::GetBuyCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return -100;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);

	for (int i = 0; i < (int)m_buy_cfg.size(); ++i)
	{
		if (m_buy_cfg[i].section_start <= act_real_open_day && m_buy_cfg[i].section_end >= act_real_open_day)
		{
			return m_buy_cfg[i].price;
		}
	}

	return -1;
}

const YSLSRecycleCfg * RandActivityYiShouLinShiConfig::GetRecycleCfg(int item_id) const
{
	for (std::map<int, YSLSRecycleCfg>::const_iterator it = m_recycle_cfg.begin(); it != m_recycle_cfg.end(); it++)
	{
		if (it->second.item_id == item_id)
		{
			return &it->second;
		}
	}

	return NULL;
}

const YSLSRecycleCfg * RandActivityYiShouLinShiConfig::GetCurRecycleCfg(int exchange_group) const
{
	std::map<int, YSLSRecycleCfg>::const_iterator it = m_recycle_cfg.find(exchange_group);
	if (it != m_recycle_cfg.end()) return &it->second;

	return NULL;
}


int RandActivityYiShouLinShiConfig::GetStartTimeCfg(unsigned int& start_time, int& type) const
{
	unsigned int cur_time = EngineAdapter::Instance().Time();

	for (int i = 0; i < (int)m_cycle_time_cfg.size(); i++)
	{
		if (m_cycle_time_cfg[i].start_time <= cur_time && cur_time < m_cycle_time_cfg[i].end_time)
		{
			start_time = m_cycle_time_cfg[i].start_time;
			type = m_cycle_time_cfg[i].type;
			return m_cycle_time_cfg[i].exchange_group;
		}
	}

	return -1;
}

const YSLSCycleCfg * RandActivityYiShouLinShiConfig::GetCurCycleCfg(int seq) const
{
	for (size_t i = 0; i < m_cycle_time_cfg.size(); i++)
	{
		if (m_cycle_time_cfg[i].type == seq)
		{
			return &m_cycle_time_cfg[i];
		}
	}

	return NULL;
}

int RandActivityYiShouLinShiConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_task_section_cfg.size() > 0)
		{
			int pre_index = m_task_section_cfg.size() - 1;
			if (section_start != m_task_section_cfg[pre_index].section_start ||
				section_end != m_task_section_cfg[pre_index].section_end)
			{
				if (section_start < m_task_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_task_section_cfg.push_back(YSLSTaskSectionCfg());
			}
		}

		if (m_task_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_task_section_cfg.push_back(YSLSTaskSectionCfg());
		}

		YSLSTaskSectionCfg & node_cfg = m_task_section_cfg[m_task_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;


		int task_group = 0;
		if (!GetSubNodeValue(data_element, "task_group", task_group) || task_group <= 0) return -20;

		if (node_cfg.task_group_cfg.size() > 0)
		{
			if (node_cfg.task_group_cfg[node_cfg.task_group_cfg.size() - 1].task_group != task_group)
			{
				node_cfg.task_group_cfg.push_back(YSLSTaskGroupCfg());
			}
		}
		else if(node_cfg.task_group_cfg.size() == 0)
		{
			node_cfg.task_group_cfg.push_back(YSLSTaskGroupCfg());
		}

		YSLSTaskGroupCfg & node_node_cfg = node_cfg.task_group_cfg[node_cfg.task_group_cfg.size() - 1];
		node_node_cfg.task_group = task_group;



		YSLSTaskCfg task_cfg;
		YSLSTaskRewardCfg reward_cfg;

		int seq = -1;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "task_type", task_cfg.task_type) || task_cfg.task_type < 0 || task_cfg.task_type >= YSLSTaskCfg::TASK_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "parameter", task_cfg.task_param) || task_cfg.task_param < 0) return -3;
		if (!GetSubNodeValue(data_element, "like", reward_cfg.like) || reward_cfg.like < 0) return -4;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -5;

			reward_cfg.reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}
		if (reward_cfg.reward_list.empty()) return -6;

		m_task_param_cfg[task_cfg.task_type] = task_cfg.task_param;

		node_node_cfg.task_cfg[seq] = task_cfg;
		node_node_cfg.reward_cfg[seq] = reward_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while(NULL != data_element)
	{
		int exchange_group = 0;
		if (!GetSubNodeValue(data_element, "exchange_group", exchange_group) || exchange_group < 0) return -1;

		if (m_exchange_section_cfg.empty())
		{
			m_exchange_section_cfg.push_back(YSLSExchangeSectionCfg());
		}
		else
		{
			if (m_exchange_section_cfg[m_exchange_section_cfg.size() - 1].exchange_group != exchange_group)
			{
				m_exchange_section_cfg.push_back(YSLSExchangeSectionCfg());
			}
		}

		YSLSExchangeSectionCfg & node_cfg = m_exchange_section_cfg[m_exchange_section_cfg.size() - 1];
		node_cfg.exchange_group = exchange_group;


		YSLSExchangeCfg exchange_cfg;
		int seq = -1;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "unlock_integral", exchange_cfg.unlock_integral) || exchange_cfg.unlock_integral < 0) return -2;
		if (!GetSubNodeValue(data_element, "limit_type", exchange_cfg.limit_type) || exchange_cfg.limit_type < 0) return -3;
		if (!GetSubNodeValue(data_element, "limit_times", exchange_cfg.limit_times) || exchange_cfg.limit_times < 0) return -4;
		if (!GetSubNodeValue(data_element, "item_id", exchange_cfg.consume_item_id) || NULL == ITEMPOOL->GetItem(exchange_cfg.consume_item_id)) return -5;
		if (!GetSubNodeValue(data_element, "num", exchange_cfg.consume_num) || exchange_cfg.consume_num < 0) return -6;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;
		if (!exchange_cfg.reward.ReadConfig(reward_element)) return -7;

		node_cfg.exchange_cfg[seq] = exchange_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitFbCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_fb_cfg.size() > 0)
		{
			int pre_index = m_fb_cfg.size() - 1;
			if (section_start != m_fb_cfg[pre_index].section_start ||
				section_end != m_fb_cfg[pre_index].section_end)
			{
				if (section_start < m_fb_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_fb_cfg.push_back(YSLSFbSectionCfg());
			}
		}

		if (m_fb_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_fb_cfg.push_back(YSLSFbSectionCfg());
		}

		YSLSFbSectionCfg & node_cfg = m_fb_cfg[m_fb_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		if (!GetSubNodeValue(data_element, "secret_area_type", node_cfg.secret_area_type) || node_cfg.secret_area_type < 0) return -1;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_buy_cfg.size() > 0)
		{
			int pre_index = m_buy_cfg.size() - 1;
			if (section_start != m_buy_cfg[pre_index].section_start ||
				section_end != m_buy_cfg[pre_index].section_end)
			{
				if (section_start < m_buy_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_buy_cfg.push_back(YSLSBuySectionCfg());
			}
		}

		if (m_buy_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_buy_cfg.push_back(YSLSBuySectionCfg());
		}

		YSLSBuySectionCfg & node_cfg = m_buy_cfg[m_buy_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price < 0) return -1;

		data_element = data_element->NextSiblingElement();	
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitRecycleCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while(NULL != data_element)
	{
		YSLSRecycleCfg recycle_cfg;

		int exchange_group = 0;
		if (!GetSubNodeValue(data_element, "att_exchange_group", exchange_group) || exchange_group < 0) return -12;

		if (!GetSubNodeValue(data_element, "recycle_item_id", recycle_cfg.item_id) || NULL == ITEMPOOL->GetItem(recycle_cfg.item_id)) return -1;
		if (!GetSubNodeValue(data_element, "coin", recycle_cfg.coin) || recycle_cfg.coin < 0) return -2;
		if (!GetSubNodeValue(data_element, "gold", recycle_cfg.gold) || recycle_cfg.gold < 0) return -3;
		if (!GetSubNodeValue(data_element, "silver_coin", recycle_cfg.silver_coin) || recycle_cfg.silver_coin < 0) return -4;
		if (!GetSubNodeValue(data_element, "is_return", recycle_cfg.has_item) || recycle_cfg.has_item < 0) return -5;

		if (recycle_cfg.has_item != 0)
		{
			TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == reward_list_element) return -__LINE__;

			TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
			while (NULL != reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -6;
				recycle_cfg.item_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		if ((int)recycle_cfg.item_list.size() > MAX_ATTACHMENT_ITEM_NUM) return -7;		// 防止超过发邮件最大道具数量

		m_recycle_cfg[exchange_group] = recycle_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitCycleCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		YSLSCycleCfg cycle_cfg;

		if (!GetSubNodeValue(data_element, "type", cycle_cfg.type) || cycle_cfg.type < 0) return -5;
		if (!GetSubNodeValue(data_element, "start_time", cycle_cfg.start_time) || cycle_cfg.start_time < 0) return -1;
		if (!GetSubNodeValue(data_element, "end_time", cycle_cfg.end_time) || cycle_cfg.end_time < 0) return -2;
		if (!GetSubNodeValue(data_element, "exchange_group", cycle_cfg.exchange_group) || cycle_cfg.exchange_group < 0) return -3;

		if (!m_cycle_time_cfg.empty())
		{
			if (cycle_cfg.start_time != m_cycle_time_cfg[m_cycle_time_cfg.size() - 1].end_time) return -4;
		}

		m_cycle_time_cfg.push_back(cycle_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYiShouLinShiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "is_more_change", m_other_cfg.is_more_change) || m_other_cfg.is_more_change < 0 ) return -1;
	if (!GetSubNodeValue(data_element, "integral_up", m_other_cfg.integral_up) || m_other_cfg.integral_up < 0) return -2;

	return 0;
}
