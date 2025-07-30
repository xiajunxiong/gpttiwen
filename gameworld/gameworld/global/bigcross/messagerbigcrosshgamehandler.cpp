#include "bigcrossmanager.hpp"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include <set>
#include "engineadapter.h"
#include "gamelog.h"
#include "internalcomm.h"

#include "global/bigcross/utachampionrecordmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "global/bigcross/utaguessshadow.hpp"

#include "other/route/mailroute.hpp"
#include "obj/character/role.h"
#include "servercommon/servercommon.h"

#define REGISTER_GAME_BIGC_HANLDER(BIGC_MSG_CODE , FUNCTION_NAME , FUNCTION_SET) { \
	if(NULL != m_bigc_hgame_handler_list[bigchgameprotocol::BIGC_MSG_CODE - bigchgameprotocol::MT_BIGC_HGAME_PROTOCOL_BEGIN])\
	{																												\
		printf("REGISTER_GAME_BIGC_HANLDER msg_code[%d] repeat\n", bigchgameprotocol::BIGC_MSG_CODE);				\
		assert(0);																									\
		return;																										\
	}																												\
	if(FUNCTION_SET.find(#FUNCTION_NAME) != FUNCTION_SET.end())														\
	{																												\
		printf("REGISTER_GAME_BIGC_HANLDER msg_code[%d] function[%s] repeat\n", bigchgameprotocol::BIGC_MSG_CODE, #FUNCTION_NAME);\
		assert(0);																												\
		return;																													\
	}																															\
	m_bigc_hgame_handler_list[bigchgameprotocol::BIGC_MSG_CODE - bigchgameprotocol::MT_BIGC_HGAME_PROTOCOL_BEGIN] = &FUNCTION_NAME;		\
	FUNCTION_SET.insert(#FUNCTION_NAME);																						\
}\



void BigCrossManager::InitBigCHGameHandler()
{
	memset(m_bigc_hgame_handler_list, 0, sizeof(m_bigc_hgame_handler_list));
	std::set<std::string> function_add_set;

	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_REGISTER_SERVER_ACK, BigCrossManager::OnRegisterServerAck, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_HEART_BEAT, BigCrossManager::OnHeartBeat, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SECRET_KEY_ACK, BigCrossManager::OnSecretKeyAck, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_GET_UTA_CHAMPION_RECORD_ACK, BigCrossManager::OnGetUTAChampionRecordAck, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SYNC_UTA_BASE_DATA, BigCrossManager::OnSyncUTABaseInfo, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_REQUEST_SYNC_TEAM_INFO, BigCrossManager::OnRequestSyncUTATeamList, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_MESSAGE_SEND_TO_USER, BigCrossManager::OnBigCrossSendMessageToUser, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SYNC_MAIL_INFO, BigCrossManager::OnSyncMailInfo, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SYNC_ROLE_REQ_RET, BigCrossManager::OnSyncRoleReqRet, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SEND_MSG_TO_ROLE, BigCrossManager::OnSendMsgToRole, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_GIVE_LOTTERY_TO_ROLE, BigCrossManager::OnGiveLotteryToRole, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_QUERY_MEMBER_RANK, BigCrossManager::OnBigCRequestUTATeamMemberRank, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_GIVE_WIN_BOX_REWARD, BigCrossManager::OnBigCGiveUserUTAWinBoxReward, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_GIVE_USER_ITEMS, BigCrossManager::OnUTAGiveUserItems, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_SYNC_SCHEDULES, BigCrossManager::OnSyncUTASchedules, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_SCHEDULE_TEAMMEMBER_DETAIL_RESP, BigCrossManager::OnUTAScheduleTeamMemberDetailResp, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_TRANSFER_MSG_FROM_BATTLE_TO_HIDDEN, BigCrossManager::OnTransferMsgFromBigcrossBattleToHidden, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_SYNC_VIDEO_DATA, BigCrossManager::OnSyncVideoData, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_SYNC_BATTLE_INFO_FOR_OB, BigCrossManager::OnUTASyncBattleInfoForOB, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_SYNC_ROUND_RESULT_FOR_OB, BigCrossManager::OnUTASyncRoundResultForOB, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_BATTLE_END_STOP_OB, BigCrossManager::OnUTABattleEndStopOB, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_CHAMPION_RECORD_REQUEST_UPDATE_DATA, BigCrossManager::OnUTAChampionRecordRequestUserUpdateData, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_USER_ENTER_SCENE_CONTINUE_UTA_BATTLE, BigCrossManager::OnUTAUserEnterSceneContinueBattle, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_BATTLE_END_NOTICE_TEAM, BigCrossManager::OnBattleEndNoticeTeam, function_add_set);
	REGISTER_GAME_BIGC_HANLDER(MT_BIGC_HGAME_UTA_GIVE_SERVER_ITEMS, BigCrossManager::OnUTAGiveServerItems, function_add_set);
}

void BigCrossManager::OnRegisterServerAck(NetID _netid, const char * _data, int _length)
{
	// 收到这个表示注册服务器通过
	gamelog::g_log_world.printf(LL_INFO, "%s Succ !!!", __FUNCTION__);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

}

void BigCrossManager::OnSecretKeyAck(NetID _netid, const char * _data, int _length)
{
	// 收到这个表示验证通过 , 需要注册服务器
	gamelog::g_log_world.printf(LL_INFO, "%s Succ !!!", __FUNCTION__);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	// 通过验证后 注册服务器
	this->RegisterGameServer();
}

void BigCrossManager::OnHeartBeat(NetID _netid, const char * _data, int _length)
{
	m_bigc_server_hearbeat_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void BigCrossManager::OnGetUTAChampionRecordAck(NetID _netid, const char * _data, int _length)
{
	UTAChampionRecordManager::Instance().SendUTAChampionRecordToRole(_data, _length);
}

void BigCrossManager::OnSyncUTABaseInfo(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameUTABaseInfoSync* msg = (bigchgameprotocol::BigCHGameUTABaseInfoSync*)_data;
	UTAShadow::Instance().OnSyncDataFromBigCross(msg->cur_status, msg->season_index, msg->round_idx, 
		msg->next_battle_timestamp, msg->active_reward_begin_timestamp, 
		msg->active_reward_end_timestamp, msg->season_end_timestamp);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameUTABaseInfoSync sync_msg;
		sync_msg.cur_status = msg->cur_status;
		sync_msg.next_battle_timestamp = msg->next_battle_timestamp;
		sync_msg.reserve_sh = msg->reserve_sh;
		sync_msg.round_idx = msg->round_idx;
		sync_msg.season_index = msg->season_index;
		sync_msg.active_reward_begin_timestamp = msg->active_reward_begin_timestamp;
		sync_msg.active_reward_end_timestamp = msg->active_reward_end_timestamp;
		sync_msg.season_end_timestamp = msg->season_end_timestamp;

		InternalComm::Instance().SendToAllGameThroughCross(&sync_msg, sizeof(sync_msg));
	}
}

void BigCrossManager::OnRequestSyncUTATeamList(NetID _netid, const char * _data, int _length)
{
	UTAShadow::Instance().OnSyncTeamInfoRequest();
}

void BigCrossManager::OnBigCrossSendMessageToUser(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameMessageToUser* header = (bigchgameprotocol::BigCHGameMessageToUser*)_data;

	if (_length < sizeof(bigchgameprotocol::BigCHGameMessageToUser) + sizeof(Protocol::MessageHeader))
	{
		return;
	}

	int body_len = _length - sizeof(bigchgameprotocol::BigCHGameMessageToUser);
	const char* body = _data + sizeof(bigchgameprotocol::BigCHGameMessageToUser);

	World::GetInstWorld()->TrySendMsgToUser(IntToUid(header->uid), body, body_len);
}

void BigCrossManager::OnSyncMailInfo(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameSyncMailInfo* msg = (bigchgameprotocol::BigCHGameSyncMailInfo*)_data;

	MailRoute::Instance().MailToUser(IntToUid(msg->uid), MAIL_REASON_DEFAULT, msg->param);
}

void BigCrossManager::OnSyncRoleReqRet(NetID _netid, const char * _data, int _length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	bigchgameprotocol::BigCHGameSyncRoleReqRet* msg = (bigchgameprotocol::BigCHGameSyncRoleReqRet*)_data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	if (NULL != role)
	{
		role->NoticeNum(msg->error_num);
	}
	else
	{
		crossgameprotocal::HiddenGameSyncRoleReqRet ret;
		ret.role_id = msg->uid;
		ret.error_num = msg->error_num;
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(ret.role_id).db_index, (char *)&ret, sizeof(ret));
	}
}

void BigCrossManager::OnSendMsgToRole(NetID _netid, const char * _data, int _length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	bigchgameprotocol::BigCHGameSendMsgToRole* bh_to_role = (bigchgameprotocol::BigCHGameSendMsgToRole*)_data;

	const void* _msg = _data + sizeof(bigchgameprotocol::BigCHGameSendMsgToRole);
	int _msg_len = _length - sizeof(bigchgameprotocol::BigCHGameSendMsgToRole);
	World::GetInstWorld()->SendToRole(bh_to_role->target_role_uid, _msg, _msg_len);
}

void BigCrossManager::OnGiveLotteryToRole(NetID _netid, const char * _data, int _length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	bigchgameprotocol::BigCHGameGiveLotteryToRole* msg = (bigchgameprotocol::BigCHGameGiveLotteryToRole*)_data;

	UTAGuessShadow::Instance().GiveLotteryToRole(msg->target_uid, msg->num, msg->reason);
}

void BigCrossManager::OnBigCRequestUTATeamMemberRank(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameUTARequestTeamMemberRank* msg = (bigchgameprotocol::BigCHGameUTARequestTeamMemberRank*)_data;
	UTAShadow::Instance().OnBigCrossQueryTeamMemberRankReq(msg->unique_team_id, 
		ARRAY_ITEM_COUNT(msg->member_uid_list), msg->member_uid_list, msg->member_profession_list);
}

void BigCrossManager::OnBigCGiveUserUTAWinBoxReward(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameUTAGiveWinBoxReward* msg = (bigchgameprotocol::BigCHGameUTAGiveWinBoxReward*)_data;
	UTAShadow::Instance().OnGiveWinBoxResp(msg->role_id, msg->box_seq);
}

void BigCrossManager::OnUTAGiveUserItems(NetID _netid, const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameUTAGiveItemsToUser real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(_data, _length, &offset))
	{
		return;
	}

	UTAShadow::Instance().OnUTAGiveItemsToUser(real_msg.role_id, real_msg.uta_put_reason_type, real_msg.itemlist, real_msg.only_send_mail > 0, real_msg.mail_subject, real_msg.mail_content);
}

void BigCrossManager::OnSyncUTASchedules(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameSyncSchedules real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(_data, _length, &offset))
	{
		return;
	}

	UTAShadow::Instance().OnUTASyncSchedules(&real_msg);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameUTASyncSchedules sync_msg;
		int msg_type = sync_msg.GetMessageType();
		int offset = 0;
		if (!sync_msg.Deserialize(_data, _length, &offset))
		{
			assert(0);
			return;
		}
		sync_msg.header.msg_type = msg_type;
		UNSTD_STATIC_CHECK(sizeof(sync_msg) == sizeof(real_msg));
		UNSTD_STATIC_CHECK(sizeof(crossgameprotocal::MsgScheduleInfo) == sizeof(bigchgameprotocol::MsgScheduleInfo));
		InternalComm::Instance().SendSerializeMsgToAllGameThroughCross(&sync_msg);
	}
}