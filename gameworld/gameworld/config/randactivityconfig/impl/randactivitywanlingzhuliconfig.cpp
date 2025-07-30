#include "randactivitywanlingzhuliconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityWanLingZhuLiConfig::RandActivityWanLingZhuLiConfig()
{
}

RandActivityWanLingZhuLiConfig::~RandActivityWanLingZhuLiConfig()
{
	for (std::map<short, std::vector<RAWanLingZhuLiRewardCfg> >::iterator it = m_reward_cfg.begin(); it!= m_reward_cfg.end(); it++)
	{
		std::vector<RAWanLingZhuLiRewardCfg>().swap(it->second);
	}
}

bool RandActivityWanLingZhuLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAWanLingZhuLiRewardCfg * RandActivityWanLingZhuLiConfig::GetRewardCfg(RandActivityManager * ramgr, int day) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_WANLING_ZHULI);

	std::map<short, std::vector<RAWanLingZhuLiRewardCfg> >::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	--it;

	if (day < 0 || day >= (int)it->second.size())
		return NULL;

	return &it->second[day];
}

int RandActivityWanLingZhuLiConfig::GetRewardNum(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return 0;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_WANLING_ZHULI);

	std::map<short, std::vector<RAWanLingZhuLiRewardCfg> >::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return 0;
	--it;

	return (int)it->second.size();
}

int RandActivityWanLingZhuLiConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		short section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAWanLingZhuLiRewardCfg cfg;
		TiXmlElement * list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -2;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -3;

			cfg.reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_reward_cfg[section_start].push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityWanLingZhuLiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "price", m_other_cfg.price) || m_other_cfg.price <= 0)
		return -1;

	TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
	if (NULL == item_element)
		return -2;

	if (!m_other_cfg.reward_item.ReadConfig(item_element))
		return -3;

	return 0;
}
