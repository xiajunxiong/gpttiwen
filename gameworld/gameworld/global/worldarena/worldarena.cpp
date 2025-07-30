#include "worldarena.hpp"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "worldarenasignup.hpp"
#include "worldarenaschedule.hpp"
#include "worldarenausermgr.hpp"
#include "gamelog.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "worldarenabattlemgr.hpp"
#include "worldarenauser.hpp"
#include "protocol/msgworldarena.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "global/guild/guild.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/rank/personrank.hpp"
#include "worldarenagrouphash.hpp"
#include "servercommon/errornum.h"
#include "rmiclient/rmiglobal.hpp"
#include "internalcomm.h"
#include "global/rmibackobjdef.h"
#include "scene/scenemanager.hpp"
#include "scene/speciallogic.hpp"
#include "scene/scene.h"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "global/worldarena/worldarenaconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "engineadapter.h"
#include "scene/speciallogicimpl/speciallogicworldarenstandbyroom.hpp"
#include "servercommon/noticenum.h"
#include "other/rolecross/rolecross.hpp"
#include "global/team/team.hpp"

void RMISaveWorldArenaBaseDataBackObjectImpl::SaveRet(int ret)
{
	if (0 != ret)
	{
		gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::SaveRet ret:%d", ret);
		gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::SaveRet ret:%d", ret);
	}
}

void RMISaveWorldArenaBaseDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::__exception error:%d", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::__exception error:%d", error);
}

void RMISaveWorldArenaBaseDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaBaseDataBackObjectImpl::__timeout");
}

void RMIInitWorldArenaBaseDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaBaseDataBackObjectImpl::__exception error:%d", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaBaseDataBackObjectImpl::__exception error:%d", error);
}

void RMIInitWorldArenaBaseDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaBaseDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaBaseDataBackObjectImpl::__timeout");
}

void RMIInitWorldArenaBaseDataBackObjectImpl::InitRet(int ret, const WorldArenaBaseParam& param)
{
	WorldArena::Instance().OnLoadBaseDataReturn(ret, param);
}

void RMIInitWorldArenaUserDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaUserDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaUserDataBackObjectImpl::__exception error[%d]", error);
}

void RMIInitWorldArenaUserDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaUserDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaUserDataBackObjectImpl::__timeout");
}

void RMIInitWorldArenaUserDataBackObjectImpl::InitRet(int ret, const WorldArenaUserListParamForInit& param)
{
	WorldArena::Instance().OnLoadUserReturn(ret, param);
}

void RMISaveWorldArenaUserDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::__exception error[%d]", error);
}

void RMISaveWorldArenaUserDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::__timeout");
}

void RMISaveWorldArenaUserDataBackObjectImpl::SaveRet(int ret)
{
	if (0 != ret)
	{
		gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::SaveRet ret:%d", ret);
		gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaUserDataBackObjectImpl::SaveRet ret:%d", ret);
	}
	else
	{
		WorldArena::Instance().OnAsyncSaveUserDataReturnSucc(m_delete_list);
	}
}

void RMIInitWorldArenaScheduleDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaScheduleDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaScheduleDataBackObjectImpl::__exception error[%d]", error);
}

void RMIInitWorldArenaScheduleDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMIInitWorldArenaScheduleDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMIInitWorldArenaScheduleDataBackObjectImpl::__timeout");
}

void RMIInitWorldArenaScheduleDataBackObjectImpl::InitRet(int ret, const WorldArenaScheduleListParamForInit& param)
{
	WorldArena::Instance().OnLoadScheduleReturn(ret, param);
}

void RMISaveWorldArenaScheduleDataBackObjectImpl::__exception(int error)
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::__exception error[%d]", error);
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::__exception error[%d]", error);
}

void RMISaveWorldArenaScheduleDataBackObjectImpl::__timeout()
{
	gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::__timeout");
	gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::__timeout");
}

void RMISaveWorldArenaScheduleDataBackObjectImpl::SaveRet(int ret)
{
	if (0 != ret)
	{
		gamelog::g_log_world_arena.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::SaveRet ret:%d", ret);
		gamelog::g_log_serious_error.printf(LL_WARNING, "RMISaveWorldArenaScheduleDataBackObjectImpl::SaveRet ret:%d", ret);
	}
}

WorldArena& WorldArena::Instance()
{
	static WorldArena instance;
	return instance;
}

void WorldArena::Update(unsigned int now)
{
	if (!this->IsLoadAllFinish())
	{
		return;
	}

	//本地服允许WorldArena，方便测试
#ifndef _MSC_VER 
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
#endif

	if (0u == m_check_save_timestamp)
	{
		m_check_save_timestamp = now + 30;
	}

	if (now > m_check_save_timestamp)
	{
		this->SaveChanged();
		m_check_save_timestamp = now + 30;
	}

	if (m_base_param.next_status_timestamp < now)
	{
		this->EnterNextStatus();
		return;
	}

	if (this->IsInBattleStatus() && this->IsInRoundBattleTime(now, m_base_param.round_battle_start_timestamp))
	{
		if (!m_battle_mgr->HasCreateScenesForToday() && this->IsInRoundBattleStandbyTime(now, m_base_param.round_battle_start_timestamp))
		{
			this->CreateBattleScene();
			return;
		}
	}

	m_schedule_module->Update(now);
}

bool WorldArena::IsLoadAllFinish()
{
	return m_load_all_finished ||
		(this->IsLoadBaseDataFinish() && m_user_mgr->IsLoadFinish() && m_schedule_module->IsLoadFinish());
}

bool WorldArena::IsLoadBaseDataFinish()
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_base_data_status;
}

void WorldArena::OnUserLogin(const UserID& user_id)
{
	//本地服允许WorldArena，方便测试
#ifndef _MSC_VER 
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
#endif

	this->SendWorldArenaInfo(user_id);
	this->SendWorldArenaUserSignupStatus(user_id);
	this->SendUserBaseInfo(user_id);

	this->CheckTodayGiveup(user_id);
	this->CheckAndSendNoticeEnterScene(user_id);

	this->SyncUserInfo(user_id);
}

void WorldArena::OnUserLoginOrigin(const UserID& user_id)
{
	this->SendWorldArenaInfo(user_id);
	this->SendWorldArenaUserSignupStatus(user_id);
	this->SendUserBaseInfo(user_id);

	this->CheckTodayGiveup(user_id);
	this->CheckAndSendNoticeEnterScene(user_id);
}

void WorldArena::OnUserLogout(const UserID& user_id)
{
#ifndef _MSC_VER
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
#endif

	this->SyncUserInfo(user_id);
}

void WorldArena::OnRoleOperate(Role* role, int oper_type, int param1, int param2)
{
	bool need_transfer = false;

	//本地服允许WorldArena，方便测试
#ifndef _MSC_VER 
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		switch (oper_type)
		{
		case Protocol::CSWorldArenaOperate::OPER_TYPE_SIGNUP:
			{
				this->TransferSignupOperateToHidden(role);
				return;
			}
			break;
		case Protocol::CSWorldArenaOperate::OPER_TYPE_ENTER_SCENE:
			{
				this->TryEnterSceneInOrigin(role);
				return;
			}
			break;
		case Protocol::CSWorldArenaOperate::OPER_TYPE_STANDBY_FOR_FIGHT:
			{
				role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
				return;
			}
			break;

		default:
			{
				need_transfer = true;
			}
			break;
		}
	}
#endif

	if (need_transfer)
	{
		this->TransferOperateToHidden(role, oper_type, param1, param2);
		return;
	}

	switch (oper_type)
	{
	case Protocol::CSWorldArenaOperate::OPER_TYPE_INFO:
		{
			this->SendWorldArenaInfo(role->GetUserId());
			this->SendWorldArenaUserSignupStatus(role->GetUserId());
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_SIGNUP:
		{
			WorldArenaSignUpInfo info;
			this->AssembleUserSignUpInfo(role, &info);

			RoleBattleData rbd;
			role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, true);

			this->OnUserSignUp(role->GetUserId(), info, rbd);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_SCHEDULE_INFO:
		{
			this->SendUserScheduleInfo(role->GetUserId());
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_USER_BASE_INFO:
		{
			this->SendUserBaseInfo(role->GetUserId());
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_ENTER_SCENE:
		{
			this->EnterScene(role);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_WIN_BOX:
		{
			this->FetchWinBoxReward(role->GetUserId(), param1);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_SIGNUP_REWARD:
		{
			this->FetchSignupReward(role->GetUserId());
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_DAILY_REWARD:
		{
			this->FetchDailyReward(role->GetUserId());
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_GROUP_RANK_LIST:
		{
			this->QueryGroupRankList(role->GetUserId(), param1);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_STANDBY_FOR_FIGHT:
		{
			//暂时弃用 现在进场就自动准备了
			//this->StandbyForFight(role, param1 > 0);
		}
		break;
	}
}

void WorldArena::TransferOperateToHidden(Role* role, int oper_type, int param1, int param2)
{
	crossgameprotocal::GameHiddenWorldArenaOperate msg;
	msg.uid = role->GetUID();
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.oper_type = oper_type;
	msg.param1 = param1;
	msg.param2 = param2;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
}

void WorldArena::TransferSignupOperateToHidden(Role* role)
{
	crossgameprotocal::GameHiddenWorldArenaSignup msg;
	this->AssembleUserSignUpInfo(role, &msg.signup_info);
	role->GetRoleModuleManager()->CollectRoleBattleData(&msg.role_battle_data, true);

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
}

void WorldArena::OnUserOperateFromOrigin(const UserID& user_id, int oper_type, int param1, int param2)
{
	switch (oper_type)
	{
	case Protocol::CSWorldArenaOperate::OPER_TYPE_INFO:
		{
			this->SendWorldArenaInfo(user_id);
			this->SendWorldArenaUserSignupStatus(user_id);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_SCHEDULE_INFO:
		{
			this->SendUserScheduleInfo(user_id);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_USER_BASE_INFO:
		{
			this->SendUserBaseInfo(user_id);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_GROUP_RANK_LIST:
		{
			this->QueryGroupRankList(user_id, param1);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_WIN_BOX:
		{
			this->FetchWinBoxReward(user_id, param1);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_SIGNUP_REWARD:
		{
			this->FetchSignupReward(user_id);
		}
		break;

	case Protocol::CSWorldArenaOperate::OPER_TYPE_FETCH_DAILY_REWARD:
		{
			this->FetchDailyReward(user_id);
		}
		break;

	default:
		{
			World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_OPERATE_NEED_IN_CROSS);
		}
		break;
	}
}

void WorldArena::OnUserSignUp(const UserID& user_id, const WorldArenaSignUpInfo& info, const RoleBattleData& rbd)
{
	if (m_signup_module->HasUserSignedUp(info.uuid))
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_ALREADY_SIGNED_UP);
		return;
	}

	bool succ = m_user_mgr->OnUserSignUp(info, rbd);
	if (succ)
	{
		m_signup_module->OnUserSignUp(info.uuid);
		this->SendWorldArenaUserSignupStatus(user_id);

		World::GetInstWorld()->SendNoticeNumToUser(user_id, noticenum::NT_WORLD_ARENA_SIGNUP_SUCC);
	}
}

bool WorldArena::IsInBattleStatus()
{
	return WORLD_ARENA_STATUS_BATTLE == m_base_param.cur_status;
}

bool WorldArena::IsInSignupStatus()
{
	return WORLD_ARENA_STATUS_SIGNUP == m_base_param.cur_status;
}

bool WorldArena::IsInRoundBattleTime(unsigned int now, unsigned int round_battle_start_timestamp)
{
	return now >= round_battle_start_timestamp && now < round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_DURATION_S;
}

bool WorldArena::IsInRoundBattleStandbyTime(unsigned int now, unsigned int round_battle_start_timestamp)
{
	return now >= round_battle_start_timestamp && now < round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_STANDBY_TIME_S;
}

bool WorldArena::IsInRoundBattleFightTime(unsigned int now, unsigned int round_battle_start_timestamp)
{
	// 等待时间也算是战斗时间中，因为等待场景中两人只要都到场就可以开始
	return now >= round_battle_start_timestamp
		&& now < round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_DURATION_S;
}

bool WorldArena::IsRobot(long long uuid)
{
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL != user)
	{
		return user->IsRobot();
	}

	return false;
}

const RoleBattleData* WorldArena::GetRobotBattleData(long long uuid)
{
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		return NULL;
	}

	return &user->GetRoleBattleData();
}

bool WorldArena::AssembleRobotParam(long long uuid, RobotParam& param)
{
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		return NULL;
	}
	assert(user->IsRobot());

	long long group_id = user->GetGroupID();
	int average_lv = this->CalculateAverageLevelInGroup(group_id);

	user->AssembleRobotParam(param, average_lv);

	if (user->GetLevel() != average_lv)
	{
		user->UpdateRobotLevel(average_lv);
	}
	return true;
}

void WorldArena::SetFightResult(long long schedule_id, int result, int round_num, bool is_attacker_giveup /* = false */, bool is_defender_giveup /* = false */)
{
	m_schedule_module->SetFightResult(schedule_id, result);

	const WorldArenaScheduleData* info = m_schedule_module->GetScheduleInfo(schedule_id);
	if (NULL == info) return;

	bool is_draw = (WORLD_ARENA_RESULT_DRAW == result);
	bool is_attacker_win = (WORLD_ARENA_RESULT_ATTACKER_WIN == result);
	bool is_attacker_lose = (WORLD_ARENA_RESULT_DEFENDER_WIN == result && !is_attacker_giveup);
	bool is_defender_win = (WORLD_ARENA_RESULT_DEFENDER_WIN == result);
	bool is_defender_lose = (WORLD_ARENA_RESULT_ATTACKER_WIN == result && !is_defender_giveup);
	bool is_both_giveup = (WORLD_ARENA_RESULT_BOTH_GIVEUP == result);
	
	if (is_both_giveup)
	{
		is_attacker_lose = true;
		is_attacker_giveup = true;

		is_defender_lose = true;
		is_defender_giveup = true;
	}

	WorldArenaUser* attacker_user = m_user_mgr->GetWorldArenaUser(info->GetScheduleData().attacker_uuid);
	if (NULL != attacker_user)
	{
		attacker_user->OnSetFightResult(is_attacker_win, is_draw, is_attacker_lose, is_attacker_giveup, round_num);
		if (!attacker_user->IsRobot())
		{
			this->SendUserBaseInfo(IntToUid(attacker_user->GetUID()));
			if (is_attacker_win && is_defender_giveup)
			{
				World::GetInstWorld()->SendNoticeNumToUser(IntToUid(attacker_user->GetUID()), noticenum::NT_WORLD_ARENA_OPPONENT_NOT_SHOW_UP);
			}
			this->CheckAndSendNoticeEnterScene(IntToUid(attacker_user->GetUID()));
			
			if (!is_attacker_giveup)
			{
				attacker_user->SetDailyRewardAvaliable();
				//this->GiveDayReward(IntToUid(attacker_user->GetUID()));
			}
		}
	}

	WorldArenaUser* defender_user = m_user_mgr->GetWorldArenaUser(info->GetScheduleData().defender_uuid);
	if (NULL != defender_user)
	{
		defender_user->OnSetFightResult(is_defender_win, is_draw, is_defender_lose, is_defender_giveup, round_num);
		if (!defender_user->IsRobot())
		{
			this->SendUserBaseInfo(IntToUid(defender_user->GetUID()));
			if (is_defender_win && is_attacker_giveup)
			{
				World::GetInstWorld()->SendNoticeNumToUser(IntToUid(defender_user->GetUID()), noticenum::NT_WORLD_ARENA_OPPONENT_NOT_SHOW_UP);
			}
			this->CheckAndSendNoticeEnterScene(IntToUid(defender_user->GetUID()));
			
			if (!is_defender_giveup)
			{
				defender_user->SetDailyRewardAvaliable();
				//this->GiveDayReward(IntToUid(defender_user->GetUID()));
			}
		}
	}

	if (NULL != attacker_user && !attacker_user->IsRobot())
	{
		this->SendUserBaseInfo(IntToUid(attacker_user->GetUID()));
	}

	if (NULL != defender_user && !defender_user->IsRobot())
	{
		this->SendUserBaseInfo(IntToUid(defender_user->GetUID()));
	}
}

void WorldArena::GmRestartSeason()
{
#ifdef _DEBUG
	m_load_all_finished = true;
#endif

	// 初次启动时，进入休赛期，并在下个月开放
	this->CalculateAllStatusTimestampNextMonth();
	m_base_param.cur_status = WORLD_ARENA_STATUS_REST;
	m_base_param.next_status_timestamp = m_base_param.next_signup_status_timestamp;

	m_signup_module->OnSeasonRestart();
	m_schedule_module->OnSeasonRestart();
	m_user_mgr->OnSeasonRestart();
	m_group_hash->OnSeasonRestart();

	this->SetDirty(true);
	this->SaveAll();
}

void WorldArena::GmNextStatus()
{
	m_base_param.next_status_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void WorldArena::GmStartBattle(int round_idx)
{
	if (!this->IsInBattleStatus())
	{
		return;
	}

	if (round_idx < 0 || round_idx >= WORLD_ARENA_ROUND_NUM)
	{
		return;
	}

	m_battle_mgr->ResetCreateScenesFlag();
	m_base_param.round_battle_start_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_base_param.next_round_battle_start_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + EngineAdapter::Instance().NextDayInterval(WORLD_ARENA_ROUND_BATTLE_START_HOUR, WORLD_ARENA_ROUND_BATTLE_START_MINUTE, 0);
	if (this->IsInRoundBattleTime(m_base_param.round_battle_start_timestamp, m_base_param.next_round_battle_start_timestamp))
	{
		// 确保时间不重叠
		m_base_param.next_round_battle_start_timestamp += SECOND_PER_DAY;
	}
	this->CreateBattleScene(&round_idx);
}

short WorldArena::GetCurSeasonIndex() const
{
	return m_base_param.season_index;
}

void WorldArena::OnAsyncSaveUserDataReturnSucc(const std::vector<long long>& delete_list)
{
	m_user_mgr->ClearDirtyMark();
	m_user_mgr->ClearDeletedUser(delete_list);
}

void WorldArena::OnAsyncSaveScheduleDataReturnSucc(const std::vector<long long>& delete_list)
{
	m_schedule_module->ClearDirtyMark();
	m_schedule_module->ClearDeletedSchedule(delete_list);
}

void WorldArena::OnLoadUserReturn(int ret, const WorldArenaUserListParamForInit& param)
{
	if (0 != ret)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		gamelog::g_log_world_arena.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_1 ret[%d]", __FUNCTION__, ret).c_str());
		return;
	}

	for (size_t i = 0; i < param.user_list.size(); ++i)
	{
		if (!m_user_mgr->AddUserFromInit(param.user_list[i]))
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "%s_2 ret[%d]", __FUNCTION__, ret);
			gamelog::g_log_world_arena.printf(LL_ERROR, "%s_2 ret[%d]", __FUNCTION__, ret);
			ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_2 ret[%d]", __FUNCTION__, ret).c_str());

			return;
		}
		else
		{
			m_signup_module->OnInitSetUserSignup(param.user_list[i].signup_info.uuid);
		}
	}

	if (!param.user_list.empty())
	{
		gamelog::g_log_world_arena.printf(LL_INFO, "OnLoadUserReturn Continue Load User, next_id_from[%lld]", param.next_id_from);
		m_user_mgr->LoadUser(param.next_id_from);
	}
	else
	{
		m_user_mgr->OnLoadUserSucc();
		this->CheckLoadAllFinish();
	}
}

void WorldArena::OnLoadScheduleReturn(int ret, const WorldArenaScheduleListParamForInit& param)
{
	if (0 != ret)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		gamelog::g_log_world_arena.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_1 ret[%d]", __FUNCTION__, ret).c_str());
		return;
	}

	for (size_t i = 0; i < param.schedule_list.size(); ++i)
	{
		if (!m_schedule_module->AddScheduleFromInit(param.schedule_list[i]))
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "%s_2 ret[%d]", __FUNCTION__, ret);
			gamelog::g_log_world_arena.printf(LL_ERROR, "%s_2 ret[%d]", __FUNCTION__, ret);
			ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_2 ret[%d]", __FUNCTION__, ret).c_str());

			return;
		}
	}

	if (!param.schedule_list.empty())
	{
		gamelog::g_log_world_arena.printf(LL_INFO, "OnLoadScheduleReturn Continue Load User, next_id_from[%lld]", param.next_id_from);
		m_schedule_module->LoadSchedule(param.next_id_from);
	}
	else
	{
		m_schedule_module->OnLoadScheduleSucc();
		this->CheckLoadAllFinish();
	}
}

void WorldArena::OnLoadBaseDataReturn(int ret, const WorldArenaBaseParam& param)
{
	if (ret != 0)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		gamelog::g_log_world_arena.printf(LL_ERROR, "%s_1 ret[%d]", __FUNCTION__, ret);
		ServerLogic::GetInstServerLogic()->StopServer(STRING_SPRINTF("function[%s]_1 ret[%d]", __FUNCTION__, ret).c_str());
		return;
	}

	m_base_param = param;
	m_load_base_data_status = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	this->CheckLoadAllFinish();
}

void WorldArena::CheckLoadAllFinish()
{
	if (!m_load_all_finished && this->IsLoadAllFinish())
	{
		m_load_all_finished = true;
		this->OnLoadAllFinish();
	}
}

bool WorldArena::TryReEnterBattleScene(long long uuid, int* out_scene_id, int* out_fb_key)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInRoundBattleTime(now, m_base_param.round_battle_start_timestamp))
	{
		return false;
	}

	SpecialLogic* spl = m_battle_mgr->GetSpecialLogicForUser(uuid);
	if (NULL == spl) return false;

#ifdef _DEBUG
	SpecialLogicWorldArenaStandbyRoom* room = dynamic_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
	assert(NULL != room);
#else
	SpecialLogicWorldArenaStandbyRoom* room = static_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
#endif

	bool is_re_enter_scene = false;
	if (uuid == room->GetAttackerUUID() && room->HasAttackerEverEntered())
	{
		is_re_enter_scene = true;
	}
	else if (uuid == room->GetDefenderUUID() && room->HasDefenderEverEntered())
	{
		is_re_enter_scene = true;
	}

	if (!is_re_enter_scene) return false;

	if (NULL != out_scene_id) *out_scene_id = spl->GetScene()->GetSceneID();
	if (NULL != out_fb_key) *out_fb_key = spl->GetScene()->GetSceneKey();

	return true;
}

WorldArena::WorldArena() : m_load_all_finished(false), m_load_base_data_status(GLOBAL_SYSTEM_DATA_STATE_INVALID),
	m_is_dirty(false), m_signup_module(NULL), m_schedule_module(NULL),
	m_user_mgr(NULL), m_battle_mgr(NULL), m_group_hash(NULL), m_has_battle_scene_created(false),
	m_check_save_timestamp(0u)
{
	m_signup_module = new WorldArenaSignUp(this);
	m_schedule_module = new WorldArenaSchedule(this);
	m_user_mgr = new WorldArenaUserMgr(this);
	m_battle_mgr = new WorldArenaBattleMgr(this);
	m_group_hash = new WorldArenaGroupHash(this);
}

void WorldArena::OnLoadAllFinish()
{
	if (WORLD_ARENA_STATUS_INVALID == m_base_param.cur_status)
	{
		// 初次启动时，进入休赛期，并在下个月开放
		this->CalculateAllStatusTimestampNextMonth();
		m_base_param.cur_status = WORLD_ARENA_STATUS_REST; 
		m_base_param.next_status_timestamp = m_base_param.next_signup_status_timestamp;
		this->SetDirty(true);
	}
	else
	{
		m_group_hash->AllGroupIdHashToIdx(m_schedule_module->GetAllGroupIds());
	}

	if (this->IsInBattleStatus())
	{
		// 给遗漏了的玩家补上赛程
		this->MakeSchedule(true);
	}

	this->LogStatus(__FUNCTION__);
}

void WorldArena::EnterNextStatus()
{
	switch (m_base_param.cur_status)
	{
	case WORLD_ARENA_STATUS_REST:
		{
			this->EnterSignUpStatus();
		}
		break;

	case WORLD_ARENA_STATUS_SIGNUP:
		{
			this->EnterScheduleStatus();
		}
		break;

	case WORLD_ARENA_STATUS_SCHEDULE:
		{
			this->EnterBattleStatus();
		}
		break;

	case WORLD_ARENA_STATUS_BATTLE:
		{
			this->EnterCalculateStatus();
		}
		break;

	case WORLD_ARENA_STATUS_CALCULATE:
		{
			this->EnterRestStatus();
		}
		break;
	}
}

void WorldArena::EnterSignUpStatus()
{
	m_base_param.cur_status = WORLD_ARENA_STATUS_SIGNUP;
	m_base_param.next_status_timestamp = m_base_param.next_schedule_status_timestamp;
	m_base_param.season_index += 1;

	this->LogStatus(__FUNCTION__);

	m_signup_module->OnSeasonRestart();
	m_schedule_module->OnSeasonRestart();
	m_user_mgr->OnSeasonRestart();
	m_group_hash->OnSeasonRestart();

	this->SetDirty(true);
	this->SaveAll();

	this->SendWorldArenaInfoToAll();
	this->SendWorldArenaUserSignupStatusToAllUserOnSeasonStart();
}

void WorldArena::EnterScheduleStatus()
{
	m_base_param.cur_status = WORLD_ARENA_STATUS_SCHEDULE;
	m_base_param.next_status_timestamp = m_base_param.next_battle_status_timestamp;

	this->LogStatus(__FUNCTION__);

	this->MakeSchedule(false);

	this->SetDirty(true);

	this->SendWorldArenaInfoToAll();

	m_user_mgr->SendScheduleReleaseNoticeMailToAllUsers();
}

void WorldArena::EnterBattleStatus()
{
	m_base_param.cur_status = WORLD_ARENA_STATUS_BATTLE;
	m_base_param.next_status_timestamp = m_base_param.next_calculate_status_timestamp;

	m_base_param.round_battle_start_timestamp = 
		m_base_param.next_battle_status_timestamp 
		+ WORLD_ARENA_ROUND_BATTLE_START_HOUR * SECOND_PER_HOUR
		+ WORLD_ARENA_ROUND_BATTLE_START_MINUTE * SECOND_PER_MIN;

	m_base_param.next_round_battle_start_timestamp = m_base_param.round_battle_start_timestamp + SECOND_PER_DAY;

	this->LogStatus(__FUNCTION__);

	this->SetDirty(true);

	this->SendWorldArenaInfoToAll();
}

void WorldArena::EnterCalculateStatus()
{
	m_base_param.cur_status = WORLD_ARENA_STATUS_CALCULATE;
	m_base_param.next_status_timestamp = m_base_param.next_rest_status_timestamp;

	m_base_param.round_battle_start_timestamp = 0u;
	m_base_param.next_round_battle_start_timestamp = 0u;

	this->LogStatus(__FUNCTION__);

	this->CalculateSeasonRankReward();

	this->SetDirty(true);

	this->SaveAll();

	this->SendWorldArenaInfoToAll();
}

void WorldArena::EnterRestStatus()
{
	m_base_param.cur_status = WORLD_ARENA_STATUS_REST;
	this->CalculateAllStatusTimestampNextMonth();
	m_base_param.next_status_timestamp = m_base_param.next_signup_status_timestamp;

	this->LogStatus(__FUNCTION__);

	this->SetDirty(true);

	this->SendWorldArenaInfoToAll();
}

void WorldArena::CalculateAllStatusTimestampNextMonth()
{
	m_base_param.next_signup_status_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	m_base_param.next_schedule_status_timestamp = m_base_param.next_signup_status_timestamp + 5 * SECOND_PER_DAY; 
	m_base_param.next_battle_status_timestamp = m_base_param.next_signup_status_timestamp + 5 * SECOND_PER_DAY;
	m_base_param.next_calculate_status_timestamp = m_base_param.next_signup_status_timestamp + 24 * SECOND_PER_DAY;
	m_base_param.next_rest_status_timestamp = m_base_param.next_signup_status_timestamp + 24 * SECOND_PER_DAY;

	this->SetDirty(true);
}

void WorldArena::MakeSchedule(bool is_schedule_for_omit_users)
{
	std::vector<const WorldArenaSignUpInfo*> signup_vec = this->ExtractAndShuffleAllSignUpInfo(is_schedule_for_omit_users);

	//总共分组数量 = （1）不整除时：(总人数 + 20) / 20， （2）整除时：总人数 / 20
	int total_signup_num = static_cast<int>(signup_vec.size());
	int total_group_num = total_signup_num / WORLD_ARENA_FIGHTER_NUM_PER_GROUP;
	if (total_signup_num % WORLD_ARENA_FIGHTER_NUM_PER_GROUP != 0)
	{
		total_group_num += 1;
	}
	if (total_group_num <= 0)
	{
		return;
	}
	int fighter_num_per_group = total_signup_num / total_group_num;
	
	std::vector<long long> group_id_list;
	group_id_list.reserve(total_group_num);

	std::vector<const WorldArenaSignUpInfo*> group_fighter_vec;
	group_fighter_vec.reserve(fighter_num_per_group);
	for (size_t i = 0; i < signup_vec.size(); ++i)
	{
		group_fighter_vec.push_back(signup_vec[i]);
		if (group_fighter_vec.size() == fighter_num_per_group
			|| i == signup_vec.size() - 1)
		{
			// 完成一次分组
			long long group_id = this->MakeScheduleForGroup(group_fighter_vec);
			if (group_id > 0)
			{
				group_id_list.push_back(group_id);
			}

			group_fighter_vec.clear();
		}
	}

	bool is_clear_old_hash = !is_schedule_for_omit_users;
	m_group_hash->AllGroupIdHashToIdx(group_id_list, is_clear_old_hash);

	this->SaveAll();
}

int CalculateAverageLevel(const std::vector<const WorldArenaSignUpInfo*>& group_fighter_vec)
{
	if (group_fighter_vec.empty()) return 0;

	int total_level = 0;
	for (size_t i = 0; i < group_fighter_vec.size(); ++i)
	{
		total_level += group_fighter_vec[i]->level;
	}
	return total_level / static_cast<int>(group_fighter_vec.size());
}

long long WorldArena::MakeScheduleForGroup(const std::vector<const WorldArenaSignUpInfo*>& group_fighter_vec)
{
	std::vector<WorldArenaUser*> user_vec;
	user_vec.reserve(WORLD_ARENA_FIGHTER_NUM_PER_GROUP);

	for (size_t i = 0; i < group_fighter_vec.size(); ++i)
	{
		WorldArenaUser* user = m_user_mgr->CreateAndInsertUser(group_fighter_vec[i]->uuid);
		if (NULL == user)
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "MakeScheduleForGroup_Failed_1, role[%d,%s]", group_fighter_vec[i]->uid, group_fighter_vec[i]->role_name);
			continue;
		}
		user->SetSignUpInfo(*group_fighter_vec[i]);
		user_vec.push_back(user);
	}

	if (user_vec.empty()) return 0;

	int average_level = CalculateAverageLevel(group_fighter_vec);
	int try_times = 0;
	while (user_vec.size() < WORLD_ARENA_FIGHTER_NUM_PER_GROUP && ++try_times < 1000)
	{
		// 加机器人直到满员		
		long long robot_uuid = this->GenerateRobotUUID();
		WorldArenaUser* robot_user = m_user_mgr->CreateAndInsertRobot(robot_uuid, average_level);
		if (NULL != robot_user)
		{
			user_vec.push_back(robot_user);
		}
	}

	// 洗牌一次
	std::random_shuffle(user_vec.begin(), user_vec.end());

	long long group_id = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&group_id);
	m_schedule_module->MakeScheduleForGroup(group_id, user_vec);

	return group_id;
}

std::vector<const WorldArenaSignUpInfo*> WorldArena::ExtractAndShuffleAllSignUpInfo(bool is_schedule_for_omit_users)
{
	const std::set<long long>& signup_recorder = m_signup_module->GetSignupRecorder();

	std::vector<const WorldArenaSignUpInfo*> signup_vec;
	signup_vec.reserve(signup_recorder.size());

	
	std::set<long long>::const_iterator it = signup_recorder.begin();
	for (; it != signup_recorder.end(); ++it)
	{
		const WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(*it);
		if (NULL != user)
		{
			if (is_schedule_for_omit_users && user->GetGroupID() > 0LL)
			{
				continue;
			}

			signup_vec.push_back(user->GetSignUpInfoPtr());
		}	
	}
	std::random_shuffle(signup_vec.begin(), signup_vec.end());

	return signup_vec;
}

long long WorldArena::GenerateRobotUUID()
{
	UniqueUserID unique_userid;
	unique_userid.plat_type = 0;

	// 现版本战斗服不支持不同plat_type的对战，唯有出此下策（开到2000服以上才有机会出现重复uid)
	// 等战斗服支持不同plat_type玩家对战时就把这里改掉
	unique_userid.user_id.db_index = MAX_DB_IDNEX - CrossConfig::Instance().GetHiddenServerIDX() - 1;
	unique_userid.user_id.role_id = m_base_param.robot_index_generator;

	m_base_param.robot_index_generator = (m_base_param.robot_index_generator + 1) % (1 << DB_INDEX_MARK_BIT);
	if (m_base_param.robot_index_generator <= 0)
	{
		m_base_param.robot_index_generator = 1;
	}


	this->SetDirty(true);
	return UniqueUserIDToLongLong(unique_userid);
}

bool WorldArena::IsDirty()
{
	return m_is_dirty;
}

void WorldArena::SetDirty(bool is_dirty)
{
	m_is_dirty = is_dirty;
}

void WorldArena::SaveChanged()
{
	if (this->IsDirty())
	{
		this->SaveBaseData();
	}

	this->SaveUserData(false);
	this->SaveScheduleData(false);
}

void WorldArena::SaveAll()
{
	this->SaveBaseData();
	this->SaveUserData(true);
	this->SaveScheduleData(true);
}

bool WorldArena::HasCreatedBattleScene()
{
	return m_has_battle_scene_created;
}

void WorldArena::CreateBattleScene(ARG_IN int* force_start_round_idx)
{
	int round_index = this->CalculateRoundIndex(m_base_param.round_battle_start_timestamp, m_base_param.next_battle_status_timestamp);
	if (NULL != force_start_round_idx)
	{
		round_index = *force_start_round_idx;
	}
	
	std::vector<const WorldArenaScheduleInfo*> today_schedules = m_schedule_module->GetScheduleInfoListByRoundIndex(round_index);
	m_battle_mgr->CreateScenesForSchedulesToday(round_index, today_schedules);

	if (NULL != force_start_round_idx)
	{
		// 强制开启战斗时，需要修改赛程检查结果的时间戳
		this->ModifyScheduleCheckResultTimestampOnForceStart(today_schedules, m_base_param.round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_DURATION_S + 10);
	}

	m_user_mgr->NoticeAllUsersStartEnterScene(round_index, m_base_param.round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_STANDBY_TIME_S);
}

int WorldArena::CalculateRoundIndex(unsigned int today_round_timestamp, unsigned int battle_status_timestamp)
{
	if (today_round_timestamp >= battle_status_timestamp)
	{
		return (today_round_timestamp - battle_status_timestamp) / SECOND_PER_DAY;
	}
	
	return 0;
}

void WorldArena::SendWorldArenaInfo(const UserID& user_id)
{
	Protocol::SCWorldArenaInfo msg;
	this->AssembleSeasonInfo(&msg);

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, sizeof(msg));
}

void WorldArena::SendWorldArenaUserSignupStatus(const UserID& user_id)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));

	Protocol::SCWorldArenaUserSignupInfo msg;
	msg.has_signup = m_signup_module->HasUserSignedUp(uuid) ? 1 : 0;
	msg.reserve_sh = 0;

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, sizeof(msg));
}

void WorldArena::SendWorldArenaInfoToAll()
{
	Protocol::SCWorldArenaInfo msg;
	this->AssembleSeasonInfo(&msg);

	//World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));
	World::GetInstWorld()->SendToALLGameAllGateway(&msg, sizeof(msg));
}

void WorldArena::SendWorldArenaUserSignupStatusToAllUserOnSeasonStart()
{
	// 赛季开始所有人都没有报名，所以直接设置为0
	Protocol::SCWorldArenaUserSignupInfo msg;
	msg.has_signup = 0;
	msg.reserve_sh = 0;

	World::GetInstWorld()->SendToALLGameAllGateway(&msg, sizeof(msg));
}

void WorldArena::SendUserScheduleInfo(const UserID& user_id)
{
	Protocol::SCWorldArenaUserScheduleInfo msg;
	this->AssembleUserScheduleMsg(user_id, &msg);

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, sizeof(msg));
}

void WorldArena::SendUserBaseInfo(const UserID& user_id)
{
	Protocol::SCWorldArenaUserBaseInfo msg;
	this->AssembleUserBaseInfoMsg(user_id, &msg);

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, sizeof(msg));
}

void WorldArena::AssembleUserSignUpInfo(Role* role, ARG_OUT WorldArenaSignUpInfo* info)
{
	info->avatar_type = role->GetAvatarType();
	
	Guild* guild = role->GetGuild();
	if (NULL != guild)
	{
		F_STRNCPY(info->guild_name, guild->GetGuildName(), sizeof(info->guild_name));
	}

	info->headshot_id = role->GetHeadshotID();
	info->level = role->GetLevel();
	info->plat_type = role->GetPlatSpid();
	info->profession = role->GetProfession();
	info->season_index = 0;
	memset(info->reserve_ll, 0, sizeof(info->reserve_ll));
	std::string name_with_sid = GetRoleNameWithServerId(role->GetName(), IntToUid(role->GetUID()).db_index);
	F_STRNCPY(info->role_name, name_with_sid.c_str(), sizeof(info->role_name));
	info->top_level = role->GetTopLevel();
	info->uid = role->GetUID();
	info->uuid = role->GetUUID();
}

void WorldArena::AssembleUserScheduleMsg(const UserID& user_id, ARG_OUT Protocol::SCWorldArenaUserScheduleInfo* msg)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		msg->has_schedule = 0;
		return;
	}

	msg->has_schedule = 1;
	msg->group_idx = this->GetGroupIdx(user->GetGroupID());

	long long schedule_list[WORLD_ARENA_ROUND_NUM] = { 0 };
	user->GetScheduleList(schedule_list, ARRAY_ITEM_COUNT(schedule_list));

	for (int round = 0; round < ARRAY_ITEM_COUNT(schedule_list); ++round)
	{
		msg->schedule_list[round].Reset();
		msg->schedule_list[round].round_idx = round;
		msg->schedule_list[round].result = 0;

		const WorldArenaScheduleData* schedule = m_schedule_module->GetScheduleInfo(schedule_list[round]);
		if (NULL != schedule)
		{
			const WorldArenaUser* opponent_user = NULL;
			if (schedule->GetScheduleData().attacker_uuid == uuid)
			{
				opponent_user = m_user_mgr->GetWorldArenaUser(schedule->GetScheduleData().defender_uuid);		
				msg->schedule_list[round].SetFightResult(schedule->GetScheduleData().fight_result, true);
			}
			else
			{
				opponent_user = m_user_mgr->GetWorldArenaUser(schedule->GetScheduleData().attacker_uuid);
				msg->schedule_list[round].SetFightResult(schedule->GetScheduleData().fight_result, false);
			}
			
			if (NULL != opponent_user)
			{
				msg->schedule_list[round].uid = opponent_user->GetUID();
				msg->schedule_list[round].avatar_type = static_cast<short>(opponent_user->GetAvatarType());
				F_STRNCPY(msg->schedule_list[round].guild_name, opponent_user->GetGuildName(), sizeof(msg->schedule_list[round].guild_name));
				msg->schedule_list[round].headshot_id = opponent_user->GetHeadshotId();
				msg->schedule_list[round].level = static_cast<short>(opponent_user->GetLevel());
				F_STRNCPY(msg->schedule_list[round].name, opponent_user->GetName(), sizeof(msg->schedule_list[round].name));
				msg->schedule_list[round].plat_type = opponent_user->GetPlatType();
				msg->schedule_list[round].profession = opponent_user->GetProfession();
				msg->schedule_list[round].rank = this->GetUserServerCapabilityRank(IntToUid(opponent_user->GetUID()));
			}
		}
	}
}

void WorldArena::AssembleSeasonInfo(ARG_OUT Protocol::SCWorldArenaInfo* msg)
{
	msg->already_signup_num = m_signup_module->GetSignupNum();
	msg->cur_status = m_base_param.cur_status;
	msg->next_status_timestamp = m_base_param.next_status_timestamp;
	msg->season_index = m_base_param.season_index;
	msg->total_group_num = m_group_hash->GetTotalGroupNum();
}

void WorldArena::AssembleUserBaseInfoMsg(const UserID& user_id, ARG_OUT Protocol::SCWorldArenaUserBaseInfo* msg)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		msg->cur_rank = -1;
		msg->group_index = -1;
		msg->has_signup = 0;
		msg->score = 0;
		msg->win_box_reward_flag = 0;
		msg->win_num = 0;
		msg->has_fetch_signup_reward = 0;
		msg->daily_reward_fetch_flag = 0;
		msg->reserve_ch = 0;
		return;
	}

	msg->has_signup = m_signup_module->HasUserSignedUp(uuid) ? 1 : 0;
	msg->cur_rank = this->GetRankInGroup(uuid);
	msg->group_index = m_group_hash->GetGroupIdx(user->GetGroupID());
	msg->score = user->GetScore();
	msg->win_box_reward_flag = user->GetWinBoxRewardFlag();
	msg->win_num = user->GetWinNum();
	msg->has_fetch_signup_reward = user->HasFetchSignupReward() ? 1 : 0;
	msg->daily_reward_fetch_flag = user->GetFetchDailyRewardFlag();
}

int WorldArena::GetGroupIdx(long long group_id)
{
	return m_group_hash->GetGroupIdx(group_id);
}

int WorldArena::GetUserServerCapabilityRank(const UserID& user_id)
{
	return RankManager::Instance().GetPersonRank()->GetPersonRankIndex(PERSON_RANK_TYPE_CAPABILITY, user_id);
}

void WorldArena::SaveBaseData()
{
	RMISaveWorldArenaBaseDataBackObjectImpl* impl = new RMISaveWorldArenaBaseDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	if (!ac.SaveWorldArenaBaseDataAsyn(m_base_param, impl))
	{
		gamelog::g_log_world_arena.printf(LL_WARNING, "SaveWorldArenaBaseDataAsyn return false");
	}
}

void WorldArena::SaveUserData(bool is_save_all)
{
	m_user_mgr->Save(is_save_all);
}

void WorldArena::SaveScheduleData(bool is_save_all)
{
	m_schedule_module->Save(is_save_all);
}

bool WorldArena::LoadBaseData()
{
	if (GLOBAL_SYSTEM_DATA_STATE_INVALID == m_load_base_data_status)
	{
		m_load_base_data_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	}

	RMIInitWorldArenaBaseDataBackObjectImpl* impl = new RMIInitWorldArenaBaseDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	return ac.InitWorldArenaBaseDataAsyn(impl);
}

void WorldArena::EnterScene(Role* role)
{
	if (NULL == role) return;

	if (!this->IsInBattleStatus())
	{
		role->NoticeNum(errornum::EN_WORLD_ARENA_NOT_IN_BATTLE_STATUS);
		return;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInRoundBattleStandbyTime(now, m_base_param.round_battle_start_timestamp))
	{
		role->NoticeNum(errornum::EN_WORLD_ARENA_NOT_IN_TIME_TO_ENTER_SCENE);
		return;
	}

	if (!role->CanEnterFB(true))
	{
		return;
	}

	if (role->InTeam())
	{
		Team* team = role->GetMyTeam();
		if (NULL != team)
		{
			UserID user_id = role->GetUserId();
			team->LeaveTeamByUid(role->GetUID());

			// 重新获取role实例（怕离开队伍导致切场景）
			role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		}
	}

	if (role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	long long uuid = role->GetUUID();
	SpecialLogic* spl = m_battle_mgr->GetSpecialLogicForUser(uuid);
	if (NULL == spl)
	{
		role->NoticeNum(errornum::EN_WORLD_ARENA_UNABLE_ENTER_SCENE);
		return;
	}

#ifdef _DEBUG
	SpecialLogicWorldArenaStandbyRoom* room = dynamic_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
	assert(NULL != room);
#else
	SpecialLogicWorldArenaStandbyRoom* room = static_cast<SpecialLogicWorldArenaStandbyRoom*>(spl);
#endif

	bool is_attacker = (room->GetAttackerUUID() == uuid);
	Posi target_pos = is_attacker ?
		LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().attacker_posi :
		LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().defender_posi ;

	if (!World::GetInstWorld()->GetSceneManager()->GoTo(role, spl->GetScene()->GetSceneID(), spl->GetScene()->GetSceneKey(), target_pos))
	{
		role->NoticeNum(errornum::EN_WORLD_ARENA_ENTER_SCENE_FAILED);
		return;
	}
}

void WorldArena::TryEnterSceneInOrigin(Role* role)
{
	if (role->InTeam())
	{
		Team* team = role->GetMyTeam();
		if (NULL != team)
		{
			UserID user_id = role->GetUserId();
			team->LeaveTeamByUid(role->GetUID());

			// 重新获取role实例（怕离开队伍导致切场景）
			role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		}
	}

	CrossData_Business business;
	business.cross_business_type = CrossData_Business::BUSINESS_TYPE_ENTER_WORLD_ARENA_BATTLE_SCENE;

	role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business);
}

void WorldArena::FetchWinBoxReward(const UserID& user_id, int sn)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));

	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const WorldArenaWinBoxRewardCfg* cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetWinBoxRewardCfg(sn);
	if (NULL == cfg)
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (user->GetWinNum() < cfg->win_num)
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_WIN_NUM_NOT_ENOUGH);
		return;
	}

	if (0 != (user->GetWinBoxRewardFlag() & (1 << sn)))
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_WIN_BOX_ALREADY_FETCHED);
		return;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(cfg->reward_list))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}

		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(cfg->reward_list, PUT_REASON_WORLD_ARENA_WIN_BOX))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	else if (!CrossConfig::Instance().IsHiddenServer())
	{
#ifndef _MSC_VER
		assert(0); // 只有Windows下才能在本服领
#endif
		return;
	}
	else
	{
		crossgameprotocal::HiddenGameWorldArenaGiveWinBoxReward msg;
		msg.uid = UidToInt(user_id);
		msg.sn = sn;
		if (!InternalComm::Instance().SendToGameThroughCross(plat_type, user_id.db_index, (const char*)&msg, sizeof(msg)))
		{
			return;
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendToOrigin user[%d,%s] sn[%d]",
				__FUNCTION__, user->GetUID(), user->GetName(), sn);
		}
	}

	user->OnFetchWinBox(sn);

	this->SendUserBaseInfo(user_id);

	gamelog::g_log_world_arena.printf(LL_INFO, "%s user[%d,%s] sn[%d] win_num[%d] new_flag[%d]", 
		__FUNCTION__, user->GetUID(), user->GetName(), sn, user->GetWinNum(), user->GetWinBoxRewardFlag());
}

void WorldArena::FetchSignupReward(const UserID& user_id)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));

	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (user->HasFetchSignupReward())
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_HAS_FETCH_SIGNUP_REWARD);
		return;
	}

	const WorldArenaOtherCfg& other_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg();

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(other_cfg.signup_reward_list))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}

		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(other_cfg.signup_reward_list, PUT_REASON_WORLD_ARENA_SIGNUP_REWARD))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	else if (!CrossConfig::Instance().IsHiddenServer())
	{
#ifndef _MSC_VER
		assert(0); // 只有Windows下才能在本服领
#endif
		return;
	}
	else
	{
		crossgameprotocal::HiddenGameWorldArenaGiveSignupReward msg;
		msg.uid = UidToInt(user_id);
		if (!InternalComm::Instance().SendToGameThroughCross(plat_type, user_id.db_index, (const char*)&msg, sizeof(msg)))
		{
			return;
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendToOrigin user[%d,%s]",
				__FUNCTION__, user->GetUID(), user->GetName());
		}
	}

	user->OnFetchSignupReward();

	this->SendUserBaseInfo(user_id);

	gamelog::g_log_world_arena.printf(LL_INFO, "%s user[%d,%s]", 
		__FUNCTION__, user->GetUID(), user->GetName());
}

void WorldArena::FetchDailyReward(const UserID& user_id)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));

	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!user->CanFetchDailyReward())
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_DAILY_REWARD_FETCH_FAIL);
		return;
	}

	const WorldArenaOtherCfg& other_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg();

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(other_cfg.day_reward_list))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}

		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(other_cfg.day_reward_list, PUT_REASON_WORLD_ARENA_DAILY_REWARD))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	else if (!CrossConfig::Instance().IsHiddenServer())
	{
#ifndef _MSC_VER
		assert(0); // 只有Windows下才能在本服领
#endif
		return;
	}
	else
	{
		crossgameprotocal::HiddenGameWorldArenaGiveDailyReward msg;
		msg.uid = UidToInt(user_id);
		if (!InternalComm::Instance().SendToGameThroughCross(plat_type, user_id.db_index, (const char*)&msg, sizeof(msg)))
		{
			return;
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendToOrigin user[%d,%s]",
				__FUNCTION__, user->GetUID(), user->GetName());
		}
	}

	user->OnFetchDailyRerward();

	this->SendUserBaseInfo(user_id);

	gamelog::g_log_world_arena.printf(LL_INFO, "%s user[%d,%s]",
		__FUNCTION__, user->GetUID(), user->GetName());
}

void WorldArena::GiveWinBoxRewardAtOrigin(const UserID& user_id, int sn)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		assert(0);
		return;
	}

	const WorldArenaWinBoxRewardCfg* cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetWinBoxRewardCfg(sn);
	if (NULL == cfg)
	{
		gamelog::g_log_world_arena.printf(LL_INFO, "%s FAILED[config not found] role[%d] sn[%d]",
			__FUNCTION__, UidToInt(user_id), sn);

		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(cfg->reward_list, PUT_REASON_WORLD_ARENA_WIN_BOX))
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->SendMail(cfg->reward_list, SEND_MAIL_TYPE_WORLD_ARENA_WIN_BOX_REWARD, true))
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 FAILED role[%d,%s] sn[%d]",
					__FUNCTION__, role->GetUID(), role->GetName(), sn);
			}
			else
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 SUCC role[%d,%s] sn[%d]",
					__FUNCTION__, role->GetUID(), role->GetName(), sn);
			}
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s PutInKnapsack SUCC role[%d,%s] sn[%d]",
				__FUNCTION__, role->GetUID(), role->GetName(), sn);
		}
	}
	else
	{
		static MailContentParam contentparam;
		contentparam.Reset();
		for (size_t i = 0; i < cfg->reward_list.size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
		{
			contentparam.item_list[i].item_id = cfg->reward_list[i].item_id;
			contentparam.item_list[i].is_bind = cfg->reward_list[i].is_bind ? 1 : 0;
			contentparam.item_list[i].num = cfg->reward_list[i].num;
		}

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_win_box_reward_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_win_box_reward_content);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(user_id, MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 SUCC role[%d] sn[%d]",
				__FUNCTION__, UidToInt(user_id), sn);
		}	
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 FAILED role[%d] sn[%d]",
				__FUNCTION__, UidToInt(user_id), sn);
		}
	}
}

void WorldArena::GiveSignupRewardAtOrigin(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		assert(0);
		return;
	}

	const WorldArenaOtherCfg& other_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg();

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(other_cfg.signup_reward_list, PUT_REASON_WORLD_ARENA_SIGNUP_REWARD))
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->SendMail(other_cfg.signup_reward_list, SEND_MAIL_TYPE_WORLD_ARENA_SIGNUP_REWARD, true))
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 FAILED role[%d,%s]",
					__FUNCTION__, role->GetUID(), role->GetName());
			}
			else
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 SUCC role[%d,%s]",
					__FUNCTION__, role->GetUID(), role->GetName());
			}
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s PutInKnapsack SUCC role[%d,%s]",
				__FUNCTION__, role->GetUID(), role->GetName());
		}
	}
	else
	{
		static MailContentParam contentparam;
		contentparam.Reset();
		for (size_t i = 0; i < other_cfg.signup_reward_list.size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
		{
			contentparam.item_list[i].item_id = other_cfg.signup_reward_list[i].item_id;
			contentparam.item_list[i].is_bind = other_cfg.signup_reward_list[i].is_bind ? 1 : 0;
			contentparam.item_list[i].num = other_cfg.signup_reward_list[i].num;
		}

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_signup_reward_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_signup_reward_content);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(user_id, MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 SUCC role[%d]",
				__FUNCTION__, UidToInt(user_id));
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 FAILED role[%d]",
				__FUNCTION__, UidToInt(user_id));
		}
	}
}

void WorldArena::GiveDailyRewardAtOrigin(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		assert(0);
		return;
	}

	const WorldArenaOtherCfg& other_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg();

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(other_cfg.day_reward_list, PUT_REASON_WORLD_ARENA_DAILY_REWARD))
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->SendMail(other_cfg.day_reward_list, SEND_MAIL_TYPE_WORLD_ARENA_DAILY_REWARD, true))
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 FAILED role[%d,%s]",
					__FUNCTION__, role->GetUID(), role->GetName());
			}
			else
			{
				gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_111 SUCC role[%d,%s]",
					__FUNCTION__, role->GetUID(), role->GetName());
			}
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s PutInKnapsack SUCC role[%d,%s]",
				__FUNCTION__, role->GetUID(), role->GetName());
		}
	}
	else
	{
		static MailContentParam contentparam;
		contentparam.Reset();
		for (size_t i = 0; i < other_cfg.day_reward_list.size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
		{
			contentparam.item_list[i].item_id = other_cfg.day_reward_list[i].item_id;
			contentparam.item_list[i].is_bind = other_cfg.day_reward_list[i].is_bind ? 1 : 0;
			contentparam.item_list[i].num = other_cfg.day_reward_list[i].num;
		}

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_day_reward_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_day_reward_content);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(user_id, MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 SUCC role[%d]",
				__FUNCTION__, UidToInt(user_id));
		}
		else
		{
			gamelog::g_log_world_arena.printf(LL_INFO, "%s SendByMail_222 FAILED role[%d]",
				__FUNCTION__, UidToInt(user_id));
		}
	}
}

void WorldArena::AssembleShowUpRoleInfo(long long uuid, Protocol::MsgWorldArenaShowUpRoleInfo* role_info)
{
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user) return;

	role_info->plat_type = user->GetPlatType();
	role_info->uid = user->GetUID();
	role_info->avatar_type = user->GetAvatarType();
	role_info->headshot_id = user->GetHeadshotId();
	role_info->level = user->GetLevel();
	role_info->profession = user->GetProfession();
	F_STRNCPY(role_info->name, user->GetName(), sizeof(role_info->name));
}

const WorldArenaUser* WorldArena::GetWorldArenaUser(long long uuid)
{
	return m_user_mgr->GetWorldArenaUser(uuid);
}

void WorldArena::UpdateUserSignupInfo(Role* role)
{
	WorldArenaSignUpInfo info;
	this->AssembleUserSignUpInfo(role, &info);

#ifndef _MSC_VER
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->UpdateUserSignupInfo(role->GetUUID(), info);
	}
	else
	{
		crossgameprotocal::GameHiddenWorldArenaUpdateSignupInfo msg;
		msg.signup_info = info;

		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
	}
#else
	this->UpdateUserSignupInfo(role->GetUUID(), info);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenWorldArenaUpdateSignupInfo msg;
		msg.signup_info = info;

		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
	}
#endif
}

void WorldArena::UpdateUserSignupInfo(long long uuid, const WorldArenaSignUpInfo& info)
{
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user) return;

	user->UpdateUserSignupInfo(info);
}

bool CompareForRank(const WorldArenaUser* a, const WorldArenaUser* b)
{
	int a_score = a->GetScore();
	int b_score = b->GetScore();
	if (a_score != b_score)
	{
		return a_score > b_score;
	}

	int a_win_num = a->GetWinNum();
	int b_win_num = b->GetWinNum();
	if (a_win_num != b_win_num)
	{
		return a_win_num > b_win_num;
	}

	int a_draw_num = a->GetDrawNum();
	int b_draw_num = b->GetDrawNum();
	if (a_draw_num != b_draw_num)
	{
		return a_draw_num > b_draw_num;
	}

	int a_total_fight_num = a->GetTotalFightNum();
	int b_total_fight_num = b->GetTotalFightNum();
	if (a_total_fight_num != b_total_fight_num)
	{
		return a_total_fight_num < b_total_fight_num;
	}

	//int a_lose_num = a->GetLoseNum();
	//int b_lose_num = b->GetLoseNum();
	//if (a_lose_num != b_lose_num)
	//{
	//	return a_lose_num > b_lose_num;
	//}

	int a_total_battle_round = a->GetTotalBattleRound();
	int b_total_battle_round = b->GetTotalBattleRound();
	if (a_total_battle_round != b_total_battle_round)
	{
		return a_total_battle_round < b_total_battle_round;
	}

	int a_is_robot = static_cast<int>(a->IsRobot());
	int b_is_robot = static_cast<int>(b->IsRobot());
	if (a_is_robot != b_is_robot)
	{
		return a_is_robot < b_is_robot; // 玩家排在机器人前面
	}

	return a->GetUUID() < b->GetUUID();
}

int WorldArena::GetRankInGroup(long long uuid)
{
	if (this->IsInSignupStatus()) return -1;

	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user)
	{
		return -1;
	}

	// 数据量不大，暴力一点就好
	long long group_id = user->GetGroupID();
	std::vector<WorldArenaUser*> users = m_user_mgr->GetUsersInGroup(group_id);
	assert(!users.empty());

	std::sort(users.begin(), users.end(), CompareForRank);
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (uuid == users[i]->GetUUID())
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}

void WorldArena::QueryGroupRankList(const UserID& user_id, int group_idx)
{
	if (this->IsInSignupStatus())
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_WORLD_ARENA_IN_SIGNUP_STATUS_LIMIT);
		return;
	}

	long long group_id = m_group_hash->GetGroupId(group_idx);
	std::vector<WorldArenaUser*> users = m_user_mgr->GetUsersInGroup(group_id);
	if (users.empty())
	{
		World::GetInstWorld()->SendNoticeNumToUser(user_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::sort(users.begin(), users.end(), CompareForRank);
	this->SendGroupRankList(user_id, group_idx, users);
}

void WorldArena::SendGroupRankList(const UserID& user_id, int group_idx, const std::vector<WorldArenaUser*>& users)
{
	Protocol::SCWorldArenaGroupRankList msg;
	msg.group_idx = group_idx;
	msg.user_num = 0;
	for (size_t i = 0; i < users.size() && msg.user_num < ARRAY_ITEM_COUNT(msg.user_list); ++i)
	{
		if (i < 3)
		{
			msg.top3_info_list[i].appearance = users[i]->GetAppearance();
		}
		users[i]->AssembleMsgRankListUserInfo(&msg.user_list[msg.user_num]);
		msg.user_num += 1;
	}

	int length = sizeof(msg) - sizeof(msg.user_list) + msg.user_num * sizeof(msg.user_list[0]);

	World::GetInstWorld()->SendToRole(UidToInt(user_id), &msg, length);
}

int WorldArena::CalculateAverageLevelInGroup(long long group_id)
{
	std::vector<WorldArenaUser*> users = m_user_mgr->GetUsersInGroup(group_id);
	if (users.empty())
	{
		assert(0);
		return 80;
	}

	int total_level = 0;
	int user_num = 0;
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->IsRobot()) continue;

		total_level += users[i]->GetLevel();
		user_num += 1;
	}

	if (user_num <= 0)
	{
		assert(0);
		return 80;
	}

	return total_level / user_num;
}

void WorldArena::StandbyForFight(Role* role, bool is_ready)
{
	m_battle_mgr->StandbyForFight(role, is_ready);
}

void WorldArena::CalculateSeasonRankReward()
{
	std::vector<long long> all_groups = m_group_hash->GetAllGroupIds();
	for (size_t i = 0; i < all_groups.size(); ++i)
	{
		this->CalcSeasonRankRewardForGroup(all_groups[i]);
	}
}

void WorldArena::CalcSeasonRankRewardForGroup(long long group_id)
{
	std::vector<WorldArenaUser*> users = m_user_mgr->GetUsersInGroup(group_id);
	if (users.empty())
	{
		return;
	}

	std::sort(users.begin(), users.end(), CompareForRank);
	for (size_t i = 0; i < users.size(); ++i)
	{
		if (users[i]->IsRobot()) continue;

		this->SendSeasonRankRewardMailToUser(users[i], static_cast<int>(i + 1));
	}
}

void WorldArena::SendSeasonRankRewardMailToUser(WorldArenaUser* user, int rank)
{
	gamelog::g_log_world_arena.printf(LL_INFO, "%s user[%d,%s] rank[%d]", __FUNCTION__, user->GetUID(), user->GetName(), rank);

	const WorldArenaSeasonRankRewardCfg* cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetSeasonRankRewardCfg(rank);
	if (NULL == cfg)
	{
		gamelog::g_log_world_arena.printf(LL_ERROR, "%s Config Not Found, user[%d,%s] rank[%d]", __FUNCTION__, user->GetUID(), user->GetName(), rank);
		return;
	}

	static MailContentParam contentparam;
	contentparam.Reset();

	for (size_t i = 0; i < cfg->reward_list.size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
	{
		contentparam.item_list[i].item_id = cfg->reward_list[i].item_id;
		contentparam.item_list[i].is_bind = cfg->reward_list[i].is_bind ? 1 : 0;
		contentparam.item_list[i].num = cfg->reward_list[i].num;
	}

	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_season_rank_reward_subject, m_base_param.season_index);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_season_rank_reward_content, m_base_param.season_index, rank);
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(IntToUid(user->GetUID()), MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM);
	}
}

void WorldArena::ModifyScheduleCheckResultTimestampOnForceStart(const std::vector<const WorldArenaScheduleInfo*>& schedule_list, unsigned int check_timestamp)
{
	for (size_t i = 0; i < schedule_list.size(); ++i)
	{
		m_schedule_module->ModifyScheduleCheckResultTimestamp(schedule_list[i]->schedule_id, check_timestamp);
	}
}

void WorldArena::SendTodayGiveNotice(const UserID& user_id)
{
	World::GetInstWorld()->SendNoticeNumToUser(user_id, noticenum::NT_WORLD_ARENA_TODAY_GIVEUP);
}

void WorldArena::CheckTodayGiveup(const UserID& user_id)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL != user && user->HasTodayGiveupNotice())
	{
		this->SendTodayGiveNotice(user_id);
		user->ClearTodayGiveNotice();
	}
}

void WorldArena::CheckAndSendNoticeEnterScene(const UserID& user_id)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!this->IsInBattleStatus() || !this->IsInRoundBattleStandbyTime(now, m_base_param.round_battle_start_timestamp))
	{
		return;
	}

	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user) return;

	int round_index = m_battle_mgr->GetCurRoundIndex();
	long long schedule_id = user->GetScheduleID(round_index);
	if (0 == schedule_id) return;

	Protocol::SCWorldArenaStartEnterSceneNotice msg;
	msg.enter_scene_end_timestamp = m_base_param.round_battle_start_timestamp + WORLD_ARENA_ROUND_BATTLE_STANDBY_TIME_S;
	msg.start_round_index = round_index;

	int fight_result = this->GetFightResult(schedule_id);
	if (fight_result != WORLD_ARENA_RESULT_INVALID)
	{
		msg.has_finish_this_round = 1;
	}
	else
	{
		msg.has_finish_this_round = 0;
	}

	World::GetInstWorld()->TrySendMsgToUser(user_id, &msg, sizeof(msg));
}

void WorldArena::SyncUserInfo(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	int plat_type = LocalConfig::Instance().GetPlatType();
	long long uuid = UniqueUserIDToLongLong(UniqueUserID(plat_type, user_id));
	WorldArenaUser* user = m_user_mgr->GetWorldArenaUser(uuid);
	if (NULL == user) return;

	WorldArenaSignUpInfo info = user->GetSignUpInfoRef();
	info.avatar_type = role->GetAvatarType();
	info.headshot_id = role->GetHeadshotID();
	info.level = role->GetLevel();
	info.profession = role->GetProfession();
	F_STRNCPY(info.role_name, role->GetName(), sizeof(info.role_name));
	Guild* guild = role->GetGuild();
	if (NULL != guild)
	{
		F_STRNCPY(info.guild_name, guild->GetGuildName(), sizeof(info.guild_name));
	}
	info.top_level = role->GetTopLevel();
	user->SetSignUpInfo(info);

	static RoleBattleData rbd;
	rbd.Reset();
	role->GetRoleModuleManager()->CollectRoleBattleData(&rbd, true);
	user->SetRoleBattleData(rbd);
}

void WorldArena::GiveDayReward(const UserID& user_id)
{
	//const WorldArenaOtherCfg& cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg();
	//
	//static MailContentParam contentparam;
	//contentparam.Reset();
	//for (int i = 0; i < cfg.day_reward_list.size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
	//{
	//	contentparam.item_list[i].item_id = cfg.day_reward_list[i].item_id;
	//	contentparam.item_list[i].num = cfg.day_reward_list[i].num;
	//	contentparam.item_list[i].is_bind = cfg.day_reward_list[i].is_bind ? 1 : 0;
	//}
	//
	//int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_day_reward_subject);
	//int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_day_reward_content);
	//if (lensub > 0 && length > 0)
	//{
	//	MailRoute::MailToUser(UidToInt(user_id), MAIL_REASON_DEFAULT, contentparam);
	//
	//	gamelog::g_log_world_arena.printf(LL_INFO, "GiveDayReward user[%d]", UidToInt(user_id));
	//}
}

void WorldArena::LogStatus(const char* func)
{
	gamelog::g_log_world_arena.printf(LL_INFO, "WorldArena::LogStatus func[%s] season[%d] cur_status[%s] next_status_timestamp[%u] \
	signup_timestamp[%u] schedule_timestamp[%u] battle_timestamp[%u] calculate_timestamp[%u] rest_timestamp[%u]",
		func, m_base_param.season_index, this->GetStatusStr(m_base_param.cur_status),
		m_base_param.next_status_timestamp, m_base_param.next_signup_status_timestamp,
		m_base_param.next_schedule_status_timestamp, m_base_param.next_battle_status_timestamp,
		m_base_param.next_calculate_status_timestamp, m_base_param.next_rest_status_timestamp);
}

const char* WorldArena::GetStatusStr(int status)
{
	switch (status)
	{
	case WORLD_ARENA_STATUS_INVALID:
		{
			return "Invalid";
		}
		break;

	case WORLD_ARENA_STATUS_SIGNUP:
		{
			return "SignUp";
		}
		break;

	case WORLD_ARENA_STATUS_SCHEDULE:
		{
			return "Schedule";
		}
		break;

	case WORLD_ARENA_STATUS_BATTLE:
		{
			return "Battle";
		}
		break;

	case WORLD_ARENA_STATUS_CALCULATE:
		{
			return "Calculate";
		}
		break;

	case WORLD_ARENA_STATUS_REST:
		{
			return "Rest";
		}
		break;

	default:
		{
			return "UnknownStatus";
		}
		break;
	}

	return "UnknownStatus";
}

unsigned int WorldArena::CalculateRoundBattleEndTimestamp(int round_index)
{
	if (round_index < 0 || round_index >= WORLD_ARENA_ROUND_NUM) return 0u;

	// 用6号0点时间 + 18小时 + 加轮数*每天的秒数 + 战斗持续时长 + 10秒
	return m_base_param.next_battle_status_timestamp 
		+ round_index * SECOND_PER_DAY 
		+ WORLD_ARENA_ROUND_BATTLE_START_HOUR * SECOND_PER_HOUR
		+ WORLD_ARENA_ROUND_BATTLE_START_MINUTE * SECOND_PER_MIN
		+ WORLD_ARENA_ROUND_BATTLE_DURATION_S + 10;
}

int WorldArena::GetFightResult(long long schedule_id)
{
	return m_schedule_module->GetFightResult(schedule_id);
}

WorldArena::~WorldArena()
{
	delete m_signup_module;
	m_signup_module = NULL;

	delete m_schedule_module;
	m_schedule_module = NULL;

	delete m_user_mgr;
	m_user_mgr = NULL;

	delete m_battle_mgr;
	m_battle_mgr = NULL;

	delete m_group_hash;
	m_group_hash = NULL;
}

void WorldArena::OnDayChange()
{
	if (this->IsInBattleStatus())
	{
		m_base_param.round_battle_start_timestamp = m_base_param.next_round_battle_start_timestamp;
		m_base_param.next_round_battle_start_timestamp += SECOND_PER_DAY;
		m_battle_mgr->ResetCreateScenesFlag();
	}

	m_user_mgr->OnDayChange();
}

void WorldArena::OnServerStart()
{
	bool ret = this->LoadBaseData() && m_user_mgr->LoadUser(0) && m_schedule_module->LoadSchedule(0);
	if (!ret)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "WorldArena::OnServerStart Async Load Data Failed!");
		gamelog::g_log_world_arena.printf(LL_ERROR, "WorldArena::OnServerStart Async Load Data Failed!");
		ServerLogic::GetInstServerLogic()->StopServer("WorldArena::OnServerStart Async Load Data Failed!");
	}
}

void WorldArena::OnServerStop()
{
	this->SaveAll();
}

