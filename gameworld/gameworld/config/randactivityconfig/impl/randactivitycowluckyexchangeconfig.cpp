#include "randactivitycowluckyexchangeconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityCowLuckyExchangeConfig::RandActivityCowLuckyExchangeConfig()
{
}

RandActivityCowLuckyExchangeConfig::~RandActivityCowLuckyExchangeConfig()
{
	for (std::map<long long, std::vector<RACowLuckyExchangeCfg> >::iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		std::vector<RACowLuckyExchangeCfg>().swap(it->second);
	}
}

bool RandActivityCowLuckyExchangeConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitCowLuckyExchangeCfg);

	return true;
}

const RACowLuckyExchangeCfg * RandActivityCowLuckyExchangeConfig::GetCowLuckyExchangeCfgByIndex(int id) const
{
	std::map<int, RACowLuckyExchangeCfg>::const_iterator it = m_index_map.find(id);
	if (it != m_index_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RACowLuckyExchangeCfg * RandActivityCowLuckyExchangeConfig::GetCowLuckyExchangeCfgByActivityChongzhi(long long chongzhi_num, int id) const
{
	for (std::map<long long, std::vector<RACowLuckyExchangeCfg> >::const_iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		int accumulated_down = 0;
		int accumulated_up = 0;
		this->GetSectionByKey(it->first, &accumulated_down, &accumulated_up);
		if (accumulated_down <= chongzhi_num && accumulated_up >= chongzhi_num)
		{
			for (std::vector<RACowLuckyExchangeCfg>::const_iterator itor = it->second.begin(); itor != it->second.end(); itor++)
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

const RACowLuckyExchangeCfg * RandActivityCowLuckyExchangeConfig::RandCowLuckyExchangeCfgByChongzhi(long long chongzhi_num) const
{
	for (std::map<long long, std::vector<RACowLuckyExchangeCfg> >::const_iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		int accumulated_down = 0;
		int accumulated_up = 0;
		this->GetSectionByKey(it->first, &accumulated_down, &accumulated_up);
		if (accumulated_down <= chongzhi_num && accumulated_up >= chongzhi_num)
		{
			std::map<long long, int>::const_iterator it_weight = m_section_weight_map.find(it->first);
			if (it_weight == m_section_weight_map.end() || it_weight->second <= 0)
			{
				return NULL;
			}
			int rand_num = rand() % it_weight->second;
			for (std::vector<RACowLuckyExchangeCfg>::const_iterator itor = it->second.begin(); itor != it->second.end(); itor++)
			{
				if (itor->rate >= rand_num)
				{
					return &(*itor);
				}
				rand_num -= itor->rate;
			}
			break;
		}
	}
	return NULL;
}

int RandActivityCowLuckyExchangeConfig::InitCowLuckyExchangeCfg(TiXmlElement * RootElement)
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

		RACowLuckyExchangeCfg &node_cfg = m_index_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_COW_LOG_TIMES) return -2;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price < 0) return -3;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0) return -3;

		TiXmlElement * reward_element_list = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_element_list)return -4;
		TiXmlElement * reward_element = reward_element_list->FirstChildElement("reward_item");
		while (reward_element != NULL)
		{
			ItemConfigData rewar_node;
			if (!rewar_node.ReadConfig(reward_element))
			{
				return -9;
			}
			node_cfg.reward_item_vec.push_back(rewar_node);
			reward_element = reward_element->NextSiblingElement();
		}
		

		if (!GetSubNodeValue(data_element, "accumulated_down", node_cfg.accumulated_down) || node_cfg.accumulated_down < 0) return -10;
		if (!GetSubNodeValue(data_element, "accumulated_up", node_cfg.accumulated_up) || node_cfg.accumulated_up < 0) return -11;

		long long key = this->GetKeyBySection(node_cfg.accumulated_down, node_cfg.accumulated_up);
		if (m_section_weight_map.find(key) == m_section_weight_map.end())
		{
			m_section_weight_map[key] = node_cfg.rate;
		}
		else
		{
			m_section_weight_map[key] += node_cfg.rate;
		}
		m_section_map[key].push_back(node_cfg);
		
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityCowLuckyExchangeConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivityCowLuckyExchangeConfig::GetSectionByKey(long long key, int * section_start, int * section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	*section_start = un_key.section[0];
	*section_end = un_key.section[1];
}