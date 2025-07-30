#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityzhuiyueshangdianconfig.hpp"

RandActivityZhuiYueShangDianConfig::RandActivityZhuiYueShangDianConfig()
{
}

RandActivityZhuiYueShangDianConfig::~RandActivityZhuiYueShangDianConfig()
{
}

bool RandActivityZhuiYueShangDianConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOthercfg);
	LOAD_RA_CONFIG("draw_configuration", InitRandActivityZhuiYueShangDianCfg);
	LOAD_RA_CONFIG("reward_group", InitRandActivityZhuiYueShangDianRewardGroupItemCfg);
	LOAD_RA_CONFIG("shop_configuration", InitRandActivityZhuiYueShangDianBuyItemCfg);

	return true;
}

const RandActivityZhuiYueShangDianOtherCfg & RandActivityZhuiYueShangDianConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityZhuiYueShangDianCfg * RandActivityZhuiYueShangDianConfig::GetRandActivityZhuiYueShangDianCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN);
	for (std::vector<RandActivityZhuiYueShangDianSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(it->cfg);
		}
	}

	return NULL;
}

const RandActivityZhuiYueShangDianRewardGroupItemCfg * RandActivityZhuiYueShangDianConfig::GetRandActivityZhuiYueShangDianRewardGroupItemCfg(RandActivityManager * ramgr, int reward_seq) const
{
	const RandActivityZhuiYueShangDianCfg * cfg = this->GetRandActivityZhuiYueShangDianCfg(ramgr);
	if (!cfg)
	{
		return NULL;
	}

	const RandActivityZhuiYueShangDianRewardGroupCfg * reward_cfg = MapValuesConstPtr(m_reward_group_cfg, cfg->selec_reward_group_id);
	if (!reward_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(reward_cfg->item_list, reward_seq);
}

const RandActivityZhuiYueShangDianRewardGroupItemCfg * RandActivityZhuiYueShangDianConfig::GetRandActivityZhuiYueShangDianSpecialRewardItemCfg(RandActivityManager * ramgr) const
{
	const RandActivityZhuiYueShangDianCfg * cfg = this->GetRandActivityZhuiYueShangDianCfg(ramgr);
	if (!cfg)
	{
		return NULL;
	}

	const RandActivityZhuiYueShangDianRewardGroupCfg * reward_cfg = MapValuesConstPtr(m_reward_group_cfg, cfg->special_reward_group_id);
	if (!reward_cfg)
	{
		return NULL;
	}

	if (reward_cfg->item_list.empty())
	{
		return NULL;
	}

	return &(reward_cfg->item_list.begin()->second);
}

const RandActivityZhuiYueShangDianRewardGroupCfg * RandActivityZhuiYueShangDianConfig::GetRandActivityZhuiYueShangDianRewardGroupCfg(RandActivityManager * ramgr) const
{
	const RandActivityZhuiYueShangDianCfg * cfg = this->GetRandActivityZhuiYueShangDianCfg(ramgr);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(m_reward_group_cfg, cfg->selec_reward_group_id);
}

const RandActivityZhuiYueShangDianBuyItemCfg * RandActivityZhuiYueShangDianConfig::GetRandActivityZhuiYueShangDianBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN);
	for (std::vector<RandActivityZhuiYueShangDianBuyItemSectionCfg>::const_iterator it = m_buy_section_cfg.begin(); it != m_buy_section_cfg.end(); it++)
	{
		const RandActivityZhuiYueShangDianBuyItemSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return MapValuesConstPtr(curr.item_list, buy_seq);
		}
	}

	return NULL;
}

int RandActivityZhuiYueShangDianConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityZhuiYueShangDianConfig::InitOthercfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityZhuiYueShangDianConfig::InitRandActivityZhuiYueShangDianCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
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

				m_section_cfg.push_back(RandActivityZhuiYueShangDianSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityZhuiYueShangDianSectionCfg());
		}

		RandActivityZhuiYueShangDianSectionCfg & node_cfg = m_section_cfg.back();
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityZhuiYueShangDianCfg & cfg = node_cfg.cfg;

		if (!GetSubNodeValue(data_element, "reward_group",cfg.selec_reward_group_id)) ret_line;
		if (!GetSubNodeValue(data_element, "big_reward_group", cfg.special_reward_group_id)) ret_line;
		if (!GetSubNodeValue(data_element, "consume_item_id", cfg.draw_need_item_id)) ret_line;
		if (!GetSubNodeValue(data_element, "consume_num", cfg.draw_need_item_num)) ret_line;		

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhuiYueShangDianConfig::InitRandActivityZhuiYueShangDianRewardGroupItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id)) return -__LINE__;
		RandActivityZhuiYueShangDianRewardGroupCfg & cfg = m_reward_group_cfg[reward_group_id];
		cfg.reward_group_id = reward_group_id;

		RandActivityZhuiYueShangDianRewardGroupItemCfg item_cfg;
		item_cfg.reward_group_id = reward_group_id;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.reward_seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "bao_di_id", item_cfg.bao_di_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "guarantee_times", item_cfg.bao_di_times)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.is_broadcast)) return -__LINE__;

		cfg.item_list[item_cfg.reward_seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityZhuiYueShangDianConfig::InitRandActivityZhuiYueShangDianBuyItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
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
		std::vector<RandActivityZhuiYueShangDianBuyItemSectionCfg> & section_cfg_ref = m_buy_section_cfg;
		if (section_cfg_ref.size() > 0)
		{
			int pre_index = section_cfg_ref.size() - 1;
			if (section_start != section_cfg_ref[pre_index].section_start ||
				section_end != section_cfg_ref[pre_index].section_end)
			{
				if (section_start < section_cfg_ref[pre_index].section_end)
				{
					return -888;
				}

				section_cfg_ref.push_back(RandActivityZhuiYueShangDianBuyItemSectionCfg());
			}
		}
		if (section_cfg_ref.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg_ref.push_back(RandActivityZhuiYueShangDianBuyItemSectionCfg());
		}

		RandActivityZhuiYueShangDianBuyItemSectionCfg & node_cfg = section_cfg_ref[section_cfg_ref.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityZhuiYueShangDianBuyItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq >= ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "day_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
		if (ReadItemConfigData2(data_element, "give_item", item_cfg.rewards) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "cfg_or", item_cfg.is_need_chong_zhi)) return -__LINE__;

		if (item_cfg.is_need_chong_zhi)
		{
			if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold)) return -__LINE__;
		}
		else
		{
			if (!GetSubNodeValue(data_element, "price_type", item_cfg.money_type)) return -__LINE__;
			if (!GetSubNodeValue(data_element, "price", item_cfg.money_value)) return -__LINE__;
		}

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
