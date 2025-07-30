#include "randactivitypartnerexchangeconfig.hpp"

#include "engineadapter.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "world.h"

RandActivityPartnerExchangeConfig::RandActivityPartnerExchangeConfig()
{

}

RandActivityPartnerExchangeConfig::~RandActivityPartnerExchangeConfig()
{

}

bool RandActivityPartnerExchangeConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("partner_exchange", InitPartnerExchangeCfg);

	return true;
}

const RAPartnerExchangeCfg* RandActivityPartnerExchangeConfig::GetPartnerExchangeBySeq(RandActivityManager* ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;

	if (seq < 0 || seq >= MAX_RA_PARTNER_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE);
	for (std::map<int, std::map<int, RAPartnerExchangeCfg> >::const_iterator it = m_partner_exchange_map.begin(); it != m_partner_exchange_map.end(); ++it)
	{
		std::map<int, RAPartnerExchangeCfg>::const_iterator it2 = it->second.begin();
		if (it2->second.section_start <= act_real_open_day && (it2->second.section_end == 0 || it2->second.section_end >= act_real_open_day))
		{
			std::map<int, RAPartnerExchangeCfg>::const_iterator it3 = it->second.find(seq);
			if (it3 != it->second.end())
			{
				return &it3->second;
			}
		}
	}

	return NULL;
}

int RandActivityPartnerExchangeConfig::GetActOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	if (NULL == ramgr) return 0;

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	int act_real_open_day = GetDayIndex(activity_begin_time, cur_time) + 1; // 活动开启天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityPartnerExchangeConfig::GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	if (NULL == ramgr)
	{
		return 0;
	}

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int act_real_open_day = GetDayIndex(open_server_timestamp, activity_begin_time) + 1; // 活动开启时的开服天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityPartnerExchangeConfig::InitPartnerExchangeCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}

	while (NULL != data_element)
	{
		int seq = -1;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		std::map<int, RAPartnerExchangeCfg> &node_cfg = m_partner_exchange_map[seq];


		int seq1 = -1;
		if (!GetSubNodeValue(data_element, "seq1", seq1) || seq1 < 0 || seq1 >= MAX_RA_PARTNER_NUM)
		{
			return -2;
		}
		RAPartnerExchangeCfg &node = node_cfg[seq1];
		node.seq1 = seq1;

		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || node.section_start <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || (node.section_end != 0 && node.section_end < node.section_start))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "condition_id", node.condition_id) || node.condition_id < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "item_id", node.item_id) || node.item_id < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "condition_num", node.condition_num) || node.condition_num < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(data_element, "hearsay", node.hearsay) || node.hearsay < 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(data_element, "buy_tepy", node.buy_tepy) || node.buy_tepy < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(data_element, "buy_seccond_day", node.buy_seccond_day) || node.buy_seccond_day < 0)
		{
			return -10;
		}

		if (!GetSubNodeValue(data_element, "number", node.number) || node.number < 0)
		{
			return -11;
		}

		TiXmlElement *item1_element = data_element->FirstChildElement("item1");
		if (item1_element != NULL)
		{
			int item_id = 0;
			if (GetSubNodeValue(item1_element, "item_id", item_id) && NULL != ITEMPOOL->GetItem(item_id))
			{
				if (!node.item1.ReadConfig(item1_element))
				{
					return -12;
				}
			}
		}

		TiXmlElement *item2_element = data_element->FirstChildElement("item2");
		if (item2_element != NULL)
		{
			int item_id = 0;
			if (GetSubNodeValue(item2_element, "item_id", item_id) && NULL != ITEMPOOL->GetItem(item_id))
			{
				if (!node.item2.ReadConfig(item2_element))
				{
					return -13;
				}
			}
		}

		TiXmlElement *item3_element = data_element->FirstChildElement("item3");
		if (item3_element != NULL)
		{
			int item_id = 0;
			if (GetSubNodeValue(item3_element, "item_id", item_id) && NULL != ITEMPOOL->GetItem(item_id))
			{
				if (!node.item3.ReadConfig(item3_element))
				{
					return -14;
				}
			}
		}

		TiXmlElement *item_element = data_element->FirstChildElement("item");
		if (item_element == NULL)
		{
			return -15;
		}
		if (!node.reward_item.ReadConfig(item_element))
		{
			return -16;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
