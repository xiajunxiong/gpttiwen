#include "randactivityxunmenghudieconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityXunMengHuDieConfig::RandActivityXunMengHuDieConfig()
{
}

RandActivityXunMengHuDieConfig::~RandActivityXunMengHuDieConfig()
{
	std::vector<RAXunMengHuDieSceneCfg>().swap(m_scene_cfg);
}

bool RandActivityXunMengHuDieConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("scene", InitSceneCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RAXunMengHuDieRewardCfg * RandActivityXunMengHuDieConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_XUNMENG_HUDIE);

	std::map<short, RAXunMengHuDieRewardCfg>::const_iterator it = m_reward_cfg.upper_bound(act_real_open_day);
	if (it == m_reward_cfg.begin())
		return NULL;
	it--;

	return &it->second;
}

int RandActivityXunMengHuDieConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		RAXunMengHuDieSceneCfg cfg;
		if (!GetSubNodeValue(data_element, "scene_id", cfg.scene_id) || cfg.scene_id <= 0)
			return -1;

		if (!GetSubNodeValue(data_element, "npc_id", cfg.npc_id) || cfg.npc_id <= 0)
			return -2;

		if (!GetSubNodeValue(data_element, "x", cfg.pos.x) || cfg.pos.x <= 0)
			return -3;
		if (!GetSubNodeValue(data_element, "y", cfg.pos.y) || cfg.pos.y <= 0)
			return -3;

		m_scene_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityXunMengHuDieConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
			return -1;

		RAXunMengHuDieRewardCfg cfg;
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

		m_reward_cfg[section_start] = cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityXunMengHuDieConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency <= 0)
		return -1;

	return 0;
}
