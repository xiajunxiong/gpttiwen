#include "randactivityspringfestivalstoreconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivitySpringFestivalStoreConfig::RandActivitySpringFestivalStoreConfig()
{
}

RandActivitySpringFestivalStoreConfig::~RandActivitySpringFestivalStoreConfig()
{
	for (std::map<long long, std::map<long long, std::vector<RASpringFestivalStoreCfg> > >::iterator it = m_section_map.begin(); it != m_section_map.end(); it++){
		for (std::map<long long, std::vector<RASpringFestivalStoreCfg> >::iterator node_it = it ->second.begin(); node_it != it->second.end(); node_it++){
			std::vector<RASpringFestivalStoreCfg>().swap(node_it->second);
		}
	}
}

bool RandActivitySpringFestivalStoreConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitSpringFestivalStoreCfg);

	return true;
}

const RASpringFestivalStoreCfg * RandActivitySpringFestivalStoreConfig::GetSpringFestivalStoreCfgByIndex(int id) const
{
	std::map<int, RASpringFestivalStoreCfg>::const_iterator it =  m_index_map.find(id);
	if (it != m_index_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RASpringFestivalStoreCfg * RandActivitySpringFestivalStoreConfig::GetSpringFestivalStoreCfgByActivityDay(int server_open_day, int open_day, int id) const
{
	for (std::map<long long, std::map<long long, std::vector<RASpringFestivalStoreCfg> > >::const_iterator begin_it = m_section_map.begin(); begin_it != m_section_map.end(); begin_it++)
	{
		int time_start = 0;
		int time_end = 0;
		this->GetSectionByKey(begin_it->first, &time_start, &time_end);
		if (time_start <= server_open_day && time_end >= server_open_day)
		{
			for (std::map<long long, std::vector<RASpringFestivalStoreCfg> >::const_iterator it = begin_it->second.begin(); it != begin_it->second.end(); it++)
			{
				int section_start = 0;
				int section_end = 0;
				this->GetSectionByKey(it->first, &section_start, &section_end);
				if (section_start <= open_day && section_end >= open_day)
				{
					for (std::vector<RASpringFestivalStoreCfg>::const_iterator itor = it->second.begin(); itor != it->second.end(); itor++)
					{
						if (itor->seq == id)
						{
							return &(*itor);
						}
					}
				}
			}
		}
	}
	return NULL;
}

int RandActivitySpringFestivalStoreConfig::InitSpringFestivalStoreCfg(TiXmlElement * RootElement)
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

		RASpringFestivalStoreCfg &node_cfg = m_index_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase) || node_cfg.phase < 0) return -2;
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_LOG_TIMES) return -3;

		if (!GetSubNodeValue(data_element, "limit_type", node_cfg.limit_type) || node_cfg.limit_type < 0) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", node_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", node_cfg.today_times)) return -5;
		if (!GetSubNodeValue(data_element, "price_type", node_cfg.price_type) || node_cfg.price_type < RASpringFestivalStoreCfg::PRICE_TYPE_MIN ||
			node_cfg.price_type > RASpringFestivalStoreCfg::PRICE_TYPE_MONEY) return -6;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price < 0) return -7;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element)return -8;

		if (!node_cfg.item_reward.ReadConfig(reward_element))
		{
			return -9;
		}

		if (!GetSubNodeValue(data_element, "section_start", node_cfg.section_start) || node_cfg.section_start < 0) return -10;
		if (!GetSubNodeValue(data_element, "section_end", node_cfg.section_end) || node_cfg.section_end < 0) return -11;
		if (node_cfg.section_end == 0)
		{
			node_cfg.section_end = MAX_INT;
		}
		long long key = this->GetKeyBySection(node_cfg.section_start, node_cfg.section_end);

		if (!GetSubNodeValue(data_element, "start_time", node_cfg.start_time) || node_cfg.start_time < 0) return -10;
		if (!GetSubNodeValue(data_element, "end_time", node_cfg.end_time) || node_cfg.end_time < 0) return -11;
		if (node_cfg.end_time == 0)
		{
			node_cfg.end_time = MAX_INT;
		}
		long long key2 = this->GetKeyBySection(node_cfg.start_time, node_cfg.end_time);

		m_section_map[key2][key].push_back(node_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivitySpringFestivalStoreConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivitySpringFestivalStoreConfig::GetSectionByKey(long long key, int * section_start, int * section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	*section_start = un_key.section[0];
	*section_end = un_key.section[1];
}