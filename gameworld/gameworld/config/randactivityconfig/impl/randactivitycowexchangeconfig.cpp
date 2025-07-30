#include "randactivitycowexchangeconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityCowExchangeConfig::RandActivityCowExchangeConfig()
{
}

RandActivityCowExchangeConfig::~RandActivityCowExchangeConfig()
{
	for (std::map<long long, std::vector<RACowExchangeCfg> >::iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		std::vector<RACowExchangeCfg>().swap(it->second);
	}
}

bool RandActivityCowExchangeConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitCowExchangeCfg);

	return true;
}

const RACowExchangeCfg * RandActivityCowExchangeConfig::GetCowExchangeCfgByIndex(int id) const
{
	std::map<int, RACowExchangeCfg>::const_iterator it = m_index_map.find(id);
	if (it != m_index_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RACowExchangeCfg * RandActivityCowExchangeConfig::GetCowExchangeCfgByActivityDay(int open_day, int id) const
{
	for (std::map<long long, std::vector<RACowExchangeCfg> >::const_iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		int section_start = 0;
		int section_end = 0;
		this->GetSectionByKey(it->first, &section_start, &section_end);
		if (section_start <= open_day && section_end >= open_day)
		{
			for (std::vector<RACowExchangeCfg>::const_iterator itor = it->second.begin(); itor != it->second.end(); itor++)
			{
				if (itor->seq == id)
				{
					return &(*itor);
				}
			}
		}
	}
	return NULL;
}

int RandActivityCowExchangeConfig::InitCowExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0) return -1;

		RACowExchangeCfg &node_cfg = m_index_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_COW_LOG_TIMES) return -3;

		if (!GetSubNodeValue(data_element, "limit_type", node_cfg.limit_type) || node_cfg.limit_type < 0) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", node_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", node_cfg.today_times)) return -5;
		if (!GetSubNodeValue(data_element, "unlock_integral", node_cfg.unlock_integral) || node_cfg.unlock_integral < 0) return -6;
		if (!GetSubNodeValue(data_element, "price_type", node_cfg.price_type) || node_cfg.price_type < RACowExchangeCfg::PRICE_TYPE_GOLD ||
			node_cfg.price_type > RACowExchangeCfg::PRICE_TYPE_MONEY) return -7;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price < 0) return -8;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element)return -9;
		
		if (!node_cfg.reward_item.ReadConfig(reward_element))
		{
			return -9;
		}

		if (!GetSubNodeValue(data_element, "section_start", node_cfg.section_start) || node_cfg.section_start < 0) return -10;
		if (!GetSubNodeValue(data_element, "section_end", node_cfg.section_end) || node_cfg.section_end < 0) return -11;

		long long key = this->GetKeyBySection(node_cfg.section_start, node_cfg.section_end);
		m_section_map[key].push_back(node_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityCowExchangeConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivityCowExchangeConfig::GetSectionByKey(long long key, int * section_start, int * section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	*section_start = un_key.section[0];
	*section_end = un_key.section[1];
}