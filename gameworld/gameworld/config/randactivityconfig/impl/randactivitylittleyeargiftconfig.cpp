#include "randactivitylittleyeargiftconfig.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/ralittleyeargiftparam.hpp"
#include "npc//npcpool.h"

RandActivityLittleYearGiftConfig::RandActivityLittleYearGiftConfig()
{
}

RandActivityLittleYearGiftConfig::~RandActivityLittleYearGiftConfig()
{
	for (std::map<long long, std::vector<RALittleYearGiftCfg> >::iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		std::vector<RALittleYearGiftCfg>().swap(it->second);
	}
	std::vector<RALittleYearGiftThreadBankCfg>().swap(m_thread_bank_vec);
}

bool RandActivityLittleYearGiftConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitLittleYearGiftCfg);
	LOAD_RA_CONFIG("thread_bank", InitThreadBankCfg);

	return true;
}

const RALittleYearGiftCfg * RandActivityLittleYearGiftConfig::GetLittleYearGiftCfgByActivityDay(int open_day, int thread_id) const
{
	for (std::map<long long, std::vector<RALittleYearGiftCfg> >::const_iterator it = m_section_map.begin(); it != m_section_map.end(); it++)
	{
		int section_start = 0;
		int section_end = 0;
		this->GetSectionByKey(it->first, &section_start, &section_end);
		if (section_start <= open_day && section_end >= open_day)
		{
			for (std::vector<RALittleYearGiftCfg>::const_iterator itor = it->second.begin(); itor != it->second.end(); itor++)
			{
				if (itor->thread_id == thread_id)
				{
					return &(*itor);
				}
			}
		}
	}
	return NULL;
}

const RALittleYearGiftThreadBankCfg * RandActivityLittleYearGiftConfig::RandThreadCfg() const
{
	if (bank_weight_count > 0)
	{
		int rand_num = rand() % bank_weight_count;
		for (std::vector<RALittleYearGiftThreadBankCfg>::const_iterator it = m_thread_bank_vec.begin(); it != m_thread_bank_vec.end(); it++)
		{
			if (rand_num <= it->rate)
			{
				return &(*it);
			}
			rand_num -= it->rate;
		}
	}
	
	return NULL;
}

int RandActivityLittleYearGiftConfig::InitLittleYearGiftCfg(TiXmlElement * RootElement)
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

		RALittleYearGiftCfg &node_cfg = m_index_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0) return -2;

		if (!GetSubNodeValue(data_element, "thread_id", node_cfg.thread_id) || node_cfg.thread_id < 0) return -3;

		if (!GetSubNodeValue(data_element, "NPC_id", node_cfg.npc_id)) return -4;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element)return -8;
		
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (reward_element != NULL)
		{
			ItemConfigData node_reward;
			if (!node_reward.ReadConfig(reward_element))
			{
				return -9;
			}
			node_cfg.reward_item_vec.push_back(node_reward);
			reward_element = reward_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(data_element, "section_start", node_cfg.section_start) || node_cfg.section_start < 0) return -10;
		if (!GetSubNodeValue(data_element, "section_end", node_cfg.section_end) || node_cfg.section_end < 0) return -11;

		long long key = this->GetKeyBySection(node_cfg.section_start, node_cfg.section_end);
		m_section_map[key].push_back(node_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLittleYearGiftConfig::InitThreadBankCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	bank_weight_count = 0;
	while (NULL != data_element)
	{
		RALittleYearGiftThreadBankCfg node_cfg;
		if (!GetSubNodeValue(data_element, "type", node_cfg.type) || node_cfg.type < 0) return -1;

		if (!GetSubNodeValue(data_element, "thread_id", node_cfg.thread_id) || node_cfg.thread_id < 0) return -2;

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0) return -3;
		bank_weight_count += node_cfg.rate;
		m_thread_bank_vec.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityLittleYearGiftConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivityLittleYearGiftConfig::GetSectionByKey(long long key, int * section_start, int * section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	*section_start = un_key.section[0];
	*section_end = un_key.section[1];
}