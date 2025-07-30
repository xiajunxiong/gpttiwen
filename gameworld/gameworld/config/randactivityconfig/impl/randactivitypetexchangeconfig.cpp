#include "randactivitypetexchangeconfig.hpp"

#include "engineadapter.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "world.h"

RandActivityPetExchangeConfig::RandActivityPetExchangeConfig()
{

}

RandActivityPetExchangeConfig::~RandActivityPetExchangeConfig()
{
	std::vector<RAPetExchangeShopSectionCfg>().swap(m_shop_section_cfg);
}

bool RandActivityPetExchangeConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("pet_exchange", InitPetExchangeCfg);
	LOAD_RA_CONFIG("item_buy", InitPetExchangeShopCfg);

	return true;
}

const RAPetExchangeCfg* RandActivityPetExchangeConfig::GetPetExchangeBySeq(RandActivityManager* ramgr, int seq) const
{
	if (seq < 0 || seq >= MAX_RA_PET_NUM)return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PET_EXCHANGE);
	for (std::map<int, std::map<int, RAPetExchangeCfg> >::const_iterator it = m_pet_exchange_map.begin(); it != m_pet_exchange_map.end(); ++it)
	{
		std::map<int, RAPetExchangeCfg>::const_iterator it2 = it->second.begin();
		if (it2->second.section_start <= act_real_open_day && (it2->second.section_end == 0 || it2->second.section_end >= act_real_open_day))
		{
			std::map<int, RAPetExchangeCfg>::const_iterator it3 = it->second.find(seq);
			if (it3 != it->second.end())
			{
				return &it3->second;
			}
		}
	}

	return NULL;
}

const RAPetExchangeShopCfg * RandActivityPetExchangeConfig::GetShopBySeq(RandActivityManager * ramgr, int seq) const
{
	if(NULL == ramgr || seq < 0 || seq >= MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PET_EXCHANGE);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day &&
			seq < (int)m_shop_section_cfg[i].shop_list.size())
		{
			return &m_shop_section_cfg[i].shop_list[seq];
		}
	}
	return NULL;
}

const RAPetExchangeShopSectionCfg * RandActivityPetExchangeConfig::GetShopSectionCfg(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return NULL;
	
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_PET_EXCHANGE);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_shop_section_cfg[i];
		}
	}
	return NULL;
}

int RandActivityPetExchangeConfig::GetActOpenDay(RandActivityManager* ramgr, int activity_type) const
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

int RandActivityPetExchangeConfig::GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	if (NULL == ramgr) return 0;

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int act_real_open_day = GetDayIndex(open_server_timestamp, activity_begin_time) + 1; // 活动开启时的开服天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityPetExchangeConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityPetExchangeConfig::InitOtherCfg(TiXmlElement * RootElement)
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

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver < 0)
	{
		return -1;
	}

	return 0;
}

int RandActivityPetExchangeConfig::InitPetExchangeCfg(TiXmlElement *RootElement)
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
		std::map<int, RAPetExchangeCfg> &node_cfg = m_pet_exchange_map[seq];


		int seq1 = -1;
		if (!GetSubNodeValue(data_element, "seq1", seq1) || seq1 < 0 || seq1 >= MAX_RA_PET_NUM)
		{
			return -2;
		}
		RAPetExchangeCfg &node = node_cfg[seq1];
		node.seq1 = seq1;

		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || node.section_start <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || (node.section_end != 0 && node.section_end < node.section_start))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "daily_purchase", node.daily_purchase) || node.daily_purchase < 0)
		{
			return -5;
		}


		if (!GetSubNodeValue(data_element, "hearsay", node.hearsay) || node.hearsay < 0)
		{
			return -6;
		}

		TiXmlElement *item_element = data_element->FirstChildElement("item");
		if (item_element == NULL)
		{
			return -7;
		}
		if (!node.reward_item.ReadConfig(item_element))
		{
			return -8;
		}

		TiXmlElement *consume_item_list_element = data_element->FirstChildElement("consume_item_list");
		if (NULL == consume_item_list_element)
		{
			return -9;
		}
		TiXmlElement *consume_item_element = consume_item_list_element->FirstChildElement("consume_item");
		if (NULL == consume_item_element)
		{
			return -10;
		}
		while (NULL != consume_item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(consume_item_element))
			{
				return -11;
			}

			node.consume_item_list.push_back(item);
			consume_item_element = consume_item_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(data_element, "today_times", node.today_times) || node.today_times < 0)
		{
			return -12;
		}

		if (!GetSubNodeValue(data_element, "limit_type", node.limit_type) || node.limit_type <= 0 || node.limit_type >= ACTVITY_BUY_LIMIT_TYPE_MAX)
		{
			return -13;
		}
		if (ACTVITY_BUY_LIMIT_TYPE_ALL == node.limit_type && node.daily_purchase <= 0)
		{
			return -14;
		}
		if (ACTVITY_BUY_LIMIT_TYPE_DAY == node.limit_type && node.today_times <= 0)
		{
			return -15;
		}
		if (!GetSubNodeValue(data_element, "grade_limit", node.open_level_limit) || node.open_level_limit < 0 || node.open_level_limit > MAX_ROLE_LEVEL)
		{
			return -16;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityPetExchangeConfig::InitPetExchangeShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
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

				m_shop_section_cfg.push_back(RAPetExchangeShopSectionCfg());
				next_seq = 0;
			}
		}
		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_shop_section_cfg.push_back(RAPetExchangeShopSectionCfg());
		}
		RAPetExchangeShopSectionCfg & node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAPetExchangeShopCfg shop_cfg;

		if (!GetSubNodeValue(data_element, "seq", shop_cfg.seq) || shop_cfg.seq != next_seq || shop_cfg.seq < 0 || 
			shop_cfg.seq >= MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM) return -1;
		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type)) return -2;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times) || shop_cfg.buy_times < 0) return -3;

		int is_cmd_buy = 0;
		if (!GetSubNodeValue(data_element, "cfg_or", is_cmd_buy)) return -4;
		shop_cfg.is_cmd_buy = (0 != is_cmd_buy) ? true : false;
		
		if (!GetSubNodeValue(data_element, "buy_money", shop_cfg.price) || shop_cfg.price <= 0) return -5;

		if (!shop_cfg.is_cmd_buy)
		{
			if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < 0 ||
				shop_cfg.price_type >= MONEY_TYPE_MAX) return -6;
		}
	
		TiXmlElement * reward_element = data_element->FirstChildElement("item");
		if (reward_element == NULL)
		{
			return -100;
		}

		if (!shop_cfg.reward_item.ReadConfig(reward_element))
		{
			return -101;
		}

		node_cfg.shop_list.push_back(shop_cfg);
		next_seq = (int)node_cfg.shop_list.size();
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
