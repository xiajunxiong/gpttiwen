#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitydingzhibaoxiang3config.hpp"

RandActivityDingZhiBaoXiang3Config::RandActivityDingZhiBaoXiang3Config()
{
}

RandActivityDingZhiBaoXiang3Config::~RandActivityDingZhiBaoXiang3Config()
{
	std::vector<RADingZhiBaoXiang3SectionCfg>().swap(m_section_cfg);
}

bool RandActivityDingZhiBaoXiang3Config::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("box_configure", InitBuyCfg);
	LOAD_RA_CONFIG("reward_pool", InitRewardGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RADingZhiBaoXiang3ItemCfg * RandActivityDingZhiBaoXiang3Config::GetRADingZhiBaoXiangItemCfg(RandActivityManager * ramgr, int seq) const
{
	if (seq < 0 || seq >= RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3);
	for (std::vector<RADingZhiBaoXiang3SectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
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

const RADingZhiBaoXiang3RewardGroup * RandActivityDingZhiBaoXiang3Config::GetRADingZhiBaoXiangRewardGroup(int reward_group_id) const
{
	std::map<int, RADingZhiBaoXiang3RewardGroup>::const_iterator iter = m_reward_group.find(reward_group_id);
	if (iter != m_reward_group.end())
	{
		return &iter->second;
	}

	return NULL;
}

int RandActivityDingZhiBaoXiang3Config::GetRADingZhiBaoXiangItemNum(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3);
	for (std::vector<RADingZhiBaoXiang3SectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return (int)it->item.size();
		}
	}
	return 0;
}

int RandActivityDingZhiBaoXiang3Config::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityDingZhiBaoXiang3Config::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
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
					return -888;
				}

				next_seq = 0;
				m_section_cfg.push_back(RADingZhiBaoXiang3SectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RADingZhiBaoXiang3SectionCfg());
		}

		RADingZhiBaoXiang3SectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RADingZhiBaoXiang3ItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_id", item_cfg.reward_group_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_num", item_cfg.reward_select_num) || item_cfg.reward_select_num < 0) return -__LINE__;

		next_seq += 1;

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDingZhiBaoXiang3Config::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(data_element, "item_id", group_id)) return -__LINE__;

		RADingZhiBaoXiang3RewardGroup & item_cfg = m_reward_group[group_id];

		int seq = 0;
		if (!GetSubNodeValue(data_element, "type", seq) || seq != (int)item_cfg.rewards.size()) return -__LINE__;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			ItemConfigData temp;
			if (!temp.ReadConfig(item_element)) return -__LINE__;
			item_cfg.rewards.push_back(temp);
		}


		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityDingZhiBaoXiang3Config::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0) return -__LINE__;

	TiXmlElement * free_reward_item_list_elemenet = data_element->FirstChildElement("free_reward_item_list");
	if (NULL != free_reward_item_list_elemenet)
	{
		TiXmlElement * free_reward_item_element = free_reward_item_list_elemenet->FirstChildElement("free_reward_item");
		if (NULL == free_reward_item_element) return -__LINE__;

		while (NULL != free_reward_item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(free_reward_item_element))
			{
				return -__LINE__;
			}

			m_other_cfg.free_reward.push_back(item);
			free_reward_item_element = free_reward_item_element->NextSiblingElement();
		}
	}

	if (m_other_cfg.free_reward.empty()) return -__LINE__;

	return 0;
}
