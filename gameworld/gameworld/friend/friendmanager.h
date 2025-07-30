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
	FRIEND_DATA_STATUS_INVALID = 0,						// 初始状态
	FRIEND_DATA_STATUS_LOADING,							// 数据加载中 
	FRIEND_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	FRIEND_DATA_STATUS_MAX,
};

static const int FRIEND_SAVE_INTERVAL = 60;				// 半分钟回写一次

class Role;

class FriendManager
{
	friend class RMIInitFriendBackObjectImpl;
	friend class RMIInitBlackBackObjectImpl;
public:
	class UserFriendInfo;
	class UserBlackInfo;

	typedef std::set< long long> UserFrinedSet;														// < 好友关系llid >
	typedef std::set< long long>::iterator UserFrinedSetIt;
	typedef std::map<int, UserFriendInfo > UserFrinedInfoMap;										// < 玩家UID , 玩家的好友信息 >
	typedef std::map<int, UserFriendInfo >::iterator	UserFrinedInfoMapIt;

	typedef std::set< long long> UserBlackSet;														// < 黑名单关系llid >
	typedef std::set< long long>::iterator UserBlackSetIt;
	typedef std::map<int, UserBlackInfo > UserBlackInfoMap;											// < 玩家UID , 玩家对应的黑名单关系llid >
	typedef std::map<int, UserBlackInfo >::iterator	UserBlackInfoMapIt;

	typedef std::map<long long, FriendItem> FriendItemMap;											// < 好友关系LLID , 好友信息 >
	typedef std::map<long long, FriendItem>::iterator  FriendItemMapIt;

	typedef std::map<long long, BlackItem> BlackItemMap;											// < 黑名单关系LLID , 黑名单信息 >
	typedef std::map<long long, BlackItem>::iterator  BlackItemMapIt;

	class UserFriendInfo
	{
	public:
		UserFriendInfo() {}

		UserFrinedSet user_friend_set;						// 好友LLID 列表

		std::map<int , Protocol::AskAddInfo > ask_map;		// 申请加我为好友的玩家列表
	};


	class UserBlackInfo
	{
	public:
		UserBlackInfo() {}

		UserBlackSet user_black_set;						// 黑名单 LLID 列表  玩家添加别人的
		std::set<int > user_be_black_set;					// 黑名单 UID 列表  玩家被人添加黑名单的 , 主要用于玩家上下线及所在状态变更
	};

public:
	/*
	注: 主服不在做任何逻辑相关操作 , 所有逻辑相关都传递到隐藏服去做处理
		主服只做数据读取保存和接受来至跨服数据的同步
	函数命名规则 :
	xxxxByGameWorld :	表示由主服调用 , 非处理来至隐藏服信息的 , 可能会向隐藏服发送信息
	xxxxByHidden :		表示由隐藏服调用 , 非处理来至隐藏服信息的 , 可能会向主服发送信息
	xxxxFromGameWorld : 表示由隐藏服调用 , 处理来至主服信息的
	xxxxFromHidden :	表示由主服调用 , 处理来至隐藏服信息的
	xxxxToGameWorld :	表现隐藏服调用 ,会发送信息到主服
	xxxxToHidden :		表现主服服调用 ,会发送信息到隐藏服

	未注明的则表示 主服和隐藏服都有可能调用 ,这种需要看具体逻辑
	*/

	/// [>[publish]系统基础相关<](↓↓↓) ////////////////////////////////////////////////////////////////
	static FriendManager & Instance();
	void InitFriendParam(int _ret,  FriendListParam & _friends_list_param);
	void InitBlackParam(int _ret, BlacklistsParam & _blacks_list_param);
	void OnConnectHiddenSucc();
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	/// [>[publish]系统基础相关<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>[publish]处理来至主服的信息<](↓↓↓) //////////////////////////////////////////////////////////
	// 处理来至主服的信息 , 获取玩家的好友相关的信息
	void OnGetFriendInfoFromGameWorld(crossgameprotocal::GameHiddenGetFriendInfo * _get_info);
	// 处理来至主服的信息 , 玩家添加好友申请操作
	void OnAddFriendReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReq * _req);
	// 处理来至主服的信息 , 好友申请应答操作
	void OnAddFriendReplyReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReplyReq * _req);
	// 处理来至主服的信息 , 删除好友操作
	void OnRemoveFriendReqFromGameWorld(crossgameprotocal::GameHiddenRemoveFriendReq * _req);
	// 处理来至主服的信息 , 添加黑名单操作 
	void OnAddBlackReqFromGameWorld(crossgameprotocal::GameHiddenAddBlackReq * _req);
	// 处理来至主服的信息 , 删除黑名单操作  
	void OnRemoveBlackReqFromGameWorld(crossgameprotocal::GameHiddenRemoveBlackReq * _req);
	// 处理来至主服的信息 , 好友组操作 
	void OnFriendGroupReqFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	// 处理来至主服的信息 , 近聊相关操作 
	void OnRecentlyReqFromGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req);
	// 处理来至主服的信息 , 玩家上下线信息处理 
	void OnRoleLoginLogoutFriendNotifyFromGameWorld(crossgameprotocal::GameHiddenRoleLoginLogoutFriendNotify * _notify);

	/// [>[publish]处理来至主服的信息<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>[publish]处理来至隐藏服的信息<](↓↓↓) //////////////////////////////////////////////////////////
	// 处理来至隐藏服的  好友申请列表信息变更
	void OnSyncFrinedAskListChangeFromHidden(crossgameprotocal::HiddenGameFriendAskListChange * _change);
	// 处理来至隐藏服的  玩家好友列表信息
	void OnSyncFriendListToRoleFromHidden(crossgameprotocal::HiddenGameFriendListToRole * _friend_list);
	// 处理来至隐藏服的  玩家黑名单列表信息
	void OnGetBlackListFromHidden(crossgameprotocal::HiddenGameBlackList * _list);
	// 处理来至隐藏服的  玩家黑名单列表变更信息
	void OnGetBlackListChangeFromHidden(crossgameprotocal::HiddenGameBlackListChange * _change);
	// 处理来至隐藏服的  玩家好友申请列表
	void OnAskListToRoleFromHidden(crossgameprotocal::HiddenGameAskListToRole * _ask_list);
	// 处理来至隐藏服的  玩家近聊信息
	void OnRecentlyInfoFromHidden(crossgameprotocal::HiddenGameRecentlyInfo * _info);
	// 处理来至隐藏服的  好友信息变更
	void OnFriendChangeToRoleFromHidden(crossgameprotocal::HiddenGameFriendChangeToRole * _msg);
	/// [>[publish]处理来至隐藏服的信息<](↑↑↑) ////////////////////////////////////////////////////////////////

	void OnLogin(Role * _role);
	void OnLogout(Role * _role);

	// 客户端操作请求
	void OnClientReq(Role * _role, Protocol::CSFriendReq * _req);
	// 添加近聊
	void AddRecentlyChat(Role * _role, int _target_id);
	// 添加最近组队
	void AddRecentlyTeam(Role * _role, int _target_id);
	// 添加好友请求
	void AddFriendReq(Role * _role, int _target_id, bool _is_yi_jian);
	// 获取玩家的好友关系列表
	FriendManager::UserFrinedSet * GetUserFriendSetInfo(Role * _role);
	// 发送邮件给好友
	void SystemMailToAllFriend(int _role_uid, MailSubject subject, MailContentTxt contenttxt);

	// 是否是黑名单
	bool IsBlack(int _role_id, int _black_id);
	// 是否是好友
	bool IsFriend(int _role_id, int _friend_id);

	// 编辑组
	void EditGroup(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id);
	// 组改名
	void RenameGroup(Role * _role, int _group_type, GameName _group_name);
	// 移动组
	void MoveGroup(Role * _role, int _group_type, int _count, int * _role_id);


	// 处理同步来的好友信息  消息可能来源于主服 也可能来源于 隐藏服
	void OnSyncFriendInfo(crossgameprotocal::SyncFriendInfo * _friend_info);
	// 处理同步来的黑名单信息  消息可能来源于主服 也可能来源于 隐藏服
	void OnSyncBlackInfo(crossgameprotocal::SyncBlackInfo * _black_info);
	// 处理同步来的添加好友离线事件  消息可能来源于主服 也可能来源于 隐藏服
	void OnSyncAddFriendOfflienEvent(crossgameprotocal::SyncAddFriendOfflineEvent * _event);

	// 添加好友离线事件 , 用于处理玩家添加好友时需要计算好友数量的成就的事件
	void OnAddFriendOfflineEventByLogin(Role * _role);

	// 获取好友数量
	int GetFriendNumber(int _role_id);
	// 获取好友数量 , 不小于目标亲密度的
	int GetFriendNumber(int _role_id ,int intimacy_num);
private:
	FriendManager();
	~FriendManager();

	/// [>[private]系统基础相关<](↓↓↓) ////////////////////////////////////////////////////////////////
	void Save();
	void SaveFriendParam();
	void SaveBlackParam();
	void ClearFriendDirtyMark();
	void ClearBlackDirtyMark();
	void SetFriendDirtyMark(long long _friend_for_index);
	void SetBlackDirtyMark(long long _black_for_index);
	void CheckLoadFinish();
	/// [>[private]系统基础相关<](↑↑↑) ////////////////////////////////////////////////////////////////

	// 同步所有数据到隐藏服
	void SyncAllFriendDataToHidden();
	void SyncAllBlackDataToHidden();

	// 同步好友数据  隐藏服 -> 原服  
	void SyncFriendDataToGameWorld(long long friend_for_index, int _sync_status);
	void SyncBlackDataToGameWorld(long long black_for_index, int _sync_status);

	// 在隐藏服处理来源于主服的同步信息
	void InitFriendInfoFromGameWorld(crossgameprotocal::SyncFriendInfo * _friend_info);
	void UpdataFriendInfoFromHidden(crossgameprotocal::SyncFriendInfo * _friend_info);
	void InitBlackInfoFromGameWorld(crossgameprotocal::SyncBlackInfo * _black_info);
	void UpdataBlackInfoFromHidden(crossgameprotocal::SyncBlackInfo * _black_info);


	/// [>[private]获取好友列表<](↓↓↓) ////////////////////////////////////////////////////////////////
	void SendFriendListToRole(Role * _role);
	void SendFriendListToRoleByGameWorld(Role * _role);
	void SendFriendListToRoleByHidden(Role * _role);
	void GetFriendInfoToHidden(Role * _role, int _get_info_type);
	// 发送好友列表到原服
	void SendFriendListToRoleToGameWorld(const UniqueServerID _usid, int _role_id);
	/// [>[private]获取好友列表<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>[private]获取玩家黑名单列表<](↓↓↓) ////////////////////////////////////////////////////////////////
	void SendBlackListToRole(Role * _role);
	void SendBlackListToRoleByGameWorld(Role * _role);
	void SendBlackListToRoleByHidden(Role * _role);
	void SendBlackListToRoleToGameWorld(const UniqueServerID _usid, int _role_id);
	/// [>[private]获取玩家黑名单列表<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>[private]添加好友请求<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 能否添加好友
	bool CanAddFriend(int _role_id, int _target_id, int * _role_error_num, int * _target_error_num);
	// 发送添加好友信息
	void SendAddFriendRouteByHidden(Role * _role, Role * _target_role);
	void SendAddFriendRouteByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, Role * _target_role);
	void SendAddFriendRouteByGameWorld(crossgameprotocal::HiddenGameFriendAskListChange * _change, Role * _target_role);
	void SendAddFriendRoute(Role * _role, Protocol::AskAddInfo & _ask_add_info);
	// 添加好友请求
	void AddFriendReqByGameWorld(Role * _role, int _target_id, bool _is_yi_jian);
	void AddFriendReqByHidden(Role * _role, int _target_id, bool _is_yi_jian);
	// 添加玩家到目标的好友申请待审核列表中
	void AddTargetAskListByHidden(int _target_id, Role * _role,  bool _is_yijian, bool _is_send_friend_route);
	void AddTargetAskListByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_route);
	void AddTargetAskListByGameWorld(int _target_id, crossgameprotocal::HiddenGameFriendAskListChange * _change);
	// 同步好友申请待审核列表信息变更
	void AskListChangeByInsertByHidden(int _target_id, Role * _role, bool _is_yijian, bool _is_send_friend_route);
	void AskListChangeByInsertByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_rout);
	void AskListChangeByDeleteByHidden(int _target_id, int _role_id);
	// 同步好友申请结果
	void AddFriendReqRetToGameWorld(int _role_id, int _ret);
	/// [>[private]添加好友请求<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>[private]添加好友应答<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 申请添加好友应答
	void AddFriendReply(Role * _role, int _apply_id, bool _is_accept);
	void AddFriendReplyByGameWorld(Role * _role, int _apply_id, bool _is_accept);
	void AddFriendReplyByHidden(int _role_id , int _apply_id, bool _is_accept);
	// 添加好友
	void AddFriendByHidden(int _role_id, int _apply_id);
	// 添加好友
	void AddFriendByUserCacheNodeByHidden(int _role_id, int _apply_id);
	// 从目标的好友申请待审核列表中移除玩家
	void RemoveTargetAskList(int _target_id, int _role_id, bool _is_sync);
	// 同步好友添加时玩家处于离线状态 , 则上线时需要处理一下成就等相关的操作
	// 因为offlinemanager是独立存在于各个服务器数据之间不互通
	// 而玩家再次登录时可能是在原服也可能是在隐藏服 , 所以需要主动去同步这个离线信息
	void SyncInfoAddFriendOfflineEvent(int _role_id, int _req);
	/// [>[private]添加好友应答<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// 更新好友信息
	void UpdateFriendInfo(int _friend_id, FriendItem & _friend_item);

	/// [>删除好友<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 删除好友
	void RemoveFriend(Role * _role, int _target_id);
	void RemoveFriendByGameWorld(Role * _role, int _target_id);
	void RemoveFriendByHidden(int _role_id, int _target_id, const char * _reason);
	bool RemoveFriendHelpByHidden(int _role_id, int _target_id, const  char * _reason);
	/// [>删除好友<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>添加黑名单<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 添加黑名单
	void AddBlack(Role * _role, int _target_id);
	void AddBlackByGameWorld(Role * _role, int _target_id);
	bool AddBlackByHidden(int _role_id, int _target_id , int * _error_num);
	void AddBlackHelpByHidden(int _role_id, int _target_id);
	// 发送黑名单变更信息
	void SendBlackChangeToRoleByHidden(Role * _role, int _black_id, int _change_type , int _online_status = -1);
	void SendBlackChangeToRoleFromHidden(Role * _role ,crossgameprotocal::HiddenGameBlackListChange * _change);
	void SendBlackChangeToRoleToGameWorld( int _role_id, int _black_id, int _change_type , int _online_status = -1);
	/// [>添加黑名单<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>删除黑名单<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 从黑名单移除
	void RemoveBlack(Role * _role, int _target_id);
	void RemoveBlackByGameWorld(Role * _role, int _target_id);
	void RemoveBlackByHidden(int _role_id, int _target_id);
	/// [>删除黑名单<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>删除分组<](↓↓↓) ////////////////////////////////////////////////////////////////
	void DelGroup(Role * _role, int _group_type);
	void DelGroupByHidden( int _role_id , int _group_type);
	void DelGroupByGameWorld(Role * _role, int _group_type);
	void SendGroupInfo(Role * _role);
	/// [>删除分组<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>发送好友申请列表<](↓↓↓) ////////////////////////////////////////////////////////////////
	void SendAddList(Role * _role);
	void SendAddListByHidden(Role * _role);
	void SendAddListByGameWorld(Role * _role);
	void SendAddListToRoleToGameWorld(UniqueServerID _usid , int _role_id);
	void SendAskListToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameAskListToRole * _ask_list);
	/// [>发送好友申请列表<](↑↑↑) ////////////////////////////////////////////////////////////////

	// 发送最近信息到主服
	void OnSendRecentlyInfoToGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req, int _info_type);

	/// [>添加近聊<](↓↓↓) ////////////////////////////////////////////////////////////////
	void AddRecentlyChatByHidden(Role * _role, int _target_id);
	void AddRecentlyChatByGameWorld(Role * _role, int _target_id);
	void SendRecentlyChatChangeByHidden(Role * _role, int _del_role_id, int _target_id);
	void SendRecentlyChatChangeFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void AddRecentlyChatFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>添加近聊<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>获取近聊列表<](↓↓↓) ////////////////////////////////////////////////////////////////
	void SendRecentlyChatInfo(Role * _role);
	void SendRecentlyChatInfoByHidden(Role * _role);
	void SendRecentlyChatInfoByGameWorld(Role * _role);
	void SendRecentlyChatInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>获取近聊列表<](↑↑↑) ////////////////////////////////////////////////////////////////


	/// [>获取最近组队的信息<](↓↓↓) ////////////////////////////////////////////////////////////////
	void AddRecentlyTeamByHidden(Role * _role, int _target_id);
	void AddRecentlyTeamByGameWorld(Role * _role, int _target_id);
	void AddRecentlyTeamFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void SendRecentlyTeamChangeInfoByHidden(Role * _role, int _del_role_id, int _target_id);
	void SendRecentlyTeamChangeInfoFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	void SendRecentlyTeamInfo(Role * _role);
	void SendRecentlyTeamInfoByHidden(Role * _role);
	void SendRecentlyTeamInfoByGameWorld(Role * _role);
	void SendRecentlyTeamInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info);
	/// [>获取最近组队的信息<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>查询目标玩家是否在线<](↓↓↓) ////////////////////////////////////////////////////////////////
	void SendRoleOnlineStatus(Role * _role, int _target_id);
	/// [>查询目标玩家是否在线<](↑↑↑) ////////////////////////////////////////////////////////////////

	/// [>编辑组操作<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 编辑组
	void EditGroupByHidden(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id);
	void EditGroupByGameWorld(Role * _role, int group_type, GameName group_name, int count, int * role_id);
	void EditGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	/// [>编辑组操作<](↑↑↑) ////////////////////////////////////////////////////////////////
	
	/// [>编辑组操作<](↓↓↓) ////////////////////////////////////////////////////////////////
	// 移动组
	void MoveGroupByHidden(Role * _role, int _group_type, int _count, int * _role_id);
	void MoveGroupByGameWorld(Role * _role, int _group_type, int _count, int * _role_id);
	void MoveGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req);
	/// [>编辑组操作<](↑↑↑) ////////////////////////////////////////////////////////////////


	// 好友
	long long  AddFriendInfo(int _role_id, int _friend_id , const FriendItem & _friend_item);
	long long  RemoveFriendInfo(int _role_id, int _friend_id);
	void AddUserFriendRelation(int _role_id, long long _friend_for_index);
	void AddUserFriendRelation(int _role_id, int _friend_id);
	void RemoveUserFriendRelation(int _role_id, int _friend_id);
	void RemoveUserFriendRelation(int _role_id, long long _friend_for_index);
	bool IsFriendFull(int _role_id);// 好友位是否已满


	// 黑名单
	long long  AddBlackInfo(int  _role_id, int _black_id, const BlackItem & _black_item);
	long long  RemoveBlackInfo(int _role_id, int _black_id);
	void AddUserBlackRelation(int _role_id, long long _black_for_index);
	void AddUserBlackRelation(int _role_id, int _black_id);
	void RemoveUserBlackdRelation(int _role_id, int _black_id);
	void RemoveUserBlackRelation(int _role_id, long long _black_for_index);

	// 登录
	// 通知好友上线了
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

	// 处理字符串用的函数
	void CheckOnlienStatusAndStrCut(char _online_status ,GameName & _str ,int _role_id , int _target_id );
	void StrCut(GameName & _str);
	char GetOnlineStatus(int _role_id , int _target_uid , bool is_to_cross_target, char _online_status = -1 );


	int m_data_state;
	bool m_friends_load_flag;
	bool m_blacks_load_flag;

	// 好友系统是否有数据发生过变更
	bool m_friends_change_flag;
	// 黑名单是否有数据发生过变更
	bool m_blacks_change_flag;

	// 有信息变动的玩家列表
	std::set< long long > m_friend_dirty_mark;	
	std::set< long long > m_friend_old_state;
	std::set< long long > m_black_dirty_mark;
	std::set< long long > m_black_old_state;


	// 所有玩家的好友信息列表
	// 原服为原服所有玩家
	// 隐藏服为所有连接服的所有玩家
	UserFrinedInfoMap m_user_friend_info_map;
	UserBlackInfoMap m_user_black_info_map;

	FriendItemMap m_friend_item_map;
	BlackItemMap m_black_item_map;
};

#endif