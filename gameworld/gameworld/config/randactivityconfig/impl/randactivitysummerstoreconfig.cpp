#include "randactivitysummerstoreconfig.hpp"


RandActivitySummerStoreConfig::RandActivitySummerStoreConfig()
{
}

RandActivitySummerStoreConfig::~RandActivitySummerStoreConfig()
{
	std::vector<RASummerStoreSeverSectionShopCfg>().swap(m_shop_section_cfg);
}

bool RandActivitySummerStoreConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);		
	LOAD_RA_CONFIG("summer_store", InitShopCfg);

	return true;
}

const RASummerStoreShopCfg * RandActivitySummerStoreConfig::GetShopCfg(RandActivityManager * ramgr, int seq) const
{
	if(NULL == ramgr || seq < 0 || seq >= MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM) return NULL;
	
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUMMER_STORE);
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUMMER_STORE);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				const RASummerStoreSectionShopCfg & node_cfg = m_shop_section_cfg[i].section_list[k];
				if (node_cfg.start_time <= act_open_day && node_cfg.end_time >= act_open_day && seq < node_cfg.shop_list.size())
				{
					return &node_cfg.shop_list[seq];
				}
			}
		}
	}
	return NULL;
}

int RandActivitySummerStoreConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivitySummerStoreConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "phase", m_other_cfg.phase) || m_other_cfg.phase <= 0) return -1;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0) return -2;

	return 0;
}

int RandActivitySummerStoreConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int seq = 0;
	int last_phase = -1;
	int phase = 0;
	while (NULL != data_element)
	{
		bool has_server_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_server_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_server_section_cfg = false;
		}

		if (!has_server_section_cfg) // 没配置，默认给一个无限区间
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
					return -777;
				}

				m_shop_section_cfg.push_back(RASummerStoreSeverSectionShopCfg());
				last_phase = -1;
			}
		}
		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_shop_section_cfg.push_back(RASummerStoreSeverSectionShopCfg());
		}
		RASummerStoreSeverSectionShopCfg & server_node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
		server_node_cfg.section_start = section_start;
		server_node_cfg.section_end = section_end;

		int start_time = 0, end_time = 0;
		bool has_section_cfg = true;
		if (!GetSubNodeValue(data_element, "start_time", start_time))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "end_time", end_time))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			start_time = 1;
			end_time = INT_MAX;
		}

		if (0 == end_time) // 0 代表无穷
		{
			end_time = INT_MAX;
		}

		if (server_node_cfg.section_list.size() > 0)
		{
			int pre_index = server_node_cfg.section_list.size() - 1;
			if (start_time != server_node_cfg.section_list[pre_index].start_time ||
				end_time != server_node_cfg.section_list[pre_index].end_time)
			{
				if (start_time < server_node_cfg.section_list[pre_index].end_time)
				{
					return -666;
				}

				server_node_cfg.section_list.push_back(RASummerStoreSectionShopCfg());
				seq = 0;
				last_phase = phase;
			}
		}
		if (server_node_cfg.section_list.size() == 0)
		{
			if (start_time != 1)
			{
				return -555;
			}
			server_node_cfg.section_list.push_back(RASummerStoreSectionShopCfg());
			seq = 0;
		}

		RASummerStoreSectionShopCfg & node_cfg = server_node_cfg.section_list[server_node_cfg.section_list.size() - 1];
		node_cfg.start_time = start_time;
		node_cfg.end_time = end_time;

		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase) || node_cfg.phase <= 0) return -22;
		phase = node_cfg.phase;
		if (-1 != last_phase)
		{
			if (phase != last_phase + 1)
			{
				return -33;
			}
		}
		else
		{
			if (phase != 1)		//阶段固定从1开始
			{
				return -44;
			}
		}
		RASummerStoreShopCfg shop_cfg;

		if (!GetSubNodeValue(data_element, "index", shop_cfg.index) || shop_cfg.index < 0 || shop_cfg.index >= MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM) return -1;
		if (!GetSubNodeValue(data_element, "seq", shop_cfg.seq) || shop_cfg.seq < 0 || shop_cfg.seq >= MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM) return -2;
		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type)) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", shop_cfg.today_times)) return -5;

		switch (shop_cfg.limit_type)
		{
			case ACTVITY_BUY_LIMIT_TYPE_ALL:
			{
				if(shop_cfg.buy_times <= 0) return -6;
			}
			break;
			case ACTVITY_BUY_LIMIT_TYPE_DAY:
			{
				if(shop_cfg.today_times <= 0) return -7;
			}
			break;
		default:
			return -8;
		}

		if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < RASummerStoreShopCfg::PRICE_TYPE_MIN ||
			shop_cfg.price_type > RASummerStoreShopCfg::PRICE_TYPE_MONEY) return -9;
		if (!GetSubNodeValue(data_element, "price", shop_cfg.price) || shop_cfg.price <= 0) return -10;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)
		{
			return -100;
		}

		if (!shop_cfg.reward_item.ReadConfig(reward_element))
		{
			return -101;
		}

		if (seq++ != shop_cfg.seq)
		{
			return -66;
		}

		node_cfg.shop_list.push_back(shop_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
