#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityshencihuafuconfig.hpp"

RandActivityShenCiHuaFuConfig::RandActivityShenCiHuaFuConfig()
{
}

RandActivityShenCiHuaFuConfig::~RandActivityShenCiHuaFuConfig()
{
	std::vector<RandActivityShenCiHuaFuSectionCfg>().swap(m_section_cfg);
	std::vector<RandActivityShenCiHuaFuBuyItemSectionCfg>().swap(m_buy_section_cfg);
	std::vector<RandActivityShenCiHuaFuExchangeShopItemSectionCfg>().swap(m_exchange_shop_item_section_cfg);
}

bool RandActivityShenCiHuaFuConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOthercfg);
	LOAD_RA_CONFIG("basis_configuration", InitRandActivityShenCiHuaFuCfg);
	LOAD_RA_CONFIG("reward_group", InitRandActivityShenCiHuaFuRewardGroupItemCfg);
	LOAD_RA_CONFIG("shop_configuration", InitRandActivityShenCiHuaFuBuyItemCfg);
	LOAD_RA_CONFIG("recharge_shop", InitRandActivityShenCiHuaFuExchangeShopItemCfg);

	return true;
}

const RandActivityShenCiHuaFuOtherCfg & RandActivityShenCiHuaFuConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityShenCiHuaFuCfg * RandActivityShenCiHuaFuConfig::GetRandActivityShenCiHuaFuCfg(RandActivityManager * ramgr, int reward_type) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU);
	for (std::vector<RandActivityShenCiHuaFuSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return MapValuesConstPtr(it->item_list, reward_type);
		}
	}

	return NULL;
}

const RandActivityShenCiHuaFuRewardGroupItemCfg * RandActivityShenCiHuaFuConfig::GetRandActivityShenCiHuaFuRewardGroupItemCfg(RandActivityManager * ramgr, int reward_type, int reward_seq) const
{
	const RandActivityShenCiHuaFuCfg * cfg = this->GetRandActivityShenCiHuaFuCfg(ramgr, reward_type);
	if (!cfg)
	{
		return NULL;
	}

	const RandActivityShenCiHuaFuRewardGroupCfg * reward_cfg = MapValuesConstPtr(m_reward_group_cfg, cfg->reward_group_id);
	if (!reward_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(reward_cfg->item_list, reward_seq);
}

const RandActivityShenCiHuaFuRewardGroupCfg * RandActivityShenCiHuaFuConfig::GetRandActivityShenCiHuaFuRewardGroupCfg(RandActivityManager * ramgr, int reward_type) const
{
	const RandActivityShenCiHuaFuCfg * cfg = this->GetRandActivityShenCiHuaFuCfg(ramgr, reward_type);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(m_reward_group_cfg, cfg->reward_group_id);
}

const RandActivityShenCiHuaFuBuyItemCfg * RandActivityShenCiHuaFuConfig::GetRandActivityShenCiHuaFuBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU);
	for (std::vector<RandActivityShenCiHuaFuBuyItemSectionCfg>::const_iterator it = m_buy_section_cfg.begin(); it != m_buy_section_cfg.end(); it++)
	{
		const RandActivityShenCiHuaFuBuyItemSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return MapValuesConstPtr(curr.item_list, buy_seq);
		}
	}

	return NULL;
}

const RandActivityShenCiHuaFuExchangeShopItemCfg * RandActivityShenCiHuaFuConfig::GetRandActivityShenCiHuaFuExchangeShopItemCfg(RandActivityManager * ramgr, int buy_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU);
	for (std::vector<RandActivityShenCiHuaFuExchangeShopItemSectionCfg>::const_iterator it = m_exchange_shop_item_section_cfg.begin(); it != m_exchange_shop_item_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return MapValuesConstPtr(it->item_list, buy_seq);
		}
	}

	return NULL;
}

int RandActivityShenCiHuaFuConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityShenCiHuaFuConfig::InitOthercfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.draw_need_item_id)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityShenCiHuaFuConfig::InitRandActivityShenCiHuaFuCfg(TiXmlElement * RootElement)
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

				m_section_cfg.push_back(RandActivityShenCiHuaFuSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityShenCiHuaFuSectionCfg());
		}

		RandActivityShenCiHuaFuSectionCfg & node_cfg = m_section_cfg.back();
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenCiHuaFuCfg item_cfg_1;
		item_cfg_1.type = RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_SELECT;
		if (!GetSubNodeValue(data_element, "rare_reward_group", item_cfg_1.reward_group_id)) return -__LINE__;
		node_cfg.item_list[item_cfg_1.type] = item_cfg_1;

		RandActivityShenCiHuaFuCfg item_cfg_2;
		item_cfg_2.type = RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_RAND;
		if (!GetSubNodeValue(data_element, "common_reward_group", item_cfg_2.reward_group_id)) return -__LINE__;
		node_cfg.item_list[item_cfg_2.type] = item_cfg_2;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShenCiHuaFuConfig::InitRandActivityShenCiHuaFuRewardGroupItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "section_start", reward_group_id)) return -__LINE__;
		RandActivityShenCiHuaFuRewardGroupCfg & cfg = m_reward_group_cfg[reward_group_id];
		cfg.reward_group_id = reward_group_id;

		RandActivityShenCiHuaFuRewardGroupItemCfg item_cfg;
		item_cfg.reward_group_id = reward_group_id;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.reward_seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.is_broadcast)) return -__LINE__;

		cfg.item_list[item_cfg.reward_seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShenCiHuaFuConfig::InitRandActivityShenCiHuaFuBuyItemCfg(TiXmlElement * RootElement)
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
		std::vector<RandActivityShenCiHuaFuBuyItemSectionCfg> & section_cfg_ref = m_buy_section_cfg;
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

				section_cfg_ref.push_back(RandActivityShenCiHuaFuBuyItemSectionCfg());
			}
		}
		if (section_cfg_ref.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg_ref.push_back(RandActivityShenCiHuaFuBuyItemSectionCfg());
		}

		RandActivityShenCiHuaFuBuyItemSectionCfg & node_cfg = section_cfg_ref[section_cfg_ref.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenCiHuaFuBuyItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq >= SHEN_CI_HUA_FU_MAX_BUY_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
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

int RandActivityShenCiHuaFuConfig::InitRandActivityShenCiHuaFuExchangeShopItemCfg(TiXmlElement * RootElement)
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
		std::vector<RandActivityShenCiHuaFuExchangeShopItemSectionCfg> & section_cfg_ref = m_exchange_shop_item_section_cfg;
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

				section_cfg_ref.push_back(RandActivityShenCiHuaFuExchangeShopItemSectionCfg());
			}
		}
		if (section_cfg_ref.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg_ref.push_back(RandActivityShenCiHuaFuExchangeShopItemSectionCfg());
		}

		RandActivityShenCiHuaFuExchangeShopItemSectionCfg & node_cfg = section_cfg_ref[section_cfg_ref.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityShenCiHuaFuExchangeShopItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq >= SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_times) || item_cfg.limit_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_times_day) || item_cfg.limit_times_day < 0) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "recharge_item", item_cfg.need_item_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "num", item_cfg.need_item_num)) return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
