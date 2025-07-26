#ifndef __SPID_CONFIG_H__
#define __SPID_CONFIG_H__

#include <map>
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include <set>
#include "inetworkmodule.h"

struct SpidConfigItem
{
	SpidConfigItem() : new_role_limit_days(0), new_role_limit_role_num(0) {}

	int new_role_limit_days;
	int new_role_limit_role_num;
};

struct AuditServerCfg
{
	AuditServerCfg() : avatar_type(0), birth_level(0)
	{}

	int avatar_type;
	std::set<int> job_list;
	int birth_level;
	int scene_group_id;
};

struct AuditServerBirthSceneCfg
{
	AuditServerBirthSceneCfg() : birth_scene_id(0), birth_pos(0, 0)
	{}

	int birth_scene_id;
	Posi birth_pos;
};

#define SPID_CONFIG SpidConfig::Instance()

class SpidConfig
{
public:
	static SpidConfig * Instance();

	bool Init(const std::string &path, std::string *err);
	bool Init_1(const std::string &path, std::string *err);
	bool CanCreateNewRole(int spid, IP ip = 0);
	static bool Reload(const std::string& path, std::string* err);
	int SetCreateRoleControlSwitch(int is_open);
	int SetOnlineRoleNumThreshold(int threshold);
	int SetAutoCreateRoleSwitch(int is_open);
	int GetAutoCreateRoleSwitch() { return m_auto_create_role_switch; }
	int GetOnlineRoleNumThreshold() const { return m_online_role_num_threshold; }
	bool IsLimitCreateRole(int cur_online_role_num);
	bool IsLimitLogin(int cur_online_role_num);

	int GetSpidByStrPlatName(const std::string& str_plat_name);
	int GetPlatNameBySpid(int spid, char plat_name[4]);

	const AuditServerCfg * GetAuditServerCfg(int audit_seq, const AuditServerBirthSceneCfg ** birth_cfg);

private:
	int InitAgentAdapt(TiXmlElement *RootElement);
	int InitAllowedIPList(TiXmlElement *RootElement);
	int InitAuditServerCfg(TiXmlElement *RootElement);
	int InitAuditServerBirthSceneCfg(TiXmlElement *RootElement);
	IP GetIP(std::string ip_str);

	SpidConfig();
	~SpidConfig();
	SpidConfig(const SpidConfig&);
	SpidConfig& operator=(const SpidConfig &);

	std::map<int, SpidConfigItem> m_spid_cfg_map;
	std::set<IP> m_allow_ip_set;
	int m_create_role_control_switch;
	int m_auto_create_role_switch;
	std::vector<AuditServerCfg> m_audit_server_cfg;			//审核服配置
	std::map<int, std::vector<AuditServerBirthSceneCfg> > m_audit_server_birth_map;	//审核服出生点位配置
	int m_online_role_num_threshold;
};

#endif	// __LOCAL_CONFIG_H__

