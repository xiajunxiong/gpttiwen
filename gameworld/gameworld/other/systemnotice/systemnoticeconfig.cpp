#include "systemnoticeconfig.hpp"
#include "item/itempool.h"

SystemNoticeConfig::SystemNoticeConfig()
{
}

SystemNoticeConfig::~SystemNoticeConfig()
{
}

bool SystemNoticeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("advancenotice", InitAwardList);

	return true;
}

const SystemNoticeAwardCon * SystemNoticeConfig::GetSystemNoticeAwardCfg(int seq)
{
	std::map<int, SystemNoticeAwardCon>::iterator f_it = m_award_cfg.find(seq);
	if (m_award_cfg.end() != f_it) return &f_it->second;

	return NULL;
}

int SystemNoticeConfig::InitAwardList(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_seq = 0; SystemNoticeAwardCon t_snac;
		if (!GetSubNodeValue(root_element, "id", t_seq) || t_seq < 0 || t_seq >= 32)
		{
			return -1;
		}

		if (m_award_cfg.empty() && 0 != t_seq)		// 必须从 0 开始
		{
			return -2;
		}

		if (!m_award_cfg.empty() && m_award_cfg.rbegin()->first + 1 != t_seq)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "end_level", t_snac.need_level) || t_snac.need_level < 0)
		{
			return -4;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_item_list");
		if (NULL == award_list) return -2000;

		TiXmlElement * award_item = award_list->FirstChildElement("reward_item");
		while (NULL != award_item)
		{
			if (t_snac.award_num >= SYSTEM_NOTICE_NUM_MAX) return -5;
			
			if (!t_snac.award_list[t_snac.award_num++].ReadConfig(award_item))
			{
				return -6;
			}

			award_item = award_item->NextSiblingElement();
		}

		m_award_cfg.insert(std::pair<int, SystemNoticeAwardCon>(t_seq, t_snac));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
