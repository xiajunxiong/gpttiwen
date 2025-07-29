
#include "common/platform/system.h"
#include "common/platform/socket/pisocket.h"
#include "common/syspath.h"

#include "servercommon/servercommon.h"
#include "iclockmodule.h"
#include "inetworkmodule.h"
#include "ilogmodule.h"
#include "igamemodule.h"
#include "iconfigmodule.h"
#include "irmimodule.h"
#include "itimermodule.h"

#include "gatewaymodule.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "engineadapter.h"
#include "utility/configpath.h"

#include "servercommon/internalprotocal/lwprotocal.h"
#include "servercommon/internalprotocal/wgprotocal.h"
#include "servercommon/userprotocal/msgheader.h"

#include "servercommon/userprotocal/chatmsgcode.h"
#include "servercommon/userprotocal/globalmsgcode.h"
#include "servercommon/userprotocal/battlemsgcode.h"

#include "servercommon/serverconfig/serverconfigpool.h"

#include "gameserver/game.h"

#include "common/memory/memmonitor.h"

#include "servercommon/userprotocal/msgbattle.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/userprotocal/msgsystem.h"

class ServerNetworkCallback : public IEngineNetCallback
{
public:
	ServerNetworkCallback(GatewayModule *gateway):m_gateway(gateway){}
	virtual ~ServerNetworkCallback(){}
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)
	{
		m_gateway->m_log.printf(LL_MAINTANCE, "Network OnAccept netid:%d.", netid);

		if (netid > 100000)	// netid网络层是从0开始顺序分配并回收重复使用的，怎么可能连接10W个啊
		{
			m_gateway->m_network->Disconnect(netid);
			return;
		}

		if (netid >= m_gateway->m_user_list.size())
		{
			while (netid >= m_gateway->m_user_list.size())
			{
				m_gateway->m_user_list.resize((m_gateway->m_user_list.size() + 1) * 2);
			}
		}
		m_gateway->m_user_list[netid].netid = netid;
		m_gateway->m_user_list[netid].userip = ip;
		m_gateway->m_user_list[netid].userport = port;
		m_gateway->m_user_list[netid].role_id = 0;
		m_gateway->m_user_list[netid].has_checked = false;
		m_gateway->m_user_list[netid].last_active_time = m_gateway->m_current_time;

#ifdef _DEBUG
		printf("OnAccept netid[%d] ip[%d] port[%d]\n", netid, ip, port);
#endif
	}
	virtual void OnRecv(NetID netid, const char *data, int length)
	{
		m_gateway->OnRecvUserMsg(netid, m_gateway->m_user_list[netid].userip, data, length);
		m_gateway->m_user_list[netid].last_active_time = m_gateway->m_current_time;
	}
	virtual void OnDisconnect(NetID netid)
	{
		m_gateway->m_log.printf(LL_DEBUG, "Network OnDisconnect netid:%d.", netid);

		// 处理用户断开
		m_gateway->OnUserDisconnect(netid);
		m_gateway->m_user_list[netid].Reset();
	}

	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port)
	{
	}
private:
	GatewayModule *m_gateway;
};

class ServerInternalNetCallback : public IEngineNetCallback
{
public:
	ServerInternalNetCallback(GatewayModule *gateway):m_gateway(gateway), m_reconnet_login_handle(-1) {}
	virtual ~ServerInternalNetCallback(){}
	void SetAllowIpPrefix(const std::string &allow_ip_prefix) { m_allow_ip_prefix = allow_ip_prefix; }
	virtual void OnAccept(Port listen_port, NetID netid, IP ip, Port port)
	{
		m_gateway->m_log.printf(LL_DEBUG, "Internal Network OnAccept netid:%d.", netid);

		// 检查IP前缀是否合法
		sockaddr_in host_adr;
		memset(&host_adr, 0, sizeof(sockaddr_in));
		host_adr.sin_family = AF_INET;
		host_adr.sin_addr.s_addr = htonl(ip);
		std::string ip_str = inet_ntoa(host_adr.sin_addr);
		std::string ip_record = ip_str;
		ip_str.resize(m_allow_ip_prefix.size());
		if (ip_str != m_allow_ip_prefix)
		{
			m_gateway->m_log.printf(LL_WARNING, "Internal Network Accept NOT allow ip[%s].", ip_record.c_str());
			m_gateway->m_internal_network->Disconnect(netid);
			return;
		}
	}
	virtual void OnRecv(NetID netid, const char *data, int length)
	{
		if (m_gateway->m_login_server_id == netid)
		{
			// 收到登陆服务器消息
			m_gateway->OnRecvLoginServerMsg(data, length);
			return;
		}

		// 其他的都是游戏服务器、战斗服务器、聊天服务器等，在此统一称之为GS，使用相同的处理方法
		m_gateway->OnRecvGSMsg(data, length);

	}
	virtual void OnDisconnect(NetID netid)
	{
		m_gateway->m_log.printf(LL_DEBUG, "Internal Network OnDisconnect netid:%d.", netid);

		if (m_gateway->m_login_server_id == netid)
		{
			// 与登陆服务器断开连接
			m_gateway->m_login_server_id = -1;

			// 启动重连机制
			ReConnectToLogin();

			return ;
		}

		// 其他的都是gameserver?
		for (int i = 0; i < (int)m_gateway->m_gs_list.size(); ++i)
		{
			if (m_gateway->m_gs_list[i].netid == netid)
			{
				m_gateway->m_gs_list[i].Reset();

				for (GatewayModule::UserList::iterator u = m_gateway->m_user_list.begin(); u != m_gateway->m_user_list.end(); ++u)
				{
					if (u->gs_index == i)
					{
						EngineAdapter::Instance().NetDisconnect(u->netid);
					}
				}
				return;
			}
		}
		if (m_gateway->m_chat_server_id == netid)
		{
			m_gateway->m_chat_server_id = -1;
		}
		if (m_gateway->m_global_server_id == netid)
		{
			m_gateway->m_global_server_id = -1;
		}

		GatewayModule::BattleServerMap::iterator it = m_gateway->m_battle_server_map.begin();
		for (; it != m_gateway->m_battle_server_map.end(); ++it)
		{
			if (it->second.battle_server_net_id == netid)
			{
				m_gateway->m_log.printf(LL_MAINTANCE, "OnDisconnect battle server[%s:%d] index[%d]", it->second.battle_server_ip.c_str(), it->second.battle_server_port, it->second.battle_server_index);
				it->second.battle_server_net_id = -1; // 等待重新注册重新连接
				return;
			}
		}
	}
	virtual void OnConnect(bool result, int handle, NetID netid, IP ip, Port port)
	{
		if (handle == m_reconnet_login_handle)
		{
			m_reconnet_login_handle = -1;

			if (!result)
			{
				ReConnectToLogin();
			}
			else
			{
				m_gateway->m_login_server_id = netid;
				m_gateway->RegisterToLogin();
			}

			return;
		}

		GatewayModule::BattleServerMap::iterator it = m_gateway->m_battle_server_map.begin();
		for (; it != m_gateway->m_battle_server_map.end(); ++it)
		{
			if (it->second.battle_server_connect_handle == handle)
			{
				
				it->second.battle_server_connect_handle = -1;
				if (!result)
				{
					m_gateway->m_log.printf(LL_MAINTANCE, "OnConnect battle server[%s:%d] index[%d] FAIL", it->second.battle_server_ip.c_str(), it->second.battle_server_port, it->second.battle_server_index);
					it->second.battle_server_ip = "";
					it->second.battle_server_port = -1;
					it->second.battle_server_index = -1;
				}
				else
				{
					it->second.battle_server_net_id = netid;
					m_gateway->m_log.printf(LL_MAINTANCE, "OnConnect battle server[%s:%d] index[%d] SUCC", it->second.battle_server_ip.c_str(), it->second.battle_server_port, it->second.battle_server_index);

					//向Battle注册自己
					wgprotocal::WBRegisterToBattle wgr;
					wgr.gate_index = m_gateway->m_gateway_index;
					InternalComm::Instance().NetSend(netid, (const char*)&wgr, sizeof(wgr));
				}
				return;
			}
		}
	}
private:
	void ReConnectToLogin()
	{
		std::string login_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.server_ip;
		Port login_server_port = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.for_gateway.port;
		int handle = -1;
		bool ret = m_gateway->m_internal_network->ConnectAsyn(login_server_ip.c_str(), login_server_port, &handle, 1000);
		if (ret)
		{
			m_reconnet_login_handle = handle;
		}
		else
		{
			// log
		}
	}

	GatewayModule *m_gateway;
	std::string m_allow_ip_prefix;
	int m_reconnet_login_handle;
};

GatewayModule::GatewayModule(int gateway_index)
	: m_clock(0), m_game(0), m_network(0), m_internal_network(0), m_timer(0),
	m_current_time(0), m_network_callback(0), m_internal_network_callback(0),
	m_check_timeout(true), m_last_check_timeout(0), m_check_timeout_interval(5000), m_heartbeat_dead_time(60000),
	m_buffer(0), m_point_to_msg_beg(0), m_gateway_index(gateway_index)
{
	m_network_callback = new ServerNetworkCallback(this);
	m_internal_network_callback = new ServerInternalNetCallback(this);

	m_buffer = new char [MAX_MSG_SIZE];	// 4M , 发一个包肯定够了!!!
	memset(m_buffer, 0, MAX_MSG_SIZE);

	m_point_to_msg_beg = m_buffer + sizeof(wgprotocal::WGNetRecvMsg);

	wgprotocal::WGNetRecvMsg nrm;
	wgprotocal::WGNetRecvMsg *msg = (wgprotocal::WGNetRecvMsg*)m_buffer;
	*msg = nrm;
}

GatewayModule::~GatewayModule()
{
	delete []m_buffer;
	delete m_network_callback;
	delete m_internal_network_callback;
}

void GatewayModule::Free()
{
	delete this;
}

void GatewayModule::StopServer()
{
	Game *game = (Game *)this->Interface();
	game->Stop();
}

int GatewayModule::Init()
{
	srand(PITime());

	EXPECT_ON_INIT(LOG_MODULE)

	ILogModule *log = dynamic_cast<ILogModule *>(Interface()->QueryModule(LOG_MODULE));

	m_log.SetLogModule(log);
	m_log.SetCatagory(GATEWAY_MODULE);

	return IModule::Succeed;
}

class MsgRecordTimer : public ITimerCallback
{
	GatewayModule *gateway;
public:
	MsgRecordTimer(GatewayModule *gw):gateway(gw){}
	virtual ~MsgRecordTimer(){}

	virtual void OnTimer()
	{
		gateway->FlushRecord();
	}
	virtual void Free() { delete this; }
};

class MsgMoniterTimer : public ITimerCallback
{
	int interval_s;

public:
	MsgMoniterTimer(int i):interval_s(i) {}

	virtual void OnTimer()
	{
		// static char buff[1024 * 10];

		// memmonitor::PrintDyMemInfo(buff);
		// printf("%s \n", buff);
		// gamelog::g_log_msgmoniter.print(LL_INFO, buff);

		EngineAdapter::Instance().CreateTimerSecond(interval_s, new MsgMoniterTimer(interval_s));
	}

	virtual void Free() { delete this; }
};

int GatewayModule::Start()
{
	DEPEND_ON_START(CLOCK_MODULE)
	DEPEND_ON_START(NETWORK_MODULE)
	DEPEND_ON_START(GAME_MODULE)
	DEPEND_ON_START(CONFIG_MODULE)
	DEPEND_ON_START(INTERNAL_NETWORK)
	DEPEND_ON_START(TIMER_MODULE)
	
	ILogModule *log = dynamic_cast<ILogModule *>(Interface()->QueryModule(LOG_MODULE));
	IConfigModule *config = dynamic_cast<IConfigModule *>(Interface()->QueryModule(CONFIG_MODULE));
	m_timer = dynamic_cast<ITimerModule *>(Interface()->QueryModule(TIMER_MODULE));
	m_clock = dynamic_cast<IClockModule*>(Interface()->QueryModule(CLOCK_MODULE));
	m_network = dynamic_cast<INetworkModule*>(Interface()->QueryModule(NETWORK_MODULE));
	m_internal_network = dynamic_cast<INetworkModule*>(Interface()->QueryModule(INTERNAL_NETWORK));
	m_game = dynamic_cast<IGameModule*>(Interface()->QueryModule(GAME_MODULE));
	EngineAdapter::Instance().Init(log, config, m_game, m_network, m_clock, m_timer);

	// 获取日志生成目录
	std::string log_dir = "log/";
	if (!config->GetValue(ROOT / "LogDir", &log_dir))
	{
		m_log.printf(LL_ERROR, "Gateway Server On Start, Config [%s] Read Failed!!", (ROOT / "LogDir").c_str());

		this->StopServer();

		return Fail;
	}
	if (log_dir[log_dir.length() - 1] != '/')
	{
		log_dir += '/';
	}

	char gamelog_dir[256] = {0};
	SNPRINTF(gamelog_dir, sizeof(gamelog_dir), "gatewaylog_%d/", m_gateway_index);
	std::string real_logdir = log_dir + std::string(gamelog_dir);

	m_msg_record_log.SetLogModule(log);
	m_msg_record_log.SetCatagory("MsgRecord", (real_logdir+"msgrecord.txt").c_str());

	m_disconnect_log.SetLogModule(log);
	m_disconnect_log.SetCatagory("Disconnect", (real_logdir+"disconnect.txt").c_str(), ILogModule::NFIS_HALF_HOUR);

	m_user_check_log.SetLogModule(log);
	m_user_check_log.SetCatagory("UserCheck", (real_logdir + "usercheck.txt").c_str(), ILogModule::NFIS_HALF_DAY);

	// 获取配置目录
	std::string config_dir = "../config";
	if (!config->GetValue(ROOT / "ConfigDir", &config_dir))
	{
		m_log.printf(LL_ERROR, "Gateway Server On Start, Config [%s] Read Failed!!", (ROOT / "ConfigDir").c_str());

		this->StopServer();

		return Fail;
	}
	if (config_dir[config_dir.length() - 1] != '/')
	{
		config_dir += '/';
	}

	// 初始化全局配置
	char err_info[1024]={0};
	if(!ServerConfigPool::Instance().Init(config_dir,err_info))
	{
		m_log.printf(LL_ERROR, err_info);
		m_log.printf(LL_ERROR, "Gateway Server On Start, ServerConfigPool::Init return FALSE.");

		this->StopServer();

		return Fail;
	}
	m_log.printf(LL_INFO, "Gateway Server On Start, ServerConfigPool::Init Success.");

	std::string allow_ip_prefix = "127.0.0.1";
	if (!config->GetValue(ROOT / GATEWAY_MODULE / "AllowIPPrefix", &allow_ip_prefix))
	{
		m_log.printf(LL_ERROR, "Gateway Server On Start, Config [%s] Read Failed!!", (ROOT / GATEWAY_MODULE / "AllowIPPrefix").c_str());

		this->StopServer();

		return Fail;
	}
	m_internal_network_callback->SetAllowIpPrefix(allow_ip_prefix);

	m_network->RegisterCallback(m_network_callback);
	m_internal_network->RegisterCallback(m_internal_network_callback);

	InternalComm::Instance().Init(m_internal_network);

	// 连接聊天服务器
	/*if (!ConnectToChatServer())
	{
		this->StopServer();

		return Fail;
	}*/

	// 连接到全局服务器
	/*if (!ConnectToGlobalServer())
	{
		this->StopServer();

		return Fail;
	}*/

	// 连接loginserver
	if (!ConnectToLoginServer())
	{
		this->StopServer();

		return Fail;
	}

	// 为玩家开listen端口
	if (!ListenForUser())
	{
		this->StopServer();

		return Fail;
	}

	m_check_timeout = ServerConfigPool::Instance().common_config.timeout.is_check_timeout;
	m_check_timeout_interval = ServerConfigPool::Instance().common_config.timeout.check_timeout_interval_ms;
	m_heartbeat_dead_time = ServerConfigPool::Instance().common_config.timeout.gateway_heartbeat_dead_time_ms;

	FlushRecord();

	m_log.printf(LL_MAINTANCE, "Gateway started");

	SysPath::CreateFileByPath("status/gate.start");

	static const int MSG_MONITER_INTERVAL_S = 5;
	EngineAdapter::Instance().CreateTimerSecond(MSG_MONITER_INTERVAL_S, new MsgMoniterTimer(MSG_MONITER_INTERVAL_S));

	return IModule::Succeed;
}

int GatewayModule::Update()
{
	// 处理超时
	unsigned long m_current_time = m_clock->GetFrameTime();

	if (m_check_timeout && (m_current_time - m_last_check_timeout > m_check_timeout_interval))
	{
#ifndef _DEBUG // 暂时改为内网不处理心跳超时的客户端，方便客户端调试
		EraseTimeOut();
#endif
		m_last_check_timeout = m_current_time;
	}

	return IModule::Pending;
}

int GatewayModule::Stop()
{
	SysPath::CreateFileByPath("status/gate.end");
	return IModule::Succeed;
}

int GatewayModule::Release()
{
	return IModule::Succeed;
}

bool GatewayModule::ConnectToChatServer()
{
	std::string chat_server_ip = "127.0.0.1";
	Port chat_server_port = 8777;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "ChatServer" / "Ip", &chat_server_ip))
	{
		m_log.printf(LL_MAINTANCE, "Connect to ChatServer, Config [%s] Read Failed!!!", (ROOT / GATEWAY_MODULE / "ChatServer" / "Ip").c_str());
		return false;
	}
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "ChatServer" / "Port", &chat_server_port))
	{
		m_log.printf(LL_MAINTANCE, "Connect to ChatServer, Config [%s] Read Failed!!!", (ROOT / GATEWAY_MODULE / "ChatServer" / "Port").c_str());
		return false;
	}

	bool ret1 = m_internal_network->Connect(chat_server_ip.c_str(), chat_server_port, &m_chat_server_id);
	if (!ret1)
	{
		m_log.printf(LL_MAINTANCE, "Connect to ChatServer[%s:%d] Fail!", chat_server_ip.c_str(), (int)chat_server_port);
		return false;
	}
	m_log.printf(LL_MAINTANCE, "Connect to ChatServer[%s:%d] suc.", chat_server_ip.c_str(), (int)chat_server_port);

	return true;
}
bool GatewayModule::ConnectToGlobalServer()
{
	std::string global_server_ip = "127.0.0.1";
	Port global_server_port = 9477;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GlobalServer" / "Ip", &global_server_ip))
	{
		m_log.printf(LL_MAINTANCE, "ConnectToGlobalServer Config [%s] Read Failed", (ROOT / GATEWAY_MODULE / "GlobalServer" / "Ip").c_str());
		return false;
	}
	
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GlobalServer" / "Port", &global_server_port))
	{
		m_log.printf(LL_MAINTANCE, "ConnectToGlobalServer Config [%s] Read Failed", (ROOT / GATEWAY_MODULE / "GlobalServer" / "Port").c_str());
		return false;
	}

	bool ret1 = m_internal_network->Connect(global_server_ip.c_str(), global_server_port, &m_global_server_id);
	if (!ret1)
	{
		m_log.printf(LL_MAINTANCE, "Connect to GlobalServer[%s:%d] Fail!", global_server_ip.c_str(), (int)global_server_port);
		return false;
	}
	m_log.printf(LL_MAINTANCE, "Connect to GlobalServer[%s:%d] suc.", global_server_ip.c_str(), (int)global_server_port);

	return true;
}
bool GatewayModule::ConnectToLoginServer()
{
	std::string login_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.server_ip;
	Port login_server_port = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.for_gateway.port;

	bool ret = m_internal_network->Connect(login_server_ip.c_str(), login_server_port, &m_login_server_id);
	if (!ret)
	{
		m_log.printf(LL_MAINTANCE, "Connect to LoginServer[%s:%d] Fail!", login_server_ip.c_str(), (int)login_server_port);
		return false;
	}
	m_log.printf(LL_MAINTANCE, "Connect to LoginServer[%s:%d] suc.", login_server_ip.c_str(), (int)login_server_port);

	return true;
}

bool GatewayModule::ListenForUser()
{
	int game_thread = 0;
	std::string hostname = "127.0.0.1";
	Port listen_port = 4777;
	int backlog = 64;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "GameThread", &game_thread))
	{
		m_log.printf(LL_MAINTANCE, "ListenForUser Config [%s] Read Failed!", (ROOT / "GameThread").c_str());
		return false;
	}
	
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP", &hostname))
	{
		m_log.printf(LL_MAINTANCE, "ListenForUser Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP").c_str());
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "ListenPort", &listen_port))
	{
		m_log.printf(LL_MAINTANCE, "ListenForUser Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "ListenPort").c_str());
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "Backlog", &backlog))
	{
		m_log.printf(LL_MAINTANCE, "ListenForUser Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "Backlog").c_str());
		return false;
	}
	
	listen_port += (m_gateway_index - 1);  // 在基础上进行偏移 

	bool ret = m_network->Listen(listen_port, backlog);
	if (!ret)
	{
		m_log.printf(LL_MAINTANCE, "NetListen return FAIL! listen_port[%d]", listen_port);
		return false;
	}
	m_log.printf(LL_MAINTANCE, "Gateway Listen[%s:%d].", hostname.c_str(), listen_port);

	lwprotocal::WLRegisterGW rgw;

	if (hostname.size() > sizeof(rgw.hostname) - 1)
	{
		m_log.print(LL_MAINTANCE, "HostName[LocalIP] too long!");
		return false;
	}
	
	STRNCPY(rgw.hostname, hostname.c_str(), sizeof(rgw.hostname));
	rgw.port = listen_port;

	m_internal_network->Send(m_login_server_id, (const char *)&rgw, sizeof(lwprotocal::WLRegisterGW));
	return true;
}

bool GatewayModule::RegisterToLogin()
{
	std::string hostname = "127.0.0.1";
	Port listen_port = 4777;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP", &hostname))
	{
		m_log.printf(LL_MAINTANCE, "RegisterToLogin Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP").c_str());
		return false;
	}
	
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "ListenPort", &listen_port))
	{
		m_log.printf(LL_MAINTANCE, "RegisterToLogin Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP").c_str());
		return false;
	}

	listen_port += (m_gateway_index - 1);  // 在基础上进行偏移 

	lwprotocal::WLRegisterGW rgw;

	if (hostname.size() > sizeof(rgw.hostname) - 1)
	{
		m_log.print(LL_MAINTANCE, "HostName[LocalIP] too long!");
		return false;
	}

	STRNCPY(rgw.hostname, hostname.c_str(), sizeof(rgw.hostname));
	rgw.port = listen_port;

	m_internal_network->Send(m_login_server_id, (const char *)&rgw, sizeof(lwprotocal::WLRegisterGW));

	return true;
}

void GatewayModule::EraseTimeOut()
{
	//m_log.printf(LL_DEBUG, "EraseTimeOut.");
	for (UserList::iterator i = m_user_list.begin(); i != m_user_list.end(); ++i)
	{
		if (i->netid == (unsigned int)(-1))
		{
			continue;
		}
		if (m_current_time - i->last_active_time > m_heartbeat_dead_time)
		{
			m_log.printf(LL_INFO, "EraseTimeOut netid:%d.", i->netid);
			m_network->Disconnect(i->netid);
		}
	}
}

/************************************************************************
	用户相关
************************************************************************/ 
void GatewayModule::OnRecvUserMsg(NetID netid, IP userip, const char *data, int length)
{
	static const int MAX_LENGTH = MAX_MSG_SIZE - sizeof(wgprotocal::WGNetRecvMsg);

	if (length < int(sizeof(Protocol::MessageHeader)) || length >= MAX_LENGTH)
	{
		m_network->Disconnect(netid);
		return;
	}

	Protocol::MessageHeader *header = (Protocol::MessageHeader*)data;
	unsigned short server_id = header->server_id;
	unsigned short msg_type = header->msg_type;
	//printf("server_index:%d\n", server_id);
	if (server_id < (unsigned short)m_gs_list.size()
		&& m_gs_list[server_id].index != -1)
	{
		NetID sendtonetid = m_gs_list[server_id].netid; // 先默认转发到gs

		// 暂时将所有请求发向GameWorld, 由GameWorld分发 -- by chenyulin

		// 服务器消息路由，此处简单分析用户消息协议头，以决定分发到不同的服务器
		//if (msg_type >= Protocol::BATTLE_SERVER_MSG_BEGIN && msg_type < Protocol::BATTLE_SERVER_MSG_END)
		//{
		//	if (m_user_list[netid].gs_index == -1)
		//	{
		//		m_log.printf(LL_DEBUG, "Netid[%d] Send to battleserver first, Disconnect it!", netid);
		//		m_network->Disconnect(netid);	// 未登陆gs则先发到战斗服务器则断开
		//		return ;
		//	}
		//
		//	sendtonetid = m_battle_server_net_id;
		//}

		//	if (msg_type > Protocol::GLOBAL_SERVER_SC_BEGIN)
		//	{
		//		sendtonetid = m_global_server_id;
		//	}
		//	else if (msg_type > Protocol::CHATCTRL_SC_BEGIN)
		//	{
		//		if (m_user_list[netid].gs_index == -1)
		//		{
		//			m_log.printf(LL_DEBUG, "Netid[%d] Send to charserver first, Disconnect it!", netid);
		//			m_network->Disconnect(netid);	// 未登陆gs则先发到聊天服务器则断开
		//			return ;
		//		}
		//		// 聊天相关消息，直接发送到聊天服务器
		//		sendtonetid = m_chat_server_id;
		//	}
		//	else
		//	{
		//		/*
		//		// 是发送到gameworld的
		//		if (m_user_list[netid].gs_index == -1)
		//		{
		//			m_log.printf(LL_DEBUG, "Netid[%d] Send to gameworld first, Disconnect it!", netid);
		//			m_network->Disconnect(netid);	// 未登陆gs则先发到聊天服务器则断开
		//			return ;
		//		}*/
		//	}
		//}

		wgprotocal::WGNetRecvMsg *msg = (wgprotocal::WGNetRecvMsg*)m_buffer;
		msg->header.netid = netid;
		msg->header.user_ip = userip;
		memcpy(m_point_to_msg_beg, data, length);

		m_internal_network->Send(sendtonetid, m_buffer, length + sizeof(wgprotocal::WGNetRecvMsg));

		m_user_list[netid].gs_index = (int)header->server_id;	// 记录服务器，用于断开的时候通知
																// 这样简单记录可能不是非常准确，错了就由逻辑层去做超时处理吧

		RecordMsg(msg_type, 1, length);

		if (Protocol::MT_HEARTBEAT_CS == msg_type)
		{
			Protocol::SCHeartbeatResp resp;
			m_network->Send(netid, (const char*)&resp, sizeof(resp));
		}
	}
	else
	{
		// 这小子乱发消息，果断地把你断开
		m_network->Disconnect(netid);
	}
}

void GatewayModule::OnUserDisconnect(NetID netid)
{
	int gs_index = m_user_list[netid].gs_index;
	if (gs_index != -1)
	{
		wgprotocal::WGNetDisconnect netdis;
		netdis.header.netid = netid;

		if (gs_index < (int)m_gs_list.size() && m_gs_list[gs_index].netid != (unsigned int)(-1))
		{
			m_internal_network->Send(m_gs_list[gs_index].netid, (const char *)&netdis, sizeof(wgprotocal::WGNetDisconnect));
		}

		//m_internal_network->Send(m_global_server_id, (const char*)&netdis, sizeof(wgprotocal::WGNetDisconnect));

		// 是否要发送到chatserver
#ifdef _DEBUG
		printf("GatewayModule::OnUserDisconnect user[%d] netid[%d]\n", m_user_list[netid].role_id, netid);
#endif
	}
}

/************************************************************************
	 收到登陆服务器的消息
************************************************************************/
void GatewayModule::OnRecvLoginServerMsg(const char *data, int length)
{
	lwprotocal::MessageHeader *header = (lwprotocal::MessageHeader*)data;
	switch (header->msg_type)
	{
	case lwprotocal::MT_LW_REGISTER_GS:
		OnRegisterGS(data);
		break;

	case lwprotocal::MT_LW_REGISTER_BATTLE:
		OnRegisterBattleServer(data);
		break;
	default:
		break;
	}
}

void GatewayModule::OnRegisterGS(const char *data)
{
	lwprotocal::LWOnGSRegister *ogsr = (lwprotocal::LWOnGSRegister *)data;
	ogsr->ip[sizeof(ogsr->ip) - 1] = '\0';

	if (ogsr->index >= 0 && ogsr->index < (int)m_gs_list.size())
	{
		if (m_gs_list[ogsr->index].index != -1 && m_gs_list[ogsr->index].ip == ogsr->ip && m_gs_list[ogsr->index].port == ogsr->port) // 已经连接过该GameWorld
		{
			return;
		}
	}

	NetID netid;
	bool ret = InternalComm::Instance().NetConnect(ogsr->ip, ogsr->port, &netid); 
	if (!ret)
	{
		m_log.printf(LL_MAINTANCE, "Connect to Gameserver[%s:%d] Fail!", ogsr->ip, (int)ogsr->port);
		return;
	}

	std::string gateway_hostname = "127.0.0.1";
	Port gateway_listen_port = 4777;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP", &gateway_hostname))
	{
		m_log.printf(LL_MAINTANCE, "GatewayModule::OnRegisterGS Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "LocalIP").c_str());
		return;
	}
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / GATEWAY_MODULE / "GameUser" / "ListenPort", &gateway_listen_port))
	{
		m_log.printf(LL_MAINTANCE, "GatewayModule::OnRegisterGS Config [%s] Read Failed!", (ROOT / GATEWAY_MODULE / "GameUser" / "ListenPort").c_str());
		return;
	}

	if ((int)m_gs_list.size() <= ogsr->index)
	{
		m_gs_list.resize(ogsr->index + 1);
	}

	m_gs_list[ogsr->index].index = ogsr->index;
	m_gs_list[ogsr->index].ip = ogsr->ip;
	m_gs_list[ogsr->index].port = ogsr->port;
	m_gs_list[ogsr->index].netid = netid;

	m_log.printf(LL_MAINTANCE, "Connect to Gameserver[%s:%d] index:%d.", ogsr->ip, (int)ogsr->port, ogsr->index);

	wgprotocal::WGRegisterToGame wgr;
	wgr.gate_index = static_cast<unsigned short>(m_gateway_index);
	wgr.port_for_user = gateway_listen_port;
	F_STRNCPY(wgr.hostname_for_user, gateway_hostname.c_str(), sizeof(wgr.hostname_for_user));
	InternalComm::Instance().NetSend(netid, (const char*)&wgr, sizeof(wgr));
}

void GatewayModule::OnRegisterBattleServer(const char* data)
{
	lwprotocal::LWOnBattleRegister *ogsr = (lwprotocal::LWOnBattleRegister *)data;
	ogsr->ip[sizeof(ogsr->ip) - 1] = '\0';

	BattleServerInfo* bsi = this->GetBattleServerInfo(ogsr->index);
	if (NULL != bsi)
	{
		if (-1 != bsi->battle_server_net_id || -1 != bsi->battle_server_connect_handle)
		{
			m_log.printf(LL_MAINTANCE, "GatewayModule::OnRegisterBattleServer battle_server[%d] already connected or connecting", ogsr->index);
			return;
		}
	}
	else
	{
		BattleServerInfo info;
		BattleServerMap::iterator it = m_battle_server_map.insert(BattleServerMap::value_type(ogsr->index, info)).first;
		bsi = &it->second;
	}

	bsi->battle_server_ip = ogsr->ip;
	bsi->battle_server_port = ogsr->port;
	bsi->battle_server_index = ogsr->index;

	m_log.printf(LL_MAINTANCE, "GatewayModule::OnRegisterBattleServer ip[%s] port[%d] index[%d]", ogsr->ip, (int)ogsr->port, ogsr->index);

	bool ret = InternalComm::Instance().NetConnectAsyn(ogsr->ip, ogsr->port, &bsi->battle_server_connect_handle);
	if (!ret)
	{
		m_log.printf(LL_MAINTANCE, "GatewayModule::OnRegisterBattleServer ConnectAsyn to Battle[%s:%d] index[%d] Fail!", ogsr->ip, (int)ogsr->port, ogsr->index);
		return;
	}
}

/************************************************************************
	收到gameserver来的消息
************************************************************************/
void GatewayModule::OnRecvGSMsg(const char *data, int length)
{
	wgprotocal::MessageHeader *header = (wgprotocal::MessageHeader*)data;
	switch (header->msg_type)
	{
	case wgprotocal::MT_NET_SEND_MSG:
		OnRecvSend(data, length);
		break;
	case wgprotocal::MT_NET_MUL_SEND_MSG:
		OnRecvMulSend(data, length);
		break;
	case wgprotocal::MT_NET_SEND_ALL:
		OnRecvSendAll(data, length);
		break;
	case wgprotocal::MT_NET_DISCONNECT:
		OnRecvDisconnect(data, length);
		break;
	case wgprotocal::MT_HAS_CHECK:
		OnHasCheck(data, length);
		break;
	case wgprotocal::MT_BATTLE_NET_SEND_MSG:
		OnRecvBattleSend(data, length);
		break;
	default:
		break;
	}
}

void GatewayModule::OnRecvSend(const char *data, int length)
{
	wgprotocal::MessageHeader *header = (wgprotocal::MessageHeader*)data;

	const char *senddata = data + sizeof(wgprotocal::GWNetSendMsg);
	int send_length = length - sizeof(wgprotocal::GWNetSendMsg);
	if (!m_network->Send((NetID)header->netid, senddata, length - sizeof(wgprotocal::GWNetSendMsg)))
	{
		m_network->Disconnect((NetID)header->netid);

		// printf("m_network->Send length:%d Failed \n", length); fflush(stdout);
	}

	Protocol::MessageHeader *user_header = (Protocol::MessageHeader*)senddata;
	RecordMsg(user_header->msg_type, 1, send_length);
}

void GatewayModule::OnRecvMulSend(const char *data, int length)
{
	wgprotocal::GWNetMulSendMsg *msg = (wgprotocal::GWNetMulSendMsg*)data;

	unsigned short *netid = (unsigned short *)(data + sizeof(wgprotocal::GWNetMulSendMsg));

	int crtmsglength = sizeof(wgprotocal::GWNetMulSendMsg) + msg->count * sizeof(unsigned short);
	const char *senddata = data + crtmsglength;
	int sendlength = length - crtmsglength;

	for (int i = 0; i < msg->count; ++i)
	{
		m_network->Send((NetID)*netid, senddata, sendlength);
		++netid;
	}

	Protocol::MessageHeader *user_header = (Protocol::MessageHeader *)senddata;
	RecordMsg(user_header->msg_type, msg->count, sendlength);
}

void GatewayModule::OnRecvSendAll(const char *data, int length)
{
	//wgprotocal::GWNetSendAll *sa = (wgprotocal::GWNetSendAll*)data;

	const char *senddata = data + sizeof(wgprotocal::GWNetSendAll);
	int send_length = length - sizeof(wgprotocal::GWNetSendAll);

	int count = 0;
	for (UserList::iterator i = m_user_list.begin(); i != m_user_list.end(); ++i)
	{
		if (i->netid != (NetID)(-1) && i->has_checked)
		{
			m_network->Send(i->netid, senddata, send_length);
			++count;
		}
	}

	Protocol::MessageHeader *user_header = (Protocol::MessageHeader *)senddata;
	RecordMsg(user_header->msg_type, count, send_length);
}

void GatewayModule::OnRecvDisconnect(const char *data, int length)
{
	wgprotocal::GWNetDisconnect *nd = (wgprotocal::GWNetDisconnect *)data;

	nd->role_name[sizeof(nd->role_name) - 1] = 0;
	nd->server_name[sizeof(nd->server_name) - 1] = 0;
	nd->reason[sizeof(nd->reason) - 1] = 0;

	m_network->Disconnect((NetID)nd->header.netid);

	m_disconnect_log.printf(LL_INFO, "[Gateway::OnDisconnect Succ][user[%s %d] server_name:%s scene_id:%d reason:[%s]]", nd->role_name, nd->uid, nd->server_name, nd->scene_id, nd->reason);
}

void GatewayModule::OnHasCheck(const char *data, int length)
{
	wgprotocal::GWHasCheck *hc = (wgprotocal::GWHasCheck*)data;
	if (hc->header.netid < m_user_list.size())
	{
		m_user_list[hc->header.netid].has_checked = true;
		m_user_list[hc->header.netid].role_id = hc->role_id;
		User& user = m_user_list[hc->header.netid];
		m_user_check_log.printf(LL_INFO, "OnHasCheck user[%d,%s] ip[%s] port[%d]", hc->role_id, hc->role_name, GetIPStr(user.userip), user.userport);

#ifdef _DEBUG
		printf("OnHasCheck user[%d,%s] ip[%s] port[%d]\n", hc->role_id, hc->role_name, GetIPStr(user.userip), user.userport);
#endif
	}
}

void GatewayModule::OnRecvBattleSend(const char* data, int length)
{
	wgprotocal::BWNetSendMsg* bwnsm = (wgprotocal::BWNetSendMsg*)data;
	NetID netid = bwnsm->header.netid;
	if (netid >= m_user_list.size() || (bwnsm->role_id > 0 && m_user_list[netid].role_id != bwnsm->role_id))
	{
		return; // 判断role_id可能不正确，就放弃发送
	}

	wgprotocal::MessageHeader *header = (wgprotocal::MessageHeader*)data;

	const char *senddata = data + sizeof(wgprotocal::BWNetSendMsg);
	if (!m_network->Send((NetID)header->netid, senddata, length - sizeof(wgprotocal::BWNetSendMsg)))
	{
		m_network->Disconnect((NetID)header->netid);

		// printf("m_network->Send length:%d Failed \n", length); fflush(stdout);
	}

	Protocol::MessageHeader *user_header = (Protocol::MessageHeader*)senddata;
	RecordMsg(user_header->msg_type, 1, length - sizeof(wgprotocal::BWNetSendMsg));
}

void GatewayModule::RecordMsg(unsigned short type, int num, int size)
{
	m_msg_recorder[type].cur_num += num;
	m_msg_recorder[type].total_num += num;
	m_msg_recorder[type].total_pack_size += size;
}

GatewayModule::BattleServerInfo* GatewayModule::GetBattleServerInfo(int index)
{
	BattleServerMap::iterator it = m_battle_server_map.find(index);
	if (it == m_battle_server_map.end()) return NULL;

	return &it->second;
}

void GatewayModule::FlushRecord()
{
	m_msg_record_log.buff_printf("type[cur:total] ");

	for (int i = 0; i < MAX_MSG_TYPE; ++i)
	{
		if (m_msg_recorder[i].total_num != 0)
		{
			m_msg_record_log.buff_printf("%d[%u:%u:%lld] ", (int)i, (unsigned int)m_msg_recorder[i].cur_num, (unsigned int)m_msg_recorder[i].total_num, (unsigned long long)m_msg_recorder[i].total_pack_size);
			m_msg_recorder[i].cur_num = 0;
			m_msg_recorder[i].total_pack_size = 0;
		}
	}
	m_msg_record_log.commit_buff(LL_INFO);

	m_timer->CreateTimerSecond(600, new MsgRecordTimer(this));
}
