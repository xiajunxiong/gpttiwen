#include "cloudarena.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "engineadapter.h"
#include "global/cloudarena/cloudarenauser.hpp"
#include "obj/character/role.h"
#include "global/cloudarena/cloudarenaconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "protocol/msgcloudarena.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "servercommon/errornum.h"
#include "global/rmibackobjdef.h"
#include "global/usercache/usercache.hpp"
#include "gamelog.h"
#include "serverlogic.h"
#include "rmiclient/rmiglobal.hpp"
#include "internalcomm.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "other/funopen/funopen.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/funopen/funopenconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "protocol/msgformation.h"
#include "other/formation/pethelperformation.hpp"
#include "other/event/eventhandler.hpp"

void RMIInitCloudArenaDataBackObjectImpl::InitRet(int ret, const CloudArenaParam& param)
{
	if (ret != 0)
	{
		printf("RMIInitCloudArenaDataBackObjectImpl::InitRet ERROR ret[%d]\n", ret);
		gamelog::g_log_serious_error.printf(LL_ERROR, "RMIInitCloudArenaDataBackObjectImpl::InitRet ERROR ret[%d]", ret);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitCloudArenaDataBackObjectImpl::InitRet");
		return;
	}

	CloudArena::Instance().Init(param);
	CloudArena::Instance().m_data_loader.SetDataStatus(GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH);
}

void RMIInitCloudArenaUserDataBackObjectImpl::InitRet(int result, const CloudArenaUserListParam& data)
{
	if (result < 0 || data.count < 0 || data.count > CLOUD_ARENA_ONCE_OPER_USER_COUNT)
	{
		printf(" RMIInitCloudArenaUserDataBackObjectImpl::InitRet Failed result[%d] data.count[%d]\n",
			result, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitCloudArenaUserDataBackObjectImpl::InitRet ret[%d] data.count[%d]",
			result, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitCloudArenaUserDataBackObjectImpl::InitRet_1");
		return;
	}

	for (int i = 0; i < data.count; ++i)
	{
		if (data.user_list[i].uid <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RMIInitCloudArenaUserDataBackObjectImpl::InitRet INVALID USER INFO] uid[%d]",
				data.user_list[i].uid);
			continue;
		}

		if (!CloudArena::Instance().InitUserData(data.user_list[i]))
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RMIInitCloudArenaUserDataBackObjectImpl::InitRet INIT USER FAILED] uid[%d]",
				data.user_list[i].uid);
			continue;
		}
	}

	if (data.count > 0)
	{
		CloudArena::Instance().m_data_loader.LoadUserData(data.next_id_from);
	}
	else
	{
		CloudArena::Instance().m_data_loader.SetUserDataStatus(GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH);
	}
}

CloudArena& CloudArena::Instance()
{
	static CloudArena inst;
	return inst;
}

void CloudArena::Init(const CloudArenaParam& param)
{
	m_param = param;

	if (0 == m_param.season_info.world_level)
	{
		m_param.season_info.world_level = static_cast<short>(GetMax(WorldStatus::Instance().GetWorldLevel(), FunOpen::GetFunLimitLevel(FUN_TYPE_CLOUD_ARENA)));
	}

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::Init SEASON_INFO  season_timestamp[%u] season_end_timestamp[%u] season_idx[%d] has_release_season_reward[%d]",
		m_param.season_info.season_timestamp, m_param.season_info.season_end_timestamp, m_param.season_info.season_idx, m_param.season_info.has_release_season_reward);
}

bool CloudArena::InitUserData(const CloudArenaUserListParam::CloudArenaUserData& data)
{
	int uid = data.uid;
	CloudArenaUser* user = this->MutableUser(uid);
	if (NULL != user)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "CloudArena::InitUserData uid[%d] duplicate", uid);
		return false; // 重复了
	}

	user = this->CreateUserPtr(data);
	if (NULL == user)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "CloudArena::InitUserData uid[%d] create user ptr failed!", uid);
		return false;
	}

	user->SetOldState(true);

	m_user_map.insert(UserMap::value_type(data.uid, user)).first;
	this->AddUserToScoreHash(user->GetScore(), user);

	m_rank_mgr.OnUserUpdateRankInfo(user);

	int dan_type = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanTypeByScore(user->GetScore());
	m_rank_mgr.OnUserUpdateDanRank(user, CLOUD_ARENA_DAN_TYPE_INVALID, dan_type);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::InitUserData user[%d,%s] score[%d] rank[%d]",
		user->GetUID(), user->GetName(), user->GetScore(), m_rank_mgr.GetUserRank(user->GetUID()));

	return true;
}

void CloudArena::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_CLOUD_ARENA);
	if (NULL == cfg || role->GetLevel() < cfg->limit_level)
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 为防止跨服没有连上，先在本服计算一次称号
		const CommonDataParam& common_data = role->GetRoleModuleManager()->GetCommonData();
		int last_season_rank = common_data.cloud_arena_last_season_rank;
		unsigned int expire_timestamp = common_data.cloud_arena_season_timestamp + CLOUD_ARENA_SEASON_DURATION_S;
		role->GetRoleModuleManager()->CalculateCloudArenaTitle(last_season_rank, expire_timestamp);

		crossgameprotocal::GameHiddenSyncCloudArenaUserDataReq req;
		req.plat_type = LocalConfig::Instance().GetPlatType();
		req.server_id = LocalConfig::Instance().GetMergeServerId();
		req.uid = UidToInt(user_id);
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
		
		crossgameprotocal::GameHiddenRoleLoginSyncCloudArenaUserInfo req1;
		req1.uid = UidToInt(user_id);
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req1, sizeof(req1));
		
		return;
	}

	const CloudArenaUser* user = this->GetUser(role, true); // 内含create
	if (NULL == user) return;

	this->SendUserSelfInfo(role->GetUID(), user, role);
	this->QueryTargetInfo(role);

	role->GetRoleModuleManager()->UpdateCloudArenaData(m_param.season_info.season_timestamp,
		user->GetLastSeasonRank(), user->GetSeasonHighestDanId(), user->GetSeasonHighestRank(), user->GetSeasonHighestScore());

	{
		Protocol::SCCloudArenaRealTimeInfo info;
		info.cur_score = user->GetScore();
		info.cur_rank = this->GetUserRank(role->GetUID());
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}

	this->SyncRoleBattleData(role);
}

void CloudArena::OnUserLoginOriginSendUserInfo(int uid)
{
	const CloudArenaUser* user = CloudArena::Instance().GetUser(uid);
	if (NULL != user)
	{
		this->SendUserSelfInfo(uid, user);
	}
	else
	{
		this->SendUserFakeSelfInfo(uid, NULL);
	}
}

void CloudArena::OnDayChange()
{
	this->ResetDefenseBuff();

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		bool has_change = false;
		user->OnDayChange(&has_change);

		if (has_change)
		{
			this->SetUserDirty(user);
		}
	}
}

void CloudArena::Update(unsigned long interval, unsigned int now_second)
{
	if (!this->IsLoadFinish())
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (now_second > m_param.season_info.season_end_timestamp - CLOUD_ARENA_SEASON_REWARD_TIME
		&& 0 == m_param.season_info.has_release_season_reward)
	{
		m_param.season_info.has_release_season_reward = 1;
		this->SettleSeasonFinalRank();
		this->ReleaseSeasonReward();
		this->SaveData(true, now_second);
	}

	if (now_second > m_param.season_info.season_end_timestamp)
	{
		this->StartNewSeason();
		m_next_save_time = now_second + 10; // 10秒后再保存，避免在0点时触发太多的保存导致mysql比较繁忙
		//this->SaveData(true, now_second);
	}

	if (now_second > m_next_save_time)
	{
		this->SaveData(false, now_second);
	}

	this->UserUpdate(interval, now_second);

	this->CheckAllUserRank(interval, now_second);
}

void CloudArena::UserUpdate(unsigned long interval, unsigned int now_second)
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		bool has_change = false;
		user->Update(interval, now_second, &has_change);

		if (has_change)
		{
			this->SetUserDirty(user);
		}
	}

	it = m_newbie_user_map.begin();
	for (; it != m_newbie_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		user->Update(interval, now_second, NULL);  // newbie不保存数据
	}
}

bool CloudArena::OnServerStart()
{
	return this->LoadAllData();
}

void CloudArena::OnServerStop()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	this->SaveData(true, now);
	return;
}

void CloudArena::OnRoleOperateReq(Role* role, int oper_type, int param1, int param2)
{
	if (!FunOpen::IsFunOpen(role, FUN_TYPE_CLOUD_ARENA, true))
	{
		return;
	}

	const CloudArenaUser* user = this->GetUser(role, true); // 内含create
	if (NULL == user) return;

	switch (oper_type)
	{
	case Protocol::CSCloudArenaOperate::OPER_TYPE_SELF_INFO:
		{
			this->SendUserSelfInfo(role->GetUID(), user, role);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_TARGET_INFO:
		{
			this->QueryTargetInfo(role);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_REFRESH_TARGET:
		{
			this->ManualRefreshOpponent(role);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_START_FIGHT:
		{
			this->StartFight(role, param1);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_RECORD_LIST:
		{
			this->QueryFightRecord(role, param1, param2);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_RANK_LIST:
		{
			this->QueryRankList(role, param1);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_FETCH_RANKING_REWARD:
		{
			this->FetchRankingReward(role, param1);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_OPEN_UI:
		{
			if (!user->HasNoticeSeasonInitialScore())
			{
				this->SendSeasonInitialScoreNotice(role->GetUID(), user, role);
				this->OnNoticeSeasonInitialScore(role);
			}
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_ACHIEVEMENT_INFO:
		{
			this->SendAchievementInfo(role->GetUID(), user, role);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_FETCH_ACHIEVE:
		{
			this->FetchAchievementReward(role, param1);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_FETCH_DAY_REWARD:
		{
			this->FetchDayReward(role);
		}
		break;

	case Protocol::CSCloudArenaOperate::OPER_TYPE_QUERY_FORMATION:
		{
			this->QueryFormation(role);
		}
		break;
	}
}

bool CloudArena::OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req, Role* role)
{
	if (NULL == role || NULL == req) return false;

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (!user->OnRoleSetFormation(req, role))
	{
		return false;
	}

	
	this->SetUserDirty(user);
	this->SendUserFormationInfo(role->GetUID(), user, role);

	return true;
}

void CloudArena::OnFightFinish(int attacker_uid, int defender_uid, bool is_win, bool is_draw, const RoleBattleData& attacker_battle_data, const BattleData_Statistic& stats)
{
	static CloudArenaFightRecord record;
	record.Reset();
	if (is_win)
	{
		record.is_attacker_win = 1;
	}
	else if (is_draw)
	{
		record.is_attacker_win = 2;
	}
	else
	{
		record.is_attacker_win = 0;
	}
	
	record.record_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&record.record_id))
	{
		record.record_id = 0LL;
	}

	int default_score = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().initial_score;
	int attacker_score = default_score;
	int defender_score = default_score;
	int old_attacker_rank = m_rank_mgr.GetUserRank(attacker_uid);
	int old_defender_rank = m_rank_mgr.GetUserRank(defender_uid);
	int new_attacker_rank = -1;
	int new_defender_rank = -1;
	CloudArenaUser* attacker_user = this->MutableUser(attacker_uid);
	CloudArenaUser* defender_user = this->MutableUser(defender_uid);
	if (NULL != attacker_user) attacker_score = attacker_user->GetScore();
	if (NULL != defender_user) defender_score = defender_user->GetScore();

	int old_attacker_dan_id = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanIdByScore(attacker_score);
	int old_defender_dan_id = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanIdByScore(defender_score);

	int modify_attacker_score = 0;
	int modify_attacker_arena_coin = 0;
	int defender_seq = -1;
	if (NULL != attacker_user && attacker_user->FindTargetByUid(defender_uid, &defender_seq))
	{
		LOGIC_CONFIG->GetCloudArenaConfig()->GetScoreAndArenaCoinReward(defender_seq, is_win, is_draw, &modify_attacker_score, &modify_attacker_arena_coin);
	}

	if (NULL != attacker_user)
	{
		int consume_fight_times = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().each_fight_consume_times;
		attacker_user->UseFightTimes(consume_fight_times);

		attacker_user->AddTodayChallengeList(defender_uid);
		attacker_user->SetLastTimeFightResult(is_win);
		this->ModifyUserScore(attacker_user, modify_attacker_score);

		attacker_user->SetRoleBattleData(attacker_battle_data); // 更新战斗数据
		if (m_newbie_user_map.find(attacker_uid) != m_newbie_user_map.end()) // 如果是第一次打的，后续就可以被打了，所以插入usermap
		{
			m_newbie_user_map.erase(attacker_uid);
			m_user_map.insert(UserMap::value_type(attacker_uid, attacker_user));
		}

		this->AddArenaCoin(attacker_uid, modify_attacker_arena_coin);
		this->SetUserDirty(attacker_user);
		this->SendUserSelfInfo(attacker_uid, attacker_user);		
		this->RefreshOpponent(attacker_uid);
		this->SendUserTargetInfo(attacker_uid, attacker_user);

		EventHandler::Instance().OnCloudArenaUseTimes(attacker_uid);
	}

	if (NULL != attacker_user)
	{
		attacker_user->GetFightRecordUserInfo(&record.attacker_info);
		record.attacker_info.change_score = modify_attacker_score;
		record.attacker_info.change_arena_coin = modify_attacker_arena_coin;

		new_attacker_rank = m_rank_mgr.GetUserRank(attacker_uid);
		record.attacker_info.new_rank = new_attacker_rank;
		record.attacker_info.old_rank = old_attacker_rank;
		record.attacker_info.old_score = attacker_score;
		record.attacker_info.is_robot = 0;
	}

	if (NULL != defender_user)
	{
		defender_user->GetFightRecordUserInfo(&record.defender_info);
		record.defender_info.change_score = 0;
		record.defender_info.change_arena_coin = 0;
		
		new_defender_rank = m_rank_mgr.GetUserRank(defender_uid);
		record.defender_info.new_rank = new_defender_rank;	
		record.defender_info.old_rank = old_defender_rank;
		record.defender_info.old_score = defender_score;
		record.defender_info.is_robot = defender_user->IsRobot() ? 1 : 0;
	}

	if (NULL != attacker_user)
	{
		attacker_user->AddFightRecord(record);
		this->SendUserNewFightRecordNotice(attacker_uid, attacker_user);

		attacker_user->OnRankUpdate(new_attacker_rank);
		attacker_user->TryUpdateHighestDanAndRank();

		bool is_higher_dan = old_attacker_dan_id < old_defender_dan_id;
		attacker_user->OnFightFinishUpdateAchievement(true, is_win, is_higher_dan,
			(int)(stats.attacker_total_damage + stats.total_attacker_partner_damage_to_enemy), 
			stats.round_num, stats.attacker_use_medicine_times, stats.attacker_realive_times);
	}

	if (NULL != defender_user && !defender_user->IsRobot())
	{
		defender_user->AddFightRecord(record);
		this->SendUserNewFightRecordNotice(defender_uid, defender_user);

		defender_user->OnRankUpdate(new_defender_rank);
		defender_user->TryUpdateHighestDanAndRank();

		bool is_higher_dan = old_attacker_dan_id > old_defender_dan_id;
		defender_user->OnFightFinishUpdateAchievement(false, !is_win, is_higher_dan,
			(int)(stats.defender_total_damage + stats.total_defender_partner_damage_to_enemy), 
			stats.round_num, stats.defender_use_medicine_times, stats.defender_realive_times);
	}
	EventHandler::Instance().OnCloudArenaFightFinish(attacker_uid, is_win);
	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::OnFightFinish attacker[%d] defender[%d] is_attacker_win[%d] \
	attacker_modify_score[%d] defender_modify_score[%d] attacker_modify_arena_coin[%d] new_attacker_rank[%d] new_defender_rank[%d]",
		attacker_uid, defender_uid, is_win ? 1 : 0, modify_attacker_score, 0, modify_attacker_arena_coin,
		new_attacker_rank, new_defender_rank);
}

void CloudArena::OnFightStart(int attacker_uid)
{

}

void CloudArena::OnGameRegisterCross(const UniqueServerID& mother_sid)
{
	m_rank_mgr.OnGameRegisterCross(mother_sid);

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (NULL == user || !user->IsRobot()) continue;

		m_rank_mgr.OnRobotUpdateServerRankInfo(user, mother_sid);
	}

	this->SyncSeasonInfoToGame(&mother_sid);
}

int CloudArena::GetUserRank(int uid)
{
	return m_rank_mgr.GetUserRank(uid);
}

void CloudArena::OnUserScoreChangedUpdateScoreHash(int old_score, CloudArenaUser* user)
{
	this->RemoveUserFromScoreHash(old_score, user->GetUID());
	this->AddUserToScoreHash(user->GetScore(), user);
}

void CloudArena::Release()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_user_map.clear();

	it = m_newbie_user_map.begin();
	for (; it != m_newbie_user_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_newbie_user_map.clear();
}

CloudArenaUser* CloudArena::MutableUser(int uid, bool include_newbie /* = true */)
{
	UserMap::iterator it = m_user_map.find(uid);
	if (it != m_user_map.end())
	{
		return it->second;
	}

	if (IsRobot(uid))
	{
		return NULL;
	}

	if (include_newbie)
	{
		it = m_newbie_user_map.find(uid);
		if (it != m_newbie_user_map.end())
		{
			return it->second;
		}
	}
	
	return NULL;
}

const CloudArenaUser* CloudArena::GetUser(int uid, bool include_newbie /* = true */)
{
	return this->MutableUser(uid, include_newbie);
}

const CloudArenaUser* CloudArena::GetUser(Role* role, bool create_if_not_found /* = false */)
{
	if (NULL == role) return NULL;

	UserMap::iterator it = m_user_map.find(role->GetUID());
	if (it != m_user_map.end())
	{
		return it->second;
	}

	it = m_newbie_user_map.find(role->GetUID());
	if (it != m_newbie_user_map.end())
	{
		return it->second;
	}

	if (!create_if_not_found)
	{
		return NULL;
	}

	// 找不到就创建user实例并放到m_newbie_user_map中
	CloudArenaUser* user = this->CreateUserPtr(role);
	if (NULL == user) return NULL;

	it = m_newbie_user_map.insert(UserMap::value_type(role->GetUID(), user)).first;
	return it->second;
}

void CloudArena::GmAddChallengeTimes(Role* role, int times)
{
	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	user->AddFightTimes(times);
	this->SetUserDirty(user);

	this->SendUserSelfInfo(role->GetUID(), user, role);
}

void CloudArena::GmAddScore(Role* role, int score)
{
	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	this->ModifyUserScore(user, score);
	this->SetUserDirty(user);
	this->SendUserSelfInfo(role->GetUID(), user, role);
}

void CloudArena::GmClearRefreshCD(Role* role)
{
	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	user->SetNextManualRefreshTargetTimestamp(0u);
	this->SetUserDirty(user);
	this->SendUserTargetInfo(role->GetUID(), user, role);
}

void CloudArena::GmRandomSetFormation(Role* role)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user) return;

	Protocol::CSSetPetHelperFormation req;

	static const int ROLE_GRID[2] = { 2, 7 };
	int role_grid = ROLE_GRID[rand() % 2];
	req.grid_list[role_grid].id = role->GetUID();
	req.grid_list[role_grid].type = PET_HELPER_FORMATION_GRID_ROLE;

	std::vector<int> partner_list = role->GetRoleModuleManager()->GetPartner()->GetActiveParnerList();
	int PARNER_COLUMN[ROW_CHARACTER_NUM - 1] = { 1, 3, 0, 4 };
	RandomShuffle(PARNER_COLUMN, ARRAY_LENGTH(PARNER_COLUMN));
	if (!partner_list.empty())
	{
		RandomShuffle(&partner_list[0], partner_list.size());
	}
	for (int i = 0; i < ARRAY_LENGTH(PARNER_COLUMN) && i < static_cast<int>(partner_list.size()); ++i)
	{
		int grid = PARNER_COLUMN[i];
		if (rand() % 2 > 0)
		{
			grid = (grid + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;	
		}
		req.grid_list[grid].id = partner_list[i];
		req.grid_list[grid].type = PET_HELPER_FORMATION_GRID_PARTNER;
	}

	int pet_count = role->GetRoleModuleManager()->GetPet()->GetPetCount();
	std::vector<int> permuatation = RandomPermutation(0, pet_count);
	
	static const int PET_COLUMN[ROW_CHARACTER_NUM] = { 2, 1, 3, 0, 4 };
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(PET_COLUMN) == 5);
	int standby_num = 0;
	for (int i = 0; i < pet_count && standby_num < ARRAY_LENGTH(req.standby_list) - 1; ++i)
	{
		if (i < ARRAY_ITEM_COUNT(PET_COLUMN))
		{
			int grid = PET_COLUMN[i];
			if (req.grid_list[grid].id > 0)
			{
				grid = (grid + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
			}
			
			const ItemGridData* pet_data = role->GetRoleModuleManager()->GetPet()->GetPetGridData(permuatation[i]);
			if (NULL == pet_data) continue;

			req.grid_list[grid].id = pet_data->item_id;
			req.grid_list[grid].unique_id = pet_data->item_unique_id;
			req.grid_list[grid].type = PET_HELPER_FORMATION_GRID_PET;
		}
		else
		{
			const ItemGridData* pet_data = role->GetRoleModuleManager()->GetPet()->GetPetGridData(permuatation[i]);
			if (NULL == pet_data) continue;

			req.standby_list[standby_num].pet_id = pet_data->item_id;
			req.standby_list[standby_num].unique_id = pet_data->item_unique_id;
			standby_num += 1;
		}
	}

	user->OnRoleSetFormation(&req, role);
}

int CloudArena::GetSeasonWorldLevel()
{
	return m_param.season_info.world_level;
}

std::set<int> CloudArena::GetAllUserUidSet() const
{
	std::set<int> tmpset;
	UserMap::const_iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		tmpset.insert(user->GetUID());
	}

	return tmpset;
}

CloudArenaUser* CloudArena::CreateUserPtr(Role* role)
{
	if (NULL == role) return NULL;

	CloudArenaUser* user = new CloudArenaUser();
	if (NULL == user) return NULL;

	if (!user->FillDataWithRole(role))
	{
		delete user;
		return NULL;
	}
	return user;
}

CloudArenaUser* CloudArena::CreateUserPtr(const CloudArenaUserListParam::CloudArenaUserData& data)
{
	CloudArenaUser* user = new CloudArenaUser();
	if (NULL == user) return NULL;

	if (!user->FillDataWithUserData(data))
	{
		delete user;
		return NULL;
	}

	return user;
}

bool SortByScore(const CloudArena::Opponent& a, const CloudArena::Opponent& b)
{
	if (a.score != b.score)
		return a.score > b.score;
	else
		return a.uid < b.uid;
}

bool CloudArena::RefreshOpponent(int uid, bool is_manual_refresh /* = false */)
{
	CloudArenaUser* user = this->MutableUser(uid);
	if (NULL == user) return false;

	bool ignore_robot = false;
	int user_rank = m_rank_mgr.GetUserRank(uid);
	if (user_rank >= 0 && user_rank < 100) // 尝试不匹配到robot
	{
		ignore_robot = true;
	}

	OpponentList cur_opponent_list;
	cur_opponent_list.reserve(CLOUD_ARENA_NEW_OPPONENT_NUM);

	std::vector<int> exclude_list(user->GetTodayChallengeList());
	exclude_list.push_back(uid); // 不能刷到自己
	for (int i = 0; i < CLOUD_ARENA_NEW_OPPONENT_NUM; ++i)
	{
		int low_match_factor = 0;
		int high_match_factor = 0;
		LOGIC_CONFIG->GetCloudArenaConfig()->CalculateMatchFactor(i, user->IsLastTimeWin(), user->IsLastTimeLose(), user->GetManualRefreshTimes(), &low_match_factor, &high_match_factor);	

		const CloudArenaUser* opponent = this->FindOpponent(user->GetScore(), low_match_factor, high_match_factor, cur_opponent_list, exclude_list, ignore_robot);
		if (NULL == opponent && ignore_robot)
		{
			// 如果忽略robot找不到对手，就不忽略robot试试
			ignore_robot = false;
			opponent = this->FindOpponent(user->GetScore(), low_match_factor, high_match_factor, cur_opponent_list, exclude_list, ignore_robot);
		}
		
		if (NULL == opponent)
		{

#ifdef _DEBUG
			gamelog::g_log_cloud_arena.printf(LL_WARNING, "CloudArena::RefreshOpponent role[%d,%s] cur_score[%d] low_factor[%d] high_factor[%d] Find Opponent NULL", user->GetUID(), user->GetName(), user->GetScore(), low_match_factor, high_match_factor);
#endif

			continue;
		}
		cur_opponent_list.push_back(Opponent(opponent->GetUID(), opponent->GetScore()));

#ifdef _DEBUG
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::RefreshOpponent role[%d,%s] cur_score[%d] low_factor[%d] high_factor[%d] Find Opponent user[%d,%s] score[%d]", user->GetUID(), user->GetName(), user->GetScore(), low_match_factor, high_match_factor,
			opponent->GetUID(), opponent->GetName(), opponent->GetScore());
#endif

	}
	std::sort(cur_opponent_list.begin(), cur_opponent_list.end(), SortByScore);

	user->SetOpponentList(cur_opponent_list);
	if (is_manual_refresh)
	{
		user->OnManualRefreshOpponent();

		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::RefreshOpponent Manually role[%d,%s] cur_score[%d] manual_times[%d]", 
			user->GetUID(), user->GetName(), user->GetScore(), user->GetManualRefreshTimes());
	}
	else
	{
		//user->ResetManualRefreshTimes();
	}

	this->SetUserDirty(user);

	return true;
}

bool CloudArena::ManualRefreshOpponent(Role* role)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= m_param.season_info.season_end_timestamp - CLOUD_ARENA_SETTLEMENT_DURATION_S)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_SETTLEMENT_LIMIT);
		return false;
	}

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (user->GetManualRefreshTimes() >= LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().manual_refresh_times_limit)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_MANUAL_REFRESH_TIMES_LIMIT);
		return false;
	}
	
	if (now < user->GetNextManualRefreshTargetTimestamp())
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_MANUAL_REFRESH_CD_LIMIT);
		return false;
	}

	int cd_s = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().refresh_opponent_cd_s;
	user->SetNextManualRefreshTargetTimestamp(now + cd_s);

	this->RefreshOpponent(role->GetUID(), true);
	this->SendUserTargetInfo(role->GetUID(), user, role);

	return true;
}

bool IsFoundCandidate(CloudArenaUser* user, const CloudArena::OpponentList& cur_opponent_list, const std::vector<int>& exclude_list)
{
	for (size_t k = 0; k < cur_opponent_list.size(); ++k)
	{
		if (user->GetUID() == cur_opponent_list[k].uid)
		{
			return true;
		}
	}

	for (size_t k = 0; k < exclude_list.size(); ++k)
	{
		if (user->GetUID() == exclude_list[k])
		{
			return true;
		}
	}

	return false;
}

void FilterCandidateList(std::vector<CloudArenaUser*>& candidate_list, const CloudArena::OpponentList& cur_opponent_list, const std::vector<int>& exclude_list, bool ignore_robot)
{
	for (int i = candidate_list.size() - 1; i >= 0; --i)
	{
		if (NULL == candidate_list[i] 
			|| IsFoundCandidate(candidate_list[i], cur_opponent_list, exclude_list)
			|| (ignore_robot && candidate_list[i]->IsRobot()))
		{
			std::swap(candidate_list[i], candidate_list[candidate_list.size() - 1]);
			candidate_list.pop_back();
		}
	}
}

const CloudArenaUser* CloudArena::FindOpponent(int base_score, int low_match_factor, int high_match_factor, const OpponentList& cur_opponent_list, const std::vector<int>& exclude_list, bool ignore_robot)
{
	int try_times = 30;
	while (--try_times > 0)
	{
		int low_match_score = static_cast<int>(1.0 * base_score * low_match_factor / 100);
		int high_match_score = static_cast<int>(1.0 * base_score * high_match_factor / 100);

		ScoreUserListHash::iterator lowbound_it = m_score_user_list_hash.lower_bound(low_match_score);
		if (lowbound_it == m_score_user_list_hash.end())
		{
			// 找不到比low_factor更大的元素，则减小low_match_factor的值继续找
			low_match_factor -= LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().boundry_rank_wave_value;
			continue;
		}

		ScoreUserListHash::iterator highbound_it = m_score_user_list_hash.upper_bound(high_match_score);
		std::vector<CloudArenaUser*> candidate_list;
		candidate_list.reserve(10);
		ScoreUserListHash::iterator it = lowbound_it;
		for (; it != highbound_it; ++it)
		{
			std::vector<CloudArenaUser*>& user_list = it->second;
			candidate_list.insert(candidate_list.end(), user_list.begin(), user_list.end());
		}

		FilterCandidateList(candidate_list, cur_opponent_list, exclude_list, ignore_robot);
		if (!candidate_list.empty())
		{
			return candidate_list[rand() % candidate_list.size()];
		}
		else
		{
			low_match_factor -= LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().boundry_rank_wave_value;
			high_match_factor += LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().boundry_rank_wave_value;
			continue;
		}
	}

	return NULL;
}

CloudArena::CloudArena() : m_next_save_time(0u), m_has_gen_robot(false), m_next_update_rank_time(0u)
{

}

CloudArena::~CloudArena()
{
	this->Release();
}

void CloudArena::StartNewSeason()
{
	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::StartNewSeason OldSeasonInfo old_season_timestamp[%u] old_season_end_timestamp[%u] old_season_idx[%d] old_has_release_season_reward[%d]", 
		m_param.season_info.season_timestamp, m_param.season_info.season_end_timestamp, m_param.season_info.season_idx, m_param.season_info.has_release_season_reward);

	m_param.season_info.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();
	m_param.season_info.season_timestamp = this->CalcMondayTimestamp();
	m_param.season_info.season_end_timestamp = this->CalcSeasonEndTimestamp(m_param.season_info.season_timestamp);
	m_param.season_info.season_idx += 1;
	m_param.season_info.has_release_season_reward = 0;
	m_param.season_info.world_level = static_cast<short>(GetMax(WorldStatus::Instance().GetWorldLevel(), FunOpen::GetFunLimitLevel(FUN_TYPE_CLOUD_ARENA)));
	
	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::StartNewSeason NewSeasonInfo new_season_timestamp[%u] new_season_end_timestamp[%u] new_season_idx[%d]",
		m_param.season_info.season_timestamp, m_param.season_info.season_end_timestamp, m_param.season_info.season_idx);

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		user->OnStartNewSeason();
		this->SetUserDirty(user);
	}

	this->ResetDefenseBuff();

	m_rank_mgr.OnStartNewSeason();

	this->ClearAllRobot();
	this->AddRobotFromConfig(); // 重建机器人
	m_has_gen_robot = true;

	this->SyncSeasonInfoToGame(NULL); 

	it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		m_rank_mgr.OnUserUpdateRankInfo(user);
	}
}

unsigned int CloudArena::CalcMondayTimestamp()
{
	return static_cast<unsigned int>(EngineAdapter::Instance().Time()) + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0) - 7 * 24 * 60 * 60;
}

unsigned int CloudArena::CalcSeasonEndTimestamp(unsigned int season_start_timestamp)
{
	return season_start_timestamp + CLOUD_ARENA_SEASON_DURATION_S;
}

void CloudArena::OnLoadAllSucc()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now_second > m_param.season_info.season_end_timestamp - CLOUD_ARENA_SEASON_REWARD_TIME
		&& 0 == m_param.season_info.has_release_season_reward)
	{
		m_param.season_info.has_release_season_reward = 1;
		this->SettleSeasonFinalRank();
		this->ReleaseSeasonReward();
		this->SaveData(true, now_second);
	}

	if (now_second > m_param.season_info.season_end_timestamp)
	{
		this->StartNewSeason();
		this->SaveData(true, now_second);
	}

	m_next_save_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 30;

	if (!m_has_gen_robot)
	{
		this->AddRobotFromConfig(); // 重建机器人
		m_has_gen_robot = true;
	}

	this->CheckAllUserSeasonTimestamp();

	m_next_update_rank_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 60;
}

void CloudArena::AddRobotFromConfig()
{
	int robot_cfg_num = LOGIC_CONFIG->GetCloudArenaConfig()->GetRobotNum();
	for (int id = 1; id <= robot_cfg_num; ++id)
	{
		this->AddARobotByID(id);
	}
}

void CloudArena::AddARobotByID(int id)
{
	CloudArenaUser* user = new CloudArenaUser();
	if (NULL == user) return;

	if (!user->FillRobotData(id))
	{
		delete user; user = NULL;
		return;
	}

	m_user_map.insert(UserMap::value_type(user->GetUID(), user));
	this->AddUserToScoreHash(user->GetScore(), user);
	
	m_rank_mgr.OnUserUpdateRankInfo(user);

	int dan_type = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanTypeByScore(user->GetScore());
	m_rank_mgr.OnUserUpdateDanRank(user, CLOUD_ARENA_DAN_TYPE_INVALID, dan_type);
}

void CloudArena::AddUserToScoreHash(int score, CloudArenaUser* user)
{
	m_score_user_list_hash[score].push_back(user);
}

void CloudArena::RemoveUserFromScoreHash(int score, int uid)
{
	std::vector<CloudArenaUser*>& user_list = m_score_user_list_hash[score];
	std::vector<CloudArenaUser*>::iterator it = user_list.begin();
	for (; it != user_list.end(); ++it)
	{
		CloudArenaUser* user = *it;
		if (uid == user->GetUID())
		{
			user_list.erase(it);
			break;
		}
	}

	if (user_list.empty())
	{
		m_score_user_list_hash.erase(score);
	}
}

void CloudArena::SendUserSelfInfo(int uid, const CloudArenaUser* user /* = NULL */, Role* role /* = NULL */)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	if (user->IsRobot()) return;

	Protocol::SCCloudArenaSelfInfo info;
	info.cur_score = user->GetScore();
	info.cur_rank = m_rank_mgr.GetUserRank(uid);
	info.capability = user->GetCapability();
	info.remain_fight_times = user->GetRemainFightTimes();
	info.day_reward_status = static_cast<char>(user->GetDayRewardStatus());
	info.has_today_fight = user->HasTodayFight() ? 1 : 0;
	info.season_finish_timestamp = m_param.season_info.season_end_timestamp;
	info.fetch_ranking_reward_flag = user->GetRankingRewardFlag();
	info.reach_ranking_flag = user->GetReachRankingFlag();
	info.season_idx = m_param.season_info.season_idx;
	info.today_buy_times = user->GetTodayBuyFightTimesTimes();
	info.next_recover_fighttimes_timestamp = user->GetNextRecoverFightTimesTimestamp();
	info.last_day_score = user->GetLastDayScore();

	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(uid, &info, sizeof(info));
	}
}

void CloudArena::SendUserFakeSelfInfo(int uid, Role* role /*= NULL*/)
{
	Protocol::SCCloudArenaSelfInfo info;
	info.cur_score = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().initial_score;
	info.cur_rank = -1;
	info.capability = 0;
	info.remain_fight_times = 0;
	info.day_reward_status = 0;
	info.has_today_fight = 0;
	info.season_finish_timestamp = m_param.season_info.season_end_timestamp;
	info.fetch_ranking_reward_flag = 0;
	info.reach_ranking_flag = 0;
	info.season_idx = m_param.season_info.season_idx;
	info.today_buy_times = 0;
	info.next_recover_fighttimes_timestamp = 0;
	info.last_day_score = 0;

	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(uid, &info, sizeof(info));
	}
}

void CloudArena::SendUserTargetInfo(int uid, const CloudArenaUser* user /* = NULL */, Role* role /* = NULL */)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	Protocol::SCCloudArenaTargetInfo info;
	info.next_manual_refresh_timestamp = user->GetNextManualRefreshTargetTimestamp();
	info.today_already_refresh_times = user->GetManualRefreshTimes();
	info.count = 0;

	int target_num = 0;
	int target_uid_list[CLOUD_ARENA_OPPONENT_NUM] = { 0 };
	user->GetOpponentList(CLOUD_ARENA_OPPONENT_NUM, &target_num, target_uid_list);
	for (int i = 0; i < target_num && info.count < ARRAY_ITEM_COUNT(info.target_list); ++i)
	{
		const CloudArenaUser* target = this->GetUser(target_uid_list[i]);
		if (NULL == target) continue;

		info.target_list[info.count].score = target->GetScore();
		info.target_list[info.count].rank = m_rank_mgr.GetUserRank(target->GetUID());
		info.target_list[info.count].level = target->GetLevel();
		if (target->IsRobot())
		{
			info.target_list[info.count].is_robot = 1;
			info.target_list[info.count].top_level = 0;
		}
		else
		{
			info.target_list[info.count].is_robot = 0;
			info.target_list[info.count].top_level = UserCacheManager::Instance().GetUserTopLevel(target->GetUID());
		}
		info.target_list[info.count].is_robot = target->IsRobot() ? 1 : 0;
		info.target_list[info.count].capability = target->GetCapability();
		F_STRNCPY(info.target_list[info.count].name, target->GetName(), sizeof(GameName));
		info.target_list[info.count].appearance = target->GetAppearance();
		info.count += 1;
	}

	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		int send_len = sizeof(info) - sizeof(info.target_list) + info.count * sizeof(info.target_list[0]);
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, send_len);
	}
}

void CloudArena::SendUserNewFightRecordNotice(int uid, const CloudArenaUser* user /*= NULL*/)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	Protocol::SCCloudArenaNewFightRecordNotice notice;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &notice, sizeof(notice));
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(uid, &notice, sizeof(notice));
	}
}

void CloudArena::SendUserFormationInfo(int uid, const CloudArenaUser* user /*= NULL*/, Role* role /*= NULL*/)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	if (user->IsRobot()) return;

	static Protocol::SCPetHelperFormationInfo info;
	info.Reset();
	this->AssembleFormationInfo(&info, user->GetBattleData());
	
	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

void CloudArena::QueryTargetInfo(Role* role)
{
	const CloudArenaUser* user = this->GetUser(role->GetUID());
	if (NULL == user) return;

	if (user->HasOpponent())
	{
		this->SendUserTargetInfo(role->GetUID(), user, role);
	}
	else
	{
		this->RefreshOpponent(role->GetUID());
		this->SendUserTargetInfo(role->GetUID(), user, role);
	}
}

void CloudArena::QueryFightRecord(Role* role, int offset, int count)
{
	const CloudArenaUser* user = this->GetUser(role->GetUID());
	if (NULL == user) return;

	Protocol::SCCloudArenaFightRecordList list;
	list.offset = offset;
	list.count = 0;
	user->GetFightRecord(offset, count, list.record_list, &list.count, ARRAY_ITEM_COUNT(list.record_list));

	int send_len = sizeof(list) - sizeof(list.record_list) + list.count * sizeof(list.record_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &list, send_len);
}

void CloudArena::SendAchievementInfo(int uid, const CloudArenaUser* user /* = NULL */, Role* role /* = NULL */)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	if (user->IsRobot()) return;

	Protocol::SCCloudArenaAchievementInfo msg;
	
	for (int achieve_type = 0; achieve_type < CLOUD_ARENA_ACHIEVEMENT_MAX; ++achieve_type)
	{
		const CloudArenaAchievementItem* achieve_item = user->GetAchievementItem(achieve_type);
		if (NULL != achieve_item)
		{
			msg.achieve_list[achieve_type].cur_level = achieve_item->cur_achievement_lv;
			msg.achieve_list[achieve_type].progress_num = achieve_item->process_num;
			msg.achieve_list[achieve_type].reward_status = achieve_item->reward_status;
		}
		else
		{
			msg.achieve_list[achieve_type].cur_level = 0;
			msg.achieve_list[achieve_type].progress_num = 0;
			msg.achieve_list[achieve_type].reward_status = 0;
		}
	}
	
	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
	}
}

bool CloudArena::LoadAllData()
{
	m_data_loader.LoadData();
	m_data_loader.LoadUserData(0);

	return true;
}

void CloudArena::StartFight(Role* role, int target_idx)
{
	if (NULL == role) return;

	if (role->GetRoleStatusManager()->IsInBattleStatus())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return;
	}

	if (role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= m_param.season_info.season_end_timestamp - CLOUD_ARENA_SETTLEMENT_DURATION_S)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_SETTLEMENT_LIMIT);
		return;
	}

	const CloudArenaUser* user = this->GetUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int consume_fight_times = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().each_fight_consume_times;
	if (user->GetRemainFightTimes() < consume_fight_times)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_FIGHT_TIMES_NOT_ENOUGH);
		return;
	}

	int target_uid = user->GetOpponentUid(target_idx);
	const CloudArenaUser* target = this->GetUser(target_uid);
	if (NULL == target)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int max_round = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().max_round;
	int allow_surrender_round = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().allow_surrender_round;
	BattleManagerLocal::GetInstance().StartCloudArenaReq(role, user->GetBattleData(), target_uid, 1, &m_param.other_info.defense_buff_id, max_round, 60 * 60, target->GetBattleData(), allow_surrender_round);
	WorldStatus::Instance().LogFunctionStats(role, "CloudArena");
	role->LogActive(LOG_ACTIVE_TYPE_CLOUD_ARENA);
}

void CloudArena::ClearAllRobot()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); )
	{
		CloudArenaUser* user = it->second;
		if (user->IsRobot())
		{
			m_user_map.erase(it++);
			this->RemoveUserFromScoreHash(user->GetScore(), user->GetUID());
			delete user;
			user = NULL;
		}
		else
		{
			++it;
		}
	}
}

void CloudArena::ModifyUserScore(CloudArenaUser* user, int modify_score)
{
	int old_score = user->GetScore();
	user->ModifyScore(modify_score);

	this->OnUserScoreChangedUpdateScoreHash(old_score, user);

	m_rank_mgr.OnUserUpdateRankInfo(user);

	int old_dan_type = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanTypeByScore(old_score);
	int new_dan_type = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanTypeByScore(user->GetScore());
	m_rank_mgr.OnUserUpdateDanRank(user, old_dan_type, new_dan_type);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::ModifyUserScore user[%d,%s] \
		old_score[%d] new_score[%d] old_dan[%d] new_dan[%d]  cur_rank[%d]", 
		user->GetUID(), user->GetName(), old_score, user->GetScore(),
		old_dan_type, new_dan_type, m_rank_mgr.GetUserRank(user->GetUID()));

	if (old_dan_type != new_dan_type)
	{
		this->SendUserDanTypeChangeNotice(user->GetUID(), old_dan_type, new_dan_type, user);
	}
}

void CloudArena::SaveData(bool is_save_all, unsigned int now)
{
	m_next_save_time = now + RandomNum(70, 85);

	this->SaveCloudArenaParam();

	if (is_save_all)
	{
		this->SaveAllUserData();
	}
	else
	{
		this->SaveUserData();

		if (!m_dirty_user_queue.empty())
		{
			m_next_save_time = now + 8;
		}
	}

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::SaveData now[%d] next[%d]",
		now, m_next_save_time);
}

bool CloudArena::SaveCloudArenaParam()
{
	RMISaveCloudArenaDataBackObjectImpl* impl = new RMISaveCloudArenaDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.SaveCloudArenaDataAsyn(m_param, impl);
}

bool CloudArena::SaveAllUserData()
{
	static CloudArenaUserListParam param;
	param.count = 0;
	
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (!user->IsDirty() || user->IsRobot()) continue;

		user->GetUserData(&param.user_list[param.count]);
		param.user_list[param.count].change_state = user->GetOldState() ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
		param.count += 1;
		
		if (param.count >= CLOUD_ARENA_ONCE_OPER_USER_COUNT)
		{
			if (this->CallRMISaveUserData(param))
			{
				this->ClearUserDirtyFlag(param);
			}
			param.count = 0;
		}
	}

	if (param.count > 0)
	{
		if (this->CallRMISaveUserData(param))
		{
			this->ClearUserDirtyFlag(param);
		}
		param.count = 0;
	}

	return true;
}

bool CloudArena::SaveUserData()
{
	static CloudArenaUserListParam param;
	param.count = 0;

	std::set<int> hash_set;
	while (!m_dirty_user_queue.empty())
	{
		int uid = m_dirty_user_queue.front();
		m_dirty_user_queue.pop();

		if (hash_set.find(uid) != hash_set.end()) continue;
		hash_set.insert(uid);

		const CloudArenaUser* user = this->GetUser(uid, false);
		if (NULL == user || !user->IsDirty() || user->IsRobot()) continue;		

		user->GetUserData(&param.user_list[param.count]);
		param.user_list[param.count].change_state = user->GetOldState() ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
		param.count += 1;

		if (param.count >= CLOUD_ARENA_ONCE_OPER_USER_COUNT)
		{
			break;
		}
	}

	if (param.count > 0)
	{
		if (this->CallRMISaveUserData(param))
		{
			this->ClearUserDirtyFlag(param);
		}
		param.count = 0;
	}

	return true;
}

bool CloudArena::CallRMISaveUserData(const CloudArenaUserListParam& param)
{
	RMISaveChallengeUserObjectImpl *impl = new RMISaveChallengeUserObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.SaveCloudArenaUserDataAsyn(param, impl);
}

void CloudArena::ClearUserDirtyFlag(const CloudArenaUserListParam& param)
{
	for (int i = 0; i < param.count && i < CLOUD_ARENA_ONCE_OPER_USER_COUNT; ++i)
	{
		CloudArenaUser* user = this->MutableUser(param.user_list[i].uid);
		if (NULL != user)
		{
			user->SetIsDirty(false);
			user->SetOldState(true);
		}
	}
}

void CloudArena::SetUserDirty(CloudArenaUser* user)
{
	if (NULL == user) return;

	user->SetIsDirty(true);
	m_dirty_user_queue.push(user->GetUID());
}

void CloudArena::ResetDefenseBuff()
{
	int buff_id = 0;
	if (LOGIC_CONFIG->GetCloudArenaConfig()->RandomDefenseBuff(&buff_id))
	{
		m_param.other_info.defense_buff_id = buff_id;
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::ResetDefenseBuff random buff buff_id[%d]", buff_id);
	}
	else
	{
		gamelog::g_log_cloud_arena.printf(LL_WARNING, "CloudArena::ResetDefenseBuff random buff failed!");
	}
}

void CloudArena::AddArenaCoin(int uid, int add_num)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(add_num, MONEY_TYPE_CLOUD_ARENA_COIN, __FUNCTION__);
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::AddArenaCoin user[%d,%s] add_num[%d]", role->GetUID(), role->GetName(), add_num);
	}
	else
	{
		OfflineRegisterManager::Instance().OnCloudArenaAddArenaCoin(uid, add_num);
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::AddArenaCoin Offline uid[%d] add_num[%d]", uid, add_num);
	}
}

void CloudArena::QueryRankList(Role* role , int rank_type)
{
	switch (rank_type)
	{
	case CLOUD_ARENA_RANK_TYPE_TOTAL:
		{
			this->QueryTotalRankList(role);
		}
		break;

	case CLOUD_ARENA_RANK_TYPE_LOCAL_SERVER:
		{
			this->QueryServerRankList(role);
		}
		break;

	case CLOUD_ARENA_RANK_TYPE_DAN:
		{
			this->QueryDanRankList(role);
		}
		break;

	default:
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		}
		break;
	}
}

void CloudArena::QueryTotalRankList(Role* role)
{
	Protocol::SCCloudArenaRankList msg;
	m_rank_mgr.AssembleTotalRankMsg(role->GetUID(), &msg);

	int send_len = sizeof(msg) - sizeof(msg.rank_list) + msg.rank_num * sizeof(msg.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_len);
}

void CloudArena::QueryDanRankList(Role* role)
{
	const CloudArenaUser* user = this->GetUser(role->GetUID());
	if (NULL == user) return;

	int dan_type = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanTypeByScore(user->GetScore());
	Protocol::SCCloudArenaRankList msg;
	m_rank_mgr.AssembleDanRankMsg(role->GetUID(), dan_type, &msg);

	int send_len = sizeof(msg) - sizeof(msg.rank_list) + msg.rank_num * sizeof(msg.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_len);
}

void CloudArena::QueryServerRankList(Role* role)
{
	Protocol::SCCloudArenaRankList msg;
	m_rank_mgr.AssembleServerRankMsg(role->GetUID(), &msg);

	int send_len = sizeof(msg) - sizeof(msg.rank_list) + msg.rank_num * sizeof(msg.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, send_len);
}

void CloudArena::FetchRankingReward(Role* role, int rank_id)
{
	const CloudArenaRankCfg* rank_cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetRankCfgByRankId(rank_id);
	if (NULL == rank_cfg)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user) return;

	if (!user->HasReachRanking(rank_id))
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_RANKING_NOT_REACH);
		return;
	}

	if (user->HasFetchRankingReward(rank_id))
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_RANKING_REWARD_HAS_FETCHED);
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList(rank_cfg->rank_reward_num, rank_cfg->rank_reward_list))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->PutList(rank_cfg->rank_reward_num, rank_cfg->rank_reward_list, PUT_REASON_CLOUD_ARENA_RANK_REWARD))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(rank_cfg->rank_reward_arena_money, MONEY_TYPE_CLOUD_ARENA_COIN, __FUNCTION__);

	user->OnFetchRankingReward(rank_id);
	this->SetUserDirty(user);
	this->SendUserSelfInfo(role->GetUID(), user, role);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::FetchRankingReward user[%d,%s] rank_id[%d]",
		role->GetUID(), role->GetName(), rank_id);
}

void CloudArena::ReleaseSeasonReward()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		int rank = user->GetSeasonFinalRank();
		
		this->SendSeasonRewardMail(user->GetUID(), rank, user->GetScore(), 
			m_param.season_info.season_idx);
	}
}

void CloudArena::SettleSeasonFinalRank()
{
	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		int rank = m_rank_mgr.GetUserRank(user->GetUID());
		user->SetSeasonFinalRank(rank);
		this->SetUserDirty(user);
	}
}

bool CloudArena::SendSeasonRewardMail(int uid, int rank, int score, int season_idx)
{
	const CloudArenaSeasonRewardCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetSeasonRewardCfg(rank);
	if (NULL == cfg)
	{
		gamelog::g_log_cloud_arena.printf(LL_WARNING, "CloudArena::SendSeasonRewardMail \
			GetSeasonRewardCfg ERROR! user[%d] rank[%d] score[%d] season_idx[%d]",
			uid, rank, score, season_idx);
		return false;
	}

	static MailContentParam contentparam;
	contentparam.Reset();
	int reward_count = 0;
	for (int i = 0; i < cfg->reward_num && reward_count < ARRAY_ITEM_COUNT(contentparam.item_list); ++i)
	{
		contentparam.item_list[reward_count].item_id = cfg->reward_list[i].item_id;
		contentparam.item_list[reward_count].num = cfg->reward_list[i].num;
		contentparam.item_list[reward_count].is_bind = cfg->reward_list[i].is_bind ? 1 : 0;
		++reward_count;
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_cloud_arena_season_reward_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_cloud_arena_season_reward_content, season_idx, score, rank + 1);
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(uid, MAIL_REASON_DEFAULT, contentparam);
	}

	gamelog::g_log_cloud_arena.printf(LL_WARNING, "CloudArena::SendSeasonRewardMail \
	 user[%d] rank[%d] score[%d] season_idx[%d]", uid, rank, score, season_idx);

	return true;
}

void CloudArena::SendSeasonInitialScoreNotice(int uid, const CloudArenaUser* user /*= NULL*/, Role* role /*= NULL*/)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	if (user->IsRobot()) return;

	Protocol::SCCloudArenaNoticeSeasonInitialScore msg;
	msg.season_initial_score = user->GetSeasonInitialScore();

	if (NULL == role)
	{
		role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	}

	if (NULL != role)
	{
		bool ret = EngineAdapter::Instance().NetSend(role->GetNetId(), &msg, sizeof(msg));
		gamelog::g_log_cloud_arena.printf(LL_INFO, "SendSeasonInitialScoreNotice role[%d,%s] initial_score[%d] ret[%s]",
			user->GetUID(), user->GetName(), msg.season_initial_score, ret ? "True" : "False");
	}
}

void CloudArena::OnNoticeSeasonInitialScore(Role* role)
{
	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user) return;

	user->OnNoticeSeasonInitialScore();

	this->SetUserDirty(user);
}

void CloudArena::SendUserDanTypeChangeNotice(int uid, int old_dan_type, int new_dan_type, const CloudArenaUser* user /*= NULL*/)
{
	if (NULL == user)
	{
		user = this->GetUser(uid);
		if (NULL == user) return;
	}

	Protocol::SCCloudArenaNoticeDanChange notice;
	notice.old_dan = static_cast<short>(old_dan_type);
	notice.new_dan = static_cast<short>(new_dan_type);

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &notice, sizeof(notice));
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(uid, &notice, sizeof(notice));
	}
}

void CloudArena::CheckAllUserRank(unsigned long interval, unsigned int now_second)
{
	if (now_second < m_next_update_rank_time)
	{
		return;
	}

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		int rank = m_rank_mgr.GetUserRank(it->first);
		user->OnRankUpdate(rank);
	}

	m_next_update_rank_time = now_second + 60;
}

void CloudArena::FetchAchievementReward(Role* role, int achieve_type)
{
	if (NULL == role) return;

	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const CloudArenaAchievementItem* achieve = user->GetAchievementItem(achieve_type);
	if (NULL == achieve)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (CLOUD_ARENA_ACHIEVE_STATUS_ALREADY_CLAIM == achieve->reward_status)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_ALREADY_FETCH_ACHIVEMENT);
		return;
	}

	if (CLOUD_ARENA_ACHIEVE_STATUS_NOT_FINISH == achieve->reward_status)
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_ACHIEVEMENT_NOT_FETCHABLE);
		return;
	}

	const CloudArenaAchievementCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetAchievementCfg(achieve_type, achieve->cur_achievement_lv);
	if (NULL == cfg)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (cfg->is_hide)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList(cfg->reward_num, cfg->reward_list))
	//{
	//	role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
	//	return;
	//}

	//if (!role->GetRoleModuleManager()->GetKnapsack()->PutList(cfg->reward_num, cfg->reward_list, PUT_REASON_CLOUD_ARENA_ACHIEVE_REWARD))
	//{
	//	role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
	//	return;
	//}

	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->reward_arena_coin, MONEY_TYPE_CLOUD_ARENA_COIN, __FUNCTION__);

	user->OnFetchAchievementReward(achieve_type);
	this->SetUserDirty(user);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::FetchAchievementReward role[%d,%s] achieve_type[%d] status[%d] cur_achieve_lv[%d] process_num[%d]",
		role->GetUID(), role->GetName(), achieve_type, achieve->reward_status, achieve->cur_achievement_lv, achieve->process_num);
}

void CloudArena::FetchDayReward(Role* role)
{
	if (NULL == role) return;

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (CLOUD_ARENA_ACHIEVE_STATUS_ALREADY_CLAIM == user->GetDayRewardStatus())
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_ALREADY_FETCH_DAY_REWARD);
		return;
	}

	if (CLOUD_ARENA_DAY_REWARD_INVALID == user->GetDayRewardStatus())
	{
		role->NoticeNum(errornum::EN_CLOUD_ARENA_DAY_REWARD_NOT_FETCHABLE);
		return;
	}

	int score = user->GetScore();
	const CloudArenaRankCfg* cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetRankCfg(score);
	if (NULL == cfg)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->day_reward_bind_coin, __FUNCTION__);
	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->day_reward_arena_money, MONEY_TYPE_CLOUD_ARENA_COIN, __FUNCTION__);

	user->OnFetchDayReward();
	this->SetUserDirty(user);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArena::FetchDayReward role[%d,%s] score[%d] reward_status[%d]",
		role->GetUID(), role->GetName(), score, user->GetDayRewardStatus());
}

void CloudArena::QueryFormation(Role* role)
{
	if (NULL == role) return;

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (role->GetRoleModuleManager()->GetPetHelperFormation()->IsFormationValid(PET_HELPER_FORMATION_CLOUD_ARENA))
	{
		role->GetRoleModuleManager()->GetPetHelperFormation()->OnRoleQueryFormation(PET_HELPER_FORMATION_CLOUD_ARENA);
		return;
	}

	this->CheckFormation(user, role);
	this->SendUserFormationInfo(role->GetUID(), user, role);
}

void CloudArena::CheckAllUserSeasonTimestamp()
{
	// 1. 防止赛季变更后服务器崩溃，导致可能出现的CloudArena成功变更赛季但玩家的
	// 赛季数据没有变更成功的问题
	// 2. 同时也可以通过改变数据库中的season_timestamp实现强制CloudArena或
	// CloudArenaUser赛季变更的操作

	UserMap::iterator it = m_user_map.begin();
	for (; it != m_user_map.end(); ++it)
	{
		CloudArenaUser* user = it->second;
		if (user->IsRobot()) continue;

		if (user->CheckSeasonTimestamp())
		{
			this->SetUserDirty(user);
		}
	}
}

void CloudArena::CheckFormation(CloudArenaUser* user, Role* role)
{
	user->CheckFormation(role);
}

void CloudArena::AssembleFormationInfo(ARG_OUT Protocol::SCPetHelperFormationInfo* info, const RoleBattleData& battle_data)
{
	info->system_type = PET_HELPER_FORMATION_CLOUD_ARENA;

	int role_grid = 2;
	if (1 == battle_data.role_stand_row)
	{
		role_grid = 7;
	}

	info->grid_list[role_grid].id = battle_data.role_character_data.character_id;
	info->grid_list[role_grid].type = PET_HELPER_FORMATION_GRID_ROLE;
	info->grid_list[role_grid].pet_exclusive_skill_disable_flag = 0;
	info->grid_list[role_grid].lian_yao_exclusive_skill_disable_flag = 0;
	info->grid_list[role_grid].unique_id = 0;

	int pet_grid = 7;
	if (1 == battle_data.role_stand_row)
	{
		pet_grid = 2;
	}

	if (battle_data.fight_pet_idx >= 0 && battle_data.fight_pet_idx < battle_data.pet_num)
	{
		info->grid_list[pet_grid].id = battle_data.pet_character_data[battle_data.fight_pet_idx].battle_data.character_id;
		info->grid_list[pet_grid].type = PET_HELPER_FORMATION_GRID_PET;
		info->grid_list[pet_grid].pet_exclusive_skill_disable_flag = 0;
		info->grid_list[pet_grid].lian_yao_exclusive_skill_disable_flag = 0;
		info->grid_list[pet_grid].unique_id = battle_data.pet_character_data[battle_data.fight_pet_idx].unique_id;
	}

	int standby_num = 0;
	for (int i = 0; i < battle_data.pet_num && standby_num < ARRAY_LENGTH(info->standby_list); ++i)
	{
		if (battle_data.fight_pet_idx == i) continue;

		info->standby_list[standby_num].pet_id = battle_data.pet_character_data[i].battle_data.character_id;
		info->standby_list[standby_num].unique_id = battle_data.pet_character_data[i].unique_id;
		info->standby_list[standby_num].pet_exclusive_skill_disable_flag = battle_data.pet_character_data[i].pet_exclusive_skill_disable_flag;
		info->standby_list[standby_num].lian_yao_exclusive_skill_disable_flag = battle_data.pet_character_data[i].lian_yao_exclusive_skill_disable_flag;
		standby_num += 1;
	}

	for (int i = 0; i < battle_data.partner_num; ++i)
	{
		int row = battle_data.partner_character_data[i].battle_row;
		int column = battle_data.partner_character_data[i].battle_column;

		int grid_idx = row * ROW_CHARACTER_NUM + column;
		if (grid_idx < 0 || grid_idx >= ARRAY_LENGTH(info->grid_list) || info->grid_list[grid_idx].id > 0)
		{
			continue;
		}

		info->grid_list[grid_idx].id = battle_data.partner_character_data[i].battle_data.character_id;
		info->grid_list[grid_idx].type = PET_HELPER_FORMATION_GRID_PARTNER;
		info->grid_list[grid_idx].pet_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].lian_yao_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].unique_id = 0;
	}

	for (int i = 0; i < battle_data.pet_helper_num; ++i)
	{
		int row = battle_data.pet_helper_character_data[i].battle_row;
		int column = battle_data.pet_helper_character_data[i].battle_column;

		int grid_idx = row * ROW_CHARACTER_NUM + column;
		if (grid_idx < 0 || grid_idx >= ARRAY_LENGTH(info->grid_list) || info->grid_list[grid_idx].id > 0)
		{
			continue;
		}
		
		info->grid_list[grid_idx].id = battle_data.pet_helper_character_data[i].pet_data.battle_data.character_id;
		info->grid_list[grid_idx].type = PET_HELPER_FORMATION_GRID_PET;
		info->grid_list[grid_idx].pet_exclusive_skill_disable_flag = battle_data.pet_helper_character_data[i].pet_data.pet_exclusive_skill_disable_flag;
		info->grid_list[grid_idx].lian_yao_exclusive_skill_disable_flag = battle_data.pet_helper_character_data[i].pet_data.lian_yao_exclusive_skill_disable_flag;
		info->grid_list[grid_idx].unique_id = battle_data.pet_helper_character_data[i].pet_data.unique_id;
	}

	info->auto_info.battle_auto_first_move_type = battle_data.auto_first_move_type;
	info->auto_info.battle_auto_first_skill_id = battle_data.auto_first_skill_id;
	info->auto_info.battle_auto_first_skill_level = battle_data.auto_first_skill_level;
	info->auto_info.battle_auto_second_move_type = battle_data.auto_second_move_type;
	info->auto_info.battle_auto_second_skill_id = battle_data.auto_second_skill_id;
	info->auto_info.battle_auto_second_skill_level = battle_data.auto_second_skill_level;
}

void CloudArena::AssembleFormationInfo(ARG_OUT Protocol::CSSetPetHelperFormation* info, const RoleBattleData& battle_data)
{
	int role_grid = 2;
	if (1 == battle_data.role_stand_row)
	{
		role_grid = 7;
	}

	info->grid_list[role_grid].id = battle_data.role_character_data.character_id;
	info->grid_list[role_grid].type = PET_HELPER_FORMATION_GRID_ROLE;
	info->grid_list[role_grid].pet_exclusive_skill_disable_flag = 0;
	info->grid_list[role_grid].lian_yao_exclusive_skill_disable_flag = 0;
	info->grid_list[role_grid].unique_id = 0;

	int pet_grid = 7;
	if (1 == battle_data.role_stand_row)
	{
		pet_grid = 2;
	}

	if (battle_data.fight_pet_idx >= 0 && battle_data.fight_pet_idx < battle_data.pet_num)
	{
		info->grid_list[pet_grid].id = battle_data.pet_character_data[battle_data.fight_pet_idx].battle_data.character_id;
		info->grid_list[pet_grid].type = PET_HELPER_FORMATION_GRID_PET;
		info->grid_list[pet_grid].pet_exclusive_skill_disable_flag = 0;
		info->grid_list[pet_grid].lian_yao_exclusive_skill_disable_flag = 0;
		info->grid_list[pet_grid].unique_id = battle_data.pet_character_data[battle_data.fight_pet_idx].unique_id;
	}

	int standby_num = 0;
	for (int i = 0; i < battle_data.pet_num && standby_num < ARRAY_LENGTH(info->standby_list); ++i)
	{
		if (battle_data.fight_pet_idx == i) continue;

		info->standby_list[standby_num].pet_id = battle_data.pet_character_data[i].battle_data.character_id;
		info->standby_list[standby_num].unique_id = battle_data.pet_character_data[i].unique_id;
		standby_num += 1;
	}

	for (int i = 0; i < battle_data.partner_num; ++i)
	{
		int row = battle_data.partner_character_data[i].battle_row;
		int column = battle_data.partner_character_data[i].battle_column;

		int grid_idx = row * ROW_CHARACTER_NUM + column;
		if (grid_idx < 0 || grid_idx >= ARRAY_LENGTH(info->grid_list) || info->grid_list[grid_idx].id > 0)
		{
			continue;
		}

		info->grid_list[grid_idx].id = battle_data.partner_character_data[i].battle_data.character_id;
		info->grid_list[grid_idx].type = PET_HELPER_FORMATION_GRID_PARTNER;
		info->grid_list[grid_idx].pet_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].lian_yao_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].unique_id = 0;
	}

	for (int i = 0; i < battle_data.pet_helper_num; ++i)
	{
		int row = battle_data.pet_helper_character_data[i].battle_row;
		int column = battle_data.pet_helper_character_data[i].battle_column;

		int grid_idx = row * ROW_CHARACTER_NUM + column;
		if (grid_idx < 0 || grid_idx >= ARRAY_LENGTH(info->grid_list) || info->grid_list[grid_idx].id > 0)
		{
			continue;
		}

		info->grid_list[grid_idx].id = battle_data.pet_helper_character_data[i].pet_data.battle_data.character_id;
		info->grid_list[grid_idx].type = PET_HELPER_FORMATION_GRID_PET;
		info->grid_list[grid_idx].pet_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].lian_yao_exclusive_skill_disable_flag = 0;
		info->grid_list[grid_idx].unique_id = battle_data.pet_helper_character_data[i].pet_data.unique_id;
	}

	info->auto_info.battle_auto_first_move_type = battle_data.auto_first_move_type;
	info->auto_info.battle_auto_first_skill_id = battle_data.auto_first_skill_id;
	info->auto_info.battle_auto_first_skill_level = battle_data.auto_first_skill_level;
	info->auto_info.battle_auto_second_move_type = battle_data.auto_second_move_type;
	info->auto_info.battle_auto_second_skill_id = battle_data.auto_second_skill_id;
	info->auto_info.battle_auto_second_skill_level = battle_data.auto_second_skill_level;
}

void CloudArena::SyncSeasonInfoToGame(const UniqueServerID* mother_sid)
{
	crossgameprotocal::HiddenGameSyncCloudArenaSeasonInfo info;
	info.cur_season_start_timestamp = m_param.season_info.season_timestamp;
	info.cur_season_end_timestamp = m_param.season_info.season_end_timestamp;

	if (NULL != mother_sid)
	{
		InternalComm::Instance().SendToGameThroughCross(mother_sid->plat_type, mother_sid->server_id, (const char*)&info, sizeof(info));
	}
	else
	{
		InternalComm::Instance().SendToAllGameThroughCross((const char*)&info, sizeof(info));
	}
}

void CloudArena::SyncRoleBattleData(Role* role)
{
	if (NULL == role) return;

	CloudArenaUser* user = this->MutableUser(role->GetUID());
	if (NULL == user) return;

	Protocol::CSSetPetHelperFormation req;
	req.system_type = PET_HELPER_FORMATION_CLOUD_ARENA;
	memset(req.grid_list, 0, sizeof(req.grid_list));
	memset(req.standby_list, 0, sizeof(req.standby_list));
	this->AssembleFormationInfo(&req, user->GetBattleData());

	if (user->OnRoleSetFormation(&req, role))
	{
		this->SetUserDirty(user);
		m_rank_mgr.OnUserUpdateRankInfo(user);
	}
}

