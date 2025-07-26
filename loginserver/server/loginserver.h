#ifndef __LOGINSERVER_H__
#define __LOGINSERVER_H__

#include <map>
#include <vector>
#include <set>

#include "servercommon/logindef.h"

#include "inetworkmodule.h"

#include "gameserver.h"
#include "utility/logagent.h"

class Platform;
namespace lgprotocal
{
	struct RoleForbid;
	struct GLRegisterCrossGateway;
	struct GLOnCrossDisconnect;
	struct GLSyncOnlineRoleNum;
}

class LoginServer
{
public:
	~LoginServer();

	static LoginServer & Instance();

	bool Init(const std::vector<int> &db_index_to_create, ILogModule *log, const std::string &configpath, 
		int min_login_interval_s, int can_not_login_global_time);

	void Update();
	
	void OnAccept(Port listen_port, NetID netid, IP ip, Port port);			// 接入用户 
	void OnRecv(IP ip, NetID netid, const char *data, int length);
	void OnDisconnect(NetID netid);
	
	void OnGSAccept(Port listen_port, NetID netid, IP ip, Port port);		// 接入Game
	void OnGateAccept(Port listen_port, NetID netid, IP ip, Port port);		// 接入Gateway
	void OnBattleServerAccept(Port listen_port, NetID net_id, IP ip, Port port); // 接入BattleServer
	
	void OnGSDisconnect(GameServer *gs);									// Game断开
	void OnGateDisconnect(NetID netid);										// Geteway断开 

	void OnInnerRecv(NetID netid, const char *data, int length);
	void OnInnerDisconnect(NetID netid);

	GameServer * GetGSServer(int scene_id);
	void BroadcastToAllGSServer(const char *data, int sendlen);

	void StartCmd();
	bool OnCmd(int cmd_id, const std::string &creator, const std::string &cmd, ARG_OUT std::string& error_msg, long long opid);

	void SetProfNum(int prof1_num, int prof2_num, int prof3_num, int prof4_num);
	void GetProfNum(int *prof1_num, int *prof2_num, int *prof3_num, int *prof4_num);

	void SetTotalRoleNum(int total_role_num) { m_total_role_num = total_role_num; }
	int GetTotalRoleNum() const { return m_total_role_num; }

	void SetOnlineRoleNum(int online_role_num) { m_online_role_num = online_role_num; }
	int GetOnlineRoleNum() const { return m_online_role_num; }
protected:
	LoginServer();

	LogAgent		m_log;

	int				m_min_login_interval_s;

	Platform		*m_platform;

	void OnLoginToAccountReq(IP ip, NetID netid, const char *data);
	void OnLoginToRoleReq(IP ip, NetID netid, const char *data);
	void OnCreateRole(IP ip, NetID netid, const char *data);
	void OnDestroyRole(NetID netid, const char *data);
	void OnGetRandomName(NetID netid, const char* data);
	void OnStoryBattleInfoReq(NetID netid, const char* data);
	void OnStoryRoundResultReq(NetID netid, const char* data);

	// 目前login与gameworld和gateway都是1:1的关系， 但暂时保留可以扩展成1:n的代码
	typedef std::vector<GameServer*> GSList;
	typedef std::vector<GameServer*>::iterator GSListIt;
	GSList m_gs_list; // 接入的gamesvr列表
	
	typedef std::map<int, GameServer*> GSSceneList;
	typedef std::map<int, GameServer*>::iterator GSSceneListIt;
	GSSceneList m_scene_list;	

	void OnRegisterGS(GameServer *gs, const char *data);
	void AddGSServer(int scene_id, GameServer *gs);
	void RemoveGSServer(int scene_id);

	void OnGSRecv(GameServer *gs, const char *data, int length);

	void OnRoleForbid(lgprotocal::RoleForbid *rf);
	void OnRegisterCrossGateway(lgprotocal::GLRegisterCrossGateway* msg);
	void OnCrossDisconnect(lgprotocal::GLOnCrossDisconnect* msg);
	void OnSyncOnlineRoleNum(lgprotocal::GLSyncOnlineRoleNum* msg);
public:
	struct GateWayServer
	{
		GateWayServer() : netid(-1), port(0) {}

		NetID			netid;
		std::string		hostname;
		unsigned short	port;
	};

	GateWayServer * RandomGatewayServer();

	struct CrossGatewayForUser
	{
		CrossGatewayForUser() : gateway_num(0), is_cross_connected(false), cross_gw_register_time(0u) {}

		struct GatewayInfo
		{
			unsigned short cross_gw_port_for_user;
			std::string cross_gw_hostname_for_user;
		};

		int gateway_num;
		bool is_cross_connected;
		unsigned int cross_gw_register_time;
		GatewayInfo gateway_list[5];
	};

	bool IsCrossConnected() const { return m_cross_gateway.is_cross_connected; }
	bool RandCrossGateway(ARG_OUT std::string* host_name, ARG_OUT int* host_port);
	unsigned int GetCrossGatewayRegisterTime() const { return m_cross_gateway.cross_gw_register_time; }

protected:
	typedef std::vector<GateWayServer*>	GateWayList;
	typedef std::vector<GateWayServer*>::iterator GateWayListIt;
	GateWayList		m_gateway_list;										// 快速索引使用 

	GateWayList		m_gameway_use_list;									// 当前有用的队列 仅仅用于随机选择gateway
	
	CrossGatewayForUser m_cross_gateway;

	void AddGateWay(GateWayServer *gw);
	void RemoveGateWay(GateWayServer *gw);

	void OnRegisterGW(NetID netid, const char *data);
	void OnGateRecv(NetID netid, const char *data, int length);

	time_t m_last_check_prof_num_timestamp;
	int m_prof1_num;
	int m_prof2_num;
	int m_prof3_num;
	int m_prof4_num;

	time_t m_last_check_role_num_timestamp;
	int m_total_role_num;
	int m_online_role_num;
public:
	struct BattleServer
	{
		BattleServer() : netid(ItemID(-1)), index(-1), gateway_port(0), game_port(0) {}

		NetID netid;
		int index;
		std::string hostname;
		Port gateway_port;
		Port game_port;
	};

protected:
	void OnRegisterBattleServer(NetID netid, const char* data);
	void OnBattleServerRecv(NetID netid, const char* data, int length);

	void OnSyncStoryBattleInfo(NetID netid, const char* data, int length);
	void OnSyncStoryRoundResult(NetID netid, const char* data, int length);

	typedef std::map<NetID, BattleServer> BattleServerMap;	// 
	BattleServerMap m_battle_server_map;
};

#endif


