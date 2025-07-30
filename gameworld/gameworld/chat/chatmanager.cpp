#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"

#include "engineadapter.h"
#include "scene/scene.h"
#include "world.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/usercache/usercache.hpp"

#include "protocol/msgother.h"

#include "other/rolemodulemanager.hpp"
#include "friend/friendmanager.h"
#include "other/pet/pet.hpp"
#include "other/facescore/facescore.hpp"
#include "equipment/equipmentmanager.h"
#include "other/medal/medal.hpp"
#include "other/flyupsystem/flyupsystem.hpp"

#include "item/knapsack.h"
#include "item/itempool.h"
#include "chat/chatuser.h"
#include "chatmanager.h"
#include "other/funopen/funopen.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/worldstatus/worldstatus.hpp"
#include "gamedef.h"
#include "global/rmibackobjdef.h"
#include "global/cross/crossusershadow.hpp"
#include "other/route/mailroute.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/cross/crossmanager.h"
#include "servercommon/noticenum.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/offlineregister/offlineregister.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "crosschatgroupmanager.h"
#include "muteplayerrecorder.hpp"

#include "friend/friendintimacy.hpp"
#include "timer/delayqueryuserchatrecord.hpp"

static const int MSX_GSNETID_NUM = 1024 * 16;
static GSNetID gsnetidlist[MSX_GSNETID_NUM];

ChatManager::ChatManager(): chat_item_info_curr_seq(0), m_chat_group_seq(0), m_data_state(CHAT_GROUP_DATA_STATUS_INVALID), m_last_save_time(0u), m_is_change(false)
{
	chat_item_last_checkout_tamp = 0;
	m_buffer = new char[MAX_MSG_SIZE];	// 4M, 不可能有比4M还大的一个网络包，一定够用了
	memset(m_buffer, 0, MAX_MSG_SIZE);
}

ChatManager::~ChatManager()
{

}

ChatManager & ChatManager::Instance()
{
	static ChatManager umm;
	return umm;
}

void ChatManager::OnUserLogin(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	this->SetChatUser(role);

	this->DelayQueryUserChatRecord(role->GetUID());

	if (CrossConfig::Instance().IsHiddenServer())//跨服本地群组不检查
	{
		return;
	}

	{
		crossgameprotocal::GameHiddenRoleLoginSendCrossChatGroupSimpleInfo msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.uid = role->GetUID();
		memcpy(msg.cross_chat_group_list, role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list, sizeof(msg.cross_chat_group_list));
		UNSTD_STATIC_CHECK(sizeof(msg.cross_chat_group_list) == sizeof(role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list));
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}

	//this->SendRoleGroupInfo(role, Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_LOGIN);
	this->CheckRoleInGroup(role, true);
	this->CheckSingleChatBoard(role);
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		this->CheckGroupChatBoard(role, role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i]);
	}

	this->SendChatRecrodList(role);

	this->SendSimpleRecord(role);
}

void ChatManager::OnUserLogout(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	ChatUserListIterator it = m_chat_user_list.find(role->GetUID());
	if (it != m_chat_user_list.end())
	{
		m_chat_user_list.erase(it);
	}	

	std::map<int, std::list<ChatUser> >::iterator prof_chat_list_iter = m_prof_chat_list.find(role->GetBaseProfession());
	if (prof_chat_list_iter != m_prof_chat_list.end())
	{
		ChatUser chat_user(this, role->GetUID(), role->GetNetId());
		prof_chat_list_iter->second.remove(chat_user);
	}
	//修改群组信息玩家下线
	this->CheckRoleInGroup(role, false);
}

ChatUser * ChatManager::GetChatUser(int uid)
{
	ChatUserListIterator it = m_chat_user_list.find(uid);
	if (it == m_chat_user_list.end())
	{
		return NULL;
	}

	return &it->second;
}

void ChatManager::SetChatUser(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	ChatUser chat_user(this, role->GetUID(), role->GetNetId());

	m_chat_user_list[role->GetUID()] = chat_user;
	m_prof_chat_list[role->GetBaseProfession()].push_back(chat_user);
}

void ChatManager::OnChangeProf(Role * role, int old_prof, int new_prof)
{
	if(NULL == role || old_prof == new_prof) return;
	
	ChatUser chat_user(this, role->GetUID(), role->GetNetId());
	//prof PROF_TYPE_MAX
	//删除原先的职业频道信息
	int old_base_prof = old_prof / PROFESSION_BASE * PROFESSION_BASE;
	int new_base_prof = new_prof / PROFESSION_BASE * PROFESSION_BASE;
	std::map<int, std::list<ChatUser> >::iterator prof_chat_list_iter = m_prof_chat_list.find(old_base_prof);
	if (prof_chat_list_iter != m_prof_chat_list.end())
	{
		prof_chat_list_iter->second.remove(chat_user);
	}
	//添加新职业职业频道聊天信息
	m_prof_chat_list[new_base_prof].push_back(chat_user);

	//群组修改
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
	{
		long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		ChatGroupInfo* info = this->MutableGroupInfo(group_id);
		if (NULL == info) continue;

		ChatGroupUserInfo * member_info = info->GetUserInfo(role->GetUID());
		if (member_info == NULL)continue;
		
		//发往跨服且发往各自成员服务器
		static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_PROF;
		chat_msg.uid = role->GetUID();
		chat_msg.group_id = group_id;
		chat_msg.param1 = new_prof;
		chat_msg.param2 = this->GetRoleBaseAva(role);
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));

		this->OnGetGroupDetailInfo(role, group_id);
		this->OnGroupChangeSendGroupList(group_id);
	}
}

void ChatManager::OnRoleAvaChange(Role * role)
{
	//群组修改
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
	{
		long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		ChatGroupInfo* info = this->MutableGroupInfo(group_id);
		if (NULL == info) continue;

		ChatGroupUserInfo * member_info = info->GetUserInfo(role->GetUID());
		if (member_info == NULL) continue;

		//发往跨服且发往各自成员服务器
		static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_AVA;
		chat_msg.uid = role->GetUID();
		chat_msg.group_id = group_id;
		chat_msg.param1 = this->GetRoleBaseAva(role);
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));

		this->OnGroupChangeSendGroupList(group_id);
	}
}

void ChatManager::OnSpeakerMsg(Role * role, Protocol::SCChannelChat * msg)
{
	if(NULL == role|| NULL == msg) return;

	int total_len = sizeof(Protocol::SCChannelChat) - sizeof(msg->content) + msg->msg_length;
	if (chatdef::CHANNEL_TYPE_COMPLEX == msg->channel_type)
	{
		this->OnComplexChat(role, (const char *)msg, total_len);
	}
	else
	{
		this->OnWorldChat(role, (const char *)msg, total_len);
	}
}

void ChatManager::OnChannelChat(Role *role, void* msg, int length)
{
	Protocol::CSChannelChat *cc = (Protocol::CSChannelChat*)msg;

	if (cc->msg_length + (int)sizeof(Protocol::CSChannelChat) > length) // 检查格式
	{
		gamelog::g_log_debug.printf(LL_DEBUG, "OnChannelChat msg_length:%d is too long.", cc->msg_length);
		return;
	}

	if (cc->msg_length <= 0 || cc->msg_length > MAX_CHAT_LENGTH)
	{
		return;
	}

	ChatUser *user = this->GetChatUser(role->GetUID());
	if (NULL == user)
	{
		return;
	}

	Protocol::SCChannelChat channel_chat;

	channel_chat.from_uid = role->GetUID();
	channel_chat.channel_type = (char)cc->channel_type;
	channel_chat.msg_length = cc->msg_length;
	role->GetName(channel_chat.username);
	channel_chat.avatar_type = role->GetAvatarType();
	channel_chat.prof = static_cast<short>(role->GetProfession());
	channel_chat.headshot_id = role->GetHeadshotID();
	channel_chat.level = (short)role->GetLevel();
	channel_chat.avatar_timestamp = role->GetAvatarTimestamp();
	channel_chat.content_type = cc->content_type;
	channel_chat.top_level = role->GetTopLevel();
	channel_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
	channel_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	channel_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
	channel_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
	channel_chat.msg_type = Protocol::SCChannelChat::CHANNEL_CHAT_MSG_TYPE_BEGIN;	
	channel_chat.group_id = 0;

	Guild* guild = role->GetGuild();
	if (NULL != guild)
	{
		F_STRNCPY(channel_chat.guild_name, guild->GetName(), sizeof(channel_chat.guild_name));
	}
	else
	{
		memset(channel_chat.guild_name, 0, sizeof(channel_chat.guild_name));
	}

	memcpy(channel_chat.content, (char *)msg + sizeof(Protocol::CSChannelChat), cc->msg_length);

	int total_len = sizeof(Protocol::SCChannelChat) - sizeof(channel_chat.content) + cc->msg_length;

	if (cc->channel_type == chatdef::CHANNEL_TYPE_WORLD)
	{
		if (WorldStatus::Instance().IsAnswerTime())//在世界答题活动时间内
		{
			int str_hash = GetStringHash(channel_chat.msg_length, channel_chat.content);
			WorldStatus::Instance().RoleAnswerQuestion(role, str_hash);
		}
	}

	if (!user->ChannelCanChat(role, cc->channel_type))
	{
		return;
	}

	{
		// 通知客户端玩家是否被禁言
		Protocol::SCPlayerIsForbidden pif;
		pif.is_forbidden = user->IsForbidChannel(role) ? 1 : 0;
		pif.forbid_talk_end_timestamp = pif.is_forbidden ? role->GetForbidTalkTime() : 0;
		pif.forbid_talk_type = role->GetForbidTalkType();

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&pif, sizeof(pif));
	}

	if (user->IsForbidChannel(role))
	{
		if (user->IsShowMyTalk(role) || role->GetLevel() < GLOBALCONFIG->GetForbidTalkTipsLevel())
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&channel_chat, total_len);
		}

		return;
	}


	//记录聊天中的物品信息，给客户端点击查询
	for (int i = 0; i < ARRAY_LENGTH(cc->item_info) && i < ARRAY_LENGTH(channel_chat.item_info); ++i)
	{
		ChatMsgItemInfo &item_info = channel_chat.item_info[i];
		item_info.type = 0;
		item_info.param1 = 0;
		item_info.show = 0;
		item_info.show2 = 0;

		ChatItemInfo temp;

		switch (cc->item_info[i].type)
		{
		case CHAT_MSG_ITEM_INFO_TYPE_ITEM:
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(cc->item_info[i].param1, (int)cc->item_info[i].param2, &temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
					if (temp.item.has_param == 1)
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(temp.item.item_id);
						if (item_base != NULL && item_base->GetItemType() == I_TYPE_MEDAL)
						{
							static NetValueItemParam p_nvip; p_nvip.Reset();
							if (temp.item.has_param == 1)
							{
								p_nvip.SetData(temp.item.param_data);
								item_info.show = p_nvip.medal_param.sp_id;
								item_info.show2 = p_nvip.medal_param.second_sp_id;
							}
						}
					}
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_PET:
			{
				AttributeList  attrs;
				if (role->GetRoleModuleManager()->GetPet()->GetPetItemGridWrapData(cc->item_info[i].param1, temp.item, attrs))
				{
					//temp.atts.assign(attrs.m_attrs, attrs.m_attrs + ARRAY_LENGTH(attrs.m_attrs));
					for (int i = 0; i < ARRAY_LENGTH(attrs.m_attrs); i++)
					{
						temp.atts[i] = attrs.m_attrs[i];
					}

					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_EQUIPMENT:
			{
				if (role->GetRoleModuleManager()->GetEquipmentManager()->GetEquipmentGridData(cc->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_JEWELRY:
			{
				if (role->GetRoleModuleManager()->GetEquipmentManager()->GetJewelryGridData(cc->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_MEDAL:
			{
				if (role->GetRoleModuleManager()->GetMedal()->GetMedalGridData(cc->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
					const ItemBase * item_base = ITEMPOOL->GetItem(temp.item.item_id);
					if (item_base != NULL && item_base->GetItemType() == I_TYPE_MEDAL)
					{
						static NetValueItemParam p_nvip; p_nvip.Reset();
						if (temp.item.has_param == 1)
						{
							p_nvip.SetData(temp.item.param_data);
							item_info.show = p_nvip.medal_param.sp_id;
							item_info.show2 = p_nvip.medal_param.second_sp_id;
						}
					}
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_HOLYEQUIP:
			{
				if(role->GetRoleModuleManager()->GetHolyEquipManager()->GetHolyEquipGridData(cc->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		default:
			break;
		}
	}
	switch (cc->channel_type)
	{
		case chatdef::CHANNEL_TYPE_WORLD:			// 世界聊天
		{
			if (!user->IgnoreWorldTalk())
			{
				this->OnWorldChat(role, (const char*)&channel_chat, total_len);
			}
			user->OnWorldChat(cc);	
		}
		break;
		case chatdef::CHANNEL_TYPE_SCENE:			// 当前聊天/场景聊天
		{
			this->OnSceneChat(role, (const char*)&channel_chat, total_len);

		}
		break;
		case chatdef::CHANNEL_TYPE_TEAM:			// 队伍聊天
		{
			this->OnTeamChat(role, (const char*)&channel_chat, total_len);
		}
		break;
		case chatdef::CHANNEL_TYPE_GUILD:			// 家族聊天
		{
			this->OnGuildChat(role, (const char*)&channel_chat, total_len);
		}
		break;
		case chatdef::CHANNEL_TYPE_PROF:			// 职业聊天
		{
			this->OnProfChat(role, (const char*)&channel_chat, total_len);
		}
		break;
		case chatdef::CHANNEL_TYPE_GROUP:			// 群组聊天
		{
			if (TEMP_CHAT_GROUP_ID == cc->group_id)
			{
				this->OnTemporaryGroupChat(role, (const char*)&channel_chat, total_len);
			}
			else if (CrossConfig::Instance().IsHiddenServer())
			{
				CrossChatGroupManager::Instance().OnGroupChat(role, cc->group_id, (const char*)&channel_chat, total_len);
			}
			else
			{
				this->OnGroupChat(role, cc->group_id, (const char*)&channel_chat, total_len);
			}
		}
		break;
		case chatdef::CHANNEL_TYPE_COMPLEX:			// 综合聊天
		{
			this->OnComplexChat(role, (const char*)&channel_chat, total_len);
		}
		break;
	}
	role->GetRoleModuleManager()->OnChannelChat(cc->channel_type);
}

void ChatManager::OnSingleChat(Role *role, void* msg, int length, bool is_force)
{
	if (NULL == role || NULL == msg || length <= 0)
	{
		return;
	}

	Protocol::CSSingleChat *chat_req = (Protocol::CSSingleChat*)msg;
	ChatUser *chat_user = this->GetChatUser(role->GetUID());
	if (NULL == chat_user)
	{
		return;
	}

	{
		// 通知客户端玩家是否被禁言
		Protocol::SCPlayerIsForbidden pif;
		pif.is_forbidden = chat_user->IsForbidChannel(role) ? 1 : 0;
		pif.forbid_talk_end_timestamp = pif.is_forbidden ? role->GetForbidTalkTime() : 0;
		pif.forbid_talk_type = role->GetForbidTalkType();

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&pif, sizeof(pif));
	}

	if (!is_force)
	{
		if (chat_req->to_uid == role->GetUID())		// 检查是不是自己
		{
			role->NoticeNum(errornum::EN_CHAT_CAN_NOT_CHAT_WITH_YOURSELF);
			return;
		}

		if (chat_req->msg_length + (int)sizeof(Protocol::CSSingleChat) > length) // 检查格式
		{
			gamelog::g_log_debug.printf(LL_DEBUG, "OnSingleChat msg_length:%d is too long.", chat_req->msg_length);
			return;
		}

		if (chat_req->msg_length <= 0 || chat_req->msg_length > MAX_CHAT_LENGTH)
		{
			gamelog::g_log_debug.printf(LL_DEBUG, "OnSingleChat msg_length:%d is too long.", chat_req->msg_length);
			return;
		}

		if (role->GetLevel() < GLOBALCONFIG->GetTalkLevelSiLiao())
		{
			role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
			return;
		}

		if (chat_user->IsForbidChannel(role) && !chat_user->IsShowMyTalk(role))  // 被禁言
		{
			gamelog::g_log_forbid.printf(LL_INFO, "IsForbidChannel user[%d %s]", role->GetUID(), role->GetName());
			return;
		}

		//已经拉黑对象.
		if (FriendManager::Instance().IsBlack(role->GetUID(), chat_req->to_uid))
		{
			role->NoticeNum(errornum::EN_SINGLE_CHAT_ROLE_TO_BLACK);
			return;
		}

		if (!chat_user->CanSingleChatLimit(role, chat_req->to_uid))
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_single_chat_is_limit_notice, GLOBALCONFIG->GetSingleChatNumLevel());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			return;
		}
	}

	bool transfer_to_cross = false;	// 是否由跨服转发
	bool send_to_hidden = false;	// 是否发到隐藏服
	bool check_online = true;		// 是否检查在线
	
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		const std::set<int>& serverid_set = LocalConfig::Instance().GetAllowServerIdSet();

		// 如果发送方不在隐藏服，且对方dbindex并非合服中(收发双方不在同一服务器内) 则必须发送到跨服判断
		if (serverid_set.end() == serverid_set.find(IntToUid(chat_req->to_uid).db_index))
		{
			check_online = false;
			transfer_to_cross = true;
		}
	}

	if (check_online)
	{
		int status_type = UserCacheManager::Instance().GetUserOnlineStatus(chat_req->to_uid);
		switch (status_type)
		{
		case UserStatusNode::STATUS_TYPE_OFFLINE:
			{
				// 如果发送方不在隐藏服且接收方不在线 则只需本地发送到聊天记录面板
				if (!CrossConfig::Instance().IsHiddenServer())
				{
					break;
				}
				// 发送方在隐藏服且接收方不在线 则需要发送到跨服(并且明确需要转发到接收方所在服务器
				transfer_to_cross = true;
			}
			break;

		case UserStatusNode::STATUS_TYPE_CROSS:
			{
				// 如果接收方在跨服，发送方也在跨服则只需本地发送
				// 如果接收方在跨服，发送方不在跨服，那必然需要通过跨服转发到隐藏服
				if (!CrossConfig::Instance().IsHiddenServer())
				{
					transfer_to_cross = true;
					send_to_hidden = true;
				}
			}
			break;

		case UserStatusNode::STATUS_TYPE_ONLINE:
			{
				// 接收方在原服但发送方在跨服，就要转到跨服
				if (CrossConfig::Instance().IsHiddenServer())
				{
					transfer_to_cross = true;
				}
			}
			break;
		}
	}

	Protocol::SCSingleChat single_chat;
	single_chat.from_uid = role->GetUID();
	single_chat.to_uid = chat_req->to_uid;
	single_chat.msg_length = chat_req->msg_length;
	role->GetName(single_chat.username);
	single_chat.avatar_type = role->GetAvatarType();
	single_chat.prof = role->GetBaseProfession();
	single_chat.headshot_id = role->GetHeadshotID();
	single_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
	single_chat.authority_type = role->GetAuthority();
	single_chat.level = (short)role->GetLevel();
	single_chat.top_level = role->GetTopLevel();
	single_chat.content_type = chat_req->content_type;
	single_chat.avatar_timestamp = role->GetAvatarTimestamp();
	single_chat.msg_timestamp = (unsigned int)EngineAdapter::Instance().Time();
	memcpy(single_chat.content, (char *)msg + sizeof(Protocol::CSSingleChat), chat_req->msg_length);
	single_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
	single_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
	
	int single_chat_real_total_len = UNSTD_OFFSETOFF(Protocol::SCSingleChat, content) + chat_req->msg_length;

	//记录聊天中的物品信息，给客户端点击查询
	for (int i = 0; i < ARRAY_LENGTH(chat_req->item_info) && i < ARRAY_LENGTH(single_chat.item_info); ++i)
	{
		ChatMsgItemInfo & item_info = single_chat.item_info[i];
		item_info.type = 0;
		item_info.param1 = 0;
		item_info.param2 = 0;
		item_info.show = 0;
		item_info.show2 = 0;

		ChatItemInfo temp;

		switch (chat_req->item_info[i].type)
		{
		case CHAT_MSG_ITEM_INFO_TYPE_ITEM:
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(chat_req->item_info[i].param1, (int)chat_req->item_info[i].param2, &temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
					if (temp.item.has_param == 1)
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(temp.item.item_id);
						if (item_base != NULL && item_base->GetItemType() == I_TYPE_MEDAL)
						{
							static NetValueItemParam p_nvip; p_nvip.Reset();
							if (temp.item.has_param == 1)
							{
								p_nvip.SetData(temp.item.param_data);
								item_info.show = p_nvip.medal_param.sp_id;
								item_info.show2 = p_nvip.medal_param.second_sp_id;
							}
						}
					}
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_PET:
			{
				AttributeList  attrs;
				if (role->GetRoleModuleManager()->GetPet()->GetPetItemGridWrapData(chat_req->item_info[i].param1, temp.item, attrs))
				{
					//temp.atts.assign(attrs.m_attrs, attrs.m_attrs + ARRAY_LENGTH(attrs.m_attrs));
					for (int i = 0; i < ARRAY_LENGTH(attrs.m_attrs); i++)
					{
						temp.atts[i] = attrs.m_attrs[i];
					}

					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_EQUIPMENT:
			{
				if (role->GetRoleModuleManager()->GetEquipmentManager()->GetEquipmentGridData(chat_req->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_JEWELRY:
			{
				if (role->GetRoleModuleManager()->GetEquipmentManager()->GetJewelryGridData(chat_req->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
				}
			}
			break;
		case CHAT_MSG_ITEM_INFO_TYPE_MEDAL:
			{
				if (role->GetRoleModuleManager()->GetMedal()->GetMedalGridData(chat_req->item_info[i].param1, temp.item))
				{
					item_info.param1 = temp.item.item_id;
					item_info.param2 = this->AddChatItemInfo(temp);
					const ItemBase * item_base = ITEMPOOL->GetItem(temp.item.item_id);
					if (item_base != NULL && item_base->GetItemType() == I_TYPE_MEDAL)
					{
						static NetValueItemParam p_nvip; p_nvip.Reset();
						if (temp.item.has_param == 1)
						{
							p_nvip.SetData(temp.item.param_data);
							item_info.show = p_nvip.medal_param.sp_id;
							item_info.show2 = p_nvip.medal_param.second_sp_id;
						}
					}
				}
			}
			break;
		default:
			break;
		}
	}

	//禁言判断
	if (!chat_user->IsForbidChannel(role) ) 
	{
		if (!transfer_to_cross)
		{
			UserID to_user = IntToUid(chat_req->to_uid);

			if (to_user.db_index > 0)
			{
				Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(chat_req->to_uid));
				if (to_role != NULL)
				{
					if (FriendManager::Instance().IsBlack(to_role->GetUID() , role->GetUID()))
					{
						role->NoticeNum(errornum::EN_YOU_ARE_IN_BLACKLISTS);
						return;
					}

					EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&single_chat, single_chat_real_total_len);
			
					single_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
					this->AddSingleChatBoard(chat_req->to_uid, (const char *)&single_chat, single_chat_real_total_len); // 在线也加到聊天板

				}
				else
				{
					gamelog::g_log_debug.printf(LL_DEBUG, "OnSignleChat Can NOT find to_user:%d.", chat_req->to_uid);

					// 离线的话发到私聊板，上线就能看到
					Protocol::SCSingleChatUserNotExist scune;
					scune.to_uid = chat_req->to_uid;
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&scune, sizeof(Protocol::SCSingleChatUserNotExist));

					single_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
					this->AddSingleChatBoard(chat_req->to_uid, (const char *)&single_chat, single_chat_real_total_len);
				}	

				this->RecordUserSingleChat(chat_req->to_uid, single_chat.from_uid, (const char *)&single_chat, single_chat_real_total_len);
			}
		}
		else
		{
			crossgameprotocal::G2GRoleChatTransfer g2grct;
			g2grct.plat_type = LocalConfig::Instance().GetPlatType();
			g2grct.to_gameserver_id = IntToUid(chat_req->to_uid).db_index;
			g2grct.to_role_id = chat_req->to_uid;		
			g2grct.send_to_hidden_first = send_to_hidden ? 1 : 0;
			g2grct.Push(&single_chat, single_chat_real_total_len);

			size_t send_len = sizeof(crossgameprotocal::G2GRoleChatTransfer) - sizeof(g2grct.data) + g2grct.data_size;
			InternalComm::Instance().SendToCross((char *)&g2grct, send_len);
		}
		FriendIntimacyManager::Instance().OnSingleChat(role->GetUID() ,chat_req->to_uid ,(unsigned int)EngineAdapter::Instance().Time() , __FUNCTION__);
	}

	FriendManager::Instance().AddRecentlyChat(role, chat_req->to_uid);
	chat_user->OnSingleChat(chat_req);

	this->RecordUserSingleChat(role->GetUID(), chat_req->to_uid, (const char *)&single_chat, single_chat_real_total_len);
}

void ChatManager::OnSingleChat(crossgameprotocal::G2GRoleChatTransfer *protocal)
{
	Protocol::SCSingleChat single_chat;
	if (protocal->data_size > sizeof(single_chat) || !protocal->Pop(&single_chat, protocal->data_size))
	{
		return;
	}

	int single_chat_real_total_len = protocal->data_size;

	UserID to_user = IntToUid(protocal->to_role_id);
	if (to_user.db_index > 0)
	{
		Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(to_user);
		if (to_role != NULL)
		{
			if (FriendManager::Instance().IsBlack(to_role->GetUID(), single_chat.from_uid))
			{
				return;
			}

			EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&single_chat, single_chat_real_total_len);

			single_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			this->AddSingleChatBoard(protocal->to_role_id, (const char *)&single_chat, single_chat_real_total_len); // 在线也加到聊天板
		
			this->RecordUserSingleChat(protocal->to_role_id, single_chat.from_uid, (const char *)&single_chat, single_chat_real_total_len);
		}
		else if (CrossConfig::Instance().IsHiddenServer() && protocal->send_to_hidden_first)
		{
			// 隐藏服找不到该玩家，则发到玩家所在的服
			crossgameprotocal::G2GRoleChatTransfer g2grct;
			g2grct.plat_type = protocal->plat_type;
			g2grct.to_gameserver_id = protocal->to_gameserver_id;
			g2grct.to_role_id = protocal->to_role_id;
			g2grct.send_to_hidden_first = 0; // 这里必须写0，否则这条信息可能永远都在跨服与隐藏服之间转发
			g2grct.Push(&single_chat, single_chat_real_total_len);

			size_t send_len = sizeof(crossgameprotocal::G2GRoleChatTransfer) - sizeof(g2grct.data) + g2grct.data_size;
			InternalComm::Instance().SendToCross((char *)&g2grct, send_len);
		}
		else
		{
			gamelog::g_log_debug.printf(LL_DEBUG, "OnSignleChat Can NOT find to_user:%d.", protocal->to_role_id);

			single_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			this->AddSingleChatBoard(protocal->to_role_id, (const char *)&single_chat, single_chat_real_total_len); 
		
			this->RecordUserSingleChat(protocal->to_role_id, single_chat.from_uid, (const char *)&single_chat, single_chat_real_total_len);
		}
	}
}

void ChatManager::SystemSendRoleChannelChat(Role* role, int channel, const char* content, int length, bool is_team_info)
{
	if (length >= MAX_CHAT_LENGTH)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	Protocol::SCChannelChat channel_chat;

	channel_chat.from_uid = role->GetUID();
	channel_chat.channel_type = (char)channel;
	channel_chat.msg_length = length;
	role->GetName(channel_chat.username);
	channel_chat.avatar_type = role->GetAvatarType();
	channel_chat.prof = static_cast<short>(role->GetProfession());
	channel_chat.headshot_id = role->GetHeadshotID();
	channel_chat.level = (short)role->GetLevel();
	channel_chat.avatar_timestamp = role->GetAvatarTimestamp();
	channel_chat.top_level = role->GetTopLevel();
	channel_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
	channel_chat.content_type = Protocol::CHAT_CONTENT_TYPE_TEXT;
	channel_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	channel_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
	channel_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
	channel_chat.msg_type = is_team_info ? Protocol::SCChannelChat::CHANNEL_CHAT_MSG_TYPE_TEAM : Protocol::SCChannelChat::CHANNEL_CHAT_MSG_TYPE_BEGIN;	//如果是组队信息则为true(当前只有组队招募)

	Guild* guild = role->GetGuild();
	if (NULL != guild)
	{
		F_STRNCPY(channel_chat.guild_name, guild->GetName(), sizeof(channel_chat.guild_name));
	}
	else
	{
		memset(channel_chat.guild_name, 0, sizeof(channel_chat.guild_name));
	}

	memcpy(channel_chat.content, content, length);

	int total_len = sizeof(Protocol::SCChannelChat) - sizeof(channel_chat.content) + length * sizeof(channel_chat.content[0]);

	if (channel == chatdef::CHANNEL_TYPE_WORLD)	// 世界聊天
	{
		this->OnWorldChat(role, (const char*)&channel_chat, total_len);
		return;
	}
	else if (channel == chatdef::CHANNEL_TYPE_SCENE)
	{
		this->OnSceneChat(role, (const char*)&channel_chat, total_len);
		return;
	}
	else if (channel == chatdef::CHANNEL_TYPE_TEAM)
	{
		this->OnTeamChat(role, (const char*)&channel_chat, total_len);
		return;
	}
	else if (channel == chatdef::CHANNEL_TYPE_GUILD)
	{
		this->OnGuildChat(role, (const char*)&channel_chat, total_len);
		return;
	} 
	else if (channel == chatdef::CHANNEL_TYPE_PROF)
	{
		this->OnProfChat(role, (const char*)&channel_chat, total_len);
		return;
	}
	else if (channel == chatdef::CHANNEL_TYPE_COMPLEX)
	{
		this->OnComplexChat(role, (const char*)&channel_chat, total_len);
		return;
	}
}

void ChatManager::SendChatItemInfo(Role * role, long long item_info_seq)
{
	if (role == NULL)
	{
		return; 
	}
	
	ChatItemUniqueSeq tmp;
	tmp.unique_seq = item_info_seq;

	if (tmp.server_id != LocalConfig::Instance().GetMergeServerId())
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameChatItemInfoReq req;
			req.role_id = role->GetUID();
			req.unique_item_seq = item_info_seq;

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), tmp.server_id, (const char *)&req, sizeof(req));
		}
		else
		{
			crossgameprotocal::GameHiddenChatItemInfoReq req;
			req.role_id = role->GetUID();
			req.unique_item_seq = item_info_seq;

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));
		}
		
		return;
	}
	std::map<long long, ChatItemInfo>::const_iterator iter = m_chat_item_info.find(item_info_seq);
	if (iter == m_chat_item_info.end())
	{
		role->NoticeNum(errornum::EN_CHAT_ITEM_INFO_NOT_EXIST);
		return;
	}
	const ChatItemInfo  & info = iter->second;
	const ItemDataWrapper & item_data_wrapper = info.item;

	Protocol::SCChatItemInfoRet  protocol;
	protocol.item_info_seq = item_info_seq;
	protocol.item_id = item_data_wrapper.item_id;
	protocol.is_bind = (char)item_data_wrapper.is_bind;
	protocol.num = item_data_wrapper.num;
	protocol.has_param = false;
	protocol.invalid_time = item_data_wrapper.invalid_time;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data_wrapper.item_id);
	if (item_data_wrapper.has_param && NULL != item_base) // 只发送有参数的
	{
		protocol.has_param = true;
		protocol.param_length = ((NetValueItemParam *)(&item_data_wrapper.param_data))->GetDataByItemType(item_base->GetItemType(), protocol.param_data);

		if (item_base->GetItemType() == I_TYPE_PET)
		{
			protocol.has_attrs = 1;
		}
		for (int i = 0; i < ARRAY_LENGTH(protocol.attrs); ++i)
		{
			protocol.attrs[i] = info.atts[i];
		}
	}
	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &protocol);
}

void ChatManager::SendChatItemInfo2(int role_id, long long item_info_seq, bool req_from_hidden)
{
	ChatItemUniqueSeq tmp;
	tmp.unique_seq = item_info_seq;


	std::map<long long, ChatItemInfo>::const_iterator iter = m_chat_item_info.find(item_info_seq);
	if (iter == m_chat_item_info.end())
	{
		Protocol::SCNoticeNum err_msg;
		err_msg.notice_num = errornum::EN_CHAT_ITEM_INFO_LOST;
		if (req_from_hidden) 
		{
			UniqueUserID user_uuid;
			user_uuid.plat_type = LocalConfig::Instance().GetPlatType();
			user_uuid.user_id = IntToUid(role_id);
			InternalComm::Instance().SendToHiddenThroughCross((const char *)&err_msg, sizeof(err_msg), UniqueUserIDToLongLong(user_uuid));
		}
		else
		{
			InternalComm::Instance().SendToGameUserThroughCross(role_id, &err_msg, sizeof(err_msg));
		}
		return;
	}

	const ChatItemInfo & info = iter->second;
	const ItemDataWrapper & item_data_wrapper = info.item;


	Protocol::SCChatItemInfoRet  protocol;
	protocol.item_info_seq = item_info_seq;
	protocol.item_id = item_data_wrapper.item_id;
	protocol.is_bind = (char)item_data_wrapper.is_bind;
	protocol.num = item_data_wrapper.num;
	protocol.has_param = false;
	protocol.invalid_time = item_data_wrapper.invalid_time;
	protocol.has_attrs = false;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data_wrapper.item_id);
	if (item_data_wrapper.has_param && NULL != item_base) // 只发送有参数的
	{
		protocol.has_param = true;
		protocol.param_length = ((NetValueItemParam *)(&item_data_wrapper.param_data))->GetDataByItemType(item_base->GetItemType(), protocol.param_data);

		if (item_base->GetItemType() == I_TYPE_PET)
		{
			protocol.has_attrs = 1;
		}
		for (int i = 0; i < ARRAY_LENGTH(protocol.attrs); ++i)
		{
			protocol.attrs[i] = info.atts[i];
		}
	}

	if (req_from_hidden)
	{
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&protocol, UniqueUserIDToLongLong(UniqueUserID(LocalConfig::Instance().GetPlatType(), role_id)));
	}
	else
	{
		InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(role_id, &protocol);
	}	
}

void ChatManager::ChatItemInfoSyn(long long key, ChatItemInfo * info)
{
	std::map<long long, ChatItemInfo>::iterator it = m_chat_item_info.find(key);
	if (it != m_chat_item_info.end())
	{
		return;
	}

	ChatItemInfo &node = m_chat_item_info[key];

	for (int i = 0; i < ARRAY_LENGTH(node.atts) && i < ARRAY_LENGTH(info->atts); ++i)
	{
		node.atts[i] = info->atts[i];
	}
	node.item = info->item;
}

void ChatManager::AddSingleChat(int uid, const char * msg, int msg_len)
{
	this->AddSingleChatBoard(uid,msg,msg_len);
}

void ChatManager::OnGuildChatt(Role * role, const char * buff, int total_len)
{
	this->OnGuildChat(role,buff,total_len);
}

void ChatManager::OnGuildChatt(UserID userid, const char * buff, int total_len)
{
	this->OnGuildChat(userid, buff, total_len);
}


void ChatManager::OnProfChat(Role * role, const char * buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;

	this->SendProfChannel(role->GetBaseProfession(), buff, total_len);
}

void ChatManager::SendProfChannel(int prof, const char * buff, int total_len)
{
	if (NULL == buff || total_len <= 0) return;

	std::map<int, std::list<ChatUser> >::iterator prof_iter = m_prof_chat_list.find(GetBaseProfession(prof));
	if (prof_iter == m_prof_chat_list.end())
	{
		return;
	}

	for (std::list<ChatUser>::iterator list_iter = prof_iter->second.begin(); list_iter != prof_iter->second.end(); ++list_iter)
	{
		EngineAdapter::Instance().NetSend(list_iter->GetNetId(), buff, total_len);
	}
}

void ChatManager::OnWorldChat(Role *role, const char* buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;
	World::GetInstWorld()->SendToAllGateway((const char*)buff, total_len);
}

void ChatManager::OnSceneChat(Role *role, const char* buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;

	Scene *scene = role->GetScene();
	if (NULL == scene)
	{
		return;
	}

	int net_num = 0;

	int role_num = scene->RoleNum();
	for (int i = 0; i < role_num; ++i)
	{
		if (net_num >= MSX_GSNETID_NUM)
		{
			break;
		}

		Role *tmp_role = scene->GetRoleByIndex(i);
		if (NULL != tmp_role)
		{
			gsnetidlist[net_num++] = tmp_role->GetNetId();
		}
	}

	if (net_num > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetidlist, net_num, buff, total_len);
	}
}

void ChatManager::OnTeamChat(Role *role, const char* buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;

	if (!role->InTeam())
	{
		return;
	}

	Team *team = role->GetMyTeam();
	if (NULL == team)
	{
		return;
	}

	int net_num = 0;

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* tmp_role = team->GetMemberRoleByIndex(i);
		if (NULL != tmp_role)
		{
			gsnetidlist[net_num++] = tmp_role->GetNetId();
		}
	}

	if (net_num > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetidlist, net_num, buff, total_len);
	}
}

void ChatManager::OnGuildChat(Role *role, const char* buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;


	Guild *guild = GuildManager::Instance().GetGuild(role->GetGuildID());
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManagerRef().OnChat(buff, total_len, true);
}

void ChatManager::OnGuildChat(UserID userid, const char * buff, int total_len)
{
	if (NULL == buff || total_len <= 0) return;


	Guild *guild = GuildManager::Instance().GetGuild(userid);
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManagerRef().OnChat(buff, total_len, true);
}

void ChatManager::OnGroupChat(Role * role, long long group_id, const char * buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	ChatGroupUserInfo * member_info = info->GetUserInfo(role->GetUID());
	if (member_info == NULL)return;

	//发言标记玩家在线
	member_info->member_logout_tamp = 0;

	crossgameprotocal::GameHiddenSendChatGroup* chat_msg = (crossgameprotocal::GameHiddenSendChatGroup*)m_buffer;
	memset(m_buffer, 0, MAX_MSG_SIZE);
	chat_msg->header.msg_type = crossgameprotocal::MT_GAME_HIDDEN_CHAT_GROUP;
	chat_msg->group_id = group_id;
	chat_msg->main_server_id = LocalConfig::Instance().GetMergeServerId();
	memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenSendChatGroup), buff, total_len);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)m_buffer, sizeof(crossgameprotocal::GameHiddenSendChatGroup) + total_len);//发往隐藏服，之后在进一步操作

	this->SendToGroup(group_id, (void *)buff, total_len);
}

void ChatManager::OnComplexChat(Role * role, const char * buff, int total_len)
{
	if(NULL == role || NULL == buff || total_len <= 0) return;
	//Protocol::SCChannelChat * msg_info = (Protocol::SCChannelChat *)buff;
	//跨服没开则发到本地
	if (!CrossManager::Instance().IsCrossServerConnected())
	{
		World::GetInstWorld()->SendToAllGateway(buff, total_len);
	}
	else //跨服开了则发到所有连接到跨服的游戏服
	{
		InternalComm::Instance().SendToAllGameAllUserThroughCross(buff, total_len);
	}
}

void ChatManager::OnTemporaryGroupChat(Role* role, const char* buff, int total_len)
{
	if (NULL == role || NULL == buff) return;

	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	team->OnTempGroupChat(buff, total_len);
}

void ChatManager::AddSingleChatBoard(int uid, const char *msg, int msg_len)
{
	if (NULL == msg || msg_len <= 0) 
	{
		return;
	}

	/*
	UserCacheNode *user_node = UserCacheManager::Instance().GetUserNode(uid);
	if (NULL == user_node)
	{
		return;
	}

	MsgBoardMapIterator board_it = m_single_chat_board_map.find(uid);
	if (m_single_chat_board_map.end() == board_it)
	{
		MsgBoardItemQueue tmp_queue;
		m_single_chat_board_map[uid] = tmp_queue;

		board_it = m_single_chat_board_map.find(uid);
		if (m_single_chat_board_map.end() == board_it)
		{
			return;
		}
	}

	const static unsigned int MAX_MSG_BOARD_ITEM_COUNT = 60;

	MsgBoardItemQueue &msg_item_queue = board_it->second;
	if (msg_item_queue.size() >= MAX_MSG_BOARD_ITEM_COUNT)
	{
		msg_item_queue.pop_front();
	}

	MsgBoardItem msg_board_item;
	msg_board_item.SetMsg(msg, msg_len);
	msg_item_queue.push_back(msg_board_item);
	*/
}

void ChatManager::CheckSingleChatBoard(Role *role)
{
	if (NULL == role)
	{
		return;
	}
	
	MsgBoardMapIterator board_it = m_single_chat_board_map.find(role->GetUID());
	if (m_single_chat_board_map.end() == board_it)
	{
		return;
	}

	MsgBoardItemQueue &msg_item_queue = board_it->second;

	for (MsgBoardItemQueueIterator it = msg_item_queue.begin(); msg_item_queue.end() != it;)
	{
		int send_len = 0;
		const char *send_buffer = it->GetSendBuffer(&send_len);
		
		int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::SCSingleChat, msg_length));
		if (send_len >= offset)
		{
			Protocol::SCSingleChat *single_chat = (Protocol::SCSingleChat*)send_buffer;
			if (FriendManager::Instance().IsBlack(role->GetUID(), single_chat->from_uid))
			{
				++it;
				continue;
			}

			if (MutePlayerRecorder::Instance().IsMute(single_chat->from_uid))
			{
				it = msg_item_queue.erase(it);
				continue;
			}

			FriendManager::Instance().AddRecentlyChat(role, single_chat->from_uid);
		}

		if (send_len > 0 && NULL != send_buffer)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), send_buffer, send_len);
		}

		++it;
	}

	m_single_chat_board_map.erase(board_it);
}

void ChatManager::AddGuildChatBoard(int guild_id, const char *msg, int msg_len)
{
	if (NULL == msg || msg_len <= 0)
	{
		return;
	}

	MsgBoardMapIterator board_it = m_guild_chat_board_map.find(guild_id);
	if (m_guild_chat_board_map.end() == board_it)
	{
		MsgBoardItemQueue tmp_queue;
		m_guild_chat_board_map[guild_id] = tmp_queue;

		board_it = m_guild_chat_board_map.find(guild_id);
		if (m_guild_chat_board_map.end() == board_it)
		{
			return;
		}
	}

	const static unsigned int MAX_MSG_BOARD_ITEM_COUNT = 30;

	MsgBoardItemQueue &msg_item_queue = board_it->second;
	if (msg_item_queue.size() >= MAX_MSG_BOARD_ITEM_COUNT)
	{
		msg_item_queue.pop_front();
	}

	MsgBoardItem msg_board_item;
	msg_board_item.SetMsg(msg, msg_len);
	msg_item_queue.push_back(msg_board_item);
}

void ChatManager::CheckGuildChatBoard(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	MsgBoardMapIterator board_it = m_guild_chat_board_map.find(role->GetGuildID());
	if (m_guild_chat_board_map.end() == board_it)
	{
		return;
	}

	MsgBoardItemQueue &msg_item_queue = board_it->second;

	for (MsgBoardItemQueueIterator it = msg_item_queue.begin(); msg_item_queue.end() != it; )
	{
		int send_len = 0;
		const char *send_buffer = it->GetSendBuffer(&send_len);

		int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::SCChannelChat, msg_length));
		if (send_len >= offset)
		{
			Protocol::SCChannelChat *channel_chat = (Protocol::SCChannelChat*)send_buffer;
			if (MutePlayerRecorder::Instance().IsMute(channel_chat->from_uid))
			{
				it = msg_item_queue.erase(it);
				continue;
			}
		}

		if (send_len > 0 && NULL != send_buffer)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), send_buffer, send_len);
		}

		++it;
	}
}

long long ChatManager::AddChatItemInfo(const ChatItemInfo & chat_item_info)
{
	ChatItemUniqueSeq tmp;

	tmp.server_id = LocalConfig::Instance().GetMergeServerId();
	tmp.seq = chat_item_info_curr_seq++;
	tmp.time = (unsigned int)EngineAdapter::Instance().Time();


	m_chat_item_info[tmp.unique_seq] = chat_item_info;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		//本地服道具信息发往跨服一份
		crossgameprotocal::GameHiddenChatItemSyn protocol;

		protocol.item_info_seq = tmp.unique_seq;
		protocol.item_id = chat_item_info.item.item_id;
		protocol.is_bind = (char)chat_item_info.item.is_bind;
		protocol.num = chat_item_info.item.num;
		protocol.has_param = false;
		protocol.invalid_time = chat_item_info.item.invalid_time;

		const ItemBase * item_base = ITEMPOOL->GetItem(chat_item_info.item.item_id);
		if (chat_item_info.item.has_param && NULL != item_base) // 只发送有参数的
		{
			protocol.has_param = true;
			protocol.param_length = ((NetValueItemParam *)(chat_item_info.item.param_data))->GetDataByItemType(item_base->GetItemType(), protocol.param_data);
		}

		{
			protocol.has_attrs = true;
			for (int i = 0; i < ARRAY_LENGTH(protocol.attrs); ++i)
			{
				protocol.attrs[i] = chat_item_info.atts[i];
			}
		}

		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&protocol);
	}

	return tmp.unique_seq;
}

ChatGroupInfo* ChatManager::MutableGroupInfo(long long group_id)
{
	std::map<long long, ChatGroupInfo>::iterator it = m_cross_chat_group_info.find(group_id);
	if (it != m_cross_chat_group_info.end())
	{
		return &it->second;
	}

	return NULL;
}

void ChatManager::EraseGroup(long long group_id)
{
	m_cross_chat_group_info.erase(group_id);
}

UserChatRecorder* ChatManager::MutableUserChatRecorder(int uid)
{
	UserChatRecorderMap::iterator it = m_user_chat_recorder_map.find(uid);
	if (it != m_user_chat_recorder_map.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}

void ChatManager::CreateUserChatRecoder(int uid)
{
	UserChatRecorder ucr;
	m_user_chat_recorder_map.insert(UserChatRecorderMap::value_type(uid, ucr));
}

void ChatManager::RecordUserSingleChat(int uid, int chat_with_uid, const char* msg, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		UserChatRecorder* ucr = this->MutableUserChatRecorder(uid);
		if (NULL == ucr)
		{
			this->CreateUserChatRecoder(uid);
			ucr = this->MutableUserChatRecorder(uid);
		}

		if (NULL == ucr) return;

		ucr->AddSingleChatRecord(chat_with_uid, msg, length);
	}
	else
	{
		// 发往原服落地
		crossgameprotocal::HiddenGameAddSingleChatRecord hgascr;
		UNSTD_STATIC_CHECK(sizeof(hgascr.msg) >= sizeof(Protocol::SCSingleChat));
		memcpy(hgascr.msg, msg, GetMin(length, (int)sizeof(hgascr.msg)));
		hgascr.msg_length = length;
		hgascr.chat_with_uid = chat_with_uid;
		hgascr.uid = uid;

		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&hgascr, IntToUid(uid).db_index);
	}	
}

void ChatManager::InitChatGroup(int count, const ChatGroupInfo info_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (CrossConfig::Instance().IsHiddenServer())//跨服群组存在CrossChatGroupManager
	{
		ChatManager::Instance().LoadDataSucc();
		return;
	}
}

void ChatManager::Update(time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer()) return; //跨服群组存在CrossChatGroupManager

	if (CHAT_GROUP_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	if (now_second > m_last_save_time + SAVE_GROUP_DATA_INTERVAL)
	{
		this->SaveChatGroup(-1);
		m_last_save_time = (unsigned int)now_second;
	}

	if (chat_item_last_checkout_tamp < now_second)
	{
		for (std::map<long long, ChatItemInfo>::iterator it = m_chat_item_info.begin(); it != m_chat_item_info.end();)
		{
			ChatItemUniqueSeq tmp;
			tmp.unique_seq = it->first;
			if (tmp.time + CHAT_ITEM_CHECK_SAVE_TIME < now_second)
			{
				m_chat_item_info.erase(it++);
			}
			else
			{
				it++;
			}
		}
		chat_item_last_checkout_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SAVE_GROUP_DATA_INTERVAL);
	}
}

void ChatManager::SaveChatGroup(long long last_save_id)
{
	//跨服群组在CrossChatGroupManager保存，而原服不再保存本地群组
	return;
}

void RMISaveChatGroupBackObjectImpl::SaveChatGroupRet(int ret)
{
	if (ret < 0)
	{
		return;
	}

	ChatManager::Instance().ClearDirtyMark(m_last_save_group_id);
	ChatManager::Instance().SaveChatGroup(m_last_save_group_id);
}

void RMIInitChatGroupBackObjectImpl::InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (CrossConfig::Instance().IsHiddenServer() ||count <= 0)
	{	
		ChatManager::Instance().LoadDataSucc();
		return;
	}

	ChatManager::Instance().InitChatGroup(count, param_list);
	ChatManager::Instance().LoadData(last_id);
}

void ChatManager::LoadData(long long from_id)
{
	// 不再使用原服群组，就不需要读原服群组数据了
	ChatManager::Instance().LoadDataSucc();
	//RMIInitChatGroupBackObjectImpl *backobj = new RMIInitChatGroupBackObjectImpl();
	//RMIGlobalClient gc;
	//gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	//gc.InitChatGroupAsyn(from_id, backobj);
}

void ChatManager::MarkDirty(long long group_id)
{
	m_dirty_mark[group_id] = true;
	m_is_change = true;
}

int ChatManager::GetRoleBaseAva(Role * role)
{
	if (role == NULL)return 0;
	if (role->GetHeadshotID() >= 1000)
	{
		return role->GetHeadshotID();
	}
	else
	{
		return role->GetAvatarType();
	}
}

void ChatManager::SetChatRecrodList(Role * role, long long chat_list[CHAT_CORD_MAX_LIST])
{
	if (role == 0)return;
	Protocol::SCChatRecordList msg;

	for (int i = 0; i < CHAT_CORD_MAX_LIST; i++)
	{
		role->GetRoleModuleManager()->GetCommonData().chat_id_list[i] = chat_list[i];
		msg.recent_chat_list[i] = role->GetRoleModuleManager()->GetCommonData().chat_id_list[i];
	}
	role->GetRoleModuleManager()->NetSend((const char*)&msg, sizeof(msg));
}

void ChatManager::SendChatRecrodList(Role * role)
{
	if (role == 0)return;
	Protocol::SCChatRecordList msg;
	
	for (int i = 0; i < CHAT_CORD_MAX_LIST; i++)
	{
		msg.recent_chat_list[i] = role->GetRoleModuleManager()->GetCommonData().chat_id_list[i];
	}
	role->GetRoleModuleManager()->NetSend((const char*)&msg, sizeof(msg));
}

void ChatManager::SendSimpleRecord(Role * role)
{
	if (NULL == role)return;
	CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();
	Protocol::SCChatSimpleRecordInfo info;
	memcpy(info.simple_data_list, common_data.simple_record_list, sizeof(info.simple_data_list));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void ChatManager::SetSimpleRecord(Role * role, int index, GameName data)
{
	if (index <= 0 || index > QUICK_SPEECH_MAX_NUM)return;
	CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();
	memcpy(common_data.simple_record_list[index - 1], data, sizeof(GameName));
	this->SendSimpleRecord(role);
}

void ChatManager::DelayQueryUserChatRecord(int uid)
{
	// 由于客户端会在收到4052后删掉聊天内容，而4052需要从跨服发到原服，所以这里延迟2秒

	DelayQueryUserChatRecordTimer* timmer = new DelayQueryUserChatRecordTimer(uid);
	EngineAdapter::Instance().CreateTimer(2000, timmer);
}

void ChatManager::QueryUserChatRecord(Role* role)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		UserChatRecorder* ucr = this->MutableUserChatRecorder(role->GetUID());
		if (NULL != ucr)
		{
			ucr->OnRoleFetchRecentChat(role);
		}
	}
	else
	{
		// 从原服拉取聊天记录
		crossgameprotocal::HiddenGameQueryUserChatRecord msg;
		UserChatRecorder::AssembleChatWithUidSetByRole(role, msg.chat_with_uid_set);
		msg.uid = role->GetUID();
		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&msg);
	}
}

void ChatManager::OnHiddenGameAddSingleChatRecord(crossgameprotocal::HiddenGameAddSingleChatRecord* msg)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	this->RecordUserSingleChat(msg->uid, msg->chat_with_uid, msg->msg, msg->msg_length);
}

void ChatManager::OnHiddenGameQueryUserChatRecord(crossgameprotocal::HiddenGameQueryUserChatRecord* msg)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	UserChatRecorder* ucr = this->MutableUserChatRecorder(msg->uid);
	if (NULL != ucr)
	{
		ucr->OnUserFetchRecentChatFromHidden(msg->uid, msg->chat_with_uid_set);
	}
}

void ChatManager::ClearDirtyMark(long long last_save_id)
{
	// 回调时清除写入成功的部分，未写入成功的留到下一次尝试
	std::map<long long, bool>::iterator last_it = m_dirty_mark.find(last_save_id);
	if (m_dirty_mark.end() == last_it)
	{
		return;
	}

	//m_old_state.clear();
	for (std::map<long long, bool>::iterator it = m_dirty_mark.begin(); it != m_dirty_mark.end() && it->first != last_save_id;)
	{
		m_old_state[it->first] = true;
		m_dirty_mark.erase(it++);
	}
	//还有最后一个没清除掉
	m_old_state[last_it->first] = true;
	m_dirty_mark.erase(last_it);
}

void ChatManager::OnUserLoginByHide(long long group_id, int uid)
{
	//SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	//Role * login_role = scene_manager->GetRole(IntToUid(uid));

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{		
		//跨服返回群组了，但是本服依旧没有，代表这个群组已经不在了
		/* 原服不做清空group_id处理
		if (NULL != login_role)
		{
			for (int login_idx = 0; login_idx < MAX_ROLE_CROSS_GROUP; login_idx++)
			{
				long long &or_group_id = login_role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[login_idx];
				if (or_group_id == group_id)
				{
					gamelog::g_log_debug.printf(LL_INFO, "ChatManager::OnUserLoginByHide role[%d,%s] group_id[%lld] not found",
						login_role->GetUID(), login_role->GetName(), group_id);
					or_group_id = 0;
				}
			}
		}
		*/
		return;
	}

	//if (NULL != login_role && info->is_cross == 1)
	//{
	//	this->SendRoleOneGroupInfo(login_role, group_id);
	//}

	ChatGroupUserInfo * member_info = info->GetUserInfo(uid);
	if (member_info == NULL)
	{
		return;
	}
	member_info->member_logout_tamp = 0;
}

void ChatManager::OnUserLogoutByHide(long long group_id, int uid)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	ChatGroupUserInfo * member_info = info->GetUserInfo(uid);
	if (member_info == NULL)
	{
		return;
	}
	member_info->member_logout_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void ChatManager::OnHidenServerBack(int count, const ChatGroupInfo info_list[crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST])
{
	if (count <= 0)
	{
		return;
	}

	for (int i = 0; i < count && i < crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST; ++i)
	{
		long long group_id = info_list[i].group_id;
		ChatGroupInfo &info = m_cross_chat_group_info[group_id];
		info = info_list[i];
		//printf("ChatManager::OnHidenServerBack group_id[%lld] group_member[%d] group_count[%d] group_index[%d]\n", group_id, info.member_count, count, i);
	}
}

bool ChatManager::OnRoleStarCrossSendLocalGroup(Role * role)
{
	return false;

	/*
	if (NULL == role)return false;
	crossgameprotocal::GameCrossSyncCrossChatGroupInfo msg;
	int count = 0;
	for (int i = 0; i < MAX_ROLE_LOCAL_GROUP; i++)
	{
		long long &group_id = role->GetRoleModuleManager()->GetCommonData().local_chat_group_list[i];
		std::map<long long, ChatGroupInfo>::iterator it = m_chat_group_info.find(group_id);
		if (it != m_chat_group_info.end())
		{
			std::map<long long, bool>::iterator send_flag = m_send_cross.find(it->first);
			if (send_flag != m_send_cross.end())
			{
				if (!send_flag->second)//检测是否已经同步到跨服
				{
					msg.chat_group_list[count++] = it->second;
					send_flag->second = true;
				}
			}
		}
	}

	msg.count = count;
	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossChatGroupInfo) - ((crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, send_length);
	return true;
	*/
}

void ChatManager::OnCrossGroupDismiss(long long group_id, int uid)
{
	const ChatGroupInfo* info = this->GetGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		Role *member = mananger->GetRole(info->member_uid_list[i].member_uid);
		if (NULL == member)//解散时玩家离线状态，或者是玩家跨服
		{
			continue;//找不到直接返回，等跨服自己同步了解散操作在找下发
		}

		for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
		{
			long long &role_group = member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
			if (role_group == group_id)
			{
				role_group = 0;
			}
		}

		this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
		static MailContentParam contentparam; contentparam.Reset();

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chat_group_subject);
		int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chat_group_dismiss_content, info->group_name);

		if (lensub > 0 && lencont > 0)
		{
			MailRoute::MailToUser(member->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	this->EraseGroup(group_id);
}

void ChatManager::OnConnectCrossSucc()
{
	/*if (CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::GameCrossSyncCrossChatGroupInfo msg;
	int count = 0;
	for (std::map<long long, ChatGroupInfo>::iterator it = m_chat_group_info.begin(); it != m_chat_group_info.end() && count < crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST; ++it)
	{
		std::map<long long, bool>::iterator send_flag = m_send_cross.find(it->first);
		if (send_flag != m_send_cross.end())
		{
			if (!send_flag->second)//检测是否已经同步到跨服，只有连进去的第一次会这样
			{
				msg.chat_group_list[count++] = it->second;
				send_flag->second = true;
			}
		}
	}

	msg.count = count;
	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossChatGroupInfo) - ((crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, send_length);
	if (CrossManager::Instance().IsCrossServerConnected() && !this->CheckSendAllToCross())
	{
		this->OnConnectCrossSucc();//没发完再来一次
	}
	printf("ChatManager::OnConnectCrossSucc!\n");*/
}

/*
bool ChatManager::CheckSendAllToCross()
{
	/ *for (std::map<long long, bool>::iterator it = m_send_cross.begin(); it != m_send_cross.end(); ++it)
	{
		if (!it->second)
		{
			//printf("ChatManager::CheckSendAllChatGroupInfoToCross Continue!\n");
			return false;
		}
	}
	return true;
	//printf("ChatManager::CheckSendAllChatGroupInfoToCross Succ!\n");* /
}*/

void ChatManager::OnDisconnectFromCrossServer()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	
	m_send_cross.clear();
}

void ChatManager::OnCrossRegisterGameServer()
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	
	m_send_cross.clear();

	return;
	//取消启动同步逻辑
	/*if (CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::GameCrossSyncCrossChatGroupInfo msg;
	int count = 0;
	for (std::map<long long, ChatGroupInfo>::iterator it = m_chat_group_info.begin(); it != m_chat_group_info.end() && count < crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST; ++it)
	{
		std::map<long long, bool>::iterator send_flag = m_send_cross.find(it->first);
		if (send_flag != m_send_cross.end())
		{
			if (!send_flag->second)//检测是否已经同步到跨服，只有连进去的第一次会这样
			{
				msg.chat_group_list[count++] = it->second;
				send_flag->second = true;
			}
		}
	}

	msg.count = count;
	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossChatGroupInfo) - ((crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, send_length);
	if (CrossManager::Instance().IsCrossServerConnected() && !this->CheckSendAllToCross())
	{
		this->OnConnectCrossSucc();//没发完再来一次
	}
	printf("ChatManager::OnConnectCrossSucc!\n");*/
}

void ChatManager::OnGameSynCross(const ChatGroupInfo * msg_info)
{
	crossgameprotocal::GameCrossSyncCrossChatGroupInfo msg;
	msg.chat_group_list[0] = *msg_info;
	msg.count = 1;
	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::GameCrossSyncCrossChatGroupInfo) - ((crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, send_length);
}

void ChatManager::OnCrossSynGame(const ChatGroupInfo* msg_info, int user_id)
{
	if (msg_info == NULL)return;

	ChatGroupUniqueId tmp;
	tmp.unique_id = msg_info->group_id;
	//检查是否本服群组，如果是则不允许写入跨服临时群组列表
	const std::set<int>& allow_list = LocalConfig::Instance().GetAllowServerIdSet();
	if (allow_list.find(tmp.server_id) != allow_list.end()) return;

	m_cross_chat_group_info[msg_info->group_id] = *msg_info;

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	if (user_id == 0)return;
	Role * role = mananger->GetRole(IntToUid(user_id));
	this->OnGroupChangeSendGroupList(msg_info->group_id);
	if (role == NULL)return;
	this->OnGetGroupDetailInfo(role, msg_info->group_id);
}

void ChatManager::AddGroupChatBoard(long long group_id, const char *msg, int msg_len)
{
	if (NULL == msg || msg_len <= 0)
	{
		return;
	}

	MsgBoardMapIterator board_it = m_group_chat_board_map.find(group_id);
	if (m_group_chat_board_map.end() == board_it)
	{
		MsgBoardItemQueue tmp_queue;
		m_group_chat_board_map[group_id] = tmp_queue;

		board_it = m_group_chat_board_map.find(group_id);
		if (m_group_chat_board_map.end() == board_it)
		{
			return;
		}
	}

	const static unsigned int MAX_MSG_BOARD_ITEM_COUNT = 30;

	MsgBoardItemQueue &msg_item_queue = board_it->second;
	if (msg_item_queue.size() >= MAX_MSG_BOARD_ITEM_COUNT)
	{
		msg_item_queue.pop_front();
	}

	MsgBoardItem msg_board_item;
	msg_board_item.SetMsg(msg, msg_len);
	msg_item_queue.push_back(msg_board_item);

}

void ChatManager::CheckGroupChatBoard(Role *role, long long group_id)
{
	if (NULL == role)
	{
		return;
	}

	MsgBoardMapIterator board_it = m_group_chat_board_map.find(group_id);
	if (m_group_chat_board_map.end() == board_it)
	{
		return;
	}

	MsgBoardItemQueue &msg_item_queue = board_it->second;

	for (MsgBoardItemQueueIterator it = msg_item_queue.begin(); msg_item_queue.end() != it;)
	{
		int send_len = 0;
		const char *send_buffer = it->GetSendBuffer(&send_len);

		int offset = static_cast<int>(UNSTD_OFFSETOFF(Protocol::SCChannelChat, msg_length));
		if (send_len >= offset)
		{
			Protocol::SCChannelChat *channel_chat = (Protocol::SCChannelChat*)send_buffer;
			if (MutePlayerRecorder::Instance().IsMute(channel_chat->from_uid))
			{
				it = msg_item_queue.erase(it);
				continue;
			}
		}

		if (send_len > 0 && NULL != send_buffer)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), send_buffer, send_len);
		}
		++it;
	}
}

void ChatManager::CheckRoleInGroup(Role *role, bool is_login)
{
	if (NULL == role)
	{
		return;
	}

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (is_login)
	{
		now_second = 0;
	}
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if (group_id <= 0) continue;

		if (CrossManager::Instance().IsHiddenServerAlive())
		{
			crossgameprotocal::GameCrossSyncChatOpreta msg;
			msg.group_id = group_id;
			msg.uid = role->GetUID();
			msg.inviter_id = 0;
			msg.is_cross = 1;
			msg.opreta_type = is_login ? crossgameprotocal::CHAT_OPRETA_TYPE_LOGIN_GROUP : crossgameprotocal::CHAT_OPRETA_TYPE_LOGOUT_GROUP;
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		}

		ChatGroupInfo* info = this->MutableGroupInfo(group_id);
		if (NULL == info) continue;

		ChatGroupUserInfo* user_info = info->GetUserInfo(role->GetUID());
		if (NULL != user_info)
		{
			user_info->member_logout_tamp = is_login ? 0u : now_second;
		}
	}
}

void ChatManager::OnServerStart()
{
	m_data_state = CHAT_GROUP_DATA_STATUS_LOADING;
	this->LoadData(-1);
}

void ChatManager::OnServerStop()
{
	
}

const ChatGroupInfo * ChatManager::GetGroupInfo(long long group_id)
{
	return this->MutableGroupInfo(group_id);
}

bool ChatManager::CheckRoleCreateGroup(Role * role, bool is_cross, int * invalid_index)
{
	return this->RoleCanEnterGroupCount(role, is_cross, invalid_index);
}

void ChatManager::OnCreateGroup(Role *role, GameName group_name, bool is_cross)
{
	if (NULL == role)
	{
		return;
	}

	role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
}

bool ChatManager::SendToGroup(long long group_id, void *msg, size_t length)
{
	const ChatGroupInfo* info = this->GetGroupInfo(group_id);
	if (NULL == info) return false;
	
	if (info->member_count <= 0)
	{
		return false;
	}

	SceneManager *manager = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		Role *member = manager->GetRole(info->member_uid_list[i].member_uid);
		if (NULL == member)
		{
			continue;//不操作，等待协议发往再自行操作
		}

		Protocol::SCChannelChat * real_msg = (Protocol::SCChannelChat *)msg;
		real_msg->group_id = info->group_id;//修改一下，这个下发为群id

		EngineAdapter::Instance().NetSend(member->GetNetId(), (char *)real_msg, length);
	}

	return true;
}

bool ChatManager::RoleCanEnterGroupCount(Role *role, bool is_cross, int *invalid_index)
{
	if (NULL == role)
	{
		return false;
	}

	if (is_cross)
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return false;
		}
	}

	int group_count = 0;
	int cross_count = 0;
	int local_count = 0;

	if(is_cross)
	{
		for (int cross_group = 0; cross_group < MAX_ROLE_CROSS_GROUP; cross_group++)
		{
			long long &group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[cross_group];
			if (group_id > 0)//跨服接通中，同时玩家的群组id不为空
			{
				/* 原服不在找不到的情况下重置group_id
				if (CrossManager::Instance().IsCrossServerConnected() && NULL == this->GetGroupInfo(group_id))//直接检查如果找不到就是没有了
				{
					gamelog::g_log_debug.printf(LL_INFO, "ChatManager::RoleCanEnterGroupCount role[%d,%s] group_id[%lld] not found",
						role->GetUID(), role->GetName(), group_id);
					group_id = 0;
					continue;
				}
				*/

				++cross_count;
				++group_count;
			}
			else
			{
				if (NULL != invalid_index) *invalid_index = cross_group;
			}
		}
	}
	
	if (group_count >= MAX_ROLE_GROUP || (is_cross ? cross_count >= MAX_ROLE_CROSS_GROUP : local_count >= MAX_ROLE_LOCAL_GROUP))
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_MAX);
		return false;
	}

	return true;
}

void ChatManager::SendRoleGroupInfo(Role *role,int reason /*= Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_DEF*/)
{
	if (NULL == role)
	{
		return;
	}

	Protocol::SCRoleGroupInfo msg;
	msg.reason = reason;
	msg.group_count = 0;
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		long long &group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if (group_id <= 0)
		{
			continue;
		}

		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			continue;
		}

		ChatGroupInfo* info = this->MutableGroupInfo(group_id);
		if (NULL == info)
		{
			continue;
		}

		//查到了这个群组，先检查一下玩家是否在这个群组里面
		if (NULL == info->GetUserInfo(role->GetUID()))
		{
			/* 原服在找不到的情况下不重置group_id
			//玩家不在群组里面了
			gamelog::g_log_debug.printf(LL_INFO, "ChatManager::SendRoleGroupInfo user not found in group, role[%d,%s] group[%lld]",
				role->GetUID(), role->GetName(), group_id);

			group_id = 0;//清除玩家群组标记
			*/
			continue;
		}

		memset(msg.group_list[msg.group_count].top_user_ava, 0, sizeof(msg.group_list[msg.group_count].top_user_ava));
		int top_index = 0;
		for (int member_info = 0;member_info < info->member_count && top_index < 3; member_info++)
		{
			if(info->member_uid_list[member_info].member_uid == 0)continue;
			msg.group_list[msg.group_count].top_user_ava[top_index++] = info->member_uid_list[member_info].member_ava;
		}
		msg.group_list[msg.group_count].group_id = info->group_id;
		msg.group_list[msg.group_count].is_cross = info->is_cross;
		STRNCPY(msg.group_list[msg.group_count].group_name, info->group_name, sizeof(GameName));
		msg.group_list[msg.group_count].group_master_id = info->creator_uid;
		msg.group_list[msg.group_count].member_count = info->member_count;
		++msg.group_count;
	}
	
	size_t len = sizeof(Protocol::SCRoleGroupInfo) - sizeof(msg.group_list) + sizeof(msg.group_list[0]) * msg.group_count;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&msg, len);
}

void ChatManager::SendRoleOneGroupInfo(Role* role, long long group_id, int reason /*= Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE*/)
{
	if (NULL == role)
	{
		return;
	}

	Protocol::SCRoleOneGroupInfo msg;
	msg.reason = reason;

	if (Protocol::SCRoleOneGroupInfo::REASON_REMOVE == reason)
	{
		msg.group.group_id = group_id;
	}
	else
	{
		const ChatGroupInfo* group = this->GetGroupInfo(group_id);
		if (NULL == group) return;

		if (NULL == group->GetConstUserInfo(role->GetUID()))
		{
			return;
		}

		const ChatGroupInfo& info = *group;

		memset(msg.group.top_user_ava, 0, sizeof(msg.group.top_user_ava));
		int top_index = 0;
		for (int member_info = 0; member_info < info.member_count && top_index < 3; member_info++)
		{
			if (info.member_uid_list[member_info].member_uid == 0)continue;
			msg.group.top_user_ava[top_index++] = info.member_uid_list[member_info].member_ava;
		}
		msg.group.group_id = info.group_id;
		msg.group.is_cross = info.is_cross;
		STRNCPY(msg.group.group_name, info.group_name, sizeof(GameName));
		msg.group.group_master_id = info.creator_uid;
		msg.group.member_count = info.member_count;
	}

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg));
}

void ChatManager::OnChangeGroupNotice(Role * role, GameName group_name, GuildNotice group_notice, long long group_id)
{
	if (NULL == role)return;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	if (info->creator_uid != role->GetUID())
	{
		role->NoticeNum(errornum::EN_CHAT_GROUP_POWER_LIMIT);
		return;
	}

	if (info->is_cross == 0)
	{
		F_STRNCPY(info->group_name, group_name, sizeof(GameName));
		F_STRNCPY(info->group_notice, group_notice, sizeof(GuildNotice));
		this->MarkDirty(group_id);
	}

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	for (int member_idx = 0; member_idx < info->member_count && member_idx < CHAT_GROUP_MEMBER_MAX; ++member_idx)
	{
		if (scene_manager == NULL)continue;
		Role *member_role = scene_manager->GetRole(IntToUid(info->member_uid_list[member_idx].member_uid));
		if (member_role != NULL)
		{
			this->OnGetGroupDetailInfo(member_role, group_id);
		}
	}

	//发往跨服且发往各自成员服务器
	static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_GROUP_NAME_OR_NOTICE;//操作传到跨服，直接发往各自群员游戏服，数据先行同步过了
	chat_msg.uid = role->GetUID();
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_name, group_name, sizeof(GameName));
	F_STRNCPY(chat_msg.group_notice, group_notice, sizeof(GuildNotice));
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	this->OnGroupChangeSendGroupList(group_id);

	gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnDismissGroup [%s %d] group_id: %lld", role->GetName(), role->GetUID(), group_id);
}

void ChatManager::OnRoleLeaveGroup(Role *role, long long group_id, bool notice)
{
	if (NULL == role)
	{
		return;
	}
	CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
		{
			if (common_data.cross_chat_group_list[i] == group_id)
			{
				common_data.cross_chat_group_list[i] = 0;
				role->NoticeNum(noticenum::NT_CHAT_GROUP_LEAVE_SUCC);
			}
		}
		return;
	}
	
	if (info->creator_uid == role->GetUID())
	{
		// 创建者不能退出，只能解散
		role->NoticeNum(errornum::EN_CRATER_LEAVE_CHAT_GROUP_FAIL);
		return;
	}

	if (NULL == info->GetUserInfo(role->GetUID()))
	{
		if (info->is_cross)
		{
			for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
			{
				if (common_data.cross_chat_group_list[i] == group_id)
				{
					common_data.cross_chat_group_list[i] = 0;
					role->NoticeNum(noticenum::NT_CHAT_GROUP_LEAVE_SUCC);
				}
			}
		}
	}

	bool ret = info->RemoveUserInfo(role->GetUID());
	if (!ret)
	{
		gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnRoleLeaveGroup [%s %d] RemoveUserInfo fail group_id: %lld", role->GetName(), role->GetUID(), group_id);
	}

	if (info->is_cross)
	{
		for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
		{
			if (common_data.cross_chat_group_list[i] == group_id)
			{
				common_data.cross_chat_group_list[i] = 0;
				role->NoticeNum(noticenum::NT_CHAT_GROUP_LEAVE_SUCC);
			}
		}
	}

	this->MarkDirty(group_id);
	this->SendRoleOneGroupInfo(role, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);

	//发往跨服操作，等待操作返回
	static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LEAVEN_GROUP_RET;
	chat_msg.uid = role->GetUID();
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_master, role->GetName(), sizeof(GameName));
	chat_msg.param1 = 0;//非跨服标记
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
	
	if (notice)
	{
		Protocol::SCGroupAllInfo msg;
		msg.info = *info;

		this->SendToGroup(group_id, &msg, sizeof(msg));
	}

	this->OnGroupChangeSendGroupList(group_id);

	gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnRoleLeaveGroup [%s %d] group_id: %lld", role->GetName(), role->GetUID(), group_id);
}

void ChatManager::OnDismissGroup(Role *role, long long group_id)
{
	
}

void ChatManager::OnRoleLeaveGroupInCross(long long group_id, int uid, GameName member_name)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	if (info->is_cross)
	{
		bool ret = info->RemoveUserInfo(uid);
		if (!ret)
		{
			gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnRoleLeaveGroupInCross [%s %d] group_id: %lld", member_name, uid, group_id);
		}
	}
	
	Role * member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (member_role != NULL)
	{
		this->OnGetGroupDetailInfo(NULL, group_id);
		this->SendRoleOneGroupInfo(member_role, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
		member_role->NoticeNum(noticenum::NT_CHAT_GROUP_LEAVE_SUCC);
	}
	
	this->OnGroupChangeSendGroupList(group_id);

	gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnRoleLeaveGroupInCross [%s %d] group_id: %lld", member_name, uid, group_id);
}

void ChatManager::OnRoleEnterGroupRet(crossgameprotocal::CrossGameSyncChatOpreta * msg)
{
	if (NULL == msg)return;

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager == NULL)return;
	Role * role = scene_manager->GetRole(IntToUid(msg->uid));
	Role * inviter_role = scene_manager->GetRole(IntToUid(msg->inviter_id));
	switch (msg->param1)
	{
	case crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC://成功了
		if (role != NULL)role->NoticeNum(noticenum::NT_CHAT_GROUP_ENTER_SUCC);
		break;
	case crossgameprotocal::CHAT_OPRETA_RET_TYPE_GROUP_FULL://失败了理由是满人
		if (role != NULL)role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_FULL);
		break;
	case crossgameprotocal::CHAT_OPRETA_RET_TYPE_ALREADY_EXISTS://失败了理由是你已经再这个群组了
		if (role != NULL)role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_ALREADY_IN);
		break;
	default:
		break;
	}

	ChatGroupInfo* info = this->MutableGroupInfo(msg->group_id);
	if (NULL == info) return;

	bool add_ret = info->AddUserInfo(msg->uid, msg->param2, msg->param3, msg->param4, msg->group_master);
	if (!add_ret)
	{
		return;
	}

	if (role != NULL && msg->param5 >= 0 && msg->param5 < MAX_ROLE_CROSS_GROUP)
	{
		role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[msg->param5] = msg->group_id;
		this->SendRoleOneGroupInfo(role, msg->group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);
	}

	//
	if(inviter_role != NULL)
	{
		this->OnGetGroupDetailInfo(inviter_role, msg->group_id);
	}

	this->OnGroupChangeSendGroupList(msg->group_id);//TODO
}

void ChatManager::OnRoleEnterGroup(Role *role, long long group_id, int invite_uid)
{
	if (NULL == role)
	{
		return;
	}

	ChatGroupUniqueId tmp;
	tmp.unique_id = group_id;

	if (role->GetUID() == invite_uid)
	{
		return;
	}

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_INVALID);
		return;
	}

	if (info->member_count >= CHAT_GROUP_MEMBER_MAX)
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_FULL);
		return;
	}

	int invalid_index = 0;
	if (!this->RoleCanEnterGroupCount(role, 0 != info->is_cross, &invalid_index))
	{
		return;
	}

	if (NULL != info->GetUserInfo(role->GetUID()))
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_IN);
		return;//玩家已经在群组了
	}

	if (info->is_cross == 1)
	{
		//发往跨服处理
		crossgameprotocal::GameCrossSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = role->GetUID();
		msg.inviter_id = invite_uid;
		F_STRNCPY(msg.group_master, role->GetName(), sizeof(GameName));
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP;
		msg.param1 = this->GetRoleBaseAva(role);
		msg.param2 = role->GetLevel();
		msg.param3 = role->GetProfession();
		msg.param4 = invalid_index;
		msg.is_cross = 1;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		return;
	}
}

void ChatManager::OnRoleInviteGroup(Role *role, int be_invite, long long group_id)
{
	if (NULL == role || 0 == be_invite)
	{
		return;
	}

	if (role->GetUID() == be_invite)
	{
		return;
	}

	if (role->GetChatGroupInv() > EngineAdapter::Instance().Time())
	{
		role->NoticeNum(errornum::EN_CHAT_INV_TIME_LIMIT);
		return;
	}
	time_t & chat_group_inv = role->GetChatGroupInv();
	chat_group_inv = EngineAdapter::Instance().Time() + 60;//一分钟邀请一次

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	ChatGroupUniqueId tmp;
	tmp.unique_id = group_id;

	// 先检查被邀请对象以及自身是否在群里

	if (info->member_count >= CHAT_GROUP_MEMBER_MAX)
	{
		role->NoticeNum(errornum::EN_CHAT_GROUP_INV_FULL_ERR);
		return;
	}

	bool found = false;
	for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		if (info->member_uid_list[i].member_uid == role->GetUID())
		{
			found = true;
		}

		if (info->member_uid_list[i].member_uid == be_invite)
		{
			role->NoticeNum(errornum::EN_BE_INVITE_MEMBER_WAS_IN_GROUP);
			found = false;
			break;
		}
	}

	if (!found)
	{
		return;
	}
	if (info->is_cross == 1)
	{
		Role *be_invite_role = World::GetInstWorld()->GetSceneManager()->GetRole(be_invite);
		if (NULL == be_invite_role)//
		{
			//检查这个玩家是否本服玩家
			if (CrossUserShadow::Instance().IsUserInCross(be_invite))
			{
				crossgameprotocal::GameCrossSyncChatOpreta msg;
				msg.group_id = group_id;
				msg.uid = be_invite;
				msg.inviter_id = role->GetUID();
				F_STRNCPY(msg.group_master, role->GetName(), sizeof(GameName));
				F_STRNCPY(msg.group_name, info->group_name, sizeof(GameName));
				msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_INVITE_MEMBER;
				msg.is_cross = info->is_cross;
				InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
				role->NoticeNum(noticenum::NT_CHAT_GROUP_IVENTE_SUCC);
				return;
			}
			else//跨服好友or离线
			{
				if (LocalConfig::Instance().IsAllowedServerId(IntToUid(be_invite).db_index))//预先检查这个玩家是否同服玩家
				{
					//离线
					role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
					return;
				}
				else
				{
					crossgameprotocal::GameCrossSyncChatOpreta msg;
					msg.group_id = group_id;
					msg.uid = be_invite;
					msg.inviter_id = role->GetUID();
					F_STRNCPY(msg.group_master, role->GetName(), sizeof(GameName));
					F_STRNCPY(msg.group_name, info->group_name, sizeof(GameName));
					msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_INVITE_MEMBER;
					msg.is_cross = info->is_cross;
					InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
					role->NoticeNum(noticenum::NT_CHAT_GROUP_IVENTE_SUCC);
					return;
				}
			}
		}
		else//本服玩家直接邀请
		{
			Protocol::SCRoleInviteEnterGroup msg;
			msg.invite_server_id = tmp.server_id;
			STRNCPY(msg.invite_group_name, info->group_name, sizeof(GameName));
			msg.invite_group_id = group_id;
			msg.is_cross = info->is_cross;
			msg.invite_uid = role->GetUID();
			STRNCPY(msg.inviter_name, role->GetName(), sizeof(GameName));
			EngineAdapter::Instance().NetSend(be_invite_role->GetNetId(), (char *)&msg, sizeof(msg));
			role->NoticeNum(noticenum::NT_CHAT_GROUP_IVENTE_SUCC);
		}
	}
}

void ChatManager::OnKickMemberFromGroup(Role *role, int be_kick, long long group_id)
{
	//if (CrossConfig::Instance().IsHiddenServer())return;
	if (NULL == role)
	{
		return;
	}

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	if (info->creator_uid != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_KICK_RIGHT);
		return;
	}

	if (role->GetUID() == be_kick)
	{
		return;
	}

	ChatGroupUserInfo * find2 = info->GetUserInfo(be_kick);

	if (NULL == find2)
	{
		role->NoticeNum(errornum::EN_CHAT_GROUP_KICK_OUT_ERR);
		return;
	}
	
	if (info->is_cross)
	{
		static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER;
		chat_msg.uid = be_kick;
		chat_msg.group_id = group_id;
		chat_msg.inviter_id = role->GetUID();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		return;
	}

	this->OnGetGroupDetailInfo(role, group_id);//详细信息发往操作者
	this->SendRoleOneGroupInfo(role, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);

	//发往跨服且发往各自成员服务器
	static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER_RET;
	chat_msg.uid = be_kick;
	chat_msg.group_id = group_id;
	chat_msg.inviter_id = role->GetUID();
	chat_msg.param1 = 0;
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnKickMemberFromGroup master:[%s %d] bekick_role:[%d] group_id: %lld", role->GetName(), role->GetUID(), be_kick, group_id);
}

void ChatManager::OnRoleBeKickOutGroup(int uid, long long group_id)
{
	
}

void ChatManager::OnGetGroupDetailInfo(Role *role, long long group_id)
{
	if (NULL == role)
	{
		return;
	}

	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	if (NULL == info->GetUserInfo(role->GetUID()))return; //不是群组人员无反应

	Protocol::SCGroupAllInfo msg;
	msg.info = *info;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&msg, sizeof(msg));
}

void ChatManager::OnGroupChangeSendGroupList(long long group_id)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		//群组信息改变，下发略缩图给群员
		Role * member_role = mananger->GetRole(IntToUid(info->member_uid_list[i].member_uid));
		if (member_role != NULL)
		{
			this->SendRoleOneGroupInfo(member_role, group_id, Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE);
		}
	}
}

void ChatManager::OnGroupMemberChangeName(Role * role, GameName new_name)
{
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
	{
		long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if(group_id <= 0) continue;

		const ChatGroupInfo* info = this->GetGroupInfo(group_id);
		if (NULL == info) continue;
		
		const ChatGroupUserInfo * member_info = info->GetConstUserInfo(role->GetUID());
		if (member_info == NULL) continue;

		//发往跨服且发往各自成员服务器
		static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_NAME;
		chat_msg.uid = role->GetUID();
		chat_msg.group_id = group_id;
		F_STRNCPY(chat_msg.group_master, new_name, sizeof(GameName));
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&chat_msg, sizeof(chat_msg));
	}
}

void ChatManager::OnCrossEnterGroup(long long group_id, int member_id, int member_ava, int member_level, int member_prof, GameName member_name, int inviter_id, int invalid_index)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	/*int invalid_index = 0;
	if (!this->RoleCanEnterGroupCount(role, 0 != it->second.is_cross, &invalid_index))
	{
		return;
	}*/
	//上面这个步骤已经再跨服检查过了

	if (info->member_count >= CHAT_GROUP_MEMBER_MAX)
	{
		crossgameprotocal::GameCrossSyncChatOpreta msg;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;//本地服处理发往跨服
		msg.group_id = group_id;
		msg.uid = member_id;
		msg.inviter_id = inviter_id;
		F_STRNCPY(msg.group_name, info->group_name, sizeof(GameName));
		msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_GROUP_FULL;//满人了
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		return;
	}

	if (info->is_cross == 1)
	{
		return;//跨服群组不会操作到这里来
	}

	//this->OnGameSynCross(&info);//发往跨服

	this->OnGroupChangeSendGroupList(group_id);

	Role * inviter_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(inviter_id));
	if (inviter_role != NULL)
	{
		this->OnGetGroupDetailInfo(inviter_role, group_id);
	}

	gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnRoleEnterGroup [%s %d] group_id: %lld", member_name, member_id, group_id);

	//返回
	crossgameprotocal::GameCrossSyncChatOpreta msg;
	msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;//本地服处理发往跨服
	msg.group_id = group_id;
	msg.uid = member_id;
	msg.inviter_id = inviter_id;
	F_STRNCPY(msg.group_name, info->group_name, sizeof(GameName));
	msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC;//加入了
	msg.param2 = member_ava;
	msg.param3 = member_level;
	msg.param4 = member_prof;
	msg.param5 = invalid_index;
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
}

void ChatManager::OnRoleChangeName(long long group_id, int member_id, GameName new_name)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	ChatGroupUserInfo * member_info = info->GetUserInfo(member_id);
	if (member_info == NULL)return;

	F_STRNCPY(member_info->member_name, new_name, sizeof(GameName));
	this->OnGroupChangeSendGroupList(group_id);
}

void ChatManager::OnRoleChangeProf(long long group_id, int member_id, int new_prof, int new_ava)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	ChatGroupUserInfo * member_info = info->GetUserInfo(member_id);
	if (member_info == NULL) return;

	member_info->member_prof = new_prof;
	member_info->member_ava = new_ava;

	this->OnGroupChangeSendGroupList(group_id);
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member_id));
	if (role != NULL)this->OnGetGroupDetailInfo(role, group_id);
}

void ChatManager::OnRoleChangeAva(long long group_id, int member_id, int new_ava)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	ChatGroupUserInfo * member_info = info->GetUserInfo(member_id);
	if (member_info == NULL) return;

	member_info->member_ava = new_ava;

	this->OnGroupChangeSendGroupList(group_id);
}

void ChatManager::OnRoleCreateCrossGroupRet(long long group_id, int uid, int invalid_index)
{
	const ChatGroupInfo* info = this->GetGroupInfo(group_id);
	if (NULL == info) return;

	if (info->creator_uid == uid)
	{
		Role *  create_role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
		if(NULL == create_role)
		{
			OfflineRegisterManager::Instance().OnCrossGroupCreateSucc(uid, group_id, invalid_index);
		}
		else
		{
			create_role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[invalid_index] = group_id;
			this->SendRoleOneGroupInfo(create_role, group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);
		}
	}
}

void ChatManager::OnGroupNameOrNoticeChange(long long group_id, int uid, GameName group_name, GuildNotice group_notice)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;
	
	F_STRNCPY(info->group_name, group_name, sizeof(GameName));
	F_STRNCPY(info->group_notice, group_notice, sizeof(GuildNotice));

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	for (int member_idx = 0; member_idx < info->member_count && member_idx < CHAT_GROUP_MEMBER_MAX; ++member_idx)
	{
		if (scene_manager == NULL)continue;
		Role *member_role = scene_manager->GetRole(IntToUid(info->member_uid_list[member_idx].member_uid));
		if (member_role != NULL)
		{
			this->OnGetGroupDetailInfo(member_role, group_id);
		}
	}

	this->OnGroupChangeSendGroupList(group_id);
	Role * creator_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (creator_role != NULL) this->OnGetGroupDetailInfo(creator_role, group_id);
}

void ChatManager::OnGroupLeaderChangeSync(long long group_id, int new_leader_uid)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	info->creator_uid = new_leader_uid;
}

void ChatManager::OnGameSyncDismissGroup(long long group_id, int user_id)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info)
	{
		return;
	}

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	if (info->is_cross == 1)
	{
		for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			Role *member = mananger->GetRole(info->member_uid_list[i].member_uid);
			if (NULL == member)
			{
				continue;
			}

			for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
			{
				long long &role_group = member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
				if (role_group == group_id)
				{
					role_group = 0;
				}

				this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
			}
			static MailContentParam contentparam; contentparam.Reset();

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chat_group_subject);
			int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chat_group_dismiss_content, info->group_name);

			if (lensub > 0 && lencont > 0)
			{
				MailRoute::MailToUser(member->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		this->EraseGroup(group_id);
	}
}

void ChatManager::OnGameSyncKickMemberFromGroup(long long group_id, int user_id, int be_kick_id)
{
	if (CrossConfig::Instance().IsHiddenServer())//这操作只会在本地服返回结果到这
	{
		SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
		//刷新踢人的人的群组详细信息
		Role * role = mananger->GetRole(IntToUid(user_id));
		if (role != NULL)this->OnGetGroupDetailInfo(role, group_id);

		Role *member = mananger->GetRole(be_kick_id);
		if (NULL != member)//如果玩家还在，那就清掉common_data.chat_group_list
		{
			for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
			{
				long long &role_group = member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
				if (role_group == group_id)
				{
					role_group = 0;
				}

				this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
			}
		}

		this->OnGroupChangeSendGroupList(group_id);
	}
	else
	{
		bool is_my_server_group = true;
		ChatGroupInfo* info = this->MutableGroupInfo(group_id);
		if (NULL == info)
		{
			return;
		}

		SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
		for (int i = 0; i < info->member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
		{
			if (info->member_uid_list[i].member_uid != be_kick_id)
			{
				continue;
			}
			info->member_uid_list[i] = info->member_uid_list[info->member_count - 1];
			info->member_uid_list[info->member_count].member_uid = 0;
			--info->member_count;
			Role *member = mananger->GetRole(be_kick_id);
			if (NULL == member)
			{
				continue;//来自其他服的，找不到直接不操作
			}
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				gamestring::g_chat_group_tick_out_content, info->group_name);
			if (length > 0)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&sm, sendlen);
				}
			}
			if (info->is_cross)
			{
				for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
				{
					long long &role_group = member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
					if (role_group == group_id)
					{
						role_group = 0;
					}
					this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
				}
			}
		}

		//本地修改之后同步到跨服

		if(is_my_server_group)this->MarkDirty(info->group_id);

		this->OnGameSynCross(info);//发往跨服

		this->OnGroupChangeSendGroupList(group_id);//TODO

		//踢人返回到跨服
		static crossgameprotocal::GameCrossSyncChatOpreta msg;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER_RET;
		msg.uid = be_kick_id;
		msg.inviter_id = user_id;
		msg.group_id = group_id;
		msg.param1 = 0;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void ChatManager::OnGameSyncKickMemberFromGroupRet(long long group_id, int user_id, int be_kick_id)
{
	ChatGroupInfo* info = this->MutableGroupInfo(group_id);
	if (NULL == info) return;

	if (NULL == info->GetUserInfo(be_kick_id))
	{
		return;
	}

	bool ret = info->RemoveUserInfo(be_kick_id);
	if (!ret)
	{
		gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnGameSyncKickMemberFromGroupRet user[%d] group_id[%lld] RemoveUserInfo fail",be_kick_id,group_id);
	}
	this->OnGroupChangeSendGroupList(group_id);//所有群员缩略下发

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();

	//群主详细下发
	Role * creator = mananger->GetRole(IntToUid(info->creator_uid));
	if (creator != NULL)
	{
		this->OnGetGroupDetailInfo(creator, info->group_id);
	}

	Role *member = mananger->GetRole(be_kick_id);
	if (NULL == member)
	{
		//玩家可能离线了
		return;
	}
	if (info->is_cross == 1)
	{
		for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
		{
			member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		}
	}
	
	this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_chat_group_tick_out_content, info->group_name);
	if (length > 0)
	{
		static Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&sm, sendlen);
		}
	}
}

unsigned long long ChatManager::GetUniqueGroupId()
{
	ChatGroupUniqueId tmp;

	tmp.server_id = LocalConfig::Instance().GetMergeServerId();
	tmp.seq = m_chat_group_seq++;
	tmp.time = (unsigned int)EngineAdapter::Instance().Time();

	return tmp.unique_id;
}

void ChatManager::AddChatItemInfo(int * sign_id1, int * sign_id2, const ItemDataWrapper & item_info, const AttributeList * attrs)
{
	ChatItemInfo chat_item_info;
	chat_item_info.item = item_info;
	if (NULL != attrs)	//此时代表宠物
	{
		for (int i = 0; i < ARRAY_LENGTH(attrs->m_attrs); i++)
		{
			chat_item_info.atts[i] = attrs->m_attrs[i];
		}
	}
	long long unique_seq = this->AddChatItemInfo(chat_item_info);
	*sign_id1 = unique_seq & 0x00000000ffffffffLL;
	*sign_id2 = (unique_seq & 0xffffffff00000000LL) >> 32;
	
	//memcpy(sign_id1, &unique_seq, sizeof(sign_id1));
	//memcpy(sign_id2, (char *)&unique_seq + sizeof(sign_id1), sizeof(sign_id2));
}
