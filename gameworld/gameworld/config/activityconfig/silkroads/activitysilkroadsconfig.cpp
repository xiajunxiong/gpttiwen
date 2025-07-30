#include "activitysilkroadsconfig.hpp"

ActivitySilkRoadsConfig::ActivitySilkRoadsConfig()
{
}

ActivitySilkRoadsConfig::~ActivitySilkRoadsConfig()
{
	std::vector<ActivitySilkRoadsCommodityCfg>().swap(m_commodity_list);
	for (std::map<int, std::vector<ActivitySilkRoadsCityRewardCfg> >::iterator it = m_city_reward_cfg.begin(); it != m_city_reward_cfg.end(); it++)
	{
		std::vector<ActivitySilkRoadsCityRewardCfg>().swap(it->second);
	}
}

bool ActivitySilkRoadsConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("commodity", InitCommodityCfg);			//货物配置需要放在城镇配置读取之前
	LOAD_CONFIG_2("city_reward", InitCityRewardCfg);		//进入城镇奖励配置需要放在城镇配置读取之前
	LOAD_CONFIG_2("city_cfg", InitCityCfg);
	LOAD_CONFIG_2("line", InitLineCfg);						//线路图配置需要放在城镇配置和其他分页读取之后
	LOAD_CONFIG_2("shopping", InitShoppingCfg);	
	LOAD_CONFIG_2("buy_warehouse_price", InitBuyWarehouseCfg);	
	LOAD_CONFIG_2("warehouse_expend", InitWarehouseExpendCfg);
	LOAD_CONFIG_2("buy_supply_peice", InitBuySupplyPeiceCfg);
	LOAD_CONFIG_2("buy_news", InitBuyNewsCfg);
	LOAD_CONFIG_2("settlement_box", InitSettlementBoxCfg);
	LOAD_CONFIG_2("ranking_list", InitRankListCfg);			
	LOAD_CONFIG_2("silk_roads_skill", InitRoadsSkillCfg);	//技能配置放在扩充仓库与其他配置之后

	return true;
}

const ActivitySilkRoadsCityCfg * ActivitySilkRoadsConfig::GetCityCfg(int city_map_seq)
{
	std::map<int, ActivitySilkRoadsCityCfg>::const_iterator it = m_city_map.find(city_map_seq);
	if (it == m_city_map.end())
	{
		return NULL;
	}
	return &it->second;
}

const ActivitySilkRoadsCommodityCfg * ActivitySilkRoadsConfig::GetCommodityCfg(int commodity_seq)
{
	int seq = commodity_seq - 1;
	if(seq < 0 || seq >= (int)m_commodity_list.size()) return NULL;

	return &m_commodity_list[seq];
}

const ActivitySilkRoadsLineCfg * ActivitySilkRoadsConfig::GetLineCfg(int group)
{
	std::map<int, ActivitySilkRoadsLineCfg>::const_iterator it = m_line_map.find(group);
	if (it == m_line_map.end())
	{
		return NULL;
	}
	return &it->second;
}

const ActivitySilkRoadsLineCfg * ActivitySilkRoadsConfig::GetRandLineCfg()
{
	if(m_line_map.empty()) return NULL;

	int rand_index = rand() % (int)m_line_map.size();
	int i = 0;
	for (std::map<int, ActivitySilkRoadsLineCfg>::const_iterator it = m_line_map.begin(); it != m_line_map.end(); ++it, ++i)
	{
		if (i == rand_index)
		{
			return &it->second;
		}
	}

	return NULL;
}

int ActivitySilkRoadsConfig::GetRandFreightId(int type)
{
	if(type <= SILK_ROADS_LOG_TYPE_BEGIN || type >= SILK_ROADS_LOG_TYPE_MAX || m_shopping_list[type].empty()) return 0;

	int rand_index = RandomNum((int)m_shopping_list[type].size());
	int index = 0;
	for (std::set<int>::const_iterator it = m_shopping_list[type].begin(); it != m_shopping_list[type].end(); ++it, index++)
	{
		if (rand_index == index)
		{
			return *it;
		}
	}

	return 0;
}

int ActivitySilkRoadsConfig::GetBuyWarehousePrice(int buy_times)
{
	for (std::map<int, int>::const_reverse_iterator it = m_buy_warehouse_pirce_map.rbegin(); it != m_buy_warehouse_pirce_map.rend(); ++it)
	{
		if (it->first <= buy_times)
		{
			return it->second;
		}
	}

	return 0;
}

int ActivitySilkRoadsConfig::GetMaxBuyWarehouseTimes()
{
	if (m_buy_warehouse_pirce_map.empty()) return 0;

	return m_buy_warehouse_pirce_map.rbegin()->first;
}

int ActivitySilkRoadsConfig::GetWarehouseExpend(int warehouse_grid_count)
{
	if(m_warehouse_expend_map.empty()) return 0;

	for (std::map<int, int>::const_reverse_iterator it = m_warehouse_expend_map.rbegin(); it != m_warehouse_expend_map.rend(); ++it)
	{
		if (it->first <= warehouse_grid_count)
		{
			return it->second;
		}
	}

	return 0;
}

int ActivitySilkRoadsConfig::GetBuySupplyPrice(int buy_times)
{
	if(m_buy_supply_peice_map.empty()) return 0;

	for (std::map<int, int>::const_reverse_iterator it = m_buy_supply_peice_map.rbegin(); it != m_buy_supply_peice_map.rend(); ++it)
	{
		if (it->first <= buy_times)
		{
			return it->second;
		}
	}

	return m_buy_supply_peice_map.begin()->second;
}

int ActivitySilkRoadsConfig::GetMaxBuySupplyTimes()
{
	if(m_buy_supply_peice_map.empty()) return 0;

	return m_buy_supply_peice_map.rbegin()->first;
}

int ActivitySilkRoadsConfig::GetBuyNewsPrice(int buy_times)
{
	if(m_buy_news_map.empty()) return 0;

	for (std::map<int, int>::const_reverse_iterator it = m_buy_news_map.rbegin(); it != m_buy_news_map.rend(); ++it)
	{
		if (it->first <= buy_times)
		{
			return it->second;
		}
	}

	return m_buy_news_map.begin()->second;
}

int ActivitySilkRoadsConfig::GetMaxBuyNewsTimes()
{
	if(m_buy_news_map.empty()) return 0;

	return m_buy_news_map.rbegin()->first;
}

const ActivitySilkRoadsSettlementBoxCfg * ActivitySilkRoadsConfig::GetSettlementBoxCfg(int coin_count)
{
	for (std::map<int, ActivitySilkRoadsSettlementBoxCfg>::const_reverse_iterator it = m_settlement_box_map.rbegin(); it != m_settlement_box_map.rend(); ++it)
	{
		if (it->first <= coin_count)
		{
			return &it->second;
		}
	}

	return NULL;
}

const ItemConfigData * ActivitySilkRoadsConfig::GetRankReward(int rank)
{
	for (std::map<ParamSection, ItemConfigData>::const_iterator it = m_rank_reward_map.begin(); it != m_rank_reward_map.end(); ++it)
	{
		if (it->first.section_min <= rank && rank <= it->first.section_max)
		{
			return &it->second;
		}
	}

	return NULL;
}

const ActivitySilkRoadsSkillCfg * ActivitySilkRoadsConfig::GetSkillCfg(int skill_id)
{
	std::map<int, ActivitySilkRoadsSkillCfg>::const_iterator it = m_skill_map.find(skill_id);
	if (it == m_skill_map.end())
	{
		return NULL;
	}

	return &it->second;
}

const ActivitySilkRoadsSkillCfg * ActivitySilkRoadsConfig::GetRandSkillCfg()
{
	if(m_skill_map.empty()) return NULL;

	int rand_index = rand() % (int)m_skill_map.size();
	int i = 0;
	for (std::map<int, ActivitySilkRoadsSkillCfg>::const_iterator it = m_skill_map.begin(); it != m_skill_map.end(); ++it, ++i)
	{
		if (i == rand_index)
		{
			return &it->second;
		}
	}

	return NULL;
}

const ActivitySilkRoadsCityRewardCfg * ActivitySilkRoadsConfig::GetRandCityRewardByCity(int city_map_seq)
{
	const ActivitySilkRoadsCityCfg * city_cfg = this->GetCityCfg(city_map_seq);
	if (NULL == city_cfg)
	{
		return NULL;
	}

	int rate_num = RandomNum(MAX_SILK_ROADS_REWAD_RATE_COUNT);
	if (rate_num >= m_other_cfg.city_reward_group_rate)
	{
		return NULL;
	}

	return this->GetRandCityReward(city_cfg->city_reward_group_id);
}

const ActivitySilkRoadsCityRewardCfg * ActivitySilkRoadsConfig::GetRandCityReward(int reward_group_id)
{
	std::map<int, std::vector<ActivitySilkRoadsCityRewardCfg> >::const_iterator it = m_city_reward_cfg.find(reward_group_id);
	if (it == m_city_reward_cfg.end() || it->second.empty())
	{
		return NULL;
	}
	int rate_num = RandomNum(MAX_SILK_ROADS_REWAD_RATE_COUNT);
	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (rate_num < it->second[i].rate)
		{
			return &it->second[i];
		}
		rate_num -= it->second[i].rate;
	}
	
	return NULL;
}

int ActivitySilkRoadsConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	if (!GetSubNodeValue(root_element, "buy_bag", m_other_cfg.buy_bag) || m_other_cfg.buy_bag <= 0)
	{
		errormsg = STRING_SPRINTF("buy_bag[%d] <= 0?", m_other_cfg.buy_bag);
		return -1;
	}

	if (!GetSubNodeValue(root_element, "buy_supply", m_other_cfg.buy_supply) || m_other_cfg.buy_supply <= 0)
	{
		errormsg = STRING_SPRINTF("buy_supply[%d] <= 0?", m_other_cfg.buy_supply);
		return -2;
	}

	if (!GetSubNodeValue(root_element, "act_num", m_other_cfg.act_num) || m_other_cfg.act_num <= 0)
	{
		errormsg = STRING_SPRINTF("act_num[%d] <= 0?", m_other_cfg.act_num);
		return -3;
	}

	if (!GetSubNodeValue(root_element, "start_bag", m_other_cfg.start_bag) || m_other_cfg.start_bag <= 0)
	{
		errormsg = STRING_SPRINTF("start_bag[%d] <= 0?", m_other_cfg.start_bag);
		return -4;
	}

	if (!GetSubNodeValue(root_element, "start_icon", m_other_cfg.start_coin) || m_other_cfg.start_coin <= 0)
	{
		errormsg = STRING_SPRINTF("start_icon[%d] <= 0?", m_other_cfg.start_coin);
		return -5;
	}

	if (!GetSubNodeValue(root_element, "start_supply", m_other_cfg.start_supply) || m_other_cfg.start_supply <= 0)
	{
		errormsg = STRING_SPRINTF("start_supply[%d] <= 0?", m_other_cfg.start_supply);
		return -6;
	}

	if (!GetSubNodeValue(root_element, "city_reward_group_10000", m_other_cfg.city_reward_group_rate) || m_other_cfg.city_reward_group_rate <= 0 || m_other_cfg.city_reward_group_rate > MAX_SILK_ROADS_REWAD_RATE_COUNT)
	{
		errormsg = STRING_SPRINTF("city_reward_group_10000[%d] <= 0 || > %d?", m_other_cfg.city_reward_group_rate, MAX_SILK_ROADS_REWAD_RATE_COUNT);
		return -7;
	}

	if (!GetSubNodeValue(root_element, "start_city", m_other_cfg.start_city_seq) || m_other_cfg.start_city_seq <= 0)
	{
		errormsg = STRING_SPRINTF("start_city[%d] <= 0?", m_other_cfg.start_city_seq);
		return -8;
	}

	if (!GetSubNodeValue(root_element, "start_supply_num", m_other_cfg.start_supply_num) || m_other_cfg.start_supply_num <= 0)
	{
		errormsg = STRING_SPRINTF("start_supply_num[%d] <= 0?", m_other_cfg.start_supply_num);
		return -9;
	}

	if (!GetSubNodeValue(root_element, "city_news", m_other_cfg.city_max_news) || m_other_cfg.city_max_news <= 0)
	{
		errormsg = STRING_SPRINTF("city_news[%d] <= 0?", m_other_cfg.city_max_news);
		return -10;
	}

	if (!GetSubNodeValue(root_element, "coin", m_other_cfg.find_coin) || m_other_cfg.find_coin < 0)
		return -11;

	TiXmlElement* item_element = root_element->FirstChildElement("reward_item");
	if (NULL == item_element) return -12;
	if (!m_other_cfg.find_reward.ReadConfig(item_element))
		return -13;

	return 0;
}

int ActivitySilkRoadsConfig::InitCityCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_city_map_seq = 0;
	while (NULL != root_element)
	{
		ActivitySilkRoadsCityCfg node_cfg;
		if (!GetSubNodeValue(root_element, "city_map", node_cfg.city_map_seq) || node_cfg.city_map_seq != last_city_map_seq + 1)
		{
			errormsg = STRING_SPRINTF("city_map[%d] != last_city_map_seq[%d] + 1?", node_cfg.city_map_seq, last_city_map_seq);
			return -1;
		}
		last_city_map_seq = node_cfg.city_map_seq;

		TiXmlElement * city_specialty_element = root_element->FirstChildElement("city_specialty");
		if (NULL == city_specialty_element)
		{
			errormsg = STRING_SPRINTF("city_map[%d], [city_specialty] not find?", node_cfg.city_map_seq);
			return -2;
		}
		TiXmlElement * node_data_element = city_specialty_element->FirstChildElement("node");
		if (NULL == node_data_element)
		{
			errormsg = STRING_SPRINTF("city_map[%d], [city_specialty] not list?", node_cfg.city_map_seq);
			return -3;
		}
		while (NULL != node_data_element)
		{
			int city_specialty = 0;
			if (!GetNodeValue(node_data_element, city_specialty) || city_specialty <= 0 || city_specialty > (int)m_commodity_list.size())
			{
				errormsg = STRING_SPRINTF("city_map[%d], [city_specialty] not find || commodity page not find city_specialty_seq[%d]?", node_cfg.city_map_seq, 
					city_specialty);
				return -4;
			}
			if (node_cfg.city_specialty_list.end() != node_cfg.city_specialty_list.find(city_specialty))
			{
				errormsg = STRING_SPRINTF("city_map[%d], city_specialty[%d] repeat?", node_cfg.city_map_seq, city_specialty);
				return -5;
			}

			node_cfg.city_specialty_list.insert(city_specialty);
			node_data_element = node_data_element->NextSiblingElement();
		}
		if (!GetSubNodeValue(root_element, "city_reward", node_cfg.city_reward_group_id) || m_city_reward_cfg.end() == m_city_reward_cfg.find(node_cfg.city_reward_group_id))
		{
			errormsg = STRING_SPRINTF("[city_map:%d], city_reward[%d] not find [city_reward]page?", node_cfg.city_map_seq, node_cfg.city_reward_group_id);
			return -6;
		}

		TiXmlElement * double_price_up_element = root_element->FirstChildElement("double_price_up_1");
		if (NULL == double_price_up_element)
		{
			errormsg = STRING_SPRINTF("city_map[%d], [double_price_up_1] not find?", node_cfg.city_map_seq);
			return -7;
		}
		TiXmlElement * node_element = double_price_up_element->FirstChildElement("node");
		if (NULL == node_element)
		{
			errormsg = STRING_SPRINTF("city_map[%d], [double_price_up_1] not list?", node_cfg.city_map_seq);
			return -8;
		}
		while (NULL != node_element)
		{
			int double_price_up = 0;
			if (!GetNodeValue(node_element, double_price_up) || double_price_up <= 0 || double_price_up > MAX_SILK_ROADS_REWAD_RATE_COUNT)
			{
				errormsg = STRING_SPRINTF("city_map[%d], [double_price_up_1] not find || double_price_up_1[%d] <= [0] || > [%d]?", node_cfg.city_map_seq,
					double_price_up, MAX_SILK_ROADS_REWAD_RATE_COUNT);
				return -9;
			}

			node_cfg.double_price_list.push_back(double_price_up);
			node_element = node_element->NextSiblingElement();
		}
		if ((int)node_cfg.double_price_list.size() > (int)node_cfg.city_specialty_list.size())
		{
			errormsg = STRING_SPRINTF("city_map[%d], [double_price_up_1] count[%d] > [city_specialty] count[%d]?", node_cfg.city_map_seq,
				(int)node_cfg.double_price_list.size(), (int)node_cfg.city_specialty_list.size());
			return -10;
		}

		m_city_map[node_cfg.city_map_seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	if (m_city_map.end() == m_city_map.find(m_other_cfg.start_city_seq))		//没有起点城镇的配置
	{
		errormsg = STRING_SPRINTF("other page[start_city:%d], not city_cfg page find?", m_other_cfg.start_city_seq);
		return -88;
	}

	return 0;
}

int ActivitySilkRoadsConfig::InitCommodityCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ActivitySilkRoadsCommodityCfg node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.city_specialty_seq) || node_cfg.city_specialty_seq != (int)m_commodity_list.size() + 1)
		{
			errormsg = STRING_SPRINTF("seq[%d] != %d?", node_cfg.city_specialty_seq, (int)m_commodity_list.size() + 1);
			return -1;
		}

		if (!GetSubNodeValue(root_element, "freight_market_price", node_cfg.freight_market_price) || node_cfg.freight_market_price <= 0)
		{
			errormsg = STRING_SPRINTF("[seq:%d], freight_market_price[%d] <= 0?", node_cfg.city_specialty_seq, node_cfg.freight_market_price);
			return -4;
		}

		if (!GetSubNodeValue(root_element, "min_price", node_cfg.min_price) || node_cfg.min_price <= -ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM)
		{
			errormsg = STRING_SPRINTF("[seq:%d], min_price [not find data] or <= [%d]?", node_cfg.city_specialty_seq, -ActivitySilkRoadsCommodityCfg::MAX_COMMODITY_BASE_NUM);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "max_price", node_cfg.max_price) || node_cfg.max_price < node_cfg.min_price || node_cfg.max_price <= 0)
		{
			errormsg = STRING_SPRINTF("[seq:%d], max_price[%d] < min_price[%d] or <= [0]?", node_cfg.city_specialty_seq, node_cfg.max_price, node_cfg.min_price);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "min_price_up", node_cfg.min_price_up) || node_cfg.min_price_up < node_cfg.max_price)
		{
			errormsg = STRING_SPRINTF("[seq:%d], min_price_up[%d] < max_price[%d]?", node_cfg.city_specialty_seq, node_cfg.min_price_up, node_cfg.max_price);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "max_price_up", node_cfg.max_price_up) || node_cfg.max_price_up < node_cfg.min_price_up)
		{
			errormsg = STRING_SPRINTF("[seq:%d], max_price_up[%d] < min_price_up[%d]?", node_cfg.city_specialty_seq, node_cfg.max_price_up, node_cfg.min_price_up);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "lowest_price", node_cfg.lowest_price) || node_cfg.lowest_price > node_cfg.freight_market_price)
		{
			errormsg = STRING_SPRINTF("[seq:%d], lowest_price[%d] < freight_market_price[%d]?", node_cfg.city_specialty_seq, 
				node_cfg.lowest_price, node_cfg.freight_market_price);
			return -7;
		}

		m_commodity_list.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	if ((int)m_commodity_list.size() > MAX_SILK_ROADS_CITY_COMMODITY_NUM)
	{
		errormsg = STRING_SPRINTF("commodity count[%d] > max_commodity[%d]?", (int)m_commodity_list.size(), MAX_SILK_ROADS_CITY_COMMODITY_NUM);
		return -88;
	}

	return 0;
}

int ActivitySilkRoadsConfig::InitShoppingCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{	
		int type = 0, freight_id = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type <= SILK_ROADS_LOG_TYPE_BEGIN || type >= SILK_ROADS_LOG_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("type[%d] <= [%d] or >= [%d]?", type, SILK_ROADS_LOG_TYPE_BEGIN, SILK_ROADS_LOG_TYPE_MAX);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "freight_id", freight_id) || freight_id <= 0)
		{
			errormsg = STRING_SPRINTF("freight_id[%d] <= 0?", freight_id);
			return -2;
		}
		if (m_shopping_list[type].end() != m_shopping_list[type].find(freight_id))
		{
			errormsg = STRING_SPRINTF("freight_id[%d] repeat?", freight_id);
			return -2;
		}

		m_shopping_list[type].insert(freight_id);
		root_element = root_element->NextSiblingElement();
	}
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_shopping_list); i++)
	{
		if(SILK_ROADS_LOG_TYPE_BEGIN == i) continue;
		
		if (m_shopping_list[i].empty())
		{
			errormsg = STRING_SPRINTF("type[%d] config empty?", i);
			return -55;
		}
	}
	
	return 0;
}

int ActivitySilkRoadsConfig::InitLineCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int line_group_id = 0;
		if (!GetSubNodeValue(root_element, "group", line_group_id) || line_group_id <= 0)
		{
			errormsg = STRING_SPRINTF("group[%d] <= 0?", line_group_id);
			return -1;
		}

		ActivitySilkRoadsLineCfg node_cfg;
		node_cfg.line_group_id = line_group_id;
		TiXmlElement * line_element = root_element->FirstChildElement("line");
		if (NULL == line_element)
		{
			errormsg = STRING_SPRINTF("group[%d], [line] not find?", line_group_id);
			return -2;
		}
		TiXmlElement * node_data_element = line_element->FirstChildElement("node");
		if (NULL == node_data_element)
		{
			errormsg = STRING_SPRINTF("group[%d], [line] not list?", line_group_id);
			return -3;
		}
		std::set<int> line_set;		//线路图不会路过重复地点
		while (NULL != node_data_element)
		{
			int line = 0;
			if (!GetNodeValue(node_data_element, line) || m_city_map.end() == m_city_map.find(line))
			{
				errormsg = STRING_SPRINTF("group[%d], city_cfg not find line[%d]?", line_group_id, line);
				return -4;
			}
			if (line_set.end() != line_set.find(line))
			{
				errormsg = STRING_SPRINTF("group[%d], line[%d] repeat?", line_group_id, line);
				return -5;
			}

			node_cfg.line_vec.push_back(line);
			node_data_element = node_data_element->NextSiblingElement();
		}
		//这里减一是因为起始城镇并不在路线图中
		if ((int)node_cfg.line_vec.size() > MAX_SILK_ROADS_LINE_NUM - 1)
		{
			errormsg = STRING_SPRINTF("group[%d], line count[%d] > %d?", line_group_id, (int)node_cfg.line_vec.size(), MAX_SILK_ROADS_LINE_NUM - 1);
			return -6;
		}
		for (int i = 0; i < (int)node_cfg.line_vec.size(); i++)
		{
			if (node_cfg.line_vec[i] == m_other_cfg.start_city_seq)
			{
				errormsg = STRING_SPRINTF("group[%d], line list has start_map_seq[%d]?", line_group_id, m_other_cfg.start_city_seq);
				return -7;
			}
		}

		m_line_map[line_group_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitBuyWarehouseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int buy_num = 0, price = 0;
		if (!GetSubNodeValue(root_element, "buy_num", buy_num) || buy_num <= 0 || m_buy_warehouse_pirce_map.end() != m_buy_warehouse_pirce_map.find(buy_num))
		{
			errormsg = STRING_SPRINTF("buy_num[%d] <= 0 || repeat?", buy_num);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "price", price) || price <= 0)
		{
			errormsg = STRING_SPRINTF("[buy_num:%d], price[%d] <= 0?", buy_num, price);
			return -2;
		}

		m_buy_warehouse_pirce_map[buy_num] = price;
		root_element = root_element->NextSiblingElement();
	}
	if (m_buy_warehouse_pirce_map.empty())
	{
		errormsg = STRING_SPRINTF("config empty?");
		return -66;
	}
	int max_bag_num = m_buy_warehouse_pirce_map.rbegin()->first * m_other_cfg.buy_bag + m_other_cfg.start_bag;
	if (max_bag_num > MAX_SILK_ROADS_GRID_NUM)
	{
		errormsg = STRING_SPRINTF("can expansion bag count + start_bag > %d?", MAX_SILK_ROADS_GRID_NUM);
		return -77;
	}

	return 0;
}

int ActivitySilkRoadsConfig::InitWarehouseExpendCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int warehouse_num = 0, supply_expend = 0;
		if (!GetSubNodeValue(root_element, "warehouse_num", warehouse_num) || warehouse_num <= 0 || m_warehouse_expend_map.end() != m_warehouse_expend_map.find(warehouse_num))
		{
			errormsg = STRING_SPRINTF("warehouse_num[%d] <= 0 || repeat?", warehouse_num);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "supply_expend", supply_expend) || supply_expend <= 0)
		{
			errormsg = STRING_SPRINTF("[warehouse_num:%d], supply_expend[%d] <= 0?", warehouse_num, supply_expend);
			return -2;
		}

		m_warehouse_expend_map[warehouse_num] = supply_expend;
		root_element = root_element->NextSiblingElement();
	}
	if (m_warehouse_expend_map.empty())
	{
		errormsg = STRING_SPRINTF("config empty?");
		return -88;
	}

	return 0;
}

int ActivitySilkRoadsConfig::InitBuySupplyPeiceCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int buy_num = 0, price = 0;
		if (!GetSubNodeValue(root_element, "buy_num", buy_num) || buy_num <= 0 || m_buy_supply_peice_map.end() != m_buy_supply_peice_map.find(buy_num))
		{
			errormsg = STRING_SPRINTF("buy_num[%d] <= 0 || repeat?", buy_num);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "price", price) || price <= 0)
		{
			errormsg = STRING_SPRINTF("[buy_num:%d], price[%d] <= 0?", buy_num, price);
			return -2;
		}

		m_buy_supply_peice_map[buy_num] = price;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitBuyNewsCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int buy_num = 0, price = 0;
		if (!GetSubNodeValue(root_element, "buy_num", buy_num) || buy_num <= 0 || m_buy_news_map.end() != m_buy_news_map.find(buy_num))
		{
			errormsg = STRING_SPRINTF("buy_num[%d] <= 0 || repeat?", buy_num);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "price", price) || price <= 0)
		{
			errormsg = STRING_SPRINTF("[buy_num:%d], price[%d] <= 0?", buy_num, price);
			return -2;
		}

		m_buy_news_map[buy_num] = price;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitSettlementBoxCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_levels = 0;
	int last_num_end = 0;
	while (NULL != root_element)
	{
		ActivitySilkRoadsSettlementBoxCfg node_cfg;
		if (!GetSubNodeValue(root_element, "levels", node_cfg.levels) || node_cfg.levels != last_levels + 1)
		{
			errormsg = STRING_SPRINTF("levels[%d] != last_levels + 1?", node_cfg.levels);
			return -1;
		}
		last_levels = node_cfg.levels;

		if (!GetSubNodeValue(root_element, "num_end", node_cfg.num_end) || node_cfg.num_end < last_num_end || 
			m_settlement_box_map.end() != m_settlement_box_map.find(node_cfg.num_end))
		{
			errormsg = STRING_SPRINTF("[levels:%d] num_end[%d] < last_num_end || repeat?", node_cfg.levels, node_cfg.num_end);
			return -2;
		}
		last_num_end = node_cfg.num_end;

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin <= 0)
		{
			errormsg = STRING_SPRINTF("[levels:%d] coin[%d] < 0?", node_cfg.levels, node_cfg.coin);
			return -3;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("reward_item");		//允许没道具奖励
		if (NULL != reward_element)
		{
			if (!node_cfg.reward_item.ReadConfig(reward_element))
			{
				errormsg = STRING_SPRINTF("[levels:%d] item[%d, %d]?", node_cfg.levels, node_cfg.reward_item.item_id, node_cfg.reward_item.num);
				return -4;
			}
		}

		m_settlement_box_map[node_cfg.num_end] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitRankListCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	ParamSection last_level_sec;
	while (NULL != root_element)
	{
		ParamSection level_sec;
		if (!GetSubNodeValue(root_element, "levels_min", level_sec.section_min) || level_sec.section_min <= last_level_sec.section_max)
		{
			errormsg = STRING_SPRINTF("levels_min[%d] <= last_levels_max[%d]?", level_sec.section_min, last_level_sec.section_max);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "levels_max", level_sec.section_max) || level_sec.section_max < level_sec.section_min ||
			level_sec.section_max <= last_level_sec.section_max)
		{
			errormsg = STRING_SPRINTF("levels_max[%d] < levels_min[%d] || <= last_levels_max[%d]?", level_sec.section_max, 
				level_sec.section_min,	last_level_sec.section_max);
			return -2;
		}

		ItemConfigData reward_item;
		TiXmlElement * reward_element = root_element->FirstChildElement("reward_item");		//允许没道具奖励
		if (NULL == reward_element)
		{	
			errormsg = STRING_SPRINTF("[levels_min:%d levels_max:%d] not find[reward_item]?", level_sec.section_min, level_sec.section_max);
			return -3;
		}
		if (!reward_item.ReadConfig(reward_element))
		{
			errormsg = STRING_SPRINTF("[levels_min:%d levels_max:%d] item[%d, %d]?", level_sec.section_min, level_sec.section_max,
				reward_item.item_id, reward_item.num);
			return -4;
		}

		m_rank_reward_map[level_sec] = reward_item;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitRoadsSkillCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	while (NULL != root_element)
	{
		int skill_id = 0;
		if (!GetSubNodeValue(root_element, "skill_id", skill_id) || skill_id <= 0 || m_skill_map.end() != m_skill_map.find(skill_id))
		{
			errormsg = STRING_SPRINTF("skill_id[%d] <= 0 || repeat?", skill_id);
			return -1;
		}
		ActivitySilkRoadsSkillCfg node_cfg;
		node_cfg.skill_id = skill_id;
		if (!GetSubNodeValue(root_element, "silk_roads_skill", node_cfg.skill_type) || node_cfg.skill_type <= ACTIVITY_SILK_ROADS_SKILL_TYPE_BEGIN || node_cfg.skill_type >= ACTIVITY_SILK_ROADS_SKILL_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[skill_id:%d], silk_roads_skill[%d] <= %d || >= %d?", skill_id, node_cfg.skill_type, ACTIVITY_SILK_ROADS_SKILL_TYPE_BEGIN, ACTIVITY_SILK_ROADS_SKILL_TYPE_MAX);
			return -2;
		}
		if (!GetSubNodeValue(root_element, "silk_roads_skill_num", node_cfg.skill_roads_skill_num) || node_cfg.skill_roads_skill_num <= 0)
		{
			errormsg = STRING_SPRINTF("[skill_id:%d], silk_roads_skill_num[%d] <= 0?", skill_id, node_cfg.skill_roads_skill_num);
			return -3;
		}
		if (ACTIVITY_SILK_ROADS_SKILL_TYPE_WAREHOUSE == node_cfg.skill_type)
		{
			int max_bag_num = m_buy_warehouse_pirce_map.rbegin()->first * m_other_cfg.buy_bag + m_other_cfg.start_bag + node_cfg.skill_roads_skill_num;
			if (max_bag_num > MAX_SILK_ROADS_GRID_NUM)
			{
				errormsg = STRING_SPRINTF("skill_id[%d]can expansion bag count + start_bag + skill_up > %d?", skill_id, MAX_SILK_ROADS_GRID_NUM);
				return -66;
			}
		}

		m_skill_map[skill_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	if (m_skill_map.empty())
	{
		errormsg = STRING_SPRINTF("config empty?");
		return -88;
	}
	return 0;
}

int ActivitySilkRoadsConfig::InitCityRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	while (NULL != root_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group_id) || reward_group_id <= 0)
		{
			errormsg = STRING_SPRINTF("reward_group[%d] <= 0?", reward_group_id);
			return -1;
		}

		ActivitySilkRoadsCityRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate_10000", node_cfg.rate) || node_cfg.rate <= 0 || node_cfg.rate > MAX_SILK_ROADS_REWAD_RATE_COUNT)
		{
			errormsg = STRING_SPRINTF("[reward_group:%d], rate_10000[%d] <= 0 || > %d?", reward_group_id, node_cfg.rate, MAX_SILK_ROADS_REWAD_RATE_COUNT);
			return -2;
		}
		TiXmlElement * reward_freight_element = root_element->FirstChildElement("reward_freight");
		if (NULL == reward_freight_element)
		{
			errormsg = STRING_SPRINTF("reward_group[%d], [reward_freight] not find?", reward_group_id);
			return -3;
		}
		TiXmlElement * node_data_element = reward_freight_element->FirstChildElement("node");
		if (NULL == node_data_element)
		{
			errormsg = STRING_SPRINTF("reward_group[%d], [reward_freight] not list?", reward_group_id);
			return -4;
		}
		while (NULL != node_data_element)
		{
			int reward_commodity_seq = 0;
			if (!GetNodeValue(node_data_element, reward_commodity_seq) || reward_commodity_seq <= 0 || reward_commodity_seq > (int)m_commodity_list.size())
			{
				errormsg = STRING_SPRINTF("reward_group[%d], [reward_freight] not find || commodity page not find reward_freight[%d]?", reward_group_id,
					reward_commodity_seq);
				return -5;
			}
			if (node_cfg.reward_commodity_list.end() != node_cfg.reward_commodity_list.find(reward_commodity_seq))
			{
				errormsg = STRING_SPRINTF("reward_group[%d], reward_freight[%d] repeat?", reward_group_id, reward_commodity_seq);
				return -6;
			}

			node_cfg.reward_commodity_list.insert(reward_commodity_seq);
			node_data_element = node_data_element->NextSiblingElement();
		}
		if (node_cfg.reward_commodity_list.empty())
		{
			errormsg = STRING_SPRINTF("reward_group[%d], [reward_freight] empty?", reward_group_id);
			return -7;
		}

		if (!GetSubNodeValue(root_element, "freight_num", node_cfg.commodity_count) || node_cfg.commodity_count <= 0 || node_cfg.commodity_count >= MAX_SILK_ROADS_GRID_NUM)
		{
			errormsg = STRING_SPRINTF("[reward_group:%d], freight_num[%d] <= 0 || >= %d?", reward_group_id, node_cfg.commodity_count, MAX_SILK_ROADS_GRID_NUM);
			return -8;
		}

		m_city_reward_cfg[reward_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<ActivitySilkRoadsCityRewardCfg> >::const_iterator it = m_city_reward_cfg.begin(); it != m_city_reward_cfg.end(); ++it)
	{
		int rate_count = 0;
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			rate_count += it->second[i].rate;
		}
		if (rate_count != MAX_SILK_ROADS_REWAD_RATE_COUNT)
		{
			errormsg = STRING_SPRINTF("[reward_group:%d], [rate_10000] sum:%d != %d?", it->first, rate_count, MAX_SILK_ROADS_REWAD_RATE_COUNT);
			return -55;
		}
	}
	
	return 0;
}
