#ifndef __RECENT_CHAT_HPP__
#define __RECENT_CHAT_HPP__

#include "servercommon/servercommon.h"
#include <list>

class MsgBoardItem;
class RecentChat
{
public:
	typedef std::list<MsgBoardItem*> RecentChatMsgList;

public:
	RecentChat(int max_save_recent_chat_num);
	~RecentChat();
	NONCOPYABLE(RecentChat);

	void AddChatMsg(const char* msg, int msg_len);
	void GetChatMsgList(int max_num, ARG_OUT int* out_num, ARG_OUT MsgBoardItem** msg_list);
private:

	int m_max_save_num;
	RecentChatMsgList m_msg_list;
};

#endif