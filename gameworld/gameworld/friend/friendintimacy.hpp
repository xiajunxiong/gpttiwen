#ifndef __FRIEND_INTIMACY_HPP__
#define __FRIEND_INTIMACY_HPP__

#include "gamedef.h"
#include "servercommon/struct/friendintimacyparam.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include <map>
#include <set>


enum FRIEND_INTIMACY_DATA_STATUS
{
	FRIEND_INTIMACY_DATA_STATUS_INVALID = 0,						// ��ʼ״̬
	FRIEND_INTIMACY_DATA_STATUS_LOADING,							// ���ݼ����� 
	FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH,						// ���ݼ������ 
	FRIEND_INTIMACY_DATA_STATUS_MAX,
};

static const int FRIEND_INTIMACY_SAVE_INTERVAL = 60;				// ����ӻ�дһ��

class Role;

class FriendIntimacyManager
{
	friend class RMIInitFriendIntimacyBackObjectImpl;
public:
	typedef std::map<long long , FriendIntimacyInfo > FriendIntimacyMap;				// < friend_for_index , ��ҵĺ������ܶ���Ϣ >
	typedef std::map<long long , FriendIntimacyInfo >::iterator	FriendIntimacyMapIt;	

public:
	static FriendIntimacyManager & Instance();
	void InitFriendIntimacyParam(int _ret, FriendIntimacyListParam & _param);
	void OnConnectHiddenSucc();
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	void OnLogin(Role * _role);

	// ����ͬ�����ĺ������ܶ���Ϣ  ��Ϣ������Դ������ Ҳ������Դ�� ���ط�
	void OnSyncFriendIntimacyInfo(crossgameprotocal::SyncFriendIntimacyInfo * _info);
	// ��������������  �޸����ܶȲ���
	void OnAddIntimacyReqFromGameWorld(crossgameprotocal::GameHiddenAddIntimacyReq * _req);
	// ��������������  ���ܶȹ�ϵ����
	void OnIntimacyRelationReqFromGameWorld(crossgameprotocal::GameHiddenIntimacyRelationReq * _req);
	//  ��������������  ���ܶ�˽��ʱ�������
	void OnSetIntimacySingleChatTimesFromGameWorld(crossgameprotocal::GameHiddenIntimacySingleChatTimes * _times);
	//  ��������������  ���ܶ�˽��ʱ������� [��Ҳ��������� , ����û�ط�����]
	void OnGiveFlowerNoticeFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerNotice * _notice);
	//  �����������ط���  ���ܶ�˽��ʱ������� [��Ҳ��������� , ����û�ط�����]
	void OnGiveFlowerNoticeFromHidden(crossgameprotocal::HiddenGameGiveFlowerNotice * _notice);
	//  ��������������  �ͻ����� [��Ҳ��������� , ����û�ط�����]
	void OnGiveFlowerBackKissFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerBackKiss * _back_kiss);
	//  �����������ط���  �ͻ����� [��Ҳ��������� , ����û�ط�����]
	void OnGiveFlowerBackKissFromHidden(crossgameprotocal::HiddenGameGiveFlowerBackKiss * _back_kiss);



	// ��ȡ����ѵ����ܶ�
	int GetFriendIntimacy(int _role_uid, int _friend_uid);
	// ���Ӻ��ѵ����ܶ�
	void AddIntimacy(int _role_uid, int _friend_uid , int _add_val , const  char * _reason);
	//�������ܶ�Ϊ0
	void ClearIntimacy(int _role_uid, int _friend_uid, const  char * _reason);
	// ��ȡ���һ�ε�ÿ���״�˽�ĵ�ʱ���
	// to : ��ҶԺ��� 
	unsigned int GetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id);
	// from : ���Ѷ���ҵ�
	unsigned int GetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id);
	// �������һ�ε�ÿ���״�˽�ĵ�ʱ���
	// to : ��ҶԺ��� 
	void SetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id , unsigned int _to_times ,const  char * _reason);
	// from : ���Ѷ���ҵ�
	void SetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id, unsigned int _from_times, const  char * _reason);
	// ��Ӻ���
	void OnAddFriend(int _role_id, int _friend_id);
	// ɾ������
	void OnRemoveFriend(int _role_id, int _friend_id);

	// �߻� :����ÿ��ĵ�һ��˽����Ҫ�����ܶ�
	void OnSingleChat(int _role_id, int _friend_id, unsigned int now_times, const char * _reason);

	// ���ս��Ҳ��Ҫ�����ܶ�
	void OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg);

	// �ͻ��Ĺ㲥֪ͨ [Ҫ˵Ϊɶ�ͻ��ķ��ںøж��� , ��Ϊû�ط�����!!!]
	void GiveFlowerNotify(int _role_id, int _friend_id , int _item_id);
	// �ͻ����� 
	void OnBackToKiss(Role * _role, int _to_role_id);

private:
	FriendIntimacyManager();
	~FriendIntimacyManager();

	void Save();
	void SaveParam();
	void ClearDirtyMark();
	void SetDirtyMark(long long _friend_for_index);
	void CheckLoadFinish();

	// ͬ���������ܶ�����  ԭ��  -> ���ط�
	void SyncAllDataToHidden();
	// ͬ��������������  ���ط� -> ԭ��  
	void SyncFriendIntimacyDataToGameWorld(long long friend_for_index, int _sync_status);
	// �����ط�������Դ��������ͬ����Ϣ
	void InitFriendIntimacyInfoFromGameWorld(crossgameprotocal::SyncFriendIntimacyInfo * _info);
	void UpdataFriendIntimacyInfoFromHidden(crossgameprotocal::SyncFriendIntimacyInfo * _info);

	long long  AddFriendInfo(int _role_id, int _friend_id, const FriendIntimacyInfo & _item);
	long long  RemoveFriendInfo(int _role_id, int _friend_id);

	void AddIntimacyByHidden(int _role_uid, int _friend_uid, int _add_val, const  char * _reason);
	void AddIntimacyByGameWorld(int _role_uid, int _friend_uid, int _add_val,const  char * _reason);

	void OnAddFriendByHidden(int _role_id , int _friend_id);
	void OnRemoveFriendByHidden(int _role_id, int _friend_id);
	void OnIntimacyRelationReqToHidden(int _role_id, int _friend_id , bool _is_add);

	void SendList(Role * _role);
	void SendChangeInfo(Role * _role, int friend_id);

	bool IsCanAddInitimacyBattleType(int _battle_type);

	void SetLastDayFristToSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristFromSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristToSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristFromSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SendLastDayFristSingleChatTimesToHidden(int _role_uid, int _friend_id, unsigned int _times, bool is_to);


	void GiveFlowerNotifyByHidden(int _role_id, int _friend_id, int _item_id, int _type);
	void GiveFlowerNotifyByGameWorld(int _role_id, int _friend_id, int _item_id, int _type);

	// �ͻ����� 
	void OnBackToKissByHidden(int _role_id , GameName _role_name , int _to_role_id);
	void OnBackToKissByGameWorld(Role * _role, int _to_role_id);

	int m_data_state;
	bool m_load_flag;

	// ����ϵͳ�Ƿ������ݷ��������
	bool m_change_flag;
	// ����Ϣ�䶯������б�
	std::set< long long > m_dirty_mark;
	std::set< long long > m_old_state;

	// ������ҵ���Ϣ�б�
	// ԭ��Ϊԭ���������
	// ���ط�Ϊ�������ӷ����������

	FriendIntimacyMap m_friend_intimacy_map;
};

#endif	// __FRIEND_INTIMACY_HPP__