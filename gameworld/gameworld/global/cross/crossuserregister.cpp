#include "crossuserregister.hpp"
#include "internalcomm.h"
#include "utility/configpath.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossglobalprotocal.h"
#include "obj/character/role.h"
#include "world.h"
#include "scene/scene.h"
#include "scene/scenemanager.hpp"
#include "rmibackobjdef.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/event/eventhandler.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "gamelog.h"

CrossUserRegister::CrossUserRegister() : m_uid_generator(0)
{
	for (size_t i = 0; i < sizeof(SessionKey); ++i)
	{
		m_cross_session_key[i] = i + 1;
	}
	m_cross_session_key[sizeof(SessionKey) - 1] = 0;
}

CrossUserRegister::~CrossUserRegister()
{

}

CrossUserRegister & CrossUserRegister::Instance()
{
	static CrossUserRegister cum;
	return cum;
}

bool CrossUserRegister::OnStartCrossReq(crossgameprotocal::CrossHiddenStartCrossReq *req)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return false;

	gamelog::g_log_rolecross.printf(LL_INFO, "CrossUserRegister::OnStartCrossReq STEP_2 rolecross[%d,%s]", req->uid, req->pname);

	//if (IsCrossUser(IntToUid(req->uid))) return false;

	long long origin_uuid = UniqueUserIDToLongLong(UniqueUserID(req->plat_type, IntToUid(req->uid)));
	if (!this->CheckCanStartCross(origin_uuid)) return false;

	const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(UniqueServerID(req->plat_type, req->server_id));
	if (NULL == db_handler) return false;

	gamelog::g_log_rolecross.printf(LL_INFO, "CrossUserRegister::OnStartCrossReq STEP_3 rolecross[%d,%s]", req->uid, req->pname);

	RMIHiddenGetRoleInfoImpl* impl = new RMIHiddenGetRoleInfoImpl();
	impl->origin_plat_type = req->plat_type;
	impl->origin_server_id = req->server_id;
	impl->origin_uid = req->uid;
	F_STRNCPY(impl->origin_pname, req->pname, sizeof(impl->origin_pname));
	impl->cross_business_data = req->cross_business_data;
	
	RMIHiddenClient hc;
	hc.__bind_session(db_handler->GetRoleDB());
	return hc.GetRoleInfoAsyn(&req->uid, 1, false, impl); // 此处不需要读入Cache，因为在操作跨服前应有Cache
}


void RMIHiddenGetRoleInfoImpl::GetRoleInfoRet(int ret, const RoleInfoList &role_info)
{
	gamelog::g_log_rolecross.printf(LL_INFO, "RMIHiddenGetRoleInfoImpl::GetRoleInfoRet STEP_4 rolecross[%d,%s] ret[%d]", origin_uid, origin_pname, ret);

	if (0 == ret)
	{
		do
		{
			UserID new_user_id;
			new_user_id = IntToUid(role_info.role_info_list[0].role_id);

			const RoleInfoList::__roleinfo& ri = role_info.role_info_list[0];

			GameName role_name;
			F_STRNCPY(role_name, ri.role_name, sizeof(GameName));

			unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

			static SessionKey session_key;
			GetSessionKey(InternalComm::Instance().m_session_key_base, ri.scene_id, 0,
				ri.last_scene_id, UidToInt(new_user_id), role_name, now_time, 1, origin_pname, session_key);

			CrossUserRegister::Instance().CacheCrossRoleInfo(origin_pname, new_user_id, 
				origin_plat_type, origin_server_id, IntToUid(origin_uid), role_name,cross_business_data);

			{
				static crossgameprotocal::HiddenCrossStartCrossAck ack;
				ack.result = 0;
				ack.origin_plat_type = origin_plat_type;
				ack.origin_server_id = origin_server_id;
				ack.origin_uid = origin_uid;
				F_STRNCPY(ack.orgin_platname, origin_pname, sizeof(ack.orgin_platname));
				F_STRNCPY(ack.role_name, role_name, sizeof(ack.role_name));
				ack.new_uid = UidToInt(new_user_id);
				ack.scene_id = ri.scene_id;
				ack.last_scene_id = ri.last_scene_id;
				ack.time = now_time;

				std::string tmp_host_name = "127.0.0.1";
				unsigned short tmp_port = 7777;
				if (!World::GetInstWorld()->RandGatewayInfo(&tmp_host_name, &tmp_port))
				{
					EngineAdapter::Instance().ConfigSyncValue(ROOT / "GatewayModule" / "GameUser" / "LocalIP", &tmp_host_name, tmp_host_name);
					EngineAdapter::Instance().ConfigSyncValue(ROOT / "GatewayModule" / "GameUser" / "ListenPort", &tmp_port, tmp_port);
				}
				
				STRNCPY(ack.gateway_hostname, tmp_host_name.c_str(), sizeof(ack.gateway_hostname));
				ack.gateway_port = tmp_port;
				
				F_STRNCPY(ack.session_key, session_key, sizeof(ack.session_key));

				InternalComm::Instance().SendToCross((const char*)&ack, sizeof(ack));
			}
		
			// 成功的情况，直接return
			return;

		} while (0);
	}
	
	// 失败的情况
	static crossgameprotocal::HiddenCrossStartCrossAck ack;
	ack.result = -1;
	ack.origin_plat_type = origin_plat_type;
	ack.origin_server_id = origin_server_id;
	ack.origin_uid = origin_uid;

	InternalComm::Instance().SendToCross((const char*)&ack, sizeof(ack));	
}

void CrossUserRegister::CacheCrossRoleInfo(PlatName pname, const UserID& new_user_id, 
	int origin_plat_type, int origin_server_id, const UserID& origin_user_id, 
	GameName origin_rolename, const CrossData_Business& business_data)
{
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(origin_plat_type, origin_user_id));
	m_origin_uuid_uid_map[uuid] = new_user_id;

	static CrossUserInfo cui;
	cui.original_plat_type = origin_plat_type;
	cui.original_server_id = origin_server_id;
	cui.original_user_id = origin_user_id;
	F_STRNCPY(cui.original_rolename, origin_rolename, sizeof(GameName));
	cui.can_enter_hidden = false;
	cui.business_data = business_data;

	m_cross_user_info_map[new_user_id] = cui;
}

bool CrossUserRegister::CheckCanStartCross(long long uuid)
{
	OriginUuidToUserIDmap::iterator it_id = m_origin_uuid_uid_map.find(uuid);
	if (it_id != m_origin_uuid_uid_map.end())
	{
		Role *user = World::GetInstWorld()->GetSceneManager()->GetRole(it_id->second);
		if (NULL != user) return false;
	}

	return true;
}

void CrossUserRegister::OnUserLogin(const UserID& user_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	this->SendCrossUserLoginToCross(user_id);

	CrossUserInfo * cui = this->GetCrossUserInfo(user_id);
	if (NULL != cui)
	{
		EventHandler::Instance().OnUserLoginCrossHidden(user_id, cui->business_data);		
		cui->business_data.Reset();
	}
}

void CrossUserRegister::OnUserLogout(int uid)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	//!< \note 这里只当玩家被踢时才清掉信息  所以暂时先注释以下两句(否则玩家无法在掉线后重连进跨服）
	//m_origin_uuid_uid_map.erase(user->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID());
	//m_cross_user_info_map.erase(user->GetUserId());

	static crossgameprotocal::HiddenCrossUserLogout gcul;
	gcul.uid = uid;
	InternalComm::Instance().SendToCross((const char *)&gcul, sizeof(gcul));
}

void CrossUserRegister::OnUserKicked(Role* user)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	
	m_origin_uuid_uid_map.erase(user->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID());
	m_cross_user_info_map.erase(user->GetUserId());
}

void CrossUserRegister::OnConnectCrossServerSucc()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	
	//for (CrossUserInfoMapIt it = m_cross_user_info_map.begin(), end = m_cross_user_info_map.end(); it != end; ++ it)
	//{
	//	if (NULL != World::GetInstWorld()->GetSceneManager()->GetRole(it->first))
	//	{
	//		this->SendCrossUserLoginToCross(it->first);
	//	}
	//}
}

void CrossUserRegister::SendCrossUserLoginToCross(const UserID &user_id)
{
	CrossUserInfoMapIt it = m_cross_user_info_map.find(user_id);
	if (it != m_cross_user_info_map.end())
	{
		static crossgameprotocal::HiddenCrossUserLogin gcul;
		gcul.uid = UidToInt(user_id);
		gcul.original_plat_type = it->second.original_plat_type;
		gcul.original_uid = UidToInt(it->second.original_user_id);
		F_STRNCPY(gcul.original_user_name, it->second.original_rolename, sizeof(GameName));
		InternalComm::Instance().SendToCross((const char *)&gcul, sizeof(gcul));
	}
}

void CrossUserRegister::CheckActivityEnterScene(const UserID& user_id)
{

}

void CrossUserRegister::SendToAllCrossUser(const char *msg, int len)
{
	if (NULL == msg || len <= 0) return;

	for (CrossUserInfoMapIt it = m_cross_user_info_map.begin(), end = m_cross_user_info_map.end(); it != end; ++ it)
	{
		Role *user = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != user)
		{
			EngineAdapter::Instance().NetSend(user->GetNetId(), msg, len);
		}
	}
}

void CrossUserRegister::KickAllCrossUser()
{
	for (CrossUserInfoMapIt it = m_cross_user_info_map.begin(), end = m_cross_user_info_map.end(); it != end; ++ it)
	{	
		Role *user = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != user && NULL != user->GetScene())
		{
			EngineAdapter::Instance().NetDisconnect(user->GetNetId(), user->GetUserId(), user->GetName(), user->GetScene()->GetSceneID(), "cross");
		}	
	}
}

CrossUserRegister::CrossUserInfo* CrossUserRegister::GetCrossUserInfo(const UserID& new_user_id)
{
	CrossUserInfoMap::iterator it = m_cross_user_info_map.find(new_user_id);
	if (it == m_cross_user_info_map.end())
	{
		return NULL;
	}

	return &it->second;
}

bool CrossUserRegister::IsHasCrossUser(const UserID & new_user_id)
{
	CrossUserInfoMap::iterator it = m_cross_user_info_map.find(new_user_id);
	if (it == m_cross_user_info_map.end())
	{
		return false;
	}
	
	return true;
}

UserID CrossUserRegister::GetCrossUserID(long long uuid)
{
	OriginUuidToUserIDmap::iterator it = m_origin_uuid_uid_map.find(uuid);
	if (it != m_origin_uuid_uid_map.end())
	{
		return it->second;
	}

	return INVALID_USER_ID;
}
