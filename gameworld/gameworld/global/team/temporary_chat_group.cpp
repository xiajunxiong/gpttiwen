#include "temporary_chat_group.hpp"
#include "obj/character/role.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "engineadapter.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "team.hpp"
#include "chat/msgboarditem.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "chat/crosschatgroupmanager.h"
#include "gamelog.h"
#include "servercommon/noticenum.h"

TemporaryChatGroup::TemporaryChatGroup() : m_recent_chat(RECENT_CHAT_SAVE_NUM), m_team(NULL)
{

}

TemporaryChatGroup::~TemporaryChatGroup()
{
	this->OnRelease();
}

void TemporaryChatGroup::Init(Team* team)
{
	m_team = team;
	m_info.group_id = TEMP_CHAT_GROUP_ID;
	F_STRNCPY(m_info.group_name, gamestring::g_temp_chat_group_name, sizeof(m_info.group_name));
	memset(m_info.group_notice, 0, sizeof(m_info.group_notice));
	m_info.is_cross = 1;
	m_info.member_count = 0;
	m_info.creator_uid = TEMP_CHAT_GROUP_CREATOR_UID;
}

void TemporaryChatGroup::UserJoinGroup(Role* role)
{
	if (NULL == role) return;

	m_info.AddUserInfo(role->GetUID(), role->GetAvatarType(), role->GetLevel(), role->GetProfession(), role->GetName());

	this->SendGroupSimpleInfoToAllMember();
	
	this->SendRecentChatMsg(role);

	if (role->GetUID() == m_team->GetLeaderUID())
	{		
		this->SendCreateTempChatGroupNotice(role);
	}
	else
	{
		this->SendJoinTempChatGroupNotice(role);
		this->SendWelcomeNoticeToAllMember(role);
	}
}

void TemporaryChatGroup::UserLeaveGroup(int uid)
{
	m_info.RemoveUserInfo(uid);

	this->SendLeaveNotice(uid);
	this->SendGroupSimpleInfoToAllMember();
}

void TemporaryChatGroup::OnUserLogin(Role* role)
{
	if (NULL == role) return;

	ChatGroupUserInfo* ui = m_info.GetUserInfo(role->GetUID());
	if (NULL != ui)
	{
		ui->member_logout_tamp = 0;
	}

	this->SendGroupSimpleInfo(role->GetUID(), role);
	this->SendRecentChatMsg(role);
}

void TemporaryChatGroup::OnUserLogout(Role* role)
{
	if (NULL == role) return;

	ChatGroupUserInfo* ui = m_info.GetUserInfo(role->GetUID());
	if (NULL != ui)
	{
		ui->member_logout_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
}

void TemporaryChatGroup::OnGroupChat(const char* msg, int total_len)
{
	Protocol::SCChannelChat * real_msg = (Protocol::SCChannelChat *)msg;
	real_msg->group_id = m_info.group_id; //修改一下，这个下发为群id

	for (int i = 0; i < m_info.member_count; ++i)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_info.member_uid_list[i].member_uid));
		if (NULL != role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), msg, total_len);
		}
	}

	m_recent_chat.AddChatMsg(msg, total_len);
}

void TemporaryChatGroup::BecomeOfficialChatGroup(Role* role, GameName group_name)
{
	if (NULL == role) return;

	int limit_level = GLOBALCONFIG->GetCreateChatGroupLevel();
	if (role->GetLevel() < limit_level)
	{
		this->SendLimitLevelNotice(role, limit_level);
		return;
	}

	unsigned int remain_cd_minutes = 0;
	if (role->GetRoleModuleManager()->IsInCreateChatGroupCD(&remain_cd_minutes))
	{
		this->SendCreateGroupCDNotice(role, remain_cd_minutes);
		return;
	}

	if (!role->GetRoleModuleManager()->FindEmptyCrossGroupIndex(NULL))
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_MAX);
		return;
	}

	role->GetRoleModuleManager()->SetCreateChatGroupCD();

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->BecomeOfficialInLocal(role, group_name);
	}
	else
	{
		this->BecomeOfficialInCross(role, group_name);
	}
}

void TemporaryChatGroup::GetGroupDetailInfo(Role* role)
{
	if (NULL == role) return;

	this->SendGroupInfo(role->GetUID(), role);
}

void TemporaryChatGroup::OnRelease()
{
	//通知群员，群组已解散
	for (int i = 0; i < m_info.member_count; ++i)
	{
		this->SendLeaveNotice(m_info.member_uid_list[i].member_uid);
	}
}

void TemporaryChatGroup::SendLeaveNotice(int uid)
{
	Protocol::SCTempGroupLeaveNotice notice;
	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &notice, sizeof(notice));
	}
}

void TemporaryChatGroup::SendGroupInfoToAllMember()
{
	for (int i = 0; i < m_info.member_count; ++i)
	{
		this->SendGroupInfo(m_info.member_uid_list[i].member_uid);
	}
}

void TemporaryChatGroup::SendGroupSimpleInfoToAllMember(int exclude_uid /* = 0 */)
{
	for (int i = 0; i < m_info.member_count; ++i)
	{
		if (exclude_uid == m_info.member_uid_list[i].member_uid) continue;

		this->SendGroupSimpleInfo(m_info.member_uid_list[i].member_uid);	
	}
}

void TemporaryChatGroup::SendGroupInfo(int uid, Role* role /* = NULL */)
{
	if (NULL == role) role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));

	if (NULL != role)
	{
		Protocol::SCGroupAllInfo msg;
		msg.info = m_info;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
	}
}

void TemporaryChatGroup::SendGroupSimpleInfo(int uid, Role* role /* = NULL */)
{
	if (NULL == role) role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	
	if (NULL == role)
	{
		return;
	}

	Protocol::SCTempGroupSimpleInfo msg;

	memset(msg.group.top_user_ava, 0, sizeof(msg.group.top_user_ava));
	int top_index = 0;
	for (int member_info = 0; member_info < m_info.member_count && top_index < 3; member_info++)
	{
		if (m_info.member_uid_list[member_info].member_uid == 0)continue;
		msg.group.top_user_ava[top_index++] = m_info.member_uid_list[member_info].member_ava;
	}
	msg.group.group_id = m_info.group_id;
	msg.group.is_cross = m_info.is_cross;
	STRNCPY(msg.group.group_name, m_info.group_name, sizeof(GameName));
	msg.group.group_master_id = m_info.creator_uid;
	msg.group.member_count = m_info.member_count;
	
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg));
}

void TemporaryChatGroup::SendJoinTempChatGroupNotice(Role* role)
{
	if (NULL == role) return;

	const char* leader_name = m_team->GetLeaderName();

	Protocol::SCChatGroupMsgNotice msg;
	msg.group_id = m_info.group_id;
	
	msg.msg_length = SNPRINTF(msg.msg_content, sizeof(msg.msg_content), gamestring::g_chat_group_msg_notice_join_temp, leader_name ? leader_name : "-");
	if (msg.msg_length > 0)
	{
		int send_length = sizeof(msg) - sizeof(msg.msg_content) + msg.msg_length;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_length);
	}
}

void TemporaryChatGroup::SendCreateTempChatGroupNotice(Role* role)
{
	if (NULL == role) return;

	Protocol::SCChatGroupMsgNotice msg;
	msg.group_id = m_info.group_id;

	msg.msg_length = SNPRINTF(msg.msg_content, sizeof(msg.msg_content), gamestring::g_chat_group_msg_notice_create_temp);
	if (msg.msg_length > 0)
	{
		int send_length = sizeof(msg) - sizeof(msg.msg_content) + msg.msg_length;
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_length);
	}
}

void TemporaryChatGroup::SendWelcomeNoticeToAllMember(Role* welcome_role)
{
	if (NULL == welcome_role) return;

	Protocol::SCChatGroupMsgNotice msg;
	msg.group_id = m_info.group_id;

	msg.msg_length = SNPRINTF(msg.msg_content, sizeof(msg.msg_content), gamestring::g_welcome_join_temp_group_notice, welcome_role->GetName());
	if (msg.msg_length > 0)
	{
		int send_length = sizeof(msg) - sizeof(msg.msg_content) + msg.msg_length;

		for (int i = 0; i < m_info.member_count; ++i)
		{
			if (welcome_role->GetUID() == m_info.member_uid_list[i].member_uid) continue;

			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_info.member_uid_list[i].member_uid));
			if (NULL == role) continue;

			EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_length);
		}
	}
}

void TemporaryChatGroup::SendRecentChatMsg(Role* role)
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

void TemporaryChatGroup::SendTeammateChatGroupFullNotice(Role* role, Role* teammate)
{
	if (NULL == role || NULL == teammate) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_teammate_chat_group_full, teammate->GetName());
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void TemporaryChatGroup::SendLimitLevelNotice(Role* role, int limit_level)
{
	if (NULL == role) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_create_chat_group_level_limit_notice, limit_level);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void TemporaryChatGroup::SendCreateGroupCDNotice(Role* role, unsigned int remain_cd_minutes)
{
	if (NULL == role) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_create_chat_group_cd_limit_notice, remain_cd_minutes);
	World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
}

void TemporaryChatGroup::BecomeOfficialInCross(Role* role, GameName group_name)
{
	ChatGroupUserInfo info_list[5];
	Role* role_list[5];
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(role_list) == ARRAY_ITEM_COUNT(info_list));

	int member_num = 0;
	info_list[member_num].member_uid = role->GetUID();
	info_list[member_num].member_ava = role->GetRoleBaseAva();
	info_list[member_num].member_level = role->GetLevel();
	info_list[member_num].member_prof = role->GetProfession();
	F_STRNCPY(info_list[member_num].member_name, role->GetName(), sizeof(GameName));
	role_list[member_num] = role;
	member_num += 1;
	for (int i = 0; i < m_team->GetMemberCount() && member_num < ARRAY_ITEM_COUNT(info_list); ++i)
	{
		// 不在线的不管， 但暂离的要算进来
		Role* member = m_team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == role->GetUID()) continue;
		if (!member->GetRoleModuleManager()->FindEmptyCrossGroupIndex(NULL))
		{
			this->SendTeammateChatGroupFullNotice(role, member);
			continue; // 没有空位就不加进来了
		}

		info_list[member_num].member_uid = member->GetUID();
		info_list[member_num].member_ava = member->GetRoleBaseAva();
		info_list[member_num].member_level = member->GetLevel();
		info_list[member_num].member_prof = member->GetProfession();
		F_STRNCPY(info_list[member_num].member_name, member->GetName(), sizeof(GameName));
		role_list[member_num] = member;
		member_num += 1;
	}

	long long cross_group_id = 0LL;
	int ret = CrossChatGroupManager::Instance().OnTempGroupBecomeOfficial(ARRAY_ITEM_COUNT(info_list), member_num, info_list, group_name, &cross_group_id);
	if (0 == ret)
	{
		Protocol::SCChatGroupMsgNotice msg;
		msg.group_id = cross_group_id;
		msg.msg_length = SNPRINTF(msg.msg_content, sizeof(msg.msg_content), gamestring::g_chat_group_msg_notice_become_official, role->GetName(), group_name);

		for (int i = 0; i < member_num && i < ARRAY_ITEM_COUNT(role_list); ++i)
		{
			Role* member = role_list[i];
			int empty_index = -1;
			if (member->GetRoleModuleManager()->FindEmptyCrossGroupIndex(&empty_index))
			{
				member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[empty_index] = cross_group_id;
				CrossChatGroupManager::Instance().SendRoleOneGroupInfo(member, cross_group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);

				if (msg.msg_length > 0) // 转正群通知
				{
					int send_length = sizeof(msg) - sizeof(msg.msg_content) + msg.msg_length;
					EngineAdapter::Instance().NetSend(member->GetNetId(), &msg, send_length);
				}
			}

			Protocol::SCTempBecomeOfficialNotice notice;
			notice.group_id = cross_group_id;
			EngineAdapter::Instance().NetSend(member->GetNetId(), &notice, sizeof(notice));
		}

		role->NoticeNum(noticenum::NT_TEMP_GROUP_BECOME_OFFICIAL_SUCC);

		gamelog::g_log_chat_system.printf(LL_INFO, "TemporaryChatGroup::BecomeOfficialInCross SUCC! role[%d,%s] group_name[%s] group_id[%lld]", role->GetUID(), role->GetName(), group_name, cross_group_id);
	}
	else
	{
		gamelog::g_log_chat_system.printf(LL_INFO, "TemporaryChatGroup::BecomeOfficialInCross Failed! role[%d,%s] ret[%d]", role->GetUID(), role->GetName(), ret);
	}
}

void TemporaryChatGroup::BecomeOfficialInLocal(Role* role, GameName group_name)
{
	crossgameprotocal::GameHiddenTempGroupBecomeOfficialReq req;
	req.plat_type = LocalConfig::Instance().GetPlatType();
	req.server_id = LocalConfig::Instance().GetMergeServerId();
	F_STRNCPY(req.group_name, group_name, sizeof(GameName));
	req.member_num = 0;
	req.member_list[req.member_num].uid = role->GetUID();
	req.member_list[req.member_num].avatar_type = role->GetAvatarType();
	req.member_list[req.member_num].level = role->GetLevel();
	req.member_list[req.member_num].profession = role->GetProfession();
	F_STRNCPY(req.member_list[req.member_num].name, role->GetName(), sizeof(GameName));
	req.member_num += 1;

	for (int i = 0; i < m_team->GetMemberCount(); ++i)
	{
		// 不在线的不管， 但暂离的要算进来
		Role* member = m_team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == role->GetUID()) continue;
		if (!member->GetRoleModuleManager()->FindEmptyCrossGroupIndex(NULL))
		{
			this->SendTeammateChatGroupFullNotice(role, member);
			continue; // 没有空位就不加进来了
		}

		req.member_list[req.member_num].uid = member->GetUID();
		req.member_list[req.member_num].avatar_type = member->GetAvatarType();
		req.member_list[req.member_num].level = member->GetLevel();
		req.member_list[req.member_num].profession = member->GetProfession();
		F_STRNCPY(req.member_list[req.member_num].name, member->GetName(), sizeof(GameName));
		req.member_num += 1;
	}

	int send_len = sizeof(req) - sizeof(req.member_list) + req.member_num * sizeof(req.member_list[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, send_len);
}

