#ifndef __CHANNELMANAGER_HPP__
#define __CHANNALMANAGER_HPP__

#include <map>
#include <list>

#include "chat/msgboarditem.h"
#include "servercommon/chatserverdef.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
class Role;
class Channel;
class ChatUser;
class UserChatRecorder;
class ChatManager
{
public:
	static ChatManager & Instance();

	void OnUserLogin(Role *role);
	void OnUserLogout(Role *role);

	ChatUser * GetChatUser(int uid);
	void SetChatUser(Role *role);

	void OnChangeProf(Role *role, int old_base_prof, int new_base_prof);			//玩家转职,职业频道修改
	void OnRoleAvaChange(Role *role);
	
	void OnServerStart();
	void OnServerStop();
	void Update(time_t now_second);
	void InitChatGroup(int count, const ChatGroupInfo info_list[ONCE_SEND_CHAT_GROUP_COUNT]);
	void SaveChatGroup(long long last_save_id);
	void LoadData(long long from_id);
	void LoadDataSucc() { m_data_state = CHAT_GROUP_DATA_STATUS_LOAD_FINISH; }
	void ClearDirtyMark(long long last_save_id);
	void OnUserLoginByHide(long long group_id, int uid);//玩家游戏服上线
	void OnUserLogoutByHide(long long group_id, int uid);//玩家游戏服下线

	//-------------------------------跨服聊天群组数据同步与返回
	//			跨服群组载入
	void OnHidenServerBack(int count, const ChatGroupInfo info_list[crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST]);
	//玩家开始跨服前才同步玩家的本地群组，已同步的自动跳过
	bool OnRoleStarCrossSendLocalGroup(Role *role);

	void OnCrossGroupDismiss(long long group_id,int uid);

	void OnConnectCrossSucc();//已无用
	//bool CheckSendAllToCross();//已无用
	void OnDisconnectFromCrossServer();//已无用

	void OnCrossRegisterGameServer();//隐藏服连接

	void OnGameSynCross(const ChatGroupInfo* msg_info);//				游戏服创建群组同步到跨服-单个群组
	void OnCrossSynGame(const ChatGroupInfo* msg_info, int user_id = 0);//				玩家请求信息，从跨服返回的群组信息（暂存）

	void OnSpeakerMsg(Role *role, Protocol::SCChannelChat *msg);
	void OnChannelChat(Role *role, void* msg, int length);
	void OnSingleChat(Role *role, void* msg, int length, bool is_force = false);
	void OnSingleChat(crossgameprotocal::G2GRoleChatTransfer *protocal);
	
	void SystemSendRoleChannelChat(Role* role, int channel, const char* content, int length, bool is_team_info = true);

	void SendChatItemInfo(Role * role, long long item_info_seq);
	void SendChatItemInfo2(int role_id, long long item_info_seq, bool req_from_hidden);
	void ChatItemInfoSyn(long long key, ChatItemInfo * info);

	void AddSingleChat(int uid, const char *msg, int msg_len);
	void OnGuildChatt(Role *role, const char* buff, int total_len);
	void OnGuildChatt(UserID userid, const char* buff, int total_len);

	const ChatGroupInfo * GetGroupInfo(long long group_id);
	bool CheckRoleCreateGroup(Role *role, bool is_cross, int *invalid_index);

	void OnCreateGroup(Role *role, GameName group_name, bool is_cross);
	void OnDismissGroup(Role *role, long long group_id);
	void OnRoleInviteGroup(Role *role, int be_invite, long long group_id);
	void OnRoleEnterGroup(Role *role, long long group_id, int invite_uid);
	void OnKickMemberFromGroup(Role *role, int be_kick, long long group_id);
	void OnRoleLeaveGroup(Role *role, long long group_id, bool notice = true);
	void SendRoleGroupInfo(Role *role, int reason = Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_DEF);
	void SendRoleOneGroupInfo(Role* role, long long group_id, int reason = Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE);
	void OnChangeGroupNotice(Role *role, GameName group_name, GuildNotice group_notice, long long group_id);
	void OnGetGroupDetailInfo(Role *role, long long group_id);

	void OnRoleLeaveGroupInCross(long long group_id, int uid,GameName member_name);
	void OnRoleEnterGroupRet(crossgameprotocal::CrossGameSyncChatOpreta * msg);
	void OnGroupChangeSendGroupList(long long group_id);//下发略缩图给所有群员

	bool SendToGroup(long long group_id, void *msg, size_t length);//仅处理在线玩家，其余一律跳过，各服务器自行处理
	void OnRoleBeKickOutGroup(int uid,long long group_id);
	void OnGroupMemberChangeName(Role * role,GameName new_name);

	//
	void OnCrossEnterGroup(long long group_id, int member_id, int member_ava, int member_level, int member_prof, GameName member_name,int inviter_id, int invalid_index);//跨服加入群组

	void OnRoleChangeName(long long group_id,int member_id,GameName new_name);//操作传入--改名不同步直接操作
	void OnRoleChangeProf(long long group_id, int member_id, int new_prof,int new_ava);//操作传入--改名不同步直接操作
	void OnRoleChangeAva(long long group_id,int member_id,int new_ava);

	void OnRoleCreateCrossGroupRet(long long group_id,int uid,int invalid_index);

	void OnGroupNameOrNoticeChange(long long group_id, int uid, GameName group_name, GuildNotice group_notice);
	void OnGroupLeaderChangeSync(long long group_id, int new_leader_uid);

	void OnGameSyncDismissGroup(long long group_id,int user_id);//非本地服操作传入
	void OnGameSyncKickMemberFromGroup(long long group_id, int user_id,int be_kick_id);//非本地服操作传入
	void OnGameSyncKickMemberFromGroupRet(long long group_id, int user_id, int be_kick_id);//被踢返回，修改玩家common_data

	unsigned long long GetUniqueGroupId();
	void AddChatItemInfo(int *sign_id1, int *sign_id2, const ItemDataWrapper & item_info, const AttributeList * attrs = NULL);
	bool IsLoadFinish() { return m_data_state == CHAT_GROUP_DATA_STATUS_LOAD_FINISH; }

	void OnProfChat(Role * role, const char * buff, int total_len);
	void SendProfChannel(int prof, const char * buff, int total_len);
	int GetRoleBaseAva(Role* role);

	void SetChatRecrodList(Role *role, long long chat_list[CHAT_CORD_MAX_LIST]);
	void SendChatRecrodList(Role *role);

	void SendSimpleRecord(Role *role);
	void SetSimpleRecord(Role *role, int index, GameName data);

	void DelayQueryUserChatRecord(int uid);
	void QueryUserChatRecord(Role* role);
	void OnHiddenGameAddSingleChatRecord(crossgameprotocal::HiddenGameAddSingleChatRecord* msg);
	void OnHiddenGameQueryUserChatRecord(crossgameprotocal::HiddenGameQueryUserChatRecord* msg);
	void RecordUserSingleChat(int uid, int chat_with_uid, const char* msg, int length);
protected:
	void OnWorldChat(Role *role, const char* buff, int total_len);
	void OnSceneChat(Role *role, const char* buff, int total_len);
	void OnTeamChat(Role *role, const char* buff, int total_len);
	void OnGuildChat(Role *role, const char* buff, int total_len);
	void OnGuildChat(UserID userid, const char* buff, int total_len);
	void OnGroupChat(Role *role, long long group_id, const char* buff, int total_len);
	void OnComplexChat(Role * role, const char * buff, int total_len);
	void OnTemporaryGroupChat(Role* role, const char* buff, int total_len);

	void AddSingleChatBoard(int uid, const char *msg, int msg_len);
	void CheckSingleChatBoard(Role *role);

	void AddGuildChatBoard(int guild_id, const char *msg, int msg_len);
	void CheckGuildChatBoard(Role *role);

	void AddGroupChatBoard(long long group_id, const char *msg, int msg_len);
	void CheckGroupChatBoard(Role *role, long long group_id);
	void CheckRoleInGroup(Role *role,bool is_login);// 检查玩家是否在群组中
	bool RoleCanEnterGroupCount(Role *role, bool is_cross, int *invalid_index);
	void MarkDirty(long long group_id);

	long long AddChatItemInfo(const ChatItemInfo & item_data_wrapper);		//返回一个用于客户端查询详细物品信息的id

	ChatGroupInfo* MutableGroupInfo(long long group_id);
	void EraseGroup(long long group_id);

	UserChatRecorder* MutableUserChatRecorder(int uid);
	void CreateUserChatRecoder(int uid);

	ChatManager();
	~ChatManager();


	typedef std::map<int, ChatUser> ChatUserList; 
	typedef std::map<int, ChatUser>::iterator ChatUserListIterator; 

	ChatUserList m_chat_user_list;
	std::map<int, std::list<ChatUser> > m_prof_chat_list;		//职业频道

	typedef std::list<MsgBoardItem> MsgBoardItemQueue;
	typedef std::list<MsgBoardItem>::iterator MsgBoardItemQueueIterator;
	typedef std::map<long long, MsgBoardItemQueue> MsgBoardMap;
	typedef std::map<long long, MsgBoardItemQueue>::iterator MsgBoardMapIterator;

	MsgBoardMap m_single_chat_board_map;
	MsgBoardMap m_guild_chat_board_map;
	MsgBoardMap m_group_chat_board_map;

	unsigned int chat_item_last_checkout_tamp;
	unsigned short chat_item_info_curr_seq;
	std::map<long long, ChatItemInfo> m_chat_item_info;

	std::map<long long, bool> m_dirty_mark;		// 实际上找到就行，值无用
	std::map<long long, bool> m_old_state;
	std::map<long long, bool> m_send_cross;	// for both cross and local

	//跨服群组--本地找不到的先来这里找
	std::map<long long, ChatGroupInfo> m_cross_chat_group_info;

	unsigned short m_chat_group_seq;
	int m_data_state;
	unsigned int m_last_save_time;
	bool m_is_change;

	char*				m_buffer;		// 用于是组包

	typedef std::map<int, UserChatRecorder> UserChatRecorderMap;
	UserChatRecorderMap m_user_chat_recorder_map;
};

#endif

