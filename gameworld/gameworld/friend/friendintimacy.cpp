#include "friendintimacy.hpp"

#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "rmiclient/rmiglobal.hpp"
#include "internalcomm.h"
#include "serverlogic.h"
#include "gamelog.h"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/friendgiftconfig.hpp"
#include "friend/friendmanager.h"
#include "protocol/msgfriend.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "friend/friendmanager.h"

#include "global/usercache/usercache.hpp"
#include "global/cross/crossmanager.h"

#include "global/datasynccheck/datasynccheck.hpp"
#include "other/event/eventhandler.hpp"


void RMIInitFriendIntimacyBackObjectImpl::InitFriendIntimacyRet(int _ret, FriendIntimacyListParam & _param)
{
	FriendIntimacyManager::Instance().InitFriendIntimacyParam(_ret, _param);
}

FriendIntimacyManager & FriendIntimacyManager::Instance()
{
	static FriendIntimacyManager st_fim;
	return st_fim;
}

void FriendIntimacyManager::InitFriendIntimacyParam(int _ret, FriendIntimacyListParam & _param)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (0 != _ret)
	{
		printf(" FriendIntimacyManager::InitFriendIntimacyParam Failed . ret:%d  \n", _ret);
		ServerLogic::GetInstServerLogic()->StopServer("FriendIntimacyManager::InitFriendIntimacyParam");
		EngineAdapter::Instance().StopGame();
		return;
	}
	else
	{
		for (int i = 0; i < _param.count && i < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++i)
		{
			int role_id = _param.intimacy_info_arr[i].role_id;
			int friend_id = _param.intimacy_info_arr[i].friend_id;
			long long friend_for_index = this->AddFriendInfo(role_id, friend_id, _param.intimacy_info_arr[i]);

			m_old_state.insert(friend_for_index);// 标记该数据是已经存在的数据
		}

		if (_param.count >= MAX_ONCE_FRIENDS_INTIMACY_COUNT)
		{
			RMIInitFriendIntimacyBackObjectImpl *impl = new RMIInitFriendIntimacyBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitFriendIntimacyDataAsyn(_param.id_from , impl);
		}
		else
		{
			printf("Init FriendIntimacyManager::InitFriendIntimacyParam Succ,  size:%d  \n", (int)m_friend_intimacy_map.size());
			m_load_flag = true;
		}
	}

	this->CheckLoadFinish();
}

void FriendIntimacyManager::OnConnectHiddenSucc()
{
	if (!this->IsLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SyncAllDataToHidden();
}

void FriendIntimacyManager::OnServerStart()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	{
		// 初始化好友信息
		RMIInitFriendIntimacyBackObjectImpl *impl = new RMIInitFriendIntimacyBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitFriendIntimacyDataAsyn(0, impl);
	}
}

void FriendIntimacyManager::OnServerStop()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	this->Save();
}

bool FriendIntimacyManager::IsLoadFinish() const
{
	if (CrossConfig::Instance().IsHiddenServer())
		return true;

	return (FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH == m_data_state);
}

void FriendIntimacyManager::Update(unsigned long interval, time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	static time_t last_save_time = now_second;

	if (now_second >= last_save_time + FRIEND_INTIMACY_SAVE_INTERVAL)
	{
		this->Save();
		last_save_time = now_second;
	}
}

void FriendIntimacyManager::OnLogin(Role * _role)
{
	if (NULL == _role)
		return;

	this->SendList(_role);
}

void FriendIntimacyManager::OnSyncFriendIntimacyInfo(crossgameprotocal::SyncFriendIntimacyInfo * _info)
{
	if (NULL == _info || 0 > _info->count || MAX_ONCE_FRIENDS_INTIMACY_COUNT < _info->count)
	{
		return;
	}

	if (_info->is_init)
	{
		// 初始化数据通过只可能是 原服 -> 隐藏服
		this->InitFriendIntimacyInfoFromGameWorld(_info);

		//验证用回执
		crossgameprotocal::CrossGameDataSyncCheckAck ack;
		ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY;
		InternalComm::Instance().SendToGameThroughCross(_info->from_usid.plat_type, _info->from_usid.server_id, (const char*)&ack, sizeof(ack));
	}
	else
	{
		// 更新数据只可能是隐藏服 -> 原服
		this->UpdataFriendIntimacyInfoFromHidden(_info);
	}
}

void FriendIntimacyManager::OnAddIntimacyReqFromGameWorld(crossgameprotocal::GameHiddenAddIntimacyReq * _req)
{
	if (NULL == _req ||!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->AddIntimacyByHidden(_req->role_uid, _req->friend_uid, _req->add_val, __FUNCTION__);
}

void FriendIntimacyManager::OnIntimacyRelationReqFromGameWorld(crossgameprotocal::GameHiddenIntimacyRelationReq * _req)
{
	if (NULL == _req || !CrossConfig::Instance().IsHiddenServer())
		return;

	if (_req->is_add)
	{
		this->OnAddFriendByHidden(_req->role_uid, _req->friend_uid);
	}
	else
	{
		this->OnRemoveFriendByHidden(_req->role_uid, _req->friend_uid);
	}
}

void FriendIntimacyManager::OnSetIntimacySingleChatTimesFromGameWorld(crossgameprotocal::GameHiddenIntimacySingleChatTimes * _times)
{
	if (NULL == _times || !CrossConfig::Instance().IsHiddenServer())
		return;

	if (_times->is_to)
	{
		this->SetLastDayFristToSingleChatTimesByHidden(_times->role_uid, _times->friend_uid, _times->timestamp, __FUNCTION__);
	}
	else
	{
		this->SetLastDayFristFromSingleChatTimesByHidden(_times->role_uid, _times->friend_uid, _times->timestamp, __FUNCTION__);
	}
}

void FriendIntimacyManager::OnGiveFlowerNoticeFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerNotice * _notice)
{
	if (NULL == _notice || !CrossConfig::Instance().IsHiddenServer())
		return;

	this->GiveFlowerNotifyByHidden(_notice->role_uid, _notice->friend_uid, _notice->item_id, _notice->type);
}

void FriendIntimacyManager::OnGiveFlowerNoticeFromHidden(crossgameprotocal::HiddenGameGiveFlowerNotice * _notice)
{
	if (NULL == _notice || CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCGiveFlowerNotice notice;

	F_STRNCPY(notice.giver_name, _notice->role_name, sizeof(GameName));
	F_STRNCPY(notice.receiver_name, _notice->friend_name, sizeof(GameName));
	notice.give_type = _notice->type;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_notice->role_id));
	if (NULL != role)
	{
		Scene * scene = role->GetScene();
		if (NULL != scene)
		{
			scene->SendToRole((const char*)&notice, sizeof(notice));
		}
	}

	Role * friend_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_notice->friend_id));
	if (NULL != friend_role)
	{
		Scene * scene = friend_role->GetScene();
		if (NULL != scene)
		{
			scene->SendToRole((const char*)&notice, sizeof(notice));
		}
	}

	// 传闻 
	int len = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_give_flower_notice,
		_notice->role_name, _notice->item_id, _notice->friend_name);

	if (len > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, len, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}

}

void FriendIntimacyManager::OnGiveFlowerBackKissFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerBackKiss * _back_kiss)
{
	if (NULL == _back_kiss || CrossConfig::Instance().IsHiddenServer())
		return;

	this->OnBackToKissByHidden(_back_kiss->role_uid, _back_kiss->role_name, _back_kiss->to_role_id);
}

void FriendIntimacyManager::OnGiveFlowerBackKissFromHidden(crossgameprotocal::HiddenGameGiveFlowerBackKiss * _back_kiss)
{
	if (NULL == _back_kiss || CrossConfig::Instance().IsHiddenServer())
		return;

	// 消息来源于隐藏服 说明隐藏服找不到人了 , 所以不需要再回传给隐藏服
	Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_back_kiss->to_role_id));
	if (NULL != to_role)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_give_flower_back_kiss, _back_kiss->role_name);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
	}
}

int FriendIntimacyManager::GetFriendIntimacy(int _role_uid, int _friend_uid)
{
	long long friend_for_index = TranslateInt64(_role_uid, _friend_uid);

	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() != it)
	{
		return it->second.intimacy_val;
	}

	return 0;
}

void FriendIntimacyManager::AddIntimacy(int _role_uid, int _friend_uid, int _add_val, const  char * _reason)
{
	// 为了数据的一致性 , 所有修改都在隐藏服完成 , 然后同步会主服
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddIntimacyByHidden(_role_uid, _friend_uid, _add_val, _reason);
	}
	else
	{
		this->AddIntimacyByGameWorld(_role_uid, _friend_uid, _add_val, _reason);
	}
}

void FriendIntimacyManager::ClearIntimacy(int _role_uid, int _friend_uid, const char * _reason)
{
	int intimacy = this->GetFriendIntimacy(_role_uid, _friend_uid);
	if (intimacy > 0)
	{
		this->AddIntimacy(_role_uid, _friend_uid, -intimacy, _reason);
	}	
}

unsigned int FriendIntimacyManager::GetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_uid, _friend_id);

	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() != it)
	{
		return it->second.day_frist_to_singchat_timestamp;
	}

	return 0;
}

unsigned int FriendIntimacyManager::GetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_uid, _friend_id);

	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() != it)
	{
		return it->second.day_frist_from_singchat_timestamp;
	}

	return 0;
}

void FriendIntimacyManager::SetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason)
{
	// 应该由外部验证来保证有效性
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SetLastDayFristToSingleChatTimesByHidden(_role_uid, _friend_id, _to_times, _reason);
	}
	else
	{
		this->SetLastDayFristToSingleChatTimesByGameWorld(_role_uid, _friend_id, _to_times, _reason);
	}
}

void FriendIntimacyManager::SetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id, unsigned int _from_times, const  char * _reason)
{
	// 应该由外部验证来保证有效性
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SetLastDayFristFromSingleChatTimesByHidden(_role_uid, _friend_id, _from_times, _reason);
	}
	else
	{
		this->SetLastDayFristFromSingleChatTimesByGameWorld(_role_uid, _friend_id, _from_times, _reason);
	}
}

void FriendIntimacyManager::OnAddFriend(int _role_id, int _friend_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnAddFriendByHidden(_role_id, _friend_id);
	}
	else
	{
		this->OnIntimacyRelationReqToHidden(_role_id, _friend_id, true);
	}
}

void FriendIntimacyManager::OnRemoveFriend(int _role_id, int _friend_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnRemoveFriendByHidden(_role_id, _friend_id);
	}
	else
	{
		this->OnIntimacyRelationReqToHidden(_role_id, _friend_id, false);
	}
}

void FriendIntimacyManager::OnSingleChat(int _role_id, int _friend_id, unsigned int now_times , const char * _reason)
{
	// 再验证一下吧 ..
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() == it)
		return;


	unsigned int last_to_singlechat_times = FriendIntimacyManager::Instance().GetLastDayFristToSingleChatTimes(_role_id, _friend_id);
	int day_index = GetDayIndex(GetZeroTime(now_times), last_to_singlechat_times);
	if (0 != day_index)
	{
		// 然后设置双方的聊天时间
		FriendIntimacyManager::Instance().SetLastDayFristToSingleChatTimes(_role_id, _friend_id, now_times, _reason);
		FriendIntimacyManager::Instance().SetLastDayFristFromSingleChatTimes(_friend_id, _role_id, now_times, _reason);

		// 非0表示 当前距离上一次私聊不是同一天 , 需要加亲密度
		int add_val = LOGIC_CONFIG->GetFriendGiftConfig()->GetOtherCfg().first_talk_add;
		FriendIntimacyManager::Instance().AddIntimacy(_role_id, _friend_id, add_val, _reason);

		gamelog::g_log_give_gift.printf(LL_INFO, "FriendIntimacyManager::OnSingleChat  user[%d] to_role_id:%d  now_times:%u , reason:%s ",
			_role_id, _friend_id, now_times , NULL == _reason ? "" : _reason);
	}
}

void FriendIntimacyManager::OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg)
{
	if (NULL == _msg)
		return;

	if (!this->IsCanAddInitimacyBattleType(_msg->battle_mode))
		return;

	const FriendGiftOtherCfg & other_cfg = LOGIC_CONFIG->GetFriendGiftConfig()->GetOtherCfg();

	// 先检查攻击方 
	for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < _msg->attacker_list.role_count; ++i)
	{
		for (int j = i + 1; j < SIDE_MAX_ROLE_NUM && j < _msg->attacker_list.role_count; ++j)
		{
			this->AddIntimacy(_msg->attacker_list.role_list[i].uid, _msg->attacker_list.role_list[j].uid, other_cfg.hook_add, __FUNCTION__);
		}
	}

	// 再检查防守方
	for (int i = 0; i < SIDE_MAX_ROLE_NUM && i < _msg->defender_list.role_count; ++i)
	{
		for (int j = i + 1; j < SIDE_MAX_ROLE_NUM && j < _msg->defender_list.role_count; ++j)
		{
			this->AddIntimacy(_msg->defender_list.role_list[i].uid, _msg->defender_list.role_list[j].uid, other_cfg.hook_add, __FUNCTION__);
		}
	}
}

void FriendIntimacyManager::GiveFlowerNotify(int _role_id, int _friend_id, int _item_id)
{
	//先检查是否需要广播
	const FriendGiftSendFlowerCfg * flower_cfg = LOGIC_CONFIG->GetFriendGiftConfig()->GetSendFlowerCfg(_item_id);
	if (NULL == flower_cfg)
	{
		return;
	}

	if (SEND_FLOWER_TYPE_HIGH != flower_cfg->type)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GiveFlowerNotifyByHidden(_role_id, _friend_id, _item_id, SEND_FLOWER_TYPE_HIGH);
	}
	else
	{
		this->GiveFlowerNotifyByGameWorld(_role_id, _friend_id, _item_id, SEND_FLOWER_TYPE_HIGH);
	}
}

void FriendIntimacyManager::OnBackToKiss(Role * _role, int _to_role_id)
{
	if (NULL == _role)
		return;



}

FriendIntimacyManager::FriendIntimacyManager()
	:m_data_state(FRIEND_INTIMACY_DATA_STATUS_INVALID), m_load_flag(false),m_change_flag(false)
{

}

FriendIntimacyManager::~FriendIntimacyManager()
{

}

void FriendIntimacyManager::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SaveParam();
}

void FriendIntimacyManager::SaveParam()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!m_change_flag)
	{
		return;
	}

	static FriendIntimacyListParam param;
	param.count = 0;
	int count = 0;

	std::set < long long >::iterator it_dirty_begin = m_dirty_mark.begin();
	std::set < long long >::iterator it_dirty_end = m_dirty_mark.end();
	for (; it_dirty_begin != it_dirty_end && count < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++it_dirty_begin)
	{
		long long  friend_for_index = *it_dirty_begin;
		FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
		if (m_friend_intimacy_map.end() == it)
		{
			std::set <  long long  > ::iterator it_old = m_old_state.find(friend_for_index);
			if (m_old_state.end() == it_old)
			{
				// 既然之前不存在于数据库 ,那就直接丢弃这份数据好了 没必要去数据库里计算
				continue;
			}
			else
			{
				int role_id = friend_for_index >> 32;
				int friend_id = (friend_for_index << 32) >> 32;

				// 这个ID之前存在于数据里内 , 那么去数据库里删除一下
				param.intimacy_info_arr[count].change_state = structcommon::CS_DELETE;
				param.intimacy_info_arr[count].role_id = role_id;
				param.intimacy_info_arr[count].friend_id = friend_id;
			}
		}
		else
		{
			param.intimacy_info_arr[count] = it->second;

			std::set <  long long  > ::iterator it_old = m_old_state.find(friend_for_index);
			if (m_old_state.end() == it_old)
			{
				param.intimacy_info_arr[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				param.intimacy_info_arr[count].change_state = structcommon::CS_UPDATE;
			}
		}
		++count;
	}

	param.count = count;

	if (0 != param.count)
	{
		RMISaveFriendIntimacyBackObjectImpl *backobj = new RMISaveFriendIntimacyBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveFriendIntimacyDataAsyn(param, backobj);
		if (ret)
		{
			this->ClearDirtyMark();
		}
	}
}

void FriendIntimacyManager::ClearDirtyMark()
{
	if (!m_change_flag)
		return;

	m_dirty_mark.clear();
	m_old_state.clear();

	FriendIntimacyMapIt it_begin = m_friend_intimacy_map.begin();
	FriendIntimacyMapIt it_end = m_friend_intimacy_map.end();
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state.insert(it_begin->first);
	}

	m_change_flag = false;
}

void FriendIntimacyManager::SetDirtyMark(long long _friend_for_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if ((int)m_dirty_mark.size() >= MAX_ONCE_FRIENDS_INTIMACY_COUNT)
	{
		this->SaveParam();
	}

	m_change_flag = true;
	m_dirty_mark.insert(_friend_for_index);
}

void FriendIntimacyManager::CheckLoadFinish()
{
	if (m_load_flag)
		m_data_state = FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH;

	if (FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH == m_data_state)
	{
		// 如果有需要加载完成时进行处理的在这里处理
	}
}

void FriendIntimacyManager::SyncAllDataToHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::SyncFriendIntimacyInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = true;
	sync_info.is_init_add = 0;

	FriendIntimacyMapIt it_begin = m_friend_intimacy_map.begin();
	FriendIntimacyMapIt it_end = m_friend_intimacy_map.end();

	int count = 0;
	for (; it_begin != it_end && count < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++it_begin)
	{
		sync_info.item_list[count++].friend_item = it_begin->second;
		if (count >= MAX_ONCE_FRIENDS_INTIMACY_COUNT)
		{
			sync_info.count = count;
			int send_length = sizeof(crossgameprotocal::SyncFriendIntimacyInfo) -
				((MAX_ONCE_FRIENDS_INTIMACY_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
			DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY);

			count = 0;
			sync_info.count = 0;
			sync_info.is_init_add = 1;
		}
	}

	if (count <= 0)
		return;

	sync_info.count = count;
	int send_length = sizeof(crossgameprotocal::SyncFriendIntimacyInfo) -
		((MAX_ONCE_FRIENDS_INTIMACY_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&sync_info, send_length);
	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY);
}

void FriendIntimacyManager::SyncFriendIntimacyDataToGameWorld(long long friend_for_index, int _sync_status)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	// 同步好友亲密度数据 隐藏服->原服 
	static crossgameprotocal::SyncFriendIntimacyInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.is_init = false;
	sync_info.is_init_add = 0;

	int role_id = (int)(friend_for_index >> 32);
	int friend_id = (int)((friend_for_index << 32) >> 32);
	sync_info.item_list[0].friend_item.role_id = role_id;
	sync_info.item_list[0].friend_item.friend_id = friend_id;
	sync_info.item_list[0].sync_type = _sync_status;

	if (crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA == _sync_status ||
		crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INSERT == _sync_status)
	{
		FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
		if (m_friend_intimacy_map.end() == it)
			return;

		sync_info.item_list[0].friend_item = it->second;
	}

	int send_length = sizeof(crossgameprotocal::SyncFriendIntimacyInfo) -
		((MAX_ONCE_FRIENDS_INTIMACY_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&sync_info, send_length);
}

void FriendIntimacyManager::InitFriendIntimacyInfoFromGameWorld(crossgameprotocal::SyncFriendIntimacyInfo * _info)
{
	if (NULL == _info || !CrossConfig::Instance().IsHiddenServer())
		return;

	// 初始化就无脑添加就是了
	for (int i = 0; i < _info->count && i < MAX_ONCE_FRIENDS_INTIMACY_COUNT; ++i)
	{
		int role_id = _info->item_list[i].friend_item.role_id;
		int friend_id = _info->item_list[i].friend_item.friend_id;

		this->AddFriendInfo(role_id, friend_id, _info->item_list[i].friend_item);
	}
}

void FriendIntimacyManager::UpdataFriendIntimacyInfoFromHidden(crossgameprotocal::SyncFriendIntimacyInfo * _info)
{
	if (NULL == _info || CrossConfig::Instance().IsHiddenServer())
		return;

	for (int i = 0; i < _info->count; ++i)
	{
		int role_id = _info->item_list[i].friend_item.role_id;
		int friend_id = _info->item_list[i].friend_item.friend_id;

		switch (_info->item_list[i].sync_type)
		{
		case crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INSERT:
			{
				long long friend_for_index = this->AddFriendInfo(role_id, friend_id, _info->item_list[i].friend_item);
				this->SetDirtyMark(friend_for_index);
			}
			break;
		case crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA:
			{
				long long friend_for_index = TranslateInt64(role_id, friend_id);
				m_friend_intimacy_map[friend_for_index] = _info->item_list[i].friend_item;
				this->SetDirtyMark(friend_for_index);
			}
			break;
		case crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_DELETE:
			{
				long long friend_for_index = this->RemoveFriendInfo(role_id, friend_id);
				this->SetDirtyMark(friend_for_index);
			}
			break;
		default:
			continue;
		}

		if (crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA == _info->item_list[i].sync_type)
		{
			// 添加好友关系时,必然是空数据, 也不需要告知变更
			// 删除时必然是删除好友了 , 删除好友不需要额外通知亲密度变更
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
			if (NULL != role)
			{
				this->SendChangeInfo(role, friend_id);
				EventHandler::Instance().OnFriendIntimacyChange(role);
			}
		}

		// 记录一下变更 , 方便查问题
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::UpdataFriendIntimacyInfoFromHidden  role_id:%d  friend_id:%d  sync_type:%d ",
			role_id, friend_id, _info->item_list[i].sync_type);
	}
}

long long FriendIntimacyManager::AddFriendInfo(int _role_id, int _friend_id, const FriendIntimacyInfo & _item)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	m_friend_intimacy_map[friend_for_index] = _item;

	return friend_for_index;
}

long long FriendIntimacyManager::RemoveFriendInfo(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	m_friend_intimacy_map.erase(friend_for_index);

	return friend_for_index;
}

void FriendIntimacyManager::AddIntimacyByHidden(int _role_uid, int _friend_uid, int _add_val, const char * _reason)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	long long role_for_index = TranslateInt64(_role_uid, _friend_uid);
	long long friend_for_index = TranslateInt64(_friend_uid, _role_uid);

	// 查不到说明双方不是好友
	FriendIntimacyMapIt it_role = m_friend_intimacy_map.find(role_for_index);
	if (m_friend_intimacy_map.end() == it_role)
	{
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::AddIntimacyByHidden[Error_1] no find role_for_index . role_id:%d  friend_id:%d  role_for_index:%lld  add_val:%d  reason:%s",
			_role_uid, _friend_uid, role_for_index, _add_val, NULL == _reason ? "" : _reason);
		return;
	}

	FriendIntimacyMapIt it_friend = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() == it_friend)
	{
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::AddIntimacyByHidden[Error_2] no find friend_for_index . role_id:%d  friend_id:%d  friend_for_index:%lld  add_val:%d  reason:%s",
			_role_uid, _friend_uid, friend_for_index, _add_val, NULL == _reason ? "" : _reason);
		return;
	}

	const FriendGiftOtherCfg & other_cfg = LOGIC_CONFIG->GetFriendGiftConfig()->GetOtherCfg();

	// 加亲密度需要双方增加
	if (it_role->second.intimacy_val + _add_val > other_cfg.intimacy_max)
	{
		it_role->second.intimacy_val = other_cfg.intimacy_max;
	}
	else
	{
		it_role->second.intimacy_val += _add_val;
	}
	// A B的亲密是一样的  所以直接A = B;
	it_friend->second.intimacy_val = it_role->second.intimacy_val;

	// 数据同步回主服
	this->SyncFriendIntimacyDataToGameWorld(role_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA);
	this->SyncFriendIntimacyDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA);

	// 变更需要通知客户端 , 不在线或者不在隐藏服的 , 会在数据同步到主服后 由主服再通知客户端
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_uid));
	if (NULL != role)
	{
		this->SendChangeInfo(role , _friend_uid);
		EventHandler::Instance().OnFriendIntimacyChange(role);
	}

	Role * friend_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_friend_uid));
	if (NULL != friend_role)
	{
		this->SendChangeInfo(friend_role, _role_uid);
		EventHandler::Instance().OnFriendIntimacyChange(friend_role);
	}

	gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::AddIntimacyByHidden  role_id:%d  friend_id:%d  add_intimacy:%d  now_role_intimacy:%d  now_friend_intimacy:%d  reason:%s",
		_role_uid, _friend_uid, _add_val, it_role->second.intimacy_val , it_friend->second.intimacy_val , NULL == _reason ? "" : _reason);
}

void FriendIntimacyManager::AddIntimacyByGameWorld(int _role_uid, int _friend_uid, int _add_val, const  char * _reason)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::AddIntimacyByGameWorld[ERROR_1] cross_not_connected . role_id:%d  friend_id:%d  add_intimacy:%d  reason:%s",
			_role_uid, _friend_uid, _add_val,  NULL == _reason ? "" : _reason);
		return ;
	}

	crossgameprotocal::GameHiddenAddIntimacyReq req;
	req.role_uid = _role_uid;
	req.friend_uid = _friend_uid;
	req.add_val = _add_val;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&req, sizeof(req));
}

void FriendIntimacyManager::OnAddFriendByHidden(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);

	static FriendIntimacyInfo info;
	info.Reset();

	info.role_id = _role_id;
	info.friend_id = _friend_id;
	this->AddFriendInfo(_role_id, _friend_id , info);
	this->SyncFriendIntimacyDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INSERT);
}

void FriendIntimacyManager::OnRemoveFriendByHidden(int _role_id, int _friend_id)
{
	long long friend_for_index = TranslateInt64(_role_id, _friend_id);
	this->RemoveFriendInfo(_role_id, _friend_id);
	this->SyncFriendIntimacyDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_DELETE);
}

void FriendIntimacyManager::OnIntimacyRelationReqToHidden(int _role_id, int _friend_id, bool _is_add)
{
	crossgameprotocal::GameHiddenIntimacyRelationReq req;

	req.role_uid = _role_id;
	req.friend_uid = _friend_id;
	req.is_add = _is_add;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
}

void FriendIntimacyManager::SendList(Role * _role)
{
	if (NULL == _role)
		return;

	Protocol::SCFriendIntimacyListInfo list;
	list.count = 0;

	FriendManager::UserFrinedSet * friend_set = FriendManager::Instance().GetUserFriendSetInfo(_role);
	if (NULL != friend_set)
	{
		for (FriendManager::UserFrinedSet::const_iterator it = friend_set->begin(); it != friend_set->end() && list.count < MAX_FRIEND_NUM; it++)
		{
			long long friend_for_index = *it;
			FriendIntimacyMapIt it_friend = m_friend_intimacy_map.find(friend_for_index);
			if (m_friend_intimacy_map.end() == it_friend)
			{
				continue;
			}

			list.info[list.count].friend_id = it_friend->second.friend_id;
			list.info[list.count].intimacy_val = it_friend->second.intimacy_val;
			list.info[list.count].day_frist_to_singchat_timestamp = it_friend->second.day_frist_to_singchat_timestamp;
			list.info[list.count].day_frist_from_singchat_timestamp = it_friend->second.day_frist_from_singchat_timestamp;

			++list.count;
		}
	}

	int length = sizeof(list) - (MAX_FRIEND_NUM - list.count) * sizeof(list.info[0]);
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&list, length);
}

void FriendIntimacyManager::SendChangeInfo(Role * _role, int friend_id)
{
	if (NULL == _role)
		return;

	long long friend_for_index = TranslateInt64(_role->GetUID(), friend_id);

	FriendIntimacyMapIt it_friend = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() == it_friend)
	{
		return;
	}

	Protocol::SCFriendIntimacyInfoChange change;
	change.info.friend_id = it_friend->second.friend_id;
	change.info.intimacy_val = it_friend->second.intimacy_val;
	change.info.day_frist_to_singchat_timestamp = it_friend->second.day_frist_to_singchat_timestamp;
	change.info.day_frist_from_singchat_timestamp = it_friend->second.day_frist_from_singchat_timestamp;

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&change, sizeof(change));
}

bool FriendIntimacyManager::IsCanAddInitimacyBattleType(int _battle_type)
{
	if (LOGIC_CONFIG->GetFriendGiftConfig()->IsExcludeFightType(_battle_type))
	{
		return false;
	}

	return true;
}

void FriendIntimacyManager::SetLastDayFristToSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _to_times, const char * _reason)
{
	long long friend_for_index = TranslateInt64(_role_uid, _friend_id);

	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() != it)
	{
		// 记录一下变更 , 方便查问题
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::SetLastDayFristToSingleChatTimes  role_id:%d  friend_id:%d  old_times:%u  _to_times:%u  reason:%s",
			_role_uid, _friend_id, it->second.day_frist_to_singchat_timestamp, _to_times, NULL == _reason ? "" : _reason);

		it->second.day_frist_to_singchat_timestamp = _to_times;
		this->SyncFriendIntimacyDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA);
	}
}

void FriendIntimacyManager::SetLastDayFristFromSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _from_times, const char * _reason)
{
	long long friend_for_index = TranslateInt64(_role_uid, _friend_id);

	FriendIntimacyMapIt it = m_friend_intimacy_map.find(friend_for_index);
	if (m_friend_intimacy_map.end() != it)
	{
		// 记录一下变更 , 方便查问题
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::SetLastDayFristFromSingleChatTimes  role_id:%d  friend_id:%d  old_times:%u   _from_times:%u  reason:%s",
			_role_uid, _friend_id, it->second.day_frist_from_singchat_timestamp, _from_times, NULL == _reason ? "" : _reason);

		it->second.day_frist_from_singchat_timestamp = _from_times;
		this->SyncFriendIntimacyDataToGameWorld(friend_for_index, crossgameprotocal::SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA);
	}
}

void FriendIntimacyManager::SetLastDayFristToSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _to_times, const char * _reason)
{
	this->SendLastDayFristSingleChatTimesToHidden(_role_uid, _friend_id, _to_times, true);

	gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::SetLastDayFristToSingleChatTimesByGameWorld  role_id:%d  friend_id:%d   _to_times:%u  reason:%s",
		_role_uid, _friend_id, _to_times, NULL == _reason ? "" : _reason);

}

void FriendIntimacyManager::SetLastDayFristFromSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _from_times, const char * _reason)
{
	this->SendLastDayFristSingleChatTimesToHidden(_role_uid, _friend_id, _from_times, false);

	gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::SetLastDayFristFromSingleChatTimesByGameWorld  role_id:%d  friend_id:%d  _from_times:%u  reason:%s",
		_role_uid, _friend_id , _from_times, NULL == _reason ? "" : _reason);

}

void FriendIntimacyManager::SendLastDayFristSingleChatTimesToHidden(int _role_uid, int _friend_id, unsigned int _times, bool _is_to)
{
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		gamelog::g_log_social.printf(LL_INFO, "FriendIntimacyManager::SendLastDayFristSingleChatTimesToHidden[ERROR_1] cross_not_connected . role_id:%d  friend_id:%d  _times:%u  _is_to:%d",
			_role_uid, _friend_id, _times, _is_to);
		return;
	}

	crossgameprotocal::GameHiddenIntimacySingleChatTimes times;

	times.role_uid = _role_uid;
	times.friend_uid = _friend_id;
	times.timestamp = _times;
	times.is_to = _is_to;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&times, sizeof(times));
}

void FriendIntimacyManager::GiveFlowerNotifyByHidden(int _role_id, int _friend_id, int _item_id, int _type)
{
	GameName role_name , friend_name;
	UserCacheNode * role_node = UserCacheManager::Instance().GetUserNode(_role_id);
	if (NULL != role_node)
	{
		role_node->GetName(role_name);
	}

	UserCacheNode * friend_node = UserCacheManager::Instance().GetUserNode(_friend_id);
	if (NULL != friend_node)
	{
		friend_node->GetName(friend_name);
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_id));
	if (NULL != role)
	{
		Scene * scene = role->GetScene();
		if (NULL != scene)
		{
			Protocol::SCGiveFlowerNotice notice;

			F_STRNCPY(notice.giver_name, role_name, sizeof(GameName));
			F_STRNCPY(notice.receiver_name, friend_name, sizeof(GameName));
			notice.give_type = _type;

			scene->SendToRole((const char*)&notice, sizeof(notice));
		}
	}

	Role * friend_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_friend_id));
	if (NULL != friend_role)
	{
		Scene * scene = friend_role->GetScene();
		if (NULL != scene)
		{
			Protocol::SCGiveFlowerNotice notice;

			F_STRNCPY(notice.giver_name, role_name, sizeof(GameName));
			F_STRNCPY(notice.receiver_name, friend_name, sizeof(GameName));
			notice.give_type = _type;
			scene->SendToRole((const char*)&notice, sizeof(notice));
		}
	}

	// 传闻 
	int len = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_give_flower_notice,
		role_name, _item_id, friend_name);

	if (len > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, len, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}

	// 同步发送到主服去
	crossgameprotocal::HiddenGameGiveFlowerNotice notice;

	notice.role_id = _role_id;
	F_STRNCPY(notice.role_name, role_name, sizeof(GameName));
	notice.friend_id = _friend_id;
	F_STRNCPY(notice.friend_name, friend_name, sizeof(GameName));
	notice.item_id = _item_id;
	notice.type = _type;

	// 检查玩家是否同服
	const UniqueServerID* my_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index));
	const UniqueServerID* friend_mother_server = World::GetInstWorld()->GetMotherServerId(UniqueServerID(LocalConfig::Instance().GetPlatType(), IntToUid(_friend_id).db_index));
	if (NULL != my_mother_server && NULL != friend_mother_server && my_mother_server->server_id != friend_mother_server->server_id)
	{
		// 发到各自的主服去
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&notice, sizeof(notice));
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_friend_id).db_index, (const char *)&notice, sizeof(notice));
	}
	else
	{
		// 发到其中一个人主服就可以了
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&notice, sizeof(notice));
	}
}

void FriendIntimacyManager::GiveFlowerNotifyByGameWorld(int _role_id, int _friend_id, int _item_id, int _type)
{
	// 主服不做处理 发到跨服去 由跨服去解决

	crossgameprotocal::GameHiddenGiveFlowerNotice notice;

	notice.role_uid = _role_id;
	notice.friend_uid = _friend_id;
	notice.item_id = _item_id;
	notice.type = _type;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&notice, sizeof(notice));
}

void FriendIntimacyManager::OnBackToKissByHidden(int _role_id, GameName _role_name, int _to_role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	bool _is_find = false;
	Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_to_role_id));
	if (NULL != to_role)
	{
		_is_find = true;

		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_give_flower_back_kiss, _role_name);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
	}

	if (!_is_find)
	{
		// 没找到则发到目标主服去
		crossgameprotocal::HiddenGameGiveFlowerBackKiss back_kiss;
		back_kiss.role_uid = _role_id;
		F_STRNCPY(back_kiss.role_name, _role_name, sizeof(GameName));
		back_kiss.to_role_id = _to_role_id;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_to_role_id).db_index, (const char *)&back_kiss, sizeof(back_kiss));
	}
}

void FriendIntimacyManager::OnBackToKissByGameWorld(Role * _role, int _to_role_id)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		_role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return ;
	}

	// 主服不直接做处理 发到隐藏服再去处理 , 保证流程的一致性 , 以免消息来回传
	crossgameprotocal::GameHiddenGiveFlowerBackKiss back_kiss;
	back_kiss.role_uid = _role->GetUID();
	_role->GetName(back_kiss.role_name);
	back_kiss.to_role_id = _to_role_id;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&back_kiss, sizeof(back_kiss));
}
