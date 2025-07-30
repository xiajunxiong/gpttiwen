#include "chatgroup.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "engineadapter.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "gamelog.h"
#include "chat/msgboarditem.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"

ChatGroup::ChatGroup() : m_is_dirty(false), m_old_state(false), m_is_valid(false), 
	m_recent_chat(RECENT_CHAT_SAVE_NUM), m_recent_notice(RECENT_NOTICE_SAVE_NUM)
{

}

ChatGroup::~ChatGroup()
{

}

void ChatGroup::InitChatGroupInfo(const ChatGroupInfo& info, bool is_from_init)
{
	m_info = info;
	m_is_valid = true;
	if (is_from_init)
	{
		m_is_dirty = false;
		m_old_state = true;
	}
	else
	{
		m_is_dirty = true;
		m_old_state = false;
	}
}

const ChatGroupUserInfo* ChatGroup::GetUserInfo(int role_id, ARG_OUT int* member_index /* = NULL */)
{
	return m_info.GetUserInfo(role_id, member_index);
}

bool ChatGroup::IsMember(int role_id) const
{
	return NULL != m_info.GetConstUserInfo(role_id);
}

void ChatGroup::UpdateMemberBaseInfo(Role* role)
{
	if (NULL == role) return;

	ChatGroupUserInfo* role_info = this->MutableUserInfo(role->GetUID());
	
	if (role_info->member_ava != role->GetRoleBaseAva())
	{
		role_info->member_ava = role->GetRoleBaseAva();
	}

	if (role_info->member_level != role->GetLevel())
	{
		role_info->member_level = role->GetLevel();
	}

	if (role_info->member_prof != role->GetBaseProfession())
	{
		role_info->member_prof = role->GetBaseProfession();
	}

	F_STRNCPY(role_info->member_name, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));

	this->SetDirty();
}

void ChatGroup::OnMemberLogout(int uid)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	role_info->member_logout_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	this->SetDirty();
}

void ChatGroup::OnMemberLogin(int uid)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	role_info->member_logout_tamp = 0u;

	this->SetDirty();

	this->SendRecentMsgToMember(uid);
	this->SendRecentNoticeToMember(uid);
}

void ChatGroup::OnMemberChat(int uid)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	role_info->member_logout_tamp = 0u;

	this->SetDirty();
}

void ChatGroup::OnMemberChangeProf(int uid, int new_prof, int new_ava)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	role_info->member_prof = new_prof;
	role_info->member_ava = new_ava;

	this->SetDirty();
}

void ChatGroup::OnMemberChangeName(int uid, GameName new_name)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	F_STRNCPY(role_info->member_name, new_name, sizeof(GameName));

	this->SetDirty();
}

void ChatGroup::OnMemberChangeAva(int uid, int new_ava)
{
	ChatGroupUserInfo* role_info = this->MutableUserInfo(uid);
	if (NULL == role_info) return;

	role_info->member_ava = new_ava;

	this->SetDirty();
}

void ChatGroup::DismissGroup()
{
	this->SetInvalid(); //TODO add log
	this->SetDirty();

	gamelog::g_log_chat_system.printf(LL_INFO, "ChatGroup::DismissGroup group_id=[%lld]", m_info.group_id);
}

void ChatGroup::OnServerStop()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	for (int member_idx = 0; member_idx < m_info.member_count && member_idx < CHAT_GROUP_MEMBER_MAX; member_idx++)
	{
		if (m_info.member_uid_list[member_idx].member_logout_tamp == 0)
		{
			m_info.member_uid_list[member_idx].member_logout_tamp = now;
			this->SetDirty();
		}
	}
}

void ChatGroup::OnChangeGroupNotice(GameName group_name, GuildNotice group_notice)
{
	F_STRNCPY(m_info.group_name, group_name, sizeof(GameName));
	F_STRNCPY(m_info.group_notice, group_notice, sizeof(GuildNotice));

	this->SetDirty();
}

void ChatGroup::OnModifyLeader(int new_leader_uid)
{
	m_info.creator_uid = new_leader_uid;

	this->SetDirty();

	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPERATE_TYPE_LEADER_CHANGE_SYNC_GAME; //操作传到游戏服
	chat_msg.uid = new_leader_uid;
	chat_msg.group_id = m_info.group_id;
	
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
}

bool ChatGroup::SendToGroup(void *msg, size_t length)
{
	const ChatGroupInfo &info = this->GetGroupInfo();
	if (info.member_count <= 0)
	{
		return false;
	}

	Protocol::SCChannelChat * real_msg = (Protocol::SCChannelChat *)msg;
	real_msg->group_id = info.group_id;//修改一下，这个下发为群id

	SceneManager *manager = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info.member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		Role *member = manager->GetRole(info.member_uid_list[i].member_uid);
		if (NULL == member)
		{
			continue;//不操作，等待协议发往再自行操作
		}

		EngineAdapter::Instance().NetSend(member->GetNetId(), (char *)real_msg, length);
	}

	m_recent_chat.AddChatMsg((const char*)msg, length);

	return true;
}

bool ChatGroup::EraseUser(int uid)
{
	bool ret = m_info.RemoveUserInfo(uid);
	if (ret)
	{
		this->SetDirty();
	}

	return ret;
}

bool ChatGroup::AddUserInfo(int member_id, int member_ava, int member_level, int member_prof, const GameName member_name)
{
	bool ret = m_info.AddUserInfo(member_id, member_ava, member_level, member_prof, member_name);
	if (ret)
	{
		this->SetDirty();
	}

	return ret;
}

ChatGroupUserInfo* ChatGroup::MutableUserInfo(int role_id, int* member_idx /* = NULL */)
{
	return m_info.GetUserInfo(role_id, member_idx);
}

void ChatGroup::SendRecentMsgToMember(int member_uid)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member_uid));
	if (NULL != role)
	{
		MsgBoardItem* recent_msg_list[RECENT_CHAT_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		m_recent_chat.GetChatMsgList(ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);
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
	else
	{
		// 发回原服
		MsgBoardItem* recent_msg_list[RECENT_CHAT_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		m_recent_chat.GetChatMsgList(ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);
		for (int i = 0; i < recent_msg_num; ++i)
		{
			int send_len = 0;
			const char* msg = recent_msg_list[i]->GetSendBuffer(&send_len);
			if (send_len > 0)
			{
				InternalComm::Instance().SendToGameUserThroughCross(member_uid, msg, send_len);
			}
		}
	}
}

void ChatGroup::SendRecentNoticeToMember(int member_uid)
{
	int member_index = 0;
	ChatGroupUserInfo* user_info = this->MutableUserInfo(member_uid, &member_index);
	if (NULL == user_info) return;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member_uid));
	if (NULL != role)
	{
		MsgBoardItem* recent_msg_list[RECENT_NOTICE_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		m_recent_notice.GetChatMsgList(m_info.member_last_check_notice_time[member_index], ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);
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
	else
	{
		// 发回原服
		MsgBoardItem* recent_msg_list[RECENT_NOTICE_SAVE_NUM] = { 0 };
		int recent_msg_num = 0;
		m_recent_notice.GetChatMsgList(m_info.member_last_check_notice_time[member_index], ARRAY_LENGTH(recent_msg_list), &recent_msg_num, recent_msg_list);
		for (int i = 0; i < recent_msg_num; ++i)
		{
			int send_len = 0;
			const char* msg = recent_msg_list[i]->GetSendBuffer(&send_len);
			if (send_len > 0)
			{
				InternalComm::Instance().SendToGameUserThroughCross(member_uid, msg, send_len);
			}
		}
	}

	m_info.member_last_check_notice_time[member_index] = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void ChatGroup::SendRecentNoticeToOnlineMember()
{
	for (int i = 0; i < m_info.member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		if (UserCacheManager::Instance().IsUserOnline(IntToUid(m_info.member_uid_list[i].member_uid)))
		{
			this->SendRecentNoticeToMember(m_info.member_uid_list[i].member_uid);
		}
	}
}

void ChatGroup::AssembleGroupInfoProtocol(ARG_OUT Protocol::SCRoleOneGroupInfo* msg)
{
	if (NULL == msg) return;

	memset(msg->group.top_user_ava, 0, sizeof(msg->group.top_user_ava));

	int top_index = 0;
	for (int member_info = 0; member_info < m_info.member_count && top_index < 3; member_info++)
	{
		if (m_info.member_uid_list[member_info].member_uid == 0)continue;
		
		msg->group.top_user_ava[top_index++] = m_info.member_uid_list[member_info].member_ava;
	}

	msg->group.group_id = m_info.group_id;
	msg->group.is_cross = m_info.is_cross;
	STRNCPY(msg->group.group_name, m_info.group_name, sizeof(GameName));
	msg->group.group_master_id = m_info.creator_uid;
	msg->group.member_count = m_info.member_count;
}

void ChatGroup::AddGroupNotice(const char* notice_msg, int notice_length)
{
	m_recent_notice.AddGroupNotice(notice_msg, notice_length);

	this->SendRecentNoticeToOnlineMember();
}

