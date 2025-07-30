#include "randactivitysuperoffshopconfig.hpp"
#include <set>

RandActivitySuperOffShopConfig::RandActivitySuperOffShopConfig()
{
}

RandActivitySuperOffShopConfig::~RandActivitySuperOffShopConfig()
{
	std::vector<RASuperOffShopSeverSectionShopCfg>().swap(m_shop_section_cfg);
}

bool RandActivitySuperOffShopConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("random_group", InitDiscGroupCfg);
	LOAD_RA_CONFIG("random_configuration", InitPhaseRangeCfg);
	LOAD_RA_CONFIG("super_off_shop", InitShopCfg);

	return true;
}

const RASuperOffShopDiscCfg * RandActivitySuperOffShopConfig::GetDiscItemCfgByType(int type) const
{	
	std::map<int, RASuperOffShopDiscCfg>::const_iterator it = m_disc_item_map.find(type);
	if(it == m_disc_item_map.end()) return NULL;

	return &it->second;
}

bool RandActivitySuperOffShopConfig::GetDiscItemRandListCfg(RandActivityManager * ramgr, short * rand_disc_list, short list_num) const
{
	if(NULL == ramgr || NULL == rand_disc_list || list_num <= 0 || list_num > MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM) return false;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	std::set<int> phase_list;

	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				if (m_shop_section_cfg[i].section_list[k].phase > 0 && m_shop_section_cfg[i].section_list[k].phase <= MAX_RA_SUPER_OFF_SHOP_PHASE_NUM)
				{
					phase_list.insert(m_shop_section_cfg[i].section_list[k].phase);
				}
			}
		}
	}
	if(phase_list.empty()) return false;

	int count = 0;
	for (std::set<int>::iterator list_it = phase_list.begin(); list_it != phase_list.end(); ++list_it)
	{
		std::map<int, RASuperOffShopPhaseRangeCfg>::const_iterator phase_it = m_phase_disc_cfg.find(*list_it);
		if (phase_it == m_phase_disc_cfg.end()) 
		{
			memset(rand_disc_list, 0, sizeof(short) * list_num);
			return false;
		}
		int rand_num = 0;
		if (phase_it->second.random_down_num == phase_it->second.random_up_num)
		{
			rand_num = phase_it->second.random_up_num;
		}
		else
		{
			rand_num = RandomNum(phase_it->second.random_down_num, phase_it->second.random_up_num + 1);
		}
		if (rand_num <= 0)
		{
			memset(rand_disc_list, 0, sizeof(short) * list_num);
			return false;
		}
		if (rand_num > MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM)
		{
			rand_num = MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
		}

		std::map<int, RASuperOffShopDiscGroupCfg>::const_iterator disc_group_it = m_disc_group_cfg.find(phase_it->second.discount_group_id);
		if (disc_group_it == m_disc_group_cfg.end() || (int)disc_group_it->second.item_list.size() < rand_num)
		{
			memset(rand_disc_list, 0, sizeof(short) * list_num);
			return false;
		}
		int seq_begin = (*list_it - 1) * MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
		RASuperOffShopDiscGroupCfg disc_group_cfg = disc_group_it->second;
		for (int i = 0; i < rand_num && count < list_num; i++)
		{
			int rate_item_num = RandomNum(disc_group_cfg.rate_count);
			for (int k = 0; k < (int)disc_group_cfg.item_list.size(); k++)
			{
				if (rate_item_num < disc_group_cfg.item_list[k].rate)
				{
					rand_disc_list[seq_begin++] = disc_group_cfg.item_list[k].type;
					count++;
					disc_group_cfg.rate_count -= disc_group_cfg.item_list[k].rate;
					disc_group_cfg.item_list.erase(disc_group_cfg.item_list.begin() + k);
					break;
				}
				rate_item_num -= disc_group_cfg.item_list[k].rate;
			}
		}
	}
	return true;
}

const RASuperOffShopShopCfg * RandActivitySuperOffShopConfig::GetShopCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr || seq < 0 || seq >= MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				const RASuperOffShopSectionShopCfg & node_cfg = m_shop_section_cfg[i].section_list[k];
				if (node_cfg.start_time <= act_open_day && node_cfg.end_time >= act_open_day && seq < (int)node_cfg.shop_list.size())
				{
					return &node_cfg.shop_list[seq];
				}
			}
		}
	}
	return NULL;
}

int RandActivitySuperOffShopConfig::GetPhase(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return 0;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				const RASuperOffShopSectionShopCfg & node_cfg = m_shop_section_cfg[i].section_list[k];
				if (node_cfg.start_time <= act_open_day && node_cfg.end_time >= act_open_day)
				{
					return node_cfg.phase;
				}
			}
		}
	}
	return 0;
}

int RandActivitySuperOffShopConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivitySuperOffShopConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0) return -1;
	if (!GetSubNodeValue(data_element, "phase", m_other_cfg.phase) || m_other_cfg.phase != MAX_RA_SUPER_OFF_SHOP_PHASE_NUM) return -2;

	return 0;
}

int RandActivitySuperOffShopConfig::InitDiscGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int disc_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", disc_group_id) || disc_group_id <= 0) return -1;

		RASuperOffShopDiscCfg node_cfg;
		if (!GetSubNodeValue(data_element, "type", node_cfg.type) || node_cfg.type <= 0) return -2;

		if (NULL != this->GetDiscItemCfgByType(node_cfg.type))
		{
			return -22;
		}

		if (!GetSubNodeValue(data_element, "limit_type", node_cfg.limit_type)) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", node_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", node_cfg.today_times)) return -5;

		switch (node_cfg.limit_type)
		{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (node_cfg.buy_times <= 0) return -6;
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (node_cfg.today_times <= 0) return -7;
		}
		break;
		default:
			return -8;
		}

		if (!GetSubNodeValue(data_element, "price_type", node_cfg.price_type) || node_cfg.price_type < RandActivitySuperOffShopConfig::PRICE_TYPE_MIN ||
			node_cfg.price_type > RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY) return -9;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price <= 0) return -10;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -11;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)
		{
			return -100;
		}

		if (!node_cfg.reward_item.ReadConfig(reward_element))
		{
			return -101;
		}

		m_disc_group_cfg[disc_group_id].item_list.push_back(node_cfg);
		m_disc_group_cfg[disc_group_id].rate_count += node_cfg.rate;
		m_disc_item_map[node_cfg.type] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivitySuperOffShopConfig::InitPhaseRangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int phase = 0;
		if (!GetSubNodeValue(data_element, "phase", phase) || phase <= 0 || phase > MAX_RA_SUPER_OFF_SHOP_PHASE_NUM) return -1;

		RASuperOffShopPhaseRangeCfg node_cfg;
		if (!GetSubNodeValue(data_element, "reward_group", node_cfg.discount_group_id)) return -2;
		if (!GetSubNodeValue(data_element, "random_down_num", node_cfg.random_down_num) || node_cfg.random_down_num <= 0) return -3;
		if (!GetSubNodeValue(data_element, "random_up_num", node_cfg.random_up_num) || node_cfg.random_up_num < node_cfg.random_down_num || node_cfg.random_up_num > MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM) return -4;

		std::map<int, RASuperOffShopDiscGroupCfg >::iterator it = m_disc_group_cfg.find(node_cfg.discount_group_id);
		if (it == m_disc_group_cfg.end() || node_cfg.random_up_num > (int)it->second.item_list.size())
		{
			return -5;
		}

		m_phase_disc_cfg[phase] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivitySuperOffShopConfig::InitShopCfg(TiXmlElement * RootElement)
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

				m_shop_section_cfg.push_back(RASuperOffShopSeverSectionShopCfg());
				last_phase = -1;
			}
		}
		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_shop_section_cfg.push_back(RASuperOffShopSeverSectionShopCfg());
		}
		RASuperOffShopSeverSectionShopCfg & server_node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
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

				server_node_cfg.section_list.push_back(RASuperOffShopSectionShopCfg());
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
			server_node_cfg.section_list.push_back(RASuperOffShopSectionShopCfg());
			seq = 0;
		}

		RASuperOffShopSectionShopCfg & node_cfg = server_node_cfg.section_list[server_node_cfg.section_list.size() - 1];
		node_cfg.start_time = start_time;
		node_cfg.end_time = end_time;

		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase) || m_phase_disc_cfg.end() == m_phase_disc_cfg.find(node_cfg.phase)) return -22;
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
		RASuperOffShopShopCfg shop_cfg;

		if (!GetSubNodeValue(data_element, "index", shop_cfg.index) || shop_cfg.index < 0 || shop_cfg.index >= MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM) return -1;
		if (!GetSubNodeValue(data_element, "seq", shop_cfg.seq) || shop_cfg.seq < 0 || shop_cfg.seq >= MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM) return -2;
		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type)) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", shop_cfg.today_times)) return -5;

		switch (shop_cfg.limit_type)
		{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (shop_cfg.buy_times <= 0) return -6;
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (shop_cfg.today_times <= 0) return -7;
		}
		break;
		default:
			return -8;
		}

		if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < RandActivitySuperOffShopConfig::PRICE_TYPE_MIN ||
			shop_cfg.price_type > RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY) return -9;
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
