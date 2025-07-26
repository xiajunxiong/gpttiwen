#include "protocal/msglogin.h"
#include "loginserver.h"
#include "protocal/msgregister.h"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "rmibackobjdef.h"
#include "loginlog.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "platform/platform.h"

#include "loginusertrace.hpp"
#include "server/spidconfig.h"
#include "loginserver/randnamemanager/randnamemanager.hpp"
#include "server/loginmanager/loginmanager.hpp"

OLD_USE_INTERNAL_NETWORK();

void LoginServer::OnLoginToAccountReq(IP ip, NetID netid, const char *data)							// ÕÊºÅµÇÂ¼ 
{
	Protocol::CSLoginToAccount *scl = (Protocol::CSLoginToAccount *)data;
	scl->pname[sizeof(PlatName) - 1] = 0;

	//if (!m_platform->CheckAccount(netid, scl->pname, scl->login_time, scl->login_str, scl->server))
	//{
	//	loginlog::g_log_login.printf(LL_INFO, "OnLoginToAccountReq pname[%s] login_time[%u] server[%d] login_str[%s]", scl->pname, scl->login_time, scl->server, scl->login_str);
	//	return;
	//}

	LoginManager::GetInstance().OnLoginToAccountReq(ip, netid, scl->pname, scl->server);
}

void LoginServer::OnLoginToRoleReq(IP ip, NetID netid, const char *data)
{
	Protocol::CSLoginToRole *rr = (Protocol::CSLoginToRole*)data;
	rr->pname[sizeof(PlatName) - 1] = 0;

	if (!m_platform->CheckAccount(netid, rr->pname, rr->login_time, rr->login_str, rr->server))
	{
		loginlog::g_log_login.printf(LL_INFO, "OnLoginToRoleReq pname[%s] login_time[%u] server[%d] login_str[%s]", rr->pname, rr->login_time, rr->server, rr->login_str);
		return;
	}

	UserID user_id = IntToUid(rr->role_id);

	LoginManager::GetInstance().OnLoginToRoleReq(ip, netid, rr->pname, rr->role_id);
}

