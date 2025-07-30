#include "randactivityhappyupconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityHappyUpConfig::RandActivityHappyUpConfig()
{

}

RandActivityHappyUpConfig::~RandActivityHappyUpConfig()
{
	std::vector<RAHappyUpSectionCfg>().swap(m_happy_up_section_cfg);
}

bool RandActivityHappyUpConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("happy_up", InitCfg);

	return true;
}

const RAHappyUpItemCfg* RandActivityHappyUpConfig::GetHappyUpItemCfg(RandActivityManager* ramgr, int item_seq)const
{
	if (item_seq < 0 || item_seq >= MAX_HAPPY_UP_ITEM_SEQ_NUM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HAPPY_UP);
	for (std::vector<RAHappyUpSectionCfg>::const_iterator it = m_happy_up_section_cfg.begin(); it != m_happy_up_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if ((int)it->item.size() <= item_seq) break;

			return &it->item[item_seq];
		}
	}

	return NULL;
}

int RandActivityHappyUpConfig::GetHappyUpSectionItemCount(RandActivityManager* ramgr)const
{
	if (NULL == ramgr) return 0;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HAPPY_UP);
	for (std::vector<RAHappyUpSectionCfg>::const_iterator it = m_happy_up_section_cfg.begin(); it != m_happy_up_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return (int)it->item.size();
		}
	}
	return 0;
}

const RAHappyUpSectionCfg * RandActivityHappyUpConfig::GetHappyUpSectionCfg(RandActivityManager* ramgr)const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HAPPY_UP);
	for (std::vector<RAHappyUpSectionCfg>::const_iterator it = m_happy_up_section_cfg.begin(); it != m_happy_up_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}
	return NULL;
}

int RandActivityHappyUpConfig::InitCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
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

		if (m_happy_up_section_cfg.size() > 0)
		{
			int pre_index = m_happy_up_section_cfg.size() - 1;
			if (section_start != m_happy_up_section_cfg[pre_index].section_start ||
				section_end != m_happy_up_section_cfg[pre_index].section_end)
			{
				if (section_start < m_happy_up_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_happy_up_section_cfg.push_back(RAHappyUpSectionCfg());
			}
		}
		if (m_happy_up_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_happy_up_section_cfg.push_back(RAHappyUpSectionCfg());
		}

		RAHappyUpSectionCfg & node_cfg = m_happy_up_section_cfg[m_happy_up_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int item_seq;
		if (!GetSubNodeValue(data_element, "item_seq", item_seq) || item_seq < 0 || item_seq >= MAX_HAPPY_UP_ITEM_SEQ_NUM) return -1;
		RAHappyUpItemCfg item_cfg;
		item_cfg.item_seq = item_seq;

		TiXmlElement * item_element = data_element->FirstChildElement("exchange_item");
		if (NULL != item_element)
		{
			if (!item_cfg.exchange_item.ReadConfig(item_element)) return -100;
		}
		if (!GetSubNodeValue(data_element, "consume_item_id", item_cfg.consume_item_id) || item_cfg.consume_item_id < 0) return -2;
		if (!GetSubNodeValue(data_element, "consume_item_count", item_cfg.consume_item_count) || item_cfg.consume_item_count < 0) return -3;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.today_times) || item_cfg.today_times < 0) return -4;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.buy_times) || item_cfg.buy_times < 0) return -5;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type) || item_cfg.limit_type <= HAPPY_UP_LIMIT_TYPE_BEGIN ||
			item_cfg.limit_type >= HAPPY_UP_LIMIT_TYPE_MAX) return -6;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.hearsay) || item_cfg.hearsay < 0) return -7;

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

