#include "offlineexpconfig.hpp"

OfflineExpConfig::OfflineExpConfig()
{
}

OfflineExpConfig::~OfflineExpConfig()
{
}

bool OfflineExpConfig::Init(const std::string & configname, std::string * err)
{
//	PRE_LOAD_CONFIG;

//	LOAD_CONFIG("other", InitExpAccuOtherCfg);
//	LOAD_CONFIG("exp", InitExpAccuLevelCfg);

	return true;
}

unsigned int OfflineExpConfig::CalcOfflineExpAddition(int role_level, int offline_dur_m, int *out_m)
{
	*out_m = offline_dur_m - m_offline_exp_other_cfg.oe_start_calc_time;
	if (*out_m <= 0 || role_level < m_offline_exp_other_cfg.oe_open_level) return 0;

	std::map<int, float>::iterator f_it = m_exp_accumu_level_list.find(role_level);
	if (m_exp_accumu_level_list.end() == f_it) return 0;
	
	*out_m = *out_m > m_offline_exp_other_cfg.oe_max_calc_dur ? m_offline_exp_other_cfg.oe_max_calc_dur : *out_m;
	return *out_m * f_it->second;
}

int OfflineExpConfig::InitExpAccuOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "offline_time", m_offline_exp_other_cfg.oe_max_calc_dur) || m_offline_exp_other_cfg.oe_max_calc_dur < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "open_level", m_offline_exp_other_cfg.oe_open_level) || m_offline_exp_other_cfg.oe_open_level <= 0 || m_offline_exp_other_cfg.oe_open_level >= MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "start_time", m_offline_exp_other_cfg.oe_start_calc_time) || m_offline_exp_other_cfg.oe_start_calc_time < 0)
		{
			return -3;
		}
	}

	return 0;
}

int OfflineExpConfig::InitExpAccuLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level = 0;
		float exp_add = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL || m_exp_accumu_level_list.end() != m_exp_accumu_level_list.find(level))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "minute_exp", exp_add) || exp_add < 0)
		{
			return -2;
		}

		m_exp_accumu_level_list.insert(std::pair<int, float>(level, exp_add));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}