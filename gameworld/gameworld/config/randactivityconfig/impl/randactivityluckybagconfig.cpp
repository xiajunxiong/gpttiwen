#include "randactivityluckybagconfig.hpp"

RandActivityLuckyBagConfig::RandActivityLuckyBagConfig()
{
}

RandActivityLuckyBagConfig::~RandActivityLuckyBagConfig()
{
	std::vector<RandActivityLuckyBagBuyCfg>().swap(m_lucky_bag_buy_cfg);
	std::vector<RandActivityLuckyBagBatchBuyCfg>().swap(m_lucky_bag_batch_buy_cfg);
}

bool RandActivityLuckyBagConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitBuyCfg);
	LOAD_RA_CONFIG("reward_group", InitBatchBuyCfg);

	return true;
}

const RandActivityLuckyBagBuyCfg * RandActivityLuckyBagConfig::GetRandActivityLuckyBagBuyCfg(int seq) const
{
	if (seq < 0 || seq >= (int)m_lucky_bag_buy_cfg.size())
	{
		return NULL;
	}

	return &m_lucky_bag_buy_cfg[seq];
}

const RandActivityLuckyBagOtherCfg & RandActivityLuckyBagConfig::GetRandActivityLuckyBagOtherCfg() const
{
	return m_lucky_bag_other_cfg;
}

const RandActivityLuckyBagBatchBuyCfg * RandActivityLuckyBagConfig::GetRandActivityLuckyBagBatchBuyCfg(int seq) const
{
	if (seq < 0 || seq >= (int)m_lucky_bag_batch_buy_cfg.size())
	{
		return NULL;
	}

	return &m_lucky_bag_batch_buy_cfg[seq];
}

int RandActivityLuckyBagConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RandActivityLuckyBagBuyCfg cfg;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq || seq >= sizeof(int) * 8)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "money", cfg.need_chong_zhi_gold) || cfg.need_chong_zhi_gold <= 0)
		{
			return -2;
		}

		int luck_coin_num = 0;
		if (!GetSubNodeValue(root_element, "praycoin", luck_coin_num))
		{
			return -3;
		}

		ItemID luck_coin_item = 0;
		if (!GetSubNodeValue(root_element, "blessing_item", luck_coin_item) || NULL == ITEMPOOL->GetItem(luck_coin_item))
		{
			return -4;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("rewards_item_list");
		if (reward_list_element == NULL)
		{
			return -__LINE__;
		}

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("rewards_item");
		while (reward_element)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -104;
			}
			cfg.reward.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		if (luck_coin_num > 0)
		{
			ItemConfigData luck_coin_reward;
			luck_coin_reward.item_id = luck_coin_item;
			luck_coin_reward.num = luck_coin_num;
			luck_coin_reward.is_bind = 1;
			cfg.reward.push_back(luck_coin_reward);
		}
		

		m_lucky_bag_buy_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
int RandActivityLuckyBagConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	TiXmlElement * reward_element = root_element->FirstChildElement("rewards_item");
	ItemConfigData reward;
	if (!reward.ReadConfig(reward_element))
	{
		return -104;
	}
	m_lucky_bag_other_cfg.day_reward.push_back(reward);

	return 0;
}

int RandActivityLuckyBagConfig::InitBatchBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		RandActivityLuckyBagBatchBuyCfg cfg;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq || seq >= sizeof(int) * 8)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "money", cfg.need_chong_zhi_gold) || cfg.need_chong_zhi_gold <= 0)
		{
			return -2;
		}

		int luck_coin_num = 0;
		if (!GetSubNodeValue(root_element, "praycoin", luck_coin_num))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "activity_days", cfg.add_day))
		{
			return -3333;
		}

		ItemID luck_coin_item = 0;
		if (!GetSubNodeValue(root_element, "blessing_item", luck_coin_item) || NULL == ITEMPOOL->GetItem(luck_coin_item))
		{
			return -4;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("rewards_item_list");
		if (reward_list_element == NULL)
		{
			return -__LINE__;
		}



		TiXmlElement * reward_element = reward_list_element->FirstChildElement("rewards_item");
		while (reward_element)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -104;
			}
			cfg.batch_buy_reward.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		if (luck_coin_num > 0)
		{
			ItemConfigData luck_coin_reward;
			luck_coin_reward.item_id = luck_coin_item;
			luck_coin_reward.num = luck_coin_num;
			luck_coin_reward.is_bind = 1;
			cfg.batch_buy_reward.push_back(luck_coin_reward);
		}


		m_lucky_bag_batch_buy_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_lucky_bag_batch_buy_cfg.size() != 1)  //只有7天的，不然要改代码
	{
		return -__LINE__;
	}

	return 0;
}
