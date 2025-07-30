
#include "gotoschoolconfig.hpp"

GotoSchoolConfig::GotoSchoolConfig()
{
}

GotoSchoolConfig::~GotoSchoolConfig()
{
}

bool GotoSchoolConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward", InitRewardCfg);

	return true;
}

const GotoSchoolOtherConfig & GotoSchoolConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int GotoSchoolConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "reward_times", m_other_cfg.can_get_reward_times))ret_line;
	if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id))ret_line;

	return 0;
}

int GotoSchoolConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "is_success", reward_type))
		{
			ret_line;
		}

		if (ReadItemConfigDataList(root_element, "reward_show", m_other_cfg.rewards[reward_type]))
		{
			ret_line;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
