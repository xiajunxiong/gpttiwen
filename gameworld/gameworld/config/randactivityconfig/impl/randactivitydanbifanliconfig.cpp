#include "randactivitydanbifanliconfig.hpp"

RandActivityDanBiFanLiConfig::RandActivityDanBiFanLiConfig()
{
}

RandActivityDanBiFanLiConfig::~RandActivityDanBiFanLiConfig()
{
	std::vector<RADanBiFanLiRewardSectionCfg>().swap(m_reward_section_cfg);
	std::vector<RADanBiFanLiPoolSectionCfg>().swap(m_pool_section_cfg);
}

bool RandActivityDanBiFanLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("other", InitOtherCfg);
	LOAD_RA_CONFIG_2("reward", InitRewardCfg);		
	LOAD_RA_CONFIG_2("gold_type", InitPoolCfg);

	iRet = this->CheckConfig();
	if (0 != iRet)
	{
		*err = STRING_SPRINTF("RandActivityDanBiFanLiConfig::CheckConfig failed %d", iRet);
		return false;
	}

	return true;
}

const RADanBiFanLiRewardRewardCfg * RandActivityDanBiFanLiConfig::GetRewardCfg(RandActivityManager * ramgr, int pool_seq, int seq) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI);
	for (size_t i = 0; i < m_reward_section_cfg.size(); i++)
	{
		if (!m_reward_section_cfg[i].IsInclude(act_real_open_day))
		{
			continue;
		}
		const RADanBiFanLiRewardSectionCfg & section_cfg = m_reward_section_cfg[i];
		if (pool_seq < 0 || pool_seq >= section_cfg.reward_pool_count || pool_seq >= ARRAY_ITEM_COUNT(section_cfg.reward_pool_list)) 
		{
			return NULL;
		}
		const std::vector<RADanBiFanLiRewardRewardCfg> & reward_list = section_cfg.reward_pool_list[pool_seq];
		if (seq < 0 || seq >= (int)reward_list.size())
		{
			return NULL;
		}

		return &reward_list[seq];
	}

	return NULL;
}

const RADanBiFanLiPoolCfg * RandActivityDanBiFanLiConfig::GetPoolCfg(RandActivityManager * ramgr, int pool_seq) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI);
	for (size_t i = 0; i < m_pool_section_cfg.size(); i++)
	{
		if (!m_pool_section_cfg[i].IsInclude(act_real_open_day))
		{
			continue;
		}
		const RADanBiFanLiPoolSectionCfg & section_cfg = m_pool_section_cfg[i];
		if (pool_seq < 0 || pool_seq >= section_cfg.pool_count || pool_seq >= ARRAY_ITEM_COUNT(section_cfg.pool_list))
		{
			return NULL;
		}

		return &section_cfg.pool_list[pool_seq];
	}

	return NULL;
}

int RandActivityDanBiFanLiConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityDanBiFanLiConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0)
	{
		err = STRING_SPRINTF("cfg_ver[%d] <= 0", m_other_cfg.cfg_ver);
		return -1;
	}

	return 0;
}

int RandActivityDanBiFanLiConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
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
				m_reward_section_cfg.push_back(RADanBiFanLiRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(RADanBiFanLiRewardSectionCfg());
		}

		RADanBiFanLiRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int gold_pool_seq = 0, seq = 0;
		if (!GetSubNodeValue(data_element, "gold_seq", gold_pool_seq) || gold_pool_seq < 0 || gold_pool_seq >= ARRAY_ITEM_COUNT(node_cfg.reward_pool_list))
		{
			return -1;
		}
		std::vector<RADanBiFanLiRewardRewardCfg> & reward_list = node_cfg.reward_pool_list[gold_pool_seq];
		if (gold_pool_seq == node_cfg.reward_pool_count)
		{
			node_cfg.reward_pool_count += 1;
			if (!reward_list.empty())
			{
				return -2;
			}
		}
		if (gold_pool_seq + 1 != node_cfg.reward_pool_count)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq != (int)reward_list.size())
		{
			return -4;
		}
		


		RADanBiFanLiRewardRewardCfg cfg;
		cfg.seq = seq;

		if (!GetSubNodeValue(data_element, "gold_num", cfg.gold_num) || cfg.gold_num <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(data_element, "gold_give", cfg.gold_give) || cfg.gold_give <= 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(data_element, "buy_money", cfg.buy_money) || cfg.buy_money <= 0 || 0 >= (cfg.buy_money / 10) || 0 != (cfg.buy_money % 10))
		{
			return -7;
		}
		
		reward_list.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDanBiFanLiConfig::InitPoolCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
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

		if (m_pool_section_cfg.size() > 0)
		{
			int pre_index = m_pool_section_cfg.size() - 1;
			if (section_start != m_pool_section_cfg[pre_index].section_start ||
				section_end != m_pool_section_cfg[pre_index].section_end)
			{
				if (section_start < m_pool_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_pool_section_cfg.push_back(RADanBiFanLiPoolSectionCfg());
			}
		}

		if (m_pool_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_pool_section_cfg.push_back(RADanBiFanLiPoolSectionCfg());
		}

		RADanBiFanLiPoolSectionCfg & node_cfg = m_pool_section_cfg[m_pool_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		
		int gold_pool_seq = 0;
		if (!GetSubNodeValue(data_element, "gold_seq", gold_pool_seq) || gold_pool_seq < 0 || gold_pool_seq >= ARRAY_ITEM_COUNT(node_cfg.pool_list) ||
			gold_pool_seq != node_cfg.pool_count)
		{
			return -1;
		}

		node_cfg.pool_count += 1;

		RADanBiFanLiPoolCfg & cfg = node_cfg.pool_list[gold_pool_seq];
		if (!GetSubNodeValue(data_element, "limit_type", cfg.limit_type) || cfg.limit_type < ACTVITY_BUY_LIMIT_TYPE_BEGIN || 
			cfg.limit_type >= ACTVITY_BUY_LIMIT_TYPE_MAX)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "times", cfg.times) || cfg.times <= 0)
		{
			return -3;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDanBiFanLiConfig::CheckConfig()
{
	std::map<long long, int> pool_section_map;
	for (int section_index = 0; section_index < (int)m_pool_section_cfg.size(); ++section_index)
	{
		long long secion_key = TranslateInt64(m_pool_section_cfg[section_index].section_start, m_pool_section_cfg[section_index].section_end);
		
		pool_section_map[secion_key] = m_pool_section_cfg[section_index].pool_count;
	}

	for (int section_index = 0; section_index < (int)m_reward_section_cfg.size(); ++section_index)
	{
		long long secion_key = TranslateInt64(m_reward_section_cfg[section_index].section_start, m_reward_section_cfg[section_index].section_end);
		std::map<long long, int>::iterator it = pool_section_map.find(secion_key);
		if (it == pool_section_map.end())
		{
			return -1;
		}
		if (m_reward_section_cfg[section_index].reward_pool_count != it->second)
		{
			return -2;
		}
	}

	return 0;
}
