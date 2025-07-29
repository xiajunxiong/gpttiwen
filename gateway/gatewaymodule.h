#pragma once

#ifndef SERVERLOGICMODULE_H
#define SERVERLOGICMODULE_H

#include <limits.h>
#include <vector>

#include "imodule.h"
#include "utility/logagent.h"
#include "irmimodule.h"
#include "inetworkmodule.h"

static const char GATEWAY_MODULE[] = "GatewayModule";

class ServerNetworkCallback;
class ServerInternalNetCallback;
class IClockModule;
class IGameModule;
class ITimerModule;

class GatewayModule : public IModule
{
	friend class ServerNetworkCallback;
	friend class ServerInternalNetCallback;
public:
	GatewayModule(int gateway_index);
	~GatewayModule();

	virtual int Init();
	virtual int Start();
	virtual int Update();
	virtual int Stop();
	virtual int Release();

	virtual void Free();

	void StopServer();

	void OnRecvUserMsg(NetID netid, IP userip, const char *data, int length);
	void OnUserDisconnect(NetID netid);
private:
	bool ConnectToChatServer();
	bool ConnectToGlobalServer();
	bool ConnectToLoginServer();
	bool ListenForUser();

	bool RegisterToLogin();

	LogAgent		m_log;

	IClockModule	*m_clock;
	IGameModule		*m_game;
	INetworkModule	*m_network;
	INetworkModule	*m_internal_network;
	ITimerModule	*m_timer;

	unsigned long	m_current_time;

	ServerNetworkCallback *m_network_callback;
	ServerInternalNetCallback *m_internal_network_callback;

	NetID			m_login_server_id;
	NetID			m_chat_server_id;
	NetID			m_global_server_id;

	void OnRecvLoginServerMsg(const char *data, int length);
	void OnRegisterGS(const char *data);
	void OnRegisterBattleServer(const char* data);

	struct GS 
	{
		GS():index(-1), port(-1), netid(-1)
		{
		}
		int				index;
		std::string		ip;
		unsigned short	port;
		NetID			netid;
		void Reset()
		{
			index = -1;
			ip = "";
			port = -1;
			netid = -1;
		}
	};
	typedef std::vector<GS>	GSList;
	GSList		m_gs_list;

	void OnRecvGSMsg(const char *data, int length);
	void OnRecvSend(const char *data, int length);
	void OnRecvMulSend(const char *data, int length);
	void OnRecvSendAll(const char *data, int length);
	void OnRecvDisconnect(const char *data, int length);
	void OnHasCheck(const char *data, int length);
	void OnRecvBattleSend(const char* data, int length);

	struct User 
	{
		User():netid(-1), userip(0), userport(0), gs_index(-1), role_id(0), last_active_time(0), has_checked(false){}
		NetID	netid;
		IP		userip;
		Port	userport;
		int		gs_index;
		int     role_id;
		unsigned long last_active_time;
		bool	has_checked;
		void Reset()
		{
			netid = -1;
			gs_index = -1;
			last_active_time = 0;
			has_checked = false;
			role_id = 0;
		}
	};
	typedef std::vector<User> UserList;
	UserList	m_user_list;

protected:
	// 超时相关
	void EraseTimeOut();
	bool			m_check_timeout;
	unsigned long	m_last_check_timeout;
	unsigned long	m_check_timeout_interval;
	unsigned long	m_heartbeat_dead_time;

protected:
	char	*m_buffer;
	char	*m_point_to_msg_beg;

public:
	void FlushRecord();
protected:
	// 消息统计相关
	struct MsgRecordItem
	{
		MsgRecordItem():cur_num(0), total_num(0), total_pack_size(0){}
		long long cur_num;
		long long total_num;
		long long total_pack_size;
	};
	static const int MAX_MSG_TYPE = (int)USHRT_MAX + 1;
	MsgRecordItem m_msg_recorder[MAX_MSG_TYPE];

	LogAgent m_msg_record_log;
	LogAgent m_disconnect_log;
	LogAgent m_user_check_log;
	
	void RecordMsg(unsigned short type, int num, int size);
	
private:
	int m_gateway_index;

	struct BattleServerInfo
	{
		BattleServerInfo() : battle_server_index(-1), battle_server_net_id(-1), battle_server_port(0), battle_server_connect_handle(-1) {}

		int battle_server_index;
		NetID battle_server_net_id;
		std::string battle_server_ip;
		unsigned short battle_server_port;
		int battle_server_connect_handle;
	};
	typedef std::map<int, BattleServerInfo> BattleServerMap;

	BattleServerInfo* GetBattleServerInfo(int index);
	
	BattleServerMap m_battle_server_map;
};

#endif
