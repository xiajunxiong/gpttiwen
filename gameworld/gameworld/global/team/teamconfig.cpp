#include "teamconfig.hpp"

TeamConfig::TeamConfig() : m_team_type_cfg_num(0)
{

}

TeamConfig::~TeamConfig()
{

}

bool TeamConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("team_target", InitTeamTypeCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const TeamTypeCfg* TeamConfig::GetTeamTypeCfg(int team_type) const
{
	if (team_type < 0 || team_type >= m_team_type_cfg_num) return NULL;

	if (!m_team_type_cfg_list[team_type].is_valid) return NULL;

	return &m_team_type_cfg_list[team_type];
}

int TeamConfig::GetTeamTypeTargetType(int team_type)
{
	const TeamTypeCfg* cfg = this->GetTeamTypeCfg(team_type);
	if (NULL == cfg) return 0;

	return cfg->team_type_target_type;
}

bool TeamConfig::IsTeamTypeLimitMaxMember(int team_type, ARG_OUT int* max_member_num) const
{
	const TeamTypeCfg* cfg = this->GetTeamTypeCfg(team_type);
	if (NULL == cfg) return false;

	if (cfg->max_member_num > 0)
	{
		if (NULL != max_member_num) *max_member_num = cfg->max_member_num;

		return true;
	}

	return false;
}

int TeamConfig::InitTeamTypeCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	int max_team_type = 0;
	while (dataElement)
	{
		int team_type = 0;
		if (!GetSubNodeValue(dataElement, "target_id", team_type) || team_type < 0 || team_type >= TEAM_TYPE_MAX)
		{
			return -1;
		}

		// TODO 命名有点别扭，待优化，暂时不纠结
		int team_type_target_type = 0;
		if (!GetSubNodeValue(dataElement, "team_type", team_type_target_type) || team_type_target_type < 0 || team_type_target_type >= TEAM_TYPE_TARGET_TYPE_MAX)
		{
			return -111;
		}

		TeamTypeCfg& cfg = m_team_type_cfg_list[team_type];
		cfg.team_type = team_type;
		cfg.team_type_target_type = team_type_target_type;

		if (!GetSubNodeValue(dataElement, "low_level", cfg.low_level_limit) || cfg.low_level_limit < 0 || cfg.low_level_limit > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "high_level", cfg.high_level_limit) || cfg.high_level_limit < cfg.low_level_limit || cfg.high_level_limit > MAX_ROLE_LEVEL)
		{
			return -3;
		}
		int is_creat_team = 0;
		if (!GetSubNodeValue(dataElement, "is_creat_team", is_creat_team))
		{
			return -4;
		}

		int is_pop_team = 0;
		if (!GetSubNodeValue(dataElement, "is_pop_team", is_pop_team))
		{
			return -5;
		}

		int max_member_num = 0;
		if (GetSubNodeValue(dataElement, "max_member_num", max_member_num))
		{
			if (max_member_num < 0 || max_member_num > MAX_TEAM_MEMBER_NUM)
			{
				return -6;
			}
		}

		cfg.is_creat_team = (0 != is_creat_team) ? true : false;
		cfg.is_recruit_team = (is_pop_team > 0);
		cfg.is_valid = true;
		cfg.max_member_num = max_member_num;

		max_team_type = GetMax(team_type, max_team_type);

		dataElement = dataElement->NextSiblingElement();
	}
	m_team_type_cfg_num = max_team_type + 1;

	return 0;
}

int TeamConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "pop_cooling_time", 
		m_other_cfg.refuse_team_recruit_time_s))
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "ignore_time",
		m_other_cfg.ignore_team_recruit_s))
	{
		return -2;
	}

	return 0;
}

