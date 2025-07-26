#include "loginmanager.hpp"
#include "server/rmibackobjdef.h"
#include "internalcomm.h"
#include "server/protocal/msglogin.h"
#include "engineadapter.h"
#include "servercommon/struct/roleinfolist.h"
#include "server/loginlog.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "server/gameserver.h"
#include "server/loginserver.h"
#include "server/spidconfig.h"
#include "server/roleinitattr.h"
#include "server/protocal/msgregister.h"
#include "servercommon/logdef.h"
#include "randnamemanager/randnamemanager.hpp"

LoginManager& LoginManager::GetInstance()
{
	static LoginManager inst;
	return inst;
}

void LoginManager::OnLoginToAccountReq(IP ip, NetID netid, PlatName original_pname, int db_index)
{
	if (NULL == original_pname) return;

	original_pname[sizeof(PlatName) - 1] = 0;

	static PlatName temp_pname = { 0 };
	F_STRNCPY(temp_pname, original_pname, sizeof(temp_pname));
	PlatNameCatPostfix(temp_pname, db_index);

	RMILoginToAccountBackObjectImpl *ulboi = new RMILoginToAccountBackObjectImpl();
	ulboi->netid = netid;
	F_STRNCPY(ulboi->pname, temp_pname, sizeof(ulboi->pname));
	F_STRNCPY(ulboi->original_pname, original_pname, sizeof(ulboi->original_pname));
	ulboi->ip = ip;

	RMILoginClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	lc.GetAccountInfoAsyn(ulboi->pname, ulboi);
}

void LoginManager::OnLoginToRoleReq(IP ip, NetID netid, PlatName original_pname, int role_id)
{
	UserID user_id = IntToUid(role_id);

	static PlatName real_pname = { 0 };
	F_STRNCPY(real_pname, original_pname, sizeof(real_pname));
	PlatNameCatPostfix(real_pname, user_id.db_index);

	RMILoginToRoleBackObjectImpl *gribo = new RMILoginToRoleBackObjectImpl();
	gribo->user_id = user_id;
	gribo->netid = netid;
	gribo->ip = ip;
	F_STRNCPY(gribo->pname, real_pname, sizeof(gribo->pname));
	gribo->min_login_interval_time = m_min_login_interval_s;

	RMILoginClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	bool result = lc.GetAccountInfoAsyn(gribo->pname, gribo);

	if (!result)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[LoginServer::OnRoleReq RMIError] pname:%s uid[%d, %d].", gribo->pname, user_id.db_index, user_id.role_id);
	}
}

void LoginManager::OnCreateRoleReq(IP ip, NetID netid, PlatName pname, GameName role_name, char avatar_type, char color, int db_index, int plat_spid, 
	long long name_str_id,int job_id, int mini_game_id, int audit_server_seq)
{
	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] ip:%u netid:%d pname:%s role_name:%s avatar_type:%d color:%d db_index:%d plat_spid:%d name_str_id:%lld job_id:%d mini_game_id:%d audit_server_seq:%d",
		ip, netid, pname, role_name, avatar_type, color, db_index, plat_spid, name_str_id, job_id, mini_game_id, audit_server_seq);

	PlatNameCatPostfix(pname, db_index);

	RoleInitParam p = RoleInitAttr::Instance().GetInitParam(avatar_type, color, plat_spid);
	F_STRNCPY(p.role_name, role_name, sizeof(p.role_name));
	p.plat_spid = plat_spid;
	p.mini_game_id = mini_game_id;

	const AuditServerBirthSceneCfg * birth_cfg = NULL;
	const AuditServerCfg * audit_server_cfg = SPID_CONFIG->GetAuditServerCfg(audit_server_seq, &birth_cfg);
	if (NULL != audit_server_cfg && NULL != birth_cfg)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] audit_server_cfg and birth_cfg found, set avatar_type:%d color(random):%d", audit_server_cfg->avatar_type, p.color);

		p.avatar_type = audit_server_cfg->avatar_type;
		p.color = RandomNum(INIT_APPREARANCE_COLOR_NUM);
		if (audit_server_cfg->job_list.empty())
		{
			p.profession = job_id;
			loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] job_list empty, use input job_id:%d", job_id);
		}
		else
		{
			std::set<int>::iterator job_it = audit_server_cfg->job_list.begin();
			advance(job_it, RandomNum((int)audit_server_cfg->job_list.size()));
			p.profession = *job_it;
			loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] random select job_id:%d from job_list", p.profession);
		}
		p.level = audit_server_cfg->birth_level;

		p.last_scene_id =birth_cfg->birth_scene_id;
		p.last_scene_x = birth_cfg->birth_pos.x;
		p.last_scene_y = birth_cfg->birth_pos.y;

		p.scene_id = p.last_scene_id;
		p.scene_x = p.last_scene_x;
		p.scene_y = p.last_scene_y;
	}
	else
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] audit_server_cfg or birth_cfg is null, use default avatar_type:%d color:%d job_id:%d", avatar_type, color, job_id);
		p.avatar_type = avatar_type;
		p.color = color;
		p.profession = job_id;
	}

	RMICreateRoleBackObjectImpl *crboi = new RMICreateRoleBackObjectImpl();
	crboi->db_index = db_index;
	crboi->netid = netid;
	F_STRNCPY(crboi->pname, pname, sizeof(crboi->pname));
	F_STRNCPY(crboi->role_name, role_name, sizeof(crboi->role_name));
	crboi->avatar_type = p.avatar_type;
	crboi->color = p.color;
	crboi->plat_spid = p.plat_spid;
	crboi->create_role_time = p.create_time;
	crboi->job_id = p.profession;

	loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] call CreateRoleAsyn with pname:%s db_index:%d name_str_id:%lld avatar_type:%d color:%d profession:%d", pname, db_index, name_str_id, p.avatar_type, p.color, p.profession);

	RMILRoleClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
	bool result = lc.CreateRoleAsyn(pname, db_index, name_str_id, p, crboi);
	if (!result)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] CreateRoleAsyn failed! pname:%s db_index:%d role_name:%s avatar_type:%d color:%d.", pname, db_index, role_name, avatar_type, color);
		loginlog::g_log_create.printf(LL_ERROR, "[RMICreateRoleBackObjectImpl RMIError] pname:%s db_index:%d role_name:%s avatar_type:%d color:%d.", pname, db_index, role_name, avatar_type, color);
	}
	else
	{
		loginlog::g_log_login.printf(LL_ERROR, "[OnCreateRoleReq] CreateRoleAsyn success! name_str_id:%lld", name_str_id);
		// 成功申请创号，则把name_strid保存好，防止重名
		RandNameManager::Instance().AddNameStrId(name_str_id);
	}
}

void LoginManager::SendLoginToAccountResult(NetID netid, int result, const RoleInfoList *role_info, PlatName original_panme, int db_index)
{
	Protocol::SCLoginToAccount rl;
	rl.most_recent_role_index = 0;
	rl.result = result;
	rl.count = 0;

	if (NULL != role_info)
	{
		rl.most_recent_role_index = static_cast<short>(role_info->recent_login_role_index);

		for (; rl.count < role_info->count && rl.count < MAX_ACCOUNT_ROLE_NUM; ++rl.count)
		{
			rl.role_item[rl.count].role_id = role_info->role_info_list[rl.count].role_id;
			F_STRNCPY(rl.role_item[rl.count].role_name, role_info->role_info_list[rl.count].role_name, sizeof(rl.role_item[0].role_name));
			rl.role_item[rl.count].avatar_type = role_info->role_info_list[rl.count].avatar_type;
			rl.role_item[rl.count].color = role_info->role_info_list[rl.count].color;
			rl.role_item[rl.count].level = static_cast<short>(role_info->role_info_list[rl.count].level);
			rl.role_item[rl.count].profession = role_info->role_info_list[rl.count].profession;
			rl.role_item[rl.count].headshot_id = role_info->role_info_list[rl.count].headshot_id;
			rl.role_item[rl.count].weapon_id = role_info->role_info_list[rl.count].weapon_id;
			rl.role_item[rl.count].top_level = role_info->role_info_list[rl.count].top_level;
			rl.role_item[rl.count].fly_flag = role_info->role_info_list[rl.count].fly_flag;
			rl.role_item[rl.count].reserve_sh = 0;

			memcpy(rl.role_item[rl.count].surface_list, role_info->role_info_list[rl.count].surface_list, sizeof(rl.role_item[rl.count].surface_list));
			rl.role_item[rl.count].forbid_time = role_info->role_info_list[rl.count].forbid_time;
		}
	}

	int sendlen = sizeof(Protocol::SCLoginToAccount) - (MAX_ACCOUNT_ROLE_NUM - rl.count) * sizeof(Protocol::RoleItem);
	EngineAdapter::Instance().NetSend(netid, (const char*)&rl, sendlen);
}

LoginManager::LoginManager() : m_min_login_interval_s(5)
{

}

LoginManager::~LoginManager()
{

}

//OnLoginToAccountReq
void RMILoginToAccountBackObjectImpl::GetAccountInfoRet(int ret, const AccountInfo* account_info)
{
	// [Debug] 入口参数
	loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] ret:%d account_info:%p", ret, account_info);

	int result = Protocol::LOGIN_RESULT_SUC;
	int db_index = 0;
	if (NULL != account_info)
	{
		db_index = account_info->db_index;
	}

	static int role_id_list[MAX_ACCOUNT_ROLE_NUM];
	int role_count = 0;

	UNSTD_STATIC_CHECK(5 == MAX_ACCOUNT_ROLE_NUM);

	// [Debug] 角色ID收集
	if (NULL != account_info)
	{
		if (account_info->role_id_1 > 0) role_id_list[role_count++] = account_info->role_id_1;
		if (account_info->role_id_2 > 0) role_id_list[role_count++] = account_info->role_id_2;
		if (account_info->role_id_3 > 0) role_id_list[role_count++] = account_info->role_id_3;
		if (account_info->role_id_4 > 0) role_id_list[role_count++] = account_info->role_id_4;
		if (account_info->role_id_5 > 0) role_id_list[role_count++] = account_info->role_id_5;
		loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] Collected %d role ids", role_count);
	}

	// [Debug] 错误码判断
	if (0 != ret)
	{
		result = Protocol::LOGIN_SERVER_ERROR;
		loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] ret != 0, set result LOGIN_SERVER_ERROR");
	}
	else
	{
		if (NULL == account_info)
		{
			result = Protocol::LOGIN_SERVER_ERROR;
			loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] account_info is NULL, set result LOGIN_SERVER_ERROR");
		}
		else if (account_info->forbid_time > 0)
		{
			unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			if (account_info->forbid_time > now)
			{
				result = Protocol::LOGIN_LOGIN_FORBID;
				loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] account forbidden, forbid_time:%u now:%u", account_info->forbid_time, now);
			}
		}
		else if (0 == role_count)
		{
			result = Protocol::LOGIN_RESULT_NO_ROLE;

			int spid = LocalConfig::Instance().GetPlatType();
			if (!SPID_CONFIG->CanCreateNewRole(spid, ip))
			{
				result = Protocol::LOGIN_FORBID_NEW_ROLE;
				loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] CanCreateNewRole failed spid:%d ip:%s", spid, ip);
			}
			else if (SPID_CONFIG->IsLimitCreateRole(LoginServer::Instance().GetOnlineRoleNum()))
			{
				result = Protocol::LOGIN_LIMIT_CREATE_NEW_ROLE;
				loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] IsLimitCreateRole limit reached. online num:%d", LoginServer::Instance().GetOnlineRoleNum());
			}
			else if (1 == SPID_CONFIG->GetAutoCreateRoleSwitch())
			{
				// 执行自动创角 暂未有需求
				loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] AutoCreateRoleSwitch enabled, but not implemented.");
			}
		}
		else if (role_count > 0)
		{
			if (SPID_CONFIG->IsLimitLogin(LoginServer::Instance().GetOnlineRoleNum()))
			{
				result = Protocol::LOGIN_LIMIT_LOGIN;
				loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] IsLimitLogin limit reached. online num:%d", LoginServer::Instance().GetOnlineRoleNum());
			}
		}
	}

	// [Debug] 结果分支
	if (Protocol::LOGIN_RESULT_SUC == result && NULL != account_info)
	{
		// 成功获取role_id，则继续请求对应的RoleInfo
		RMIGetRoleInfoBackObjectImpl* gribo = new RMIGetRoleInfoBackObjectImpl();
		gribo->db_index = db_index;
		gribo->netid = netid;
		gribo->result = result;
		F_STRNCPY(gribo->pname, pname, sizeof(pname));
		F_STRNCPY(gribo->original_pname, original_pname, sizeof(gribo->original_pname));

		loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] Success. Start GetRoleInfoAsyn. role_count:%d", role_count);

		RMILRoleClient lc;
		lc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
		lc.GetRoleInfoAsyn(role_id_list, role_count, false, gribo);
	}
	else
	{
		loginlog::g_log_login.printf(LL_ERROR, "[GetAccountInfoRet] result:%d, account_info:%p. SendLoginToAccountResult", result, account_info);
		LoginManager::GetInstance().SendLoginToAccountResult(netid, result, NULL, original_pname, db_index);
	}
}

void RMILoginToAccountBackObjectImpl::__exception(int error)
{
	loginlog::g_log_login.printf(LL_WARNING, "RMILoginToAccountBackObjectImpl::__exception error[%d] pname[%s]", error, pname);
}

void RMILoginToAccountBackObjectImpl::__timeout()
{
	loginlog::g_log_login.printf(LL_WARNING, "RMILoginToAccountBackObjectImpl::__timeout pname[%s]", pname);
}

void RMIGetRoleInfoBackObjectImpl::GetRoleInfoRet(int ret, const RoleInfoList &role_info)
{
	if (0 != ret) result = Protocol::LOGIN_RESULT_NO_ROLE;

	LoginManager::GetInstance().SendLoginToAccountResult(netid, result, &role_info, original_pname, db_index);
}

// OnLoginToRoleReq
void RMILoginToRoleBackObjectImpl::GetAccountInfoRet(int ret, const AccountInfo* account_info)
{
	if (ret == 0 && NULL != account_info)
	{
		if (account_info->db_index != user_id.db_index || 
			(account_info->role_id_1 != UidToInt(user_id) 
				&& account_info->role_id_2 != UidToInt(user_id) 
				&& account_info->role_id_3 != UidToInt(user_id)
				&& account_info->role_id_4 != UidToInt(user_id)
				&& account_info->role_id_5 != UidToInt(user_id)
			))
		{
			loginlog::g_log_login.printf(LL_WARNING, "[RMIUserLoginBackObjectImplRole UserIDError] pname:%s uid[%d, %d] game_thread:%d.",
				pname, user_id.db_index, user_id.role_id, game_thread);

			return;
		}

		RMIGetRoleInfoBackObjectImplRoleReq *gribo = new RMIGetRoleInfoBackObjectImplRoleReq();
		gribo->user_id = user_id;
		gribo->ip = ip;
		gribo->netid = netid;
		F_STRNCPY(gribo->pname, pname, sizeof(gribo->pname));
		gribo->min_login_interval_time = min_login_interval_time;

		int role_id_list = UidToInt(user_id);

		RMILRoleClient lc;
		lc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
		bool result = lc.GetRoleInfoAsyn(&role_id_list, 1, true, gribo);

		if (!result)
		{
			loginlog::g_log_login.printf(LL_ERROR, "[RMIUserLoginBackObjectImplRole RMIError] pname:%s uid[%d, %d] game_thread:%d.",
				pname, user_id.db_index, user_id.role_id, game_thread);
		}
	}
	else
	{
		Protocol::SCLoginToRole la;
		la.is_merged_server = 0;
		la.result = Protocol::LOGIN_SERVER_ERROR;			// 服务器发生错误
		la.scene_id = 0;
		la.last_scene_id = 0;
		la.uid = 0;
		la.time = 0;
		memset(la.key, 0, sizeof(SessionKey));
		memset(la.gs_hostname, 0, sizeof(la.gs_hostname));
		la.gs_port = 0;
		memset(la.backup_gs_hostname, 0, sizeof(la.backup_gs_hostname));
		la.backup_gs_port = 0;
		la.gs_index = 0;
		la.server_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		la.create_time = 0u;
		EngineAdapter::Instance().NetSend(netid, (const char*)&la, sizeof(la));
	}
}

void RMIGetRoleInfoBackObjectImplRoleReq::GetRoleInfoRet(int ret, const RoleInfoList &role_info)
{
	Protocol::SCLoginToRole la;
	la.is_merged_server = LocalConfig::Instance().IsMergedServer() ? 1 : 0;

	if (ret == 0 && role_info.count == 1 && role_info.role_info_list[0].role_id == UidToInt(user_id))
	{
		do 
		{
			unsigned int forbid_time = role_info.role_info_list[0].forbid_time;
			unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			if (forbid_time > 0 && now < forbid_time)
			{
				la.result = Protocol::LOGIN_LOGIN_FORBID;
				break;
			}

		int scene_id = role_info.role_info_list[0].scene_id;
		int last_scene_id = role_info.role_info_list[0].last_scene_id;

		GameServer *gs = LoginServer::Instance().GetGSServer(role_info.role_info_list[0].scene_id);
		if (gs == 0)
		{
			gs = LoginServer::Instance().GetGSServer(role_info.role_info_list[0].last_scene_id);
			if (gs != 0)
			{
				scene_id = last_scene_id;
			}
			else
			{
				gs = LoginServer::Instance().GetGSServer(108);
				if (gs != 0)
				{
					scene_id = last_scene_id = 108;
				}
			}
		}

		if (gs == 0)
		{
			la.result = Protocol::LOGIN_SCENE_NOT_EXIST;
		}
		else
		{
			LoginServer::GateWayServer *gw = LoginServer::Instance().RandomGatewayServer();
			if (gw == 0)
			{
				la.result = Protocol::LOGIN_RESULT_NO_GATEWAY;
			}
			else
			{
				unsigned int time = (unsigned int)(EngineAdapter::Instance().Time());

				int role_id = UidToInt(user_id);

				GameName role_name;
				F_STRNCPY(role_name, role_info.role_info_list[0].role_name, sizeof(GameName));

				GetSessionKey(InternalComm::Instance().m_session_key_base, scene_id, 0,
					last_scene_id, role_id, role_name, time, 1, pname, la.key);

					/*
				printf("%s  %d  %d  %d  %d  %s  %u  %d  %s  %s\n",
					InternalComm::Instance().m_session_key_base.c_str(),
					scene_id,
					0,
					last_scene_id,
					role_id,
					role_name,
					time,
					1,
					pname,
					la.key);
					*/

				la.scene_id = scene_id;
				la.last_scene_id = last_scene_id;
				la.uid = role_id;
				la.time = time;
				la.result = Protocol::LOGIN_RESULT_SUC;
				la.is_cross = 0;
				STRNCPY(la.gs_hostname, gw->hostname.c_str(), sizeof(la.gs_hostname));
				la.gs_port = gw->port;
					STRNCPY(la.backup_gs_hostname, gw->hostname.c_str(), sizeof(la.backup_gs_hostname));
					la.backup_gs_port = gw->port;
				if (role_info.role_info_list[0].is_in_cross)
				{
					unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
					if (now < role_info.role_info_list[0].last_save_time + 3 * 60) // 3分钟内可以重进跨服
					{
						std::string host_name = "";
						int host_port = 0;

						if (LoginServer::Instance().IsCrossConnected() 
							&& now > LoginServer::Instance().GetCrossGatewayRegisterTime() + 3 * 60 // 必须控制在gateway注册了3分钟后  因为如果重启了服务器，是无法直接登跨服的
							&& LoginServer::Instance().RandCrossGateway(&host_name, &host_port))
						{
							STRNCPY(la.gs_hostname, host_name.c_str(), sizeof(la.gs_hostname));
							la.gs_port = host_port;
							la.is_cross = 1;
						}
					}
				}
				la.gs_index = gs->index;
				la.server_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				F_STRNCPY(la.role_name, role_name, sizeof(la.role_name));
				la.create_time = role_info.role_info_list[0].create_time;

				EngineAdapter::Instance().NetSend(netid, (const char*)&la, sizeof(Protocol::SCLoginToRole));

				loginlog::g_log_login.printf(LL_INFO, "[RMIGetRoleInfoBackObjectImplRoleReq Success] pname:%s uid[%d] sid:%d last_sid:%d gw:%s:%d gs_index:%d.",
					pname, UidToInt(user_id), la.scene_id, la.last_scene_id, la.gs_hostname, (int)la.gs_port, la.gs_index);


				//向DATAACCESS更新last login time
				{
					RMIUpdateLastLoginTimeBackObjectImpl* impl = new RMIUpdateLastLoginTimeBackObjectImpl();
					impl->login_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
					//impl->role_id = role_id;
					//F_STRNCPY(impl->pname, pname, sizeof(impl->pname));
					//F_STRNCPY(impl->role_name, role_name, sizeof(impl->role_name));

					impl->role_id = role_info.role_info_list[0].role_id;
					F_STRNCPY(impl->pname, pname, sizeof(impl->pname));
					F_STRNCPY(impl->role_name, role_info.role_info_list[0].role_name, sizeof(impl->role_name));

					RMILoginClient lc;
					lc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
					bool result = lc.UpdateLastLoginTimeAsyn(pname, impl->login_timestamp, impl);

					if (!result)
					{
						loginlog::g_log_login.printf(LL_ERROR, "[RMIUpdateLastLoginTimeBackObjectImpl RMIError] pname:%s uid[%d, %s] ",
							pname, role_info.role_info_list[0].role_id, role_info.role_info_list[0].role_name);
					}
				}

				return;
			}
		}

		} while (0);
	}
	else
	{
		// 没有找到role
		la.result = Protocol::LOGIN_RESULT_NO_ROLE;
	}

	la.scene_id = 0;
	la.last_scene_id = 0;
	la.uid = 0;
	la.time = 0;
	memset(la.key, 0, sizeof(SessionKey));
	memset(la.gs_hostname, 0, sizeof(la.gs_hostname));
	la.gs_port = 0;
	la.gs_index = 0;
	la.create_time = 0;
	EngineAdapter::Instance().NetSend(netid, (const char*)&la, sizeof(Protocol::SCLoginToRole));

	loginlog::g_log_login.printf(LL_WARNING, "[RMIGetRoleInfoBackObjectImplRoleReq return result:%d] pname:%s uid[%d, %d].",
		la.result, pname, user_id.db_index, user_id.role_id);
}

void RMICreateRoleBackObjectImpl::CreateRoleRet(int ret, int role_id, int create_result)
{
	loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleRet] ret:%d role_id:%d create_result:%d pname:%s", ret, role_id, create_result, pname);

	if (0 != ret)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleRet] Error with ret[%d] create_result[%d], pname[%s]", ret, create_result, pname);
		loginlog::g_log_create.printf(LL_WARNING, "CreateRoleRet Error with ret[%d] create_result[%d], pname[%s]", ret, create_result, pname);
		Error(CREATE_ROLE_RESULT_TYPE_SERVER_ERROR);
		return;
	}

	if (CREATE_ROLE_RESULT_TYPE_SUCC != create_result)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleRet] Error with ret[%d] create_result[%d], pname[%s]", ret, create_result, pname);
		loginlog::g_log_create.printf(LL_WARNING, "CreateRoleRet Error with ret[%d] create_result[%d], pname[%s]", ret, create_result, pname);
		Error(create_result);
		return;
	}

	UserID user_id = IntToUid(role_id);
	if (user_id.role_id >= MAX_ROLE_ID)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleRet] role_id >= MAX_ROLE_ID,  pname[%s]", pname);
		loginlog::g_log_create.printf(LL_WARNING, "CreateRoleRet role_id >= MAX_ROLE_ID,  pname[%s]", pname);
		Error(CREATE_ROLE_RESULT_TYPE_SERVER_ERROR);
		return;
	}
	
	Protocol::SCCreateRoleAck cra;
	cra.result = CREATE_ROLE_RESULT_TYPE_SUCC;
	cra.role_id = UidToInt(user_id);
	F_STRNCPY(cra.role_name, role_name, sizeof(cra.role_name));
	cra.avartar_type = avatar_type;
	cra.color = color;
	cra.create_role_time = create_role_time;

	loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleRet] Success. Send SCCreateRoleAck. role_id:%d role_name:%s avartar_type:%d color:%d create_role_time:%d", cra.role_id, cra.role_name, cra.avartar_type, cra.color, cra.create_role_time);

	EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCCreateRoleAck));
	LOG_QUICK12(loginlog::g_log_quick, LOG_TYPE_REGISTER, cra.role_id, cra.role_name, pname, cra.avartar_type, cra.color, NULL, NULL, plat_spid, job_id, 0, 0, 0);

	loginlog::g_log_create.printf(LL_INFO, "[RMIAddRoleBackObjectImpl Success] pname:%s user_id[%d] role_name:%s avatar_type:%d init_appearance:%d plat_spid[%d]", pname, UidToInt(user_id), role_name, avatar_type, color, plat_spid);
}

void RMICreateRoleBackObjectImpl::Error(int create_result)
{
	loginlog::g_log_login.printf(LL_ERROR, "[CreateRoleError] create_result:%d pname:%s", create_result, pname);

	Protocol::SCCreateRoleAck cra;
	cra.result = create_result;
	cra.role_id = 0;
	memset(cra.role_name, 0, sizeof(GameName));
	cra.avartar_type = 0;
	cra.color = 0;
	EngineAdapter::Instance().NetSend(netid, (const char *)&cra, sizeof(Protocol::SCCreateRoleAck));
}

void RMIUpdateLastLoginTimeBackObjectImpl::UpdateLastLoginTimeRet(int ret)
{
	loginlog::g_log_login.printf(LL_INFO, "RMIUpdateLastLoginTimeBackObjectImpl::UpdateLastLoginTimeRet pname[%s] role[%d,%s] time[%u] ret[%d]", pname, role_id, role_name, login_timestamp, ret);
}
