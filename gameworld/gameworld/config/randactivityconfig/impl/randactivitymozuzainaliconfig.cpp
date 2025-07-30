#include "randactivitymozuzainaliconfig.hpp"

RandActivityMoZuZaiNaLiConfig::RandActivityMoZuZaiNaLiConfig()
{
}

RandActivityMoZuZaiNaLiConfig::~RandActivityMoZuZaiNaLiConfig()
{
	std::vector<RAMoZuZaiNaLiPosiCfg>().swap(m_posi_cfg);
}

bool RandActivityMoZuZaiNaLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basics", InitOtherCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("coordinate", InitPosiCfg);

	return true;
}

const RAMoZuZaiNaLiRewardCfg * RandActivityMoZuZaiNaLiConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI);

	std::map<int, RAMoZuZaiNaLiRewardCfg>::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

int RandActivityMoZuZaiNaLiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0)
		return -1;

	return 0;
}

int RandActivityMoZuZaiNaLiConfig::InitRewardCfg(TiXmlElement * RootElement)
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

		RAMoZuZaiNaLiRewardCfg cfg;
		TiXmlElement * list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == list_element)
			return -1000;

		TiXmlElement * item_element = list_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -1001;

		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -1002;

			cfg.reward_item.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_reward_cfg[section_start] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMoZuZaiNaLiConfig::InitPosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		RAMoZuZaiNaLiPosiCfg cfg;
		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id) || cfg.scene_id <= 0)
			return -1;

		if (!GetSubNodeValue(root_element, "npc_id", cfg.npc_id))
			return -2;

		if (!GetSubNodeValue(root_element, "x", cfg.pos.x) || cfg.pos.x <= 0)
			return -3;
		if (!GetSubNodeValue(root_element, "y", cfg.pos.y) || cfg.pos.y <= 0)
			return -4;

		m_posi_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
