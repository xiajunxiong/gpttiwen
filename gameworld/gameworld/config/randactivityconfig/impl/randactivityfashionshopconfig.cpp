#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityfashionshopconfig.hpp"

RandActivityFashionShopConfig::RandActivityFashionShopConfig()
{
}

RandActivityFashionShopConfig::~RandActivityFashionShopConfig()
{
}

bool RandActivityFashionShopConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("open_fashion_store", InitBuyCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAFashionShopCfg * RandActivityFashionShopConfig::GetRAFashionShopCfg(RandActivityManager * ramgr, int seq) const
{
	if (seq < 0 || seq >= RA_FASHION_SHOP_MAX_ITEM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_FASHION_SHOP);
	for (std::vector<RAFashionShopSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
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

int RandActivityFashionShopConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityFashionShopConfig::InitBuyCfg(TiXmlElement * RootElement)
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
				m_section_cfg.push_back(RAFashionShopSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAFashionShopSectionCfg());
		}

		RAFashionShopSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAFashionShopCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_FASHION_SHOP_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi) || item_cfg.need_chong_zhi < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "need_gold", item_cfg.gold) || item_cfg.gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "total_exchange", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;

		if (item_cfg.need_chong_zhi == 0 && item_cfg.gold == 0)
		{
			return -__LINE__;
		}

		TiXmlElement * gift_item_element = data_element->FirstChildElement("item");
		ItemConfigData temp;
		if (!temp.ReadConfig(gift_item_element))
		{
			return -__LINE__;
		}

		item_cfg.rewards.push_back(temp);

		next_seq += 1;

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityFashionShopConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}
