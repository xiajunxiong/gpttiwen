#include "UTAShadow.hpp"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverdef.h"
#include "obj/character/role.h"
#include "global/cross/crossmanager.h"
#include "global/usercache/usercache.hpp"
#include "global/rank/personrank.hpp"
#include "global/rank/rankmanager.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "world.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "gamelog.h"
#include "UTASceneManager.hpp"
#include "servercommon/errornum.h"
#include "scene/speciallogicimpl/speciallogicuta.hpp"
#include "global/team/team.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/userprotocal/msguta.h"
#include "global/bigcross/bigcrossvideoshadow.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldstatus/worldstatus.hpp"

UTAShadow::UTAShadow() : m_cur_status(0), m_round_idx(0), m_season_idx(0),
	m_next_battle_timestamp(0u), m_season_end_timestamp(0u), m_has_create_scene(false),
	m_broadcast_prepare_notice_timestamp(0u), m_broadcast_battle_start_timestamp(0u),
	m_send_notice_to_participants_timestamp(0u), m_uta_scene_mgr(NULL)
{
	m_uta_scene_mgr = new UTASceneManager(this);
}

UTAShadow::~UTAShadow()
{
	if (NULL != m_uta_scene_mgr)
	{
		delete m_uta_scene_mgr;
		m_uta_scene_mgr = NULL;
	}
}

void UTAShadow::Update(unsigned int now)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (this->IsInBattleTime(now))
		{
			if (!this->HasCreateScene() && this->IsInBattleStandbyTime(now))
			{
				this->CreateSceneForSchedules();
				this->OnCreateScene();

				this->SendUTABaseInfoToAll(); // 借用CreateScene标记，下发一下5001
			}
		}
	}

	this->CheckSendPrepareNoticeToParticipants(now);
	this->CheckSendBroadCastNoticeToAll(now);
}

void UTAShadow::OnUserLogin(const UserID& user_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SendUTABaseInfo(user_id);
	this->SendPrepareEnterSceneNotice(user_id);
}

void UTAShadow::OnUserLoginOrigin(const UserID& user_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SendUTABaseInfo(user_id);
	this->SendPrepareEnterSceneNotice(user_id);
}

UTAShadow& UTAShadow::Instance()
{
	static UTAShadow instance;
	return instance;
}

void UTAShadow::OnSyncDataFromBigCross(short cur_status, short season_idx, short round_idx, 
	unsigned int next_battle_timestamp, unsigned int active_reward_begin_timestamp, 
	unsigned int active_reward_end_timestamp, unsigned int season_end_timestamp)
{
	unsigned int old_next_battle_timestamp = m_next_battle_timestamp;

	m_cur_status = cur_status;
	m_season_idx = season_idx;
	m_round_idx = round_idx;
	m_next_battle_timestamp = next_battle_timestamp;
	m_season_end_timestamp = season_end_timestamp;

	if (m_next_battle_timestamp != old_next_battle_timestamp 
		&& m_next_battle_timestamp > 0u)
	{
		m_broadcast_prepare_notice_timestamp = m_next_battle_timestamp - UTA_PREPARE_ENTER_SCENE_TIME_S;
		m_broadcast_battle_start_timestamp = m_next_battle_timestamp;
		m_send_notice_to_participants_timestamp = m_next_battle_timestamp - UTA_PREPARE_ENTER_SCENE_TIME_S;
	}

	WorldStatus::Instance().GetCommonData().uta_active_reward_begin_timestamp = active_reward_begin_timestamp;
	WorldStatus::Instance().GetCommonData().uta_active_reward_end_timestamp = active_reward_end_timestamp;

	this->SendUTABaseInfoToAll();
}

void UTAShadow::OnSyncTeamInfoRequest()
{
	bigchgameprotocol::HGameBigcUTASyncTeamInfoList msg;

	std::map<int, WorldTeamArenaServerTopTeamInfo> map;
	WorldTeamArenaRankManager::Instance().GetTopRankTeam(map);

	std::map<int, WorldTeamArenaServerTopTeamInfo>::iterator it = map.begin();
	for (; it != map.end(); ++it)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(it->second.team_list); ++i)
		{
			const WorldTeamArenaTopTeamInfo& team_info = it->second.team_list[i];
			if (team_info.team_unique_id <= 0LL) continue;

			bigchgameprotocol::MsgUTASyncTeamInfo temp;
			temp.hidden_idx = CrossConfig::Instance().GetHiddenServerIDX();
			temp.plat_type = LocalConfig::Instance().GetPlatType();
			temp.rank = i;
			temp.team_id_in_hidden = team_info.team_unique_id;
			F_STRNCPY(temp.team_name, team_info.team_name, sizeof(temp.team_name));

			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(team_info.team_member_list) == ARRAY_ITEM_COUNT(temp.member_list));
			for (int k = 0; k < ARRAY_ITEM_COUNT(team_info.team_member_list) && k < ARRAY_ITEM_COUNT(temp.member_list); ++k)
			{
				UniqueUserID uuid(LocalConfig::Instance().GetPlatType(), team_info.team_member_list[k].role_uid);
				temp.member_list[k].uuid = UniqueUserIDToLongLong(uuid);
				temp.member_list[k].uid = team_info.team_member_list[k].role_uid;
				temp.member_list[k].profession = team_info.team_member_list[k].profession;
				temp.member_list[k].level = team_info.team_member_list[k].level;
				temp.member_list[k].top_level = team_info.team_member_list[k].top_level;
				temp.member_list[k].avatar_type = team_info.team_member_list[k].avatar_type;
				temp.member_list[k].headshot_id = team_info.team_member_list[k].headshot_id;
				temp.member_list[k].fly_flag = team_info.team_member_list[k].fly_flag;
				temp.member_list[k].capablity = team_info.team_member_list[k].capablity;
				F_STRNCPY(temp.member_list[k].role_name, team_info.team_member_list[k].role_name, sizeof(temp.member_list[k].role_name));
				//temp.member_list[k].appearance = team_info.team_member_list[k].appearance;
			
				UserCacheNode* node = UserCacheManager::Instance().GetUserNode(temp.member_list[k].uid);
				if (NULL != node)
				{
					temp.member_list[k].appearance = node->appearance;
				}
			}

			msg.team_list.push_back(temp);
		}
	}

	InternalComm::Instance().SendSerializeMsgToBigCrossServer(&msg);
}

void UTAShadow::OnTeamListReq(Role* role)
{
	if (NULL == role) return;

	bigchgameprotocol::HGameBigCUserRequestUTATeamList msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnTeamDetailInfoReq(Role* role, long long team_unique_id)
{
	bigchgameprotocol::HGameBigCUserRequestUTATeamDetailInfo msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	msg.team_unique_id = team_unique_id;

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnBigCrossQueryTeamMemberRankReq(long long team_unique_id, int member_num, int* uid_list, int* profession_list)
{
	assert(member_num == UTA_TEAM_MEMBER_NUM);

	crossgameprotocal::HiddenCrossUTAQueryTeamMemberRankReq req;
	req.unique_team_id = team_unique_id;
	for (int i = 0; i < ARRAY_ITEM_COUNT(req.member_uid_list) && i < member_num; ++i)
	{
		req.member_uid_list[i] = uid_list[i];
		req.member_profession_list[i] = profession_list[i];
	}
	InternalComm::Instance().SendToCross((const char*)&req, sizeof(req));
}

void UTAShadow::OnFetchWinBoxReq(Role* role, int sn)
{
	bigchgameprotocol::HGameBigCUTAFetchWinBox msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	msg.box_seq = sn;

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnMyTeamQualificationSchedulesReq(Role* role)
{
	bigchgameprotocol::HGameBigCUTAMyTeamQualificationSchedules msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnTeamQualificationRankListReq(Role* role)
{
	bigchgameprotocol::HGameBigCUTATeamQualificationRankList msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnMainSchedulesReq(Role* role, int round_idx)
{
	bigchgameprotocol::HGameBigCUserRequestUTAMainSchedules msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	msg.round_idx = round_idx;

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnFinalRankListReq(Role* role)
{
	bigchgameprotocol::HGameBigCUserRequestUTAFinalRankList msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnGetQualificationSchedulesReq(Role* role, int round_idx)
{
	bigchgameprotocol::HGameBigCUserRequestUTAQualificationSchedules msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	msg.round_idx = round_idx;

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnGetMainSchedulesOverviewReq(Role* role, int overview_type, bool is_last_season)
{
	bigchgameprotocol::HGameBigCUserRequestUTAMainSchedulesOverview msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role->GetUID();
	msg.overview_type = overview_type;
	msg.is_last_season = is_last_season ? 1 : 0;

	// SendToBigCrossServer已兼容原服
	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

void UTAShadow::OnUserWatchVideoReq(const UserID& user_id, long long video_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const char* video_data = NULL;
		int video_length = 0;
		if (BigCrossVideoShadow::Instance().GetBigCrossVideo(video_id, &video_data, &video_length))
		{
			// 跨服已有录像的缓存，直接发给玩家
			World::GetInstWorld()->TrySendMsgToUser(user_id, video_data, video_length);
		}
		else
		{
			// 跨服没有缓存，向大跨服请求录像
			bigchgameprotocol::HGameBigCUserRequestWatchVideo msg;
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
			msg.role_id = UidToInt(user_id);
			msg.video_id = video_id;

			// SendToBigCrossServer已兼容原服
			InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
		}
	}
	else
	{
		// 原服暂时不缓存录像，所以向跨服请求录像
		crossgameprotocal::GameHiddenUserRequestWatchBigCrossVideo msg;
		msg.role_id = UidToInt(user_id);
		msg.video_id = video_id;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}	
}

void UTAShadow::OnGiveWinBoxResp(int uid, int sn)
{
	const UTAWinBoxRewardCfg* cfg = UTAConfig::Instance().GetWinBoxRewardCfg(sn);
	assert(NULL != cfg);
	if (NULL == cfg) return;

	this->TryBestToGiveUserItems(uid, PUT_REASON_UTA_WIN_BOX, cfg->reward_list, gamestring::g_uta_win_box_reward_subject, gamestring::g_uta_win_box_reward_content);
}

void UTAShadow::OnUTAGiveItemsToUser(int role_id, int uta_put_reason_type, const std::vector<ItemDataWrapper>& itemlist, bool only_send_mail, const std::string& mail_subject, const std::string& mail_content)
{
	UNSTD_STATIC_CHECK(4 == UTA_PUT_REASON_TYPE_MAX);
	assert(uta_put_reason_type >= 0 && uta_put_reason_type < UTA_PUT_REASON_TYPE_MAX);

	if (uta_put_reason_type < 0 || uta_put_reason_type >= UTA_PUT_REASON_TYPE_MAX) return;

	static int PUT_REASON_HASH[UTA_PUT_REASON_TYPE_MAX];
	PUT_REASON_HASH[UTA_PUT_REASON_TYPE_NONE] = 0;
	PUT_REASON_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REWARD] = PUT_REASON_UTA_WIN_BOX;
	PUT_REASON_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REISSUE] = PUT_REASON_UTA_WIN_BOX;
	PUT_REASON_HASH[UTA_PUT_REASON_TYPE_QUALIFICATION_JOIN_REWARD] = PUT_REASON_UTA_TODAY_JOIN_REWARD;
	UNSTD_STATIC_CHECK(4 == UTA_PUT_REASON_TYPE_MAX);

	static const char* MAIL_SUBJECT_HASH[UTA_PUT_REASON_TYPE_MAX];
	MAIL_SUBJECT_HASH[UTA_PUT_REASON_TYPE_NONE] = NULL;
	MAIL_SUBJECT_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REWARD] = gamestring::g_uta_win_box_reward_subject;
	MAIL_SUBJECT_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REISSUE] = gamestring::g_uta_win_box_reward_subject;
	MAIL_SUBJECT_HASH[UTA_PUT_REASON_TYPE_QUALIFICATION_JOIN_REWARD] = gamestring::g_uta_today_join_reward_subject;
	UNSTD_STATIC_CHECK(4 == UTA_PUT_REASON_TYPE_MAX);

	static const char* MAIL_CONTENT_HASH[UTA_PUT_REASON_TYPE_MAX];
	MAIL_CONTENT_HASH[UTA_PUT_REASON_TYPE_NONE] = NULL;
	MAIL_CONTENT_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REWARD] = gamestring::g_uta_win_box_reward_content;
	MAIL_CONTENT_HASH[UTA_PUT_REASON_TYPE_WIN_BOX_REISSUE] = gamestring::g_uta_win_box_reissue_content;
	MAIL_CONTENT_HASH[UTA_PUT_REASON_TYPE_QUALIFICATION_JOIN_REWARD] = gamestring::g_uta_today_join_reward_content;
	UNSTD_STATIC_CHECK(4 == UTA_PUT_REASON_TYPE_MAX);

	if (only_send_mail && !mail_subject.empty() && !mail_content.empty())
	{
		this->TryBestToGiveUserItems(role_id, 0, itemlist, mail_subject, mail_content, only_send_mail);
	}
	else
	{
		this->TryBestToGiveUserItems(role_id, PUT_REASON_HASH[uta_put_reason_type], itemlist,
			MAIL_SUBJECT_HASH[uta_put_reason_type], MAIL_CONTENT_HASH[uta_put_reason_type], only_send_mail);
	}
}

bool RecentlyActiveUserFilter(const UserCacheNode &user_node, void **func_param)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (user_node.last_online_timestamp + 31 * SECOND_PER_DAY < now)
	{
		return false;
	}

	return true;
}

void UTAShadow::OnUTAGiveItemsToServer(const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		assert(0);
		return;
	}

	gamelog::g_log_uta.buff_printf("OnUTAGiveItemsToServer item_list[");
	for (size_t i = 0; i < itemlist.size(); ++i)
	{
		gamelog::g_log_uta.buff_printf("{%d,%d},", 
			itemlist[i].item_id, itemlist[i].num);
	}
	gamelog::g_log_uta.buff_printf("]");
	gamelog::g_log_uta.commit_buff(LL_INFO);

	static MailContentParam contentparam;
	contentparam.Reset();

	for (int k = 0; k < (int)itemlist.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
	{
		contentparam.item_list[k] = itemlist[k];
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), mail_subject.c_str());
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), mail_content.c_str());
	if (lensub > 0 && length > 0)
	{
#ifdef _DEBUG
		static const int ONCE_OPER_NUM = 1;	// 用于测试是否会重复发邮件
#else
		static const int ONCE_OPER_NUM = 1024;
#endif
		UserID user_list[ONCE_OPER_NUM];
		int from = 0;
		while (1)
		{
			int count = UserCacheManager::Instance().GetUserIDList(from, ONCE_OPER_NUM, user_list, RecentlyActiveUserFilter);
			if (count <= 0) break;

			MailRoute::Instance().MailToMultUser(user_list, count, MAIL_REASON_DEFAULT, contentparam);
		}	
	}
	else
	{
		assert(0);
	}
}

void UTAShadow::TryBestToGiveUserItems(int role_id, int put_reason_type, const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content, bool only_send_mail)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		if (only_send_mail)
		{
			if (!this->GiveUserItemsByMail(role_id, itemlist, mail_subject, mail_content))
			{
				// log 只发放邮件
				this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "OnlySendMailSucc");
			}
			else
			{
				// log 背包和邮件都不成功
				this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "OnlySendMailFailed");
			}
		}
		else
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(itemlist, put_reason_type))
			{
				if (!this->GiveUserItemsByMail(role_id, itemlist, mail_subject, mail_content))
				{
					// log 放背包不成功，但成功发放了邮件
					this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "PutFailedButMailSucc");
				}
				else
				{
					// log 背包和邮件都不成功
					this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "PutFailedAndMailFailed");
				}
			}
			else
			{
				// log 成功发放
				this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "PutSucc");
			}
		}		
	}
	else if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameUTAGiveItemsToUser msg;
		msg.role_id = role_id;
		msg.put_reason_type = static_cast<short>(put_reason_type);
		msg.only_send_mail = only_send_mail ? 1 : 0;
		msg.mail_subject = mail_subject;
		msg.mail_content = mail_content;
		msg.itemlist = itemlist;
		if (InternalComm::Instance().SendSerializeMsgToGameThroughCross(&msg, IntToUid(role_id).db_index))
		{
			this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "TransferToOrigin");
		}
		else
		{
			this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "TransferToOriginFailed!");
		}	
	}
	else
	{
		// 已在原服但玩家不在线，发邮件
		if (!this->GiveUserItemsByMail(role_id, itemlist, mail_subject, mail_content))
		{
			//log 给邮件失败，
			this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "UserNotOnlineGiveMailFailed");
		}
		else
		{
			// log 成功给邮件
			this->LogGiveUserItemsResult(role_id, put_reason_type, itemlist, "UserNotOnlineGiveMailSucc");
		}
	}
}

void UTAShadow::TryBestToGiveUserItems(int role_id, int put_reason_type, const std::vector<ItemConfigData>& itemlist, const std::string& mail_subject, const std::string& mail_content, bool only_send_mail)
{
	std::vector<ItemDataWrapper> wrapper_list;
	wrapper_list.reserve(itemlist.size());

	for (size_t i = 0; i < itemlist.size(); ++i)
	{
		wrapper_list.push_back(ItemDataWrapper(itemlist[i].item_id, itemlist[i].num, itemlist[i].is_bind ? 1 : 0));
	}
	this->TryBestToGiveUserItems(role_id, put_reason_type, wrapper_list, mail_subject, mail_content, only_send_mail);
}

bool UTAShadow::GiveUserItemsByMail(int role_id, const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content)
{
	static MailContentParam contentparam; 
	contentparam.Reset();

	for (int k = 0; k < (int)itemlist.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
	{
		contentparam.item_list[k] = itemlist[k];
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), mail_subject.c_str());
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), mail_content.c_str());
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(IntToUid(role_id), MAIL_REASON_DEFAULT, contentparam);
		return true;
	}
	else
	{
		return false;
	}
}

void UTAShadow::LogGiveUserItemsResult(int role_id, int put_reason_type, const std::vector<ItemDataWrapper>& itemlist, const std::string& msg)
{
	gamelog::g_log_uta.buff_printf("TryBestToGiveUserItems role[%d] put_reason[%d] msg[%s] items[", role_id, put_reason_type, msg.c_str());
	for (size_t i = 0; i < itemlist.size(); ++i)
	{
		gamelog::g_log_uta.buff_printf("{item:%d num:%d isbind:%d due_time:%u}",
			itemlist[i].item_id, itemlist[i].num, itemlist[i].is_bind, itemlist[i].invalid_time);
		if (i < itemlist.size() - 1)
		{
			gamelog::g_log_uta.buff_printf(",");
		}
	}
	gamelog::g_log_uta.buff_printf("]");
	gamelog::g_log_uta.commit_buff(LL_INFO);
}

void UTAShadow::OnUTASyncSchedules(bigchgameprotocol::BigCHGameSyncSchedules* msg)
{
	m_schedule_map.clear();
	m_user_hash_map.clear();
	m_has_create_scene = false;

	for (size_t i = 0; i < msg->schedule_list.size(); ++i)
	{
		UTAScheduleShadow s;
		bigchgameprotocol::MsgScheduleInfo& schedule = msg->schedule_list[i];
		s.schedule_id = schedule.schedule_id;
		s.round_idx = msg->round_idx;
		s.is_main_match = schedule.is_main_match > 0;
		s.is_win_group = schedule.is_win_group > 0;
		s.atk_plat_type = schedule.atk_plat_type;
		s.atk_hidden_idx = schedule.atk_hidden_server_idx;
		s.atk_team_unique_id = schedule.atk_team_unique_id;
		F_STRNCPY(s.atk_team_name, schedule.atk_team_name, sizeof(GameName));
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			s.atk_member_uid_list[k] = schedule.atk_member_uid_list[k];
		}

		s.def_plat_type = schedule.def_plat_type;
		s.def_hidden_idx = schedule.def_hidden_server_idx;
		s.def_team_unique_id = schedule.def_team_unique_id;
		F_STRNCPY(s.def_team_name, schedule.def_team_name, sizeof(GameName));
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			s.def_member_uid_list[k] = schedule.def_member_uid_list[k];
		}

		m_schedule_map.insert(ScheduleMap::value_type(s.schedule_id, s));

		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			m_user_hash_map.insert(
				UserScheduleHashMap::value_type(
					UniqueUserID(s.atk_plat_type, s.atk_member_uid_list[k]),
					UTAUserScheduleHash(s.schedule_id, true, s.atk_member_uid_list[0])
				)
			);
			m_user_hash_map.insert(
				UserScheduleHashMap::value_type(
					UniqueUserID(s.def_plat_type, s.def_member_uid_list[k]),
					UTAUserScheduleHash(s.schedule_id, false, s.def_member_uid_list[0])
				)
			);
		}
	}
}

void UTAShadow::OnUTAScheduleTeamMemberDetailResp(bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp* msg)
{
	ScheduleMap::iterator it = m_schedule_map.find(msg->schedule_id);
	if (it == m_schedule_map.end()) return;

	if (it->second.atk_member_uid_list[0] > 0)
	{
		SpecialLogicUTA* spl = m_uta_scene_mgr->GetSpecialLogic(it->second.atk_member_uid_list[0]);
		if (NULL != spl)
		{
			spl->OnTeamMemberDetailResp(msg);
		}
	}
	
	if (it->second.def_member_uid_list[0] > 0)
	{
		SpecialLogicUTA* spl = m_uta_scene_mgr->GetSpecialLogic(it->second.def_member_uid_list[0]);
		if (NULL != spl)
		{
			spl->OnTeamMemberDetailResp(msg);
		}
	}
}

bool UTAShadow::IsInBattleTime(unsigned int now)
{
	if (now >= m_next_battle_timestamp && now < m_next_battle_timestamp + UTA_BATTLE_DURATION_S)
	{
		return true;
	}
	return false;
}

bool UTAShadow::IsInBattleStandbyTime(unsigned int now)
{
	if (now >= m_next_battle_timestamp && now < m_next_battle_timestamp + UTA_BATTLE_STANDBY_TIME_S)
	{
		return true;
	}
	return false;
}

void UTAShadow::TryEnterBattleScene(Role* role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->EnterScene(role);
	}
	else
	{
		this->TryEnterSceneInOrigin(role);
	}
}

bool UTAShadow::TryReEnterBattleScene(int uid, int *out_scene_id, int * out_fb_key)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	const UTAScheduleShadow* schedule = this->GetUserSchedule(plat_type, uid, NULL);
	if (NULL == schedule) return false;

	SpecialLogicUTA* spl = m_uta_scene_mgr->GetSpecialLogic(plat_type, uid, *schedule);
	if (NULL == spl || !spl->IsTeamMember(uid) || spl->CanDestroy()) return false;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInBattleTime(now)) return false;

	if (NULL != out_scene_id) *out_scene_id = spl->GetScene()->GetSceneID();
	if (NULL != out_fb_key) *out_fb_key = spl->GetScene()->GetSceneKey();

	return true;
}

bool UTAShadow::CanGiveGuessCoinForActive()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= WorldStatus::Instance().GetCommonData().uta_active_reward_begin_timestamp
		&& now <= WorldStatus::Instance().GetCommonData().uta_active_reward_end_timestamp)
	{
		return true;
	}

	return false;
}

void UTAShadow::OnBattleEndNoticeTeam(long long schedule_id, long long team_unique_id)
{
	SpecialLogicUTA* spl = m_uta_scene_mgr->GetSpecialLogic(schedule_id, team_unique_id);
	if (NULL == spl) return;

	spl->OnBattleEndDelayKickoutAll();
}

const UTAScheduleShadow* UTAShadow::GetScheduleShadow(long long schedule_id)
{
	ScheduleMap::iterator it = m_schedule_map.find(schedule_id);
	if (it != m_schedule_map.end())
	{
		return &it->second;
	}

	return NULL;
}

bool UTAShadow::HasSyncDataFromBigCross()
{
	return UTA_STATUS_INVALID == m_cur_status;
}

bool UTAShadow::HasCreateScene()
{
	return m_has_create_scene;
}

void UTAShadow::OnCreateScene()
{
	m_has_create_scene = true;
}

void UTAShadow::CreateSceneForSchedules()
{
	bool is_main_match = (m_cur_status == UTA_STATUS_MAIN ? 1 : 0);
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int timeout_timestamp = GetMin(now, m_next_battle_timestamp) + UTA_BATTLE_DURATION_S - 5;
	unsigned int start_battle_timestamp = m_next_battle_timestamp + UTA_BATTLE_STANDBY_TIME_S;

	ScheduleMap::iterator it = m_schedule_map.begin();
	for (; it != m_schedule_map.end(); ++it)
	{
		const UTAScheduleShadow& schedule = it->second;
		if (schedule.atk_plat_type == LocalConfig::Instance().GetPlatType()
			&& schedule.atk_hidden_idx == CrossConfig::Instance().GetHiddenServerIDX())
		{
			m_uta_scene_mgr->CreateSceneForAttacker(schedule, start_battle_timestamp, timeout_timestamp);
		}

		if (schedule.def_plat_type == LocalConfig::Instance().GetPlatType()
			&& schedule.def_hidden_idx == CrossConfig::Instance().GetHiddenServerIDX())
		{
			m_uta_scene_mgr->CreateSceneForDefender(schedule, start_battle_timestamp, timeout_timestamp);
		}
	}
}

void UTAShadow::EnterScene(Role* role)
{
	if (NULL == role) return;
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInBattleTime(now))
	{
		role->NoticeNum(errornum::EN_UTA_NOT_IN_ENTER_SCENE_TIME);
		return;
	}

	if (!this->IsInBattleStandbyTime(now))
	{
		role->NoticeNum(errornum::EN_UTA_NOT_IN_ENTER_SCENE_TIME);
		return;
	}

	if (Scene::SCENE_TYPE_UTA == role->GetScene()->GetSceneType())
	{
		// 已经在场景内，就不用额外进场了
		return;
	}

	if (!role->CanEnterFB(true))
	{
		// 在CanEnterFB里已经有notice_num
		return;
	}

	SpecialLogicUTA* spl = this->GetSpecialLogic(role);
	if (NULL == spl || !spl->IsTeamMember(role->GetUID()))
	{
		role->NoticeNum(errornum::EN_UTA_SCENE_NOT_FOUND);
		return;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			bool someone_cannot_enter = false;
			for (int i = 0; i < team->GetMemberCount(); ++i)
			{
				Role* member = team->GetMemberRoleByIndex(i);
				if (NULL == member)
				{
					someone_cannot_enter = true;
					break;
				}

				if (!spl->IsTeamMember(member->GetUID()))
				{
					someone_cannot_enter = true;
					break;
				}

				if (!member->CanEnterFB(false))
				{
					someone_cannot_enter = true;
					break;
				}
			}
			if (someone_cannot_enter)
			{
				role->NoticeNum(errornum::EN_UTA_TEAM_ENTER_SCENE_FAILED);
				return;
			}
			else
			{
				Posi town_pos;
				spl->GetScene()->GetTownPoint(&town_pos);

				if (!World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, spl->GetScene()->GetSceneID(), spl->GetScene()->GetSceneKey(), town_pos))
				{
					role->NoticeNum(errornum::EN_UTA_TEAM_ENTER_SCENE_FAILED);
					return;
				}
			}
		}
		else
		{
			UserID user_id = role->GetUserId();
			team->LeaveTeamByUid(role->GetUID());

			// 重新获取role实例（怕离开队伍导致切场景）
			role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
			if (NULL == role) return;

			if (!World::GetInstWorld()->GetSceneManager()->GoTo(role, spl->GetScene()->GetSceneID(), spl->GetScene()->GetSceneKey()))
			{
				role->NoticeNum(errornum::EN_UTA_ENTER_SCENE_FAILED);
				return;
			}
		}	
	}
	else
	{
		if (!World::GetInstWorld()->GetSceneManager()->GoTo(role, spl->GetScene()->GetSceneID(), spl->GetScene()->GetSceneKey()))
		{
			role->NoticeNum(errornum::EN_UTA_ENTER_SCENE_FAILED);
			return;
		}
	}
}

SpecialLogicUTA* UTAShadow::GetSpecialLogic(Role* role)
{
	UserScheduleHashMap::iterator it = m_user_hash_map.find(role->GetUniqueUserID());
	if (it == m_user_hash_map.end())
	{
		return NULL;
	}

	return m_uta_scene_mgr->GetSpecialLogic(it->second.scene_key);
}

void UTAShadow::TryEnterSceneInOrigin(Role* role)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInBattleTime(now))
	{
		role->NoticeNum(errornum::EN_UTA_NOT_IN_ENTER_SCENE_TIME);
		return;
	}

	if (!this->IsInBattleStandbyTime(now))
	{
		role->NoticeNum(errornum::EN_UTA_NOT_IN_ENTER_SCENE_TIME);
		return;
	}

	if (!role->CanEnterFB(true))
	{
		// 在CanEnterFB里已经有notice_num
		return;
	}

	bool is_attacker = false;
	const UTAScheduleShadow* schedule = this->GetUserSchedule(role, &is_attacker);
	if (NULL == schedule || !schedule->IsTeamMember(role->GetUID(), is_attacker))
	{
		role->NoticeNum(errornum::EN_UTA_SCENE_NOT_FOUND);
		return;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			bool someone_cannot_enter = false;
			for (int i = 0; i < team->GetMemberCount(); ++i)
			{
				Role* member = team->GetMemberRoleByIndex(i);
				if (NULL == member)
				{
					someone_cannot_enter = true;
					break;
				}

				if (!schedule->IsTeamMember(member->GetUID(), is_attacker))
				{
					someone_cannot_enter = true;
					break;
				}

				if (!member->CanEnterFB(false))
				{
					someone_cannot_enter = true;
					break;
				}
			}
			if (someone_cannot_enter)
			{
				role->NoticeNum(errornum::EN_UTA_TEAM_ENTER_SCENE_FAILED);
				return;
			}
			else
			{
				static CrossData_Business business_data;
				business_data.Reset();
				business_data.origin_team_index = team->GetTeamIndex();
				business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_TEAM_ENTER_UTA_BATTLE_SCENE;
				business_data.level_limit_low = team->GetLimitLevelLow();
				business_data.level_limit_high = team->GetLimitLevelHigh();
				business_data.team_type = team->GetTeamType();
				business_data.allown_red_join = team->TeamIsAllowRedNameJoin() ? 1 : 0;
				int other_member_count = 0;
				for (int i = 0; i < team->GetMemberCount() && other_member_count < 4; ++i)
				{
					TeamMember* tmb = team->GetMemberInfoByIndex(i);
					if (NULL == tmb || tmb->IsRobot() || !tmb->IsFollow()) continue;

					business_data.teammate_list[other_member_count++] = tmb->uid;
				}

				if (role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data))
				{
					team->SetNotice(false, Team::AUTO_JOIN_LIMIT_TIME);
				}
			}
		}
		else
		{
			UserID user_id = role->GetUserId();
			team->LeaveTeamByUid(role->GetUID());

			// 重新获取role实例（怕离开队伍导致切场景）
			role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
			if (NULL == role) return;

			CrossData_Business business;
			business.cross_business_type = CrossData_Business::BUSINESS_TYPE_ENTER_UTA_BATTLE_SCENE;
			role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business);
		}
	}
	else
	{
		CrossData_Business business;
		business.cross_business_type = CrossData_Business::BUSINESS_TYPE_ENTER_UTA_BATTLE_SCENE;
		role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business);
	}

	
}

const UTAScheduleShadow* UTAShadow::GetUserSchedule(Role* role, bool* is_attacker)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	return this->GetUserSchedule(plat_type, role->GetUID(), is_attacker);
}

const UTAScheduleShadow* UTAShadow::GetUserSchedule(int plat_type, int role_id, bool* is_attacker)
{
	UniqueUserID uuser_id(plat_type, role_id);
	UserScheduleHashMap::iterator it = m_user_hash_map.find(uuser_id);
	if (it == m_user_hash_map.end()) return NULL;

	ScheduleMap::iterator it2 = m_schedule_map.find(it->second.schedule_id);
	if (it2 == m_schedule_map.end()) return NULL;

	if (NULL != is_attacker)
	{
		*is_attacker = it->second.is_attacker;
	}
	return &it2->second;
}

bool UTAShadow::SendPrepareNoticeToAllParticipants()
{
	if (m_user_hash_map.empty()) return false;

	Protocol::SCUTAPrepareEnterSceneNotice notice;
	this->AssemblePrepareNoticeMsg(&notice);

	int plat_type = LocalConfig::Instance().GetPlatType();
	UserScheduleHashMap::iterator it = m_user_hash_map.begin();
	for (; it != m_user_hash_map.end(); ++it)
	{
		if (it->first.plat_type != plat_type) continue;

		World::GetInstWorld()->SendToRole(UidToInt(it->first.user_id), &notice, sizeof(notice));

		gamelog::g_log_uta.printf(LL_INFO, "SCUTAPrepareEnterSceneNotice role[%d]", UidToInt(it->first.user_id));
	}

	return true;
}

void UTAShadow::AssemblePrepareNoticeMsg(Protocol::SCUTAPrepareEnterSceneNotice* notice)
{
	notice->start_enter_scene_time = m_next_battle_timestamp;
	notice->start_prepare_time = m_next_battle_timestamp - UTA_PREPARE_ENTER_SCENE_TIME_S;
	notice->stop_enter_scene_time = m_next_battle_timestamp + UTA_BATTLE_STANDBY_TIME_S;
}

void UTAShadow::CheckSendPrepareNoticeToParticipants(unsigned int now)
{
	if (m_send_notice_to_participants_timestamp > 0u &&
		now > m_send_notice_to_participants_timestamp)
	{
		if (!this->SendPrepareNoticeToAllParticipants())
		{
			// 发送失败的话，3秒后重试
			m_send_notice_to_participants_timestamp += 3;
		}
		else if (m_send_notice_to_participants_timestamp < m_next_battle_timestamp)
		{
			// 成功的话 开始进场时再发一次
			m_send_notice_to_participants_timestamp = m_next_battle_timestamp;
		}
		else
		{
			m_send_notice_to_participants_timestamp = 0u;
		}
	}
}

void UTAShadow::CheckSendBroadCastNoticeToAll(unsigned int now)
{
	if (m_broadcast_prepare_notice_timestamp > 0u 
		&& now > m_broadcast_prepare_notice_timestamp)
	{
		m_broadcast_prepare_notice_timestamp = 0u;
		this->BroadcastPrepareNotice();
	}

	if (m_broadcast_battle_start_timestamp > 0u
		&& now > m_broadcast_battle_start_timestamp)
	{
		m_broadcast_battle_start_timestamp = 0u;
		this->BroadcastBattleStartNotice();
	}
}

void UTAShadow::BroadcastPrepareNotice()
{
	if (UTA_STATUS_QUALIFICATION == m_cur_status)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_uta_qualification_prepare_notice, m_round_idx / 2 + 1);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
	else if (UTA_STATUS_MAIN == m_cur_status)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_uta_main_prepare_notice, m_round_idx + 1);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
	else
	{
		assert(0);
	}
}

void UTAShadow::BroadcastBattleStartNotice()
{
	if (UTA_STATUS_QUALIFICATION == m_cur_status)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_uta_qualification_enter_notice, m_round_idx / 2 + 1);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
	else if (UTA_STATUS_MAIN == m_cur_status)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_uta_main_enter_notice, m_round_idx + 1);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
	else
	{
		assert(0);
	}
}

void UTAShadow::SendUTABaseInfo(const UserID& user_id)
{
	Protocol::SCUTABaseInfo msg;
	this->AssembleSCUTABaseInfo(&msg);

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, sizeof(msg));
}

void UTAShadow::SendUTABaseInfoToAll()
{
	Protocol::SCUTABaseInfo msg;
	this->AssembleSCUTABaseInfo(&msg);

	World::GetInstWorld()->SendToALLGameAllGateway(&msg, sizeof(msg));
}

void UTAShadow::AssembleSCUTABaseInfo(Protocol::SCUTABaseInfo* msg)
{
	msg->cur_status = m_cur_status;
	msg->round_idx = m_round_idx;
	msg->season_idx = m_season_idx;
	msg->next_battle_timestamp = m_next_battle_timestamp;
	msg->season_end_timestamp = m_season_end_timestamp;
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= msg->next_battle_timestamp && now <= m_next_battle_timestamp + UTA_BATTLE_DURATION_S)
	{
		msg->is_in_progress = 1;
	}
	else
	{
		msg->is_in_progress = 0;
	}
}

void UTAShadow::SendPrepareEnterSceneNotice(const UserID& user_id)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	if (m_user_hash_map.find(UniqueUserID(plat_type, user_id)) == m_user_hash_map.end())
	{
		return;
	}

	if (m_next_battle_timestamp > 0u)
	{
		Protocol::SCUTAPrepareEnterSceneNotice notice;
		this->AssemblePrepareNoticeMsg(&notice);
		World::GetInstWorld()->SendToRole(UidToInt(user_id), &notice, sizeof(notice));

		gamelog::g_log_uta.printf(LL_INFO, "SCUTAPrepareEnterSceneNotice role[%d]", UidToInt(user_id));
	}
}

bool UTAScheduleShadow::IsTeamMember(int uid, bool is_attacker) const
{
	if (is_attacker)
	{
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			if (uid == atk_member_uid_list[k]) return true;
		}
	}
	else
	{
		for (int k = 0; k < UTA_TEAM_MEMBER_NUM; ++k)
		{
			if (uid == def_member_uid_list[k]) return true;
		}
	}

	return false;
}
