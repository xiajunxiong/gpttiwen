#ifndef  __BIG_CROSS_MANAGER_H__
#define  __BIG_CROSS_MANAGER_H__

#include "engineadapter.h"
#include "inetworkmodule.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include <set>

class BigCrossManager
{
public:
	static BigCrossManager & Instance();

	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned int _now_second);

	// 
	void OnRecvMsg(NetID _netid, const char * _msg, int _length);

	// 是否已经链接上大跨服服务器
	bool IsBigCrossServerConnected();
	bool IsBigCrossServerConnecting();

	// 链接大跨服服务器
	void ConnectToBigCrossServer();
	void DisconnectFromBigCrossServer();

	void OnConnectBigCrossServerSucc(NetID netid);
	void OnDisconnectFromBigCrossServer(bool is_from_disconnect);
	void CheckBigCrossServerConnect(unsigned int _now_second);

	void SyncGameServerInfoToBigCross(int _sync_type, std::set<int > & _server_set);
	void SyncGameServerInfoToBigCross(int _sync_type, int _count, int* _server_list);

	void OnUserLogin(const UserID& user_id);
	void OnUserLoginHidden(const UserID& user_id);
	void OnUserLoginOrigin(const UserID& user_id);
private:
	// 发送秘钥验证
	void SecretKey();
	// 注册服务器信息
	void RegisterGameServer();

private:
	// 接收协议
	// 下面都是协议对应处理的函数
	typedef void (BigCrossManager::*OnBigCHGameHandler)(NetID _netid, const char * _data, int _length);
	OnBigCHGameHandler m_bigc_hgame_handler_list[bigchgameprotocol::MT_BIGC_HGAME_MAX];
	void InitBigCHGameHandler();

	void OnRegisterServerAck(NetID _netid, const char * _data, int _length);
	void OnSecretKeyAck(NetID _netid, const char * _data, int _length);
	void OnHeartBeat(NetID _netid, const char * _data, int _length);
	void OnGetUTAChampionRecordAck(NetID _netid, const char * _data, int _length);
	void OnSyncUTABaseInfo(NetID _netid, const char * _data, int _length);
	void OnRequestSyncUTATeamList(NetID _netid, const char * _data, int _length);
	void OnBigCrossSendMessageToUser(NetID _netid, const char * _data, int _length);
	void OnSyncMailInfo(NetID _netid, const char * _data, int _length);
	void OnSyncRoleReqRet(NetID _netid, const char * _data, int _length);
	void OnSendMsgToRole(NetID _netid, const char * _data, int _length);
	void OnGiveLotteryToRole(NetID _netid, const char * _data, int _length);
	void OnBigCRequestUTATeamMemberRank(NetID _netid, const char * _data, int _length);
	void OnBigCGiveUserUTAWinBoxReward(NetID _netid, const char * _data, int _length);
	void OnUTAGiveUserItems(NetID _netid, const char* _data, int _length);
	void OnUTAGiveServerItems(NetID _netid, const char* _data, int _length);
	void OnSyncUTASchedules(NetID _netid, const char* _data, int _length);
	void OnUTAScheduleTeamMemberDetailResp(NetID _netid, const char* _data, int _length);
	void OnTransferMsgFromBigcrossBattleToHidden(NetID _netid, const char* _data, int _length);
	void OnSyncVideoData(NetID _netid, const char* _data, int _length);
	void OnUTASyncBattleInfoForOB(NetID _netid, const char* _data, int _length);
	void OnUTASyncRoundResultForOB(NetID _netid, const char* _data, int _length);
	void OnUTABattleEndStopOB(NetID _netid, const char* _data, int _length);
	void OnUTAChampionRecordRequestUserUpdateData(NetID _netid, const char* _data, int _length);
	void OnUTAUserEnterSceneContinueBattle(NetID _netid, const char* _data, int _length);
	void OnBattleEndNoticeTeam(NetID _netid, const char* _data, int _length);
private:
	BigCrossManager();
	~BigCrossManager();

	bool m_has_start_connect_to_bigc;							// 标记
	bool m_has_connected_to_bigc;								// 标记
	unsigned int m_bigc_server_check_timestamp;					// BigCross服务器重练检查时间戳
	unsigned int m_bigc_server_hearbeat_timestamp;				// 心跳检查时间戳
	int m_bigc_server_connect_ret;								// BigCross服务器链接结果
};


#endif	//__BIG_CROSS_MANAGER_H__
