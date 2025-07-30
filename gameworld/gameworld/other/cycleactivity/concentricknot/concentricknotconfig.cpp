#include "concentricknotconfig.hpp"

ConcentricKnotConfig::ConcentricKnotConfig()
{
}

ConcentricKnotConfig::~ConcentricKnotConfig()
{
}

bool ConcentricKnotConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("activedegree", InitActiveCfg);

	return true;
}

int ConcentricKnotConfig::GetActiveRateCfg(int active)
{
	std::map<int, int>::iterator it = m_active_cfg.find(active);
	if (it == m_active_cfg.end())
		return 0;

	return it->second;
}

bool ConcentricKnotConfig::IsEndActiveRate(int active)
{
	std::map<int, int>::iterator it = m_active_cfg.end();
	if (it == m_active_cfg.begin())
		return false;
	it--;

	return active == it->first;
}

int ConcentricKnotConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	{
		TiXmlElement * list_element = root_element->FirstChildElement("item_list_first_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[item_list_first] error, not [item_list_first] ?");
			return -1;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("item_list_first");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -2;

			m_other_cfg.list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}

	if (!GetSubNodeValue(root_element, "play_time", m_other_cfg.play_time) || m_other_cfg.play_time <= 0)
	{
		errormsg = STRING_SPRINTF("[play_time=%d] error, play_time <= 0 ?", m_other_cfg.play_time);
		return -3;
	}

	{
		TiXmlElement * list_element = root_element->FirstChildElement("reward_1_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[reward_1] error, not [reward_1] ?");
			return -1;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("reward_1");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -2;

			m_other_cfg.reward.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}

	return 0;
}

int ConcentricKnotConfig::InitActiveCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int activedegree = 0;
		if (!GetSubNodeValue(root_element, "activedegree", activedegree) || activedegree <= 0)
		{
			errormsg = STRING_SPRINTF("[activedegree=%d] error, activedegree <= 0 ?", activedegree);
			return -1;
		}

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0)
		{
			errormsg = STRING_SPRINTF("[rate=%d] error, rate <= 0 ?", rate);
			return -2;
		}

		m_active_cfg[activedegree] = rate;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
