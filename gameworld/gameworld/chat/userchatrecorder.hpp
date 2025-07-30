#ifndef __USER_CHAT_RECORDER_HPP__
#define __USER_CHAT_RECORDER_HPP__

#include <map>
#include <set>
#include <deque>

class ChatUser;
class RecentChat;
class Role;
class UserChatRecorder
{
public:
	typedef std::map<int, RecentChat*> RecentChatMap;

public:
	UserChatRecorder();
	~UserChatRecorder();

	void AddSingleChatRecord(int chat_with_uid, const char* msg, int length);
	const RecentChat* GetRecentChat(int chat_with_uid) const;
	void OnRoleFetchRecentChat(Role* role);
	void OnUserFetchRecentChatFromHidden(int uid, std::set<int>& chat_with_uid_set);
	static void AssembleChatWithUidSetByRole(Role* role, std::set<int>& chat_with_uid_set);
	void AssembleChatWithUidSetByRecentCaller(std::set<int>& chat_with_uid_set);
private:
	void Release();
	RecentChat* MutableRecentChat(int chat_with_uid);
	void RemoveChatRecord(int chat_with_uid);	
	void AddRecentCaller(int caller_uid);

	RecentChatMap m_conversations;

	std::deque<int> m_recent_caller;	// 保存若干个最近给我发私聊的uid
};

#endif