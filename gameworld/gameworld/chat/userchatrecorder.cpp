#include "userchatrecorder.hpp"
#include "recentchat.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/chatserverdef.h"
#include "other/mentor/shitu.hpp"
#include "friend/friendmanager.h"
#include "msgboarditem.h"
#include "engineadapter.h"
#include "internalcomm.h"

UserChatRecorder::UserChatRecorder()
{

}

UserChatRecorder::~UserChatRecorder()
{

}

void UserChatRecorder::AddSingleChatRecord(int chat_with_uid, const char* msg, int length)
{
	RecentChat* conversation = this->MutableRecentChat(chat_with_uid);
	if (NULL == conversation)
	{
		RecentChat* temp = new RecentChat(RECENT_CHAT_SAVE_NUM);
		conversation = m_conversations.insert(RecentChatMap::value_type(chat_with_uid, temp)).first->second;
	}

	if (NULL == conversation) return;

	conversation->AddChatMsg(msg, length);
	this->AddRecentCaller(chat_with_uid);
}

const RecentChat* UserChatRecorder::GetRecentChat(int chat_with_uid) const
{
	RecentChatMap::const_iterator it = m_conversations.find(chat_with_uid);
	if (it != m_conversations.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void UserChatRecorder::OnRoleFetchRecentChat(Role* role)
{
	std::set<int> chat_with_uid_set;
	AssembleChatWithUidSetByRole(role, chat_with_uid_set);
	this->AssembleChatWithUidSetByRecentCaller(chat_with_uid_set);

	for (std::set<int>::iterator it = chat_with_uid_set.begin(); it != chat_with_uid_set.end(); ++it)
	{
		int chat_with_uid = *it;
		RecentChat* rc = this->MutableRecentChat(chat_with_uid);
		if (NULL == rc) continue;

		MsgBoardItem* recent_msg_list[RECENT_CHAT_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		rc->GetChatMsgList(ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);

		for (int i = 0; i < recent_msg_num; ++i)
		{
			int send_len = 0;
			const char* msg = recent_msg_list[i]->GetSendBuffer(&send_len);
			if (send_len > 0)
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), msg, send_len);
			}
		}
	}

}

void UserChatRecorder::OnUserFetchRecentChatFromHidden(int uid, std::set<int>& chat_with_uid_set)
{
	this->AssembleChatWithUidSetByRecentCaller(chat_with_uid_set);
	for (std::set<int>::iterator it = chat_with_uid_set.begin(); it != chat_with_uid_set.end(); ++it)
	{
		int chat_with_uid = *it;
		RecentChat* rc = this->MutableRecentChat(chat_with_uid);
		if (NULL == rc) continue;

		MsgBoardItem* recent_msg_list[RECENT_CHAT_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		rc->GetChatMsgList(ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);

		for (int i = 0; i < recent_msg_num; ++i)
		{
			int send_len = 0;
			const char* msg = recent_msg_list[i]->GetSendBuffer(&send_len);
			if (send_len > 0)
			{
				InternalComm::Instance().SendToHiddenUserThroughCross(uid, msg, send_len);
			}
		}
	}
}

void UserChatRecorder::AssembleChatWithUidSetByRole(Role* role, std::set<int>& chat_with_uid_set)
{
	const FriendCommonParam& fcp = role->GetRoleModuleManager()->GetFriendCommonData();
	for (int i = 0; i < ARRAY_ITEM_COUNT(fcp.curr_recently_chat); ++i)
	{
		if (fcp.curr_recently_chat[i] == 0) continue;

		chat_with_uid_set.insert(fcp.curr_recently_chat[i]);
	}

	const ShiTuParam& shitu_param = role->GetRoleModuleManager()->GetShiTu()->GetShiTuParam();
	for (int i = 0; i < ARRAY_ITEM_COUNT(shitu_param.relation_data.apprentice_list); ++i)
	{
		const RelationInfo& ri = shitu_param.relation_data.apprentice_list[i];
		if (0 == ri.role_id) continue;

		chat_with_uid_set.insert(ri.role_id);
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(shitu_param.relation_data.master_list); ++i)
	{
		const RelationInfo& ri = shitu_param.relation_data.master_list[i];
		if (0 == ri.role_id) continue;

		chat_with_uid_set.insert(ri.role_id);
	}

	const FriendManager::UserFrinedSet* friend_set = FriendManager::Instance().GetUserFriendSetInfo(role);
	if (NULL != friend_set)
	{
		FriendManager::UserFrinedSet::const_iterator it = friend_set->begin();
		for (; it != friend_set->end(); ++it)
		{
			int friend_uid = UidToInt(LongLongToUniqueUserID(*it).user_id);
			if (0 == friend_uid) continue;

			chat_with_uid_set.insert(friend_uid);
		}
	}
}

void UserChatRecorder::Release()
{
	for (RecentChatMap::iterator it = m_conversations.begin(); it != m_conversations.end(); ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	m_conversations.clear();
}

RecentChat* UserChatRecorder::MutableRecentChat(int chat_with_uid)
{
	RecentChatMap::iterator it = m_conversations.find(chat_with_uid);
	if (it != m_conversations.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void UserChatRecorder::RemoveChatRecord(int chat_with_uid)
{
	RecentChat* conversation = this->MutableRecentChat(chat_with_uid);
	if (NULL == conversation) return;

	delete conversation;
	m_conversations.erase(chat_with_uid);
}

void UserChatRecorder::AssembleChatWithUidSetByRecentCaller(std::set<int>& chat_with_uid_set)
{
	for (size_t i = 0; i < m_recent_caller.size(); ++i)
	{
		chat_with_uid_set.insert(m_recent_caller[i]);
	}
}

void UserChatRecorder::AddRecentCaller(int caller_uid)
{
	for (size_t i = 0; i < m_recent_caller.size(); ++i)
	{
		if (caller_uid == m_recent_caller[i])
		{
			return;
		}
	}

	m_recent_caller.push_front(caller_uid);
	if (m_recent_caller.size() > MAX_RECENTLY_CHAT_NUM)
	{
		m_recent_caller.pop_back();
	}
}

