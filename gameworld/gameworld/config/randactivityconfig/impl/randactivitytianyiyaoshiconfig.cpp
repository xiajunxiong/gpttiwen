#include "randactivitytianyiyaoshiconfig.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"
#include "servercommon/struct/moneyotherdef.h"

const RATianYiYaoShiBasicCfg * RATianYiYaoShiBasicSectionCfg::RandBasicCfg() const
{
	int real_rate = RandomNum(total_rate);

	for (size_t i = 0; i < reward_list.size(); i++)
	{
		if (reward_list[i].rate > real_rate)
		{
			return &reward_list[i];
		}
	}

	return NULL;
}

RandActivityTianYiYaoShiConfig::RandActivityTianYiYaoShiConfig()
{
}

RandActivityTianYiYaoShiConfig::~RandActivityTianYiYaoShiConfig()
{
	std::vector<RATianYiYaoShiBasicSectionCfg>().swap(m_basic_section_cfg);
	std::vector<RATianYiYaoShiShopSectionCfg>().swap(m_shop_section_cfg);
}

bool RandActivityTianYiYaoShiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("basis_configuration", InitBasisCfg);
	LOAD_RA_CONFIG_2("shop_configuration", InitShopCfg);
	LOAD_RA_CONFIG_2("other", InitOtherCfg);

	return true;
}

const RATianYiYaoShiBasicCfg * RandActivityTianYiYaoShiConfig::RandRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);

	for (size_t i = 0; i < m_basic_section_cfg.size(); i++)
	{
		if (m_basic_section_cfg[i].IsInclude(act_real_open_day))
		{
			return m_basic_section_cfg[i].RandBasicCfg();
		}
	}

	return NULL;
}

const RATianYiYaoShiShopCfg * RandActivityTianYiYaoShiConfig::GetShopCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);

	for (size_t i = 0; i < m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].IsInclude(act_real_open_day))
		{
			if (seq < 0 || seq >= (int)m_shop_section_cfg[i].shop_list.size()) return NULL;
			return &m_shop_section_cfg[i].shop_list[seq];
		}
	}

	return NULL;
}

int RandActivityTianYiYaoShiConfig::InitBasisCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_basic_section_cfg.size() > 0)
		{
			int pre_index = m_basic_section_cfg.size() - 1;
			if (section_start != m_basic_section_cfg[pre_index].section_start ||
				section_end != m_basic_section_cfg[pre_index].section_end)
			{
				if (section_start < m_basic_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_basic_section_cfg.push_back(RATianYiYaoShiBasicSectionCfg());
			}
		}

		if (m_basic_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_basic_section_cfg.push_back(RATianYiYaoShiBasicSectionCfg());
		}

		RATianYiYaoShiBasicSectionCfg & node_cfg = m_basic_section_cfg[m_basic_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATianYiYaoShiBasicCfg basic_cfg;

		int t_rate = 0;
		if (!GetSubNodeValue(data_element, "rate", t_rate) || t_rate < 0) return-1;
		node_cfg.total_rate += t_rate;
		basic_cfg.rate = node_cfg.total_rate;

		int hearsay = 0;
		if (!GetSubNodeValue(data_element, "hearsay", hearsay) || hearsay < 0) return -2;
		basic_cfg.is_hearsay = !!hearsay;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -101;

		if (!basic_cfg.reward.ReadConfig(reward_element)) return -3;

		if (basic_cfg.reward.num > USHRT_MAX)
		{
			err = STRING_SPRINTF("reward num[%d] too much > %d", basic_cfg.reward.num, USHRT_MAX);
			return -4;
		}

		node_cfg.reward_list.push_back(basic_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTianYiYaoShiConfig::InitShopCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_shop_section_cfg.size() > 0)
		{
			int pre_index = m_shop_section_cfg.size() - 1;
			if (section_start != m_shop_section_cfg[pre_index].section_start ||
				section_end != m_shop_section_cfg[pre_index].section_end)
			{
				if (section_start < m_shop_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_shop_section_cfg.push_back(RATianYiYaoShiShopSectionCfg());
			}
		}

		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_shop_section_cfg.push_back(RATianYiYaoShiShopSectionCfg());
		}

		RATianYiYaoShiShopSectionCfg & node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RATianYiYaoShiShopCfg shop_cfg;

		int cfg_or = 0;
		if (!GetSubNodeValue(data_element, "cfg_or", cfg_or) || cfg_or < 0) return -1;
		shop_cfg.cfg_or = !!cfg_or;

		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type) || shop_cfg.limit_type <= 0 || shop_cfg.limit_type >= RA_TIAN_YI_YAO_SHI_BUY_LIMIT_MAX) return -2;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times) || shop_cfg.buy_times < 0) return -3;
		if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < 0 || shop_cfg.price_type >= MONEY_TYPE_MAX) return -4;
		if (!GetSubNodeValue(data_element, "price", shop_cfg.price) || shop_cfg.price < 0) return -5;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -101;

		if (!shop_cfg.reward.ReadConfig(reward_element)) return -6;

		node_cfg.shop_list.push_back(shop_cfg);

		if (node_cfg.shop_list.size() > MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE)
		{
			err = STRING_SPRINTF("gift type too much[%d] > %d", node_cfg.shop_list.size(), MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE);
			return -7;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTianYiYaoShiConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.consume_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.consume_item)) return -1;

	return 0;
}
