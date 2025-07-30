#ifndef __TEMPORARY_CHAT_GROUP_HPP__
#define __TEMPORARY_CHAT_GROUP_HPP__

#include "servercommon/chatserverdef.h"
#include "chat/recentchat.hpp"

class Role;
class Team;
class TemporaryChatGroup
{
public:
	TemporaryChatGroup();
	~TemporaryChatGroup();

	void Init(Team* team);

	void* operator new(size_t c);
	void operator delete(void* m);

	void UserJoinGroup(Role* role);
	void UserLeaveGroup(int uid);
	void OnUserLogin(Role* role);
	void OnUserLogout(Role* role);

	void OnGroupChat(const char* msg, int total_len);

	void BecomeOfficialChatGroup(Role* role, GameName group_name);
	void GetGroupDetailInfo(Role* role);
private:
	void OnRelease();
	void SendLeaveNotice(int uid);
	void SendGroupInfoToAllMember();
	void SendGroupSimpleInfoToAllMember(int exclude_uid = 0);
	void SendGroupInfo(int uid, Role* role = NULL);
	void SendGroupSimpleInfo(int uid, Role* role = NULL);
	void SendJoinTempChatGroupNotice(Role* role);
	void SendCreateTempChatGroupNotice(Role* role);
	void SendWelcomeNoticeToAllMember(Role* welcome_role);
	void SendRecentChatMsg(Role* role);
	void SendTeammateChatGroupFullNotice(Role* role, Role* teammate);
	void SendLimitLevelNotice(Role* role, int limit_level);
	void SendCreateGroupCDNotice(Role* role, unsigned int remain_cd_minutes);

	void BecomeOfficialInCross(Role* role, GameName group_name);
	void BecomeOfficialInLocal(Role* role, GameName group_name);

	ChatGroupInfo m_info;
	RecentChat m_recent_chat;
	Team* m_team;
};

#endif