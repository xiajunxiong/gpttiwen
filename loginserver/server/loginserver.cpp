#include "protocal/msglogin.h"
#include "loginserver.h"
#include "servercommon/userprotocal/loginmsgcode.h"
#include "servercommon/userprotocal/msgheader.h"
#include "ilogmodule.h"

#include "engineadapter.h"
#include "internalcomm.h"
#include "servercommon/servercommon.h"
#include "roleinitattr.h"
#include "iplimit.hpp"
#include "protocal/msglogin.h"
#include "protocal/msgregister.h"
#include "servercommon/internalprotocal/lgprotocal.h"
#include "servercommon/internalprotocal/lwprotocal.h"
#include "servercommon/internalprotocal/lglobalprotocal.h"
#include "servercommon/darmi/rmisystemclient.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/internalprotocal/battleprotocol.h"

#include "rmibackobjdef.h"
#include "loginlog.h"

#include "platform/platform.h"
#include "utility/configpath.h"
#include "loginservermodule.h"

#include "loginusertrace.hpp"
#include "loginserver/rmiclient/rmidaroleclient.h"
#include "config/reloadconfigmanager.hpp"
#include "server/spidconfig.h"
#include "story/story.hpp"
#include "servercommon/struct/system/cmdresult.h"
#include "dbsessionreconnector.hpp"

OLD_USE_INTERNAL_NETWORK();

LoginServer::LoginServer() 
	: m_min_login_interval_s(5), m_platform(NULL), m_last_check_prof_num_timestamp(0), m_prof1_num(0), 
	m_prof2_num(0), m_prof3_num(0), m_prof4_num(0), 
	m_last_check_role_num_timestamp(0), m_total_role_num(0),
	m_online_role_num(0)
{
	
}

LoginServer::~LoginServer()
{

}

LoginServer & LoginServer::Instance()
{
	static LoginServer ls;
	return ls;
}

bool LoginServer::Init(const std::vector<int> &db_index_to_create, ILogModule *log, const std::string &configpath, 
					   int min_login_interval_s, int can_not_login_global_time)
{
	m_log.SetLogModule(log);
	m_log.SetCatagory("LoginServer");

	// 获取日志生成目录
	std::string log_dir = "log/";
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "LogDir", &log_dir))
	{
		m_log.printf(LL_ERROR, "Login Server On INIT, LogDir Read Failed!");
		return false;
	}

	if (log_dir[log_dir.length() - 1] != '/')
	{
		log_dir += '/';
	}
	loginlog::LogInit(log, log_dir);

	// 加载注册配置
	std::string err;
	std::string configname = "registeconfig.xml";

	if (!RoleInitAttr::Instance().Init(configpath, configname, &err))
	{
		m_log.printf(LL_ERROR, err.c_str());
		m_log.printf(LL_ERROR, "Login Server On INIT, RoleInitAttr::Init return FALSE.");
		return false;
	}
	m_log.printf(LL_INFO, "Login Server On INIT, RoleInitAttr::Init Success.");
	
	configname = "iplimit.xml";

	if (!IPLimit::Instance().Init(configpath, configname, &err))
	{
		m_log.printf(LL_ERROR, err.c_str());
		m_log.printf(LL_ERROR, "Login Server On INIT, IPLimit::Init return FALSE.");
		return false;
	}

	configname = "loginusertraceconfig.xml";

	if (!LoginUserTrace::Instance().Init(configpath, configname, &err))
	{
		m_log.printf(LL_ERROR, err.c_str());
		m_log.printf(LL_ERROR, "Login Server On INIT, LoginUserTrace::Init return FALSE.");
		return false;
	}

	m_min_login_interval_s = min_login_interval_s;

	this->StartCmd();

	m_platform = Platform::Create();

	RMIInitNameStrIdBackObject *insido = new RMIInitNameStrIdBackObject();

	RMILoginClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	bool result = lc.GetNameMappingCacheAsyn(insido);

	if (!result)
	{
		m_log.printf(LL_ERROR, "Login Server On INIT, GetNameMappingCacheAsyn::Init return FALES.");
	}

	return true;
}

void RMIGetProfBackObjectImpl::GetProfNumRet(int ret, int prof1_num, int prof2_num, int prof3_num, int prof4_num)
{
	if (0 == ret)
	{
		LoginServer::Instance().SetProfNum(prof1_num, prof2_num, prof3_num, prof4_num);
	}
}

void RMIGetTotalRoleNumBackObjectImpl::GetTotalRoleNumRet(int ret, int total_role_num)
{
	if (0 == ret)
	{
		LoginServer::Instance().SetTotalRoleNum(total_role_num);
		loginlog::g_log_login.printf(LL_INFO, "RMIGetRoleNumBackObjectImpl::GetRoleNumRet role_num[%d]", total_role_num);
	}
}

void LoginServer::Update()
{
	time_t now = EngineAdapter::Instance().Time();
	
	if (now >= m_last_check_prof_num_timestamp + 10)
	{
		m_last_check_prof_num_timestamp = now;

		// 获得各个职业的人数
		RMIGetProfBackObjectImpl *ulboi = new RMIGetProfBackObjectImpl();

		RMILoginClient lc;
		lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
		lc.GetProfNum(ulboi);
	}

	if (now >= m_last_check_role_num_timestamp + 10)
	{
		m_last_check_role_num_timestamp = now;

		// 获得总玩家人数
		RMIGetTotalRoleNumBackObject *impl = new RMIGetTotalRoleNumBackObjectImpl();

		RMILoginClient lc;
		lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
		lc.GetTotalRoleNumAsyn(impl);
	}

	DBSessionReconnector::Instance().Update(static_cast<int>(now));
}

void LoginServer::OnGSAccept(Port listen_port, NetID netid, IP ip, Port port)	// 接入gamesvr
{
	if (m_gs_list.size() <= netid)
	{
		int old_size = (int)m_gs_list.size();
		m_gs_list.resize(netid + 1);
		for (int i = old_size; i < (int)m_gs_list.size(); ++i)
		{
			m_gs_list[i] = 0;
		}
	}

	GameServer *gs = new GameServer();

	gs->netid = netid;
 	gs->port = 0;
	m_gs_list[netid] = gs;
}

void LoginServer::OnGateAccept(Port listen_port, NetID netid, IP ip, Port port)		// 接入gateway
{
	if (m_gateway_list.size() <= netid)
	{
		int old_size = (int)m_gateway_list.size();
		m_gateway_list.resize(netid + 1);
		for (int i = old_size; i < (int)m_gateway_list.size(); ++i)
		{
			m_gateway_list[i] = NULL;
		}
	}

	GateWayServer *gw = new GateWayServer();

	gw->netid = netid;
	gw->port = 0;
	m_gateway_list[netid] = gw;
}

void LoginServer::OnBattleServerAccept(Port listen_port, NetID net_id, IP ip, Port port)
{
	BattleServer bs;
	bs.netid = net_id;
	m_battle_server_map.insert(BattleServerMap::value_type(net_id, bs));

	m_log.printf(LL_INFO, "OnBattleServerAccept netid[%u] ip[%u] port[%d]", net_id, ip, port);
}

void LoginServer::OnInnerRecv(NetID netid, const char *data, int length)
{
	if (m_gs_list.size() > netid && m_gs_list[netid] != 0)
	{
		OnGSRecv(m_gs_list[netid], data, length);
		return;
	}
	
	if (m_gateway_list.size() > netid && m_gateway_list[netid] != 0)
	{
		OnGateRecv(netid, data, length);
		return;
	}

	BattleServerMap::iterator it = m_battle_server_map.find(netid);
	if (it != m_battle_server_map.end())
	{
		OnBattleServerRecv(netid, data, length);
		return;
	}
}

void LoginServer::OnGSRecv(GameServer *gs, const char *data, int length)		// 处理gamesvr的消息
{
	lgprotocal::MessageHeader *header = (lgprotocal::MessageHeader*)data;

	switch (header->msg_type)
	{
	case lgprotocal::MT_REGISTER_GS:											// gamesvr注册后 相互获取之间的场景信息 用于切换场景
		OnRegisterGS(gs, data);
		break;

	case lgprotocal::MT_ROLE_FORBID:											// gamesvr请求角色封号
		{
			lgprotocal::RoleForbid *rf = (lgprotocal::RoleForbid *)data;
			OnRoleForbid(rf);
		}
		break;

	case lgprotocal::MT_GL_REGISTER_CROSS_GATEWAY:
		{
			lgprotocal::GLRegisterCrossGateway* msg = (lgprotocal::GLRegisterCrossGateway*)data;
			OnRegisterCrossGateway(msg);
		}
		break;

	case lgprotocal::MT_GL_ON_CROSS_DISCONNECT:
		{
			lgprotocal::GLOnCrossDisconnect* msg = (lgprotocal::GLOnCrossDisconnect*)data;
			OnCrossDisconnect(msg);
		}
		break;

	case lgprotocal::MT_GL_ONLINE_ROLE_NUM_SYNC:
		{
			lgprotocal::GLSyncOnlineRoleNum* msg = (lgprotocal::GLSyncOnlineRoleNum*)data;
			OnSyncOnlineRoleNum(msg);
		}
		break;

	default:
		break;
	}
}

void LoginServer::OnGateRecv(NetID netid, const char *data, int length)			// 处理gateway的消息
{
	lwprotocal::MessageHeader *header = (lwprotocal::MessageHeader*)data;

	switch (header->msg_type)
	{
	case lwprotocal::MT_REGISTER_GW:
		OnRegisterGW(netid, data);
		break;

	default:
		break;
	}
}

void LoginServer::OnRegisterBattleServer(NetID netid, const char* data)
{
	battleloginprotocol::BL_RegisterToLogin* msg = (battleloginprotocol::BL_RegisterToLogin*)data;

	BattleServer& bs = m_battle_server_map[netid]; // 外部应已判断好存在NetID为netid的BattleServer

	bs.hostname = msg->ip;
	bs.game_port = msg->port_for_gameworld;
	bs.gateway_port = msg->port_for_gateway;
	bs.index = msg->index;

	m_log.printf(LL_MAINTANCE, "OnRegisterBattleServer netid[%u] hostname[%s] game_port[%d] gate_port[%d] index[%d]", 
		netid, msg->ip, msg->port_for_gameworld, msg->port_for_gateway, msg->index);

	// 通知所有的gateway有新battleserver进来
	{
		lwprotocal::LWOnBattleRegister ogsr;
		ogsr.index = msg->index;
		F_STRNCPY(ogsr.ip, msg->ip, sizeof(ogsr.ip));
		ogsr.port = msg->port_for_gateway;

		for (int w = 0; w < (int)m_gateway_list.size(); ++w)
		{
			GateWayServer *gw = m_gateway_list[w];
			if (NULL != gw && 0 != gw->port)
			{
				InternalComm::Instance().NetSend(gw->netid, (const char *)&ogsr, sizeof(ogsr));
			}
		}
	}
	

	// 通知所有gameworld有新的battle进来
	{
		lgprotocal::LGRegisterBattle ogsr;
		ogsr.index = msg->index;
		STRNCPY(ogsr.ip, msg->ip, sizeof(ogsr.ip));
		ogsr.port = msg->port_for_gameworld;

		int gs_size = (int)m_gs_list.size();
		for (int g = 0; g < gs_size; ++g)
		{
			GameServer *gs = m_gs_list[g];
			if (NULL != gs && 0 != gs->port)
			{
				bool ret = InternalComm::Instance().NetSend(gs->netid, (const char *)&ogsr, sizeof(lgprotocal::LGRegisterBattle));
				if (!ret)
				{
					loginlog::g_log_login.printf(LL_WARNING, "OnRegisterBattleServer send battle info to gameworld FAILED! battle index[%d] host[%s:%d]",
						ogsr.index, ogsr.ip, ogsr.port);
				}
			}
		}
	}
}

void LoginServer::OnBattleServerRecv(NetID netid, const char* data, int length)
{
	battleloginprotocol::MessageHeader *header = (battleloginprotocol::MessageHeader*)data;

	switch (header->msg_type)
	{
	case battleloginprotocol::MT_REGISTER_TO_LOGIN:
		OnRegisterBattleServer(netid, data);
		break;

	case battleloginprotocol::MT_STORY_BATTLE_INFO:
		OnSyncStoryBattleInfo(netid, data, length);
		break;

	case battleloginprotocol::MT_STORY_ROUND_RESULT:
		OnSyncStoryRoundResult(netid, data, length);
		break;

	default:
		break;
	}
}

void LoginServer::OnSyncStoryBattleInfo(NetID netid, const char* data, int length)
{
	int msg_size = static_cast<int>(sizeof(battleloginprotocol::BL_SyncStoryBattleInfoMsg));
	Story::Instance().OnSyncBattleInfoMsg(data + msg_size, length - msg_size);
}

void LoginServer::OnSyncStoryRoundResult(NetID netid, const char* data, int length)
{
	battleloginprotocol::BL_SyncStoryRoundResultMsg* msg = (battleloginprotocol::BL_SyncStoryRoundResultMsg*)data;

	int msg_size = sizeof(battleloginprotocol::BL_SyncStoryRoundResultMsg);
	Story::Instance().OnSyncRoundResultMsg(msg->round, data + msg_size, length - msg_size);
}

void LoginServer::OnRegisterGW(NetID netid, const char *data)					// 有新的gateway注册进来 让gateway连接上所有的gamesvr
{
	lwprotocal::WLRegisterGW *rgw = (lwprotocal::WLRegisterGW*)data;

	GateWayServer *gw = m_gateway_list[netid];

	gw->hostname = rgw->hostname;
	gw->port = rgw->port;
	gw->netid = netid;

	AddGateWay(gw);

	int gs_size = (int)m_gs_list.size();
	for (int g = 0; g < gs_size; ++g)
	{
		GameServer *gs = m_gs_list[g];
		if (NULL != gs && 0 != gs->port)
		{
			lwprotocal::LWOnGSRegister ogsr;
			ogsr.index = gs->index;
			STRNCPY(ogsr.ip, gs->ip.c_str(), sizeof(ogsr.ip));
			ogsr.port = gs->port;
			InternalComm::Instance().NetSend(netid, (const char *)&ogsr, sizeof(lwprotocal::LWOnGSRegister));
		}
	}

	// 有新的gateway进来，让其连接battle_server
	BattleServerMap::iterator it = m_battle_server_map.begin();
	for (; it != m_battle_server_map.end(); ++it)
	{
		if (it->second.hostname != "" && -1 != it->second.index)
		{
			lwprotocal::LWOnBattleRegister ogsr;
			ogsr.index = it->second.index;
			F_STRNCPY(ogsr.ip, it->second.hostname.c_str(), sizeof(ogsr.ip));
			ogsr.port = it->second.gateway_port;
			InternalComm::Instance().NetSend(netid, (const char*)&ogsr, sizeof(lwprotocal::LWOnBattleRegister));
		}
	}
}

void LoginServer::OnInnerDisconnect(NetID netid)
{
	if (m_gs_list.size() > netid && m_gs_list[netid] != 0)
	{
		OnGSDisconnect(m_gs_list[netid]);
		return;
	}

	if (m_gateway_list.size() > netid && m_gateway_list[netid] != 0)
	{
		OnGateDisconnect(netid);
		return;
	}

	DBSession* db_session = InternalComm::Instance().GetDBSession(netid);
	if (NULL != db_session)
	{
		EngineAdapter::Instance().RMICloseSession(db_session->session);
		db_session->session.handle = 0;
		db_session->session.netid = -1;

		DBSessionReconnector::Instance().AddReconnectInfo(db_session->ip, db_session->port, &db_session->reconnect_handle);
	}
}

void LoginServer::OnGSDisconnect(GameServer *gs)					// gamesvr断开 通知其他gamesvr场景缺失
{
	lgprotocal::LGUnRegisterGS urgs;

	urgs.scene_count = 0;

	int gs_scene_size = (int)gs->scene_list.size();

	for (int s = 0; s < gs_scene_size; ++s)
	{
		RemoveGSServer(gs->scene_list[s]);
		if (urgs.scene_count != MAX_SCENE_LIST_NUM)
		{
			urgs.scene_list[urgs.scene_count++] = gs->scene_list[s];
		}
	}

	NetID netid = gs->netid;
	delete gs;
	m_gs_list[netid] = 0;

	int gs_size = (int)m_gs_list.size();
	for (int g = 0; g < gs_size; ++g)
	{
		GameServer *gameserver = m_gs_list[g];
		if (NULL != gameserver)
		{
			InternalComm::Instance().NetSend(gameserver->netid, (const char *)&urgs, sizeof(lgprotocal::LGUnRegisterGS));
		}
	}
}

void LoginServer::OnGateDisconnect(NetID netid)				// gateway断开 清理m_gameway_use_list的多余数据 避免被随机到没意义的
{
	GateWayServer *gw = m_gateway_list[netid];

	RemoveGateWay(gw);

	delete gw;
	m_gateway_list[netid] = 0;
}

void LoginServer::OnStoryBattleInfoReq(NetID netid, const char* data)
{
	Story::Instance().OnStoryBattleInfoReq(netid);
}

void LoginServer::OnStoryRoundResultReq(NetID netid, const char* data)
{
	Protocol::CSStoryRoundResult* result = (Protocol::CSStoryRoundResult*)data;
	Story::Instance().OnStoryRoundResultReq(netid, result->round);
}

void LoginServer::OnRegisterGS(GameServer *gs, const char *data)
{
	lgprotocal::GLRegisterGS *rgs = (lgprotocal::GLRegisterGS *)data;

	gs->index = rgs->index;
	gs->ip = rgs->ip;
	gs->port = rgs->port;

	for (int i = 0; i < rgs->scene_count && i < MAX_SCENE_LIST_NUM; ++i)
	{
		gs->scene_list.push_back(rgs->scene_list[i]);
		AddGSServer(rgs->scene_list[i], gs);
	}

	lgprotocal::LGRegisterGS other_rgs;
	other_rgs.index = rgs->index;
	other_rgs.scene_count = rgs->scene_count;
	memcpy(other_rgs.scene_list, rgs->scene_list, sizeof(rgs->scene_list));

	int gs_size = (int)m_gs_list.size();
	for (int g = 0; g < gs_size; ++g)
	{
		GameServer *gameserver = m_gs_list[g];
		if (NULL != gameserver && 0 != gameserver->port)
		{
			// 通知其他场景服务器 新进来场景服务携带的所有场景信息 
			InternalComm::Instance().NetSend(gameserver->netid, (const char *)&other_rgs, sizeof(lgprotocal::LGRegisterGS));

			if (gameserver->index != gs->index)
			{
				lgprotocal::LGRegisterGS my_rgs;
				my_rgs.scene_count = 0;
				my_rgs.index = gameserver->index;

				for (std::vector<int>::iterator s = gameserver->scene_list.begin(), s_end = gameserver->scene_list.end();
					my_rgs.scene_count < MAX_SCENE_LIST_NUM && s != s_end; ++s)
				{
					my_rgs.scene_list[my_rgs.scene_count++] = *s;
				}

				// 通知新进来场景服务器 其他场景服务器场景信息 
				InternalComm::Instance().NetSend(gs->netid, (const char *)&my_rgs, sizeof(lgprotocal::LGRegisterGS));
			}
		}
	}

	// 通知所有的gateway有新gamesvr进来
	{
		lwprotocal::LWOnGSRegister ogsr;
		ogsr.index = rgs->index;
		F_STRNCPY(ogsr.ip, rgs->ip, sizeof(ogsr.ip));
		ogsr.port = rgs->port;

		for (int w = 0; w < (int)m_gateway_list.size(); ++w)
		{
			GateWayServer *gw = m_gateway_list[w];
			if (NULL != gw && 0 != gw->port)
			{
				InternalComm::Instance().NetSend(gw->netid, (const char *)&ogsr, sizeof(ogsr));
			}
		}
	}

	// 有新的gateway进来，让其连接battle_server
	BattleServerMap::iterator it = m_battle_server_map.begin();
	for (; it != m_battle_server_map.end(); ++it)
	{
		if (it->second.hostname != "" && -1 != it->second.index)
		{
			lgprotocal::LGRegisterBattle ogsr;
			ogsr.index = it->second.index;
			F_STRNCPY(ogsr.ip, it->second.hostname.c_str(), sizeof(ogsr.ip));
			ogsr.port = it->second.game_port;
			InternalComm::Instance().NetSend(gs->netid, (const char*)&ogsr, sizeof(ogsr));
		}
	}
}

void LoginServer::OnAccept(Port listen_port, NetID netid, IP ip, Port port)
{
	m_log.printf(LL_DEBUG, "LoginServer::OnAccept.");
}

void LoginServer::OnRecv(IP ip, NetID netid, const char *data, int length)
{
	if (length < (int)sizeof(Protocol::MessageHeader))
	{
		EngineAdapter::Instance().NetDisconnect(netid);
		return;
	}

	Protocol::MessageHeader *header = (Protocol::MessageHeader *)data;

	switch (header->msg_type)
	{
	case Protocol::MT_LOGIN_TO_ACCOUNT_CS:
		if (length == sizeof(Protocol::CSLoginToAccount))
		{
			OnLoginToAccountReq(ip, netid, data);
		}
		break;

	case Protocol::MT_LOGIN_TO_ROLE_CS:
		if (length == sizeof(Protocol::CSLoginToRole))
		{
			OnLoginToRoleReq(ip, netid, data);
		}
		break;

	case Protocol::MT_CREATE_ROLE_REQ_CS:
		if (length == sizeof(Protocol::CSCreateRole))
		{
			OnCreateRole(ip, netid, data);
		}
		break;

	case Protocol::MT_L_HEARTBEAT_CS:
		{
			Protocol::SCLHeartBeat hb;
			EngineAdapter::Instance().NetSend(netid, (const char *)&hb, sizeof(hb));
		}
		break;

	case Protocol::MT_GET_RANDOM_NAME_CS:
		if (length == sizeof(Protocol::CSRandomNameReq))
		{
			OnGetRandomName(netid,data);
		}
		break;

	case Protocol::MT_STORY_BATTLE_INFO_CS:
		if (length == sizeof(Protocol::CSStoryBattleInfo))
		{
			OnStoryBattleInfoReq(netid, data);
		}
		break;

	case Protocol::MT_STORY_ROUND_RESULT_CS:
		if (length == sizeof(Protocol::CSStoryRoundResult))
		{
			OnStoryRoundResultReq(netid, data);
		}
		break;

	default:
		m_log.printf(LL_DEBUG, "OnRecv Unknow msg type:%d.", header->msg_type);
		EngineAdapter::Instance().NetDisconnect(netid);
		break;
	}
}

void LoginServer::OnDisconnect(NetID netid)
{
	m_log.printf(LL_DEBUG, "LoginServer::OnDisconnect.");
}

GameServer * LoginServer::GetGSServer(int scene_id)
{
	GSSceneListIt it = m_scene_list.find(scene_id);

	if (it != m_scene_list.end())
	{
		return it->second;
	}

	return NULL;
}

void LoginServer::BroadcastToAllGSServer(const char *data, int sendlen)
{
	int gs_size = (int)m_gs_list.size();
	for (int g = 0; g < gs_size; ++g)
	{
		GameServer *gameserver = m_gs_list[g];
		if (NULL != gameserver && 0 != gameserver->port)
		{
			InternalComm::Instance().NetSend(gameserver->netid, data, sendlen);
		}
	}
}

void LoginServer::AddGSServer(int scene_id, GameServer *gs)
{
	GSSceneListIt it = m_scene_list.find(scene_id);
	if (it != m_scene_list.end()) return;
	m_scene_list[scene_id] = gs;
}

void LoginServer::RemoveGSServer(int scene_id)
{
	GSSceneListIt it = m_scene_list.find(scene_id);
	if (it == m_scene_list.end()) return;
	m_scene_list.erase(it);
}

LoginServer::GateWayServer * LoginServer::RandomGatewayServer()
{
	int gw_size = (int)m_gameway_use_list.size();
	if (0 == gw_size) return NULL;

	int r = rand() % gw_size;
	return m_gameway_use_list[r];
}

bool LoginServer::RandCrossGateway(ARG_OUT std::string* host_name, ARG_OUT int* host_port)
{
	if (m_cross_gateway.gateway_num <= 0)
	{
		return false;
	}

	int idx = rand() % m_cross_gateway.gateway_num;
	*host_name = m_cross_gateway.gateway_list[idx].cross_gw_hostname_for_user;
	*host_port = m_cross_gateway.gateway_list[idx].cross_gw_port_for_user;
	return true;
}

void LoginServer::AddGateWay(GateWayServer *gw)
{
	m_gameway_use_list.push_back(gw);
}

void LoginServer::RemoveGateWay(GateWayServer *gw)
{
	for (GateWayListIt it = m_gameway_use_list.begin(); it != m_gameway_use_list.end(); ++it)
	{
		if ((*it)->netid == gw->netid)
		{
			m_gameway_use_list.erase(it);
			break;
		}
	}
}

class LoginCmdTimer : public ITimerCallback
{
public:
	LoginServer *login_server;

	virtual void OnTimer()
	{
		login_server->StartCmd();
	}

	virtual void Free() { delete this; }
};

void RMILoginGetCmdBackObjectImpl::GetCmdRet(int ret, const CmdList &cmd_list)
{
	if (ret != 0) return;

	static CmdResult result;
	result.count = 0;

	for (int i = 0; i < cmd_list.count && i < CmdList::MAX_CMD_NUM; ++i)
	{
		std::string error_msg;
		login_server->OnCmd(cmd_list.cmd_list[i].cmd_id, 
			cmd_list.cmd_list[i].creator, cmd_list.cmd_list[i].cmd,
			error_msg, cmd_list.cmd_list[i].opid);

		result.result_list[result.count].cmd_id = cmd_list.cmd_list[i].cmd_id;
		F_STRNCPY(result.result_list[result.count].opresult, error_msg.c_str(), sizeof(OPResult));
		result.count += 1;

		if (result.count >= CmdResult::MAX_RESULT_NUM)
		{
			RMIWriteCmdResultBackObjectImpl *callback_obj = new RMIWriteCmdResultBackObjectImpl();
			RMISystemClient sc;
			sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
			sc.WriteCmdResult(RMISystemClient::CMD_TYPE_LOGIN, callback_obj, &result);
		}
	}

	
	if (result.count > 0)
	{
		RMIWriteCmdResultBackObjectImpl *callback_obj = new RMIWriteCmdResultBackObjectImpl();
		RMISystemClient sc;
		sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
		sc.WriteCmdResult(RMISystemClient::CMD_TYPE_LOGIN, callback_obj, &result);
	}
}

void LoginServer::StartCmd()
{
	RMILoginGetCmdBackObjectImpl *gpnbo = new RMILoginGetCmdBackObjectImpl();
	gpnbo->login_server = this;

	unsigned int timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	RMISystemClient sc;
	sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
	sc.GetCmd(RMISystemClient::CMD_TYPE_LOGIN, gpnbo, timestamp);

	static const unsigned int CMD_CHECK_INTERVAL = ServerConfigPool::Instance().common_config.system_cmd_check_interval_s;

	LoginCmdTimer *timer = new LoginCmdTimer();
	timer->login_server = this;
	EngineAdapter::Instance().CreateTimerSecond(CMD_CHECK_INTERVAL, timer);
}

void RMIForbidBackObjectImpl::ForbidRet(int result, int db_index, int role_1, int role_2, int role_3, int role_4, int role_5)
{
	loginlog::g_log_cmd.printf(LL_INFO, "ForbidRet %s [%d][%s] %s", (result == 0 ? "Suc" : "Fail"), cmd_id, creator.c_str(), cmd.c_str());
	
	if (0 == result)
	{
		UNSTD_STATIC_CHECK(5 == MAX_ACCOUNT_ROLE_NUM);
		int role_list[MAX_ACCOUNT_ROLE_NUM] = { role_1, role_2, role_3, role_4, role_5 };
		for (int i = 0; i < MAX_ACCOUNT_ROLE_NUM; ++i)
		{
			if (role_list[i] > 0)
			{
				lgprotocal::LGKickRole lkr;
				lkr.role_id = role_list[i];
				lkr.forbid_time_s = forbid_time_s;
				LoginServer::Instance().BroadcastToAllGSServer((const char*)&lkr, sizeof(lkr));
			}		
		}
	}
}

bool LoginServer::OnCmd(int cmd_id, const std::string &creator, const std::string &cmd, ARG_OUT std::string& error_msg, long long opid)
{
	std::istringstream iss;
	iss.str(cmd);

	std::string c;
	if ((iss >> c).fail())
	{
		error_msg = "String Stream Get Param Failed";
		return false;
	}

	error_msg = "SUCCESS";

	// 命令非常简单，先简单这么写，以后命令多了再改

	if (c == "Forbid")	// 封号
	{
		std::string pname_tmp;
		unsigned int forbid_time;
		int forbid_role_id = 0;
		if (!(iss >> pname_tmp >> forbid_time >> forbid_role_id).fail() && pname_tmp.size() < sizeof(PlatName))
		{
			PlatName pname;
			STRNCPY(pname, pname_tmp.c_str(), sizeof(pname));

			RMIForbidBackObjectImpl *fboi = new RMIForbidBackObjectImpl();
			fboi->login_server = this;
			F_STRNCPY(fboi->pname, pname, sizeof(fboi->pname));
			fboi->cmd_id = cmd_id;
			fboi->creator = creator;
			fboi->cmd = cmd;
			fboi->forbid_time_s = forbid_time;

			RMILoginClient lc;
			lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
			bool ret = lc.FrobidAsyn(pname, forbid_time, forbid_role_id, fboi);
			if (!ret)
			{
				loginlog::g_log_cmd.printf(LL_WARNING, "RMICallFail [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
				error_msg = "RMICallFail";
			}
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("AddLoginUserTrace" == c)
	{
		std::string pname_tmp;
		if (!(iss >> pname_tmp).fail() && pname_tmp.size() < sizeof(PlatName))
		{
			PlatName pname;
			STRNCPY(pname, pname_tmp.c_str(), sizeof(pname));

			LoginUserTrace::Instance().AddPName(pname);
			loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("ClearLoginUserTrace" == c)
	{
		LoginUserTrace::Instance().ClearPNameList();
		loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
	}
	else if ("SetLoginUserTraceFlag" == c)
	{
		int open_flag = 0;
		if (!(iss >> open_flag).fail())
		{
			LoginUserTrace::Instance().SetOpenFlag(0 != open_flag);
			loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("ControlNewbiePlotOpenClose" == c)
	{
		int is_open = 0;
		if (!(iss >> is_open).fail() && (0 == is_open || 1 == is_open))
		{
			bool res = EngineAdapter::Instance().ConfigSetValue(ROOT/LOGIN_MODULE/"NewbiePlot"/"IsOpen", is_open);
			if (res)
			{
				RoleInitAttr::Instance().SetNewbiePlotOpenClose(is_open);	
			}	
			loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s  res:%d", cmd_id, creator.c_str(), cmd.c_str(), res);
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("ReloadLoginConfig" == c)
	{
		int config_type = 0;
		int param1 = 0, param2 = 0;
		if (!(iss >> config_type).fail())
		{
			bool ret = ReloadConfigManager::Instance().ReloadConfig(config_type, param1, param2);
			if (ret)
			{
				loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s  res:%d", cmd_id, creator.c_str(), cmd.c_str(), 0);
			}
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("setCreateRoleControl" == c)
	{
		int is_open = 0;
		if (!(iss >> is_open).fail() && (0 == is_open || 1 == is_open))
		{
			bool res = EngineAdapter::Instance().ConfigSetValue(ROOT/LOGIN_MODULE /"CreateRoleControl"/"IsOpen", is_open);
			if (res)
			{
				SpidConfig::Instance()->SetCreateRoleControlSwitch(is_open);
			}
			loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s  res:%d", cmd_id, creator.c_str(), cmd.c_str(), res);
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else if ("setOnlineRoleNumThreshold" == c) // 设置同时在线玩家数阈值
	{
		int threshold = 0;
		if (!(iss >> threshold).fail() && threshold >= 0)
		{
			bool res = EngineAdapter::Instance().ConfigSetValue(ROOT / LOGIN_MODULE / "OnlineRoleNumThreshold", threshold);
			if (res)
			{
				SpidConfig::Instance()->SetOnlineRoleNumThreshold(threshold);
			}
			loginlog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s  res:%d", cmd_id, creator.c_str(), cmd.c_str(), res);
		}
		else
		{
			error_msg = "ParamError";
		}
	}
	else
	{
		// 命令分析不成功
		error_msg = "CmdNotFound";
		loginlog::g_log_cmd.printf(LL_WARNING, "Parse Error : [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
	}

	return true;
}

void RMIWriteCmdResultBackObjectImpl::WriteResultRet(int ret)
{
	loginlog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::WriteResultRet ret:%d", ret);
}

void RMIWriteCmdResultBackObjectImpl::__exception(int error)
{
	loginlog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::__exception error:%d", error);
}

void RMIWriteCmdResultBackObjectImpl::__timeout()
{
	loginlog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::__timeout");
}


void LoginServer::SetProfNum(int prof1_num, int prof2_num, int prof3_num, int prof4_num)
{
	m_prof1_num = prof1_num;
	m_prof2_num = prof2_num;
	m_prof3_num = prof3_num;
	m_prof4_num = prof4_num;
}

void LoginServer::GetProfNum(int *prof1_num, int *prof2_num, int *prof3_num, int *prof4_num)
{
	if (NULL != prof1_num) *prof1_num = m_prof1_num;
	if (NULL != prof2_num) *prof2_num = m_prof2_num;
	if (NULL != prof3_num) *prof3_num = m_prof3_num;
	if (NULL != prof4_num) *prof4_num = m_prof4_num;
}

void LoginServer::OnRoleForbid(lgprotocal::RoleForbid *rf)
{
	if (rf->forbit_time_s <= 0) return;

	rf->plat_name[sizeof(rf->plat_name) - 1] = 0;
	rf->reason[sizeof(rf->reason) - 1] = 0;

	RMIForbidBackObjectImpl *fboi = new RMIForbidBackObjectImpl();
	fboi->login_server = this;
	F_STRNCPY(fboi->pname, rf->plat_name, sizeof(fboi->pname));
	fboi->cmd_id = 0;
	fboi->creator = rf->reason;
	fboi->cmd = "";
	fboi->forbid_time_s = rf->forbit_time_s;

	RMILoginClient lc;
	lc.__bind_session(InternalComm::Instance().GetDBHandler().GetAcounterDB());
	bool ret = lc.FrobidAsyn(rf->plat_name, rf->forbit_time_s, rf->role_id, fboi);
	
	loginlog::g_log_role_forbid.printf(LL_INFO, "Role Forbid %s! plat_name:%s reason:%s", ret ? "Succ" : "Fail", rf->plat_name, rf->reason);
}

void LoginServer::OnRegisterCrossGateway(lgprotocal::GLRegisterCrossGateway* msg)
{
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_cross_gateway.gateway_list) == ARRAY_ITEM_COUNT(msg->gateway_list));

	m_cross_gateway.gateway_num = 0;
	for (int i = 0; i < msg->gateway_num && m_cross_gateway.gateway_num < ARRAY_ITEM_COUNT(m_cross_gateway.gateway_list); ++i)
	{
		m_cross_gateway.gateway_list[m_cross_gateway.gateway_num].cross_gw_hostname_for_user = msg->gateway_list[i].hostname;
		m_cross_gateway.gateway_list[m_cross_gateway.gateway_num].cross_gw_port_for_user = msg->gateway_list[i].port;
		m_cross_gateway.gateway_num += 1;
	}

	m_cross_gateway.is_cross_connected = true;
	m_cross_gateway.cross_gw_register_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void LoginServer::OnCrossDisconnect(lgprotocal::GLOnCrossDisconnect* msg)
{
	m_cross_gateway.is_cross_connected = false;
}

void LoginServer::OnSyncOnlineRoleNum(lgprotocal::GLSyncOnlineRoleNum* msg)
{
	this->SetOnlineRoleNum(msg->online_role_num);
}

