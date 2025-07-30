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

#include "other/rolemodulemanager.hpp"
#include "friend/friendmanager.h"
#include "other/pet/pet.hpp"
#include "other/facescore/facescore.hpp"

#include "item/knapsack.h"
#include "item/itempool.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "chat/chatuser.h"
#include "crosschatgroupmanager.h"
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
#include "other/rolecross/rolecross.hpp"
#include "chat/chatmanager.h"
#include "chatgroup.hpp"

static const int MSX_GSNETID_NUM = 1024 * 16;
static GSNetID gsnetidlist[MSX_GSNETID_NUM];

CrossChatGroupManager::CrossChatGroupManager(): m_check_times(0), m_chat_group_seq(0), m_data_state(CHAT_GROUP_DATA_STATUS_INVALID), m_last_save_time(0u)
{
	m_buffer = new char[MAX_MSG_SIZE];	// 4M, 不可能有比4M还大的一个网络包，一定够用了
	memset(m_buffer, 0, MAX_MSG_SIZE);
}

CrossChatGroupManager::~CrossChatGroupManager()
{

}

CrossChatGroupManager & CrossChatGroupManager::Instance()
{
	static CrossChatGroupManager umm;
	return umm;
}

void CrossChatGroupManager::OnUserLogin(Role *role)
{
	if (NULL == role)return;
	if (!CrossConfig::Instance().IsHiddenServer())return;

	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		long long &group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if (group_id == 0)	continue;

		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) continue;

		const ChatGroupUserInfo * role_info = group->GetUserInfo(role->GetUID());
		if (NULL == role_info)
		{
			group_id = 0;
			continue;
		}

		group->OnMemberLogin(role->GetUID());
		group->UpdateMemberBaseInfo(role);
		
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = role->GetUID();
		msg.inviter_id = 0;
		msg.is_cross = 1;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGIN_GROUP;
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
	}

	this->SendRoleGroupInfo(role);
	ChatManager::Instance().SendChatRecrodList(role);
}

void CrossChatGroupManager::OnUserLogout(Role *role)
{
	if (NULL == role)return;
	if (!CrossConfig::Instance().IsHiddenServer())return;

	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; ++i)
	{
		long long &group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if (group_id == 0)continue;

		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) continue;

		const ChatGroupUserInfo * role_info = group->GetUserInfo(role->GetUID());
		if (NULL == role_info)
		{
			group_id = 0;
			continue;
		}
		group->OnMemberLogout(role->GetUID());

		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = role->GetUID();
		msg.inviter_id = 0;
		msg.is_cross = 1;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGOUT_GROUP;
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
	}
}

void CrossChatGroupManager::OnUserLoginByGame(long long group_id, int uid)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;
	
	const ChatGroupInfo &info = group->GetGroupInfo();

	group->OnMemberLogin(uid);

	if (info.is_cross == 1)
	{
		ChatGroupUniqueId group_key;
		group_key.unique_id = group_id;
		long long send_group_list[MAX_ROLE_GROUP];
		send_group_list[0] = group_id;
		bool has_send_update = this->OnFragmentGameServerInLineReturnCrossData(IntToUid(info.creator_uid).db_index, send_group_list, 1);
		
		if (has_send_update)
		{
			crossgameprotocal::CrossGameSyncChatOpreta msg;
			msg.group_id = group_id;
			msg.uid = uid;
			msg.inviter_id = 0;
			msg.is_cross = 1;
			msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGIN_GROUP;
			InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
		}
	}
	else
	{
		int plat_spid = LocalConfig::Instance().GetPlatType();
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = uid;
		msg.inviter_id = 0;
		msg.is_cross = 1;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGIN_GROUP;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(uid).db_index, (const char*)&msg, sizeof(msg));
	}
}

void CrossChatGroupManager::OnUserLogoutByGame(long long group_id, int uid)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo &info = group->GetGroupInfo();

	group->OnMemberLogout(uid);

	if (info.is_cross == 1)
	{
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = uid;
		msg.inviter_id = 0;
		msg.is_cross = 1;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGOUT_GROUP;
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		int plat_spid = LocalConfig::Instance().GetPlatType();
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = uid;
		msg.inviter_id = 0;
		msg.is_cross = 1;
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LOGOUT_GROUP;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(uid).db_index, (const char*)&msg, sizeof(msg));
	}
}

void CrossChatGroupManager::OnGroupChat(Role * role, long long group_id, const char * buff, int total_len)
{
	if (NULL == role || NULL == buff || total_len <= 0) return;

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	group->OnMemberChat(role->GetUID());

	const ChatGroupInfo& info = group->GetGroupInfo();

	if (info.is_cross == 1)//聊天组跨服发言直接发送回原服
	{
		crossgameprotocal::HiddenGameSendChatGroup* chat_msg = (crossgameprotocal::HiddenGameSendChatGroup*)m_buffer;
		memset(m_buffer, 0, MAX_MSG_SIZE);
		chat_msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_CHAT_GROUP; 
		chat_msg->group_id = group_id;
		chat_msg->main_server_id = 0;
		memcpy(m_buffer + sizeof(crossgameprotocal::HiddenGameSendChatGroup), buff, total_len);
		InternalComm::Instance().SendToAllGameThroughCross((const char*)m_buffer, sizeof(crossgameprotocal::GameHiddenSendChatGroup) + total_len);//发往所有游戏服，之后在进一步操作
	}
	else
	{
		crossgameprotocal::HiddenGameSendChatGroup* chat_msg = (crossgameprotocal::HiddenGameSendChatGroup*)m_buffer;
		memset(m_buffer, 0, MAX_MSG_SIZE);
		chat_msg->header.msg_type = crossgameprotocal::MT_HIDDEN_GAME_CHAT_GROUP;
		chat_msg->group_id = group_id;
		chat_msg->main_server_id = 0;
		memcpy(m_buffer + sizeof(crossgameprotocal::GameHiddenSendChatGroup), buff, total_len);
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index, (const char*)m_buffer, sizeof(crossgameprotocal::GameHiddenSendChatGroup) + total_len);
	}

	this->SendToGroup(group_id, (void *)buff, total_len);
}

ChatGroup* CrossChatGroupManager::GetChatGroup(long long group_id)
{
	ChatGroupMap::iterator it = m_chat_group_map.find(group_id);
	if (it != m_chat_group_map.end())
	{
		if (NULL == it->second || !it->second->IsValid())
		{
			return NULL;
		}

		return it->second;
	}

	return NULL;
}

void CrossChatGroupManager::OnRoleLoginGameSendChatGroupSimpleInfo(int uid, int plat_type, int server_id, int max_num, ARG_IN long long* group_list)
{
	Protocol::SCRoleGroupInfo msg;
	msg.reason = Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_LOGIN;
	msg.group_count = 0;
	for (int i = 0; i < max_num; ++i)
	{
		long long group_id = group_list[i];
		if (group_id <= 0)
		{
			continue;
		}

		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) continue;

		//查到了这个群组，先检查一下玩家是否在这个群组里面	
		if (NULL == group->GetUserInfo(uid)) continue;

		const ChatGroupInfo &info = group->GetGroupInfo();
		memset(msg.group_list[msg.group_count].top_user_ava, 0, sizeof(msg.group_list[msg.group_count].top_user_ava));
		int top_index = 0;
		for (int member_info = 0; member_info < info.member_count && top_index < 3; member_info++)
		{
			if (info.member_uid_list[member_info].member_uid == 0)continue;
			msg.group_list[msg.group_count].top_user_ava[top_index++] = info.member_uid_list[member_info].member_ava;
		}
		msg.group_list[msg.group_count].group_id = info.group_id;
		msg.group_list[msg.group_count].is_cross = info.is_cross;
		STRNCPY(msg.group_list[msg.group_count].group_name, info.group_name, sizeof(GameName));
		msg.group_list[msg.group_count].group_master_id = info.creator_uid;
		msg.group_list[msg.group_count].member_count = info.member_count;
		++msg.group_count;
	}

	size_t len = sizeof(Protocol::SCRoleGroupInfo) - sizeof(msg.group_list) + sizeof(msg.group_list[0]) * msg.group_count;
	InternalComm::Instance().SendToGameUserThroughCross(uid, &msg, len);
}

void CrossChatGroupManager::OnRoleHandOverLeadershipToOther(long long group_id, int old_leader_uid, int new_leader_uid)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	if (old_leader_uid == new_leader_uid) return;

	const ChatGroupInfo &info = group->GetGroupInfo();

	if (info.creator_uid != old_leader_uid)
	{
		return;
	}

	const ChatGroupUserInfo* old_leader_info = info.GetConstUserInfo(old_leader_uid);
	if (NULL == old_leader_info)
	{
		return;
	}

	const ChatGroupUserInfo* new_leader_info = info.GetConstUserInfo(new_leader_uid);
	if (NULL == new_leader_info)
	{
		return;
	}

	group->OnModifyLeader(new_leader_uid);

	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chat_group_handover_leadership_succ, new_leader_info->member_name);
		if (length > 0 && SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			this->SendProtocolToUser(old_leader_uid, &sm, sendlen);
		}
	}

	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chat_group_leader_handovered_to_you, old_leader_info->member_name, info.group_name);
		if (length > 0 && SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			this->SendProtocolToUser(new_leader_uid, &sm, sendlen);
		}
	}

	{
		Protocol::SCChatGroupMsgNotice notice;
		notice.group_id = info.group_id;
		notice.msg_length = SNPRINTF(notice.msg_content, sizeof(notice.msg_content), gamestring::g_chat_group_leadership_handovered, old_leader_info->member_name, new_leader_info->member_name);
		if (notice.msg_length > 0)
		{
			group->AddGroupNotice((const char*)&notice, sizeof(notice) - sizeof(notice.msg_content) + notice.msg_length);
		}
	}

	{
		Protocol::SCRoleOneGroupInfo msg;
		msg.reason = Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE;
		group->AssembleGroupInfoProtocol(&msg);

		this->SendProtocolToUser(old_leader_uid, &msg, sizeof(msg));
		this->SendProtocolToUser(new_leader_uid, &msg, sizeof(msg));
	}
}

void CrossChatGroupManager::InitChatGroup(int count, const ChatGroupInfo info_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		//CrossChatGroupManager在原服不起作用
		CrossChatGroupManager::Instance().LoadDataSucc();
		return;
	}
	if (count <= 0)
	{
		return;
	}

	for (int i = 0; i < count && i < ONCE_SEND_CHAT_GROUP_COUNT; ++i)
	{
		long long group_id = info_list[i].group_id;

		ChatGroup* group = new ChatGroup();
		group->InitChatGroupInfo(info_list[i], true);

		//m_send[group_id] = false;
		ChatGroupUniqueId group_key;
		group_key.unique_id = group_id;
		
		m_chat_group_map.insert(ChatGroupMap::value_type(group_id, group));

		for (int k = 0; k < info_list[i].member_count && k < ARRAY_ITEM_COUNT(info_list[i].member_uid_list); ++k)
		{
			m_all_server_send[IntToUid(info_list[i].member_uid_list[k].member_uid).db_index][group_id] = false;
		}

		m_all_server_send[IntToUid(info_list[i].creator_uid).db_index][group_id] = false;//一开始都未下发到原服
	}
}

void CrossChatGroupManager::Update(time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	if (CHAT_GROUP_DATA_STATUS_LOAD_FINISH != m_data_state) return;
	if (now_second > m_last_save_time + SAVE_GROUP_DATA_INTERVAL)
	{
		this->SaveChatGroup(-1);
		m_last_save_time = (unsigned int)now_second;
	}
}

void CrossChatGroupManager::OnRoleAvaChange(Role * role)
{
	//群组修改
	for (int i = 0; i < MAX_ROLE_CROSS_GROUP; i++)
	{
		long long group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[i];
		if (group_id <= 0) continue;

		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) continue;
	
		const ChatGroupUserInfo * member_info = group->GetUserInfo(role->GetUID());
		if (member_info == NULL)continue;

		const ChatGroupInfo &info = group->GetGroupInfo();
		if (info.is_cross == 0)//本地群聊的自行修改了再发往跨服
		{
			group->UpdateMemberBaseInfo(role);
		}

		//发往跨服且发往各自成员服务器
		static crossgameprotocal::GameCrossSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_AVA;
		chat_msg.uid = role->GetUID();
		chat_msg.group_id = group_id;
		chat_msg.param1 = this->GetRoleBaseAva(role);
		if(info.is_cross == 1)InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		else InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index, (const char*)&chat_msg, sizeof(chat_msg));

		this->OnGroupChangeSendGroupList(group_id);
	}
}

void CrossChatGroupManager::SaveChatGroup(long long last_save_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		CrossChatGroupManager::Instance().LoadDataSucc();
		return;
	}

	if (CHAT_GROUP_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	long long last_group_id = 0;
	ChatGroupParam param;
	param.count = 0;
	std::vector<long long> delete_group_vec;
	for (ChatGroupMap::iterator it = m_chat_group_map.begin(); it != m_chat_group_map.end(); ++it)
	{
		if (param.count >= 100) break;//超过100等下一轮

		ChatGroup* group = it->second;

		if (!group->IsValid())
		{
			param.change_state[param.count] = structcommon::CS_DELETE;
			param.info_list[param.count].group_id = it->first;
			++param.count;
			last_group_id = it->first;
			delete_group_vec.push_back(it->first);
			continue;
		}

		if (!group->IsDirty()) continue;

		param.info_list[param.count] = group->GetGroupInfo();
		param.change_state[param.count] = group->GetOldState() ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
		++param.count;
		last_group_id = it->first;
	}

	if (param.count <= 0) return;

	RMISaveCrossChatGroupBackObjectImpl *back_obj = new RMISaveCrossChatGroupBackObjectImpl();
	back_obj->m_last_save_group_id = last_group_id;
	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	if (gc.SaveCrossChatGroupAsyn(param, back_obj))
	{
		for (size_t i = 0; i < delete_group_vec.size(); ++i)
		{
			ChatGroupMap::iterator it = m_chat_group_map.find(delete_group_vec[i]);
			if (it != m_chat_group_map.end())
			{
				delete it->second;
				it->second = NULL;
				m_chat_group_map.erase(it);
			}
		}

		for (int i = 0; i < param.count; ++i)
		{
			ChatGroup* group = this->GetChatGroup(param.info_list[i].group_id);
			if (NULL != group)
			{
				group->SetOldState(true);
				group->ClearDirtyFlag();
			}
		}
	}
}

void RMISaveCrossChatGroupBackObjectImpl::SaveChatGroupRet(int ret)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	if (ret < 0)
	{
		return;
	}

	CrossChatGroupManager::Instance().ClearDirtyMark(m_last_save_group_id);
	CrossChatGroupManager::Instance().SaveChatGroup(m_last_save_group_id);
}

void RMIInitCrossChatGroupBackObjectImpl::InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (!CrossConfig::Instance().IsHiddenServer() || count <= 0)
	{	
		CrossChatGroupManager::Instance().LoadDataSucc();
		return;
	}

	CrossChatGroupManager::Instance().InitChatGroup(count, param_list);
	CrossChatGroupManager::Instance().LoadData(last_id);
}

void CrossChatGroupManager::LoadData(long long from_id)
{
	RMIInitCrossChatGroupBackObjectImpl *backobj = new RMIInitCrossChatGroupBackObjectImpl();
	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	gc.InitCrossChatGroupAsyn(from_id, backobj);
}

void CrossChatGroupManager::SendProtocolToUser(int uid, const void* msg, int msg_len)
{
	Role* old_leader = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (NULL != old_leader)
	{
		EngineAdapter::Instance().NetSend(old_leader->GetNetId(), msg, msg_len);
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(uid, msg, msg_len);
	}
}

int CrossChatGroupManager::GetRoleBaseAva(Role * role)
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

void CrossChatGroupManager::ClearDirtyMark(long long last_save_id)
{
	/*
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
	*/
}

void CrossChatGroupManager::OnServerStart()
{
	m_data_state = CHAT_GROUP_DATA_STATUS_LOADING;
	this->LoadData(-1);
}

void CrossChatGroupManager::OnServerStop()
{
	//关服之前修改在线玩家的离线标记为当前
	for (ChatGroupMap::iterator it = m_chat_group_map.begin(); it != m_chat_group_map.end(); ++it)
	{
		ChatGroup* group = it->second;
		if (NULL == group || !group->IsValid()) continue;

		group->OnServerStop();
	}

	this->SaveChatGroup(-1);
}

void CrossChatGroupManager::OnGameServerInLine(int server_id, int count, const ChatGroupInfo info_list[crossgameprotocal::GameCrossSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST])
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	if (count <= 0)
	{
		return;
	}
}

void CrossChatGroupManager::OnGameServerInLineReturnCrossData(int server_num,int* server_id_list)
{
	for (int i = 0; i < server_num; i++)
	{
		this->ClearSendAllToGame(server_id_list[i]);
	}
	/*//返回跨服群组到对应服务器

	crossgameprotocal::CrossGameSyncCrossChatGroupInfo msg;
	int count = 0;
	for (std::map<long long, ChatGroupInfo>::iterator it = m_hiden_chat_group_info.begin(); it != m_hiden_chat_group_info.end() && count < crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST; ++it)
	{
		std::map<long long, bool>::iterator send_flag = m_send.find(it->first);
		if (send_flag != m_send.end())
		{
			if (!send_flag->second)
			{
				msg.chat_group_list[count++] = it->second;
				send_flag->second = true;
			}
		}
	}

	msg.count = count;
	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::CrossGameSyncCrossChatGroupInfo) - ((crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&msg, send_length);

	if (!this->CheckSendAllToGame() && m_check_times < 10)
	{
		m_check_times += 1;
		this->OnGameServerInLineReturnCrossData(server_id);
	}
	this->ClearSendAllToGame();
	m_check_times = 0;*/
}

void CrossChatGroupManager::ClearSendAllToGame(int server_id)
{
	std::map<int, std::map<long long, bool> >::iterator it = m_all_server_send.find(server_id);
	if (it != m_all_server_send.end())
	{
		for (std::map<long long, bool>::iterator itor = it->second.begin(); itor != it->second.end(); ++itor)
		{
			itor->second = false;
		}
	}
}

bool CrossChatGroupManager::OnFragmentGameServerInLineReturnCrossData(int server_id, long long* check_send_group_list, int check_send_group_num)
{
	std::map<int, std::map<long long, bool> >::iterator it = m_all_server_send.find(server_id);
	if (it != m_all_server_send.end())
	{
		crossgameprotocal::CrossGameSyncCrossChatGroupInfo msg;
		msg.count = 0;
		for (int i = 0; i < check_send_group_num; i++)
		{
			if (check_send_group_list[i] <= 0)continue;
			std::map<long long, bool>::iterator itor = it->second.find(check_send_group_list[i]);
			if (itor != it->second.end())
			{
				if (!itor->second)//没有发送的才发送
				{
					ChatGroup* group = this->GetChatGroup(itor->first);
					if (NULL != group)
					{
						msg.chat_group_list[msg.count++] = group->GetGroupInfo();
					}
					itor->second = true;
				}
			}
		}
		msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
		msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
		int send_length = sizeof(crossgameprotocal::CrossGameSyncCrossChatGroupInfo) - ((crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));
		InternalComm::Instance().SendToAllGameThroughCross((const char *)&msg, send_length);
	
		return msg.count > 0;
	}

	return false;
}

/*
bool CrossChatGroupManager::CheckSendAllToGame()
{
	for (std::map<long long, bool>::iterator it = m_send.begin(); it != m_send.end(); ++it)
	{
		if (!it->second)
		{
			//printf("ChatManager::CheckSendAllChatGroupInfoToCross Continue!\n");
			return false;
		}
	}
	return true;
	//printf("ChatManager::CheckSendAllChatGroupInfoToCross Succ!\n");
}*/

void CrossChatGroupManager::OnCreateGroup(Role *role, GameName group_name)
{
	if (NULL == role)
	{
		return;
	}

	int invalid_index = -1;
	if (!this->RoleCanEnterGroupCount(role, true, &invalid_index))
	{
		return;
	}

	if (invalid_index == -1)return;

	int mother_server_id = 0;
	const UniqueServerID* role_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index));
	if (NULL == role_mother_server)
	{
		mother_server_id = role->GetUserId().db_index;
	}
	else
	{
		mother_server_id = role_mother_server->server_id;
	}

	long long chat_group_id = this->GetUniqueGroupId(mother_server_id);
	
	if (NULL != this->GetChatGroup(chat_group_id))
	{
			// 如果失败，可考虑给每个群组添加一个时间戳，如果群组内用户超过若干天未上线则强制解散
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_CREATE_FAIL);
			gamelog::g_log_social.printf(LL_WARNING, "CrossChatGroupManager::OnCreateGroup failed, too more group, [%s %d] group_name:%s group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_name, chat_group_id);
		return;
	}

	// 或可整合到加入群组当中
	ChatGroupInfo info;
	info.creator_uid = role->GetUID();
	info.group_id = chat_group_id;
	info.is_cross = true;
	info.member_count = 0;
	info.member_uid_list[info.member_count].member_ava = this->GetRoleBaseAva(role);
	info.member_uid_list[info.member_count].member_uid = role->GetUID();
	STRNCPY(info.member_uid_list[info.member_count].member_name, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
	info.member_uid_list[info.member_count].member_prof = role->GetBaseProfession();
	info.member_uid_list[info.member_count++].member_level = role->GetLevel();

	STRNCPY(info.group_name, group_name, sizeof(GameName));
	info.group_name[sizeof(GameName) - 1] = 0;

	m_all_server_send[role->GetUserId().db_index][chat_group_id] = false;

	ChatGroup* group = new ChatGroup();
	group->InitChatGroupInfo(info, false);

	m_chat_group_map.insert(ChatGroupMap::value_type(chat_group_id, group));

	role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[invalid_index] = info.group_id;

	//新增跨服群组同步到各个游戏服
	crossgameprotocal::CrossGameSyncCrossChatGroupInfo msg;
	msg.count = 0;
	msg.chat_group_list[msg.count++] = info;

	msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	int send_length = sizeof(crossgameprotocal::CrossGameSyncCrossChatGroupInfo) - ((crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

	InternalComm::Instance().SendToAllGameThroughCross((const char *)&msg, send_length);

	this->SendRoleOneGroupInfo(role, chat_group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);
	//printf("CrossChatGroupManager::OnCreateGroup group_id[%lld] group_member[%d] group_name[%s]\n", chat_group_id, info.member_count, info.group_name);
	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnCreateGroup [%s %d] group_name:%s group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_name, info.group_id);
}

int CrossChatGroupManager::OnRoleLeaveGroupInLocal(long long group_id, int uid, GameName user_name)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return -1;
	
	int member_idx = -1;
	if (NULL == group->GetUserInfo(uid, &member_idx))
	{
		return -2;
	}

	if (member_idx < 0 || member_idx >= CHAT_GROUP_MEMBER_MAX)
	{
		return -3;
	}

	const ChatGroupInfo& info = group->GetGroupInfo();

	if (info.creator_uid == uid)
	{
		return -4;
	}

	bool ret = group->EraseUser(uid);
	if (ret)
	{
		this->OnGroupChangeSendGroupList(group_id);
	}
	
	//操作返回
	//跨服且发往各自成员服务器
	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LEAVEN_GROUP_RET;
	chat_msg.uid = uid;
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_master, user_name, sizeof(GameName));
	chat_msg.param1 = 1;
	chat_msg.param2 = 0;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleLeaveGroupInLocal [%s %d] group_id: %lld", user_name, uid, group_id);

	return 0;
}

int CrossChatGroupManager::OnRoleLeaveLocalGroup(long long group_id, int uid, GameName user_name)
{
	return 0;
}

int CrossChatGroupManager::OnRoleCreateCrossGroup(int uid, int user_ava, int user_level, int user_prof, GameName group_name, GameName user_name, int invalid_index,long long * create_group_id)
{
	if (invalid_index == -1) return -1;

	int mother_server_id = 0;
	const UniqueServerID* role_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index));
	if (NULL == role_mother_server)
	{
		mother_server_id = IntToUid(uid).db_index;
	}
	else
	{
		mother_server_id = role_mother_server->server_id;
	}

	long long chat_group_id = this->GetUniqueGroupId(mother_server_id);
	if (NULL != this->GetChatGroup(chat_group_id))
	{
		gamelog::g_log_social.printf(LL_WARNING, "CrossChatGroupManager::OnCreateGroup failed, too more group, [%s %d] group_name:%s group_id: %lld", user_name, uid, group_name, chat_group_id);
		return -2;
	}

	// 或可整合到加入群组当中
	ChatGroupInfo info;
	info.creator_uid = uid;
	info.group_id = chat_group_id;
	info.is_cross = true;
	info.member_count = 0;
	info.member_uid_list[info.member_count].member_ava = user_ava;
	info.member_uid_list[info.member_count].member_uid = uid;
	STRNCPY(info.member_uid_list[info.member_count].member_name, user_name, sizeof(GameName));
	info.member_uid_list[info.member_count].member_prof = user_prof;
	info.member_uid_list[info.member_count++].member_level = user_level;

	STRNCPY(info.group_name, group_name, sizeof(GameName));
	info.group_name[sizeof(GameName) - 1] = 0;

	ChatGroup* group = new ChatGroup();
	group->InitChatGroupInfo(info, false);

	m_chat_group_map.insert(ChatGroupMap::value_type(chat_group_id, group));

	m_all_server_send[IntToUid(uid).db_index][chat_group_id] = true;

	{
		//新增跨服群组同步到各个游戏服
		crossgameprotocal::CrossGameSyncCrossChatGroupInfo msg;
		msg.count = 0;
		msg.chat_group_list[msg.count++] = info;

		msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
		msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
		int send_length = sizeof(crossgameprotocal::CrossGameSyncCrossChatGroupInfo) - ((crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

		InternalComm::Instance().SendToAllGameThroughCross((const char *)&msg, send_length);
	}

	*create_group_id = chat_group_id;

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnCreateGroup [%s %d] group_name:%s group_id: %lld", user_name, uid, group_name, info.group_id);
	return 0;
}

int CrossChatGroupManager::OnTempGroupBecomeOfficial(int max_num, int member_num, ChatGroupUserInfo* member_list, GameName group_name, long long * create_group_id)
{
	if (max_num <= 0 || member_num <= 0) return -1;

	int mother_server_id = 0;
	const UniqueServerID* role_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(member_list[0].member_uid).db_index));
	if (NULL == role_mother_server)
	{
		mother_server_id = IntToUid(member_list[0].member_uid).db_index;
	}
	else
	{
		mother_server_id = role_mother_server->server_id;
	}

	long long chat_group_id = this->GetUniqueGroupId(mother_server_id);
	if (NULL != this->GetChatGroup(chat_group_id))
	{
		gamelog::g_log_social.printf(LL_WARNING, "CrossChatGroupManager::OnCreateGroup failed, too more group, [%s %d] group_name:%s group_id: %lld", member_list[0].member_name, member_list[0].member_uid, group_name, chat_group_id);
		return -2;
	}

	ChatGroupInfo info;
	info.creator_uid = member_list[0].member_uid;
	info.group_id = chat_group_id;
	info.is_cross = true;
	info.member_count = 0;
	for (int i = 0; i < member_num && i < max_num; ++i)
	{
		info.member_uid_list[info.member_count].member_ava = member_list[i].member_ava;
		info.member_uid_list[info.member_count].member_uid = member_list[i].member_uid;
		F_STRNCPY(info.member_uid_list[info.member_count].member_name, member_list[i].member_name, sizeof(GameName));
		info.member_uid_list[info.member_count].member_prof = member_list[i].member_prof;
		info.member_uid_list[info.member_count].member_level = member_list[i].member_level;
		info.member_count += 1;
	}

	F_STRNCPY(info.group_name, group_name, sizeof(GameName));

	ChatGroup* group = new ChatGroup();
	group->InitChatGroupInfo(info, false);

	m_chat_group_map.insert(ChatGroupMap::value_type(chat_group_id, group));

	m_all_server_send[IntToUid(info.creator_uid).db_index][chat_group_id] = true;

	{
		//新增跨服群组同步到各个游戏服
		crossgameprotocal::CrossGameSyncCrossChatGroupInfo msg;
		msg.count = 0;
		msg.chat_group_list[msg.count++] = info;

		msg.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
		msg.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
		int send_length = sizeof(crossgameprotocal::CrossGameSyncCrossChatGroupInfo) - ((crossgameprotocal::CrossGameSyncCrossChatGroupInfo::MAX_ONE_SEND_LIST - msg.count) * sizeof(msg.chat_group_list[0]));

		InternalComm::Instance().SendToAllGameThroughCross((const char *)&msg, send_length);
	}

	if (create_group_id != NULL)
	{
		*create_group_id = chat_group_id;
	}

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnTempGroupBecomeOfficial [%s %d] group_name:%s group_id: %lld", member_list[0].member_name, member_list[0].member_uid, group_name, info.group_id);
	return 0;
}

void CrossChatGroupManager::OnRoleDismiss(long long group_id, int uid, GameName member_name)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo &info = group->GetGroupInfo();

	if (info.creator_uid != uid)
	{
		return;
	}

	if (info.is_cross == 1)
	{
		this->OnRoleDismissCrossGroup(uid, group_id, member_name);
	}
	else
	{
		this->OnRoleDismissLocalGroup(uid, group_id);
	}
}

int CrossChatGroupManager::OnRoleDismissCrossGroup(int uid, long long group_id, GameName member_name)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return -1;

	const ChatGroupInfo& info = group->GetGroupInfo();
	if (info.creator_uid != uid)
	{
		return -2;
	}

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info.member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		Role *member = mananger->GetRole(info.member_uid_list[i].member_uid);
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
		int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chat_group_dismiss_content, info.group_name);

		if (lensub > 0 && lencont > 0)
		{
			MailRoute::MailToUser(member->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	group->DismissGroup();

	ChatGroupUniqueId tmp;
	tmp.unique_id = group_id;
	crossgameprotocal::CrossGameSyncChatOpreta new_msg;

	new_msg.uid = uid;
	new_msg.group_id = group_id;
	new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_DISSMISS_GROUP_RET;
	new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&new_msg, sizeof(new_msg));

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleDismissCrossGroup [%s %d] group_id: %lld", member_name, uid, group_id);
	return 0;
}

void CrossChatGroupManager::OnRoleDismissLocalGroup(int uid, long long group_id)
{
	
}

void CrossChatGroupManager::OnRoleInviteGroupByLocal(crossgameprotocal::GameCrossSyncChatOpreta *msg)
{
	if (NULL == msg) return;

	ChatGroup* group = this->GetChatGroup(msg->group_id);
	if (NULL == group) return;

	const ChatGroupInfo &info = group->GetGroupInfo();

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	if (NULL != role)//在跨服
	{
		ChatGroupUniqueId tmp;
		tmp.unique_id = info.group_id;
		Protocol::SCRoleInviteEnterGroup s_msg;
		s_msg.invite_server_id = tmp.server_id;
		F_STRNCPY(s_msg.invite_group_name, msg->group_name, sizeof(GameName));
		s_msg.invite_group_id = msg->group_id;
		s_msg.is_cross = msg->is_cross;
		s_msg.invite_uid = msg->inviter_id;
		F_STRNCPY(s_msg.inviter_name, msg->group_master, sizeof(GameName));
		EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&s_msg, sizeof(s_msg));
	}
	else//不在跨服或者离线
	{
		if (info.is_cross != 1)return;//非跨服群组，邀请不发往游戏服
									  //发往本地服看看
		static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_INVITE_MEMBER;
		chat_msg.uid = msg->uid;
		chat_msg.group_id = msg->group_id;
		chat_msg.is_cross = msg->is_cross;
		chat_msg.inviter_id = msg->inviter_id;
		F_STRNCPY(chat_msg.group_name, msg->group_name, sizeof(GameName));
		F_STRNCPY(chat_msg.group_master, msg->group_master, sizeof(GameName));
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg->uid).db_index,
			(const char*)&chat_msg, sizeof(chat_msg), msg->uid);
	}
}

void CrossChatGroupManager::OnRoleEnterGroupInLocal(crossgameprotocal::GameCrossSyncChatOpreta * msg)
{
	if (msg == NULL) return;
	
	ChatGroup* group = this->GetChatGroup(msg->group_id);

	//发往玩家所在服务器
	crossgameprotocal::CrossGameSyncChatOpreta new_msg;
	int plat_spid = LocalConfig::Instance().GetPlatType();
	new_msg.uid = msg->uid;
	ChatGroupUniqueId tmp;
	tmp.unique_id = msg->group_id;
	if (NULL == group)
	{
		//发往玩家所在服务器
		new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
		new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_GROUP_INVALID;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(msg->uid).db_index, (const char*)&new_msg, sizeof(new_msg));
		return;
	}

	const ChatGroupInfo & info = group->GetGroupInfo();
	if (info.member_count >= CHAT_GROUP_MEMBER_MAX)
	{
		new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
		new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_GROUP_FULL;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(msg->uid).db_index, (const char*)&new_msg, sizeof(new_msg));
		return;
	}

	if (NULL != group->GetUserInfo(msg->uid))
	{
		new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
		new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_ALREADY_EXISTS;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(msg->uid).db_index, (const char*)&new_msg, sizeof(new_msg));
		return;
	}

	bool add_ret = group->AddUserInfo(msg->uid, msg->param1, msg->param2, msg->param3, msg->group_master);
	if (!add_ret)
	{
		new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
		new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_ADD_FAIL;
		InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(msg->uid).db_index, (const char*)&new_msg, sizeof(new_msg));
		gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleEnterGroupInLocal adduserinfo fail user[%s %d] param1[%d] param2[%d] param3[%d] param4[%d] param5[%d]",
			msg->group_master, msg->uid, msg->param1, msg->param2, msg->param3, msg->param4, msg->param5);
		return;
	}

	Role * member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->uid));
	Role * inviter_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->inviter_id));
	if (NULL != inviter_role)
	{
		this->OnGetGroupDetailInfo(inviter_role, info.group_id);
	}

	if (info.is_cross == 1)
	{
		if (member_role != NULL)
		{
			if (msg->param5 >= 0 && msg->param5 < MAX_ROLE_CROSS_GROUP)
			{
				member_role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[msg->param5] = info.group_id;
			}
		}
	}
	
	this->OnGroupChangeSendGroupList(info.group_id);
	//发往所有服务器 - 跨服群组修改
	F_STRNCPY(new_msg.group_master, msg->group_master, sizeof(GameName));
	new_msg.inviter_id = msg->inviter_id;
	new_msg.group_id = info.group_id;
	new_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
	new_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC;
	new_msg.param2 = msg->param1;
	new_msg.param3 = msg->param2;
	new_msg.param4 = msg->param3;
	new_msg.param5 = msg->param4;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&new_msg, sizeof(new_msg));
}

void CrossChatGroupManager::OnRoleEnterGroupReturn(crossgameprotocal::GameCrossSyncChatOpreta * msg)
{
	if (msg == NULL)return;

	/*
	std::map<long long, ChatGroupInfo>::iterator it = m_local_chat_group_info.find(msg->group_id);
	if (it == m_local_chat_group_info.end())
	{
		return;
	}

	ChatGroupInfo &info = it->second;
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager == NULL)return;
	Role * role = scene_manager->GetRole(IntToUid(msg->uid));

	switch (msg->param1)
	{
	case crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC://成功了
		if (role != NULL)role->NoticeNum(noticenum::NT_CHAT_GROUP_ENTER_SUCC);
		break;
	case -1://失败了理由是满人
		if (role != NULL)role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_FULL);
		break;
	case -2://失败了理由是你已经再这个群组了
		if (role != NULL)role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_FULL);
		break;
	default:
		break;
	}

	if (msg->param1 < 0)//失败直接返回
	{
		return;
	}
	
	if (info.member_count >= MAX_ROLE_GROUP)
	{
		if (role != NULL)role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_MAX);
		return;
	}

	if (NULL != info.GetUserInfo(msg->uid))
	{
		return;//玩家已经在群组了
	}

	bool add_ret = info.AddUserInfo(msg->uid, msg->param2, msg->param3, msg->param4, msg->group_master);

	if (!add_ret)
	{
		gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleEnterGroupReturn AddUserInfo fail uid[%d] member_ava[%d] member_level[%d] prof[%d] name[%s]",
			msg->uid, msg->param2, msg->param3, msg->param4, msg->group_master);
		return;
	}

	if (info.is_cross != 1)
	{
		Role * member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->uid));
		if (member_role != NULL)
		{
			if (msg->param5 >= 0 && msg->param5 < MAX_ROLE_LOCAL_GROUP)
			{
				member_role->GetRoleModuleManager()->GetCommonData().local_chat_group_list[msg->param5] = info.group_id;
			}
		}
	}

	this->OnGroupChangeSendGroupList(msg->group_id);
	*/
}

const ChatGroupInfo * CrossChatGroupManager::GetGroupInfo(long long group_id)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return NULL;

	return &group->GetGroupInfo();
}

bool CrossChatGroupManager::SendToGroup(long long group_id, void *msg, size_t length)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return false;

	return group->SendToGroup(msg, length);
}

bool CrossChatGroupManager::RoleCanEnterGroupCount(Role *role, bool is_cross, int *invalid_index)
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

	if (is_cross)
	{
		for (int cross_group = 0; cross_group < MAX_ROLE_CROSS_GROUP; cross_group++)
		{
			long long &group_id = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[cross_group];
			if (group_id > 0)//跨服接通中，同时玩家的群组id不为空
			{
				if (NULL == this->GetChatGroup(group_id))
				{
					group_id = 0;
					continue;
				}
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

void CrossChatGroupManager::SendRoleGroupInfo(Role *role, int reason/* = Protocol::SCRoleGroupInfo::SEDN_TYPE_REASON_DEF*/)
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

		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) continue;


		//查到了这个群组，先检查一下玩家是否在这个群组里面	
		if (NULL == group->GetUserInfo(role->GetUID()))
		{
			//玩家不在群组里面了
			group_id = 0;//清除玩家群组标记
			continue;
		}

		const ChatGroupInfo &info = group->GetGroupInfo();
		memset(msg.group_list[msg.group_count].top_user_ava, 0, sizeof(msg.group_list[msg.group_count].top_user_ava));
		int top_index = 0;
		for (int member_info = 0;member_info < info.member_count && top_index < 3; member_info++)
		{
			if(info.member_uid_list[member_info].member_uid == 0)continue;
			msg.group_list[msg.group_count].top_user_ava[top_index++] = info.member_uid_list[member_info].member_ava;
		}
		msg.group_list[msg.group_count].group_id = info.group_id;
		msg.group_list[msg.group_count].is_cross = info.is_cross;
		STRNCPY(msg.group_list[msg.group_count].group_name, info.group_name, sizeof(GameName));
		msg.group_list[msg.group_count].group_master_id = info.creator_uid;
		msg.group_list[msg.group_count].member_count = info.member_count;
		++msg.group_count;
	}
	
	size_t len = sizeof(Protocol::SCRoleGroupInfo) - sizeof(msg.group_list) + sizeof(msg.group_list[0]) * msg.group_count;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&msg, len);
}

void CrossChatGroupManager::SendRoleOneGroupInfo(Role* role, long long group_id, int reason /*= Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE*/)
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
		const ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) return;

		if (!group->IsMember(role->GetUID()))
		{
			return;
		}

		const ChatGroupInfo &info = group->GetGroupInfo();
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

void CrossChatGroupManager::OnChangeGroupNotice(Role * role, GameName group_name, GuildNotice group_notice, long long group_id)
{
	if (NULL == role) return;

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo &info = group->GetGroupInfo();

	if (info.creator_uid != role->GetUID())
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return;
	}

	group->OnChangeGroupNotice(group_name, group_notice);

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	for (int member_idx = 0; member_idx < info.member_count && member_idx < CHAT_GROUP_MEMBER_MAX; ++member_idx)
	{
		if (scene_manager == NULL)continue;
		Role *member_role = scene_manager->GetRole(IntToUid(info.member_uid_list[member_idx].member_uid));
		if (member_role != NULL)
		{
			this->OnGetGroupDetailInfo(member_role, group_id);
		}
	}

	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_GROUP_NAME_OR_NOTICE;//操作传到跨服，直接发往各自群员游戏服
	chat_msg.uid = role->GetUID();
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_name, group_name, sizeof(GameName));
	F_STRNCPY(chat_msg.group_notice, group_notice, sizeof(GuildNotice));
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	this->OnGroupChangeSendGroupList(group_id);

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnDismissGroup [%s %d] group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_id);
}

void CrossChatGroupManager::OnRoleLeaveGroup(Role *role, long long group_id, bool notice)
{
	//if (CrossConfig::Instance().IsHiddenServer())return;
	if (NULL == role)
	{
		return;
	}
	CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
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


	const ChatGroupInfo& info = group->GetGroupInfo();

	if (info.creator_uid == role->GetUID())
	{
		// 创建者不能退出，只能解散
		role->NoticeNum(errornum::EN_CRATER_LEAVE_CHAT_GROUP_FAIL);
		return;
	}

	if (NULL == group->GetUserInfo(role->GetUID()))
	{
		return;
	}
	else
	{
		if (info.is_cross)
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

	bool ret = group->EraseUser(role->GetUID());
	if (!ret)
	{
		gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleLeaveGroup [%s %d] ReplaceUserInfo fail group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_id);
	}

	if (info.is_cross)
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

	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_LEAVEN_GROUP_RET;
	chat_msg.uid = role->GetUID();
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_master, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
	chat_msg.param1 = 1;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	this->SendRoleOneGroupInfo(role, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
	this->OnGroupChangeSendGroupList(group_id);

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleLeaveGroup [%s %d] group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_id);
}

void CrossChatGroupManager::OnDismissGroup(Role *role, long long group_id)
{
	if (NULL == role)
	{
		return;
	}

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
		{
			long long &role_group = role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
			if (role_group == group_id)
			{
				role_group = 0;
			}
		}
		this->SendRoleOneGroupInfo(role, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
		return;
	}

	const ChatGroupInfo& info = group->GetGroupInfo();
	if (info.creator_uid != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_DISMISS_RIGHT);
		return;
	}

	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info.member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		Role *member = mananger->GetRole(info.member_uid_list[i].member_uid);
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
		int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chat_group_dismiss_content, info.group_name);

		if (lensub > 0 && lencont > 0)
		{
			MailRoute::MailToUser(member->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	//本地发往跨服解散群组
	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_DISSMISS_GROUP_RET;
	chat_msg.uid = info.creator_uid;
	chat_msg.group_id = group_id;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
	
	group->DismissGroup();

	role->GetRoleModuleManager()->NoticeNum(noticenum::NT_CHAT_GROUP_NOTICE_DISMISS);
	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnDismissGroup [%s %d] group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_id);
}

void CrossChatGroupManager::OnRoleEnterGroup(Role *role, long long group_id, int invite_uid)
{
	if (NULL == role)
	{
		return;
	}

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_INVALID);
		return;
	}

	ChatGroupUniqueId tmp;
	tmp.unique_id = group_id;

	if (role->GetUID() == invite_uid)
	{
		return;
	}

	const ChatGroupInfo & info = group->GetGroupInfo();
	int invalid_index = 0;
	if (!this->RoleCanEnterGroupCount(role, 0 != info.is_cross, &invalid_index))
	{
		return;
	}
	if (info.is_cross == 1)
	{
		if (NULL != group->GetUserInfo(role->GetUID()))
		{
			role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_FULL);
			return;
		}

		bool add_ret = group->AddUserInfo(role->GetUID(), role->GetAvatarType(), role->GetLevel(), role->GetBaseProfession(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName());
		if (!add_ret)
		{
			gamelog::g_log_social.printf(LL_INFO, "ChatManager::OnCrossEnterGroup [%s %d] group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), group_id);
			return;
		}

		role->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[invalid_index] = info.group_id;
		
		this->SendRoleOneGroupInfo(role, group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);

		Role * inviter = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(invite_uid));
		if (inviter != NULL)
		{
			this->SendRoleOneGroupInfo(inviter, group_id, Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE);
			this->OnGetGroupDetailInfo(inviter, info.group_id);
		}

		//发往所有本地服操作
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = role->GetUID();
		msg.inviter_id = invite_uid;
		F_STRNCPY(msg.group_master, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP_RET;
		msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC;
		msg.param2 = role->GetAvatarType();
		msg.param3 = role->GetLevel();
		msg.param4 = role->GetProfession();
		msg.param5 = invalid_index;
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		//发往本地操作
		crossgameprotocal::CrossGameSyncChatOpreta msg;
		msg.group_id = group_id;
		msg.uid = role->GetUID();
		msg.inviter_id = invite_uid;
		F_STRNCPY(msg.group_master, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
		msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_ENTER_GROUP;
		msg.param1 = role->GetAvatarType();
		msg.param2 = role->GetLevel();
		msg.param3 = role->GetProfession();
		msg.param4 = invalid_index;
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), tmp.server_id, (const char*)&msg, sizeof(msg));
	}
}

void CrossChatGroupManager::OnRoleInviteGroup(Role *role, int be_invite, long long group_id)
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

	ChatGroupUniqueId tmp;
	tmp.unique_id = group_id;

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo & info = group->GetGroupInfo();
	if (info.is_cross == 1)
	{
		Role *be_invite_role = World::GetInstWorld()->GetSceneManager()->GetRole(be_invite);
		if (NULL == be_invite_role)//跨服找不到人去玩家所在的服务器找一下
		{
			int plat_spid = LocalConfig::Instance().GetPlatType();
			crossgameprotocal::CrossGameSyncChatOpreta msg;
			msg.group_id = group_id;
			msg.uid = be_invite;
			msg.inviter_id = role->GetUID();
			msg.is_cross = info.is_cross;
			F_STRNCPY(msg.group_name, info.group_name, sizeof(GameName));
			F_STRNCPY(msg.group_master, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
			msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_INVITE_MEMBER;
			InternalComm::Instance().SendToGameThroughCross(plat_spid, IntToUid(be_invite).db_index, (const char*)&msg, sizeof(msg));
			return;
		}
		else
		{
			Protocol::SCRoleInviteEnterGroup msg;
			msg.invite_server_id = tmp.server_id;
			STRNCPY(msg.invite_group_name, info.group_name, sizeof(GameName));
			msg.invite_group_id = group_id;
			msg.is_cross = info.is_cross;
			msg.invite_uid = role->GetUID();
			STRNCPY(msg.inviter_name, role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
			EngineAdapter::Instance().NetSend(be_invite_role->GetNetId(), (char *)&msg, sizeof(msg));

			role->NoticeNum(noticenum::NT_CHAT_GROUP_IVENTE_SUCC);
		}
	}
	else
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_INVITE_ERR);
		return;
	}
}

void CrossChatGroupManager::OnKickMemberFromGroup(Role *role, int be_kick, long long group_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	if (NULL == role)
	{
		return;
	}

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo & group_info = group->GetGroupInfo();

	if (group_info.creator_uid != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_CHAT_GROUP_KICK_RIGHT);
		return;
	}

	if (role->GetUID() == be_kick)
	{
		return;
	}
	bool found = false;
	for (int i = 0; i < CHAT_GROUP_MEMBER_MAX && i < group_info.member_count; ++i)
	{
		if (group_info.member_uid_list[i].member_uid == be_kick)
		{
			found = true;
			break;
		}
	}

	if (!found)return;
	int plat_spid = LocalConfig::Instance().GetPlatType();
	ChatItemUniqueSeq tmp;
	tmp.unique_seq = group_info.group_id;

	if (group_info.is_cross == 1)
	{
		this->OnRoleBeKickOutGroup(be_kick, group_id);//踢这个玩家
	}
	else
	{
		//发回本服处理了再回来
		static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER;
		chat_msg.uid = be_kick;
		chat_msg.group_id = group_id;
		chat_msg.inviter_id = role->GetUID();
		InternalComm::Instance().SendToGameThroughCross(plat_spid, tmp.server_id, (const char*)&chat_msg, sizeof(chat_msg));
		return;
	}
	this->OnGetGroupDetailInfo(role, group_id);
	this->OnGroupChangeSendGroupList(group_id);
	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnKickMemberFromGroup master:[%s %d] bekick_role:[%d] group_id: %lld", role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), role->GetUID(), be_kick, group_id);
}

void CrossChatGroupManager::OnRoleBeKickOutGroup(int uid, long long group_id)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	const ChatGroupInfo& info = group->GetGroupInfo();

	if (info.creator_uid == uid)
	{
		// 创建者不能被踢出
		return;
	}

	if (group->GetUserInfo(uid) == NULL)
	{
		return;
	}

	if (!group->EraseUser(uid))
	{
		gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleBeKickOutGroup ReplaceUserInfo fail [%d] group_id: %lld", uid, group_id);
	}

	SceneManager *scene_manager = World::GetInstWorld()->GetSceneManager();
	Role *member = scene_manager->GetRole(IntToUid(uid));
	if (member != NULL)
	{
		if (info.is_cross == 1)
		{
			for (int j = 0; j < MAX_ROLE_CROSS_GROUP; ++j)
			{
				long long &role_group = member->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[j];
				if (role_group == group_id)
				{
					role_group = 0;
				}
			}
		}
		
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_chat_group_tick_out_content, info.group_name);
		if (length > 0)
		{
			static Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
	}

	if (info.is_cross == 1)
	{
		static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
		chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_TICKOUT_MEMBER_RET;
		chat_msg.uid = uid;
		chat_msg.group_id = group_id;
		chat_msg.param1 = crossgameprotocal::CHAT_OPRETA_RET_TYPE_SUCC;
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
	}

	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleBeKickOutGroup [%d] group_id: %lld", uid, group_id);
}

void CrossChatGroupManager::OnRoleBeKickOutGroupRet(int uid, long long group_id)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group) return;

	const ChatGroupInfo& info = group->GetGroupInfo();

	if (info.creator_uid == uid)
	{
		// 创建者不能被踢出
		return;
	}

	if (group->GetUserInfo(uid) == NULL)
	{
		return;
	}

	if (!group->EraseUser(uid))
	{
		gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleBeKickOutGroup ReplaceUserInfo fail [%d] group_id: %lld", uid, group_id);
	}
	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	Role * member = scene_manager->GetRole(IntToUid(uid));
	if (member != NULL)
	{
		this->SendRoleOneGroupInfo(member, group_id, Protocol::SCRoleOneGroupInfo::REASON_REMOVE);
	}
	gamelog::g_log_social.printf(LL_INFO, "CrossChatGroupManager::OnRoleBeKickOutGroup [%d] group_id: %lld", uid, group_id);
}

void CrossChatGroupManager::OnGetGroupDetailInfo(Role *role, long long group_id)
{
	if (NULL == role)
	{
		return;
	}

	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	Protocol::SCGroupAllInfo msg;
	msg.info = group->GetGroupInfo();

	EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&msg, sizeof(msg));
}

void CrossChatGroupManager::OnGroupChangeSendGroupList(long long group_id)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	const ChatGroupInfo &info = group->GetGroupInfo();
	SceneManager *mananger = World::GetInstWorld()->GetSceneManager();
	for (int i = 0; i < info.member_count && i < CHAT_GROUP_MEMBER_MAX; ++i)
	{
		//群组信息改变，下发略缩图给群员
		Role * member_role = mananger->GetRole(IntToUid(info.member_uid_list[i].member_uid));
		if (member_role != NULL)
		{
			this->SendRoleOneGroupInfo(member_role, group_id, Protocol::SCRoleOneGroupInfo::REASON_INFO_CHANGE);
			//this->SendRoleGroupInfo(member_role);//能找到就发整个列表
		}
	}
}

void CrossChatGroupManager::OnRoleChangeName(long long group_id, int member_id, GameName new_name)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		ChatGroup* group = this->GetChatGroup(group_id);
		if (NULL == group) return;

		const ChatGroupInfo &info = group->GetGroupInfo();
		int member_idx = -1;
		const ChatGroupUserInfo * member_info = group->GetUserInfo(member_id, &member_idx);
		if (member_info == NULL) return;
		
		group->OnMemberChangeName(member_id, new_name);

		if (info.is_cross == 1)
		{
			//发往各自成员服务器
			static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
			chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_NAME_RET;
			chat_msg.uid = member_id;
			chat_msg.group_id = group_id;
			F_STRNCPY(chat_msg.group_master, new_name, sizeof(GameName));
			InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
		}
		
		this->OnGroupChangeSendGroupList(group_id);
	}
}

void CrossChatGroupManager::OnRoleChangeProf(long long group_id, int member_id, int new_prof, int new_ava)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	const ChatGroupInfo &info = group->GetGroupInfo();

	group->OnMemberChangeProf(member_id, new_prof, new_ava);

	//发往各自成员服务器
	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_PROF;
	chat_msg.uid = member_id;
	chat_msg.group_id = group_id;
	chat_msg.param1 = new_prof;
	chat_msg.param2 = new_ava;

	if (info.is_cross == 1)
	{
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(member_id).db_index, (const char*)&chat_msg, sizeof(chat_msg));
	}

	this->OnGroupChangeSendGroupList(group_id);
}

void CrossChatGroupManager::OnRoleChangeAva(long long group_id, int member_id, int new_ava)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	group->OnMemberChangeAva(member_id, new_ava);

	this->OnGroupChangeSendGroupList(group_id);
}

void CrossChatGroupManager::OnGroupNameOrNoticeChange(long long group_id, int uid, GameName group_name, GuildNotice group_notice)
{
	ChatGroup* group = this->GetChatGroup(group_id);
	if (NULL == group)
	{
		return;
	}

	const ChatGroupInfo &info = group->GetGroupInfo();
	group->OnChangeGroupNotice(group_name, group_notice);

	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	for (int member_idx = 0; member_idx < info.member_count && member_idx < CHAT_GROUP_MEMBER_MAX; ++member_idx)
	{
		if (scene_manager == NULL)continue;
		Role *member_role = scene_manager->GetRole(IntToUid(info.member_uid_list[member_idx].member_uid));
		if (member_role != NULL)
		{
			this->OnGetGroupDetailInfo(member_role, group_id);
		}
	}

	static crossgameprotocal::CrossGameSyncChatOpreta chat_msg;
	chat_msg.uid = uid;
	chat_msg.opreta_type = crossgameprotocal::CHAT_OPRETA_TYPE_CHANGE_GROUP_NAME_OR_NOTICE;//操作传到跨服，直接发往各自群员游戏服
	chat_msg.group_id = group_id;
	F_STRNCPY(chat_msg.group_name, group_name, sizeof(GameName));
	F_STRNCPY(chat_msg.group_notice, group_notice, sizeof(GuildNotice));
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&chat_msg, sizeof(chat_msg));

	this->OnGroupChangeSendGroupList(group_id);
}

unsigned long long CrossChatGroupManager::GetUniqueGroupId(int _create_server_id)
{
	ChatGroupUniqueId tmp;

	tmp.server_id = _create_server_id;
	tmp.seq = m_chat_group_seq++;
	tmp.time = (unsigned int)EngineAdapter::Instance().Time();

	return tmp.unique_id;
}
