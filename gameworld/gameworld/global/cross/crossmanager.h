#ifndef __CROSS_MANAGER__
#define __CROSS_MANAGER__

#include "inetworkmodule.h"
#include "servercommon/servercommon.h"
class Role;
class CrossManager
{
public:
	static CrossManager & Instance();

	void ConnectToCrossServerAsyn();
	void DisconnectFromCrossServer();

	void OnConnectCrossServerSucc(NetID netid);
	void OnDisconnectFromCrossServer(bool is_from_disconnect);
	void CheckCrossConnection();

	bool ReloadCrossConfig();

	unsigned int GetCrossOpenTamp();
	void OnCrossOpenTamp(unsigned int cross_open_tamp);

	int GetHiddenServerIDX();
	void OnSetHiidenServerIDX(int hidden_server_idx);

	unsigned int GetCrossOldestServerOpenTamp();
	void OnCrossOldestServerOpenTamp(unsigned int cross_oldest_open_tamp);

	bool IsCrossServerConnected();
	bool IsHiddenServerAlive();
	void OnHiddenHeartBeat();

	void OnCrossHello(bool is_cross_connected_hidden);
	void Update(unsigned long interval, time_t now_second);
	void SetCrossConnectRet(int ret) { m_cross_connect_ret = ret;}
	bool CanStartCross(Role *role);
	void SendAllUserCrossConnectInfo();

	void SyncCrossDataToHidden();
	bool IsCrossServerConnecting();

	void CMDSyncInfoToHidden(int _type);
private:
	CrossManager();
	~CrossManager();

	void OnReloadCrossServerConfig(bool is_cross_addr_change, bool old_is_open);

	unsigned int m_cross_hello_timestamp;
	bool m_has_start_connect_to_cross;
	bool m_has_connected_to_cross;
	int m_cross_connect_ret;
	unsigned int m_last_recieve_hidden_hello_timestamp;
	bool m_is_cross_connected_hidden;
	unsigned int m_cross_open_server_tamp;
	unsigned int m_cross_oldest_open_server_tamp;
	int m_hidden_server_idx;							//从隐藏服同步过来的序号

	unsigned int m_next_check_cross_open_server_stamp;
};

#endif

