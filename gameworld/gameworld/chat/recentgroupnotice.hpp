#ifndef __RECENT_GROUP_NOTICE_HPP__
#define __RECENT_GROUP_NOTICE_HPP__

#include "servercommon/servercommon.h"
#include <list>

class MsgBoardItem;
struct ChatGroupNoticeItem
{
	unsigned int notice_time;
	MsgBoardItem* notice_msg;
};

class RecentGroupNotice
{
public:
	typedef std::list<ChatGroupNoticeItem> RecentNoticeList;

public:
	RecentGroupNotice(int max_save_recent_chat_num);
	~RecentGroupNotice();
	NONCOPYABLE(RecentGroupNotice);

	void AddGroupNotice(const char* msg, int msg_len);
	void GetChatMsgList(unsigned int last_check_notice_time, int max_num, ARG_OUT int* out_num, ARG_OUT MsgBoardItem** msg_list);
private:

	int m_max_save_num;
	RecentNoticeList m_notice_list;
};

#endif