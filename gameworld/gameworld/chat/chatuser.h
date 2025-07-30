#ifndef __CHATUSER_HPP__
#define __CHATUSER_HPP__

#include <stdlib.h>

#include "servercommon/chatserverdef.h"
#include "engineadapter.h"
#include "servercommon/servercommon.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "userchatrecorder.hpp"
#include <vector>

class Role;
class ChatManager;

class ChatUser
{
public:
	ChatUser();
	ChatUser(ChatManager *chat_manager, int uid, GSNetID netid);
	~ChatUser();

	const GSNetID & GetNetId() { return m_netid; }
	int GetUID() const;

	bool IgnoreWorldTalk();
	bool IsForbidChannel(Role *role);
	bool IsShowMyTalk(Role * role);	//被禁言时是否要显示自己输出的语句
	bool ChannelCanChat(Role *role, int channel_type); // 检查
	int GetChannelOpenLevel(int channel_type);

	void OnSingleChat(Protocol::CSSingleChat *sc_req);
	void OnGuildChat(Protocol::CSChannelChat *cc_req);
	void OnWorldChat(Protocol::CSChannelChat *cc_req);

	bool CanSingleChatLimit(Role *role, int to_uid);

	bool operator==(const ChatUser & other) const;
private:
	int GetCooldown(int channal_type);

	ChatManager *m_chat_manager;
	int m_uid;
	GSNetID m_netid;

	unsigned int next_active_list[chatdef::CHANNEL_TYPE_MAX];


	struct SingleChatMonitor
	{
		SingleChatMonitor() : to_uid(0), last_single_chat_time(0) {}

		UID to_uid;
		long long last_single_chat_time;
	};

	int m_world_msg_last_length;
	int m_world_msg_same_length_accumulate_count;
	time_t m_world_msg_first_same_length_timestamp;
	bool m_ignore_world_talk;
};

#endif
