#ifndef __TEAM_CONFIG_HPP__
#define __TEAM_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/teamdef.hpp"

struct TeamTypeCfg
{
	TeamTypeCfg() : is_valid(false), team_type(0), team_type_target_type(0), 
		low_level_limit(0), high_level_limit(0), is_creat_team(false),
		is_recruit_team(false), max_member_num(0){}

	bool is_valid;
	int team_type;
	int team_type_target_type;
	int low_level_limit;
	int high_level_limit;
	bool is_creat_team;
	bool is_recruit_team;
	int max_member_num;
};

struct TeamOtherCfg
{
	TeamOtherCfg() : ignore_team_recruit_s(0), 
		refuse_team_recruit_time_s(0){}

	int ignore_team_recruit_s;
	int refuse_team_recruit_time_s;
};

class TeamConfig : public ILogicConfig
{
public:
	TeamConfig();
	virtual ~TeamConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TeamTypeCfg* GetTeamTypeCfg(int team_type) const;
	int GetTeamTypeTargetType(int team_type);

	const TeamOtherCfg& GetOtherCfg() const { return m_other_cfg; }
	bool IsTeamTypeLimitMaxMember(int team_type, ARG_OUT int* max_member_num) const;
private:
	int InitTeamTypeCfg(TiXmlElement* RootElement);
	int InitOtherCfg(TiXmlElement* RootElement);

	int m_team_type_cfg_num;
	TeamTypeCfg m_team_type_cfg_list[TEAM_TYPE_MAX];

	TeamOtherCfg m_other_cfg;
};

#endif