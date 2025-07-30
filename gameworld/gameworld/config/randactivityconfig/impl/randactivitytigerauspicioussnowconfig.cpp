#include "randactivitytigerauspicioussnowconfig.hpp"


RandActivityTigerAuspiciousSnowConfig::RandActivityTigerAuspiciousSnowConfig()
{

}


RandActivityTigerAuspiciousSnowConfig::~RandActivityTigerAuspiciousSnowConfig()
{
	std::vector<RATigerSpSonwRewardSectionCfg>().swap(m_reward_section_cfg);
	std::vector<RATigerSpSonwRechargeSectionCfg>().swap(m_recharge_section_cfg);
	std::vector<RATigerSpSonwDriftSectionCfg>().swap(m_drift_section_cfg);
	std::vector<RAtigerSpSnowSceneCfg>().swap(m_scene_cfg);
}

bool RandActivityTigerAuspiciousSnowConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitRewardCfg);
	LOAD_RA_CONFIG("recharge_configure", InitRechargeCfg);
	LOAD_RA_CONFIG("snow_configure", InitSnowDriftCfg);
	LOAD_RA_CONFIG("scene_list", InitSceneCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const ItemConfigData * RandActivityTigerAuspiciousSnowConfig::GetRandRewardCfg(RandActivityManager * ramgr, ARG_OUT int & seq) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			int rate_rand = RandomNum(m_reward_section_cfg[i].total_rate);

			for (int j = 0; j < (int)m_reward_section_cfg[i].reward_list.size(); ++j)
			{
				if (rate_rand < m_reward_section_cfg[i].reward_list[j].rate)
				{
					seq = m_reward_section_cfg[i].reward_list[j].seq;
					return &m_reward_section_cfg[i].reward_list[j].reward;
				}
			}
		}
	}

	return NULL;
}

const RATigerSpSonwRechargeSectionCfg * RandActivityTigerAuspiciousSnowConfig::GetRechargeCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);

	for (int i = 0; i < (int)m_recharge_section_cfg.size(); ++i)
	{
		if (m_recharge_section_cfg[i].section_start <= act_real_open_day && m_recharge_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_recharge_section_cfg[i];
		}
	}

	return NULL;
}

const RATigerSpSonwDriftCfg * RandActivityTigerAuspiciousSnowConfig::GetDriftCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);

	for (int i = 0; i < (int)m_drift_section_cfg.size(); ++i)
	{
		if (m_drift_section_cfg[i].section_start <= act_real_open_day && act_real_open_day <= m_drift_section_cfg[i].section_end)
		{
			int rate_rand = RandomNum(m_drift_section_cfg[i].total_rate);

			for (int j = 0; j < (int)m_drift_section_cfg[i].reward_list.size(); ++j)
			{
				if (rate_rand < m_drift_section_cfg[i].reward_list[j].rate)
				{
					return &m_drift_section_cfg[i].reward_list[j];
				}
			}
		}
	}

	return NULL;
}

int RandActivityTigerAuspiciousSnowConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_reward_section_cfg.push_back(RATigerSpSonwRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RATigerSpSonwRewardSectionCfg());
		}

		RATigerSpSonwRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATigerSpSonwRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;
	
		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -__LINE__;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		ItemConfigData item;
		if (!item.ReadConfig(reward_element))
		{
			return -__LINE__;
		}

		reward_cfg.reward = item;
		m_reward_section_cfg[m_reward_section_cfg.size() - 1].reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerAuspiciousSnowConfig::InitRechargeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

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

		RATigerSpSonwRechargeSectionCfg recharge_cfg;
		recharge_cfg.section_start = section_start;
		recharge_cfg.section_end = section_end;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -__LINE__;
			}

			recharge_cfg.reward.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		m_recharge_section_cfg.push_back(recharge_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerAuspiciousSnowConfig::InitSnowDriftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

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

		if (m_drift_section_cfg.size() > 0)
		{
			int pre_index = m_drift_section_cfg.size() - 1;
			if (section_start != m_drift_section_cfg[pre_index].section_start ||
				section_end != m_drift_section_cfg[pre_index].section_end)
			{
				if (section_start < m_drift_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_drift_section_cfg.push_back(RATigerSpSonwDriftSectionCfg());
			}
		}

		if (m_drift_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_drift_section_cfg.push_back(RATigerSpSonwDriftSectionCfg());
		}

		RATigerSpSonwDriftSectionCfg & node_cfg = m_drift_section_cfg[m_drift_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATigerSpSonwDriftCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -__LINE__;
		node_cfg.total_rate += rate;
		reward_cfg.rate = node_cfg.total_rate;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -__LINE__;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -__LINE__;
			}

			reward_cfg.v_reward.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		m_drift_section_cfg[m_drift_section_cfg.size() - 1].reward_list.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerAuspiciousSnowConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		RAtigerSpSnowSceneCfg scene_cfg;

		if (!GetSubNodeValue(data_element, "scene_id", scene_cfg.scene_id) || scene_cfg.scene_id < 0) return -__LINE__;

		if (!GetSubNodeValue(data_element, "npc_id", scene_cfg.npc_id) || scene_cfg.npc_id < 0) return -__LINE__;

		m_scene_cfg.push_back(scene_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTigerAuspiciousSnowConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "refresh_times", m_other_cfg.refresh_times) || m_other_cfg.refresh_times <= 0) return -__LINE__;
	if (!GetSubNodeValue(data_element, "first_num", m_other_cfg.first_num) || m_other_cfg.first_num < 0) return -__LINE__;
	if (!GetSubNodeValue(data_element, "collect_times", m_other_cfg.collect_times) || m_other_cfg.collect_times < 0) return -__LINE__;

	if (!GetSubNodeValue(data_element, "consume_item_id_1", m_other_cfg.consume_item_id_1) || m_other_cfg.consume_item_id_1 < 0) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item_id_1)) return -__LINE__;

	if (!GetSubNodeValue(data_element, "consume_item_id_2", m_other_cfg.consume_item_id_2) || m_other_cfg.consume_item_id_2 < 0) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item_id_2)) return -__LINE__;

	if (!GetSubNodeValue(data_element, "consume_item_id_3", m_other_cfg.consume_item_id_3) || m_other_cfg.consume_item_id_3 < 0) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item_id_3)) return -__LINE__;

	if (!GetSubNodeValue(data_element, "consume_item_id_4", m_other_cfg.consume_item_id_4) || m_other_cfg.consume_item_id_4 < 0) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item_id_4)) return -__LINE__;

	if (!GetSubNodeValue(data_element, "consume_num", m_other_cfg.consume_num) || m_other_cfg.consume_num <= 0) return -__LINE__;

	if (!GetSubNodeValue(data_element, "lucky_draw_item_id", m_other_cfg.snowball_id) || m_other_cfg.snowball_id < 0) return -__LINE__;
	if (NULL == ITEMPOOL->GetItem(m_other_cfg.snowball_id)) return -__LINE__;

	return 0;
}

