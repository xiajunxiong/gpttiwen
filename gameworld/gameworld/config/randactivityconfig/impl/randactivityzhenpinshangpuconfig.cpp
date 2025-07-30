#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityzhenpinshangpuconfig.hpp"

RandActivityZhenPinShangPuConfig::RandActivityZhenPinShangPuConfig()
{
}

RandActivityZhenPinShangPuConfig::~RandActivityZhenPinShangPuConfig()
{
	std::vector<RAZhenPinShangPuSectionCfg>().swap(m_section_cfg);
}

bool RandActivityZhenPinShangPuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("zhenpinhangpu", InitBuyCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityZhenPinShangPuConfig::GetOpenDay(RandActivityManager * ramgr)const
{
	if (NULL == ramgr) return 1;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU);
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU);
	for (int i = 0; i < (int)m_section_cfg.size(); i++)
	{
		if (m_section_cfg[i].section_start <= act_real_open_day && act_real_open_day <= m_section_cfg[i].section_end)
		{
			for (int k = 0; k < (int)m_section_cfg[i].open_day.size(); k++)
			{
				const RAZhenPinShangPuOpenDay & node_cfg = m_section_cfg[i].open_day[k];
				if (node_cfg.min_buy_day <= act_open_day && act_open_day <= node_cfg.max_buy_day)
				{
					return std::min(this->GetCurRealOpenDay(ramgr), act_real_open_day + node_cfg.min_buy_day - 1);
				}
			}
		}
	}

	return 1;
}


const RAZhenPinShangPuItemCfg * RandActivityZhenPinShangPuConfig::GetRAZhenPinShangPuItemCfg(RandActivityManager * ramgr, int seq) const
{
	if (seq < 0 || seq >= RA_ZHEN_PIN_SHANG_PU_MAX_ITEM) return NULL;

	int act_real_open_day = this->GetOpenDay(ramgr);
	for (std::vector<RAZhenPinShangPuSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if (seq >= (int)it->item.size())
			{
				return NULL;
			}

			return &it->item[seq];
		}
	}

	return NULL;
}

int RandActivityZhenPinShangPuConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

bool RandActivityZhenPinShangPuConfig::GetRewards(int item_group_id, int select_num, int select_flag, std::vector<ItemConfigData> & rewards) const
{
	const RAZhenPinShangPuRewardGroupCfg * reward_group_cfg = this->GetRAZhenPinShangPuRewardGroupCfg(item_group_id);
	if (!reward_group_cfg)
	{
		return false;
	}

	if (select_num > 0)
	{
		for (int i = 0; i < BitCount(select_flag); ++i)
		{
			if (!(select_flag & (1 << i)))
			{
				continue;
			}

			if (i >= (int)reward_group_cfg->item.size())
			{
				return false;
			}

			rewards.push_back(reward_group_cfg->item[i]);
		}

		if (select_num != (int)rewards.size())
		{
			return false;
		}
	}
	else
	{
		rewards.insert(rewards.end(), reward_group_cfg->item.begin(), reward_group_cfg->item.end());
	}

	return true;
}

const RAZhenPinShangPuRewardGroupCfg * RandActivityZhenPinShangPuConfig::GetRAZhenPinShangPuRewardGroupCfg(int item_group_id) const
{
	return MapValuesConstPtr(m_reward_group_cfg, item_group_id);
}

int RandActivityZhenPinShangPuConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	int next_type = 0;
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

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
			    section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -__LINE__;
				}

				m_section_cfg.push_back(RAZhenPinShangPuSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -__LINE__;
			}
			m_section_cfg.push_back(RAZhenPinShangPuSectionCfg());
		}

		RAZhenPinShangPuSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int next_seq = (int)node_cfg.item.size();
		RAZhenPinShangPuItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "type", item_cfg.type) || item_cfg.type != next_type++) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_ZHEN_PIN_SHANG_PU_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type) || item_cfg.limit_type < ACTVITY_BUY_LIMIT_TYPE_BEGIN || item_cfg.limit_type >= ACTVITY_BUY_LIMIT_TYPE_MAX) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_num", item_cfg.add_gold) || item_cfg.add_gold < 0) return -__LINE__;

		if (!GetSubNodeValue(data_element, "reward_group", item_cfg.reward_group_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "start_time", item_cfg.min_buy_day)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "end_time", item_cfg.max_buy_day)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_num", item_cfg.select_reward_num)) return -__LINE__;

		RAZhenPinShangPuOpenDay open_day;
		open_day.min_buy_day = item_cfg.min_buy_day;
		open_day.max_buy_day= item_cfg.max_buy_day;

		if (!node_cfg.HasSame(open_day))
		{
			node_cfg.open_day.push_back(open_day);
		}

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (int i = 1; i < (int)m_section_cfg.size(); i++)
	{ 
		if (m_section_cfg[i].open_day != m_section_cfg[0].open_day)
		{
			return -__LINE__;
		}
	}
	
	return 0;
}

int RandActivityZhenPinShangPuConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	while (NULL != data_element)
	{
		int  reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "item_id", reward_group_id)) return -__LINE__;
		RAZhenPinShangPuRewardGroupCfg & cfg = m_reward_group_cfg[reward_group_id];

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		ItemConfigData item;
		if (!item.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		cfg.item.push_back(item);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhenPinShangPuConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver))
	{
		return -__LINE__;
	}

	return 0;
}

