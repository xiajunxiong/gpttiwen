#include "randactivitysecrettreasureconfig.hpp"

#include "servercommon/roleactivity/rasecrettreasureparam.hpp"
RandActivitySecretTreasureConfig::RandActivitySecretTreasureConfig()
{
}

RandActivitySecretTreasureConfig::~RandActivitySecretTreasureConfig()
{
	std::vector<RASecretTreasureShopSectionCfg>().swap(m_shop_section_cfg);
}

bool RandActivitySecretTreasureConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("special_offer_treasure", InitSectionCfg);

	return true;
}

int RandActivitySecretTreasureConfig::GetPhase(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return -1;
	
	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			return m_shop_section_cfg[i].phase;
		}
	}

	return -1;
}

const RASecretTreasureShopCfg * RandActivitySecretTreasureConfig::GetShopCfg(int phase, int seq)const
{
	if(phase < 0|| seq < 0 || seq >= MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM) return NULL;

	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].phase == phase)
		{
			if (seq < (int)m_shop_section_cfg[i].node_list.size())
			{
				return &m_shop_section_cfg[i].node_list[seq];
			}
		}
	}

	return NULL;
}

const RASecretTreasureShopCfg * RandActivitySecretTreasureConfig::GetShopCfg(int type) const
{
	std::map<int, RASecretTreasureShopCfg>::const_iterator it = m_shop_map.find(type);
	if(it != m_shop_map.end()) return &it->second;

	return NULL;
}

int RandActivitySecretTreasureConfig::InitSectionCfg(TiXmlElement * RootElement)
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
					return -777;
				}

				m_shop_section_cfg.push_back(RASecretTreasureShopSectionCfg());
				seq = 0;
				last_phase = phase;
			}
		}
		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_shop_section_cfg.push_back(RASecretTreasureShopSectionCfg());
			seq = 0;
		}

		RASecretTreasureShopSectionCfg & node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase) || node_cfg.phase < 0) return -__LINE__;
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

		RASecretTreasureShopCfg node;
		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq < 0 || node.seq >= MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM) return -1;
		if (!GetSubNodeValue(data_element, "limit_type", node.limit_type) || node.limit_type < ACTVITY_BUY_LIMIT_TYPE_BEGIN || node.limit_type >= ACTVITY_BUY_LIMIT_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "buy_times", node.buy_times) || node.buy_times < 0) return -3;
		if (!GetSubNodeValue(data_element, "today_times", node.today_times) || node.today_times < 0) return -4;

		if (node.buy_times <= 0 && node.today_times <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "price_type", node.price_type) || node.price_type < RASecretTreasureShopCfg::PRICE_TYPE_MIN ||
			node.price_type > RASecretTreasureShopCfg::PRICE_TYPE_MONEY) return -6;
		if (!GetSubNodeValue(data_element, "price", node.price) || node.price <= 0) return -7;
		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0 || node.type >= MAX_SECRET_TREASURE_SHOP_ITEM_NUM) return -8;


		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)
		{
			return -100;
		}

		if (!node.item.ReadConfig(reward_element))
		{
			return -101;
		}

		if (seq++ != node.seq)
		{
			return -33;
		}

		node_cfg.node_list.push_back(node);
		m_shop_map[node.type] = node;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
