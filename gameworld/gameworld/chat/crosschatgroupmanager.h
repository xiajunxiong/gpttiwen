#ifndef __CROSS_CHAT_GROUP_MANAGER_HPP__
#define __CROSS_CHAT_GROUP_MANAGER_HPP__

#include <map>
#include <list>

//#include "chat/msgboarditem.h"
#include "servercommon/chatserverdef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;
class Channel;
class ChatUser;
class ChatGroup;
class CrossChatGroupManager
{
public:
	typedef std::map<long long, ChatGroup*> ChatGroupMap;

public:
	static CrossChatGroupManager & Instance();

	void OnUserLogin(Role *role);
	void OnUserLogout(Role *role);

	void OnUserLoginByGame(long long group_id, int uid);//玩家游戏服上线
	void OnUserLogoutByGame(long long group_id, int uid);//玩家游戏服下线

	void OnServerStart();
	void OnServerStop();
	void Update(time_t now_second);
	void OnRoleAvaChange(Role *role);

	void InitChatGroup(int count, const ChatGroupInfo info_list[ONCE_SEND_CHAT_GROUP_COUNT]);
	void SaveChatGroup(long long last_save_id);
	void LoadData(long long from_id);
	void LoadDataSucc() { m_data_state = CHAT_GROUP_DATA_STATUS_LOAD_FINISH; }
	void ClearDirtyMark(long long last_save_id);

	//-------------------------------跨服聊天群组数据同步与返回
	//			游戏服登入-群组列表
	void OnGameServerInLine(int server_id, int count, const ChatGroupInfo info_list[crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST]);
	void OnGameServerInLineReturnCrossData(int server_num,int* server_id_list);//一个游戏服载入连带其子服群组下发重新置空

	//零碎化群组返回
	bool OnFragmentGameServerInLineReturnCrossData(int server_id, long long* check_send_group_list, int check_send_group_num);

	//bool CheckSendAllToGame();
	void ClearSendAllToGame(int server_id);

	void OnCreateGroup(Role *role, GameName group_name);
	void OnRoleInviteGroup(Role *role, int be_invite, long long group_id);

	int OnRoleLeaveGroupInLocal(long long group_id, int uid, GameName user_name);//玩家在本地服离开跨服群组
	int OnRoleLeaveLocalGroup(long long group_id, int uid, GameName user_name);//玩家在本地服离开本地服群组

	int OnRoleCreateCrossGroup(int uid, int user_ava, int user_level, int user_prof, GameName group_name,GameName user_name, int inv,long long * create_group_id);
	int OnTempGroupBecomeOfficial(int max_num, int member_num, ChatGroupUserInfo* member_list, GameName group_name, long long * create_group_id);
	void OnRoleDismiss(long long group_id, int uid, GameName member_name);
	int OnRoleDismissCrossGroup(int uid, long long group_id, GameName member_name);
	void OnRoleDismissLocalGroup(int uid,long long group_id);
	void OnRoleInviteGroupByLocal(crossgameprotocal::GameCrossSyncChatOpreta *msg);

	void OnRoleEnterGroupInLocal(crossgameprotocal::GameCrossSyncChatOpreta *msg);
	void OnRoleEnterGroupReturn(crossgameprotocal::GameCrossSyncChatOpreta *msg);

	const ChatGroupInfo * GetGroupInfo(long long group_id);

	bool SendToGroup(long long group_id, void *msg, size_t length);//仅处理在线玩家，其余一律跳过，各服务器自行处理
	void SendRoleGroupInfo(Role *role, int reason = Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_DEF);
	void SendRoleOneGroupInfo(Role* role, long long group_id, int reason = Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE);
	void OnChangeGroupNotice(Role *role,GameName group_name,GuildNotice group_notice, long long group_id);
	void OnDismissGroup(Role *role, long long group_id);
	void OnRoleLeaveGroup(Role *role, long long group_id, bool notice = true);
	void OnRoleEnterGroup(Role *role, long long group_id,int invite_uid);
	
	void OnKickMemberFromGroup(Role *role, int be_kick, long long group_id);
	void OnRoleBeKickOutGroup(int uid,long long group_id);
	void OnRoleBeKickOutGroupRet(int uid, long long group_id);
	void OnGetGroupDetailInfo(Role *role, long long group_id);
	void OnGroupChangeSendGroupList(long long group_id);//下发略缩图给所有群员

	void OnRoleChangeName(long long group_id,int member_id,GameName new_name);//操作传入--改名不同步直接操作
	void OnRoleChangeProf(long long group_id, int member_id, int new_prof,int new_ava);//操作传入--改名不同步直接操作
	void OnRoleChangeAva(long long group_id, int member_id, int new_ava);

	void OnGroupNameOrNoticeChange(long long group_id, int uid, GameName group_name, GuildNotice group_notice);

	unsigned long long GetUniqueGroupId(int _create_server_id);
	int GetRoleBaseAva(Role* role);
	void OnGroupChat(Role *role, long long group_id, const char* buff, int total_len);

	bool IsLoadFinish() { return m_data_state == CHAT_GROUP_DATA_STATUS_LOAD_FINISH; }

	ChatGroup* GetChatGroup(long long group_id);

	void OnRoleLoginGameSendChatGroupSimpleInfo(int uid, int plat_type, int server_id, int max_num, ARG_IN long long* group_list);

	void OnRoleHandOverLeadershipToOther(long long group_id, int old_leader_uid, int new_leader_uid);
protected:
	bool RoleCanEnterGroupCount(Role *role, bool is_cross, int *invalid_index);
	void SendProtocolToUser(int uid, const void* msg, int msg_len);

	CrossChatGroupManager();
	~CrossChatGroupManager();

	//std::map<long long, bool> m_send;
	std::map<int, std::map<long long, bool> > m_all_server_send;
	int m_check_times;


	unsigned short m_chat_group_seq;
	int m_data_state;
	unsigned int m_last_save_time;

	char*				m_buffer;		// 用于是组包

	ChatGroupMap m_chat_group_map;
};

#endif

