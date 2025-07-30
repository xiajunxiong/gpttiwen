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

	// �Ƿ��Ѿ������ϴ���������
	bool IsBigCrossServerConnected();
	bool IsBigCrossServerConnecting();

	// ���Ӵ���������
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
	// ������Կ��֤
	void SecretKey();
	// ע���������Ϣ
	void RegisterGameServer();

private:
	// ����Э��
	// ���涼��Э���Ӧ����ĺ���
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

	bool m_has_start_connect_to_bigc;							// ���
	bool m_has_connected_to_bigc;								// ���
	unsigned int m_bigc_server_check_timestamp;					// BigCross�������������ʱ���
	unsigned int m_bigc_server_hearbeat_timestamp;				// �������ʱ���
	int m_bigc_server_connect_ret;								// BigCross���������ӽ��
};


#endif	//__BIG_CROSS_MANAGER_H__
