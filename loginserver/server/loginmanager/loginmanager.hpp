#ifndef __LOGIN_MANAGER_HPP__
#define __LOGIN_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/logindef.h"
#include "inetworkmodule.h"

class RoleInfoList;
class LoginManager
{
public:
	static LoginManager& GetInstance();

	void OnLoginToAccountReq(IP ip, NetID netid, PlatName original_pname, int db_index);
	void OnLoginToRoleReq(IP ip, NetID netid, PlatName original_pname, int role_id);
	void OnCreateRoleReq(IP ip, NetID netid, PlatName pname, GameName role_name, char avatar_type, char color, int db_index, int plat_spid, 
		long long name_str_id,int job_id, int mini_game_id, int audit_server_seq);

	void SendLoginToAccountResult(NetID netid, int result, const RoleInfoList *role_info, PlatName original_panme, int db_index);

private:
	LoginManager();
	~LoginManager();

	LoginManager(const LoginManager&);
	LoginManager& operator=(const LoginManager &);

	int m_min_login_interval_s;
};

#endif