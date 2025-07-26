#include "protocal/msglogin.h"
#include "loginserver.h"
#include "protocal/msgregister.h"
#include "rmiclient/rmidaroleclient.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "servercommon/servercommon.h"
#include "roleinitattr.h"
#include "servercommon/darmi/darole.h"
#include "rmibackobjdef.h"
#include "loginlog.h"
#include "platform/platform.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/namefilter.hpp"
#include "servercommon/logdef.h"
#include "loginserver/randnamemanager/randnamemanager.hpp"
#include "server/spidconfig.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "server/loginmanager/loginmanager.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/errornum.h"

USE_DATAACCESS_VAR();
OLD_USE_INTERNAL_NETWORK();
USE_DATAACCESS_ROLE_VAR();

int GetSpidByPlatName(PlatName pname)
{
	PlatTypeUnion tmp_union;
	tmp_union.spid[0] = tmp_union.spid[1] = tmp_union.spid[2] = tmp_union.spid[3] = 0;
	for (unsigned int i = 0; i < 3; i++)
	{
		if ('\0' != pname[i])
		{
			tmp_union.spid[i + 1] = pname[i];	// 这样做是为了确保id大于0 字节序的关系
		}
	}

	return tmp_union.id;
}

long long GenRoleNameStrId(const char* role_name) {
	// 推荐用字符串hash，hash冲突概率极低
    std::hash<std::string> hasher;
    return std::llabs(static_cast<long long>(hasher(role_name)));
}

void LoginServer::OnCreateRole(IP ip, NetID netid, const char *data)
{
	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] called. ip:%u netid:%d data_ptr:%p", ip, netid, data);


	Protocol::CSCreateRole *cr = (Protocol::CSCreateRole*)data;
	cr->pname[sizeof(PlatName) - 1] = 0;
	cr->plat_spid_str[sizeof(cr->plat_spid_str) - 1] = 0;

	if (cr->server >= MAX_DB_IDNEX || cr->server < 0)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] server index invalid. server:%d", cr->server);
		return;		// 超过一个平台允许的服数
	}

	if (!m_platform->CheckAccount(netid, cr->pname, cr->login_time, cr->login_str, cr->server))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] CheckAccount failed. pname:%s login_time:%u server:%d login_str:%s", cr->pname, cr->login_time, cr->server, cr->login_str);
		return;
	}

	if (!AppearanceConfig::Instance().CheckAvatarAndColor(cr->avatar_type, cr->color, cr->job_id))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] Avatar and color not match. avatar_type:%d color:%d job_id:%d", cr->avatar_type, cr->color, cr->job_id);
		Protocol::SCNoticeNum errormsg;
		errormsg.notice_num = errornum::EN_AVATAR_COLOR_NOT_MATCH;
		EngineAdapter::Instance().NetSend(netid, (const char*)&errormsg, sizeof(errormsg));

		return; // 虚拟人物类型与形象ID不匹配
	}



	int user_spid = GetSpidByPlatName(cr->plat_spid_str);
	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] user_spid:%d plat_spid_str:%s", user_spid, cr->plat_spid_str);

	if (0 != cr->is_auto_rand_name)
	{
		int sex = AppearanceConfig::Instance().GetSexByAvatar(cr->avatar_type);
		NameConfig::Instance()->GetRandomName(sex, cr->role_name);
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] Auto random name. sex:%d role_name:%s", sex, cr->role_name);
	}
	
	if (0 == cr->role_name[0]) 
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] role_name is empty after random/fill.");
		return;
	}
	
	if (cr->name_str_id == 0) 
	{
	cr->name_str_id = GenRoleNameStrId(cr->role_name);
	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] auto gen name_str_id: %lld for role_name: %s", cr->name_str_id, cr->role_name);
	}
	
	if (HasInvalidChar(cr->role_name))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] role_name has invalid char: %s", cr->role_name);
		// 名字不合法
		Protocol::SCCreateRoleAck cra;
		cra.result = CREATE_ROLE_RESULT_TYPE_NAME_INVALID;
		cra.role_id = 0;
		memset(cra.role_name, 0, sizeof(GameName));
		cra.reserve_sh = 0;
		cra.avartar_type = 0;
		cra.color = 0;
		cra.create_role_time = 0;
		EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(cra));

		return;
	}

	if (SPID_CONFIG->IsLimitCreateRole(LoginServer::Instance().GetOnlineRoleNum()))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] server too hot to create new role. online_num:%d", LoginServer::Instance().GetOnlineRoleNum());
		Protocol::SCNoticeNum errormsg;
		errormsg.notice_num = errornum::EN_SERVER_TOO_HOT_TO_CREATE_NEW_ROLE;
		EngineAdapter::Instance().NetSend(netid, (const char*)&errormsg, sizeof(errormsg));

		return;
	}

	InvalidCharFilte(cr->role_name);
	RoleNameCatPostfix(cr->role_name, cr->server);
	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] role_name after postfix: %s", cr->role_name);
// 	if (0 > cr->name_str_id)
// 	{
// 		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] name_str_id invalid: %lld", cr->name_str_id);
// 		return;
// 	}

	int spid = LocalConfig::Instance().GetPlatType();
	if (!SPID_CONFIG->CanCreateNewRole(spid, ip))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] CanCreateNewRole failed. spid:%d ip:%u", spid, ip);
		// 服务器已达上限
		Protocol::SCCreateRoleAck cra;
		cra.result = CREATE_ROLE_RESULT_TYPE_OLD_SERVER_NEW_USER;
		cra.role_id = 0;
		memset(cra.role_name, 0, sizeof(GameName));
		cra.reserve_sh = 0;
		cra.avartar_type = 0;
		cra.color = 0;
		cra.create_role_time = 0;
		EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(cra));

		return;
	}

	static const bool NAMEFILTER_OPEN_FLAG = ServerConfigPool::Instance().common_config.namefilter_open_flag;
	if (NAMEFILTER_OPEN_FLAG && NameFilter::Instance().HasSensitiveWord(sizeof(cr->role_name), cr->role_name)) 
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] name has sensitive word: %s", cr->role_name);
		// 名字不合法
		Protocol::SCCreateRoleAck cra;
		cra.result = CREATE_ROLE_RESULT_TYPE_NAME_INVALID;
		cra.role_id = 0;
		memset(cra.role_name, 0, sizeof(GameName));
		cra.reserve_sh = 0;
		cra.avartar_type = 0;
		cra.color = 0;
		cra.create_role_time = 0;
		EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(cra));

		return;
	}

	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRole] Call OnCreateRoleReq with ip:%u netid:%d pname:%s role_name:%s avatar_type:%d color:%d server:%d user_spid:%d name_str_id:%lld job_id:%d mini_game_id:%d audit_server_seq:%d",
		ip, netid, cr->pname, cr->role_name, cr->avatar_type, cr->color, cr->server, user_spid, cr->name_str_id,
		cr->job_id, cr->mini_game_id, cr->audit_server_seq);

	LoginManager::GetInstance().OnCreateRoleReq(ip, netid, cr->pname, cr->role_name, cr->avatar_type, cr->color, cr->server, user_spid, cr->name_str_id, 
		cr->job_id, cr->mini_game_id, cr->audit_server_seq);
}

void RMIDeleteNameInfoBackObjectImpl::DeleteNameInfoRet(int result)
{
	if (result == 0)
	{
		//printf("DeleteNameInfoRet:%d\n", result);
	}
}

void RMIDestroyRoleBackObjectImpl::DestroyRoleRet(int ret)
{
	if (ret != 0)
	{
		RMIAddRoleBackObject *crbo = new RMIAddRoleBackObject();
		RMILoginClient lc;
		lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
		lc.AddRoleAsyn(pname, user_id.db_index, user_id.role_id, crbo);

		Protocol::SCDestroyRoleAck cra;
		cra.role_id = UidToInt(user_id);
		cra.result = -1;
		EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCDestroyRoleAck));

		loginlog::g_log_destory.printf(LL_ERROR, "[RMIDestroyRoleBackObjectImpl ret:%d] pname:%s user_id[%d, %d].", ret, pname, user_id.db_index, user_id.role_id);

		return ;
	}

	Protocol::SCDestroyRoleAck cra;
	cra.role_id = UidToInt(user_id);
	cra.result =  0;
	EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCDestroyRoleAck));

	loginlog::g_log_destory.printf(LL_INFO, "[RMIRemoveRoleBackObjectImpl Success] pname:%s user_id[%d].", pname, UidToInt(user_id));
}

void RMIRemoveRoleBackObjectImpl::RemoveRoleRet(int ret)
{
	if (ret != 0)
	{
		Protocol::SCDestroyRoleAck cra;
		cra.role_id = UidToInt(user_id);
		cra.result = -1;
		EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCDestroyRoleAck));

		loginlog::g_log_destory.printf(LL_ERROR, "[RMIRemoveRoleBackObjectImpl ret:%d] pname:%s user_id[%d, %d].", ret, pname, user_id.db_index, user_id.role_id);
		return;
	}
	
	RMIDestroyRoleBackObjectImpl *drboi = new RMIDestroyRoleBackObjectImpl();
	drboi->user_id = user_id;
	drboi->netid = netid;
	F_STRNCPY(drboi->pname, pname, sizeof(drboi->pname));

	RMILRoleClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
	bool result = lc.DestroyRoleAsyn(UidToInt(user_id), drboi);
	if (!result)
	{
		loginlog::g_log_destory.printf(LL_ERROR, "[RMIRemoveRoleBackObjectImpl RMIError] pname:%s user_id[%d, %d].", pname, user_id.db_index, user_id.role_id);
	}

	if (result)
	{
		RMIDeleteNameInfoBackObjectImpl *dnibo = new RMIDeleteNameInfoBackObjectImpl();
		RMILoginClient loginc;
		loginc.__bind_session(InternalComm::Instance().GetDBHandler().GetNameDB());
		loginc.DeleteNameInfoAsyn(user_id.db_index, UidToInt(user_id), pname, dnibo);
	}
}

void LoginServer::OnDestroyRole(NetID netid, const char *data)
{
	//Protocol::CSDestroyRole *dr = (Protocol::CSDestroyRole*)data;
	//dr->pname[sizeof(PlatName) - 1] = 0;

	//char anti_wallow;
	//if (!m_platform->CheckAccount(netid, dr->pname, dr->login_time, dr->anti_wallow, dr->login_str, &anti_wallow, dr->server)) return ;

	//UserID user_id = IntToUid(dr->role_id);
	//
	//PlatName pname;
	//F_STRNCPY(pname, dr->pname, sizeof(PlatName));

	//UserItem *user = m_user_manager.GetUser(user_id);
	//if (user != 0)
	//{
	//	// 踢一次
	//	KickUser(user_id);

	//	Protocol::SCDestroyRoleAck cra;
	//	cra.role_id = UidToInt(user_id);
	//	cra.result = -2;
	//	EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCDestroyRoleAck));

	//	loginlog::g_log_destory.printf(LL_WARNING, "[LoginServer::OnDestroyRole UserHasLogin] pname:%s user_id[%d, %d] login_time:%u.", pname, 
	//		user_id.db_index, user_id.role_id, (unsigned int)user->GetLoginTime());
	//	return;
	//}

	//RMIRemoveRoleBackObjectImpl *rrboi = new RMIRemoveRoleBackObjectImpl();
	//rrboi->user_id = user_id;
	//rrboi->netid = netid;
	//F_STRNCPY(rrboi->pname, pname, sizeof(rrboi->pname));

	//RMILoginClient lc;
	//lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	//bool result = lc.RemoveRoleAsyn(pname, user_id.db_index, UidToInt(user_id), rrboi);
	//if (!result)
	//{
	//	loginlog::g_log_destory.printf(LL_ERROR, "[LoginServer::OnDestroyRole RMIError] pname:%s user_id[%d, %d].", pname, user_id.db_index, user_id.role_id);
	//}
}

void LoginServer::OnGetRandomName(NetID netid, const char* data)
{
	// 获取随机名字，发送随机名字的id到客户端
	Protocol::CSRandomNameReq *rnr = (Protocol::CSRandomNameReq*)data;
	RandNameManager::Instance().OnGetRandNameId(netid,rnr->sex);
}

void RMIInitNameStrIdBackObjectImpl::InitListRet(std::set<long long> name_str_id_list)
{
	//printf("Succ");
}

void RMIInitNameStrIdBackObject::InitList(std::set<long long> str_id_list)
{
	RandNameManager::Instance().Init(str_id_list);
}