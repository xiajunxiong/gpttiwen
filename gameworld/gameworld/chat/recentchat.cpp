#include "recentchat.hpp"
#include "chat/msgboarditem.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "chat/muteplayerrecorder.hpp"

RecentChat::RecentChat(int max_save_recent_chat_num) : m_max_save_num(max_save_recent_chat_num)
{

}

RecentChat::~RecentChat()
{
	RecentChatMsgList::iterator it = m_msg_list.begin();
	for (; it != m_msg_list.end(); ++it)
	{
		if (NULL != *it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_msg_list.clear();
}

void RecentChat::AddChatMsg(const char* msg, int msg_len)
{
	MsgBoardItem* chat = new MsgBoardItem();
	chat->SetMsg(msg, msg_len);

	m_msg_list.push_back(chat);
	if (static_cast<int>(m_msg_list.size()) > m_max_save_num)
	{
		MsgBoardItem* front = m_msg_list.front();
		m_msg_list.pop_front();

		delete front;
	}
}

void RecentChat::GetChatMsgList(int max_num, ARG_OUT int* out_num, ARG_OUT MsgBoardItem** msg_list)
{
	*out_num = 0;
	RecentChatMsgList::iterator it = m_msg_list.begin();
	for (; it != m_msg_list.end() && (*out_num) < max_num;)
	{
		int send_len = 0;
		const char *send_buffer = (*it)->GetSendBuffer(&send_len);
		int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::SCChannelChat, msg_length));
		if (send_len >= offset)
		{
			Protocol::SCChannelChat *channel_chat = (Protocol::SCChannelChat*)send_buffer;
			if (MutePlayerRecorder::Instance().IsMute(channel_chat->from_uid))
			{
				delete *it;
				it = m_msg_list.erase(it); 
				continue;
			}
		}

		msg_list[(*out_num)++] = *it;
		++it;
	}
}

