#include "randactivitycollectwordconfig.hpp"


RandActivityCollectWordConfig::RandActivityCollectWordConfig()
{
}

RandActivityCollectWordConfig::~RandActivityCollectWordConfig()
{
	std::vector<RaCollectWordExchangeCfg>().swap(m_exchange_cfg);
}

bool RandActivityCollectWordConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("word_gift", InitExchangeCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RaCollectWordExchangeCfg * RandActivityCollectWordConfig::GetExchangeCfg(int seq) const
{
	if (seq < 0 || seq >= (int)m_exchange_cfg.size())
	{
		return NULL;
	}


	return &m_exchange_cfg[seq];
}

int RandActivityCollectWordConfig::InitExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RaCollectWordExchangeCfg cfg;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "type", seq) || seq != next_seq)
		{
			return -__LINE__;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "number", cfg.limit_exchange_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "total_exchange", cfg.limit_total_exchange_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "hearsay", cfg.is_broadcast))
		{
			return -__LINE__;
		}

		for (int i = 0; i < RA_COLLECT_WORD_MAX_CONSUME_ITEM; ++i)
		{
			ItemExtern::ItemConsumeConfig consume_cfg;

			char consume_id[1024] = { 0 };
			char consume_num[1024] = { 0 };

			SNPRINTF(consume_id, sizeof(consume_id), "words%d", i + 1);
			SNPRINTF(consume_num, sizeof(consume_num), "number%d", i + 1);

			if (!GetSubNodeValue(root_element, consume_id, consume_cfg.item_id))
			{
				return -__LINE__;
			}

			if (consume_cfg.item_id == 0)
			{
				break;
			}

			if (!GetSubNodeValue(root_element, consume_num, consume_cfg.num) || consume_cfg.num <= 0)
			{
				return -__LINE__;
			}

			cfg.consume_item.push_back(consume_cfg);
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("item");
		if (!cfg.reward.ReadConfig(reward_element))
		{
			return -__LINE__;
		}

		m_exchange_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_exchange_cfg.size() > RA_COLLECT_WORD_MAX_ENTRY_NUM)
	{
		return -__LINE__;
	}

	return 0;
}

int RandActivityCollectWordConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	TiXmlElement * list_element = root_element->FirstChildElement("day_reward_item_list");
	if (NULL != list_element)
	{
		TiXmlElement * reward_element = list_element->FirstChildElement("day_reward_item");
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element))
			{
				return -__LINE__;
			}
			if (m_other_cfg.count >= RA_COLLECT_WORD_MAX_DAY_GIFT_REWARD_NUM) return __LINE__;

			m_other_cfg.day_reward_list[m_other_cfg.count++] = item;
			reward_element = reward_element->NextSiblingElement();
		}
	}

	return 0;
}
