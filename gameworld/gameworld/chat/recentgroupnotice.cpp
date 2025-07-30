#include "recentgroupnotice.hpp"
#include "msgboarditem.h"
#include "engineadapter.h"

RecentGroupNotice::RecentGroupNotice(int max_save_recent_chat_num) : m_max_save_num(max_save_recent_chat_num)
{

}

RecentGroupNotice::~RecentGroupNotice()
{

}

void RecentGroupNotice::AddGroupNotice(const char* msg, int msg_len)
{
	MsgBoardItem* notice = new MsgBoardItem();
	notice->SetMsg(msg, msg_len);

	ChatGroupNoticeItem item;
	item.notice_msg = notice;
	item.notice_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_notice_list.push_back(item);
	if (static_cast<int>(m_notice_list.size()) > m_max_save_num)
	{
		ChatGroupNoticeItem front = m_notice_list.front();
		m_notice_list.pop_front();

		delete front.notice_msg;
	}
}

void RecentGroupNotice::GetChatMsgList(unsigned int last_check_notice_time, int max_num, ARG_OUT int* out_num, ARG_OUT MsgBoardItem** msg_list)
{
	*out_num = 0;
	RecentNoticeList::iterator it = m_notice_list.begin();
	for (; it != m_notice_list.end() && (*out_num) < max_num;)
	{
		if (last_check_notice_time >= it->notice_time)
		{
			++it;
			continue;
		}

		msg_list[(*out_num)++] = it->notice_msg;
		++it;
	}
}

