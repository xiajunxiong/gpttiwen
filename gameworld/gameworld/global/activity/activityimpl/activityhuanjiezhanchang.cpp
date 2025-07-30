#include "obj/character/role.h"
#include "engineadapter.h"
#include "battle/battle_manager_local.hpp"
#include "other/route/mailroute.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "servercommon/string/gameworldstr.h"
#include "config/logicconfigmanager.hpp"
#include "world.h"
#include "global/usercache/usercache.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "other/event/eventhandler.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "item/money.h"
#include "other/title/title.hpp"
#include "global/team/team.hpp"
#include "other/robot/robotconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/funopen/funopen.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/offlinebattledata/offlinebattledata.hpp"

#include "other/gm/gmtester/activityhuanjiezhanchangtester.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "other/huanjie/rolehuanjie.hpp"
#include "config/activityconfig/huanjiezhanchang/activityhuanjiezhanchangconfig.hpp"
#include "protocol/msghuanjiezhanchang.h"
#include "activityhuanjiezhanchang.hpp"

void HuanJieZhanChangSendErrNotice(std::vector<Role *> role_list, int role_id, int err_reason)
{
	Protocol::SCHuanJieZhanChangErrReturn err_return;
	err_return.role_id = role_id;
	err_return.reson = err_reason;

	for (int i = 0; i < (int)role_list.size(); ++i)
	{
		Role * role = role_list[i];
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &err_return, sizeof(err_return));
		}		
	}
}

ActivityHuanJieZhanChang::ActivityHuanJieZhanChang(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG)
{
	m_has_sync_season_num = CrossConfig::Instance().IsHiddenServer();  //跨服里不用同步
	m_season_num = SEASON_NUM_BEGIN;
	m_last_update_match_time = 0;
	m_order_flag = false;
	m_need_build_rank = true;
	m_need_snapshot = true;
	m_last_rebuild_rank_time = 0;
	m_last_rank_snapshot_time = 0;
	m_season_end_time = EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);

	m_fight_seq = 1;
	m_single_record_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD);
	m_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA);
	assert(m_single_record_data != NULL);
	assert(m_save_data != NULL);
}

ActivityHuanJieZhanChang::~ActivityHuanJieZhanChang()
{
}

bool ActivityHuanJieZhanChang::IsNeedSave()
{
	return CrossConfig::Instance().IsHiddenServer();  //这个是跨服活动，隐藏服才保存数据
}

void ActivityHuanJieZhanChang::Init(const ActivityData & arg_data)
{
	const HuanJieZhanChangActivityData & data = arg_data.huan_jie_activity_data;
	m_season_num = data.season_num;
}

void ActivityHuanJieZhanChang::GetInitParam(ActivityData & arg_data)
{
	HuanJieZhanChangActivityData & data = arg_data.huan_jie_activity_data;
	data.season_num = m_season_num;
}

void ActivityHuanJieZhanChang::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->CheckTianTiRecord(role);

		int role_id = role->GetUID();
		ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false, true);
		if (role_data != NULL)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
			if (NULL != node)
			{
				node->GetName(role_data->role_name);				//更新一下排行榜上名字
				role_data->prof = node->GetProfession();
				role_data->avatar_type = node->avatar_type;
			}

			this->UpdateRank(RANK_LIST_HIDDEN_SERVER_ID, ActivityHuanJieZhanChangRankItem(role_id, *role_data));	//更新全服排行
			this->UpdateRank(IntToUid(role_id).db_index, ActivityHuanJieZhanChangRankItem(role_id, *role_data));		//更新玩家自己服排行
		}
	}
}

void ActivityHuanJieZhanChang::OnUserLogout(Role * role)
{
	this->OnRoleCancelMatchReq(role->GetUID());
}

void ActivityHuanJieZhanChang::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
	this->UpdateMatch(now_second);
	this->UpdateReadyFight(now_second);
	this->BuildRankList(now_second);
	this->UpdateSnapshot(now_second);
}

void ActivityHuanJieZhanChang::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);
	switch (to_status)
	{
		case ACTIVITY_STATUS_STANDY:
			{

			}
			break;
		case ACTIVITY_STATUS_OPEN:
			{
				this->ClearMatching();
			}
			break;
		case ACTIVITY_STATUS_CLOSE:
			{
				this->ClearMatching();
			}
			break;
	}
}

void ActivityHuanJieZhanChang::OnDayChange()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::vector< HuanJieTodayWinRewardCfg>  cfgs = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetHuanJieAllTodayWinRewardCfg();

	RetActivityRoleData get_role_data;
	this->BeginRoleDataForeach();
	while (this->GetNextRoleData(get_role_data))
	{
		int role_id = get_role_data.role_id;
		ActivityHuanJieZhanChangRoleData & role_data = get_role_data.data.huan_jie_zhan_chang_role_data;

		for (int i=0;i<(int)cfgs.size();++i)
		{
			const HuanJieTodayWinRewardCfg & curr = cfgs[i];
			if (role_data.today_win_reward_flag & (1 << curr.seq))
			{
				continue;
			}

			const HuanJieTodayWinRewardCfg * cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetHuanJieTodayWinRewardCfg(curr.seq);
			if (cfg == NULL)
			{
				continue;
			}

			if (role_data.today_win_times < cfg->need_win_times)
			{
				continue;
			}

			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = curr.exp;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_COIN] = curr.huan_jie_coin;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = curr.coin;
			::FillMailContentParam(contentparam, curr.rewards);

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_huan_jie_zhan_chang_today_win_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_huan_jie_zhan_chang_today_win_reward_content);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}

		role_data.today_get_huan_jie_coin = 0;
		role_data.today_win_times = 0;
		role_data.today_win_reward_flag = 0;
		this->SetRoleData(role_id, get_role_data.data);
		this->SendRoleInfoByRoleId(role_id);
		this->OnRoleDataChange(role_id, role_data);
	}
}

void ActivityHuanJieZhanChang::OnMonthChange()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_season_end_time = EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	time_t now = EngineAdapter::Instance().Time();
	this->BuildRankList(now, true);

	{
		std::vector<int> vec;
		//发送跨服排名奖励
		const std::vector<ActivityHuanJieZhanChangRankItem> & curr = m_server_rank_list_snapshot[+RANK_LIST_HIDDEN_SERVER_ID];
		for (int rank_index = 0; rank_index < (int)curr.size(); ++rank_index)
		{
			int rank_pos = rank_index + 1;
			const ActivityHuanJieZhanChangRankItem & rank_item = curr[rank_index];

			vec.push_back(rank_item.role_id);

			gamelog::g_log_huan_jie_zhan_chang.printf(LL_INFO, "%s line:%d pai_hang_bang_jie_suan, role:[%d %s] rank_pos:%d score:%d", __FUNCTION__, __LINE__, rank_item.role_id, rank_item.role_name, rank_pos, rank_item.score);

			const ActivityHuanJieZhanChangRankingListCfg * rank_list_cfg_ptr = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRankingListCfg(rank_pos);
			if (rank_list_cfg_ptr != NULL && rank_item.score >= rank_list_cfg_ptr->need_rank_score)
			{
				const ActivityHuanJieZhanChangRankingListCfg & rank_list_cfg = *rank_list_cfg_ptr;
				// 发邮件
				static MailContentParam contentparam; contentparam.Reset();

				int item_count = 0;
				for (int i = 0; i < (int)rank_list_cfg.ranking_reward.size() && item_count < ARRAY_LENGTH(contentparam.item_list); ++i)
				{
					const ItemConfigData & reward = rank_list_cfg.ranking_reward[i];
					const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
					if (item_base != NULL)
					{
						contentparam.item_list[item_count].item_id = reward.item_id;
						contentparam.item_list[item_count].num = reward.num;
						contentparam.item_list[item_count].is_bind = (reward.is_bind) ? 1 : 0;
						contentparam.item_list[item_count].invalid_time = item_base->CalInvalidTime();
						item_count += 1;
					}
				}

				int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_huan_jie_zhan_chang_ranking_list_reward_subject);
				int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_huan_jie_zhan_chang_ranking_list_reward_content, m_season_num, rank_index + 1);
				if (length1 > 0 && length2 > 0)
				{
					MailRoute::MailToUser(rank_item.role_id, MAIL_REASON_DEFAULT, contentparam);
				}
			}

		}

		this->SetSeasonTopRole(vec, true);
	}

	RetActivityRoleData get_role_data;
	this->BeginRoleDataForeach();
	while (this->GetNextRoleData(get_role_data))
	{
		int role_id = get_role_data.role_id;
		ActivityHuanJieZhanChangRoleData & role_data = get_role_data.data.huan_jie_zhan_chang_role_data;
		gamelog::g_log_huan_jie_zhan_chang.printf(LL_INFO, "%s line:%d ge_ren_jie_suan, role:[%d %s] is_finish_ding_ji:%d score:%d", __FUNCTION__, __LINE__, role_id, role_data.role_name, role_data.is_finish_ding_ji, role_data.rank_score);

		if (!role_data.is_finish_ding_ji || role_data.rank_score <= 0)
		{
			continue;
		}

		const ActivityHuanJieZhanChangRankCfg * rank_cfg_ptr = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRankCfg(role_data.rank_score);

		static MailContentParam contentparam; contentparam.Reset();

		if (rank_cfg_ptr != NULL)
		{
			const ActivityHuanJieZhanChangRankCfg & rank_cfg = *rank_cfg_ptr;
			// 发邮件
			int item_count = 0;
			for (int i = 0; i < (int)rank_cfg.settlement_rank_reward.size() && item_count < ARRAY_LENGTH(contentparam.item_list); ++i)
			{
				const ItemConfigData & reward = rank_cfg.settlement_rank_reward[i];
				const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
				if (item_base != NULL)
				{
					contentparam.item_list[item_count].item_id = reward.item_id;
					contentparam.item_list[item_count].num = reward.num;
					contentparam.item_list[item_count].is_bind = (reward.is_bind) ? 1 : 0;
					contentparam.item_list[item_count].invalid_time = item_base->CalInvalidTime();
					item_count += 1;
				}
			}

			//没奖励不发邮件
			if (item_count == 0)
			{
				continue;
			}
		}

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_huan_jie_zhan_chang_settlement_reward_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_huan_jie_zhan_chang_settlement_reward_content, m_season_num, role_data.rank_score);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
		}

		//
		if (rank_cfg_ptr != NULL)
		{
			role_data.rank_score = rank_cfg_ptr->season_reduce_score;			//角色积分
		}

		role_data.max_rank_score = 0;		//当前赛季达到最高排位积分
		role_data.winning_streak_times = 0;  //当前连胜次数
		role_data.total_times = 0;
		role_data.win_times = 0;
		role_data.draw_times = 0;
		role_data.is_finish_ding_ji = 0;			//是否完成了定级赛
		this->SetRoleData(role_id, get_role_data.data);
		this->SendRoleInfoByRoleId(role_id);
		this->OnRoleDataChange(role_id, role_data);
	}

	m_season_num += 1;		//这个活动数据要保存，改动要设置脏
	Activity::SetDirty();

	this->BuildRankList(now, true);

	this->SyncHuanJieActivityStatusHiddenToGameworld();
}


void ActivityHuanJieZhanChang::OnConnectCrossServerSucc()
{
}

void ActivityHuanJieZhanChang::OnUserLoginOriginGame(int role_id)
{
	this->CheckTianTiRecord(role_id);

	ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false, true);
	if (role_data != NULL)
	{
		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL != node)
		{
			node->GetName(role_data->role_name);
			role_data->prof = node->GetProfession();
			role_data->avatar_type = node->avatar_type;
		}

		this->UpdateRank(RANK_LIST_HIDDEN_SERVER_ID, ActivityHuanJieZhanChangRankItem(role_id, *role_data));
		this->UpdateRank(IntToUid(role_id).db_index, ActivityHuanJieZhanChangRankItem(role_id, *role_data));
	}
}

void ActivityHuanJieZhanChang::OnGameServerConnect(std::set<int> server_id_set)
{
	if (server_id_set.empty())
	{
		return;
	}

	int server_id = *server_id_set.begin();
	this->SyncHuanJieActivityStatusHiddenToGameworld();

	//发送给游戏服
	CommonSaveHuanJieData * sd = this->GetSaveData();
	if (sd != NULL)
	{
		crossgameprotocal::HiddenGameHuanJieZhanChangeSyncInfo protocol;
		protocol.is_notice = 0;
		for (int i = 0; i < ARRAY_LENGTH(sd->role_id) && i < ARRAY_LENGTH(protocol.role_id); ++i)
		{
			protocol.role_id[i] = sd->role_id[i];
		}

		InternalComm::Instance().SendToGameThroughCross2((const char *)&protocol, sizeof(protocol), server_id);
	}
}

void ActivityHuanJieZhanChang::OnRoleMatchReq(Role * role)
{
	bool stop_req = false;
	if (stop_req)
	{
		return;
	}

	std::vector<Role *> role_list;	
	Team * team = role->GetMyTeam();
	int leader_uid = 0;
	if (team != NULL)
	{
		leader_uid = team->GetLeaderUID();
		if (leader_uid != role->GetUID())
		{
			return;
		}

		if (team->HasDismissMember())
		{
			role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return;
		}

		int team_member_count = team->GetMemberCount();
		for (int i = 0; i < team_member_count; ++i)
		{
			Role * tmp_role = team->GetMemberRoleByIndex(i);
			if (tmp_role == NULL)
			{
				return;
			}
			if(!team->IsLeader(tmp_role))WorldStatus::Instance().LogFunctionStats(tmp_role, "ActivityHuanJieZhanChang");
			role_list.push_back(tmp_role);
		}
	}
	else
	{
		role_list.push_back(role);
	}
	WorldStatus::Instance().LogFunctionStats(role, "ActivityHuanJieZhanChang");
	if ((int)role_list.size() > ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM)
	{
		role->NoticeNum(errornum::EN_HUAN_JIE_ZHAN_TIAN_TI_MATCH_TOO_MANY_ROLE);
		return;
	}

	std::vector<Role *> notic_list;
	notic_list.push_back(role);

	//const ActivityHuanJieZhanChangOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg();
	for (int i = 0; i < (int)role_list.size(); ++i)
	{
		Role * curr_role = role_list[i];
		int role_id = curr_role->GetUID();
		
		if (!FunOpen::IsFunOpen(curr_role, FUN_TYPE_HUANJIEZHANCHANG))
		{
			role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
			return;
		}

		if (!curr_role->GetRoleModuleManager()->GetHuaShenManager()->IsSeasonInTime())
		{
			::HuanJieZhanChangSendErrNotice(notic_list, curr_role->GetUID(), Protocol::HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_HUA_SHEN);
			return;
		}

		if (!curr_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI))
		{
			::HuanJieZhanChangSendErrNotice(notic_list, curr_role->GetUID(), Protocol::HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_CANN_FIGHT);
			return;
		}

		if (this->GetRoleMatchState(role_id) != ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE)
		{
			::HuanJieZhanChangSendErrNotice(notic_list, curr_role->GetUID(), Protocol::HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_IN_MATCHING);
			return;
		}
	}

	ActivityHuanJieTeamInfo match_info;
	for (int i = 0; i < (int)role_list.size(); ++i)
	{
		Role * curr_role = role_list[i];
		int role_id = curr_role->GetUID();

		const ActivityHuanJieZhanChangRoleData & role_data = *this->GetActRoleData(role_id);
		this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_MATCHING);

		HuanJieZhanChangRoleItem item(curr_role, role_data.rank_score, role_data.is_finish_ding_ji);
		item.is_leader = curr_role->GetUID() == leader_uid ? 1 : 0;
		match_info.role_list.push_back(item);
	}

	m_match_set.push_front(match_info);
}

void ActivityHuanJieZhanChang::OnRoleCancelMatchReq(int arg_role_id)
{
	for (std::list<ActivityHuanJieTeamInfo> ::iterator iter = m_match_set.begin(); iter != m_match_set.end(); ++iter)
	{
		const ActivityHuanJieTeamInfo & curr = *iter;

		bool is_find = false;
		for (int i = 0; i < (int)curr.role_list.size(); ++i)
		{
			int role_id = curr.role_list[i].role_id;
			if (arg_role_id == role_id)
			{
				is_find = true;
				break;
			}
		}

		if (!is_find)
		{
			continue;
		}

		for (int i = 0; i < (int)curr.role_list.size(); ++i)
		{
			int role_id = curr.role_list[i].role_id;
			if (this->GetRoleMatchState(role_id) != ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_MATCHING)
			{
				return;
			}
		}

		for (int i = 0; i < (int)curr.role_list.size(); ++i)
		{
			int role_id = curr.role_list[i].role_id;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE);
		}

		m_match_set.erase(iter);
		break;
	}
}

void ActivityHuanJieZhanChang::QueryRoleRankInfo(Role * role, std::set<int> query_role_id_list)
{
	Protocol::SCHuanJieZhanChangRoleRankListing   protocol;
	this->FillProtocol(&protocol, query_role_id_list);

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.role_infos) - protocol.count) * sizeof(protocol.role_infos[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
}


void ActivityHuanJieZhanChang::HiddenToGameQueryRoleRankInfo(int role_id, std::set<int> query_role_id_list)
{
	Protocol::SCHuanJieZhanChangRoleRankListing   protocol;
	this->FillProtocol(&protocol, query_role_id_list);

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.role_infos) - protocol.count) * sizeof(protocol.role_infos[0]);
	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_len);
}

void ActivityHuanJieZhanChang::HiddenToGameSyncRoleInfo(int role_id, int rank_score, int is_finish_ding_ji)
{
	crossgameprotocal::HiddenGameSyncHuanJieRoleInfo hgshjri;
	hgshjri.role_uid = role_id;
	hgshjri.rank_score = rank_score;
	hgshjri.is_finish_ding_ji = is_finish_ding_ji;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&hgshjri, sizeof(hgshjri));
}

int ActivityHuanJieZhanChang::GetSeasonNum() const
{
	if (!m_has_sync_season_num)
	{
		return 0;
	}

	return m_season_num;
}

void ActivityHuanJieZhanChang::SetSeasonNum(int season_num)
{
	gamelog::g_log_huan_jie_zhan_chang.printf(LL_INFO, "%s line:%d arg:%d now_season_num:%d", __FUNCTION__, __LINE__, season_num, m_season_num);

	m_has_sync_season_num = true;
	m_season_num = season_num;
}

void ActivityHuanJieZhanChang::SyncHuanJieActivityStatusHiddenToGameworld(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameSyncHuanJieActivityStatus msg;
	msg.season_num = m_season_num;

	if (server_id == 0)
	{
		InternalComm::Instance().SendToGameThroughCross(0, 0, (const char *)&msg, sizeof(msg));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&msg, sizeof(msg));
	}
}

void ActivityHuanJieZhanChang::SyncSinleRecordAck(int role_id, int record_id)
{
	CommonSaveKey key(role_id, record_id);
	m_single_record_data->RemoveData(key);
}

void ActivityHuanJieZhanChang::OnRoleDataChange(int role_id, const ActivityHuanJieZhanChangRoleData & role_data)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->SetRoleInfo(role_data.rank_score, role_data.is_finish_ding_ji);
	}
	else
	{
		this->HiddenToGameSyncRoleInfo(role_id, role_data.rank_score, role_data.is_finish_ding_ji);
	}
}

void ActivityHuanJieZhanChang::SetSeasonTopRole(std::vector<int> & vec, bool is_notice)
{
	CommonSaveHuanJieData * sd = this->GetSaveData(true);
	if (sd == NULL)
	{
		return;
	}

	std::set<int> change_list;
	for (int i = 0; i < ARRAY_LENGTH(sd->role_id); ++i)
	{
		change_list.insert(sd->role_id[i]);
		sd->role_id[i] = 0;

		if (i < (int)vec.size())
		{
			sd->role_id[i] = vec[i];
			change_list.insert(sd->role_id[i]);
		}
	}
	m_save_data->Save();


	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameHuanJieZhanChangeSyncInfo protocol;
		protocol.is_notice = 1;
		for (int i = 0; i < ARRAY_LENGTH(sd->role_id) && i < ARRAY_LENGTH(protocol.role_id); ++i)
		{
			protocol.role_id[i] = sd->role_id[i];
		}
		InternalComm::Instance().SendToAllGameThroughCross((const char *)&protocol, sizeof(protocol));
	}

	for (std::set<int>::iterator iter = change_list.begin(); iter != change_list.end(); ++iter)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*iter);
		if (role != NULL)
		{
			role->GetRoleModuleManager()->GetTitle()->CalHuanJieZhanChangeTitle(is_notice ? TITLE_NOTICE_TYPE_DEFAULT : TITLE_NOTICE_TYPE_NO_NOTICE);
		}
	}
}


int ActivityHuanJieZhanChang::GetTopRoleTitle(int role_id)
{
	CommonSaveHuanJieData * sd = this->GetSaveData();
	if (sd == NULL)
	{
		return 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(sd->role_id); ++i)
	{
		if (sd->role_id[i] == role_id)
		{
			return LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRankingTitle(i + 1);
		}
	}

	return 0;
}

void ActivityHuanJieZhanChang::SetRoleMatchState(int role_id, int state)
{
	if (this->IsHuanJieRobotId(role_id))
	{
		return;
	}

	state == ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE ?
		m_role_match_state.erase(role_id) :
		m_role_match_state[role_id] = state;

	this->SendRoleMatchSate(role_id);
}

int ActivityHuanJieZhanChang::GetRoleMatchState(int role_id)
{
	std::map<int, int>::iterator iter = m_role_match_state.find(role_id);

	if (iter == m_role_match_state.end())
	{
		return ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE;
	}

	return iter->second;
}

void ActivityHuanJieZhanChang::SendRoleMatchSate(int role_id)
{
	if (this->IsHuanJieRobotId(role_id))
	{
		return;
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	this->SendRoleMatchSate(role);
}

void ActivityHuanJieZhanChang::SendRoleMatchSate(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangMatchState protocol;
	protocol.match_state = this->GetRoleMatchState(role->GetUID());
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void ActivityHuanJieZhanChang::SendOpponentInfo(const HuanJieZhanChangFightItem & ready_fight, int send_role_id)
{
	Protocol::SCHuanJieZhanChangOpponentInfo protocol;
	protocol.start_time = (unsigned int)ready_fight.start_fight_time;

	std::vector<Role *> send_role_list;
	for (int i = 0; i < (int)ready_fight.attacker.role_list.size() && i < ARRAY_LENGTH(protocol.opponent_info_item1); ++i)
	{
		const HuanJieZhanChangRoleItem & opponent_item = ready_fight.attacker.role_list[i];
		HuanJieZhanChangOpponentInfoItem & item = protocol.opponent_info_item1[i];
		opponent_item.GetData(item);

		if (this->IsHuanJieRobotId(opponent_item.role_id))
		{
			continue;
		}

		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(opponent_item.role_id);
		if (role)
		{
			send_role_list.push_back(role);
		}
	}

	for (int i = 0; i < (int)ready_fight.defender.role_list.size() && i < ARRAY_LENGTH(protocol.opponent_info_item2); ++i)
	{
		const HuanJieZhanChangRoleItem & opponent_item = ready_fight.defender.role_list[i];
		HuanJieZhanChangOpponentInfoItem & item = protocol.opponent_info_item2[i];
		opponent_item.GetData(item);

		if (this->IsHuanJieRobotId(opponent_item.role_id))
		{
			continue;
		}

		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(opponent_item.role_id);
		if (role)
		{
			send_role_list.push_back(role);
		}
	}

	for (int i = 0; i < (int)send_role_list.size(); ++i)
	{
		Role * role = send_role_list[i];
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}		
	}
}

void ActivityHuanJieZhanChang::SetFightResult(int role_id, int result, int fight_round, int fight_seq, bool is_attacker, const std::map<int, ActivityHuanJieZhanChangShangHaiInfo> & shang_hai_info)
{
	if (this->IsHuanJieRobotId(role_id))
	{
		return;
	}

	const ActivityHuanJieZhanChangOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg();

	const ActivityHuanJieZhanChangRoleData & role_data = *this->GetActRoleData(role_id);

	int winning_streak_times = role_data.winning_streak_times;
	int total_times = role_data.total_times + 1;
	int win_times = role_data.win_times;
	int draw_times = role_data.draw_times;
	int curr_rank_score = role_data.rank_score;
	int add_huan_jie_coin = 0;
	bool is_finish_ding_ji = total_times >= other_cfg.ding_ji_sai_times || role_data.is_finish_ding_ji;
	bool is_normal = role_data.is_finish_ding_ji;			//是不是普通赛（不是定级赛）,最后一场也是加的定级赛分
	int add_today_win_times = 0;
	int add_score = 0;
	switch (result)
	{
		case ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_WIN:
			{
				winning_streak_times += 1;
				win_times += 1;
				add_today_win_times = 1;

				if (is_normal)
				{
					winning_streak_times > other_cfg.win_streak_times ?
						add_score = other_cfg.win_add_score + other_cfg.win_streak_add_score :
						add_score = other_cfg.win_add_score;

					std::map<int, HuanJieZhanChangFightItem>::iterator iter = m_fight_item_info.find(fight_seq);
					if (iter != m_fight_item_info.end())
					{
						add_score += LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetExtraScore(iter->second.GetAverageRankScore(is_attacker), iter->second.GetAverageRankScore(!is_attacker));
					}					
				}
				else
				{
					winning_streak_times > other_cfg.win_streak_times ?
						add_score = other_cfg.ding_ji_win_add_score + other_cfg.win_streak_add_score :
						add_score = other_cfg.ding_ji_win_add_score;
				}
			
				add_huan_jie_coin = other_cfg.win_add_huan_jie_coin;
			}
			break;
		case ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL:
			{
				winning_streak_times = 0;
				is_normal ? add_score = other_cfg.lose_add_score : add_score = other_cfg.ding_ji_lose_add_score;

				add_huan_jie_coin = other_cfg.lose_add_huan_jie_coin;
			}
			break;
		case ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW:
			{
				draw_times += 1;
				add_huan_jie_coin = other_cfg.draw_add_huan_jie_coin;
			}
			break;
		default:
			break;
	}

	bool is_protect = false;
	if (is_normal && result == ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL && LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->IsRankProtect(curr_rank_score))
	{
		is_protect = true;
		add_score = 0;
	}

	int real_add_huan_jie_coin = role_data.today_get_huan_jie_coin + add_huan_jie_coin >= other_cfg.today_huan_jie_coin_limit ?
		other_cfg.today_huan_jie_coin_limit - role_data.today_get_huan_jie_coin : add_huan_jie_coin;
	int rank_score = this->AddScore(role_id, add_score, total_times, winning_streak_times, win_times, draw_times, is_finish_ding_ji, real_add_huan_jie_coin, add_today_win_times);

	CommonSaveHuanJieTianTiReceordData record_data;
	record_data.Reset();

	record_data.time = (unsigned int)EngineAdapter::Instance().Time();
	record_data.fight_round = fight_round;
	record_data.fight_result = result;
	record_data.add_score = add_score;
	record_data.add_huan_jie_coin = real_add_huan_jie_coin;
	record_data.is_ding_ji = is_normal ? 0 : 1;
	record_data.is_protect = is_protect ? 1 : 0;

	std::map<int, HuanJieZhanChangFightItem>::iterator iter = m_fight_item_info.find(fight_seq);
	if (iter != m_fight_item_info.end())
	{
		iter->second.GetFightRoleInfo(record_data, shang_hai_info);
	}

	gamelog::g_log_huan_jie_zhan_chang.printf(LL_INFO, "%s line:%d role_id:%d fight_result:%d add_score:%d fight_round:%d is_ding_ji:%d add_huan_jie_coin:%d real_add_huan_jie_coin:%d other_cfg.today_huan_jie_coin_limit:%d",
						  __FUNCTION__, __LINE__, role_id, result, add_score, fight_round, record_data.is_ding_ji, add_huan_jie_coin, real_add_huan_jie_coin, other_cfg.today_huan_jie_coin_limit);

	Protocol::SCHuanJieZhanChangResult protocol;
	protocol.result = result;
	protocol.rank_score = rank_score;
	protocol.win_streak_times = winning_streak_times;
	protocol.is_finish_ding_ji = is_finish_ding_ji;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		role->GetRoleModuleManager()->GetRoleHuanJie()->AddTianTiRecord(record_data);
	}
	else
	{
		this->AddTianTiRecord(role_id, record_data);
	}
}

void ActivityHuanJieZhanChang::RemoveFightItemInfo(int fight_seq)
{
	m_fight_item_info.erase(fight_seq);
}

void ActivityHuanJieZhanChang::SendRoleInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangRoleInfo protocol;
	this->FillProtocol(&protocol, role->GetUID());

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void ActivityHuanJieZhanChang::SendRoleInfoByRoleId(int role_id)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		this->SendRoleInfo(role);
	}
	else
	{
		this->HiddenToGameSendRoleInfo(role_id);
	}
}

void ActivityHuanJieZhanChang::SendRankInfo(Role * role, int server_id)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangRankInfo protocol;
	this->FillProtocol(&protocol, server_id);

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

int ActivityHuanJieZhanChang::AddScore(int role_id, int add_value, int total_times, int winning_streak_times, int win_times, int draw_times, bool is_finish_ding_ji, int add_huan_jie_coin,int add_today_win_times)
{
	if (this->IsHuanJieRobotId(role_id))
	{
		return 0;
	}

	const ActivityHuanJieZhanChangOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg();

	ActivityHuanJieZhanChangRoleData & role_data = *this->GetActRoleData(role_id);
	this->SetRoleDataChange(role_id);
	role_data.last_join_time = (unsigned int)EngineAdapter::Instance().Time();

	if (role_data.rank_score < other_cfg.base_score)
	{
		role_data.rank_score = other_cfg.base_score;
	}


	role_data.total_times = total_times;
	role_data.winning_streak_times = winning_streak_times;
	role_data.win_times = win_times;
	role_data.draw_times = draw_times;
	role_data.is_finish_ding_ji = is_finish_ding_ji;
	role_data.today_get_huan_jie_coin += add_huan_jie_coin;
	role_data.today_win_times += add_today_win_times;

	role_data.rank_score += add_value;

	if (role_data.rank_score < other_cfg.base_score)
	{
		role_data.rank_score = other_cfg.base_score;
	}

	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
	if (NULL != node)
	{
		node->GetName(role_data.role_name);
		role_data.prof = node->GetProfession();
		role_data.avatar_type = node->avatar_type;
	}

	m_need_build_rank = true;
	this->CheckReachedRankReward(role_id, role_data);
	this->UpdateRank(RANK_LIST_HIDDEN_SERVER_ID, ActivityHuanJieZhanChangRankItem(role_id, role_data));
	this->UpdateRank(IntToUid(role_id).db_index, ActivityHuanJieZhanChangRankItem(role_id, role_data));
	this->SendRoleInfoByRoleId(role_id);
	this->OnRoleDataChange(role_id, role_data);

	return role_data.rank_score;
}

int ActivityHuanJieZhanChang::GetRoleScore(int role_id)
{
	ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false);
	if (NULL == role_data)
	{
		return 0;
	}
	return role_data->rank_score;
}

QueryHuanJieInfo ActivityHuanJieZhanChang::GetRoleQueryInfo(int role_id)
{
	QueryHuanJieInfo info;
	ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false);
	if (NULL == role_data)
	{
		return info;
	}
	info.rank_score = role_data->rank_score;
	info.is_finish_ding_ji = role_data->is_finish_ding_ji;
	return info;
}


void ActivityHuanJieZhanChang::FillProtocol(Protocol::SCHuanJieZhanChangRankInfo * protocol_ptr, int server_id)
{
	if (protocol_ptr == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangRankInfo & protocol = *protocol_ptr;

	protocol.server_id = server_id;
	protocol.count = 0;

	std::map<int, std::vector<ActivityHuanJieZhanChangRankItem> >::iterator iter = m_server_rank_list_snapshot.find(server_id);
	if (iter != m_server_rank_list_snapshot.end())
	{
		const std::vector<ActivityHuanJieZhanChangRankItem> & rank_list = iter->second;
		protocol.count = (int)rank_list.size();
		for (int i = 0; i < (int)rank_list.size() && i < ARRAY_LENGTH(protocol.rank_list); ++i)
		{
			protocol.rank_list[i] = rank_list[i];
		}
	}

}

void ActivityHuanJieZhanChang::FillProtocol(Protocol::SCHuanJieZhanChangRoleInfo * protocol_ptr, int role_id)
{
	if (protocol_ptr == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangRoleInfo & protocol = *protocol_ptr;
	protocol.season_num = m_season_num;
	protocol.end_time = (unsigned int)m_season_end_time;
	protocol.max_rank_score = 0;
	protocol.rank_score = 0;
	protocol.win_streak_times = 0;
	protocol.total_times = 0;
	protocol.win_times = 0;
	protocol.draw_times = 0;
	protocol.is_finish_ding_ji = 0;
	protocol.today_win_times = 0;
	protocol.today_win_reward_flag = 0;

	const ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false);
	if (role_data != NULL)
	{
		protocol.rank_score = role_data->rank_score;
		protocol.max_rank_score = role_data->max_rank_score;
		protocol.win_streak_times = role_data->winning_streak_times;
		protocol.total_times = role_data->total_times;
		protocol.win_times = role_data->win_times;
		protocol.draw_times = role_data->draw_times;
		protocol.is_finish_ding_ji = role_data->is_finish_ding_ji;
		protocol.today_get_huan_jie_coin = role_data->today_get_huan_jie_coin;
		protocol.today_win_times = role_data->today_win_times;
		protocol.today_win_reward_flag = role_data->today_win_reward_flag;
	}
}

void ActivityHuanJieZhanChang::FillProtocol(Protocol::SCHuanJieZhanChangRoleRankListing * protocol_ptr, const std::set<int> & query_role_id_list)
{
	if (protocol_ptr == NULL)
	{
		return;
	}

	Protocol::SCHuanJieZhanChangRoleRankListing & protocol = *protocol_ptr;
	protocol.count = 0;

	for (std::set<int>::iterator iter = query_role_id_list.begin(); iter != query_role_id_list.end() && protocol.count < ARRAY_LENGTH(protocol.role_infos); ++iter)
	{
		int query_role_id = *iter;
		Protocol::SCHuanJieZhanChangRoleRankListing::RoleInfoItem & item = protocol.role_infos[protocol.count++];
		item.role_id = query_role_id;

		std::map<int, ActivityHuanJieZhanChangRoleRankInfo>::iterator find_iter = m_role_rank_list_info.find(query_role_id);
		if (find_iter != m_role_rank_list_info.end())
		{
			item.rank_pos = find_iter->second.rank_list_pos;
			item.cross_rank_pos = find_iter->second.hide_rank_list_pos;
		}

		const ActivityHuanJieZhanChangRoleData * role_data = GetActRoleData(query_role_id, false);
		if (role_data != NULL)
		{
			item.score = role_data->rank_score;
			item.join_time = role_data->last_join_time;
			item.is_finish_ding_ji = role_data->is_finish_ding_ji;
		}
	}
}


void ActivityHuanJieZhanChang::HiddenToGameSendRankInfo(int role_id, int server_id)
{
	Protocol::SCHuanJieZhanChangRankInfo protocol;
	this->FillProtocol(&protocol, server_id);

	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
}

void ActivityHuanJieZhanChang::HiddenToGameSendRoleInfo(int role_id)
{
	Protocol::SCHuanJieZhanChangRoleInfo protocol;
	this->FillProtocol(&protocol, role_id);

	InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
}

void ActivityHuanJieZhanChang::AdjustRobotAttr(BattleAttr & attr, int adjust_percent)
{
	if (0 == adjust_percent) return;

	int add_attr = (int)(1.0 * attr * adjust_percent / 10000);
	attr += add_attr;

	GetMax(attr, 1);
}

bool ActivityHuanJieZhanChang::IsHuanJieRobotId(int role_id)
{
	if (IntToUid(role_id).db_index == ROBOT_DB_INDEX)
	{
		return true;
	}

	return false;
}

void ActivityHuanJieZhanChang::FetchTodayWinReward(Role * role, int seq)
{
	if (role == NULL)
	{
		return;
	}
	int role_id = role->GetUID();

	ActivityHuanJieZhanChangRoleData * role_data = this->GetActRoleData(role_id, false, true);
	if (role_data == NULL)
	{
		return;
	}

	if (seq < 0 || seq >= BitCount(role_data->today_win_reward_flag))
	{
		return;
	}

	if (role_data->today_win_reward_flag & (1 << seq))
	{
		return;
	}

	const HuanJieTodayWinRewardCfg * cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetHuanJieTodayWinRewardCfg(seq);
	if (cfg == NULL)
	{
		return;
	}

	if (role_data->today_win_times < cfg->need_win_times)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_HUAN_JIE_TODAY_WIN_REWARD))
	{
		return;
	}

	if (cfg->exp > 0)role->AddExp(cfg->exp);
	if (cfg->coin > 0)role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
	if (cfg->huan_jie_coin > 0)role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->huan_jie_coin, MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN, __FUNCTION__);

	role_data->today_win_reward_flag |= (1 << seq);
	this->SendRoleInfo(role);
}

void ActivityHuanJieZhanChang::GmOp(Role * role, int op_type, int p1, int p2)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
		return;
	}

	if (op_type == 1)
	{
		int add_value = p1;
		int role_id = p2;
		if (role_id == 0)
		{
			role_id = role->GetUID();
		}

		const ActivityHuanJieZhanChangRoleData & role_data = *this->GetActRoleData(role_id);
		this->AddScore(role_id, add_value, role_data.total_times, role_data.winning_streak_times, role_data.win_times, role_data.draw_times, (0 != role_data.is_finish_ding_ji), 0,0);
	}


	if (op_type == 10)
	{
		if (p1 == 0)
		{
			this->OnMonthChange();
		}

		if (p1 == 1)
		{
			this->BuildRankList(EngineAdapter::Instance().Time(), true);
		}

		if (p1 == 2)
		{
			this->UpdateSnapshot(EngineAdapter::Instance().Time(), true);
		}
	}

	if (op_type == 11)
	{
		this->ClearMatching();
	}



}

void ActivityHuanJieZhanChang::ClearMatching()
{
	for (std::list<ActivityHuanJieTeamInfo> ::iterator iter = m_match_set.begin(); iter != m_match_set.end(); ++iter)
	{
		const ActivityHuanJieTeamInfo & curr = *iter;
		for (int i = 0; i < (int)curr.role_list.size(); ++i)
		{
			int role_id = curr.role_list[i].role_id;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE);
		}		
	}

	m_match_set.clear();
}

void ActivityHuanJieZhanChang::BuildRankList(time_t now_second, bool is_force)
{
	if (!is_force)
	{
		if (!m_need_build_rank || now_second < m_last_rebuild_rank_time + 15 * 60)
		{
			return;
		}
	}

	m_last_rebuild_rank_time = now_second;
	m_need_build_rank = false;

	m_server_rank_list.clear();
	m_rank_list_role_to_pos.clear();

	RetActivityRoleData get_role_data;
	this->BeginRoleDataForeach();
	while (this->GetNextRoleData(get_role_data))
	{
		int role_id = get_role_data.role_id;
		int server_id = IntToUid(role_id).db_index;
		const ActivityHuanJieZhanChangRoleData & role_data = get_role_data.data.huan_jie_zhan_chang_role_data;
		ActivityHuanJieZhanChangRankItem rank_item(role_id, role_data);
		if (rank_item.score <= 0)
		{
			continue;
		}

		m_server_rank_list[+RANK_LIST_HIDDEN_SERVER_ID].push_back(rank_item);    //隐藏服排行榜
		m_server_rank_list[server_id].push_back(rank_item);  //所在服排行榜
	}

	for (std::map<int, std::vector<ActivityHuanJieZhanChangRankItem> >::iterator iter = m_server_rank_list.begin(); iter != m_server_rank_list.end(); ++iter)
	{
		int server_id = iter->first;
		std::vector<ActivityHuanJieZhanChangRankItem> & curr = iter->second;
		std::vector<ActivityHuanJieZhanChangRankItem> temp = curr;
		std::sort(temp.begin(), temp.end());

		(int)temp.size() > ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM ?
			curr.assign(temp.begin(), temp.begin() + ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM) :
			curr.assign(temp.begin(), temp.end());

		for (int i = 0; i < (int)curr.size(); ++i)
		{
			const ActivityHuanJieZhanChangRankItem & rank_item = curr[i];
			this->SetUserRecordIndex(server_id, rank_item.role_id, i + 1);
		}
	}

	this->UpdateSnapshot(now_second, true);
}

void ActivityHuanJieZhanChang::UpdateRank(int server_id, const ActivityHuanJieZhanChangRankItem & rank_item)
{
	if (rank_item.score <= 0)
	{
		return;
	}

	// 1：先查询是否排行榜上 如果是 直接更新即可 
	// 2：如果不是 则取得当前玩家的信息跟排行榜上最后一名的玩家比较 是否淘汰最后一名的的玩家

	std::vector<ActivityHuanJieZhanChangRankItem> & rank_list = m_server_rank_list[server_id];
	int find_index = this->GetUserRecordIndex(server_id, rank_item.role_id);
	if (0 <= find_index && find_index < (int)rank_list.size())
	{
		if (!(rank_list[find_index] < rank_item || rank_item < rank_list[find_index]))
		{
			return;
		}

		m_need_snapshot = true;
		rank_list[find_index] = rank_item;
	}
	else
	{
		if ((int)rank_list.size() < ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM)
		{
			m_need_snapshot = true;
			rank_list.push_back(rank_item);
			this->SetUserRecordIndex(server_id, rank_item.role_id, (int)rank_list.size());
			return;
		}

		int min_index = -1;
		for (int i = 0; i < (int)rank_list.size(); ++i)
		{
			if (min_index == -1 || rank_list[min_index] < rank_list[i])
			{
				min_index = i;
			}
		}

		if (min_index != -1 && rank_item < rank_list[min_index])
		{
			m_need_snapshot = true;
			this->SetUserRecordIndex(server_id, rank_item.role_id, min_index + 1);
			this->SetUserRecordIndex(server_id, rank_list[min_index].role_id, 0);
			rank_list[min_index] = rank_item;
		}
	}
}

void ActivityHuanJieZhanChang::UpdateSnapshot(time_t now_second, bool is_force)
{
	if (!is_force)
	{
		if (!m_need_snapshot || now_second < m_last_rank_snapshot_time + 30)
		{
			return;
		}
	}

	m_need_snapshot = false;
	m_last_rank_snapshot_time = now_second;

	m_server_rank_list_snapshot = m_server_rank_list;
	m_role_rank_list_info.clear();
	for (std::map<int, std::vector<ActivityHuanJieZhanChangRankItem> >::iterator iter = m_server_rank_list_snapshot.begin(); iter != m_server_rank_list_snapshot.end(); ++iter)
	{
		int server_id = iter->first;
		std::vector<ActivityHuanJieZhanChangRankItem> & curr = iter->second;
		std::sort(curr.begin(), curr.end());

		for (int i = 0; i < (int)curr.size(); ++i)
		{
			const ActivityHuanJieZhanChangRankItem & rank_item = curr[i];
			server_id == RANK_LIST_HIDDEN_SERVER_ID ? m_role_rank_list_info[rank_item.role_id].hide_rank_list_pos = i + 1 :
				m_role_rank_list_info[rank_item.role_id].rank_list_pos = i + 1;
		}
	}

}

void ActivityHuanJieZhanChang::UpdateReadyFight(time_t now_second)
{
	for (std::list<HuanJieZhanChangFightItem>::iterator iter = m_ready_fight_list.begin(); iter != m_ready_fight_list.end(); )
	{
		const HuanJieZhanChangFightItem & ready_fight = *iter;

		if (now_second < ready_fight.start_fight_time)
		{
			++iter;
			continue;
		}

		std::vector<Role *> attack_list;
		std::vector<Role *> defender_list;
		std::vector<RoleBattleData> attacker_robot_list;
		std::vector<RoleBattleData> defender_robot_list;
		std::vector<Role *> all_list;

		bool is_succ = true;
		for (int i = 0; i < (int)ready_fight.attacker.role_list.size(); ++i)
		{
			const HuanJieZhanChangRoleItem & opponent_item = ready_fight.attacker.role_list[i];
			int role_id = opponent_item.role_id;
			if (this->IsHuanJieRobotId(role_id))
			{
				attacker_robot_list.push_back(ready_fight.attacker.role_list[i].robot_battle_data);
				continue;
			}


			bool is_leader = opponent_item.is_leader;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE);

			Role * curr_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (curr_role == NULL)
			{
				is_succ = false;
				continue;
			}

			if (!curr_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI))
			{
				is_succ = false;
				continue;
			}

			attack_list.push_back(curr_role);
			all_list.push_back(curr_role);

			if (is_leader && attack_list.size() > 1)
			{
				std::swap(attack_list[0], attack_list[attack_list.size() - 1]);
			}
		}

		for (int i = 0; i < (int)ready_fight.defender.role_list.size(); ++i)
		{
			const HuanJieZhanChangRoleItem & opponent_item = ready_fight.defender.role_list[i];

			int role_id = opponent_item.role_id;
			if (this->IsHuanJieRobotId(role_id))
			{
				defender_robot_list.push_back(ready_fight.defender.role_list[i].robot_battle_data);
				continue;
			}

			bool is_leader = opponent_item.is_leader;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE);
			Role * curr_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (curr_role == NULL)
			{
				is_succ = false;
				continue;
			}

			if (!curr_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI) || curr_role->GetRoleStatusManager()->IsInBattleStatus())
			{
				is_succ = false;
				continue;
			}

			defender_list.push_back(curr_role);
			all_list.push_back(curr_role);

			if (is_leader && attack_list.size() > 1)
			{
				std::swap(attack_list[0], attack_list[attack_list.size() - 1]);
			}
		}

		int fight_seq = this->GetFightSeq();
		is_succ = is_succ && BattleManagerLocal::GetInstance().StartHuanJieZhanChangPVPReq(attack_list, defender_list, attacker_robot_list, defender_robot_list, LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg().max_battle_rounds, fight_seq);
		if (is_succ)
		{

			for (int i = 0; i < (int)all_list.size(); ++i)
			{
				Role * temp_role = all_list[i];
				if (temp_role)
				{
					EventHandler::Instance().OnParticipateLimitActivity(temp_role, ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG, __FUNCTION__);
				}		
			}

			MapAddOrUpdate(m_fight_item_info, fight_seq, ready_fight);
		}
		else
		{
			//没能成功开始，放回匹配队列
			
			for (int i = 0; i < (int)all_list.size(); ++i)
			{
				Role * curr_role = all_list[i];
				if (!curr_role)
				{
					continue;
				}

				curr_role->NoticeNum(errornum::EN_HUAN_JIE_ZHAN_CHANG_CANNT_FIGHT);
				this->OnRoleMatchReq(curr_role);
			}
		}

		m_ready_fight_list.erase(iter++);  
	}
}

void ActivityHuanJieZhanChang::UpdateMatch(time_t now_second)
{
	if (!this->IsActivityOpen())
	{
		return;
	}

	bool is_up = true;
	if (!is_up)
	{
		return;
	}

	if (now_second < m_last_update_match_time + 7)
	{
		return;
	}
	m_last_update_match_time = now_second;

	this->MatchHelp1(true);
}

bool ActivityHuanJieZhanChangTeamInfoIsEmpty(const std::map<int, std::vector<ActivityHuanJieTeamInfo> > & role_num_to_team_info)
{
	for (std::map<int, std::vector<ActivityHuanJieTeamInfo> >::const_iterator iter = role_num_to_team_info.begin(); iter != role_num_to_team_info.end(); ++iter)
	{
		if (!iter->second.empty())
		{
			return false;
		}
	}

	return true;
}

bool ActivityHuanJieTeamInfoSort1(const ActivityHuanJieTeamInfo & item1, const ActivityHuanJieTeamInfo & item2)
{
	if (item1.GetAverageRankScore() > item2.GetAverageRankScore())
		return false;
	if (item1.GetAverageRankScore() < item2.GetAverageRankScore())
		return true;

	return false;
}

bool ActivityHuanJieTeamInfoSort2(const ActivityHuanJieTeamInfo & item1, const ActivityHuanJieTeamInfo & item2)
{
	int item1_highest_score = item1.GetHighestRankScore();
	int item2_highest_score = item2.GetHighestRankScore();
	return item1_highest_score > item2_highest_score;
}

bool ActivityHuanJieTeamInfoSort3(const ActivityHuanJieTeamInfo & item1, const ActivityHuanJieTeamInfo & item2)
{
	if (item1.GetRoleNum() != item2.GetRoleNum())
	{
		return item1.GetRoleNum() > item2.GetRoleNum();
	}

	int item1_highest_score = item1.GetHighestRankScore();
	int item2_highest_score = item2.GetHighestRankScore();
	return item1_highest_score > item2_highest_score;
}

int GetTeamListRoleNum(const std::vector<ActivityHuanJieTeamInfo>& teamlist)
{
	int num = 0;
	for (size_t i = 0; i < teamlist.size(); ++i)
	{
		num += static_cast<int>(teamlist[i].role_list.size());
	}

	return num;
}

bool AssembleAttackerAndDefenderTeam(std::vector<ActivityHuanJieTeamInfo>& teamlist, ARG_OUT ActivityHuanJieTeamInfo& attacker, ARG_OUT ActivityHuanJieTeamInfo& defender)
{
	UNSTD_STATIC_CHECK(3 == ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM);

#ifdef _DEBUG
	assert(teamlist.size() >= 2);
#endif

	if (teamlist.size() < 2) return false;

	// 先按人数，再按最高分排序
	std::sort(teamlist.begin(), teamlist.end(), ActivityHuanJieTeamInfoSort3);

	// 排序后，一共只有这些情况
	// 3 3
	// 3 2 1
	// 3 1 1 1
	// 2 2 1 1
	// 2 1 1 1 1
	// 1 1 1 1 1 1

	bool result = true;
	if (2 == teamlist.size())
	{
		// 3 3
		result = result && attacker.AddTeamInfo(teamlist[0]);
		result = result && defender.AddTeamInfo(teamlist[1]);
	}
	else if (3 == teamlist.size())
	{
		// 3 2 1
		result = result && attacker.AddTeamInfo(teamlist[0]);
		result = result && defender.AddTeamInfo(teamlist[1]);
		result = result && defender.AddTeamInfo(teamlist[2]);
	}
	else if (4 == teamlist.size())
	{
		if (teamlist[0].GetRoleNum() == 3)
		{
			// 3 1 1 1
			result = result && attacker.AddTeamInfo(teamlist[0]);
			result = result && defender.AddTeamInfo(teamlist[1]);
			result = result && defender.AddTeamInfo(teamlist[2]);
			result = result && defender.AddTeamInfo(teamlist[3]);
		}
		else
		{
			// 2 2 1 1 -> ATK DEF DEF ATK
			result = result && attacker.AddTeamInfo(teamlist[0]);
			result = result && defender.AddTeamInfo(teamlist[1]);
			result = result && defender.AddTeamInfo(teamlist[2]);
			result = result && attacker.AddTeamInfo(teamlist[3]);
		}
	}
	else if (5 == teamlist.size())
	{
		// 2 1 1 1 1 
		// 由于2人组的最高战力可能排在后面，所以随机匹配后三个任意一个作为队友
		int r = rand() % 3;
		if (0 == r)
		{
			// 2 1 1 1 1 -> ATK DEF ATK DEF DEF
			result = result && attacker.AddTeamInfo(teamlist[0]);
			result = result && defender.AddTeamInfo(teamlist[1]);
			result = result && attacker.AddTeamInfo(teamlist[2]);
			result = result && defender.AddTeamInfo(teamlist[3]);
			result = result && defender.AddTeamInfo(teamlist[4]);
		}
		else if (1 == r)
		{
			// 2 1 1 1 1 -> ATK DEF DEF ATK DEF
			result = result && attacker.AddTeamInfo(teamlist[0]);
			result = result && defender.AddTeamInfo(teamlist[1]);
			result = result && defender.AddTeamInfo(teamlist[2]);
			result = result && attacker.AddTeamInfo(teamlist[3]);
			result = result && defender.AddTeamInfo(teamlist[4]);
		}
		else
		{
			// 2 1 1 1 1 -> ATK DEF DEF DEF ATK
			result = result && attacker.AddTeamInfo(teamlist[0]);
			result = result && defender.AddTeamInfo(teamlist[1]);
			result = result && defender.AddTeamInfo(teamlist[2]);
			result = result && defender.AddTeamInfo(teamlist[3]);
			result = result && attacker.AddTeamInfo(teamlist[4]);
		}
	}
	else
	{
		// 1 1 1 1 1 1 -> ATK DEF DEF ATK ATK DEF
		// 这个排序是策划定的最完美的排序
		result = result && attacker.AddTeamInfo(teamlist[0]);
		result = result && defender.AddTeamInfo(teamlist[1]);
		result = result && defender.AddTeamInfo(teamlist[2]);
		result = result && attacker.AddTeamInfo(teamlist[3]);
		result = result && attacker.AddTeamInfo(teamlist[4]);
		result = result && defender.AddTeamInfo(teamlist[5]);
	}

#ifdef _DEBUG
	assert(result);
#endif

	// 概率攻守交换
	if (rand() % 2 == 0)
	{
		std::swap(attacker, defender);
	}

	return result;
}

void ActivityHuanJieZhanChang::MatchHelp1(bool is_log /* = false */)
{
	// 首先计算小队的匹配区间
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	for (std::list<ActivityHuanJieTeamInfo>::iterator iter = m_match_set.begin(); iter != m_match_set.end(); ++iter)
	{
		ActivityHuanJieTeamInfo& info = *iter;
		info.CalcMatchRankSection(now);
	}

	std::map<int, std::vector<ActivityHuanJieTeamInfo> > role_num_to_team_info;
	std::map<int, int> has_role_num;

	for (int rank_type = HUAN_JIE_BIG_RANK_TYPE_MAX - 1; rank_type > HUAN_JIE_BIG_RANK_TYPE_INVALID; --rank_type)
	{
		for (std::list<ActivityHuanJieTeamInfo>::iterator iter = m_match_set.begin(); iter != m_match_set.end();)
		{
			const ActivityHuanJieTeamInfo & curr = *iter;
			if (rank_type >= curr.match_rank_min && rank_type <= curr.match_rank_max)
			{
				int role_num_group = (int)curr.role_list.size();
				role_num_to_team_info[role_num_group].push_back(curr);
				has_role_num[role_num_group] += 1;
				iter = m_match_set.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		for (std::map<int, std::vector<ActivityHuanJieTeamInfo> >::iterator iter = role_num_to_team_info.begin(); iter != role_num_to_team_info.end(); ++iter)
		{
			std::sort(iter->second.begin(), iter->second.end(), ActivityHuanJieTeamInfoSort2);
		}

		const int need_group_num = 2;
		for (std::vector< ActivityHuanMatchRoleNumRequireItem> item_vec = m_require_role_num.GetActivityHuanMatchRoleNumRequireItem(need_group_num, has_role_num);
			(int)item_vec.size() == need_group_num; item_vec = m_require_role_num.GetActivityHuanMatchRoleNumRequireItem(need_group_num, has_role_num))
		{
			std::vector<ActivityHuanJieTeamInfo> teamlist;
			teamlist.reserve(item_vec[0].need_role_num.size() + item_vec[1].need_role_num.size());
			
			const std::map<int, int> & attack_require_map = item_vec[0].need_role_num;					//need_group_num 是2
			for (std::map<int, int>::const_iterator iter = attack_require_map.begin(); iter != attack_require_map.end(); ++iter)
			{
				for (int i = 0; i < iter->second && !role_num_to_team_info[iter->first].empty(); ++i)
				{
					teamlist.push_back(role_num_to_team_info[iter->first].back());
					role_num_to_team_info[iter->first].pop_back();
					has_role_num[iter->first] -= 1;
				}
			}
	
			const std::map<int, int> & defender_require_map = item_vec[1].need_role_num;					//need_group_num 是2
			for (std::map<int, int>::const_iterator iter = defender_require_map.begin(); iter != defender_require_map.end(); ++iter)
			{
				for (int i = 0; i < iter->second && !role_num_to_team_info[iter->first].empty(); ++i)
				{
					teamlist.push_back(role_num_to_team_info[iter->first].back());
					role_num_to_team_info[iter->first].pop_back();
					has_role_num[iter->first] -= 1;
				}
			}

#ifdef _DEBUG
			assert(ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM * 2 == GetTeamListRoleNum(teamlist));
#endif

			ActivityHuanJieTeamInfo attacker_item;
			ActivityHuanJieTeamInfo defender_item;
			if (!AssembleAttackerAndDefenderTeam(teamlist, attacker_item, defender_item))
			{
#ifdef _DEBUG
				assert(0);
#endif

				// 没有成功组成合法的攻方和守方，把队伍重新加回去
				for (size_t i = 0; i < teamlist.size(); ++i)
				{
					role_num_to_team_info[teamlist[i].GetRoleNum()].push_back(teamlist[i]);
					has_role_num[teamlist[i].GetRoleNum()] += 1;
				}
				continue;
			}

			HuanJieZhanChangFightItem ready_fight(attacker_item, defender_item);
			m_ready_fight_list.push_front(ready_fight);

			for (int i = 0; i < (int)attacker_item.role_list.size(); ++i)
			{
				int role_id = attacker_item.role_list[i].role_id;
				this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_READY_FIHGT);
			}

			for (int i = 0; i < (int)defender_item.role_list.size(); ++i)
			{
				int role_id = defender_item.role_list[i].role_id;
				this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_READY_FIHGT);
			}

			this->SendOpponentInfo(ready_fight);

			if (is_log)
			{
				ready_fight.MakeLog();
			}		
		}
	}

	m_match_set.clear();

	std::map<int, std::vector<ActivityHuanJieTeamInfo> > need_robot_role_num_to_team_info;  //匹配超过20秒的人需要匹配机器人
	std::map<int, int> need_robot_has_role_num;
		
	//匹配剩余的人，看有没有超过40秒的，超过就给他们匹配机器人，剩下放回匹配队列
	for (std::map<int, std::vector<ActivityHuanJieTeamInfo> >::iterator iter = role_num_to_team_info.begin(); iter != role_num_to_team_info.end(); ++iter)
	{
		for (std::vector<ActivityHuanJieTeamInfo>::iterator iter_vec = iter->second.begin(); iter_vec != iter->second.end(); ++iter_vec)
		{
			const ActivityHuanJieTeamInfo & curr = *iter_vec;
			if (now > iter_vec->begin_time + 15)
			{
				int role_num_group = (int)curr.role_list.size();
				need_robot_role_num_to_team_info[role_num_group].push_back(curr);
				need_robot_has_role_num[role_num_group] += 1;
			}
			else
			{
				m_match_set.push_front(curr);
			}
		}
	}

	
	this->MatchRobot(need_robot_role_num_to_team_info, is_log);
}

void ActivityHuanJieZhanChang::MatchRobot(std::map<int, std::vector<ActivityHuanJieTeamInfo> > & need_robot_role_num_to_team_info, bool is_log)
{
	bool has_need_match_robot = !::ActivityHuanJieZhanChangTeamInfoIsEmpty(need_robot_role_num_to_team_info);
	if (!has_need_match_robot)
	{
		return;
	}

	static int ROBOT_ROLE_ID = 100;		//战斗服那边机器人id也会判断是不是在战斗中，所以这个要分配不同id

	std::vector<const UserOfflineBattleData *> robot_battle_data_vec = OfflineBattleDataMgr::Instance().GetAllUserOfflineBattleData();
	std::random_shuffle(robot_battle_data_vec.begin(), robot_battle_data_vec.end());

	int get_index = 0;
	const int battle_data_size = (int)robot_battle_data_vec.size();
	std::vector<ActivityHuanJieTeamInfo>  fail_team_info;
	while (!::ActivityHuanJieZhanChangTeamInfoIsEmpty(need_robot_role_num_to_team_info))
	{
		std::vector<ActivityHuanJieTeamInfo>  combine_team_info;	 
		ActivityHuanJieTeamInfo attack_item_lack = this->GetMaxRoleNumTeamInfo(need_robot_role_num_to_team_info, &combine_team_info);
		int attack_lack_num = attack_item_lack.GetLackRoleNum();
		int role_leader_server_id = attack_item_lack.GetRoleLeaderServerId();

		int role_average_level = attack_item_lack.GetAverageLevel();
		int robot_score = attack_item_lack.GetAverageRankScore();
		bool has_ding_ji = attack_item_lack.HasRoleDingJi();
		std::set<int> check_repeat = attack_item_lack.GetRoleIdList();

		int find_role_level_min = role_average_level - 3;
		int find_role_level_max = role_average_level + 3;
		
		for (int i = 0, find_num = 0; i < battle_data_size && find_num < attack_lack_num; ++i)
		{
			const UserOfflineBattleData * curr = robot_battle_data_vec[get_index++ % battle_data_size];
			RoleBattleData robot_battle_data = curr->rbd;

			if (check_repeat.find(robot_battle_data.role_character_data.character_id) != check_repeat.end())
			{
				continue;
			}
			check_repeat.insert(robot_battle_data.role_character_data.character_id);

			if (find_role_level_min <= robot_battle_data.role_character_data.level && robot_battle_data.role_character_data.level <= find_role_level_max)
			{
				robot_battle_data.role_character_data.character_id = UidToInt(UserID(ROBOT_DB_INDEX, ROBOT_ROLE_ID++));
				std::string name_temp = robot_battle_data.role_character_data.name;	//SNPRINTF目标和源不能相同

				if (!CheckNameHasServerSuffix(name_temp))
				{
					SNPRINTF(robot_battle_data.role_character_data.name, sizeof(robot_battle_data.role_character_data.name), "%s_s%d", name_temp.c_str(), role_leader_server_id);
				}				

				robot_battle_data.partner_num = 0;
				ArrayCallReset(robot_battle_data.partner_character_data);
				this->AdjustRobotAttr(&robot_battle_data);

				HuanJieZhanChangRoleItem item(robot_battle_data, role_leader_server_id, robot_score, !has_ding_ji);
				attack_item_lack.AddRoleItem(item);

				find_num += 1;
			}
		}

		ActivityHuanJieTeamInfo defender_item_lack;
		int defender_lack_num = defender_item_lack.GetLackRoleNum();

		for (int i = 0, find_num = 0; i < battle_data_size && find_num < defender_lack_num; ++i)
		{
			const UserOfflineBattleData * curr = robot_battle_data_vec[get_index++ % battle_data_size];
			RoleBattleData robot_battle_data = curr->rbd;

			if (check_repeat.find(robot_battle_data.role_character_data.character_id) != check_repeat.end())
			{
				continue;
			}
			check_repeat.insert(robot_battle_data.role_character_data.character_id);


			if (find_role_level_min <= robot_battle_data.role_character_data.level && robot_battle_data.role_character_data.level <= find_role_level_max)
			{
				robot_battle_data.role_character_data.character_id = UidToInt(UserID(ROBOT_DB_INDEX, ROBOT_ROLE_ID++));
				std::string name_temp = robot_battle_data.role_character_data.name;	//SNPRINTF目标和源不能相同
				if (!CheckNameHasServerSuffix(name_temp))
				{
					SNPRINTF(robot_battle_data.role_character_data.name, sizeof(robot_battle_data.role_character_data.name), "%s_s%d", name_temp.c_str(), role_leader_server_id);
				}

				robot_battle_data.partner_num = 0;
				ArrayCallReset(robot_battle_data.partner_character_data);
				this->AdjustRobotAttr(&robot_battle_data);

				HuanJieZhanChangRoleItem item(robot_battle_data, role_leader_server_id, robot_score, !has_ding_ji);
				defender_item_lack.AddRoleItem(item);

				find_num += 1;
			}
		}

		//现在用的机器人战斗数据不是读表生成，可能找不到需要机器人，需要将这些失败的真人玩家放回匹配队列
		if (attack_item_lack.GetLackRoleNum() != 0 || defender_item_lack.GetLackRoleNum() != 0)
		{
			fail_team_info.insert(fail_team_info.end(), combine_team_info.begin(), combine_team_info.end());
			continue;
		}

		HuanJieZhanChangFightItem ready_fight(attack_item_lack, defender_item_lack);
		m_ready_fight_list.push_front(ready_fight);

		for (int i = 0; i < (int)attack_item_lack.role_list.size(); ++i)
		{
			int role_id = attack_item_lack.role_list[i].role_id;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_READY_FIHGT);
		}

		for (int i = 0; i < (int)defender_item_lack.role_list.size(); ++i)
		{
			int role_id = defender_item_lack.role_list[i].role_id;
			this->SetRoleMatchState(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_READY_FIHGT);
		}

		this->SendOpponentInfo(ready_fight);

		if (is_log)
		{
			ready_fight.MakeLog();
		}
	}

	for (int i = 0; i < (int)fail_team_info.size(); ++i)
	{
		m_match_set.push_front(fail_team_info[i]);
	}
}

void ActivityHuanJieZhanChang::AdjustRobotAttr(RoleBattleData * role_battle_data)
{
	if (!role_battle_data)
	{
		return;
	}
	//暂改公式:属性*（40+（当前机器人等级-70））
	const ActivityHuanJieZhanChangOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetOtherCfg();

	{
		BattleCharacterData & role_data = role_battle_data->role_character_data;
		const ActivityHuanJieZhanChangRobotAttrLimitCfg * limit_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRobotAttrLimitCfg(HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE_ROLE, role_battle_data->appearance.prof_base);
		if (limit_cfg)
		{
			for (int i = 0; i < (int)limit_cfg->max_attr_cfg.size(); ++i)
			{
				const AttrItemCfg & curr = limit_cfg->max_attr_cfg[i];
				if (curr.attr_type < 0 || ARRAY_LENGTH(role_data.attr_list) <= curr.attr_type)
				{
					continue;
				}

				int cur_value = curr.attr_value * (1.0 * (40 + this->GetPlusNumber(role_data.level - other_cfg.level_limit)) / 100);
				role_data.attr_list[curr.attr_type] = std::min(role_data.attr_list[curr.attr_type], cur_value);
			}
		}
		role_data.level = other_cfg.level_limit;
	}

	{

		for (int pet_i = 0; pet_i < ARRAY_LENGTH(role_battle_data->pet_character_data); ++pet_i)
		{
			BattleCharacterData & pet_data = role_battle_data->pet_character_data[pet_i].battle_data;
			const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_data.character_id);
			if (!pet_cfg)
			{
				continue;
			}

			const ActivityHuanJieZhanChangRobotAttrLimitCfg * limit_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRobotAttrLimitCfg(HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE_PET, pet_cfg->main_attribute_id);
			if (limit_cfg)
			{
				for (int i = 0; i < (int)limit_cfg->max_attr_cfg.size(); ++i)
				{
					const AttrItemCfg & curr = limit_cfg->max_attr_cfg[i];
					if (curr.attr_type < 0 || ARRAY_LENGTH(pet_data.attr_list) <= curr.attr_type)
					{
						continue;
					}

					int cur_value = curr.attr_value * (1.0 * (40 + this->GetPlusNumber(pet_data.level - other_cfg.level_limit)) / 100);
					pet_data.attr_list[curr.attr_type] = std::min(pet_data.attr_list[curr.attr_type], cur_value);
				}
			}
			pet_data.level = other_cfg.level_limit;
		}		
	}

}

int ActivityHuanJieZhanChang::GetPlusNumber(int value)
{
	if (value < 0)
	{
		value = 0;
	}

	return value;
}

ActivityHuanJieTeamInfo ActivityHuanJieZhanChang::GetMaxRoleNumTeamInfo(std::map<int, std::vector<ActivityHuanJieTeamInfo> > & role_num_to_team_info, std::vector<ActivityHuanJieTeamInfo> ARG_OUT * combine_team_info)
{
	ActivityHuanJieTeamInfo ret;

	for (std::map<int, std::vector<ActivityHuanJieTeamInfo> >::reverse_iterator iter = role_num_to_team_info.rbegin(); iter != role_num_to_team_info.rend(); ++iter)
	{
		std::vector<ActivityHuanJieTeamInfo> & curr_vec = iter->second;

		while (!curr_vec.empty())
		{
			ActivityHuanJieTeamInfo & last_team_info = curr_vec.back();
			if (ret.AddTeamInfo(last_team_info))
			{
				if (combine_team_info)
				{
					combine_team_info->push_back(last_team_info);
				}

				curr_vec.pop_back();
			}
			else
			{
				break;
			}
		}
	}

	return ret;
}


void ActivityHuanJieZhanChang::CheckReachedRankReward(int role_id, ActivityHuanJieZhanChangRoleData & role_data)
{
	if (!role_data.is_finish_ding_ji || role_data.max_rank_score >= role_data.rank_score)
	{
		return;
	}
	int temp_score = role_data.max_rank_score;
	role_data.max_rank_score = role_data.rank_score;

	const std::vector<ActivityHuanJieZhanChangRankCfg> & all_rank_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetAllRankCfg();
	for (int rank_cfg_index = 0; rank_cfg_index < (int)all_rank_cfg.size(); ++rank_cfg_index)
	{
		const ActivityHuanJieZhanChangRankCfg & curr = all_rank_cfg[rank_cfg_index];
		if (temp_score < curr.need_rank_score && curr.need_rank_score <= role_data.max_rank_score)
		{
			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();

			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_COIN] = curr.rank_reward_huan_jie_coin;

			int item_count = 0;
			for (int i = 0; i < (int)curr.rank_reward.size() && item_count < ARRAY_LENGTH(contentparam.item_list); ++i)
			{
				const ItemConfigData & reward = curr.rank_reward[i];
				const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
				if (item_base != NULL)
				{
					contentparam.item_list[item_count].item_id = reward.item_id;
					contentparam.item_list[item_count].num = reward.num;
					contentparam.item_list[item_count].is_bind = (reward.is_bind) ? 1 : 0;
					contentparam.item_list[item_count].invalid_time = item_base->CalInvalidTime();
					item_count += 1;
				}
			}

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_huan_jie_zhan_chang_rank_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_huan_jie_zhan_chang_rank_reward_content, curr.need_rank_score);
			if (item_count > 0 && length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
}

ActivityHuanJieZhanChangRoleData * ActivityHuanJieZhanChang::GetActRoleData(int role_id, bool is_auto_create, bool set_dirty)
{
	ActivityRoleData * role_data = this->GetRoleData(role_id, is_auto_create);
	if (role_data != NULL)
	{
		if (set_dirty)
		{
			this->SetRoleDataChange(role_id);
		}

		return &role_data->huan_jie_zhan_chang_role_data;
	}

	return NULL;
}

CommonSaveHuanJieData * ActivityHuanJieZhanChang::GetSaveData(bool set_dirty)
{
	CommonSaveHuanJieData * s = m_save_data->GetTypeData<CommonSaveHuanJieData>(0);

	if (set_dirty)
	{
		m_save_data->SetDataChange(0);
	}

	return s;
}

int ActivityHuanJieZhanChang::GetFightSeq()
{
	return m_fight_seq++;
}

int ActivityHuanJieZhanChang::GetRecordKey(int role_id)                                                                                                                                  
{
	CommonSaveKey key1(role_id);
	CommonSaveKey key2(role_id, INT_MAX);
	RetCommonSaveData ret_data;

	bool is_succ = m_single_record_data->GetRangeLastData(key1, key2, ret_data);
	if (is_succ)
	{
		return ret_data.key.param2 + 1;
	}

	return 1;
}

void ActivityHuanJieZhanChang::AddTianTiRecord(int role_id, const CommonSaveHuanJieTianTiReceordData & data)
{
	int record_id = this->GetRecordKey(role_id);
	CommonSaveKey key(role_id, record_id);
	CommonSaveData save_data;
	save_data.huan_jie_tian_ti_record_data = data;
	m_single_record_data->SetData(key, save_data);

	this->SyncTianTiRecrodToGameRole(role_id, record_id, data);
}

void ActivityHuanJieZhanChang::SyncTianTiRecrodToGameRole(int role_id, int record_id, const CommonSaveHuanJieTianTiReceordData & data)
{
	crossgameprotocal::HiddenGameHuanJieSyncTianTiRecord  msg;
	msg.role_id = role_id;
	msg.record_id = record_id;
	msg.record_data = data;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&msg, sizeof(msg));
}

void ActivityHuanJieZhanChang::CheckTianTiRecord(Role * role)
{
	int role_id = role->GetUID();
	CommonSaveKey key1(role_id);
	CommonSaveKey key2(role_id, INT_MAX);
	std::vector<RetCommonSaveData>  record_list = m_single_record_data->GetRangeData(key1, key2);
	for (int i = 0; i < (int)record_list.size(); ++i)
	{
		role->GetRoleModuleManager()->GetRoleHuanJie()->AddTianTiRecord(record_list[i].data.huan_jie_tian_ti_record_data);
		m_single_record_data->RemoveData(record_list[i].key);
	}
}

void ActivityHuanJieZhanChang::CheckTianTiRecord(int role_id)
{
	CommonSaveKey key1(role_id);
	CommonSaveKey key2(role_id, INT_MAX);
	std::vector<RetCommonSaveData>  record_list = m_single_record_data->GetRangeData(key1, key2);
	for (int i = 0; i < (int)record_list.size(); ++i)
	{
		this->SyncTianTiRecrodToGameRole(record_list[i].key.param1, record_list[i].key.param2, record_list[i].data.huan_jie_tian_ti_record_data);
	}

}

int ActivityHuanJieZhanChang::GetUserRecordIndex(int server_id, int role_id)
{
	std::map<int, ActivityHuanJieZhanChangRankRolePos>::iterator iter = m_rank_list_role_to_pos.find(server_id);
	if (iter == m_rank_list_role_to_pos.end())
	{
		return -1;
	}

	const ActivityHuanJieZhanChangRankRolePos & query_role_index = iter->second;
	std::map<int, int>::const_iterator iter2 = query_role_index.role_to_rank_pos.find(role_id);
	if (iter2 == query_role_index.role_to_rank_pos.end())
	{
		return -1;
	}

	return iter2->second - 1;
}

void ActivityHuanJieZhanChang::SetUserRecordIndex(int server_id, int role_id, int pos)
{
	pos == 0 ? m_rank_list_role_to_pos[server_id].role_to_rank_pos.erase(role_id) :
		m_rank_list_role_to_pos[server_id].role_to_rank_pos[role_id] = pos;
}


HuanJieZhanChangRoleItem::HuanJieZhanChangRoleItem(Role * role, int score, int is_finish_ding_ji_sai_arg)
{
	role_id = role->GetUID();
	role_level = role->GetLevel();
	role->GetName(role_name);
	avatar_type = role->GetAvatarType();
	prof = role->GetProfession();
	rank_score = score;
	headshot_id = role->GetHeadshotID();
	is_leader = 0;
	is_finish_ding_ji_sai = is_finish_ding_ji_sai_arg;
	
	int combine_server_id = 0;
	{
		int server_id = role->GetUserId().db_index;
		combine_server_id = server_id;
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), server_id);
		const UniqueServerID * usid = World::GetInstWorld()->GetMotherServerId(origin_usid);
		if (usid)
		{
			combine_server_id = usid->server_id;
		}
	}

	role_server_id = combine_server_id;
}

HuanJieZhanChangRoleItem::HuanJieZhanChangRoleItem(const RoleBattleData & arg_robot_battle_data, int arg_server_id, int score, int is_finish_ding_ji_sai_arg)
{
	role_id = arg_robot_battle_data.role_character_data.character_id;
	role_level = arg_robot_battle_data.role_character_data.level;
	SNPRINTF(role_name, sizeof(role_name), "%s", arg_robot_battle_data.role_character_data.name);
	avatar_type = arg_robot_battle_data.appearance.avatar_type;
	prof = arg_robot_battle_data.role_character_data.profession;
	rank_score = score;
	headshot_id = 0;
	is_leader = 0;
	is_finish_ding_ji_sai = is_finish_ding_ji_sai_arg;
	robot_battle_data = arg_robot_battle_data;
	role_server_id = arg_server_id;
}

HuanJieZhanChangRoleItem::HuanJieZhanChangRoleItem(const HuanJieZhanChangTesterRoleData& role_data)
{
	role_id = role_data.role_id;
	role_level = role_data.role_level;
	F_STRNCPY(role_name, role_data.role_name, sizeof(GameName));
	avatar_type = role_data.avatar;
	prof = role_data.profession;
	rank_score = role_data.rank_score;
	headshot_id = role_data.headshort_id;
	is_leader = role_data.is_leader;
	is_finish_ding_ji_sai = role_data.is_finish_ding_ji_sai;
	robot_battle_data = role_data.battle_data;
	role_server_id = role_data.role_server_id;
}

void HuanJieZhanChangRoleItem::GetData(Protocol::HuanJieZhanChangOpponentInfoItem & ARG_OUT item) const
{
	item.role_id = this->role_id;
	item.score = this->rank_score;
	memcpy(item.role_name, this->role_name, sizeof(item.role_name));
	item.prof = this->prof;
	item.avatar_type = this->avatar_type;
	item.headshot_id = this->headshot_id;
	item.is_leader = this->is_leader;
	item.is_finis_ding_ji = this->is_finish_ding_ji_sai;
}

void HuanJieZhanChangRoleItem::GetData2(HuanJieZhanChangRecordRoleItem & ARG_OUT dst_item) const
{
	dst_item.role_id = this->role_id;
	dst_item.role_level = this->role_level;
	dst_item.rank_score = this->rank_score;
	memcpy(dst_item.role_name, this->role_name, sizeof(dst_item.role_name));
	dst_item.avatar_type = this->avatar_type;
	dst_item.prof = this->prof;
	dst_item.headshot_id = this->headshot_id;
	dst_item.is_leader = this->is_leader;
	dst_item.is_ding_ding = this->is_finish_ding_ji_sai;
}

HuanJieZhanChangFightItem::HuanJieZhanChangFightItem(const ActivityHuanJieTeamInfo & attacker_item, const ActivityHuanJieTeamInfo & defender_item)
	:attacker(attacker_item), defender(defender_item)
{
	start_fight_time = EngineAdapter::Instance().Time() + 5;
}

void HuanJieZhanChangFightItem::GetFightRoleInfo(CommonSaveHuanJieTianTiReceordData ARG_OUT & record, std::map<int, ActivityHuanJieZhanChangShangHaiInfo> shanghai_info) const
{
	for (int i = 0; i < ARRAY_LENGTH(record.role_info_1) && i < (int)this->attacker.role_list.size(); ++i)
	{
		HuanJieZhanChangRecordRoleItem & dst_item = record.role_info_1[i];
		const HuanJieZhanChangRoleItem & src_item = this->attacker.role_list[i];
		src_item.GetData2(dst_item);

		const ActivityHuanJieZhanChangShangHaiInfo & shanghai= shanghai_info[src_item.role_id];
		dst_item.shanghai = (unsigned int)shanghai.shanghai;
		dst_item.zhi_liao = (unsigned int)shanghai.zhi_liao;
		dst_item.chengshou_shanghai = (unsigned int)shanghai.chengshou_shanghai;
	}

	for (int i = 0; i < ARRAY_LENGTH(record.role_info_2) && i < (int)this->defender.role_list.size(); ++i)
	{
		HuanJieZhanChangRecordRoleItem & dst_item = record.role_info_2[i];
		const HuanJieZhanChangRoleItem & src_item = this->defender.role_list[i];
		src_item.GetData2(dst_item);

		const ActivityHuanJieZhanChangShangHaiInfo & shanghai = shanghai_info[src_item.role_id];
		dst_item.shanghai = (unsigned int)shanghai.shanghai;
		dst_item.zhi_liao = (unsigned int)shanghai.zhi_liao;
		dst_item.chengshou_shanghai = (unsigned int)shanghai.chengshou_shanghai;
	}
}

int HuanJieZhanChangFightItem::GetAverageRankScore(bool is_attacker) const
{
	if (is_attacker)
	{
		return attacker.GetAverageRankScore();
	}

	return defender.GetAverageRankScore();
}

void HuanJieZhanChangFightItem::MakeLog()
{
	gamelog::g_log_huan_jie_zhan_chang.buff_printf("===========================================\nMatch Succ:\n");
	gamelog::g_log_huan_jie_zhan_chang.buff_printf("Attackers[\n");
	for (size_t i = 0; i < attacker.role_list.size(); ++i)
	{
		gamelog::g_log_huan_jie_zhan_chang.buff_printf("role[%s] score[%d] is_robot:%s\n", attacker.role_list[i].role_name, attacker.role_list[i].rank_score, ActivityHuanJieZhanChang::IsHuanJieRobotId(attacker.role_list[i].role_id) ? "true" : "false");
	}
	gamelog::g_log_huan_jie_zhan_chang.buff_printf("]\n----------------------------------------\n");
	gamelog::g_log_huan_jie_zhan_chang.buff_printf("Defenders[\n");
	for (size_t i = 0; i < defender.role_list.size(); ++i)
	{
		gamelog::g_log_huan_jie_zhan_chang.buff_printf("role[%s] score[%d] is_robot:%s\n", defender.role_list[i].role_name, defender.role_list[i].rank_score, ActivityHuanJieZhanChang::IsHuanJieRobotId(attacker.role_list[i].role_id) ? "true" : "false");
	}
	gamelog::g_log_huan_jie_zhan_chang.buff_printf("]\n========================================\n");
	gamelog::g_log_huan_jie_zhan_chang.commit_buff(LL_INFO);
}

ActivityHuanJieZhanChangRankItem::ActivityHuanJieZhanChangRankItem(int uid, const ActivityHuanJieZhanChangRoleData & role_data)
{
	role_id = uid;
	score = role_data.is_finish_ding_ji ? role_data.rank_score : 0;
	last_join_time = role_data.last_join_time;
	memcpy(role_name, role_data.role_name, sizeof(role_name));
	prof = role_data.prof;
}

bool ActivityHuanJieZhanChangRankItem::operator<(const ActivityHuanJieZhanChangRankItem & other) const
{
	if (score > other.score)
		return true;
	if (score < other.score)
		return false;

	if (last_join_time < other.last_join_time)
		return true;
	if (last_join_time > other.last_join_time)
		return false;

	if (role_id < other.role_id)
		return true;
	if (role_id > other.role_id)
		return false;

	return false;
}

bool ActivityHuanMatchRoleNumRequireItem::operator<(const ActivityHuanMatchRoleNumRequireItem & other)  const
{
	std::map<int, int>::const_reverse_iterator self_iter = need_role_num.rbegin();
	std::map<int, int>::const_reverse_iterator other_iter = other.need_role_num.rbegin();

	while (self_iter != need_role_num.rend() && other_iter != other.need_role_num.rend())
	{
		if (self_iter->first > other_iter->first)
			return true;
		if (self_iter->first < other_iter->first)
			return false;

		++self_iter;
		++other_iter;
	}
	
	if (self_iter != need_role_num.rend() && other_iter == other.need_role_num.rend())
		return true;

	if (self_iter == need_role_num.rend() && other_iter != other.need_role_num.rend())
		return false;

	return false;
}

bool ActivityHuanMatchRoleNumRequireItem::IsMeetRequirements(std::map<int, int> & has_role_num) const
{
	for (std::map<int, int>::const_iterator iter = need_role_num.begin(); iter != need_role_num.end(); ++iter)
	{
		if (iter->second  > has_role_num[iter->first])
		{
			return false;
		}
	}

	for (std::map<int, int>::const_iterator iter = need_role_num.begin(); iter != need_role_num.end(); ++iter)
	{
		has_role_num[iter->first] -= iter->second;
	}

	return true;
}

ActivityHuanJieMatchRoleNumRequire::ActivityHuanJieMatchRoleNumRequire()
{
	ActivityHuanMatchRoleNumRequireItem item1;
	item1.need_role_num[1] = 3;
	need_role_num_list.push_back(item1);


	ActivityHuanMatchRoleNumRequireItem item2;
	item2.need_role_num[2] = 1;
	item2.need_role_num[1] = 1;
	need_role_num_list.push_back(item2);


	ActivityHuanMatchRoleNumRequireItem item3;
	item3.need_role_num[3] = 1;
	need_role_num_list.push_back(item3);

	std::sort(need_role_num_list.begin(), need_role_num_list.end());
}

std::vector< ActivityHuanMatchRoleNumRequireItem> ActivityHuanJieMatchRoleNumRequire::GetActivityHuanMatchRoleNumRequireItem(int group_num, std::map<int, int> has_role_num) const
{
	std::vector< ActivityHuanMatchRoleNumRequireItem> ret;
	for (int i = 0; i < group_num; ++i)
	{
		for (int i = 0; i < (int)need_role_num_list.size(); ++i)
		{
			const ActivityHuanMatchRoleNumRequireItem & curr = need_role_num_list[i];
			if (curr.IsMeetRequirements(has_role_num))
			{
				ret.push_back(curr);
				break;
			}
		}
	}

	if ((int)ret.size() != group_num)
	{
		ret.clear();
	}

	return ret;
}

ActivityHuanJieTeamInfo::ActivityHuanJieTeamInfo()
{
	begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	max_role_num = ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM;
	match_rank_min = 0;
	match_rank_max = MAX_INT16;
}

int ActivityHuanJieTeamInfo::GetAverageRankScore() const
{
	int average = 0;

	int s = (int)role_list.size();
	if (s > 0)
	{
		for (int i = 0; i < (int)role_list.size(); ++i)
		{
			average += role_list[i].rank_score;
		}

		average = average / s;
	}

	return average;
}

int ActivityHuanJieTeamInfo::GetAverageLevel() const
{
	int average = 0;

	int s = (int)role_list.size();
	if (s > 0)
	{
		for (int i = 0; i < (int)role_list.size(); ++i)
		{
			average += role_list[i].role_level;
		}

		average = average / s;
	}

	return average;
}

int ActivityHuanJieTeamInfo::GetRoleLeaderServerId() const
{
	for (int i = 0; i < (int)this->role_list.size(); ++i)
	{
		
		int role_id = role_list[i].role_id;
		if (ActivityHuanJieZhanChang::IsHuanJieRobotId(role_id))
		{
			continue;
		}
		
		if (this->role_list[i].is_leader) 
		{
			return this->role_list[i].role_server_id;
		}
	}

	return 1;
}

bool ActivityHuanJieTeamInfo::operator<(const ActivityHuanJieTeamInfo & other) const
{
	if (this->GetAverageRankScore() > other.GetAverageRankScore())
		return false;
	if (this->GetAverageRankScore() < other.GetAverageRankScore())
		return true;

	return false;
}

bool ActivityHuanJieTeamInfo::AddTeamInfo(ActivityHuanJieTeamInfo team_info)
{
	if (team_info.role_list.empty())
	{
		return true;
	}

	if ((int)(this->role_list.size() + team_info.role_list.size()) > max_role_num)
	{
		return false;
	}

	if (team_info.role_list.size() == 1)
	{
		team_info.role_list[0].is_leader = 0;
	}

	if (this->role_list.size() == 1)
	{
		this->role_list[0].is_leader = 0;
	}

	for (int i = 0; i < (int)team_info.role_list.size(); ++i)
	{
		this->role_list.push_back(team_info.role_list[i]);
	}

	this->AutoSetLeader();
	return true;
}

bool ActivityHuanJieTeamInfo::AddRoleItem(const HuanJieZhanChangRoleItem & role_item)
{
	this->role_list.push_back(role_item);
	this->AutoSetLeader();
	return true;
}

void ActivityHuanJieTeamInfo::AutoSetLeader()
{
	int leader_num = 0;
	for (int i = 0; i < (int)this->role_list.size(); ++i)
	{
		if (this->role_list[i].is_leader)
		{
			leader_num += 1;
		}
	}

	if (leader_num == 1)
	{
		return;
	}

	int role_max_score_index = -1;
	int max_score_index = -1;
	for (int i = 0; i < (int)this->role_list.size(); ++i)
	{
		this->role_list[i].is_leader = 0;
		int role_id = role_list[i].role_id;

		if (max_score_index == -1)
		{
			max_score_index = i;
		}
		else
		{
			if (this->role_list[i].rank_score > this->role_list[max_score_index].rank_score)
			{
				max_score_index = i;
			}
		}

		if (!ActivityHuanJieZhanChang::IsHuanJieRobotId(role_id))
		{
			if (role_max_score_index == -1)
			{
				role_max_score_index = i;
			}
			else
			{
				if (this->role_list[i].rank_score > this->role_list[role_max_score_index].rank_score)
				{
					role_max_score_index = i;
				}
			}
		}
	}

	if (role_max_score_index != -1)
	{
		this->role_list[role_max_score_index].is_leader = 1;
	}
	else 	if (max_score_index != -1)
	{
		this->role_list[max_score_index].is_leader = 1;
	}

}

bool ActivityHuanJieTeamInfo::HasRoleDingJi() const
{
	for (int i = 0; i < (int)this->role_list.size(); ++i)
	{
		if (!role_list[i].is_finish_ding_ji_sai)
		{
			return true;
		}
	}

	return false;
}

int ActivityHuanJieTeamInfo::GetLackRoleNum() const
{
	int has_num = (int)role_list.size();
	return max_role_num - has_num > 0 ? max_role_num - has_num : 0;
}

std::set<int> ActivityHuanJieTeamInfo::GetRoleIdList() const
{
	std::set<int> ret;

	for (int i = 0; i < (int)this->role_list.size(); ++i)
	{
		ret.insert(role_list[i].role_id);
	}

	return ret;
}

int ActivityHuanJieTeamInfo::GetRoleNum() const
{
	return (int)role_list.size();
}

void ActivityHuanJieTeamInfo::CalcMatchRankSection(unsigned int now)
{
	int highest_score = this->GetHighestRankScore();
	match_rank_max = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetBigRankType(highest_score);
	if (now < begin_time) // 容错，但一般不太可能会出现这种情况
	{
		begin_time = now;
	}

	int wait_time = static_cast<int>(now - begin_time);
	match_rank_min = GetMax(HUAN_JIE_BIG_RANK_TYPE_INVALID + 1, match_rank_max - wait_time / 10);
}

int ActivityHuanJieTeamInfo::GetHighestRankScore() const
{
	int highest_score = 0;
	for (size_t i = 0; i < role_list.size(); ++i)
	{
		highest_score = GetMax(role_list[i].rank_score, highest_score);
	}

	return highest_score;
}
