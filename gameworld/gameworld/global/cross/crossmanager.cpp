#include "crossmanager.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/internalprotocal/crossglobalprotocal.h"
#include "utility/configpath.h"
#include "gamelog.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "global/cross/crossusershadow.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "chat/chatmanager.h"
#include "global/rank/rankmanager.hpp"
#include "protocol/msgserver.h"
#include "global/trademarket/ordermanager.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "serverlogic.h"
#include "friend/friendintimacy.hpp"
#include "friend/friendmanager.h"
#include "global/team/teammanager.hpp"
#include "global/bigcross/utaobshadow.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"

CrossManager::CrossManager() : m_cross_hello_timestamp(0u), m_has_start_connect_to_cross(false),
m_has_connected_to_cross(false), m_last_recieve_hidden_hello_timestamp(0u), m_is_cross_connected_hidden(false),
m_cross_open_server_tamp(0), m_cross_oldest_open_server_tamp(0), m_hidden_server_idx(0), m_next_check_cross_open_server_stamp(0)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
	}
}

CrossManager::~CrossManager()
{

}

CrossManager & CrossManager::Instance()
{
	static CrossManager cm;
	return cm;
}


void CrossManager::ConnectToCrossServerAsyn()
{
	if (this->IsCrossServerConnected() || this->IsCrossServerConnecting()) return;

	const CrossConfigItem cross_cfg = CrossConfig::Instance().GetCrossConfig();
	if (cross_cfg.cross_server_addr.Invalid()) return;

	int handle = -1;
	bool ret = InternalComm::Instance().NetConnectAsyn(cross_cfg.cross_server_addr.ipstr.c_str(), cross_cfg.cross_server_addr.port, &handle, CROSS_CONNECT_ASYN_TIME_OUT_MS);
	if (ret)
	{
		InternalComm::Instance().m_reconnet_cross_handle = handle;

		if (!m_has_start_connect_to_cross) // 从开始到连接上跨服，只允许打印一次
		{
			m_has_start_connect_to_cross = true;
			gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: ConnectToCrossServerAsyn addr[%s:%d] ...\n", (unsigned int)EngineAdapter::Instance().Time(), cross_cfg.cross_server_addr.ipstr.c_str(), cross_cfg.cross_server_addr.port);
		}
	}
}

void CrossManager::DisconnectFromCrossServer()
{
	if (!this->IsCrossServerConnected()) return;

	NetID netid = InternalComm::Instance().m_cross_server_netid;
	if ((NetID)-1 != netid)
	{
		InternalComm::Instance().NetDisconnect(netid);
	}
}

void CrossManager::OnConnectCrossServerSucc(NetID netid)
{
	if (!m_has_connected_to_cross) // 之前已连上跨服就不打日志
	{
		m_has_connected_to_cross = true;
		gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: Connect CrossServer Succ!\n", (unsigned int)EngineAdapter::Instance().Time());
	}

	m_cross_hello_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	const std::set<int>& server_id_set = LocalConfig::Instance().GetAllowServerIdSet();
	if (server_id_set.empty()) return;

	static const int PLAT_TYPE = LocalConfig::Instance().GetPlatType();

	static crossgameprotocal::GameCrossRegisterGameServer gcrgs;
	gcrgs.plat_type = PLAT_TYPE;
	gcrgs.role_other_init_param_length = (long long)sizeof(RoleOtherInitParam);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		std::string db_server_ip = "127.0.0.1";
		Port db_server_port = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleList[0].module_addr.port;
		if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "DataAccessServer" / "IpForCross", &db_server_ip))
		{
			gamelog::g_log_world.printf(LL_ERROR, "CrossManager::OnConnectCrossServerSucc config DataAccessServer->IpForCross read FAILED!!!");
			ServerLogic::GetInstServerLogic()->StopServer("CrossManager::OnConnectCrossServerSucc config DataAccessServer->IpForCross read FAILED!!!");
			return;
		}

		F_STRNCPY(gcrgs.data_access_ip, db_server_ip.c_str(), sizeof(gcrgs.data_access_ip));
		gcrgs.data_access_port = db_server_port;

		std::string gateway_hostname = "127.0.0.1";
		Port gateway_listen_port = 4777;
		if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "GatewayModule" / "GameUser" / "LocalIP", &gateway_hostname))
		{
			gamelog::g_log_world.printf(LL_ERROR, "CrossManager::OnConnectCrossServerSucc config GatewayModule->GameUser->LocalIP read FAILED!!!");
			ServerLogic::GetInstServerLogic()->StopServer("CrossManager::OnConnectCrossServerSucc config DataAccessServer->IpFroCross read FAILED!!!");
			return;
		}

		if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "GatewayModule" / "GameUser" / "ListenPort", &gateway_listen_port))
		{
			gamelog::g_log_world.printf(LL_ERROR, "CrossManager::OnConnectCrossServerSucc config GatewayModule->GameUser->ListenPort read FAILED!!!");
			ServerLogic::GetInstServerLogic()->StopServer("CrossManager::OnConnectCrossServerSucc config DataAccessServer->IpFroCross read FAILED!!!");
			return;
		}

		F_STRNCPY(gcrgs.gateway_hostname, gateway_hostname.c_str(), sizeof(gcrgs.gateway_hostname));
		gcrgs.gateway_port = gateway_listen_port;
	}
	else
	{
		memset(gcrgs.data_access_ip, 0, sizeof(gcrgs.data_access_ip));
		gcrgs.data_access_port = 0;
		memset(gcrgs.gateway_hostname, 0, sizeof(gcrgs.gateway_hostname));
		gcrgs.gateway_port = 0;
	}

	gcrgs.count = 0;
	for (std::set<int>::iterator it = server_id_set.begin(), end = server_id_set.end(); it != end; ++it)
	{
		gcrgs.server_id_list[gcrgs.count++] = *it;

		if (gcrgs.count >= crossgameprotocal::GameCrossRegisterGameServer::MAX_SERVER_ID_NUM) break;
	}
	gcrgs.server_open_timestamp = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();

	int len = sizeof(gcrgs) - sizeof(gcrgs.server_id_list) + (sizeof(int) * gcrgs.count);
	InternalComm::Instance().NetSend(netid, (const char *)&gcrgs, len);

}

void CrossManager::OnDisconnectFromCrossServer(bool is_from_disconnect)
{
	if (m_has_connected_to_cross) // 没连上跨服就不用重复打日志了
	{
		m_has_connected_to_cross = false;
		m_has_start_connect_to_cross = false;
		gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: Disconnect From CrossServer!\n", (unsigned int)EngineAdapter::Instance().Time());
	}

	TeamManager::Instance().OnDisconnectFromCrossServer();
	CrossUserShadow::Instance().OnDisconnectFromCrossServer();
	ChatManager::Instance().OnDisconnectFromCrossServer();
	OrderManager::Instance().OnDisconnectCross();
	MarketManager::Instance().OnDisconnectFromCrossServer();
	GuildManager::Instance().OnDisconnectFromCrossServer();
	UTAOBShadow::Instance().OnDisconnectFromCrossServer();

	if (is_from_disconnect)
	{
		this->SendAllUserCrossConnectInfo();
	}
}

void CrossManager::CheckCrossConnection()
{
	if (CrossConfig::Instance().IsOpenCross())
	{
		this->ConnectToCrossServerAsyn();
	}
	else
	{
		this->OnDisconnectFromCrossServer(false);
	}
}

bool CrossManager::ReloadCrossConfig()
{
	std::string config_dir = "../config";
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "ConfigDir", &config_dir))
	{
		return false;
	}

	if (config_dir[config_dir.length() - 1] != '/')
	{
		config_dir += '/';
	}

	bool old_is_open = CrossConfig::Instance().IsOpenCross();
	std::string err;
	std::string configpath = config_dir + "cross.xml";
	bool is_cross_addr_change = false, is_open_cross_change = false;

	if (!CrossConfig::Instance().Reload(configpath, &err, &is_cross_addr_change, &is_open_cross_change))
	{
		return false;
	}

	this->OnReloadCrossServerConfig(is_cross_addr_change, old_is_open);

	return true;
}

unsigned int CrossManager::GetCrossOpenTamp()
{
	return m_cross_open_server_tamp;
}

void CrossManager::OnCrossOpenTamp(unsigned int cross_open_tamp)
{
	m_cross_open_server_tamp = cross_open_tamp;
}

int CrossManager::GetHiddenServerIDX()
{
	return m_hidden_server_idx;
}

void CrossManager::OnSetHiidenServerIDX(int hidden_server_idx)
{
	m_hidden_server_idx = hidden_server_idx;
}

unsigned int CrossManager::GetCrossOldestServerOpenTamp()
{
	return m_cross_oldest_open_server_tamp;
}

void CrossManager::OnCrossOldestServerOpenTamp(unsigned int cross_oldest_open_tamp)
{
	m_cross_oldest_open_server_tamp = cross_oldest_open_tamp;
}

bool CrossManager::IsCrossServerConnected()
{
	return ((NetID)-1 != InternalComm::Instance().m_cross_server_netid) && 0 == m_cross_connect_ret;
}

bool CrossManager::IsHiddenServerAlive()
{
	if (CrossConfig::Instance().IsHiddenServer()) return true;
	if (!this->IsCrossServerConnected()) return false;
	if (!m_is_cross_connected_hidden) return false; //若跨服在上次心跳时告知原服 隐藏服未连上，那么就把隐藏服当作NotAlive处理

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	return m_last_recieve_hidden_hello_timestamp + 15 > now;
}

void CrossManager::OnHiddenHeartBeat()
{
	m_last_recieve_hidden_hello_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

bool CrossManager::IsCrossServerConnecting()
{
	return (-1 != InternalComm::Instance().m_reconnet_cross_handle);
}

void CrossManager::CMDSyncInfoToHidden(int _type)
{
	enum SYNC_INFO_TYPE
	{
		SYNC_INFO_TYPE_FRIEND = 0,
		SYNC_INFO_TYPE_RANK = 1,
	};

	std::string temp_str = "error!!!";

	switch (_type)
	{
	case SYNC_INFO_TYPE_FRIEND:
	{
		FriendManager::Instance().OnConnectHiddenSucc();
		FriendIntimacyManager::Instance().OnConnectHiddenSucc();

		temp_str = "sync_friend + sync_friend_intimacy!";
	}
	break;
	case SYNC_INFO_TYPE_RANK:
	{
		RankManager::Instance().OnConnectCrossSucc();

		temp_str = "rank_info!";
	}
	break;
	}

	gamelog::g_log_cmd.printf(LL_INFO, "CrossManager::CMDSyncInfoToHidden type :[%s]", temp_str.c_str());
}

void CrossManager::SendAllUserCrossConnectInfo()
{
	Protocol::SCCrossConnectInfo info;
	info.is_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
	if (1 == info.is_cross)
	{
		info.is_connected_cross = 1;
	}
	else
	{
		info.is_connected_cross = this->IsCrossServerConnected() ? 1 : 0;
	}
	info.reserve_sh = 0;

	World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
}

void CrossManager::SyncCrossDataToHidden()
{
	CrossUserRegister::Instance().OnConnectCrossServerSucc();
	GuildManager::Instance().OnConnectCrossServerSucc();
	ActivityManager::Instance().OnConnectCrossServerSucc();
	WorldStatus::Instance().OnConnectCrossServerSucc();
	UserCacheManager::Instance().OnConnectCrossServerSucc();
	ChatManager::Instance().OnConnectCrossSucc();
	RankManager::Instance().OnConnectCrossSucc();
	OrderManager::Instance().OnConnectCrossSucc();
	MarketManager::Instance().OnConnectCrossSucc();
	HongBaoManager::Instance().OnGameworldAccept();
	FriendIntimacyManager::Instance().OnConnectHiddenSucc();
	FriendManager::Instance().OnConnectHiddenSucc();
	KuaFuYingXiongLuManager::Instance().OnConnectCrossSucc();
	this->SendAllUserCrossConnectInfo();
}

void CrossManager::OnReloadCrossServerConfig(bool is_cross_addr_change, bool old_is_open)
{
	bool now_is_open = CrossConfig::Instance().IsOpenCross();

	if (old_is_open)
	{
		if (!now_is_open || is_cross_addr_change)
		{
			this->DisconnectFromCrossServer();
		}
	}
	else
	{
		if (now_is_open || is_cross_addr_change)
		{
			this->ConnectToCrossServerAsyn();
		}
	}
}

void CrossManager::OnCrossHello(bool is_cross_connected_hidden)
{
	m_cross_hello_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_is_cross_connected_hidden = is_cross_connected_hidden;
}

void CrossManager::Update(unsigned long interval, time_t now_second)
{
	const static int CROSS_HELLO_TIME_OUT = 30;

	if (this->IsCrossServerConnected())
	{
		if (0 != m_cross_hello_timestamp && static_cast<unsigned int>(now_second) >= m_cross_hello_timestamp + CROSS_HELLO_TIME_OUT)
		{
			m_cross_hello_timestamp = 0;

#ifndef _DEBUG
			this->DisconnectFromCrossServer();
			gamelog::g_log_world.printf(LL_INFO, "CrossManager::not receive cross hello more than %d s, disconnect cross_netid", CROSS_HELLO_TIME_OUT);
#endif
		}
	}
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (m_cross_open_server_tamp <= 0 && now_second >= m_next_check_cross_open_server_stamp)
		{
			crossgameprotocal::GameHiddenSyncSpecialInfoReq ghssir;
			ghssir.server_id = LocalConfig::Instance().GetMergeServerId();

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghssir, sizeof(ghssir));

			m_next_check_cross_open_server_stamp = now_second + 10;	//给跨服同步回来一点时间
		}
	}
}

bool CrossManager::CanStartCross(Role *role)
{
	if (NULL == role)
	{
		return false;
	}

	int error_num = 0;
	switch (m_cross_connect_ret)
	{
	case crossgameprotocal::PLAT_TYPE_ERROR: error_num = errornum::EN_START_CROSS_ERROR_PLAT_TYPE; break;
	case crossgameprotocal::ROLE_OTHER_INIT_PARAM_ERROR: error_num = errornum::EN_START_CROSS_ERROR_PARAM_LEN; break;
	default: return true;
	}

	role->NoticeNum(error_num);

	return false;
}
