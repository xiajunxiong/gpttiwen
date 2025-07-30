#ifndef FRIENDMANAGER_H
#define FRIENDMANAGER_H

#include <map>
#include <set>
#include "gamedef.h"
#include "servercommon/struct/friendparam.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/maildef.hpp"
#include "protocol/msgfriend.h"

enum FRIEND_DATA_STATUS
{
	FRIEND_DATA_STATUS_INVALID = 0,						// ��ʼ״̬
	FRIEND_DATA_STATUS_LOADING,							// ���ݼ����� 
	FRIEND_DATA_STATUS_LOAD_FINISH,						// ���ݼ������ 
	FRIEND_DATA_STATUS_MAX,
};

static const int FRIEND_SAVE_INTERVAL = 60;				// ����ӻ�дһ��

class Role;

class FriendManager
{
	friend class RMIInitFriendBackObjectImpl;
	friend class RMIInitBlackBackObjectImpl;
public:
	class UserFriendInfo;
	class UserBlackInfo;

	typedef std::set< long long> UserFrinedSet;														// < ���ѹ�ϵllid >
	typedef std::set< long long>::iterator UserFrinedSetIt;
	typedef std::map<int, UserFriendInfo > UserFrinedInfoMap;										// < ���UID , ��ҵĺ�����Ϣ >
	typedef std::map<int, UserFriendInfo >::iterator	UserFrinedInfoMapIt;

	typedef std::set< long long> UserBlackSet;														// < ��������ϵllid >
	typedef std::set< long long>::iterator UserBlackSetIt;
	typedef std::map<int, UserBlackInfo > UserBlackInfoMap;											// < ���UID , ��Ҷ�Ӧ�ĺ�������ϵllid >
	typedef std::map<int, UserBlackInfo >::iterator	UserBlackInfoMapIt;

	typedef std::map<long long, FriendItem> FriendItemMap;											// < ���ѹ�ϵLLID , ������Ϣ >
	typedef std::map<long long, FriendItem>::iterator  FriendItemMapIt;

	typedef std::map<long long, BlackItem> BlackItemMap;											// < ��������ϵLLID , ��������Ϣ >
	typedef std::map<long long, BlackItem>::iterator  BlackItemMapIt;

	class UserFriendInfo
	{
	public:
		UserFriendInfo() {}

		UserFrinedSet user_friend_set;						// ����LLID �б�

		std::map<int , Protocol::AskAddInfo > ask_map;		// �������Ϊ���ѵ�����б�
	};


	class UserBlackInfo
	{
	public:
		UserBlackInfo() {}

		UserBlackSet user_black_set;						// ������ LLID �б�  �����ӱ��˵�
		std::set<int > user_be_black_set;					// ������ UID �б�  ��ұ�����Ӻ������� , ��Ҫ������������߼�����״̬���
	};

public:
	/*
	ע: �����������κ��߼���ز��� , �����߼���ض����ݵ����ط�ȥ������
		����ֻ�����ݶ�ȡ����ͽ�������������ݵ�ͬ��
	������������ :
	xxxxByGameWorld :	��ʾ���������� , �Ǵ����������ط���Ϣ�� , ���ܻ������ط�������Ϣ
	xxxxByHidden :		��ʾ�����ط����� , �Ǵ����������ط���Ϣ�� , ���ܻ�������������Ϣ
	xxxxFromGameWorld : ��ʾ�����ط����� , ��������������Ϣ��
	xxxxFromHidden :	��ʾ���������� , �����������ط���Ϣ��
	xxxxToGameWorld :	�������ط����� ,�ᷢ����Ϣ������
	xxxxToHidden :		�������������� ,�ᷢ����Ϣ�����ط�

	δע�������ʾ ���������ط����п��ܵ��� ,������Ҫ�������߼�
	*/

	/// [>[publish]ϵͳ�������<](������) ////////////////////////////////////////////////////////////////
	static FriendManager & Instance();
	void InitFriendParam(int _ret,  FriendListParam & _friends_list_param);
	void InitBlackParam(int _ret, BlacklistsParam & _blacks_list_param);
	void OnConnectHiddenSucc();
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	/// [>[publish]ϵͳ�������<](������) ////////////////////////////////////////////////////////////////


	/// [>[publish]����������������Ϣ<](������) //////////////////////////////////////////////////////////
	// ����������������Ϣ , ��ȡ��ҵĺ�����ص���Ϣ
	void OnGetFriendInfoFromGameWorld(crossgameprotocal::GameHiddenGetFriendInfo * _get_info);
	// ����������������Ϣ , �����Ӻ����������
	void OnAddFriendReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReq * _req);
	// ����������������Ϣ , ��������Ӧ�����
	void OnAddFriendReplyReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReplyReq * _req);
	// ����������������Ϣ , ɾ�����Ѳ���
	void OnRemoveFriendReqFromGameWorld(crossgameprotocal::GameHiddenRemoveFriendReq * _req);
	// ����������������Ϣ , ��Ӻ��������� 
	void OnAddBlackReqFromGameWorld(crossgameprotocal::GameHiddenAddBlackReq * _req);
	// ����������������Ϣ , ɾ������������  
	void OnRemoveBlackReqFromGameWorld(crossgameprotocal::GameHiddenRemoveBlackReq * _req);
	// ����������������Ϣ , ��������� 
	void OnFriendGroupReqFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	// ����������������Ϣ , ������ز��� 
	void OnRecentlyReqFromGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req);
	// ����������������Ϣ , �����������Ϣ���� 
	void OnRoleLoginLogoutFriendNotifyFromGameWorld(crossgameprotocal::GameHiddenRoleLoginLogoutFriendNotify * _notify);

	/// [>[publish]����������������Ϣ<](������) ////////////////////////////////////////////////////////////////


	/// [>[publish]�����������ط�����Ϣ<](������) //////////////////////////////////////////////////////////
	// �����������ط���  ���������б���Ϣ���
	void OnSyncFrinedAskListChangeFromHidden(crossgameprotocal::HiddenGameFriendAskListChange * _change);
	// �����������ط���  ��Һ����б���Ϣ
	void OnSyncFriendListToRoleFromHidden(crossgameprotocal::HiddenGameFriendListToRole * _friend_list);
	// �����������ط���  ��Һ������б���Ϣ
	void OnGetBlackListFromHidden(crossgameprotocal::HiddenGameBlackList * _list);
	// �����������ط���  ��Һ������б�����Ϣ
	void OnGetBlackListChangeFromHidden(crossgameprotocal::HiddenGameBlackListChange * _change);
	// �����������ط���  ��Һ��������б�
	void OnAskListToRoleFromHidden(crossgameprotocal::HiddenGameAskListToRole * _ask_list);
	// �����������ط���  ��ҽ�����Ϣ
	void OnRecentlyInfoFromHidden(crossgameprotocal::HiddenGameRecentlyInfo * _info);
	// �����������ط���  ������Ϣ���
	void OnFriendChangeToRoleFromHidden(crossgameprotocal::HiddenGameFriendChangeToRole * _msg);
	/// [>[publish]�����������ط�����Ϣ<](������) ////////////////////////////////////////////////////////////////

	void OnLogin(Role * _role);
	void OnLogout(Role * _role);

	// �ͻ��˲�������
	void OnClientReq(Role * _role, Protocol::CSFriendReq * _req);
	// ��ӽ���
	void AddRecentlyChat(Role * _role, int _target_id);
	// ���������
	void AddRecentlyTeam(Role * _role, int _target_id);
	// ��Ӻ�������
	void AddFriendReq(Role * _role, int _target_id, bool _is_yi_jian);
	// ��ȡ��ҵĺ��ѹ�ϵ�б�
	FriendManager::UserFrinedSet * GetUserFriendSetInfo(Role * _role);
	// �����ʼ�������
	void SystemMailToAllFriend(int _role_uid, MailSubject subject, MailContentTxt contenttxt);

	// �Ƿ��Ǻ�����
	bool IsBlack(int _role_id, int _black_id);
	// �Ƿ��Ǻ���
	bool IsFriend(int _role_id, int _friend_id);

	// �༭��
	void EditGroup(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id);
	// �����
	void RenameGroup(Role * _role, int _group_type, GameName _group_name);
	// �ƶ���
	void MoveGroup(Role * _role, int _group_type, int _count, int * _role_id);


	// ����ͬ�����ĺ�����Ϣ  ��Ϣ������Դ������ Ҳ������Դ�� ���ط�
	void OnSyncFriendInfo(crossgameprotocal::SyncFriendInfo * _friend_info);
	// ����ͬ�����ĺ�������Ϣ  ��Ϣ������Դ������ Ҳ������Դ�� ���ط�
	void OnSyncBlackInfo(crossgameprotocal::SyncBlackInfo * _black_info);
	// ����ͬ��������Ӻ��������¼�  ��Ϣ������Դ������ Ҳ������Դ�� ���ط�
	void OnSyncAddFriendOfflienEvent(crossgameprotocal::SyncAddFriendOfflineEvent * _event);

	// ��Ӻ��������¼� , ���ڴ��������Ӻ���ʱ��Ҫ������������ĳɾ͵��¼�
	void OnAddFriendOfflineEventByLogin(Role * _role);

	// ��ȡ��������
	int GetFriendNumber(int _role_id);
	// ��ȡ�������� , ��С��Ŀ�����ܶȵ�
	int GetFriendNumber(int _role_id ,int intimacy_num);
private:
	FriendManager();
	~FriendManager();

	/// [>[private]ϵͳ�������<](������) ////////////////////////////////////////////////////////////////
	void Save();
	void SaveFriendParam();
	void SaveBlackParam();
	void ClearFriendDirtyMark();
	void ClearBlackDirtyMark();
	void SetFriendDirtyMark(long long _friend_for_index);
	void SetBlackDirtyMark(long long _black_for_index);
	void CheckLoadFinish();
	/// [>[private]ϵͳ�������<](������) ////////////////////////////////////////////////////////////////

	// ͬ���������ݵ����ط�
	void SyncAllFriendDataToHidden();
	void SyncAllBlackDataToHidden();

	// ͬ����������  ���ط� -> ԭ��  
	void SyncFriendDataToGameWorld(long long friend_for_index, int _sync_status);
	void SyncBlackDataToGameWorld(long long black_for_index, int _sync_status);

	// �����ط�������Դ��������ͬ����Ϣ
	void InitFriendInfoFromGameWorld(crossgameprotocal::SyncFriendInfo * _friend_info);
	void UpdataFriendInfoFromHidden(crossgameprotocal::SyncFriendInfo * _friend_info);
	void InitBlackInfoFromGameWorld(crossgameprotocal::SyncBlackInfo * _black_info);
	void UpdataBlackInfoFromHidden(crossgameprotocal::SyncBlackInfo * _black_info);


	/// [>[private]��ȡ�����б�<](������) ////////////////////////////////////////////////////////////////
	void SendFriendListToRole(Role * _role);
	void SendFriendListToRoleByGameWorld(Role * _role);
	void SendFriendListToRoleByHidden(Role * _role);
	void GetFriendInfoToHidden(Role * _role, int _get_info_type);
	// ���ͺ����б�ԭ��
	void SendFriendListToRoleToGameWorld(const UniqueServerID _usid, int _role_id);
	/// [>[private]��ȡ�����б�<](������) ////////////////////////////////////////////////////////////////


	/// [>[private]��ȡ��Һ������б�<](������) ////////////////////////////////////////////////////////////////
	void SendBlackListToRole(Role * _role);
	void SendBlackListToRoleByGameWorld(Role * _role);
	void SendBlackListToRoleByHidden(Role * _role);
	void SendBlackListToRoleToGameWorld(const UniqueServerID _usid, int _role_id);
	/// [>[private]��ȡ��Һ������б�<](������) ////////////////////////////////////////////////////////////////


	/// [>[private]��Ӻ�������<](������) ////////////////////////////////////////////////////////////////
	// �ܷ���Ӻ���
	bool CanAddFriend(int _role_id, int _target_id, int * _role_error_num, int * _target_error_num);
	// ������Ӻ�����Ϣ
	void SendAddFriendRouteByHidden(Role * _role, Role * _target_role);
	void SendAddFriendRouteByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, Role * _target_role);
	void SendAddFriendRouteByGameWorld(crossgameprotocal::HiddenGameFriendAskListChange * _change, Role * _target_role);
	void SendAddFriendRoute(Role * _role, Protocol::AskAddInfo & _ask_add_info);
	// ��Ӻ�������
	void AddFriendReqByGameWorld(Role * _role, int _target_id, bool _is_yi_jian);
	void AddFriendReqByHidden(Role * _role, int _target_id, bool _is_yi_jian);
	// �����ҵ�Ŀ��ĺ������������б���
	void AddTargetAskListByHidden(int _target_id, Role * _role,  bool _is_yijian, bool _is_send_friend_route);
	void AddTargetAskListByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_route);
	void AddTargetAskListByGameWorld(int _target_id, crossgameprotocal::HiddenGameFriendAskListChange * _change);
	// ͬ���������������б���Ϣ���
	void AskListChangeByInsertByHidden(int _target_id, Role * _role, bool _is_yijian, bool _is_send_friend_route);
	void AskListChangeByInsertByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_rout);
	void AskListChangeByDeleteByHidden(int _target_id, int _role_id);
	// ͬ������������
	void AddFriendReqRetToGameWorld(int _role_id, int _ret);
	/// [>[private]��Ӻ�������<](������) ////////////////////////////////////////////////////////////////


	/// [>[private]��Ӻ���Ӧ��<](������) ////////////////////////////////////////////////////////////////
	// ������Ӻ���Ӧ��
	void AddFriendReply(Role * _role, int _apply_id, bool _is_accept);
	void AddFriendReplyByGameWorld(Role * _role, int _apply_id, bool _is_accept);
	void AddFriendReplyByHidden(int _role_id , int _apply_id, bool _is_accept);
	// ��Ӻ���
	void AddFriendByHidden(int _role_id, int _apply_id);
	// ��Ӻ���
	void AddFriendByUserCacheNodeByHidden(int _role_id, int _apply_id);
	// ��Ŀ��ĺ������������б����Ƴ����
	void RemoveTargetAskList(int _target_id, int _role_id, bool _is_sync);
	// ͬ���������ʱ��Ҵ�������״̬ , ������ʱ��Ҫ����һ�³ɾ͵���صĲ���
	// ��Ϊofflinemanager�Ƕ��������ڸ�������������֮�䲻��ͨ
	// ������ٴε�¼ʱ��������ԭ��Ҳ�����������ط� , ������Ҫ����ȥͬ�����������Ϣ
	void SyncInfoAddFriendOfflineEvent(int _role_id, int _req);
	/// [>[private]��Ӻ���Ӧ��<](������) ////////////////////////////////////////////////////////////////

	/// ���º�����Ϣ
	void UpdateFriendInfo(int _friend_id, FriendItem & _friend_item);

	/// [>ɾ������<](������) ////////////////////////////////////////////////////////////////
	// ɾ������
	void RemoveFriend(Role * _role, int _target_id);
	void RemoveFriendByGameWorld(Role * _role, int _target_id);
	void RemoveFriendByHidden(int _role_id, int _target_id, const char * _reason);
	bool RemoveFriendHelpByHidden(int _role_id, int _target_id, const  char * _reason);
	/// [>ɾ������<](������) ////////////////////////////////////////////////////////////////

	/// [>��Ӻ�����<](������) ////////////////////////////////////////////////////////////////
	// ��Ӻ�����
	void AddBlack(Role * _role, int _target_id);
	void AddBlackByGameWorld(Role * _role, int _target_id);
	bool AddBlackByHidden(int _role_id, int _target_id , int * _error_num);
	void AddBlackHelpByHidden(int _role_id, int _target_id);
	// ���ͺ����������Ϣ
	void SendBlackChangeToRoleByHidden(Role * _role, int _black_id, int _change_type , int _online_status = -1);
	void SendBlackChangeToRoleFromHidden(Role * _role ,crossgameprotocal::HiddenGameBlackListChange * _change);
	void SendBlackChangeToRoleToGameWorld( int _role_id, int _black_id, int _change_type , int _online_status = -1);
	/// [>��Ӻ�����<](������) ////////////////////////////////////////////////////////////////

	/// [>ɾ��������<](������) ////////////////////////////////////////////////////////////////
	// �Ӻ������Ƴ�
	void RemoveBlack(Role * _role, int _target_id);
	void RemoveBlackByGameWorld(Role * _role, int _target_id);
	void RemoveBlackByHidden(int _role_id, int _target_id);
	/// [>ɾ��������<](������) ////////////////////////////////////////////////////////////////

	/// [>ɾ������<](������) ////////////////////////////////////////////////////////////////
	void DelGroup(Role * _role, int _group_type);
	void DelGroupByHidden( int _role_id , int _group_type);
	void DelGroupByGameWorld(Role * _role, int _group_type);
	void SendGroupInfo(Role * _role);
	/// [>ɾ������<](������) ////////////////////////////////////////////////////////////////

	/// [>���ͺ��������б�<](������) ////////////////////////////////////////////////////////////////
	void SendAddList(Role * _role);
	void SendAddListByHidden(Role * _role);
	void SendAddListByGameWorld(Role * _role);
	void SendAddListToRoleToGameWorld(UniqueServerID _usid , int _role_id);
	void SendAskListToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameAskListToRole * _ask_list);
	/// [>���ͺ��������б�<](������) ////////////////////////////////////////////////////////////////

	// ���������Ϣ������
	void OnSendRecentlyInfoToGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req, int _info_type);

	/// [>��ӽ���<](������) ////////////////////////////////////////////////////////////////
	void AddRecentlyChatByHidden(Role * _role, int _target_id);
	void AddRecentlyChatByGameWorld(Role * _role, int _target_id);
	void SendRecentlyChatChangeByHidden(Role * _role, int _del_role_id, int _target_id);
	void SendRecentlyChatChangeFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void AddRecentlyChatFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>��ӽ���<](������) ////////////////////////////////////////////////////////////////


	/// [>��ȡ�����б�<](������) ////////////////////////////////////////////////////////////////
	void SendRecentlyChatInfo(Role * _role);
	void SendRecentlyChatInfoByHidden(Role * _role);
	void SendRecentlyChatInfoByGameWorld(Role * _role);
	void SendRecentlyChatInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>��ȡ�����б�<](������) ////////////////////////////////////////////////////////////////


	/// [>��ȡ�����ӵ���Ϣ<](������) ////////////////////////////////////////////////////////////////
	void AddRecentlyTeamByHidden(Role * _role, int _target_id);
	void AddRecentlyTeamByGameWorld(Role * _role, int _target_id);
	void AddRecentlyTeamFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void SendRecentlyTeamChangeInfoByHidden(Role * _role, int _del_role_id, int _target_id);
	void SendRecentlyTeamChangeInfoFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void SendRecentlyTeamInfo(Role * _role);
	void SendRecentlyTeamInfoByHidden(Role * _role);
	void SendRecentlyTeamInfoByGameWorld(Role * _role);
	void SendRecentlyTeamInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>��ȡ�����ӵ���Ϣ<](������) ////////////////////////////////////////////////////////////////

	/// [>��ѯĿ������Ƿ�����<](������) ////////////////////////////////////////////////////////////////
	void SendRoleOnlineStatus(Role * _role, int _target_id);
	/// [>��ѯĿ������Ƿ�����<](������) ////////////////////////////////////////////////////////////////

	/// [>�༭�����<](������) ////////////////////////////////////////////////////////////////
	// �༭��
	void EditGroupByHidden(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id);
	void EditGroupByGameWorld(Role * _role, int group_type, GameName group_name, int count, int * role_id);
	void EditGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	/// [>�༭�����<](������) ////////////////////////////////////////////////////////////////
	
	/// [>�༭�����<](������) ////////////////////////////////////////////////////////////////
	// �ƶ���
	void MoveGroupByHidden(Role * _role, int _group_type, int _count, int * _role_id);
	void MoveGroupByGameWorld(Role * _role, int _group_type, int _count, int * _role_id);
	void MoveGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	/// [>�༭�����<](������) ////////////////////////////////////////////////////////////////


	// ����
	long long  AddFriendInfo(int _role_id, int _friend_id , const FriendItem & _friend_item);
	long long  RemoveFriendInfo(int _role_id, int _friend_id);
	void AddUserFriendRelation(int _role_id, long long _friend_for_index);
	void AddUserFriendRelation(int _role_id, int _friend_id);
	void RemoveUserFriendRelation(int _role_id, int _friend_id);
	void RemoveUserFriendRelation(int _role_id, long long _friend_for_index);
	bool IsFriendFull(int _role_id);// ����λ�Ƿ�����


	// ������
	long long  AddBlackInfo(int  _role_id, int _black_id, const BlackItem & _black_item);
	long long  RemoveBlackInfo(int _role_id, int _black_id);
	void AddUserBlackRelation(int _role_id, long long _black_for_index);
	void AddUserBlackRelation(int _role_id, int _black_id);
	void RemoveUserBlackdRelation(int _role_id, int _black_id);
	void RemoveUserBlackRelation(int _role_id, long long _black_for_index);

	// ��¼
	// ֪ͨ����������
	void OnLoginNotify( int _role_id);
	void OnLoginNotifyFriendByHidden( int _role_id, char _online_status);
	void OnLogoutNotify( int _role_id);
	void OnLogoutNotifyFriendByHidden(int _role_id);
	void OnLoginNotifyBlackByHidden(int _role_id, char _online_status);
	void OnLogoutNotifyBlackByHidden(int _role_id);
	void OnFriendLoginOrLogoutByHidden(int _role_id, long long _friend_for_index, int _online_status);
	void UpdateFriendLastOnlineTimestampByHidden(int _role_id, int _friend_id);
	void SendFriendChangeToRoleByHidden(Role * _role, int _change_role_id, int _change_type, int _group_type = 0, int _online_status = -1);
	void SendFriendChangeToRoleToGameWorld(int _role_id, int  _change_role_id, int _change_type, int _group_type = 0, int _online_status = -1);
	void SendFriendChangeToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameFriendChangeToRole * _msg);
	void SendRoleLoginLogoutToHidden(int _role_id, char _online_status);

	// �����ַ����õĺ���
	void CheckOnlienStatusAndStrCut(char _online_status ,GameName & _str ,int _role_id , int _target_id );
	void StrCut(GameName & _str);
	char GetOnlineStatus(int _role_id , int _target_uid , bool is_to_cross_target, char _online_status = -1 );


	int m_data_state;
	bool m_friends_load_flag;
	bool m_blacks_load_flag;

	// ����ϵͳ�Ƿ������ݷ��������
	bool m_friends_change_flag;
	// �������Ƿ������ݷ��������
	bool m_blacks_change_flag;

	// ����Ϣ�䶯������б�
	std::set< long long > m_friend_dirty_mark;	
	std::set< long long > m_friend_old_state;
	std::set< long long > m_black_dirty_mark;
	std::set< long long > m_black_old_state;


	// ������ҵĺ�����Ϣ�б�
	// ԭ��Ϊԭ���������
	// ���ط�Ϊ�������ӷ����������
	UserFrinedInfoMap m_user_friend_info_map;
	UserBlackInfoMap m_user_black_info_map;

	FriendItemMap m_friend_item_map;
	BlackItemMap m_black_item_map;
};

#endif