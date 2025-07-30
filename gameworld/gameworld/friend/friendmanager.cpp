#include "friendmanager.h"

#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "rmiclient/rmiglobal.hpp"
#include "internalcomm.h"
#include "serverlogic.h"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/marriage/marriage.hpp"
#include "other/route/mailroute.hpp"
#include "global/cross/crossmanager.h"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "global/usercache/usercache.hpp"
#include "friend/friendintimacy.hpp"
#include "global/qingyuan/qingyuanmanager.h"

#include "global/datasynccheck/datasynccheck.hpp"


void RMIInitFriendBackObjectImpl::InitFriendRet(int _ret, FriendListParam & _friend_list_param)
{
	FriendManager::Instance().InitFriendParam(_ret, _friend_list_param);
}

void RMIInitBlackBackObjectImpl::InitBlackRet(int _ret, BlacklistsParam & _black_list_param)
{
	FriendManager::Instance().InitBlackParam(_ret, _black_list_param);
}

FriendManager & FriendManager::Instance()
{
	static FriendManager st_fm;
	return st_fm;
}

void FriendManager::InitFriendParam(int _ret, FriendListParam & _friends_list_param)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (0 != _ret)
	{
		printf(" FriendManager::InitFriendParam Failed . ret:%d  \n", _ret);
		ServerLogic::GetInstServerLogic()->StopServer("FriendManager::InitFriendParam");
		EngineAdapter::Instance().StopGame();
		return;
	}
	else
	{
		for (int i = 0; i < _friends_list_param.count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
		{
			int role_id = _friends_list_param.friend_list[i].role_id;
			int friend_id = _friends_list_param.friend_list[i].friend_uid;
			long long friend_for_index = this->AddFriendInfo(role_id, friend_id, _friends_list_param.friend_list[i]);
			this->AddUserFriendRelation(role_id, friend_for_index);
			m_friend_old_state.insert(friend_for_index);// 标记该数据是已经存在的数据
		}

		if (_friends_list_param.count >= MAX_ONCE_FRIENDS_COUNT)
		{
			RMIInitFriendBackObjectImpl *impl = new RMIInitFriendBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitFriendDataAsyn(_friends_list_param.id_from , impl);
		}
		else
		{
			printf("Init FriendManager::Friend Succ,  size:%u  \n", (unsigned int)m_friend_item_map.size());
			m_friends_load_flag = true;
		}
	}

	this->CheckLoadFinish();
}

void FriendManager::InitBlackParam(int _ret, BlacklistsParam & _blacks_list_param)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (0 != _ret)
	{
		printf(" FriendManager::InitBlackParam Failed . ret:%d  \n", _ret);
		ServerLogic::GetInstServerLogic()->StopServer("FriendManager::InitBlackParam");
		EngineAdapter::Instance().StopGame();
		return;
	}
	else
	{
		for (int i = 0; i < _blacks_list_param.count && i < MAX_ONCE_BLACK_COUNT; ++i)
		{
			int role_id = _blacks_list_param.black_list[i].role_id;
			int black_id = _blacks_list_param.black_list[i].uid;
			long long black_for_index = this->AddBlackInfo(role_id, black_id, _blacks_list_param.black_list[i]);
			this->AddUserBlackRelation(role_id, black_for_index);
			m_black_old_state.insert(black_for_index);// 标记该数据是已经存在的数据
		}

		if (_blacks_list_param.count >= MAX_ONCE_BLACK_COUNT)
		{
			RMIInitBlackBackObjectImpl *impl = new RMIInitBlackBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitBlackListDataAsyn(_blacks_list_param.id_from , impl);
		}
		else
		{
			printf("Init FriendManager::Black Succ,  size:%u  \n", (unsigned int)m_black_item_map.size());
			m_blacks_load_flag = true;
		}
	}

	this->CheckLoadFinish();
}

void FriendManager::OnConnectHiddenSucc()
{
	if (!this->IsLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SyncAllFriendDataToHidden();
	this->SyncAllBlackDataToHidden();
}

void FriendManager::OnServerStart()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	{
		// 初始化好友信息
		RMIInitFriendBackObjectImpl *impl = new RMIInitFriendBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitFriendDataAsyn(0, impl);
	}

	{
		// 初始化黑名单	
		RMIInitBlackBackObjectImpl *impl = new RMIInitBlackBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitBlackListDataAsyn(0, impl);
	}
}

void FriendManager::OnServerStop()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	this->Save();
}

bool FriendManager::IsLoadFinish() const
{
	if (CrossConfig::Instance().IsHiddenServer())
		return true;

	return (FRIEND_DATA_STATUS_LOAD_FINISH == m_data_state);
}

void FriendManager::Update(unsigned long interval, time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (FRIEND_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	static time_t last_save_time = now_second;

	if (now_second >= last_save_time + FRIEND_SAVE_INTERVAL)
	{
		this->Save();
		last_save_time = now_second;
	}
}

void FriendManager::OnGetFriendInfoFromGameWorld(crossgameprotocal::GameHiddenGetFriendInfo * _get_info)
{
	if (NULL == _get_info || !CrossConfig::Instance().IsHiddenServer())
		return;

	switch (_get_info->info_type)
	{
	case crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_FRIEND_LIST:
		{
			this->SendFriendListToRoleToGameWorld(_get_info->usid, _get_info->role_id);
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_ASK_LIST:
		{
			this->SendAddListToRoleToGameWorld(_get_info->usid, _get_info->role_id);
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_BLACK_LIST:
		{
			this->SendBlackListToRoleToGameWorld(_get_info->usid, _get_info->role_id);
		}
		break;
	default:
		break;
	}
}

void FriendManager::OnAddFriendReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReq * _req)
{
	// 隐藏服调用 , 处理来至主服的信息
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	int role_error_num = 0;
	if (!this->CanAddFriend(_req->role_id, _req->target_id, &role_error_num, NULL))
	{
		this->AddFriendReqRetToGameWorld(_req->role_id, role_error_num);
		return;
	}

	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_req->target_id));
	if (NULL != target_role)
	{
		// 目标玩家在隐藏服内
		this->AddTargetAskListByHidden(_req, true);
		this->SendAddFriendRouteByHidden(_req, target_role);
	}
	else
	{
		// 目标玩家不在在隐藏服内
		this->AddTargetAskListByHidden(_req, false);
	}

	if (0 == _req->is_yijian)
		this->AddFriendReqRetToGameWorld(_req->role_id, noticenum::NT_SEND_ADD_FRIEND_REQ_SUCC);
}

void FriendManager::OnAddFriendReplyReqFromGameWorld(crossgameprotocal::GameHiddenAddFriendReplyReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	this->AddFriendReplyByHidden(_req->role_id, _req->apply_id, _req->is_accept);
}

void FriendManager::OnRemoveFriendReqFromGameWorld(crossgameprotocal::GameHiddenRemoveFriendReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	this->RemoveFriendByHidden(_req->role_id, _req->target_id, __FUNCTION__);
}

void FriendManager::OnAddBlackReqFromGameWorld(crossgameprotocal::GameHiddenAddBlackReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	int error_num = 0;
	if (!this->AddBlackByHidden(_req->role_id, _req->target_id, &error_num))
	{
		if (0 != error_num)
		{
			this->AddFriendReqRetToGameWorld(_req->role_id, error_num);
		}
	}
	else
	{
		this->SendBlackChangeToRoleToGameWorld( _req->role_id, _req->target_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE);
	}
}

void FriendManager::OnRemoveBlackReqFromGameWorld(crossgameprotocal::GameHiddenRemoveBlackReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	this->RemoveBlackByHidden(_req->role_id, _req->target_id);
	this->SendBlackChangeToRoleToGameWorld( _req->role_id, _req->target_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_DEL);
}

void FriendManager::OnFriendGroupReqFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	switch (_req->req_type)
	{
	case crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_DEL_GROUP:
		{
			this->DelGroupByHidden(_req->role_id, _req->group_type);
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_EDIT_GROUP:
		{
			this->EditGroupFromGameWorld(_req);
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MOVE_GROUP:
		{
			this->MoveGroupFromGameWorld(_req);
		}
		break;
	default:
		break;
	}
}

void FriendManager::OnRecentlyReqFromGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	int info_type = crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_INVALID;
	switch (_req->req_type)
	{
	case crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_CHAT:
		{
			info_type = crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_CHAT;
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_CHAT_INFO:
		{
			info_type = crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_CHAT_INFO;
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_TEAM_INFO:
		{
			info_type = crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_TEAM_INFO;
		}
		break;
	case crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_TEAM:
		{
			info_type = crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_TEAM;
		}
	default:
		return;
	}

	this->OnSendRecentlyInfoToGameWorld(_req, info_type);
}

void FriendManager::OnRoleLoginLogoutFriendNotifyFromGameWorld(crossgameprotocal::GameHiddenRoleLoginLogoutFriendNotify * _notify)
{
	if (NULL == _notify || !CrossConfig::Instance().IsHiddenServer())
		return;

	// login or logout 都是这个
	if (UserStatusNode::STATUS_TYPE_OFFLINE == _notify->online_status)
	{
		this->OnLogoutNotifyFriendByHidden(_notify->role_id);
		this->OnLogoutNotifyBlackByHidden(_notify->role_id);
	}
	else
	{
		this->OnLoginNotifyFriendByHidden(_notify->role_id, _notify->online_status);
		this->OnLoginNotifyBlackByHidden(_notify->role_id, _notify->online_status);
	}
}

void FriendManager::OnSyncFrinedAskListChangeFromHidden(crossgameprotocal::HiddenGameFriendAskListChange * _change)
{
	if (NULL == _change || CrossConfig::Instance().IsHiddenServer())
		return;

	switch (_change->change_type)
	{
	case crossgameprotocal::HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INSERT:
		{
			// 已经验证过了 , 这里只是同步信息
			this->AddTargetAskListByGameWorld(_change->target_id, _change);

			if (!_change->is_send_friend_route)
			{
				Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_change->target_id));
				if (NULL != target_role)
				{
					this->SendAddFriendRouteByGameWorld(_change, target_role);
				}
			}
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_DELETE:
		{
			this->RemoveTargetAskList(_change->target_id, _change->role_id, false);
		}
		break;
	default:
		break;
	}
}

void FriendManager::OnSyncFriendListToRoleFromHidden(crossgameprotocal::HiddenGameFriendListToRole * _friend_list)
{
	if (NULL == _friend_list || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_friend_list->role_id));
	if (NULL == role)
		return;

	Protocol::SCFriendInfo protocol;

	const FriendCommonParam & common_param = role->GetRoleModuleManager()->GetFriendCommonData();
	for (int i = 0; i < MAX_FRIEND_GROUP_NUM; ++i)
	{
		memcpy(protocol.group_name[i], common_param.group_name[i], sizeof(protocol.group_name[i]));
	}

	int count = 0;
	for (int i = 0; i < _friend_list->count && i < MAX_FRIEND_NUM; ++i)
	{
		protocol.friend_list[count].sex = _friend_list->friend_list[i].sex;
		protocol.friend_list[count].prof = _friend_list->friend_list[i].prof;
		protocol.friend_list[count].is_online = _friend_list->friend_list[i].is_online;
		F_STRNCPY(protocol.friend_list[count].role_name, _friend_list->friend_list[i].role_name, sizeof(protocol.friend_list[count].role_name));
		F_STRNCPY(protocol.friend_list[count].family_name, _friend_list->friend_list[i].family_name, sizeof(protocol.friend_list[count].role_name));
		protocol.friend_list[count].user_id = _friend_list->friend_list[i].user_id;
		protocol.friend_list[count].intimacy = _friend_list->friend_list[i].intimacy;
		protocol.friend_list[count].level = _friend_list->friend_list[i].level;
		protocol.friend_list[count].vip_level = _friend_list->friend_list[i].vip_level;
		protocol.friend_list[count].group_type = _friend_list->friend_list[i].group_type;
		protocol.friend_list[count].avatar_timestamp = _friend_list->friend_list[i].avatar_timestamp;
		protocol.friend_list[count].avatar_type = _friend_list->friend_list[i].avatar_type;
		protocol.friend_list[count].headshot_id = _friend_list->friend_list[i].headshot_id;
		protocol.friend_list[count].top_level = _friend_list->friend_list[i].top_level;
		protocol.friend_list[count].last_online_timestamp = _friend_list->friend_list[i].last_online_timestamp;

		++count;
		if (count >= MAX_FRIEND_NUM)
		{
			break;
		}
	}

	protocol.count = count;
	size_t length = sizeof(protocol) - ((MAX_FRIEND_NUM - protocol.count) * sizeof(protocol.friend_list[0]));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::OnGetBlackListFromHidden(crossgameprotocal::HiddenGameBlackList * _list)
{
	if (NULL == _list || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_list->role_id));
	if (NULL == role)
		return;

	Protocol::SCBlackInfo protocol;

	int count = 0;
	for (int i = 0; i < _list->count && i < MAX_BLACK_NUM; ++i)
	{
		protocol.black_list[count].user_id = _list->black_info[i].user_id;
		F_STRNCPY(protocol.black_list[count].gamename, _list->black_info[i].role_name, sizeof(protocol.black_list[count].gamename));
		F_STRNCPY(protocol.black_list[count].family_name, _list->black_info[i].family_name, sizeof(protocol.black_list[count].family_name));
		protocol.black_list[count].avatar_type = _list->black_info[i].avatar_type;
		protocol.black_list[count].is_online = _list->black_info[i].is_online;
		protocol.black_list[count].vip_level = _list->black_info[i].vip_level;
		protocol.black_list[count].level = _list->black_info[i].level;
		protocol.black_list[count].prof = _list->black_info[i].prof;
		protocol.black_list[count].headshot_id = _list->black_info[i].headshot_id;
		protocol.black_list[count].reserve_sh = 0;
		protocol.black_list[count].top_level = _list->black_info[i].top_level;
		protocol.black_list[count].avatar_timestamp = _list->black_info[i].avatar_timestamp;
		protocol.black_list[count].last_online_timestamp = _list->black_info[i].last_online_timestamp;

		++count;
		if (count >= MAX_BLACK_NUM)
		{
			break;
		}
	}

	protocol.count = count;
	size_t length = sizeof(protocol) - ((MAX_BLACK_NUM - protocol.count) * sizeof(protocol.black_list[0]));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::OnGetBlackListChangeFromHidden(crossgameprotocal::HiddenGameBlackListChange * _change)
{
	if (NULL == _change || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_change->role_id));
	if (NULL != role)
	{
		this->SendBlackChangeToRoleFromHidden(role, _change);
	}
}

void FriendManager::OnAskListToRoleFromHidden(crossgameprotocal::HiddenGameAskListToRole * _ask_list)
{
	if (NULL == _ask_list || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_ask_list->role_id));
	if (NULL != role)
	{
		this->SendAskListToRoleFromHidden(role, _ask_list);
	}
}

void FriendManager::OnRecentlyInfoFromHidden(crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _info || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_info->role_id));
	if (NULL == role)
		return;

	switch (_info->info_type)
	{
	case crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_CHAT:
		{
			this->AddRecentlyChatFromHidden(role, _info);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_CHAT_INFO:
		{
			this->SendRecentlyChatInfoFromHidden(role, _info);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_TEAM_INFO:
		{
			this->SendRecentlyTeamInfoFromHidden(role, _info);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_TEAM:
		{
			this->AddRecentlyTeamFromHidden(role, _info);
		}
		break;
	default:
		break;
	}
}

void FriendManager::OnFriendChangeToRoleFromHidden(crossgameprotocal::HiddenGameFriendChangeToRole * _msg)
{
	if (NULL == _msg || CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_msg->role_id));
	if (NULL == role)
		return;

	this->SendFriendChangeToRoleFromHidden(role, _msg);
}

void FriendManager::OnLogin(Role * _role)
{
	if (NULL == _role)
		return;

	// 主服不在做任何逻辑相关操作 , 所有逻辑相关都传递到隐藏服去做处理
	// 主服只做数据读取保存和接受来至跨服数据的同步

	// 0. 通知好友玩家上线
	this->OnLoginNotify(_role->GetUID());

	// 1. 获取好友申请列表
	this->SendAddList(_role);

	// 2. 获取好友列表
	this->SendFriendListToRole(_role);

	// 3. 获取黑名单列表
	this->SendBlackListToRole(_role);

	// 4. 获取最近组队信息
	this->SendRecentlyTeamInfo(_role);

	// 5. 获取近聊信息
	this->SendRecentlyChatInfo(_role);
}

void FriendManager::OnLogout(Role * _role)
{
	if (NULL == _role)
		return;

	this->OnLogoutNotify(_role->GetUID());
}

void FriendManager::OnClientReq(Role * _role, Protocol::CSFriendReq * _req)
{
	if (NULL == _role || NULL == _req)
		return;

	if (!FunOpen::IsFunOpen(_role, FUN_TYPE_FRIEND, true))
	{
		return;
	}

	if (!CrossManager::Instance().IsCrossServerConnected())
	{
		_role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return ;
	}

	switch (_req->req_type)
	{
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_INFO:
		{
			this->SendFriendListToRole(_role);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_ADD:
		{
			this->AddFriendReq(_role, _req->param1, _req->param2 != 0);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_ADD_REPLY:
		{
			this->AddFriendReply(_role, _req->param1, _req->param2 != 0);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_DEL:
		{
			this->RemoveFriend(_role, _req->param1);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_ADD_BLACK:
		{
			this->AddBlack(_role, _req->param1);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_DEL_BLACK:
		{
			this->RemoveBlack(_role, _req->param1);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_DEL_GROUP:
		{
			this->DelGroup(_role, _req->param1);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_TYPE_ASK_LIST:
		{
			this->SendAddList(_role);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_ADD_RECENTLY_CHAT:
		{
			this->AddRecentlyChat(_role, _req->param1);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_RECENTLY_CHAT_INFO:
		{
			this->SendRecentlyChatInfo(_role);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_RECENTLY_TEAM_INFO:
		{
			this->SendRecentlyTeamInfo(_role);
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_GIVE_FLOWER:
		{
			// 没了 移出去了
		}
		break;
	case Protocol::CSFriendReq::FRIEND_REQ_CHECK_IS_ONLINE:
		{
			this->SendRoleOnlineStatus(_role, _req->param1);
		}
		break;
	default:
		break;
	}
}

void FriendManager::AddRecentlyChat(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	if (_role->GetUID() == _target_id)
	{
		_role->NoticeNum(errornum::EN_CHAT_CAN_NOT_CHAT_WITH_YOURSELF);
		return;
	}

	if (this->IsBlack(_role->GetUID(), _target_id))
	{
		_role->NoticeNum(errornum::EN_SINGLE_CHAT_ROLE_TO_BLACK);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddRecentlyChatByHidden(_role, _target_id);
	}
	else
	{
		this->AddRecentlyChatByGameWorld(_role, _target_id);
	}
}

void FriendManager::AddRecentlyTeam(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	if (this->IsBlack(_role->GetUID(), _target_id))
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddRecentlyTeamByHidden(_role, _target_id);
	}
	else
	{
		this->AddRecentlyTeamByGameWorld(_role, _target_id);
	}
}

void FriendManager::AddFriendReq(Role * _role, int _target_id, bool _is_yi_jian)
{
	if (NULL == _role || 0 == _target_id)
		return;

	if (_role->GetUID() == _target_id)
	{
		_role->NoticeNum(errornum::EN_CAN_NOT_MAKE_FRIEND_WITH_SELF);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddFriendReqByHidden(_role, _target_id, _is_yi_jian);
	}
	else
	{
		this->AddFriendReqByGameWorld(_role, _target_id, _is_yi_jian);
	}
}

FriendManager::FriendManager::UserFrinedSet * FriendManager::GetUserFriendSetInfo(Role * _role)
{
	if (NULL == _role)
		return NULL;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role->GetUID());
	if (m_user_friend_info_map.end() != it)
	{
		return &it->second.user_friend_set;
	}

	return NULL;
}

void FriendManager::SystemMailToAllFriend(int _role_uid, MailSubject subject, MailContentTxt contenttxt)
{
	static UserID mail_user_list[MAX_FRIEND_NUM];
	int mail_user_num = 0;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_uid);
	if (m_user_friend_info_map.end() != it)
	{
		UserFrinedSetIt it_begin = it->second.user_friend_set.begin();
		UserFrinedSetIt it_end = it->second.user_friend_set.end();

		for (; it_begin != it_end; ++it_begin)
		{
			long long friend_for_index = *it_begin;
			int friend_uid = (friend_for_index << 32) >> 32;

			mail_user_list[mail_user_num++] = IntToUid(friend_uid);
		}

	}

	if (mail_user_num > 0)
	{
		MailRoute::Instance().MailToMultUser(mail_user_list, mail_user_num, subject, contenttxt);
	}
}

bool FriendManager::IsBlack(int _role_id, int _black_id)
{
	long long black_for_index = TranslateInt64(_role_id, _black_id);

	return  m_black_item_map.find(black_for_index) != m_black_item_map.end();
}

bool FriendManager::IsFriend(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);

	return  m_friend_item_map.find(friend_for_index) != m_friend_item_map.end();
}

void FriendManager::EditGroup(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id)
{
	if (NULL == _role || NULL == _role_id)
		return;

	if (_group_type < 0 || _group_type > MAX_FRIEND_GROUP_NUM)
		return;

	FriendCommonParam & friend_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	if (_group_type != 0)
	{
		int group_index = _group_type - 1;
		STRNCPY(friend_param.group_name[group_index], _group_name, sizeof(friend_param.group_name[group_index]));

		this->SendGroupInfo(_role);
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->EditGroupByHidden(_role, _group_type, _group_name, _count, _role_id);
	}
	else
	{
		this->EditGroupByGameWorld(_role, _group_type, _group_name, _count, _role_id);
	}
}

void FriendManager::RenameGroup(Role * _role, int _group_type, GameName _group_name)
{
	if (NULL == _role)
		return;

	if (_group_type < 0 || _group_type >= MAX_FRIEND_GROUP_NUM)
	{
		return;
	}

	FriendCommonParam & friend_param = _role->GetRoleModuleManager()->GetFriendCommonData();
	STRNCPY(friend_param.group_name[_group_type], _group_name, sizeof(friend_param.group_name[_group_type]));

	this->SendGroupInfo(_role);
}

void FriendManager::MoveGroup(Role * _role, int _group_type, int _count, int * _role_id)
{
	if (NULL == _role || NULL == _role_id)
		return;

	if (_group_type < 0 || _group_type >= MAX_FRIEND_GROUP_NUM)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->MoveGroupByHidden(_role, _group_type, _count, _role_id);
	}
	else
	{
		this->MoveGroupByGameWorld(_role, _group_type, _count, _role_id);
	}
}

void FriendManager::OnSyncFriendInfo(crossgameprotocal::SyncFriendInfo * _friend_info)
{
	if (NULL == _friend_info || 0 > _friend_info->count || MAX_ONCE_FRIENDS_COUNT < _friend_info->count)
	{
		return;
	}

	if (_friend_info->is_init)
	{
		// 初始化数据通过只可能是 原服 -> 隐藏服
		this->InitFriendInfoFromGameWorld(_friend_info);

		//验证用回执
		crossgameprotocal::CrossGameDataSyncCheckAck ack;
		ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND;
		InternalComm::Instance().SendToGameThroughCross(_friend_info->from_usid.plat_type, _friend_info->from_usid.server_id, (const char*)&ack, sizeof(ack));

	}
	else
	{
		// 更新数据只可能是隐藏服 -> 原服
		this->UpdataFriendInfoFromHidden(_friend_info);
	}
}

void FriendManager::OnSyncBlackInfo(crossgameprotocal::SyncBlackInfo * _black_info)
{
	if (NULL == _black_info || 0 > _black_info->count || MAX_ONCE_BLACK_COUNT < _black_info->count)
	{
		return;
	}

	if (_black_info->is_init)
	{
		// 初始化数据通过只可能是 原服 -> 隐藏服
		this->InitBlackInfoFromGameWorld(_black_info);


		//验证用回执
		crossgameprotocal::CrossGameDataSyncCheckAck ack;
		ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND;
		InternalComm::Instance().SendToGameThroughCross(_black_info->from_usid.plat_type, _black_info->from_usid.server_id, (const char*)&ack, sizeof(ack));

	}
	else
	{
		// 更新数据只可能是隐藏服 -> 原服
		this->UpdataBlackInfoFromHidden(_black_info);
	}
}

void FriendManager::OnSyncAddFriendOfflienEvent(crossgameprotocal::SyncAddFriendOfflineEvent * _event)
{
	if (NULL == _event)
		return;

	OfflineRegisterManager::Instance().AddFriendOfflineEvent(IntToUid(_event->role_id), _event->req);
}

void FriendManager::SendFriendListToRole(Role * _role)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendFriendListToRoleByHidden(_role);
	}
	else
	{
		this->SendFriendListToRoleByGameWorld(_role);
	}
}

void FriendManager::SendFriendListToRoleByGameWorld(Role * _role)
{
	// 获取好友列表
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	this->GetFriendInfoToHidden(_role, crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_FRIEND_LIST);
}

void FriendManager::OnAddFriendOfflineEventByLogin(Role * _role)
{
	if (NULL == _role)
		return;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role->GetUID());
	if (m_user_friend_info_map.end() != it)
	{
		EventHandler::Instance().OnAddFriend(_role, it->second.user_friend_set.size());
	}

	this->SyncInfoAddFriendOfflineEvent(_role->GetUID(), 0);
}

FriendManager::FriendManager() :m_data_state(FRIEND_DATA_STATUS_INVALID), m_friends_load_flag(false), m_blacks_load_flag(false),
m_friends_change_flag(false), m_blacks_change_flag(false)
{

}

FriendManager::~FriendManager()
{

}

void FriendManager::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SaveFriendParam();
	this->SaveBlackParam();
}

void FriendManager::SaveFriendParam()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!m_friends_change_flag)
	{
		return;
	}

	static FriendListParam param;
	param.count = 0;
	int count = 0;

	std::set < long long >::iterator it_dirty_begin = m_friend_dirty_mark.begin();
	std::set < long long >::iterator it_dirty_end = m_friend_dirty_mark.end();
	for (; it_dirty_begin != it_dirty_end && count < MAX_ONCE_FRIENDS_COUNT; ++it_dirty_begin)
	{
		long long  friend_for_index = *it_dirty_begin;
		FriendItemMapIt it_friend = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it_friend)
		{
			std::set <  long long  > ::iterator it_old = m_friend_old_state.find(friend_for_index);
			if (m_friend_old_state.end() == it_old)
			{
				// 既然之前不存在于数据库 ,那就直接丢弃这份数据好了 没必要去数据库里计算
				continue;
			}
			else
			{
				int role_id = friend_for_index >> 32;
				int friend_id = (friend_for_index << 32) >> 32;

				// 这个ID之前存在于数据里内 , 那么去数据库里删除一下
				param.friend_list[count].change_state = structcommon::CS_DELETE;
				param.friend_list[count].role_id = role_id;
				param.friend_list[count].friend_uid = friend_id;
			}
		}
		else
		{
			param.friend_list[count] = it_friend->second;

			std::set <  long long  > ::iterator it_old = m_friend_old_state.find(friend_for_index);
			if (m_friend_old_state.end() == it_old)
			{
				param.friend_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				param.friend_list[count].change_state = structcommon::CS_UPDATE;
			}
		}
		++count;
	}

	param.count = count;

	if (0 != param.count)
	{
		RMISaveFriendBackObjectImpl *backobj = new RMISaveFriendBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveFriendDataAsyn(param, backobj);
		if (ret)
		{
			this->ClearFriendDirtyMark();
		}
	}
}

void FriendManager::SaveBlackParam()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!m_blacks_change_flag)
	{
		return;
	}

	static BlacklistsParam param;
	param.count = 0;
	int count = 0;

	std::set < long long >::iterator it_dirty_begin = m_black_dirty_mark.begin();
	std::set < long long >::iterator it_dirty_end = m_black_dirty_mark.end();
	for (; it_dirty_begin != it_dirty_end && count < MAX_ONCE_BLACK_COUNT; ++it_dirty_begin)
	{
		long long  black_for_index = *it_dirty_begin;
		BlackItemMapIt it_black = m_black_item_map.find(black_for_index);
		if (m_black_item_map.end() == it_black)
		{
			std::set <  long long  > ::iterator it_old = m_black_old_state.find(black_for_index);
			if (m_black_old_state.end() == it_old)
			{
				// 既然之前不存在于数据库 ,那就直接丢弃这份数据好了 没必要去数据库里计算
				continue;
			}
			else
			{
				int role_id = black_for_index >> 32;
				int black_id = (black_for_index << 32) >> 32;

				// 这个ID之前存在于数据里内 , 那么去数据库里删除一下
				param.black_list[count].change_state = structcommon::CS_DELETE;
				param.black_list[count].role_id = role_id;
				param.black_list[count].uid = black_id;
			}
		}
		else
		{
			param.black_list[count] = it_black->second;

			std::set <  long long  > ::iterator it_old = m_black_old_state.find(black_for_index);
			if (m_black_old_state.end() == it_old)
			{
				param.black_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				param.black_list[count].change_state = structcommon::CS_UPDATE;
			}
		}
		++count;
	}

	param.count = count;

	if (0 != param.count)
	{
		RMISaveBlackBackObjectImpl *backobj = new RMISaveBlackBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveBlackListDataAsyn(param, backobj);
		if (ret)
		{
			this->ClearBlackDirtyMark();
		}
	}
}

void FriendManager::ClearFriendDirtyMark()
{
	if (!m_friends_change_flag)
		return;

	m_friend_dirty_mark.clear();
	m_friend_old_state.clear();

	FriendItemMapIt it_begin = m_friend_item_map.begin();
	FriendItemMapIt it_end = m_friend_item_map.end();
	for (; it_begin != it_end; ++it_begin)
	{
		m_friend_old_state.insert(it_begin->first);
	}

	m_friends_change_flag = false;
}

void FriendManager::ClearBlackDirtyMark()
{
	if (!m_blacks_change_flag)
		return;

	m_black_dirty_mark.clear();
	m_black_old_state.clear();

	BlackItemMapIt it_begin = m_black_item_map.begin();
	BlackItemMapIt it_end = m_black_item_map.end();
	for (; it_begin != it_end; ++it_begin)
	{
		m_black_old_state.insert(it_begin->first);
	}

	m_blacks_change_flag = false;
}

void FriendManager::SetFriendDirtyMark(long long _friend_for_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if ((int)m_friend_dirty_mark.size() >= MAX_ONCE_FRIENDS_COUNT)
	{
		this->SaveFriendParam();
	}

	m_friends_change_flag = true;
	m_friend_dirty_mark.insert(_friend_for_index);
}

void FriendManager::SetBlackDirtyMark(long long _black_for_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if ((int)m_black_dirty_mark.size() >= MAX_ONCE_BLACK_COUNT)
	{
		this->SaveBlackParam();
	}

	m_blacks_change_flag = true;
	m_black_dirty_mark.insert(_black_for_index);
}

void FriendManager::CheckLoadFinish()
{
	if (m_friends_load_flag && m_blacks_load_flag)
		m_data_state = FRIEND_DATA_STATUS_LOAD_FINISH;

	if (FRIEND_DATA_STATUS_LOAD_FINISH == m_data_state)
	{
		// 如果有需要加载完成时进行处理的在这里处理
	}
}

void FriendManager::SyncAllFriendDataToHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::SyncFriendInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = true;
	sync_info.is_init_add = 0;

	FriendItemMapIt it_begin = m_friend_item_map.begin();
	FriendItemMapIt it_end = m_friend_item_map.end();

	int count = 0;
	for (; it_begin != it_end && count < MAX_ONCE_FRIENDS_COUNT; ++it_begin)
	{
		sync_info.item_list[count++].friend_item = it_begin->second;
		if (count >= MAX_ONCE_FRIENDS_COUNT)
		{
			sync_info.count = count;
			int send_length = sizeof(crossgameprotocal::SyncFriendInfo) -
				((MAX_ONCE_FRIENDS_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
			DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND);

			count = 0;
			sync_info.count = 0;
			sync_info.is_init_add = 1;
		}
	}

	if (count > 0)
	{
		sync_info.count = count;
	}

	int send_length = sizeof(crossgameprotocal::SyncFriendInfo) -
		((MAX_ONCE_FRIENDS_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND);
}

void FriendManager::SyncAllBlackDataToHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::SyncBlackInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = true;
	sync_info.is_init_add = 0;

	BlackItemMapIt it_begin = m_black_item_map.begin();
	BlackItemMapIt it_end = m_black_item_map.end();

	int count = 0;
	for (; it_begin != it_end && count < MAX_ONCE_BLACK_COUNT; ++it_begin)
	{
		sync_info.item_list[count++].black_item = it_begin->second;

		if (count >= MAX_ONCE_BLACK_COUNT)
		{
			sync_info.count = count;
			int send_length = sizeof(crossgameprotocal::SyncBlackInfo) -
				((MAX_ONCE_BLACK_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
			DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND);
			count = 0;
			sync_info.count = 0;
			sync_info.is_init_add = 1;
		}
	}

	if (count > 0)
	{
		sync_info.count = count;
	}

	int send_length = sizeof(crossgameprotocal::SyncBlackInfo) -
		((MAX_ONCE_FRIENDS_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND);
}

void FriendManager::SyncFriendDataToGameWorld(long long friend_for_index, int _sync_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	// 同步好友数据 隐藏服->原服 
	static crossgameprotocal::SyncFriendInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = false;
	sync_info.is_init_add = 0;

	int role_id = (int)(friend_for_index >> 32);
	int friend_id = (int)((friend_for_index << 32) >> 32);
	sync_info.item_list[0].friend_item.role_id = role_id;
	sync_info.item_list[0].friend_item.friend_uid = friend_id;
	sync_info.item_list[0].sync_type = _sync_status;

	if (crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA == _sync_status ||
		crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_INSERT == _sync_status)
	{
		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it)
			return;

		sync_info.item_list[0].friend_item = it->second;
	}

	int send_length = sizeof(crossgameprotocal::SyncFriendInfo) -
		((MAX_ONCE_FRIENDS_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&sync_info, send_length);
}

void FriendManager::SyncBlackDataToGameWorld(long long black_for_index, int _sync_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	// 同步好友数据 隐藏服->原服
	static crossgameprotocal::SyncBlackInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = false;
	sync_info.is_init_add = 0;

	int role_id = (int)(black_for_index >> 32);
	int black_id = (int)((black_for_index << 32) >> 32);
	sync_info.item_list[0].black_item.role_id = role_id;
	sync_info.item_list[0].black_item.uid = black_id;
	sync_info.item_list[0].sync_type = _sync_status;

	if (crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_UPDATA == _sync_status)
	{
		BlackItemMapIt it = m_black_item_map.find(black_for_index);
		if (m_black_item_map.end() == it)
			return;

		sync_info.item_list[0].black_item = it->second;
	}

	int send_length = sizeof(crossgameprotocal::SyncBlackInfo) -
		((MAX_ONCE_FRIENDS_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&sync_info, send_length);
}

void FriendManager::InitFriendInfoFromGameWorld(crossgameprotocal::SyncFriendInfo * _friend_info)
{
	if (NULL == _friend_info || !CrossConfig::Instance().IsHiddenServer())
		return;

	// 初始化就无脑添加就是了
	for (int i = 0; i < _friend_info->count && i < MAX_ONCE_FRIENDS_COUNT; ++i)
	{
		int role_id = _friend_info->item_list[i].friend_item.role_id;
		int friend_id = _friend_info->item_list[i].friend_item.friend_uid;

		long long friend_for_index = this->AddFriendInfo(role_id, friend_id, _friend_info->item_list[i].friend_item);
		this->AddUserFriendRelation(role_id, friend_for_index);
	}
}

void FriendManager::UpdataFriendInfoFromHidden(crossgameprotocal::SyncFriendInfo * _friend_info)
{
	if (NULL == _friend_info || CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _friend_info->count; ++i)
	{
		int role_id = _friend_info->item_list[i].friend_item.role_id;
		int friend_id = _friend_info->item_list[i].friend_item.friend_uid;

		switch (_friend_info->item_list[i].sync_type)
		{
		case crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_INSERT:
			{
				long long friend_for_index = this->AddFriendInfo(role_id, friend_id, _friend_info->item_list[i].friend_item);
				this->AddUserFriendRelation(role_id, friend_for_index);
				this->SetFriendDirtyMark(friend_for_index);
			}
			break;
		case crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA:
			{
				long long friend_for_index = TranslateInt64(role_id, friend_id);
				m_friend_item_map[friend_for_index] = _friend_info->item_list[i].friend_item;
				this->SetFriendDirtyMark(friend_for_index);
			}
			break;
		case crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_DELETE:
			{
				long long friend_for_index = this->RemoveFriendInfo(role_id, friend_id);
				this->RemoveUserFriendRelation(role_id, friend_id);
				this->SetFriendDirtyMark(friend_for_index);
			}
			break;
		default:
			break;
		}

		// 记录一下变更 , 方便查问题
		gamelog::g_log_social.printf(LL_INFO, "FriendManager::UpdataFriendInfoFromHidden  role_id:%d  friend_id:%d  sync_type:%d ",
			role_id, friend_id, _friend_info->item_list[i].sync_type);
	}
}

void FriendManager::InitBlackInfoFromGameWorld(crossgameprotocal::SyncBlackInfo * _black_info)
{
	if (NULL == _black_info || !CrossConfig::Instance().IsHiddenServer())
		return;

	// 初始化就无脑添加就是了
	for (int i = 0; i < _black_info->count && i < MAX_ONCE_BLACK_COUNT; ++i)
	{
		int role_id = _black_info->item_list[i].black_item.role_id;
		int black_id = _black_info->item_list[i].black_item.uid;

		long long black_for_index = this->AddBlackInfo(role_id, black_id, _black_info->item_list[i].black_item);
		this->AddUserBlackRelation(role_id, black_for_index);
	}
}

void FriendManager::UpdataBlackInfoFromHidden(crossgameprotocal::SyncBlackInfo * _black_info)
{
	if (NULL == _black_info || CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _black_info->count; ++i)
	{
		int role_id = _black_info->item_list[i].black_item.role_id;
		int black_id = _black_info->item_list[i].black_item.uid;

		// 数据同步可能是 原服 ->隐藏服  也可能是隐藏服->原服

		switch (_black_info->item_list[i].sync_type)
		{
		case crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_INSERT:
			{
				long long black_for_index = this->AddBlackInfo(role_id, black_id, _black_info->item_list[i].black_item);
				this->AddUserBlackRelation(role_id, black_for_index);
				this->SetBlackDirtyMark(black_for_index);
			}
			break;
		case crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_UPDATA:
			{
				long long black_for_index = TranslateInt64(role_id, black_id);
				m_black_item_map[black_for_index] = _black_info->item_list[i].black_item;
				this->SetBlackDirtyMark(black_for_index);
			}
			break;
		case crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_DELETE:
			{
				long long black_for_index = this->RemoveBlackInfo(role_id, black_id);
				this->RemoveUserBlackRelation(role_id, black_id);
				this->SetBlackDirtyMark(black_for_index);
			}
			break;
		default:
			break;
		}

		// 记录一下变更 , 方便查问题
		gamelog::g_log_social.printf(LL_INFO, "FriendManager::UpdataBlackInfoFromSyncData  role_id:%d  black_id:%d  sync_type:%d ",
			role_id, black_id, _black_info->item_list[i].sync_type);
	}
}

long long FriendManager::AddFriendInfo(int _role_id, int _friend_id, const FriendItem & _friend_item)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	m_friend_item_map[friend_for_index] = _friend_item;

	return friend_for_index;
}

long long FriendManager::RemoveFriendInfo(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);

	m_friend_item_map.erase(friend_for_index);

	return friend_for_index;
}

int FriendManager::GetFriendNumber(int _role_id)
{
	int ret_num = 0;
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		ret_num = (int)it->second.user_friend_set.size();
	}

	return ret_num;
}

int FriendManager::GetFriendNumber(int _role_id, int intimacy_num)
{
	int friend_num = 0;
	UserFrinedInfoMapIt find_it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != find_it)
	{
		friend_num = (int)find_it->second.user_friend_set.size();
	}

	if (intimacy_num <= 0)
		return friend_num;

	int meet_count = 0;
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		UserFrinedSetIt iter_begin = it->second.user_friend_set.begin();
		UserFrinedSetIt iter_end = it->second.user_friend_set.begin();

		for (; iter_begin != iter_end; ++iter_begin)
		{
			long long friend_for_index = *iter_begin;
			int friend_id = (friend_for_index << 32) >> 32;

			if (FriendIntimacyManager::Instance().GetFriendIntimacy(_role_id, friend_id) >= intimacy_num)
			{
				meet_count++;
			}
		}
	}

	return meet_count;
}

void FriendManager::AddUserFriendRelation(int _role_id, long long _friend_for_index)
{
	UserFriendInfo & friend_info = m_user_friend_info_map[_role_id];
	friend_info.user_friend_set.insert(_friend_for_index);
}

void FriendManager::AddUserFriendRelation(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	UserFriendInfo & friend_info = m_user_friend_info_map[_role_id];
	friend_info.user_friend_set.insert(friend_for_index);
}


void FriendManager::RemoveUserFriendRelation(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	UserFriendInfo & friend_info = m_user_friend_info_map[_role_id];
	friend_info.user_friend_set.erase(friend_for_index);
}

void FriendManager::RemoveUserFriendRelation(int _role_id, long long _friend_for_index)
{
	UserFriendInfo & friend_info = m_user_friend_info_map[_role_id];
	friend_info.user_friend_set.erase(_friend_for_index);
}

bool FriendManager::IsFriendFull(int _role_id)
{
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		if ((int)it->second.user_friend_set.size() >= MAX_FRIEND_NUM)
		{
			return true;
		}
	}

	return false;
}

long long FriendManager::AddBlackInfo(int _role_id, int _black_id, const BlackItem & _black_item)
{
	long long black_for_index = TranslateInt64(_role_id, _black_id);
	m_black_item_map[black_for_index] = _black_item;

	return black_for_index;
}

long long FriendManager::RemoveBlackInfo(int _role_id, int _black_id)
{
	long long black_for_index = TranslateInt64(_role_id, _black_id);

	m_black_item_map.erase(black_for_index);

	return black_for_index;
}

void FriendManager::AddUserBlackRelation(int _role_id, long long _black_for_index)
{
	int black_uid = (_black_for_index << 32) >> 32;

	// A将B添加到A的黑名单列表
	UserBlackInfo & black_info = m_user_black_info_map[_role_id];
	black_info.user_black_set.insert(_black_for_index);

	// B将A添加到B的被添加黑名单列表
	UserBlackInfo & be_black_info = m_user_black_info_map[black_uid];
	be_black_info.user_be_black_set.insert(_role_id);
}

void FriendManager::AddUserBlackRelation(int _role_id, int _black_id)
{
	long long black_for_index = TranslateInt64(_role_id, _black_id);

	// A将B添加到A的黑名单列表
	UserBlackInfo & black_info = m_user_black_info_map[_role_id];
	black_info.user_black_set.insert(black_for_index);

	// B将A添加到B的被添加黑名单列表
	UserBlackInfo & be_black_info = m_user_black_info_map[_black_id];
	be_black_info.user_be_black_set.insert(_role_id);
}

void FriendManager::RemoveUserBlackdRelation(int _role_id, int _black_id)
{
	long long black_for_index = TranslateInt64(_role_id, _black_id);

	// A将B 移出A的黑名单列表
	UserBlackInfo & black_info = m_user_black_info_map[_role_id];
	black_info.user_black_set.erase(black_for_index);

	// B将A 移出B的被添加黑名单列表
	UserBlackInfo & be_black_info = m_user_black_info_map[_black_id];
	be_black_info.user_be_black_set.erase(_role_id);
}

void FriendManager::RemoveUserBlackRelation(int _role_id, long long _black_for_index)
{
	int black_uid = (_black_for_index << 32) >> 32;
	// A将B 移出A的黑名单列表
	UserBlackInfo & black_info = m_user_black_info_map[_role_id];
	black_info.user_black_set.erase(_black_for_index);

	// B将A 移出B的被添加黑名单列表
	UserBlackInfo & be_black_info = m_user_black_info_map[black_uid];
	be_black_info.user_be_black_set.erase(_role_id);
}

void FriendManager::OnLoginNotify( int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnLoginNotifyFriendByHidden( _role_id , UserStatusNode::STATUS_TYPE_CROSS);
		this->OnLoginNotifyBlackByHidden(_role_id, UserStatusNode::STATUS_TYPE_CROSS);
	}
	else
	{
		this->SendRoleLoginLogoutToHidden( _role_id, UserStatusNode::STATUS_TYPE_ONLINE);
	}
}

void FriendManager::OnLoginNotifyFriendByHidden(int _role_id, char _online_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	UserFriendInfo & user_info = m_user_friend_info_map[_role_id];
	if (0 == user_info.user_friend_set.size())
	{
		// 没有好友就不需要通知了
		return;
	}

	UserFrinedSetIt it_begin = user_info.user_friend_set.begin();
	UserFrinedSetIt it_end = user_info.user_friend_set.end();

	for (; it_begin != it_end; ++it_begin)
	{
		this->OnFriendLoginOrLogoutByHidden(_role_id, *it_begin, _online_status);
	}
}

void FriendManager::OnLogoutNotify(int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnLogoutNotifyFriendByHidden(_role_id);
		this->OnLogoutNotifyBlackByHidden(_role_id);
	}
	else
	{
		this->SendRoleLoginLogoutToHidden( _role_id, UserStatusNode::STATUS_TYPE_OFFLINE);
	}
}

void FriendManager::OnLogoutNotifyFriendByHidden(int _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		UserFrinedSetIt it_begin = it->second.user_friend_set.begin();
		UserFrinedSetIt it_end = it->second.user_friend_set.end();

		for (; it_begin != it_end; ++it_begin)
		{
			long long friend_for_index = *it_begin;

			this->OnFriendLoginOrLogoutByHidden(_role_id, friend_for_index, UserStatusNode::STATUS_TYPE_OFFLINE);
		}
	}
}

void FriendManager::OnLoginNotifyBlackByHidden(int _role_id, char _online_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	UserBlackInfo & black_info = m_user_black_info_map[_role_id];
	if (0 == black_info.user_be_black_set.size())
	{
		// 没有被人添加过黑名单 就不需要通知了
		return;
	}

	std::set<int >::iterator it_begin = black_info.user_be_black_set.begin();
	std::set<int >::iterator it_end = black_info.user_be_black_set.end();

	// 通知玩家的状态到添加玩家黑名单的人
	for (; it_begin != it_end; ++it_begin)
	{
		int black_uid = *it_begin ;

		Role * black_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(black_uid));
		if (NULL != black_role)
		{
			this->SendBlackChangeToRoleByHidden(black_role, _role_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE, _online_status);
		}
		else
		{
			// 如果玩家不在线就发送到其对应的主服去通知一下
			this->SendBlackChangeToRoleToGameWorld(black_uid, _role_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE, _online_status);
		}
	}
}

void FriendManager::OnLogoutNotifyBlackByHidden(int _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	UserBlackInfoMapIt it = m_user_black_info_map.find(_role_id);
	if (m_user_black_info_map.end() != it)
	{
		std::set<int>::iterator  it_begin = it->second.user_be_black_set.begin();
		std::set<int>::iterator  it_end = it->second.user_be_black_set.end();

		for (; it_begin != it_end; ++it_begin)
		{
			int black_uid = *it_begin ;
			Role * black_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(black_uid));
			if (NULL != black_role)
			{
				this->SendBlackChangeToRoleByHidden(black_role, _role_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE , UserStatusNode::STATUS_TYPE_OFFLINE);
			}
			else
			{
				// 如果玩家不在线就发送到其对应的主服去通知一下
				this->SendBlackChangeToRoleToGameWorld(black_uid, _role_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE, UserStatusNode::STATUS_TYPE_OFFLINE);
			}
		}
	}
}

void FriendManager::OnFriendLoginOrLogoutByHidden(int _role_id, long long _friend_for_index, int _online_status)
{
	int friend_id = (_friend_for_index << 32) >> 32;

	// 1. 先更新 玩家在好友的信息记录里的最后上线时间
	this->UpdateFriendLastOnlineTimestampByHidden(_role_id, friend_id);

	Role * friend_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(friend_id));
	if (NULL != friend_role)
	{
		this->SendFriendChangeToRoleByHidden(friend_role, _role_id, Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, 0, _online_status);
	}
	else
	{
		// 如果玩家不在线就发送到其对应的主服去通知一下
		this->SendFriendChangeToRoleToGameWorld(friend_id, _role_id , Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, 0, _online_status);
	}
}


void FriendManager::UpdateFriendLastOnlineTimestampByHidden(int _role_id, int _friend_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	// 注意!!!
	// 这里是需要查好友对玩家的 不是玩家对好友的
	long long friend_for_index = TranslateInt64(_friend_id, _role_id);

	FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
	if (m_friend_item_map.end() != it)
	{
		it->second.last_online_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA);
	}
}

void FriendManager::SendFriendChangeToRoleByHidden(Role * _role, int _change_role_id, int _change_type, int _group_type, int _online_status)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCChangeFriend cf;
	cf.change_type = _change_type;
	cf.info.user_id = _change_role_id;
	cf.info.group_type = _group_type;

	if (_change_type == Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE)
	{
		long long friend_for_index = TranslateInt64(_role->GetUID(), _change_role_id);

		FriendItemMapIt iter = m_friend_item_map.find(friend_for_index);
		if (iter == m_friend_item_map.end())
		{
			return;
		}

		char is_online = this->GetOnlineStatus(_role->GetUID(), _change_role_id, true , _online_status);

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(_change_role_id);
		if (NULL != user_cache_node)
		{
			F_STRNCPY(cf.info.role_name, user_cache_node->role_name, sizeof(cf.info.role_name));
			memset(cf.info.family_name, 0, sizeof(cf.info.family_name));
			cf.info.intimacy = iter->second.intimacy;
			cf.info.sex = user_cache_node->GetSex();
			cf.info.prof = user_cache_node->profession;
			cf.info.is_online = is_online;
			cf.info.level = user_cache_node->level;
			cf.info.vip_level = 0;
			cf.info.group_type = iter->second.group_type;
			cf.info.avatar_timestamp = user_cache_node->avatar_timestamp;
			cf.info.avatar_type = user_cache_node->avatar_type;
			cf.info.headshot_id = user_cache_node->appearance.headshot_id;
			cf.info.top_level = user_cache_node->top_level;
			cf.info.last_online_timestamp = user_cache_node->last_online_timestamp;
		}
		else
		{
			iter->second.GetData(cf.info);
			cf.info.is_online = is_online;
			cf.info.vip_level = 0;
			cf.info.top_level = 0;
		}
	}

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&cf, sizeof(cf));
}

void FriendManager::SendFriendChangeToRoleToGameWorld(int _role_id, int  _change_role_id, int _change_type, int _group_type, int _online_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;


	// change.role_id : 为接受这条信息的玩家
	// change.friend_info.user_id : 为需要发送的变更信息的玩家

	crossgameprotocal::HiddenGameFriendChangeToRole change;
	change.role_id = _role_id;
	change.change_type = _change_type;
	change.friend_info.user_id = _change_role_id;
	change.friend_info.group_type = _group_type;


	if (_change_type == Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE)
	{
		long long friend_for_index = TranslateInt64(_role_id, _change_role_id);

		FriendItemMapIt iter = m_friend_item_map.find(friend_for_index);
		if (iter == m_friend_item_map.end())
		{
			return;
		}

		char is_online = this->GetOnlineStatus(_role_id, _change_role_id, false, _online_status);
		
		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(_change_role_id);
		if (NULL != user_cache_node)
		{
			F_STRNCPY(change.friend_info.role_name, user_cache_node->role_name, sizeof(change.friend_info.role_name));
			memset(change.friend_info.family_name, 0, sizeof(change.friend_info.family_name));
			change.friend_info.intimacy = iter->second.intimacy;
			change.friend_info.sex = user_cache_node->GetSex();
			change.friend_info.prof = user_cache_node->profession;
			change.friend_info.is_online = is_online;
			change.friend_info.level = user_cache_node->level;
			change.friend_info.vip_level = 0;
			change.friend_info.group_type = iter->second.group_type;
			change.friend_info.avatar_timestamp = user_cache_node->avatar_timestamp;
			change.friend_info.avatar_type = user_cache_node->avatar_type;
			change.friend_info.headshot_id = user_cache_node->appearance.headshot_id;
			change.friend_info.top_level = user_cache_node->top_level;
			change.friend_info.last_online_timestamp = user_cache_node->last_online_timestamp;
		}
		else
		{
			iter->second.GetData(change.friend_info);
			change.friend_info.is_online = is_online;
			change.friend_info.vip_level = 0;
			change.friend_info.top_level = 0;
		}

		// 发到主服的需要处理名字尾缀
		this->CheckOnlienStatusAndStrCut(is_online, change.friend_info.role_name, _role_id , _change_role_id);
	}

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&change, sizeof(change));
}

void FriendManager::SendFriendChangeToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameFriendChangeToRole * _msg)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCChangeFriend cf;
	cf.change_type = _msg->change_type;
	cf.info.user_id = _msg->friend_info.user_id;
	cf.info.group_type = _msg->friend_info.group_type;

	F_STRNCPY(cf.info.role_name, _msg->friend_info.role_name, sizeof(cf.info.role_name));
	memset(cf.info.family_name, 0, sizeof(cf.info.family_name));
	cf.info.intimacy = _msg->friend_info.intimacy;
	cf.info.sex = _msg->friend_info.sex;
	cf.info.prof = _msg->friend_info.prof;
	cf.info.is_online = _msg->friend_info.is_online;
	cf.info.level = _msg->friend_info.level;
	cf.info.vip_level = _msg->friend_info.vip_level;
	cf.info.group_type = _msg->friend_info.group_type;
	cf.info.avatar_timestamp = _msg->friend_info.avatar_timestamp;
	cf.info.avatar_type = _msg->friend_info.avatar_type;
	cf.info.headshot_id = _msg->friend_info.headshot_id;
	cf.info.top_level = _msg->friend_info.top_level;
	cf.info.last_online_timestamp = _msg->friend_info.last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&cf, sizeof(cf));
}

void FriendManager::SendRoleLoginLogoutToHidden(int _role_id, char _online_status)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenRoleLoginLogoutFriendNotify notify;
	notify.role_id = _role_id;
	notify.online_status = _online_status;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&notify, sizeof(notify));
}

void FriendManager::CheckOnlienStatusAndStrCut(char _online_status, GameName & _str, int _role_id, int _target_id)
{
	if (UserStatusNode::STATUS_TYPE_ONLINE == _online_status )
	{
		// 需要移除从跨服传来的名字尾缀
		this->StrCut(_str);
	}
	else if (UserStatusNode::STATUS_TYPE_OFFLINE == _online_status)
	{
		//看看是否不是同个母服
		const UniqueServerID* role_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index));
		const UniqueServerID* target_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_target_id).db_index));
		if (NULL != role_mother_server && NULL != target_mother_server)
		{
			if (role_mother_server->server_id == target_mother_server->server_id ) 
			{
				// 母服相同的离线需要去掉尾缀, 非相同则不需要处理
				this->StrCut(_str);
			}
		}
	}
}

void FriendManager::StrCut(GameName & _str)
{
	char * ptr = std::strrchr(_str, '_');
	if (NULL != ptr)
	{
		int index = ptr - &_str[0];
		if (index >= 0 && index < sizeof(GameName))
			_str[index] = '\0';
	} 
}

char FriendManager::GetOnlineStatus(int _role_id, int _target_uid, bool is_to_cross_target , char _online_status /*= -1*/)
{
	if ( !CrossConfig::Instance().IsHiddenServer())
		return _online_status;

	char temp_online_status = 0;
	if (-1 != _online_status)
	{
		temp_online_status = _online_status;
	}
	else
	{
		temp_online_status = (char)UserCacheManager::Instance().GetUserOnlineStatus(_target_uid);
	}

	const UniqueServerID* role_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index));
	const UniqueServerID* target_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_target_uid).db_index));

	if (temp_online_status == UserStatusNode::STATUS_TYPE_ONLINE)
	{
		//看看是否不是同个母服
		if (NULL != role_mother_server && NULL != target_mother_server)
		{
			if (role_mother_server->server_id != target_mother_server->server_id ||
				is_to_cross_target) // is_to_cross_target : 指在跨服发送给跨服目标 ,
			{
				// 母服不同 , 或是发送给跨服的玩家 , 状态设置为跨服
				// 因为所有玩家对跨服玩家来说 状态都是跨服
				temp_online_status = UserStatusNode::STATUS_TYPE_CROSS;
			}
		}
	}
	else if (temp_online_status == UserStatusNode::STATUS_TYPE_CROSS)
	{
		//看看是否不是同个母服
		if (NULL != role_mother_server && NULL != target_mother_server)
		{
			if (role_mother_server->server_id == target_mother_server->server_id &&
				!is_to_cross_target) // is_to_cross_target : 指在跨服发送给跨服目标 ,
			{
				// 因为UserCacheManager::Instance().GetUserOnlineStatus接口在跨服获取时一定状态一定是跨服或者离线
				// 所以当母服相同时 , 且是发往主服的 , 说明请求人是在主服 , 这里就需要判断一下目标是在跨服还在主服
				// 如果目标当前不在跨服, 且状态为跨服 , 那就一定在主服了

				Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(_target_uid);
				if(NULL == target_role)
					temp_online_status = UserStatusNode::STATUS_TYPE_ONLINE;
			}
		}
	}

	return temp_online_status;
}

void FriendManager::SendFriendListToRoleByHidden(Role * _role)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCFriendInfo protocol;
	const FriendCommonParam & common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	for (int i = 0; i < MAX_FRIEND_GROUP_NUM; ++i)
	{
		memcpy(protocol.group_name[i], common_param.group_name[i], sizeof(protocol.group_name[i]));
	}

	int count = 0;
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role->GetUID());
	if (m_user_friend_info_map.end() != it)
	{
		for (UserFrinedSetIt iter = it->second.user_friend_set.begin(); iter != it->second.user_friend_set.end(); ++iter)
		{
			long long friend_for_index = *iter;

			FriendItemMapIt it_friend = m_friend_item_map.find(friend_for_index);
			if (m_friend_item_map.end() == it_friend)
			{
				continue;
			}

			char online_status = this->GetOnlineStatus(_role->GetUID() , it_friend->second.friend_uid, true);

			UserCacheNode * friend_cache_node = UserCacheManager::Instance().GetUserNode(it_friend->second.friend_uid);
			if (NULL != friend_cache_node)
			{
				protocol.friend_list[count].sex = friend_cache_node->GetSex();
				protocol.friend_list[count].prof = friend_cache_node->profession;
				protocol.friend_list[count].is_online = online_status;
				F_STRNCPY(protocol.friend_list[count].role_name, friend_cache_node->role_name, sizeof(protocol.friend_list[count].role_name));
				memset(protocol.friend_list[count].family_name, 0, sizeof(protocol.friend_list[count].family_name));
				protocol.friend_list[count].user_id = it_friend->second.friend_uid;
				protocol.friend_list[count].intimacy = it_friend->second.intimacy;
				protocol.friend_list[count].level = friend_cache_node->level;
				protocol.friend_list[count].vip_level = 0;
				protocol.friend_list[count].group_type = it_friend->second.group_type;
				protocol.friend_list[count].avatar_timestamp = friend_cache_node->avatar_timestamp;
				protocol.friend_list[count].avatar_type = friend_cache_node->avatar_type;
				protocol.friend_list[count].headshot_id = friend_cache_node->appearance.headshot_id;
				protocol.friend_list[count].top_level = friend_cache_node->top_level;
				protocol.friend_list[count].last_online_timestamp = friend_cache_node->last_online_timestamp;
			}
			else
			{
				it_friend->second.GetData(protocol.friend_list[count]);
				protocol.friend_list[count].is_online = online_status;
				protocol.friend_list[count].vip_level = 0;
				protocol.friend_list[count].top_level = 0;
			}

			++count;
			if (count >= MAX_FRIEND_NUM)
			{
				break;
			}
		}
	}

	protocol.count = count;

	size_t length = sizeof(protocol) - ((MAX_FRIEND_NUM - protocol.count) * sizeof(protocol.friend_list[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::GetFriendInfoToHidden(Role * _role, int _get_info_type)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer() ||
		_get_info_type <= crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_INVALID ||
		_get_info_type >= crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_MAX)
		return;

	static crossgameprotocal::GameHiddenGetFriendInfo info;

	info.usid = _role->GetUniqueServerID();
	info.role_id = _role->GetUID();
	info.info_type = _get_info_type;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
}

void FriendManager::SendFriendListToRoleToGameWorld(const UniqueServerID _usid, int _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	static crossgameprotocal::HiddenGameFriendListToRole frined_list;
	frined_list.role_id = _role_id;
	frined_list.usid = _usid;

	int count = 0;
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		for (UserFrinedSetIt iter = it->second.user_friend_set.begin(); iter != it->second.user_friend_set.end(); ++iter)
		{
			long long friend_for_index = *iter;

			FriendItemMapIt  it_friend = m_friend_item_map.find(friend_for_index);
			if (m_friend_item_map.end() == it_friend)
			{
				continue;
			}

			char online_status = this->GetOnlineStatus(_role_id , it_friend->second.friend_uid, false);

			UserCacheNode * friend_cache_node = UserCacheManager::Instance().GetUserNode(it_friend->second.friend_uid);
			if (NULL != friend_cache_node)
			{
				frined_list.friend_list[count].sex = friend_cache_node->GetSex();
				frined_list.friend_list[count].prof = friend_cache_node->profession;
				frined_list.friend_list[count].is_online = online_status;
				F_STRNCPY(frined_list.friend_list[count].role_name, friend_cache_node->role_name, sizeof(frined_list.friend_list[count].role_name));
				memset(frined_list.friend_list[count].family_name, 0, sizeof(frined_list.friend_list[count].family_name));
				frined_list.friend_list[count].user_id = it_friend->second.friend_uid;
				frined_list.friend_list[count].intimacy = it_friend->second.intimacy;
				frined_list.friend_list[count].level = friend_cache_node->level;
				frined_list.friend_list[count].vip_level = 0;
				frined_list.friend_list[count].group_type = it_friend->second.group_type;
				frined_list.friend_list[count].avatar_timestamp = friend_cache_node->avatar_timestamp;
				frined_list.friend_list[count].avatar_type = friend_cache_node->avatar_type;
				frined_list.friend_list[count].headshot_id = friend_cache_node->appearance.headshot_id;
				frined_list.friend_list[count].top_level = friend_cache_node->top_level;
				frined_list.friend_list[count].last_online_timestamp = friend_cache_node->last_online_timestamp;
			}
			else
			{
				it_friend->second.GetData(frined_list.friend_list[count]);
				frined_list.friend_list[count].is_online = online_status;
				frined_list.friend_list[count].vip_level = 0;
				frined_list.friend_list[count].top_level = 0;
			}

			// 发到主服的需要处理名字尾缀
			this->CheckOnlienStatusAndStrCut(online_status, frined_list.friend_list[count].role_name, _role_id, it_friend->second.friend_uid);

			++count;
			if (count >= MAX_FRIEND_NUM)
			{
				break;
			}
		}
	}

	frined_list.count = count;

	size_t length = sizeof(frined_list) - ((MAX_FRIEND_NUM - frined_list.count) * sizeof(frined_list.friend_list[0]));
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&frined_list, length);

}

void FriendManager::SendBlackListToRole(Role * _role)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendBlackListToRoleByHidden(_role);
	}
	else
	{
		this->SendBlackListToRoleByGameWorld(_role);
	}
}

void FriendManager::SendBlackListToRoleByGameWorld(Role * _role)
{
	// 获取好友列表
	if (NULL == _role)
		return;

	this->GetFriendInfoToHidden(_role, crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_BLACK_LIST);
}

void FriendManager::SendBlackListToRoleByHidden(Role * _role)
{
	Protocol::SCBlackInfo protocol;

	int count = 0;
	UserBlackInfoMapIt it = m_user_black_info_map.find(_role->GetUID());
	if (m_user_black_info_map.end() != it)
	{
		UserBlackSetIt it_begin = it->second.user_black_set.begin();
		UserBlackSetIt it_end = it->second.user_black_set.end();

		for (; it_begin != it_end; ++it_begin)
		{
			long long black_for_index = *it_begin;

			BlackItemMapIt it_black = m_black_item_map.find(black_for_index);
			if (m_black_item_map.end() == it_black)
			{
				continue;
			}

			UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(it_black->second.uid);
			if (NULL == user_cache_node)
			{
				continue;
			}

			char is_online = this->GetOnlineStatus(_role->GetUID(), it_black->second.uid, true);

			protocol.black_list[count].avatar_type = user_cache_node->avatar_type;
			protocol.black_list[count].prof = user_cache_node->profession;
			protocol.black_list[count].headshot_id = user_cache_node->appearance.headshot_id;
			protocol.black_list[count].reserve_sh = 0;
			protocol.black_list[count].is_online = is_online;
			F_STRNCPY(protocol.black_list[count].gamename, user_cache_node->role_name, sizeof(protocol.black_list[count].gamename));
			memset(protocol.black_list[count].family_name, 0, sizeof(protocol.black_list[count].family_name));
			protocol.black_list[count].user_id = it_black->second.uid;
			protocol.black_list[count].level = user_cache_node->level;
			protocol.black_list[count].vip_level = 0;
			protocol.black_list[count].top_level = user_cache_node->top_level;
			protocol.black_list[count].last_online_timestamp = user_cache_node->last_online_timestamp;

			count += 1;
			if (count >= MAX_FRIEND_NUM)
			{
				break;
			}
		}
	}

	protocol.count = count;
	size_t length = sizeof(protocol) - ((MAX_FRIEND_NUM - protocol.count) * sizeof(protocol.black_list[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::SendBlackListToRoleToGameWorld(const UniqueServerID _usid, int _role_id)
{
	static crossgameprotocal::HiddenGameBlackList black_list;
	black_list.role_id = _role_id;
	black_list.usid = _usid;

	int count = 0;
	UserBlackInfoMapIt it = m_user_black_info_map.find(_role_id);
	if (m_user_black_info_map.end() != it)
	{
		for (UserBlackSetIt iter = it->second.user_black_set.begin(); iter != it->second.user_black_set.end(); ++iter)
		{
			long long black_for_index = *iter;

			BlackItemMapIt it_black = m_black_item_map.find(black_for_index);
			if (m_black_item_map.end() == it_black)
			{
				continue;
			}

			// 好友的在线状态处理 跨服里不计算 , 信息回到对应主服后 由主服去计算
			char online_status = this->GetOnlineStatus(_role_id , it_black->second.uid, false);

			UserCacheNode * black_cache_node = UserCacheManager::Instance().GetUserNode(it_black->second.uid);
			if (NULL != black_cache_node)
			{
				black_list.black_info[count].sex = black_cache_node->GetSex();
				black_list.black_info[count].prof = black_cache_node->profession;
				black_list.black_info[count].is_online = online_status;
				F_STRNCPY(black_list.black_info[count].role_name, black_cache_node->role_name, sizeof(black_list.black_info[count].role_name));
				memset(black_list.black_info[count].family_name, 0, sizeof(black_list.black_info[count].family_name));
				black_list.black_info[count].user_id = it_black->second.uid;
				black_list.black_info[count].intimacy = 0;
				black_list.black_info[count].level = black_cache_node->level;
				black_list.black_info[count].vip_level = 0;
				black_list.black_info[count].group_type = 0;
				black_list.black_info[count].avatar_timestamp = black_cache_node->avatar_timestamp;
				black_list.black_info[count].avatar_type = black_cache_node->avatar_type;
				black_list.black_info[count].headshot_id = black_cache_node->appearance.headshot_id;
				black_list.black_info[count].top_level = black_cache_node->top_level;
				black_list.black_info[count].last_online_timestamp = black_cache_node->last_online_timestamp;
				++count;
			}

			// 发到主服需要处理尾缀
			this->CheckOnlienStatusAndStrCut(online_status, black_list.black_info[count].role_name, _role_id, it_black->second.uid);

			if (count >= MAX_BLACK_NUM)
			{
				break;
			}
		}
	}

	black_list.count = count;

	size_t length = sizeof(black_list) - ((MAX_BLACK_NUM - black_list.count) * sizeof(black_list.black_info[0]));
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&black_list, length);
}

void FriendManager::AddFriendReqByGameWorld(Role * _role, int _target_id, bool _is_yi_jian)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenAddFriendReq req;
	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.target_id = _target_id;
	req.is_yijian = _is_yi_jian;

	_role->GetName(req.role_name);
	req.avatar_type = _role->GetAvatarType();
	req.prof = _role->GetProfession();
	req.level = _role->GetLevel();
	req.sex = _role->GetSex();
	req.headshot_id = _role->GetHeadshotID();
	req.top_level = _role->GetTopLevel();
	req.avatar_timestamp = _role->GetAvatarTimestamp();

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
}

void FriendManager::AddFriendReqByHidden(Role * _role, int _target_id, bool _is_yi_jian)
{
	if (NULL == _role || 0 == _target_id)
		return;

	// 1 .检查玩家和对方是否还能添加好友
	int role_error_num = 0;
	if (!this->CanAddFriend(_role->GetUID(), _target_id, &role_error_num, NULL))
	{
		// 申请添加好友只需要给申请人发错误信息就可以了 , 被申请人没必要知道
		_role->NoticeNum(role_error_num);
		return;
	}

	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_target_id));
	if (NULL != target_role)
	{
		// 目标玩家在隐藏服内
		this->AddTargetAskListByHidden(_target_id, _role, _is_yi_jian, true);
		this->SendAddFriendRouteByHidden(_role, target_role);
	}
	else
	{
		// 目标玩家不在在隐藏服内
		this->AddTargetAskListByHidden(_target_id, _role, _is_yi_jian, false);
	}

	if (0 == _is_yi_jian)
		_role->NoticeNum(noticenum::NT_SEND_ADD_FRIEND_REQ_SUCC);
}

void FriendManager::SendAddFriendRouteByHidden(Role * _role, Role * _target_role)
{
	if (NULL == _role || NULL == _target_role)
		return;

	Protocol::SCAddFriendRoute af_route;
	_role->GetName(af_route.ask_add_info.req_gamename);
	af_route.ask_add_info.req_user_id = _role->GetUID();
	af_route.ask_add_info.req_avatar_type = _role->GetAvatarType();
	af_route.ask_add_info.req_prof = _role->GetProfession();
	af_route.ask_add_info.req_level = _role->GetLevel();
	af_route.ask_add_info.headshot_id = _role->GetHeadshotID();
	af_route.ask_add_info.top_level = _role->GetTopLevel();
	af_route.ask_add_info.req_avatar_timestamp = 0;
	EngineAdapter::Instance().NetSend(_target_role->GetNetId(), (char*)&af_route, sizeof(Protocol::SCAddFriendRoute));
}

void FriendManager::SendAddFriendRouteByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, Role * _target_role)
{
	if (NULL == _req || NULL == _target_role)
		return;

	Protocol::SCAddFriendRoute af_route;
	F_STRNCPY(af_route.ask_add_info.req_gamename, _req->role_name, sizeof(GameName));
	af_route.ask_add_info.req_user_id = _req->role_id;
	af_route.ask_add_info.req_avatar_type = _req->avatar_type;
	af_route.ask_add_info.req_prof = _req->prof;
	af_route.ask_add_info.req_level = _req->level;
	af_route.ask_add_info.headshot_id = _req->headshot_id;
	af_route.ask_add_info.top_level = _req->top_level;
	af_route.ask_add_info.req_avatar_timestamp = _req->avatar_timestamp;
	EngineAdapter::Instance().NetSend(_target_role->GetNetId(), (char*)&af_route, sizeof(Protocol::SCAddFriendRoute));
}

void FriendManager::SendAddFriendRouteByGameWorld(crossgameprotocal::HiddenGameFriendAskListChange * _change, Role * _target_role)
{
	if (NULL == _change || NULL == _target_role)
		return;

	Protocol::SCAddFriendRoute af_route;
	F_STRNCPY(af_route.ask_add_info.req_gamename, _change->role_name, sizeof(GameName));
	af_route.ask_add_info.req_user_id = _change->role_id;
	af_route.ask_add_info.req_avatar_type = _change->avatar_type;
	af_route.ask_add_info.req_prof = _change->prof;
	af_route.ask_add_info.req_level = _change->level;
	af_route.ask_add_info.headshot_id = _change->headshot_id;
	af_route.ask_add_info.top_level = _change->top_level;
	af_route.ask_add_info.req_avatar_timestamp = _change->avatar_timestamp;
	EngineAdapter::Instance().NetSend(_target_role->GetNetId(), (char*)&af_route, sizeof(Protocol::SCAddFriendRoute));
}

void FriendManager::SendAddFriendRoute(Role * _role, Protocol::AskAddInfo & _ask_add_info)
{
	if (NULL == _role)
		return;

	Protocol::SCAddFriendRoute af_route;

	F_STRNCPY(af_route.ask_add_info.req_gamename, _ask_add_info.req_gamename, sizeof(GameName));
	af_route.ask_add_info.req_user_id = _ask_add_info.req_user_id;
	af_route.ask_add_info.req_avatar_type = _ask_add_info.req_avatar_type;
	af_route.ask_add_info.req_prof = _ask_add_info.req_prof;
	af_route.ask_add_info.req_level = _ask_add_info.req_level;
	af_route.ask_add_info.headshot_id = _ask_add_info.headshot_id;
	af_route.ask_add_info.top_level = _ask_add_info.top_level;
	af_route.ask_add_info.req_avatar_timestamp = _ask_add_info.req_avatar_timestamp;
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (char*)&af_route, sizeof(Protocol::SCAddFriendRoute));
}

bool  FriendManager::CanAddFriend(int _role_id, int _target_id, int * _role_error_num, int * _target_error_num)
{
	if (this->IsFriend(_role_id, _target_id))
	{
		if (NULL != _role_error_num)
			*_role_error_num = errornum::EN_FRIEND_EXIST;

		if (NULL != _target_error_num)
			*_target_error_num = errornum::EN_FRIEND_EXIST;
		return  false;
	}

	if (this->IsFriendFull(_role_id))
	{
		if (NULL != _role_error_num)
			*_role_error_num = errornum::EN_FRIEND_NUM_MAX;

		if (NULL != _target_error_num)
			*_target_error_num = errornum::EN_FRIEND_OTHER_FULL;
		return  false;
	}

	if (this->IsBlack(_role_id, _target_id))
	{
		if (NULL != _role_error_num)
			*_role_error_num = errornum::EN_IN_YOUR_BLACKLISTS;

		if (NULL != _target_error_num)
			*_target_error_num = errornum::EN_YOU_ARE_IN_BLACKLISTS;
		return  false;
	}

	if (this->IsFriendFull(_target_id))
	{
		if (NULL != _role_error_num)
			*_role_error_num = errornum::EN_FRIEND_OTHER_FULL;

		if (NULL != _target_error_num)
			*_target_error_num = errornum::EN_FRIEND_NUM_MAX;
		return  false;
	}

	if (this->IsBlack(_target_id, _role_id))
	{
		if (NULL != _role_error_num)
			*_role_error_num = errornum::EN_YOU_ARE_IN_BLACKLISTS;

		if (NULL != _target_error_num)
			*_target_error_num = errornum::EN_IN_YOUR_BLACKLISTS;
		return  false;
	}

	return true;
}

void FriendManager::AddTargetAskListByHidden(int _target_id, Role * _role, bool _is_yijian, bool _is_send_friend_route)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	static Protocol::AskAddInfo ask_info;

	_role->GetName(ask_info.req_gamename);
	ask_info.req_user_id = _role->GetUID();
	ask_info.req_avatar_type = _role->GetAvatarType();
	ask_info.req_prof = _role->GetProfession();
	ask_info.req_level = _role->GetLevel();
	ask_info.headshot_id = _role->GetHeadshotID();
	ask_info.top_level = _role->GetTopLevel();
	ask_info.req_avatar_timestamp = _role->GetAvatarTimestamp();

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_target_id);
	if (m_user_friend_info_map.end() == it)
	{
		UserFriendInfo temp_info;
		temp_info.ask_map.insert(std::make_pair(_role->GetUID(), ask_info));
		m_user_friend_info_map.insert(std::make_pair(_target_id, temp_info));
	}
	else
	{
		it->second.ask_map.insert(std::make_pair(_role->GetUID(), ask_info));
	}

	this->AskListChangeByInsertByHidden(_target_id, _role, _is_yijian, _is_send_friend_route);
}

void FriendManager::AddTargetAskListByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_route)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	static Protocol::AskAddInfo ask_info;

	F_STRNCPY(ask_info.req_gamename, _req->role_name, sizeof(GameName));
	ask_info.req_user_id = _req->role_id;
	ask_info.req_avatar_type = _req->avatar_type;
	ask_info.req_prof = _req->prof;
	ask_info.req_level = _req->level;
	ask_info.headshot_id = _req->headshot_id;
	ask_info.top_level = _req->top_level;
	ask_info.req_avatar_timestamp = _req->avatar_timestamp;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_req->target_id);
	if (m_user_friend_info_map.end() == it)
	{
		UserFriendInfo temp_info;
		temp_info.ask_map.insert(std::make_pair(_req->role_id, ask_info));
		m_user_friend_info_map.insert(std::make_pair(_req->target_id, temp_info));
	}
	else
	{
		it->second.ask_map.insert(std::make_pair(_req->role_id, ask_info));
	}

	this->AskListChangeByInsertByHidden(_req, _is_send_friend_route);
}

void FriendManager::AddTargetAskListByGameWorld(int _target_id, crossgameprotocal::HiddenGameFriendAskListChange * _change)
{
	if (NULL == _change || CrossConfig::Instance().IsHiddenServer())
		return;

	static Protocol::AskAddInfo ask_info;

	F_STRNCPY(ask_info.req_gamename, _change->role_name, sizeof(GameName));
	ask_info.req_user_id = _change->role_id;
	ask_info.req_avatar_type = _change->avatar_type;
	ask_info.req_prof = _change->prof;
	ask_info.req_level = _change->level;
	ask_info.headshot_id = _change->headshot_id;
	ask_info.top_level = _change->top_level;
	ask_info.req_avatar_timestamp = _change->avatar_timestamp;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_target_id);
	if (m_user_friend_info_map.end() == it)
	{
		UserFriendInfo temp_info;
		temp_info.ask_map.insert(std::make_pair(_change->role_id, ask_info));
		m_user_friend_info_map.insert(std::make_pair(_target_id, temp_info));
	}
	else
	{
		it->second.ask_map.insert(std::make_pair(_change->role_id, ask_info));
	}
}

void FriendManager::RemoveTargetAskList(int _target_id, int _role_id, bool _is_sync)
{
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_target_id);
	if (m_user_friend_info_map.end() != it)
	{
		it->second.ask_map.erase(_role_id);
	}

	if (_is_sync)
		this->AskListChangeByDeleteByHidden(_target_id, _role_id);
}

void FriendManager::SyncInfoAddFriendOfflineEvent(int _role_id, int _req)
{
	crossgameprotocal::SyncAddFriendOfflineEvent offlien_event;

	offlien_event.role_id = _role_id;
	offlien_event.req = _req;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (char *)&offlien_event, sizeof(offlien_event));
	}
	else
	{
		InternalComm::Instance().SendToHiddenThroughCross((char *)&offlien_event, sizeof(offlien_event));
	}
}

void FriendManager::UpdateFriendInfo(int _friend_id, FriendItem & _friend_item)
{
	UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(_friend_id);
	if (NULL != user_cache_node)
	{
		user_cache_node->GetName(_friend_item.role_name);
		_friend_item.sex = user_cache_node->GetSex();
		_friend_item.prof = user_cache_node->profession;
		_friend_item.level = user_cache_node->level;
		_friend_item.avatar_timestamp = user_cache_node->avatar_timestamp;
		_friend_item.avatar_type = user_cache_node->avatar_type;
		_friend_item.headshot_id = user_cache_node->appearance.headshot_id;
		_friend_item.last_online_timestamp = user_cache_node->last_online_timestamp;
	}
}

void FriendManager::RemoveFriend(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	int role_id = _role->GetUID();
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
	if (qing_yuan && qing_yuan->GetOtherSideRoleId(role_id) == _target_id)
	{
		_role->NoticeNum(errornum::EN_FRIEND_REMOVE_LOVER_LIMIT);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->RemoveFriendByHidden(_role->GetUID(), _target_id , __FUNCTION__);
	}
	else
	{
		this->RemoveFriendByGameWorld(_role, _target_id);
	}
}

void FriendManager::RemoveFriendByGameWorld(Role * _role, int _target_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenRemoveFriendReq req;

	req.role_id = _role->GetUID();
	req.target_id = _target_id;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&req, sizeof(req));
}

void FriendManager::RemoveFriendByHidden(int _role_id, int _target_id, const  char * _reason)
{
	// 隐藏服内有双方的好友数据 
	// 删除后再同步数据即可

	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	// 删除好友也要双向删除
	this->RemoveFriendHelpByHidden(_role_id, _target_id, _reason);
	this->RemoveFriendHelpByHidden(_target_id, _role_id, _reason);

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_id));
	if (NULL != role)
	{
		this->SendFriendChangeToRoleByHidden(role, _target_id, Protocol::SCChangeFriend::CHANGE_TYPE_DEL);
	}
	else
	{
		this->SendFriendChangeToRoleToGameWorld(_role_id, _target_id, Protocol::SCChangeFriend::CHANGE_TYPE_DEL);
	}

	Role * friend_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_target_id));
	if (NULL != friend_role)
	{
		this->SendFriendChangeToRoleByHidden(friend_role, _role_id, Protocol::SCChangeFriend::CHANGE_TYPE_DEL);
	}
	else
	{
		this->SendFriendChangeToRoleToGameWorld(_target_id, _role_id, Protocol::SCChangeFriend::CHANGE_TYPE_DEL);
	}

	gamelog::g_log_social.printf(LL_INFO, "%s role_id:%d , target_id:%d  reason:%s", __FUNCTION__, _role_id, _target_id,
		NULL == _reason ? "" : _reason);
}

bool FriendManager::RemoveFriendHelpByHidden(int _role_id, int _target_id, const  char * _reason)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return false;

	UserFrinedInfoMapIt it_user = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() == it_user)
		return false;

	long long friend_for_index = TranslateInt64(_role_id, _target_id);
	FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
	if (m_friend_item_map.end() == it)
	{
		return false;
	}

	m_friend_item_map.erase(friend_for_index);
	it_user->second.user_friend_set.erase(friend_for_index);

	// 删除好友也需要删除亲密的关系
	FriendIntimacyManager::Instance().OnRemoveFriend(_role_id, _target_id);
	// 同步数据到其他服务器
	this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_DELETE);

	// 记录一下日志
	gamelog::g_log_social.printf(LL_INFO, "FriendManager::RemoveFriendHelpByHidden  user[%d] friend[%d] reason:%s  ",
		_role_id, _target_id, NULL == _reason ? "" : _reason);

	return true;
}

void FriendManager::AddBlack(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	int role_id = _role->GetUID();
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
	if (qing_yuan && qing_yuan->GetOtherSideRoleId(role_id) == _target_id)
	{
		_role->NoticeNum(errornum::EN_FRIEND_REMOVE_LOVER_LIMIT);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		int error_num = 0;
		if (!this->AddBlackByHidden(_role->GetUID(), _target_id, &error_num))
		{
			if (0 != error_num)
			{
				_role->NoticeNum(error_num);
			}
		}
		else
		{
			this->SendBlackChangeToRoleByHidden(_role, _target_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE);
		}
	}
	else
	{
		this->AddBlackByGameWorld(_role, _target_id);
	}
}

void FriendManager::AddBlackByGameWorld(Role * _role, int _target_id)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenAddBlackReq req;

	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.target_id = _target_id;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&req, sizeof(req));
}

bool FriendManager::AddBlackByHidden(int _role_id, int _target_id, int * _error_num)
{
	if (_target_id == _role_id || !CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (this->IsBlack(_role_id, _target_id))
	{
		if (_error_num != NULL)
			*_error_num = errornum::EN_IN_YOUR_BLACKLISTS;
		return false;
	}

	UserBlackInfoMapIt it = m_user_black_info_map.find(_role_id);
	if (m_user_black_info_map.end() != it)
	{
		if ((int)it->second.user_black_set.size() >= MAX_BLACK_NUM)
		{
			if (_error_num != NULL)
				*_error_num = errornum::EN_BLACKLISTS_NUM_MAX;
			return false;
		}
	}

	if (this->IsFriend(_role_id, _target_id))
	{
		this->RemoveFriendByHidden(_role_id, _target_id , __FUNCTION__);
		gamelog::g_log_social.printf(LL_INFO, "FriendManager::AddBlack  role_uid:[%d], friend_uid:%d  reason[black_remove]",
			_role_id, _target_id);
	}

	this->AddBlackHelpByHidden(_role_id, _target_id);

	return true;
}

void FriendManager::AddBlackHelpByHidden(int _role_id, int _target_id)
{
	BlackItem  black_item;
	black_item.role_id = _role_id;
	black_item.uid = _target_id;

	long long black_for_index = this->AddBlackInfo(_role_id, _target_id, black_item);
	this->AddUserBlackRelation(_role_id, black_for_index);

	this->SyncBlackDataToGameWorld(black_for_index, crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_INSERT);
}

void FriendManager::SendBlackChangeToRoleByHidden(Role * _role, int _black_id, int _change_type , int _online_status )
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCChangeBlack cf;
	cf.change_type = _change_type;
	cf.info.user_id = _black_id;

	if (_change_type == Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_UPDATE)
	{
		long long black_for_index = TranslateInt64(_role->GetUID(), _black_id);

		BlackItemMapIt iter = m_black_item_map.find(black_for_index);
		if (iter == m_black_item_map.end())
		{
			return;
		}

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(_black_id);
		if (NULL != user_cache_node)
		{
			char is_online = this->GetOnlineStatus(_role->GetUID(), _black_id, true, _online_status);

			cf.info.user_id = _black_id;
			F_STRNCPY(cf.info.gamename, user_cache_node->role_name, sizeof(cf.info.gamename));
			memset(cf.info.family_name, 0, sizeof(cf.info.family_name));
			cf.info.avatar_type = user_cache_node->avatar_type;
			cf.info.prof = user_cache_node->profession;
			cf.info.headshot_id = user_cache_node->appearance.headshot_id;
			cf.info.reserve_sh = 0;
			cf.info.is_online = is_online;
			cf.info.level = user_cache_node->level;;
			cf.info.vip_level = 0;
			cf.info.top_level = user_cache_node->top_level;
			cf.info.avatar_timestamp = 0;
			cf.info.last_online_timestamp = user_cache_node->last_online_timestamp;
		}
	}

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&cf, sizeof(cf));
}

void FriendManager::SendBlackChangeToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameBlackListChange * _change)
{
	if (NULL == _role || NULL == _change || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCChangeBlack cf;

	cf.change_type = _change->change_type;
	cf.info.user_id = _change->black_info.user_id;
	F_STRNCPY(cf.info.gamename, _change->black_info.role_name, sizeof(cf.info.gamename));
	memset(cf.info.family_name, 0, sizeof(cf.info.family_name));
	cf.info.avatar_type = _change->black_info.avatar_type;
	cf.info.prof = _change->black_info.prof;
	cf.info.headshot_id = _change->black_info.headshot_id;
	cf.info.reserve_sh = 0;
	cf.info.is_online = _change->black_info.is_online;
	cf.info.level = _change->black_info.level;
	cf.info.vip_level = 0;
	cf.info.top_level = _change->black_info.top_level;
	cf.info.avatar_timestamp = _change->black_info.avatar_timestamp;
	cf.info.last_online_timestamp = _change->black_info.last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (char*)&cf, sizeof(cf));
}

void FriendManager::SendBlackChangeToRoleToGameWorld( int _role_id, int _black_id, int _change_type ,int _online_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	static crossgameprotocal::HiddenGameBlackListChange change;
	change.role_id = _role_id;
	change.change_type = _change_type;

	UserCacheNode * black_cache_node = UserCacheManager::Instance().GetUserNode(_black_id);
	if (NULL == black_cache_node)
		return;

	char is_online = this->GetOnlineStatus(_role_id, _black_id, false, _online_status);
	F_STRNCPY(change.black_info.role_name, black_cache_node->role_name, sizeof(change.black_info.role_name));
	// 发送到主服需要处理名字尾缀
	this->CheckOnlienStatusAndStrCut(is_online, change.black_info.role_name , _role_id, _black_id);

	change.black_info.sex = black_cache_node->GetSex();
	change.black_info.prof = black_cache_node->profession;
	change.black_info.is_online = is_online;

	memset(change.black_info.family_name, 0, sizeof(change.black_info.family_name));
	change.black_info.user_id = _black_id;
	change.black_info.intimacy = 0;
	change.black_info.level = black_cache_node->level;
	change.black_info.vip_level = 0;
	change.black_info.group_type = 0;
	change.black_info.avatar_timestamp = black_cache_node->avatar_timestamp;
	change.black_info.avatar_type = black_cache_node->avatar_type;
	change.black_info.headshot_id = black_cache_node->appearance.headshot_id;
	change.black_info.top_level = black_cache_node->top_level;
	change.black_info.last_online_timestamp = black_cache_node->last_online_timestamp;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&change, sizeof(change));
}

void FriendManager::RemoveBlack(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->RemoveBlackByHidden(_role->GetUID(), _target_id);
		this->SendBlackChangeToRoleByHidden(_role, _target_id, Protocol::SCChangeBlack::CHANGE_TYPE_BLACK_DEL);
	}
	else
	{
		this->RemoveBlackByGameWorld(_role, _target_id);
	}
}

void FriendManager::RemoveBlackByGameWorld(Role * _role, int _target_id)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenRemoveBlackReq req;
	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.target_id = _target_id;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));
}

void FriendManager::RemoveBlackByHidden(int _role_id, int _target_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	long long black_for_index = this->RemoveBlackInfo(_role_id, _target_id);
	this->RemoveUserBlackRelation(_role_id, black_for_index);

	this->SyncBlackDataToGameWorld(black_for_index, crossgameprotocal::SYNC_BLACK_TO_CROSS_STATUS_DELETE);

	gamelog::g_log_social.printf(LL_INFO , "%s role_id:%d target_id:%d" , __FUNCTION__ , _role_id , _target_id);
}

void FriendManager::DelGroup(Role * _role, int _group_type)
{
	if (NULL == _role)
		return;

	int group_index = _group_type - 1;
	if (group_index < 0 || group_index >= MAX_FRIEND_GROUP_NUM)
	{
		return;
	}

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();
	memset(friend_common_param.group_name[group_index], 0, sizeof(friend_common_param.group_name[group_index]));
	this->SendGroupInfo(_role);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->DelGroupByHidden(_role->GetUID(), _group_type);
	}
	else
	{
		this->DelGroupByGameWorld(_role, _group_type);
	}
}

void FriendManager::DelGroupByHidden(int _role_id, int _group_type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		UserFrinedSetIt it_begin = it->second.user_friend_set.begin();
		UserFrinedSetIt it_end = it->second.user_friend_set.end();

		for (; it_begin != it_end; ++it_begin)
		{
			FriendItemMapIt it_friend = m_friend_item_map.find(*it_begin);
			if (m_friend_item_map.end() != it_friend && it_friend->second.group_type == _group_type)
			{
				it_friend->second.group_type = 0;
				this->SyncFriendDataToGameWorld(*it_begin, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA);
			}
		}
	}
}

void FriendManager::DelGroupByGameWorld(Role * _role, int _group_type)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenFriendGroupReq req;

	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_DEL_GROUP;
	req.group_type = _group_type;
	memset(req.group_name, 0, sizeof(req.group_name));
	req.count = 0;
	memset(req.friend_id_arr, 0, sizeof(req.friend_id_arr));

	int length = sizeof(req) - (MAX_FRIEND_NUM - req.count) * sizeof(req.friend_id_arr[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, length);
}

void FriendManager::SendGroupInfo(Role * _role)
{
	if (NULL == _role)
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();
	Protocol::SCFriendGroupInfo protocol;

	for (int i = 0; i < MAX_FRIEND_GROUP_NUM; ++i)
	{
		memcpy(protocol.group_name[i], friend_common_param.group_name[i], sizeof(protocol.group_name[i]));
	}

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void FriendManager::SendAddList(Role * _role)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendAddListByHidden(_role);
	}
	else
	{
		this->SendAddListByGameWorld(_role);
	}
}

void FriendManager::SendAddListByHidden(Role * _role)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role->GetUID());
	if (m_user_friend_info_map.end() != it)
	{
		for (std::map<int, Protocol::AskAddInfo>::iterator iter = it->second.ask_map.begin(); iter != it->second.ask_map.end(); ++iter)
		{
			UserCacheNode* target_node_ptr = UserCacheManager::Instance().GetUserNode(iter->first);
			if (NULL == target_node_ptr)
			{
				this->SendAddFriendRoute(_role, iter->second);
			}
			else
			{
				static Protocol::AskAddInfo info;

				target_node_ptr->GetName(info.req_gamename);
				info.req_user_id = target_node_ptr->uid;
				info.req_avatar_type = target_node_ptr->avatar_type;
				info.req_prof = target_node_ptr->profession;
				info.req_level = target_node_ptr->level;
				info.headshot_id = target_node_ptr->appearance.headshot_id;
				info.top_level = target_node_ptr->top_level;
				info.req_avatar_timestamp = target_node_ptr->avatar_timestamp;

				this->SendAddFriendRoute(_role, info);
			}
		}
	}
}

void FriendManager::SendAddListByGameWorld(Role * _role)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenGetFriendInfo info;

	info.usid = _role->GetUniqueServerID();
	info.role_id = _role->GetUID();
	info.info_type = crossgameprotocal::GAME_HIDDEN_GET_FRIEND_INFO_TYPE_ASK_LIST;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&info, sizeof(info));
}

void FriendManager::SendAddListToRoleToGameWorld(UniqueServerID _usid, int _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	static crossgameprotocal::HiddenGameAskListToRole ask_list;
	ask_list.usid = _usid;
	ask_list.role_id = _role_id;
	ask_list.count = 0;
	memset(ask_list.ask_info, 0, sizeof(ask_list.ask_info));

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		for (std::map<int, Protocol::AskAddInfo>::iterator iter = it->second.ask_map.begin(); iter != it->second.ask_map.end() && ask_list.count < MAX_FRIEND_NUM; ++iter)
		{
			UserCacheNode* target_node_ptr = UserCacheManager::Instance().GetUserNode(iter->first);
			if (NULL != target_node_ptr)
			{
				ask_list.ask_info[ask_list.count].user_id = target_node_ptr->uid;
				target_node_ptr->GetName(ask_list.ask_info[ask_list.count].role_name);
				ask_list.ask_info[ask_list.count].avatar_type = target_node_ptr->avatar_type;

				ask_list.ask_info[ask_list.count].prof = target_node_ptr->profession;
				ask_list.ask_info[ask_list.count].level = target_node_ptr->level;
				ask_list.ask_info[ask_list.count].headshot_id = target_node_ptr->appearance.headshot_id;
				ask_list.ask_info[ask_list.count].top_level = target_node_ptr->top_level;
				ask_list.ask_info[ask_list.count].avatar_timestamp = target_node_ptr->avatar_timestamp;
				ask_list.ask_info[ask_list.count].avatar_type = target_node_ptr->avatar_type;
				++ask_list.count;
			}
		}
	}

	if (ask_list.count <= 0)
		return;

	size_t length = sizeof(ask_list) - ((MAX_FRIEND_NUM - ask_list.count) * sizeof(ask_list.ask_info[0]));
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&ask_list, length);
}

void FriendManager::SendAskListToRoleFromHidden(Role * _role, crossgameprotocal::HiddenGameAskListToRole * _ask_list)
{
	if (NULL == _role || NULL == _ask_list || CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _ask_list->count && i < MAX_FRIEND_NUM; ++i)
	{
		static Protocol::AskAddInfo info;

		F_STRNCPY(info.req_gamename, _ask_list->ask_info[i].role_name, sizeof(info.req_gamename));
		info.req_user_id = _ask_list->ask_info[i].user_id;
		info.req_avatar_type = _ask_list->ask_info[i].avatar_type;
		info.req_prof = _ask_list->ask_info[i].prof;
		info.req_level = _ask_list->ask_info[i].level;
		info.headshot_id = _ask_list->ask_info[i].headshot_id;
		info.top_level = _ask_list->ask_info[i].top_level;
		info.req_avatar_timestamp = _ask_list->ask_info[i].avatar_timestamp;

		this->SendAddFriendRoute(_role, info);
	}
}

void FriendManager::OnSendRecentlyInfoToGameWorld(crossgameprotocal::GameHiddenRecentlyReq * _req, int _info_type)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameRecentlyInfo info;
	info.usid = _req->usid;
	info.role_id = _req->role_id;
	info.info_type = _info_type;
	info.count = 0;

	int count = 0;
	UNSTD_STATIC_CHECK(MAX_RECENTLY_CHAT_NUM == MAX_RECENTLY_TEAM_NUM);
	for (int i = 0; i < _req->count && count < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(_req->target_id[i]);
		if (NULL == user_cache_node)
		{
			continue;
		}

		char is_online = this->GetOnlineStatus(_req->role_id, _req->target_id[i] ,false);
		user_cache_node->GetName(info.info_arr[count].role_name);
		// 发送到主服需要处理名字尾缀
		this->CheckOnlienStatusAndStrCut(is_online, info.info_arr[count].role_name , _req->role_id, _req->target_id[i]);

		info.info_arr[count].user_id = user_cache_node->uid;
		memset(info.info_arr[count].family_name, 0, sizeof(info.info_arr[0].family_name));
		info.info_arr[count].intimacy = 0;
		info.info_arr[count].sex = user_cache_node->GetSex();
		info.info_arr[count].is_online = is_online;
		info.info_arr[count].prof = user_cache_node->GetProfession();
		info.info_arr[count].level = user_cache_node->level;
		info.info_arr[count].vip_level = 0;
		info.info_arr[count].group_type = 0;
		info.info_arr[count].avatar_timestamp = user_cache_node->avatar_timestamp;
		info.info_arr[count].avatar_type = user_cache_node->avatar_type;
		info.info_arr[count].headshot_id = user_cache_node->appearance.headshot_id;
		info.info_arr[count].top_level = user_cache_node->top_level;
		info.info_arr[count].last_online_timestamp = user_cache_node->last_online_timestamp;

		++count;
	}
	info.count = count;

	int length = sizeof(info) - (MAX_RECENTLY_CHAT_NUM - info.count) * sizeof(info.info_arr[0]);
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_req->role_id).db_index, (const char *)&info, length);
}

void FriendManager::AddRecentlyChatByHidden(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	bool is_has = false;
	for (int i = 0; i < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		if (_target_id == friend_common_param.curr_recently_chat[i])
		{
			is_has = true;
			break;
		}
	}

	if (!is_has)
	{
		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(_target_id);
		if (NULL == user_cache_node)
		{
			return;
		}

		int del_role_id = friend_common_param.curr_recently_chat[friend_common_param.curr_recently_chat_index];
		friend_common_param.curr_recently_chat[friend_common_param.curr_recently_chat_index++] = _target_id;
		friend_common_param.curr_recently_chat_index = friend_common_param.curr_recently_chat_index % MAX_RECENTLY_CHAT_NUM;

		this->SendRecentlyChatChangeByHidden(_role, del_role_id, _target_id);
	}
	else
	{
		this->SendRecentlyChatChangeByHidden(_role, 0, _target_id);
	}
}

void FriendManager::AddRecentlyChatByGameWorld(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	crossgameprotocal::GameHiddenRecentlyReq req;

	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_CHAT;
	req.count = 1;
	req.target_id[0] = _target_id;

	int length = sizeof(req) - (MAX_RECENTLY_CHAT_NUM - req.count) * sizeof(req.target_id[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, length);
}

void FriendManager::SendRecentlyChatChangeByHidden(Role * _role, int _del_role_id, int _target_id)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCRecentlyChatChange protocol;
	protocol.del_role_id = _del_role_id;

	UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(_target_id);
	if (NULL == user_cache_node)
	{
		return;
	}

	char is_online = this->GetOnlineStatus(_role->GetUID(), _target_id, true);

	protocol.recently_chat_role.avatar_type = user_cache_node->avatar_type;
	protocol.recently_chat_role.prof = user_cache_node->profession;
	protocol.recently_chat_role.headshot_id = user_cache_node->appearance.headshot_id;
	protocol.recently_chat_role.reserve_sh = 0;
	protocol.recently_chat_role.is_online = is_online;
	F_STRNCPY(protocol.recently_chat_role.gamename, user_cache_node->role_name, sizeof(protocol.recently_chat_role.gamename));
	memset(protocol.recently_chat_role.family_name, 0, sizeof(protocol.recently_chat_role.family_name));
	protocol.recently_chat_role.user_id = user_cache_node->uid;
	protocol.recently_chat_role.level = user_cache_node->level;
	protocol.recently_chat_role.vip_level = 0;
	protocol.recently_chat_role.top_level = user_cache_node->top_level;
	protocol.recently_chat_role.last_online_timestamp = user_cache_node->last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void FriendManager::SendRecentlyChatChangeFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || NULL == _info || _info->count <= 0 || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCRecentlyChatChange protocol;
	protocol.del_role_id = _del_role_id;

	protocol.recently_chat_role.avatar_type = _info->info_arr[0].avatar_type;
	protocol.recently_chat_role.prof = _info->info_arr[0].prof;
	protocol.recently_chat_role.headshot_id = _info->info_arr[0].headshot_id;
	protocol.recently_chat_role.reserve_sh = 0;
	protocol.recently_chat_role.is_online = _info->info_arr[0].is_online;
	F_STRNCPY(protocol.recently_chat_role.gamename, _info->info_arr[0].role_name, sizeof(protocol.recently_chat_role.gamename));
	memset(protocol.recently_chat_role.family_name, 0, sizeof(protocol.recently_chat_role.family_name));
	protocol.recently_chat_role.user_id = _info->info_arr[0].user_id;
	protocol.recently_chat_role.level = _info->info_arr[0].level;
	protocol.recently_chat_role.vip_level = 0;
	protocol.recently_chat_role.top_level = _info->info_arr[0].top_level;
	protocol.recently_chat_role.last_online_timestamp = _info->info_arr[0].last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void FriendManager::AddRecentlyChatFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || NULL == _info || CrossConfig::Instance().IsHiddenServer() || _info->count <= 0)
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	bool is_has = false;
	for (int i = 0; i < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		if (_info->info_arr[0].user_id == friend_common_param.curr_recently_chat[i])
		{
			is_has = true;
			break;
		}
	}

	if (!is_has)
	{
		int del_role_id = friend_common_param.curr_recently_chat[friend_common_param.curr_recently_chat_index];
		friend_common_param.curr_recently_chat[friend_common_param.curr_recently_chat_index++] = _info->info_arr[0].user_id;
		friend_common_param.curr_recently_chat_index = friend_common_param.curr_recently_chat_index % MAX_RECENTLY_CHAT_NUM;

		this->SendRecentlyChatChangeFromHidden(_role, del_role_id, _info);
	}
	else
	{
		this->SendRecentlyChatChangeFromHidden(_role, 0, _info);
	}

}

void FriendManager::SendRecentlyChatInfo(Role * _role)
{
	// 原逻辑里已经废弃 , 这里保留修改后的逻辑 , 以便以后某个时候又需要了
	return;

	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendRecentlyChatInfoByHidden(_role);
	}
	else
	{
		this->SendRecentlyChatInfoByGameWorld(_role);
	}
}

void FriendManager::SendRecentlyChatInfoByHidden(Role * _role)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	Protocol::SCRecentlyChatInfo protocol;
	protocol.recently_chat_count = 0;
	int & count = protocol.recently_chat_count;

	for (int i = 0; i < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		if (friend_common_param.curr_recently_chat[i] == 0)
		{
			break;
		}

		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(friend_common_param.curr_recently_chat[i]);
		if (NULL == user_cache_node)
		{
			continue;
		}

		char is_online = this->GetOnlineStatus(_role->GetUID(), user_cache_node->uid, true);

		protocol.recently_chat_role[count].avatar_type = user_cache_node->avatar_type;
		protocol.recently_chat_role[count].prof = user_cache_node->profession;
		protocol.recently_chat_role[count].headshot_id = user_cache_node->appearance.headshot_id;
		protocol.recently_chat_role[count].reserve_sh = 0;
		protocol.recently_chat_role[count].is_online = is_online;
		F_STRNCPY(protocol.recently_chat_role[count].gamename, user_cache_node->role_name, sizeof(protocol.recently_chat_role[0].gamename));
		memset(protocol.recently_chat_role[count].family_name, 0, sizeof(protocol.recently_chat_role[0].family_name));
		protocol.recently_chat_role[count].user_id = user_cache_node->uid;
		protocol.recently_chat_role[count].level = user_cache_node->level;
		protocol.recently_chat_role[count].vip_level = 0;
		protocol.recently_chat_role[count].top_level = user_cache_node->top_level;
		protocol.recently_chat_role[count].last_online_timestamp = user_cache_node->last_online_timestamp;

		++count;
	}

	size_t length = sizeof(protocol) - ((MAX_RECENTLY_CHAT_NUM - protocol.recently_chat_count) * sizeof(protocol.recently_chat_role[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::SendRecentlyChatInfoByGameWorld(Role * _role)
{
	if (NULL == _role)
		return;

	crossgameprotocal::GameHiddenRecentlyReq req;

	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_CHAT_INFO;
	req.count = 0;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	int count = 0;
	for (int i = 0; i < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		if (friend_common_param.curr_recently_chat[i] == 0)
			break;

		req.target_id[count++] = friend_common_param.curr_recently_chat[i];
	}

	if (count <= 0)
		return;

	req.count = count;

	int length = sizeof(req) - (MAX_RECENTLY_CHAT_NUM - req.count) * sizeof(req.target_id[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, length);
}

void FriendManager::SendRecentlyChatInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || NULL == _info || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCRecentlyChatInfo protocol;
	protocol.recently_chat_count = 0;
	int & count = protocol.recently_chat_count;

	for (int i = 0; i < _info->count && count < MAX_RECENTLY_CHAT_NUM; ++i)
	{
		protocol.recently_chat_role[count].avatar_type = _info->info_arr[i].avatar_type;
		protocol.recently_chat_role[count].prof = _info->info_arr[i].prof;
		protocol.recently_chat_role[count].headshot_id = _info->info_arr[i].headshot_id;
		protocol.recently_chat_role[count].reserve_sh = 0;
		protocol.recently_chat_role[count].is_online = _info->info_arr[i].is_online;
		F_STRNCPY(protocol.recently_chat_role[count].gamename, _info->info_arr[i].role_name, sizeof(protocol.recently_chat_role[0].gamename));
		memset(protocol.recently_chat_role[count].family_name, 0, sizeof(protocol.recently_chat_role[0].family_name));
		protocol.recently_chat_role[count].user_id = _info->info_arr[i].user_id;
		protocol.recently_chat_role[count].level = _info->info_arr[i].level;
		protocol.recently_chat_role[count].vip_level = 0;
		protocol.recently_chat_role[count].top_level = _info->info_arr[i].top_level;
		protocol.recently_chat_role[count].last_online_timestamp = _info->info_arr[i].last_online_timestamp;

		++count;
	}

	size_t length = sizeof(protocol) - ((MAX_RECENTLY_CHAT_NUM - protocol.recently_chat_count) * sizeof(protocol.recently_chat_role[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);

}

void FriendManager::AddRecentlyTeamByHidden(Role * _role, int _target_id)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	FriendCommonParam & friend_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	bool is_has = false;
	for (int i = 0; i < MAX_RECENTLY_TEAM_NUM; ++i)
	{
		if (_target_id == friend_param.recently_team[i])
		{
			is_has = true;
			break;
		}
	}

	if (!is_has)
	{
		int del_role_id = friend_param.recently_team[friend_param.curr_recently_team_index];
		friend_param.recently_team[friend_param.curr_recently_team_index++] = _target_id;
		friend_param.curr_recently_team_index = friend_param.curr_recently_team_index % MAX_RECENTLY_TEAM_NUM;

		this->SendRecentlyTeamChangeInfoByHidden(_role, del_role_id, _target_id);
	}
}

void FriendManager::AddRecentlyTeamByGameWorld(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	crossgameprotocal::GameHiddenRecentlyReq req;

	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_TEAM;
	req.count = 1;
	req.target_id[0] = _target_id;

	int length = sizeof(req) - (MAX_RECENTLY_CHAT_NUM - req.count) * sizeof(req.target_id[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, length);
}

void FriendManager::AddRecentlyTeamFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || NULL == _info || CrossConfig::Instance().IsHiddenServer() || _info->count <= 0)
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	int target_id = _info->info_arr[0].user_id;
	bool is_has = false;
	for (int i = 0; i < MAX_RECENTLY_TEAM_NUM; ++i)
	{
		if (target_id == friend_common_param.recently_team[i])
		{
			is_has = true;
			break;
		}
	}

	if (!is_has)
	{
		int del_role_id = friend_common_param.recently_team[friend_common_param.curr_recently_team_index];
		friend_common_param.recently_team[friend_common_param.curr_recently_team_index++] = target_id;
		friend_common_param.curr_recently_team_index = friend_common_param.curr_recently_team_index % MAX_RECENTLY_TEAM_NUM;

		this->SendRecentlyTeamChangeInfoFromHidden(_role, del_role_id, _info);
	}
}

void FriendManager::SendRecentlyTeamChangeInfoByHidden(Role * _role, int _del_role_id, int _target_id)
{
	if (NULL == _role)
		return;

	Protocol::SCOneRecentlyTeamInfo protocol;
	protocol.del_role_id = _del_role_id;

	UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(_target_id);
	if (NULL == user_cache_node)
	{
		return;
	}

	char is_online = this->GetOnlineStatus(_role->GetUID(), _target_id, true); 
	protocol.recently_team_role.avatar_type = user_cache_node->avatar_type;
	protocol.recently_team_role.prof = user_cache_node->profession;
	protocol.recently_team_role.headshot_id = user_cache_node->appearance.headshot_id;
	protocol.recently_team_role.reserve_sh = 0;
	protocol.recently_team_role.is_online = is_online;
	F_STRNCPY(protocol.recently_team_role.gamename, user_cache_node->role_name, sizeof(protocol.recently_team_role.gamename));
	memset(protocol.recently_team_role.family_name, 0, sizeof(protocol.recently_team_role.family_name));
	protocol.recently_team_role.user_id = user_cache_node->uid;
	protocol.recently_team_role.level = user_cache_node->level;
	protocol.recently_team_role.vip_level = 0;
	protocol.recently_team_role.top_level = user_cache_node->top_level;
	protocol.recently_team_role.last_online_timestamp = user_cache_node->last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void FriendManager::SendRecentlyTeamChangeInfoFromHidden(Role * _role, int _del_role_id, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || NULL == _info || _info->count <= 0)
		return;

	Protocol::SCOneRecentlyTeamInfo protocol;
	protocol.del_role_id = _del_role_id;

	protocol.recently_team_role.user_id = _info->info_arr[0].user_id;
	protocol.recently_team_role.avatar_type = _info->info_arr[0].avatar_type;
	protocol.recently_team_role.prof = _info->info_arr[0].prof;
	protocol.recently_team_role.headshot_id = _info->info_arr[0].headshot_id;
	protocol.recently_team_role.reserve_sh = 0;
	protocol.recently_team_role.is_online = _info->info_arr[0].is_online;
	F_STRNCPY(protocol.recently_team_role.gamename, _info->info_arr[0].role_name, sizeof(protocol.recently_team_role.gamename));
	memset(protocol.recently_team_role.family_name, 0, sizeof(protocol.recently_team_role.family_name));
	protocol.recently_team_role.level = _info->info_arr[0].level;
	protocol.recently_team_role.vip_level = 0;
	protocol.recently_team_role.top_level = _info->info_arr[0].top_level;
	protocol.recently_team_role.last_online_timestamp = _info->info_arr[0].last_online_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void FriendManager::SendRecentlyTeamInfo(Role * _role)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendRecentlyTeamInfoByHidden(_role);
	}
	else
	{
		this->SendRecentlyTeamInfoByGameWorld(_role);
	}
}

void FriendManager::SendRecentlyTeamInfoByHidden(Role * _role)
{
	if (NULL == _role ||!CrossConfig::Instance().IsHiddenServer())
		return;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	Protocol::SCRecentlyTeamInfo protocol;
	protocol.recently_team_role_count = 0;
	int & count = protocol.recently_team_role_count;

	for (int i = 0; i < MAX_RECENTLY_TEAM_NUM; ++i)
	{
		if (friend_common_param.recently_team[i] == 0)
		{
			break;
		}

		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(friend_common_param.recently_team[i]);
		if (NULL == user_cache_node)
		{
			continue;
		}

		char is_online = this->GetOnlineStatus(_role->GetUID(), friend_common_param.recently_team[i], true); 
		protocol.recently_team_role[count].avatar_type = user_cache_node->avatar_type;
		protocol.recently_team_role[count].prof = user_cache_node->profession;
		protocol.recently_team_role[count].headshot_id = user_cache_node->appearance.headshot_id;
		protocol.recently_team_role[count].reserve_sh = 0;
		protocol.recently_team_role[count].is_online = is_online;
		F_STRNCPY(protocol.recently_team_role[count].gamename, user_cache_node->role_name, sizeof(protocol.recently_team_role[0].gamename));
		memset(protocol.recently_team_role[count].family_name, 0, sizeof(protocol.recently_team_role[0].family_name));
		protocol.recently_team_role[count].user_id = user_cache_node->uid;
		protocol.recently_team_role[count].level = user_cache_node->level;
		protocol.recently_team_role[count].vip_level = 0;
		protocol.recently_team_role[count].top_level = user_cache_node->top_level;
		protocol.recently_team_role[count].last_online_timestamp = user_cache_node->last_online_timestamp;

		++count;
	}

	size_t length = sizeof(protocol) - ((MAX_RECENTLY_TEAM_NUM - protocol.recently_team_role_count) * sizeof(protocol.recently_team_role[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::SendRecentlyTeamInfoByGameWorld(Role * _role)
{
	if (NULL == _role)
		return;

	crossgameprotocal::GameHiddenRecentlyReq req;

	req.usid = _role->GetUniqueServerID();
	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_TEAM_INFO;
	req.count = 0;

	FriendCommonParam & friend_common_param = _role->GetRoleModuleManager()->GetFriendCommonData();

	int count = 0;
	for (int i = 0; i < MAX_RECENTLY_TEAM_NUM; ++i)
	{
		if (0 == friend_common_param.recently_team[i])
			break;

		req.target_id[count++] = friend_common_param.recently_team[i];
	}

	if (count <= 0)
		return;

	req.count = count;

	int length = sizeof(req) - (MAX_RECENTLY_TEAM_NUM - req.count) * sizeof(req.target_id[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, length);
}

void FriendManager::SendRecentlyTeamInfoFromHidden(Role * _role, crossgameprotocal::HiddenGameRecentlyInfo * _info)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCRecentlyTeamInfo protocol;
	protocol.recently_team_role_count = 0;
	int & count = protocol.recently_team_role_count;

	for (int i = 0; i < _info->count && count < MAX_RECENTLY_TEAM_NUM; ++i)
	{
		protocol.recently_team_role[count].avatar_type = _info->info_arr[i].avatar_type;
		protocol.recently_team_role[count].prof = _info->info_arr[i].prof;
		protocol.recently_team_role[count].headshot_id = _info->info_arr[i].headshot_id;
		protocol.recently_team_role[count].reserve_sh = 0;
		protocol.recently_team_role[count].is_online = _info->info_arr[i].is_online;
		F_STRNCPY(protocol.recently_team_role[count].gamename, _info->info_arr[i].role_name, sizeof(protocol.recently_team_role[0].gamename));
		memset(protocol.recently_team_role[count].family_name, 0, sizeof(protocol.recently_team_role[0].family_name));
		protocol.recently_team_role[count].user_id = _info->info_arr[i].user_id;
		protocol.recently_team_role[count].level = _info->info_arr[i].level;
		protocol.recently_team_role[count].vip_level = 0;
		protocol.recently_team_role[count].top_level = _info->info_arr[i].top_level;
		protocol.recently_team_role[count].last_online_timestamp = _info->info_arr[i].last_online_timestamp;

		++count;
	}

	size_t length = sizeof(protocol) - ((MAX_RECENTLY_TEAM_NUM - protocol.recently_team_role_count) * sizeof(protocol.recently_team_role[0]));
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&protocol, length);
}

void FriendManager::SendRoleOnlineStatus(Role * _role, int _target_id)
{
	if (NULL == _role)
		return;

	Protocol::SCReplyRoleIsOnline rrio;
	rrio.role_id = _target_id;
	rrio.online_status = this->GetOnlineStatus(_role->GetUID(), _target_id,
		CrossConfig::Instance().IsHiddenServer());

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&rrio, sizeof(rrio));
}

void FriendManager::EditGroupByHidden(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == _role || NULL == _role_id)
		return;

	for (int i = 0; i < _count && i < MAX_FRIEND_NUM; ++i)
	{
		long long friend_for_index = TranslateInt64(_role->GetUID(), _role_id[i]);

		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it)
		{
			continue;
		}

		it->second.group_type = _group_type;

		this->SendFriendChangeToRoleByHidden(_role, _role_id[i], Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, _group_type);
	}
}

void FriendManager::EditGroupByGameWorld(Role * _role, int _group_type, GameName _group_name, int _count, int * _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == _role || NULL == _role_id)
		return;

	crossgameprotocal::GameHiddenFriendGroupReq req;

	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_EDIT_GROUP;
	req.group_type = _group_type;
	memcpy(req.group_name, _group_name, sizeof(GameName));
	req.count = 0;
	for (int i = 0; i < _count && i < MAX_FRIEND_NUM; ++i)
	{
		req.friend_id_arr[req.count++] = _role_id[i];
	}

	int length = sizeof(req) - (MAX_FRIEND_NUM - req.count) * sizeof(req.friend_id_arr[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, length);
}

void FriendManager::EditGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _req->count && i < MAX_FRIEND_NUM; ++i)
	{
		long long friend_for_index = TranslateInt64(_req->role_id, _req->friend_id_arr[i]);

		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it)
		{
			continue;
		}

		it->second.group_type = _req->group_type;
		this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA);
		this->SendFriendChangeToRoleToGameWorld(_req->role_id, _req->friend_id_arr[i], Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, _req->group_type);
	}
}

void FriendManager::MoveGroupByHidden(Role * _role, int _group_type, int _count, int * _role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == _role || NULL == _role_id)
		return;

	for (int i = 0; i < _count && i < MAX_FRIEND_NUM; ++i)
	{
		long long friend_for_index = TranslateInt64(_role->GetUID(), _role_id[i]);

		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it)
		{
			continue;
		}

		it->second.group_type = _group_type;
		this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA);
		this->SendFriendChangeToRoleByHidden(_role, it->second.friend_uid, Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, _group_type);
	}
}

void FriendManager::MoveGroupByGameWorld(Role * _role, int _group_type, int _count, int * _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == _role || NULL == _role_id)
		return;

	crossgameprotocal::GameHiddenFriendGroupReq req;

	req.role_id = _role->GetUID();
	req.req_type = crossgameprotocal::GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MOVE_GROUP;
	req.group_type = _group_type;
	memset(req.group_name, 0, sizeof(GameName));
	req.count = 0;
	for (int i = 0; i < _count && i < MAX_FRIEND_NUM; ++i)
	{
		req.friend_id_arr[req.count++] = _role_id[i];
	}

	int length = sizeof(req) - (MAX_FRIEND_NUM - req.count) * sizeof(req.friend_id_arr[0]);
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, length);
}

void FriendManager::MoveGroupFromGameWorld(crossgameprotocal::GameHiddenFriendGroupReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _req->count && i < MAX_FRIEND_NUM; ++i)
	{
		long long friend_for_index = TranslateInt64(_req->role_id, _req->friend_id_arr[i]);

		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() == it)
		{
			continue;
		}

		it->second.group_type = _req->group_type;
		this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_UPDATA);
		this->SendFriendChangeToRoleToGameWorld(_req->role_id, _req->friend_id_arr[i], Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE, _req->group_type);
	}
}

void FriendManager::AskListChangeByInsertByHidden(int _target_id, Role * _role, bool _is_yijian, bool _is_send_friend_route)
{
	crossgameprotocal::HiddenGameFriendAskListChange change;
	change.target_id = _target_id;
	change.role_id = _role->GetUID();
	change.change_type = crossgameprotocal::HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INSERT;
	change.is_yijian = _is_yijian;
	change.is_send_friend_route = _is_send_friend_route;

	_role->GetName(change.role_name);
	change.avatar_type = _role->GetAvatarType();
	change.prof = _role->GetProfession();
	change.level = _role->GetLevel();
	change.sex = _role->GetSex();
	change.headshot_id = _role->GetHeadshotID();
	change.top_level = _role->GetTopLevel();
	change.avatar_timestamp = 0;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_target_id).db_index, (char *)&change, sizeof(change));
}

void FriendManager::AskListChangeByInsertByHidden(crossgameprotocal::GameHiddenAddFriendReq * _req, bool _is_send_friend_rout)
{
	crossgameprotocal::HiddenGameFriendAskListChange change;
	change.target_id = _req->target_id;
	change.role_id = _req->role_id;
	change.change_type = crossgameprotocal::HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INSERT;
	change.is_yijian = _req->is_yijian;
	change.is_send_friend_route = _is_send_friend_rout;

	F_STRNCPY(change.role_name, _req->role_name, sizeof(GameName));
	change.avatar_type = _req->avatar_type;
	change.prof = _req->prof;
	change.level = _req->level;
	change.sex = _req->sex;
	change.headshot_id = _req->headshot_id;
	change.top_level = _req->top_level;
	change.avatar_timestamp = _req->avatar_timestamp;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_req->target_id).db_index, (char *)&change, sizeof(change));

}

void FriendManager::AskListChangeByDeleteByHidden(int _target_id, int _role_id)
{
	crossgameprotocal::HiddenGameFriendAskListChange change;
	change.target_id = _target_id;
	change.role_id = _role_id;
	change.change_type = crossgameprotocal::HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_DELETE;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_target_id).db_index, (char *)&change, sizeof(change));
}

void FriendManager::AddFriendReqRetToGameWorld(int _role_id, int _ret)
{
	crossgameprotocal::HiddenGameSyncRoleReqRet ret;
	ret.role_id = _role_id;
	ret.error_num = _ret;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (char *)&ret, sizeof(ret));
}

void FriendManager::AddFriendReply(Role * _role, int _apply_id, bool _is_accept)
{
	if (NULL == _role)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddFriendReplyByHidden(_role->GetUID(), _apply_id, _is_accept);
	}
	else
	{
		this->AddFriendReplyByGameWorld(_role, _apply_id, _is_accept);
	}
}

void FriendManager::AddFriendReplyByGameWorld(Role * _role, int _apply_id, bool _is_accept)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenAddFriendReplyReq req;

	req.role_id = _role->GetUID();
	req.apply_id = _apply_id;
	req.is_accept = _is_accept;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&req, sizeof(req));
}

void FriendManager::AddFriendReplyByHidden(int _role_id, int _apply_id, bool _is_accept)
{
	if (_role_id == _apply_id)
		return;

	//1. 先检查一下待审核列表里是否有该玩家的申请
	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() == it)
		return;

	std::map<int, Protocol::AskAddInfo>::iterator it_ask = it->second.ask_map.find(_apply_id);
	if (it->second.ask_map.end() == it_ask)
		return;

	if (_is_accept)
	{
		this->AddFriendByHidden(_role_id, _apply_id);
	}
	else
	{
		// 拒绝了好友申请
		Role * apply_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_apply_id));
		if (NULL != apply_role)
		{
			apply_role->NoticeNum(errornum::EN_FRIEND_OTHER_REJECT_REQ);
		}
		else
		{
			this->AddFriendReqRetToGameWorld(_apply_id, errornum::EN_FRIEND_OTHER_REJECT_REQ);
		}
	}

	// 无论是添加好友还是拒绝添加完好友 , 无论成功失败都需要删除待审核的申请信息
	this->RemoveTargetAskList(_role_id, _apply_id, true);
}

void FriendManager::AddFriendByHidden(int _role_id, int _apply_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	int role_error_num = 0, target_error_num = 0;
	Role * role = NULL, *apply_role = NULL;
	if (!this->CanAddFriend(_role_id, _apply_id, &role_error_num, &target_error_num))
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_id));
		if (NULL != role)
		{
			role->NoticeNum(role_error_num);
		}
		else
		{
			this->AddFriendReqRetToGameWorld(_role_id, role_error_num);
		}

		apply_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_apply_id));
		if (NULL != apply_role)
		{
			apply_role->NoticeNum(target_error_num);
		}
		else
		{
			this->AddFriendReqRetToGameWorld(_apply_id, target_error_num);
		}
		return;
	}

	// 添加好友
	this->AddFriendByUserCacheNodeByHidden(_role_id, _apply_id);
	// 双向添加好友
	this->AddFriendByUserCacheNodeByHidden(_apply_id, _role_id);

	if (NULL != apply_role)
	{
		long long friend_for_index = TranslateInt64(_apply_id, _role_id);
		FriendItemMapIt it = m_friend_item_map.find(friend_for_index);
		if (m_friend_item_map.end() != it)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				gamestring::g_add_friend_succ_content, it->second.role_name);
			if (length > 0)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(apply_role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
	}
}

void FriendManager::AddFriendByUserCacheNodeByHidden(int _role_id, int _apply_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	FriendItem friend_item;
	friend_item.role_id = _role_id;
	friend_item.friend_uid = _apply_id;
	friend_item.intimacy = 0;
	friend_item.battle_add_intimacy = 0;
	friend_item.group_type = 0;

	// 用申请人的缓存数据添加
	this->UpdateFriendInfo(_apply_id, friend_item);

	long long friend_for_index = this->AddFriendInfo(_role_id, _apply_id, friend_item);
	this->AddUserFriendRelation(_role_id, _apply_id);

	UserFrinedInfoMapIt it = m_user_friend_info_map.find(_role_id);
	if (m_user_friend_info_map.end() != it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_id));
		if (NULL != role)
		{
			EventHandler::Instance().OnAddFriend(role, it->second.user_friend_set.size());
			this->SendFriendChangeToRoleByHidden(role, _apply_id, Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE);
		}
		else
		{
			// 如果不在线 , 则玩家上线时  再对EventHandler::Instance().OnAddFriend判断一次
			this->SendFriendChangeToRoleToGameWorld(_role_id, _apply_id, Protocol::SCChangeFriend::CHANGE_TYPE_UPDATE);
		}
	}

	// 添加好友也需要添加亲密的关系
	FriendIntimacyManager::Instance().OnAddFriend(_role_id, _apply_id);
	// 同步数据到其他服务器
	this->SyncFriendDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_TO_CROSS_STATUS_INSERT);


	gamelog::g_log_social.printf(LL_INFO, "%s role_id:%d  target_id:%d ",
		__FUNCTION__, _role_id, _apply_id);
}
