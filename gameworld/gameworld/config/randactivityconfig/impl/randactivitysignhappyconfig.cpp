#include "randactivitysignhappyconfig.hpp"

RandActivitySignHappyConfig::RandActivitySignHappyConfig()
{
}

RandActivitySignHappyConfig::~RandActivitySignHappyConfig()
{
	for (std::map<int, std::vector<RASignHappyRewardCfg> >::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		std::vector<RASignHappyRewardCfg>().swap(it->second);
	}
}

bool RandActivitySignHappyConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardCfg);

	return true;
}

const RASignHappyRewardCfg * RandActivitySignHappyConfig::GetRewardCfg(RandActivityManager * ramgr, int index) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SIGN_HAPPY);

	std::map<int, std::vector<RASignHappyRewardCfg> >::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);

	if (it == m_reward_cfg.begin())
		return NULL;

	it--;
	if (index < 0 || index >= (int)it->second.size())
		return NULL;

	return &it->second[index];
}

int RandActivitySignHappyConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(root_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RASignHappyRewardCfg cfg;
		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
			if (NULL == list_element)
				return -1000;

			TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				if (cfg.reward_count >= sizeof(cfg.reward_item) / sizeof(ItemConfigData))
					return -1001;

				if (!cfg.reward_item[cfg.reward_count].ReadConfig(item_element))
					return -2;

				cfg.reward_count++;
				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement * list_element = root_element->FirstChildElement("reward_item1_list");
			if (NULL != list_element)
			{
				TiXmlElement * item_element = list_element->FirstChildElement("reward_item1");
				while (NULL != item_element)
				{
					if (cfg.reward_count_1 >= sizeof(cfg.reward_item_1) / sizeof(ItemConfigData))
						return -2001;

					if (!cfg.reward_item_1[cfg.reward_count_1].ReadConfig(item_element))
						return -2;

					cfg.reward_count_1++;
					item_element = item_element->NextSiblingElement();
				}
			}
		}

		m_reward_cfg[section_start].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
