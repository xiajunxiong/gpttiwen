#ifndef __CHAT_GROUP_HPP__
#define __CHAT_GROUP_HPP__

#include "servercommon/chatserverdef.h"
#include "recentchat.hpp"
#include "recentgroupnotice.hpp"

namespace Protocol
{
	class SCRoleOneGroupInfo;
}

class Role;
class ChatGroup
{
public:
	ChatGroup();
	~ChatGroup();

	void InitChatGroupInfo(const ChatGroupInfo& info, bool is_from_init);

	inline bool IsDirty() const { return m_is_dirty; }
	inline bool IsValid() const { return m_is_valid; }
	inline bool GetOldState() const { return m_old_state; }
	inline void ClearDirtyFlag() { m_is_dirty = false; }
	inline void SetInvalid() { m_is_valid = false; }	// 删除群组时调用
	inline void SetOldState(bool oldstate) { m_old_state = oldstate; }
	const ChatGroupInfo& GetGroupInfo() const { return m_info; }

	const ChatGroupUserInfo* GetUserInfo(int role_id, ARG_OUT int* member_index = NULL);

	bool IsMember(int role_id) const;
	void UpdateMemberBaseInfo(Role* role);
	void OnMemberLogout(int uid);
	void OnMemberLogin(int uid);
	void OnMemberChat(int uid);
	void OnMemberChangeProf(int uid, int new_prof, int new_ava);
	void OnMemberChangeName(int uid, GameName new_name);
	void OnMemberChangeAva(int uid, int new_ava);
	void DismissGroup();
	void OnServerStop();
	void OnChangeGroupNotice(GameName group_name, GuildNotice group_notice); // 改变公告
	void OnModifyLeader(int new_leader_uid);

	bool SendToGroup(void *msg, size_t length);
	bool EraseUser(int uid);
	bool AddUserInfo(int member_id, int member_ava, int member_level, int member_prof, const GameName member_name);

	void AddGroupNotice(const char* notice_msg, int notice_length);
	void AssembleGroupInfoProtocol(ARG_OUT Protocol::SCRoleOneGroupInfo* msg);
private:
	inline void SetDirty() { m_is_dirty = true; }
	ChatGroupUserInfo* MutableUserInfo(int role_id, int* member_idx = NULL);
	void SendRecentMsgToMember(int member_uid);
	void SendRecentNoticeToMember(int member_uid);
	void SendRecentNoticeToOnlineMember();
	
	bool m_is_dirty;
	bool m_old_state;
	bool m_is_valid;

	ChatGroupInfo m_info;
	RecentChat m_recent_chat;
	RecentGroupNotice m_recent_notice;
};

#endif