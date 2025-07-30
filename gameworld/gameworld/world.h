#ifndef __WORLD_H__
#define __WORLD_H__

#include <map>
#include <set>
#include <vector>
#include <string>

#include "engineadapter.h"
#include "inetworkmodule.h"
#include "scene/scenemanager.hpp"
#include "servercommon/serverdef.h"

class ILogModule;
class IConfigModule;
class IGameModule;
class INetworkModule;
class IClockModule;
class IRMIModule;
class ITimerModule;
class InternalCommMonitor;
class LogAgent;
class World
{
public:
	struct GateWayInfo
	{
		GateWayInfo() : netid(-1), gateway_index(-1) { }

		NetID netid;
		int gateway_index;
		std::string hostname_for_user;
		unsigned short port_for_user;
	};
	typedef std::map<NetID, GateWayInfo> GateWayMap;

	struct BattleInfo
	{
		BattleInfo() : battle_server_port(0), battle_server_index(0), has_start_connect_to_battle(0), 
			has_connected_to_battle(0), battle_server_netid(NetID(-1)), reconnect_handle(-1), handle_battle_num(0) {}

		std::string		battle_server_ip;
		unsigned short	battle_server_port;
		int				battle_server_index;
		bool			has_start_connect_to_battle;
		bool			has_connected_to_battle;
		NetID			battle_server_netid;
		int				reconnect_handle;
		int				handle_battle_num;
	};
	typedef std::map<int, BattleInfo> BattleInfoMap;
	typedef std::map<NetID, BattleInfo*> BattleInfoNetidHash;	

	struct MotherServerInfo
	{
		UniqueServerID mother_server_usid;
		std::string gw_hostname;
		unsigned short gw_port;
	};

	struct ServerOpenTimeInfo
	{
		unsigned int server_real_start_time;
		unsigned int server_start_zero_time;
		unsigned int server_start_six_time;
	};

	typedef std::map<UniqueServerID, MotherServerInfo> MotherServerHash;
	typedef std::map<UniqueServerID, ServerOpenTimeInfo> ServerOpenTimeMap;
	typedef std::map<UniqueServerID, int> CanCreatAutoRobotMap;
public:
	World(ILogModule *log, 
		IConfigModule *config, 
		IGameModule *game, 
		INetworkModule *network, 
		INetworkModule *internal_network, 
		IClockModule *clock,
		IRMIModule *rmi_module,
		ITimerModule *timer,
		IFileWriteModule *filewrite,
		int total_server,
		int server_index);
	~World();

	static World * GetInstWorld();

	bool Init();
	bool Start(bool check_res_mode, bool unittest_mode, int total_server, int server_index);
	void Update();
	void Stop(bool check_res_mode);
	void Release();

	void OnAccept(Port listen_port, NetID netid, IP ip, Port port);
	void OnRecv(NetID netid, const char *data, int length);
	void OnDisconnect(NetID netid);

	void OnInternalAccept(Port listen_port, NetID netid, IP ip, Port port);
	void OnInternalAcceptByTelnet(Port listen_port, NetID netid, IP ip, Port port);
	void OnInternalRecv(NetID netid, const char *data, int length);
	void OnInternalDisconnect(NetID netid);
	void OnDisconnectTelnet();

	void ReConnectToLogin();
	bool RegisterToLogin();

	void ReConnectToBattle(int index);

	bool SendToAllGateway(const char *msg, int length);
	bool SendSerializeToAllGateway(IMessageSerializer* msg);

	bool SendToALLGameAllGateway(const void * msg, int length);
	bool SendToRoleAny(int role_id, const void * msg, int length);
	void NoticeNumAny(int role_id, int error_num);
	bool SendToRole(int role_id, const void * msg, int length);
	void NoticeNum(int role_id, int error_num);

	bool IsAllDataLoadFinish(); 
	void OnAllDataLoadFinish();

	void OnMonthResetGlobalModuleData();
	void OnWeekResetGlobalModuleData();
	void OnResetGlobalModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnTwoWeekChange();
	void OnMonthChange();
	void OnUserLogin(const UserID& user_id);

	void OnDataaccessHeartbeat();

	SceneManager * GetSceneManager() { return &m_scene_manager; }

	bool ReloadScene(const std::string &config_dir, std::string *err);

	int GetGateWayIndexByNetId(NetID netid);
	NetID GetGateWayNetIdByIndex(int index);

	const BattleInfo* GetBattleServerInfoConst(int index);
	BattleInfo* GetBattleServerInfoByReconnectHandle(int handle);
	void OnBattleServerConnect(NetID netid, BattleInfo* info);
	bool SendMsgToAllBattle(const char* data, int data_len);
	bool SendMsgToOneBattle(int index, const char* data, int data_len);
	bool SendSerializeMsgToOneBattle(int index, IMessageSerializer* serializer);
	bool ChooseBattleIndex(int* out_index);
	void ModifyBattleIndexHandleNum(int index, int modify_num);
	bool HasBattleServerConnected();

	bool SendToTelnet(const char* data, int length);
	
	void OnCrossRegisterGameServer(UniqueServerID usid, const char* ip, Port port, int plat_type, int server_num, unsigned int open_server_timestamp,int* server_list, 
		const char* gw_hostname, unsigned short gw_port);
	const UniqueServerID *GetMotherServerId(const UniqueServerID &kid_server_id);		// 跨服中，用从合服子服id获取合服母服id
	const MotherServerInfo* GetMotherServerInfo(const UniqueServerID& kid_server_id);
	bool RandOneMotherServerId(ARG_OUT int* mother_server_id);
	unsigned int GetRealServerOpenTime(const UniqueServerID& usid);
	unsigned int GetServerOpenZeroTime(const UniqueServerID& usid);
	unsigned int GetServerOpenSixTime(const UniqueServerID& usid);
	int GetOpenServerDays_6(const UniqueServerID& usid);
	int GetOpenServerDays_0(const UniqueServerID& usid);
	int GetCurRealOpenDay(const UniqueServerID &usid);
	int GetOpenDayByTimestamp(const UniqueServerID &usid, unsigned int timestamp);
	int GetCurRealOpenDayByOpenTimestamp(unsigned int open_timestamp);
	int GetDayByTimestamp(unsigned int begin_timestamp, unsigned int end_timestamp);
	int GetCurRealOpenDay();
	time_t GetMaxServerOpenTime();

	void SetCreatAutoRobotServerID(UniqueServerID usid, int can_creat_static_robot);
	bool RandCanCreatAutoRobotServerID(ARG_OUT int* mother_server_id);


	void WriteItemToWorkbench(char* path);
	void GetOneGatewayInfo(ARG_OUT std::string* hostname, ARG_OUT unsigned short* port);
	inline const GateWayMap& GetGatewayMap() const { return m_gateway_map; }
	bool RandGatewayInfo(ARG_OUT std::string* hostname, ARG_OUT unsigned short* port);

	void LOG_QUICK(int server_id, LogAgent &log_agent, long long n1, 
		long long n2, const char *z1, const char *z2, long long n3, 
		long long n4, const char *z3, const char *z4, long long n5, 
		long long n6, long long n7, long long n8, long long n9);

	bool SyncLogToOriginServer(int server_id, int log_type, const char *format, ...);
	inline InternalCommMonitor* GetCrossMonitor() const { return m_cross_comm_monitor; }
	inline InternalCommMonitor* GetBattleMonitor() const { return m_battle_comm_monitor; }
	inline InternalCommMonitor* GetLoginMonitor() const { return m_login_comm_monitor; }

	bool IsOriginServerConnected(int plat_type, int server_id);
	void OnOriginServerDisconnectCross(int plat_type, int server_id);

	void SendNoticeNumToUser(const UserID& user_id, int notice_num);
	bool TrySendMsgToUser(const UserID& user_id, const void* msg, unsigned int length);
	bool TrySendSerializeMsgToUser(const UserID& user_id, IMessageSerializer* msg);

	void GetServerIDSet(std::set<int> & _out_server_set);

protected:
	void RecalNextResetDataTimeStamp();
	bool ListenForGateway();
	bool ListenForTelnet();

	bool ConnectToDB();
	bool HiddenConnectToGameDB(UniqueServerID server_id, const char* ip, Port port);
	bool ConnectToLoginServer();

	bool ConnectToBattleServerAsyn(int index);

	bool LoadFromDBDirectly();

	void OnServerStart();
	void OnServerStop();

	void UpdateGlobal(unsigned long intervals);

	bool InitGameWorld(std::string config_dir, bool check_res_mode);
	bool InitGlobalConfig(const std::string &config_dir);						// 全局配置 存放一些不太多的模块配置
	bool InitScene(const std::string &config_dir);								// 场景
	bool InitLogicConfig(const std::string &config_dir, bool check_res_mode);	// 逻辑配置
	bool InitItem(const std::string &config_dir);								// 物品
	bool InitNPC(const std::string &config_dir);								// NpcManager
	bool InitTask(const std::string &config_dir);								// 任务
	bool InitSkill(const std::string &config_dir);								// 技能
	bool InitMonster(const std::string &config_dir);							// 怪物
	bool InitBattleCommonConfig(const std::string &config_dir);					// 战斗公共配置
	bool InitDrop(const std::string &config_dir);								// 掉落
	bool InitTradeMarketConfig(const std::string& config_dir);					// 交易行配置
	bool InitNoticeConfig(const std::string& config_dir);						// 公告配置
	bool InitRobotConfig(const std::string& config_dir);						// 机器人配置
	bool InitGodEquipConfig(const std::string& config_dir);						// 神装配置
	bool InitUTAConfig(const std::string& config_dir);
	bool InitHotfixConfig(const std::string& config_dir);

	int StartUnitTest();
private:
	// from gateway
	void OnRecvMsg(NetID netid, const char *data, int length);
	void OnNetDisconnect(NetID netid, const char *data);
	void OnGateRegisterToGame(NetID netid, const char *data, int length);

	// from loginserver
	void OnLGKickRole(const char *msg);

	// for hidden world loading cache on register original world
	bool LoadOriginDBGlobalCache(UniqueServerID unique_sid);

	// 请求跨服数据同步
	void GetCrossDataSync(UniqueServerID _unique_sid);

	bool InitRedisConfig(ARG_OUT bool& is_open, ARG_OUT std::string& ip, ARG_OUT int& port, ARG_OUT std::string auth, ARG_OUT int connect_timeout_ms);

	bool InitAsyncTaskModule();
	bool RegisterAsyncTaskFactory();
	void CheckHotFixConfig();

	void ReleaseConfig();

	INetworkModule *m_network;
	unsigned long m_game_time;

	GateWayMap		m_gateway_map;

	std::string		m_local_ip;
	Port			m_listen_port;

	std::string		m_telnet_ip;
	NetID			m_telnet_netid;
	Port			m_telnet_listen_port;

	int				m_total_server;
	int				m_server_index;

	bool			m_is_on_server_start;
	unsigned int	m_last_update_dayid;

	unsigned int	m_last_reset_role_module_data_dayid;
	time_t			m_last_reset_role_module_timestamp;
	time_t			m_next_reset_role_module_timestamp;

	unsigned int	m_next_dataaccess_heartbeat_timestamp;
	int				m_dataaccess_heartbeat_try_times;

	SceneManager	m_scene_manager;

	char			*m_buffer;	// 用于NetSend是组包

private:
	// 战斗服的信息，由login同步过来，暂时只保存一个，以后可以扩展成多战斗服
	void OnLGRegisterBattle(const char* msg);
	BattleInfo* GetBattleServerInfo(int index);

	BattleInfoMap	m_battle_info_map;
	BattleInfoNetidHash m_battle_info_netid_hash;

private:
	void HiddenSendHeartBeatToGame();

	// 跨服信息
	MotherServerHash m_mother_server_hash;
	ServerOpenTimeMap m_server_open_time_map;
	//可创自动机器人的主服id
	CanCreatAutoRobotMap m_can_creat_auto_robot_map;


	unsigned int m_hidden_next_send_heartbeat_to_game_timestamp;

	InternalCommMonitor* m_cross_comm_monitor;
	InternalCommMonitor* m_battle_comm_monitor;
	InternalCommMonitor* m_login_comm_monitor;
};

#endif
