#include "colosseummanager.hpp"
#include "gladiator.hpp"
#include <algorithm>
#include "internalcomm.h"
#include "global/rmibackobjdef.h"
#include "serverlogic.h"
#include "world.h"
#include "obj/character/role.h"
#include "gamelog.h"
#include "servercommon/serverconfig/openserverconfig.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/colosseumconfig.hpp"
#include "protocol/msgcolosseum.h"
#include "protocol/msgrole.h"
#include "global/usercache/usercache.hpp"
#include "battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "item/itempool.h"
#include "other/robot/robotconfig.hpp"
#include "skill/skillpool.hpp"
#include "other/pet/petconfig.hpp"
#include "task/taskmanager.h"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "servercommon/serverclmempool.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/nameconfig.h"
#include "global/worldstatus/worldstatus.hpp"
#include "other/partner/partnerconfig.hpp"
#include "gameworld/global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/protobuf/battle_character_data.pb.h"
#include "servercommon/protobufmanager.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysportsconfig.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitysports.hpp"
#include "other/mysterious_trial/mysterioustrial.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/rolecross/rolecross.hpp"

void ColosseumManager::OnGetRankUidList(int count, std::vector<int>& rank_uid_list)
{
	if(CrossConfig::Instance().IsHiddenServer() || count <= 0) return;

	int rank_count = 0;
	for (int i = 0; i < m_rank_user_num && rank_count < count; ++i)
	{
		if (NULL == m_rank_list[i])
		{
			rank_uid_list.push_back(0);
		}
		else
		{
			rank_uid_list.push_back(m_rank_list[i]->m_uid);
		}
		rank_count++;
	}
}

int ColosseumManager::SyncUserData(Role* role, Gladiator** out_gladiator /* = NULL */)
{
	if (NULL == role) return -1;

	if (this->IsLevelLimit(role->GetLevel()))
	{
		return -2;
	}

	int rankpos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId(), &rankpos, &is_in_pool);

	if (NULL == gladiator)
	{
		gladiator = new Gladiator();
		gladiator->SyncRoleData(role);
		this->AddRankUser(gladiator);

		if (NULL != out_gladiator) *out_gladiator = gladiator;
	}
	else
	{
		gladiator->SyncRoleData(role);
		this->SetPosDirty(rankpos, is_in_pool);

		if (NULL != out_gladiator) *out_gladiator = gladiator;
	}
	return 0;
}

void ColosseumManager::OnFightReq(Role * role, int target_index)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameQueryTargetGladiatorBeforeFightReq req;
		req.role_id = role->GetUID();
		req.target_index = target_index;
		req.is_sweep = 0;
		InternalComm::Instance().SendToGameThroughCross(role->GetUniqueServerID().plat_type, role->GetUniqueServerID().server_id,  (const char*)&req, sizeof(req));

		return;
	}

	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId(), &pos, &is_in_pool);

	if (NULL == gladiator)
	{
		role->NoticeNum(errornum::EN_COLOSSEUM_NOT_IN_RANK_LIST);
		return;
	}

	if (is_in_pool)
	{
		role->NoticeNum(errornum::EN_COLOSSEUM_CAN_NOT_FIGHT);
		return;
	}

	const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();

	// 判断次数
	if (gladiator->m_today_fight_count >= other_cfg.challenge_times)
	{
		role->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	int target_rank = gladiator->GetTargetRank(target_index);
	if (target_rank < 0 || target_rank >= (int)m_rank_user_num || NULL == m_rank_list[target_rank])
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 获取对手的竞技场数据 (需要做属性调整)
	Gladiator * target = m_rank_list[target_rank];
	//if (target->IsRobot())
	//{
	//	target->AdjustRobotAttr();
	//}

	if (BattleManagerLocal::GetInstance().StartSingleArenaReq(role, target->m_uid, 0, other_cfg.battle_max_round, target->m_battle_data))
	{
		EventHandler::Instance().OnRoleChallengeColosseum(role);
	}
	WorldStatus::Instance().LogFunctionStats(role, "ColosseumManager");
}

void ColosseumManager::OnHiddenGameQueryTargetGladiatorBeforeFight(int role_id, int target_index, int is_sweep)
{
	int pos = 0;
	bool is_in_pool = false;
	Gladiator* user_gladiator = ColosseumManager::Instance().GetUserGladiator(IntToUid(role_id), &pos, &is_in_pool);
	if (NULL == user_gladiator)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_COLOSSEUM_NOT_IN_RANK_LIST);
		return;
	}

	if (is_in_pool)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_COLOSSEUM_CAN_NOT_FIGHT);
		return;
	}

	const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();

	// 判断次数
	if (user_gladiator->m_today_fight_count >= other_cfg.challenge_times)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	int target_rank = user_gladiator->GetTargetRank(target_index);
	if (target_rank < 0 || target_rank >= (int)m_rank_user_num || NULL == m_rank_list[target_rank])
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	Gladiator * target = m_rank_list[target_rank];

	crossgameprotocal::GameHiddenQeuryTargetGladiatorBeforeFightResp resp;
	resp.role_id = role_id;
	resp.target_uid = target->GetUid();
	resp.target_index = target_index;
	resp.target_battle_data = target->m_battle_data;
	resp.is_sweep = is_sweep;
	resp.user_total_capability = user_gladiator->m_total_capability;
	resp.target_total_capability = target->m_total_capability;

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&resp);
}

void ColosseumManager::OnGameHiddenQueryTargetGladiatorBeforeFightResp(int role_id, int target_index, int target_uid, const RoleBattleData& target_battle_data)
{
	// 查询到了对手的战斗数据，进入战斗
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role)
	{
		return;
	}

	int max_round = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().battle_max_round;
	if (BattleManagerLocal::GetInstance().StartSingleArenaReq(role, target_uid, 0, max_round, target_battle_data))
	{
		EventHandler::Instance().OnRoleChallengeColosseum(role);
	}
	WorldStatus::Instance().LogFunctionStats(role, "ColosseumManager");
}

void ColosseumManager::OnRefreshTarget(int uid, int assign_target /* = -1 */)
{
	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid), &pos, &is_in_pool);
	if (NULL == gladiator || is_in_pool) return;

	time_t now_sec = EngineAdapter::Instance().Time();
	int client_refresh_interval_s = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().client_refresh_interval_s;
	if (now_sec < gladiator->last_refresh_target_time + client_refresh_interval_s)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
		if (NULL != role) role->NoticeNum(errornum::EN_YOUR_CLICK_IS_TOO_OFTEN);

		return;
	}

	gladiator->last_refresh_target_time = now_sec;

	if (!is_in_pool)
	{
		this->SetRankListDirty(pos);
	}
	else
	{
		this->SetPoolListDirty(pos);
	}

	this->RefreshTarget(gladiator, assign_target);
}

void ColosseumManager::SysReport(Role * role, int target_uid, bool is_win, BattleData_Business* business_data /* = NULL */, BattleData_StatisticRole* role_statistic_data /* = NULL */, BattleData_Statistic* statistic_data /* = NULL */, int pet_stat_num /* = 0 */, BattleData_StatisticRole* pet_stat_list /* = NULL */, bool is_sweep)
{
	if (NULL == role) return;

	// 这段是竞技场挑战/扫荡给奖励的逻辑,与跨服原服无关,放到前面以免跨服扫荡不触发逻辑
	{
		const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
		if (!role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_cfg.battle_reward, MONEY_TYPE_COLOSSEUM, __FUNCTION__))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		}

		const ColossumBattleRewardCfg* battle_reward_cfg = LOGIC_CONFIG->GetColosseumConfig()->RandBattleRewardCfg();
		if (NULL != battle_reward_cfg)
		{
			if (!role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(battle_reward_cfg->integral, MONEY_TYPE_COLOSSEUM, __FUNCTION__) ||
				!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(battle_reward_cfg->bind_coin, __FUNCTION__))
			{
				role->GetRoleModuleManager()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			}

			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporaryVec(battle_reward_cfg->reward_item_list))
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutVec(battle_reward_cfg->reward_item_list, PUT_REASON_COLOSSEUM_BATTLE_REWARD);
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->SendMail(battle_reward_cfg->reward_item_list, SEND_MAIL_TYPE_COLOSSEUM_BATTLE_REWARD);
			}

			Protocol::SCBattleRewardNotice msg;
			msg.reward_group = battle_reward_cfg->battle_price;
			role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
		}
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameColosseumFightSysReport sysreport;
		sysreport.is_win = is_win ? 1 : 0;
		sysreport.role_id = role->GetUID();
		sysreport.target_uid = target_uid;
		sysreport.is_sweep = is_sweep ? 1 : 0;
		this->CalculateFightStat(sysreport.stat, role_statistic_data, statistic_data, pet_stat_num, pet_stat_list);
		
		InternalComm::Instance().SendToGameThroughCross(role->GetUniqueServerID().plat_type, role->GetUniqueServerID().server_id, (const char*)&sysreport, sizeof(sysreport));
		return;
	}

	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId(), &pos, &is_in_pool);
	if (NULL == gladiator) return;

	int target_pos = 0;
	bool target_is_in_pool = false;
	Gladiator* target_gladiator = this->GetUserGladiator(UidToInt(target_uid), &target_pos, &target_is_in_pool);
	if (NULL == target_gladiator) return;

	gladiator->SysReport(target_gladiator, is_win, false); // gladiator先动的手
	target_gladiator->SysReport(gladiator, !is_win, true); // target是被打的一方

	gladiator->m_today_fight_count += 1;
	this->SetPosDirty(pos, is_in_pool);

	ColosseumFightStatistic stat;
	this->CalculateFightStat(stat, role_statistic_data, statistic_data, pet_stat_num, pet_stat_list);	

	if (is_win)
	{
		if (!is_in_pool && !target_is_in_pool &&
			pos > target_pos)	// 如果我的排名在目标排名的后面
		{
			gladiator->SetPos(target_pos, true);
			if (-1 == gladiator->m_highest_rank)
			{
				gladiator->m_highest_rank = target_pos;
			}
			else
			{
				gladiator->m_highest_rank = GetMin(target_pos, gladiator->m_highest_rank);
			}

			target_gladiator->SetPos(pos, true);

			gamelog::g_log_colosseum.printf(LL_INFO, "ColosseumManager::SysReport attacker[%d,%s]=>pos[%d], defender[%d,%s]=>pos[%d]",
				gladiator->GetUid(), gladiator->GetName(), target_pos, target_gladiator->GetUid(), target_gladiator->GetName(), pos);

			this->SetRankUser(target_pos, gladiator);
			this->SetRankUser(pos, target_gladiator);

			this->RefreshTarget(gladiator);
			this->RefreshTarget(target_gladiator);

			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_0, target_pos + 1);

			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COLOSSEUM, target_pos + 1);

			EventHandler::Instance().OnRoleTopRank(role, target_pos);
		}

		gladiator->CheckHighestRankReward();

		int win_streak = gladiator->GetColosseumParam(COLOSSEUN_PARAM_TYPE_4);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_4, win_streak + 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_5, win_streak + 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_2, 1);
		if (gladiator->m_total_capability < target_gladiator->m_total_capability)
		{
			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_7, 1);
		}
		if (stat.round_num >= 8)
		{
			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_11, 1);
		}
		if (!stat.has_woman_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_12, 1);
		if (!stat.has_man_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_13, 1);
		if (stat.has_swordsman_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_14, 1);
		if (stat.has_start_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_15, 1);
		if (stat.has_lich_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_16, 1);
		if (stat.has_berserk_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_17, 1);
		if (stat.has_arrower_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_18, 1);
		if (stat.has_doctor_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_19, 1);
		if (stat.has_tiewei_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_20, 1);
		if (stat.has_boxer_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_21, 1);
	}
	else
	{
		target_gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_3, 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_4, 0);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_8, 1);
	}

	//竞技场成就
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_1, 1);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_10, stat.use_medicine_times);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_9, stat.realive_times);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_6, (int)stat.total_damage_all);

	const ColossumBattleRewardCfg* battle_reward_cfg = LOGIC_CONFIG->GetColosseumConfig()->RandBattleRewardCfg();
	if (NULL != battle_reward_cfg)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(battle_reward_cfg->integral, MONEY_TYPE_COLOSSEUM, __FUNCTION__) ||
			!role->GetRoleModuleManager()->GetMoney()->AddCoinBind(battle_reward_cfg->bind_coin, __FUNCTION__))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		}

		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporaryVec(battle_reward_cfg->reward_item_list))
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutVec(battle_reward_cfg->reward_item_list, PUT_REASON_COLOSSEUM_BATTLE_REWARD);
		}
		else
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendMail(battle_reward_cfg->reward_item_list, SEND_MAIL_TYPE_COLOSSEUM_BATTLE_REWARD);
		}

		Protocol::SCBattleRewardNotice msg;
		msg.reward_group = battle_reward_cfg->battle_price;
		role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
	}

	this->SendFightResultNotice(role, pos, target_pos, is_win, 0, is_sweep);
	this->SendRoleInfo(role);
}

void ColosseumManager::SysReportFromHidden(int role_id, int target_uid, bool is_win, const ColosseumFightStatistic& stat, bool is_sweep)
{
	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(role_id), &pos, &is_in_pool);
	if (NULL == gladiator) return;

	int target_pos = 0;
	bool target_is_in_pool = false;
	Gladiator* target_gladiator = this->GetUserGladiator(UidToInt(target_uid), &target_pos, &target_is_in_pool);
	if (NULL == target_gladiator) return;

	gladiator->SysReport(target_gladiator, is_win, false); // gladiator先动的手
	target_gladiator->SysReport(gladiator, !is_win, true); // target是被打的一方

	gladiator->m_today_fight_count += 1;
	this->SetPosDirty(pos, is_in_pool);

	if (is_win)
	{
		if (!is_in_pool && !target_is_in_pool &&
			pos > target_pos)	// 如果我的排名在目标排名的后面
		{
			gladiator->SetPos(target_pos, true);
			if (-1 == gladiator->m_highest_rank)
			{
				gladiator->m_highest_rank = target_pos;
			}
			else
			{
				gladiator->m_highest_rank = GetMin(target_pos, gladiator->m_highest_rank);
			}

			target_gladiator->SetPos(pos, true);

			gamelog::g_log_colosseum.printf(LL_INFO, "ColosseumManager::SysReport attacker[%d,%s]=>pos[%d], defender[%d,%s]=>pos[%d]",
				gladiator->GetUid(), gladiator->GetName(), target_pos, target_gladiator->GetUid(), target_gladiator->GetName(), pos);

			this->SetRankUser(target_pos, gladiator);
			this->SetRankUser(pos, target_gladiator);

			this->RefreshTarget(gladiator);
			this->RefreshTarget(target_gladiator);

			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_0, target_pos + 1);

			
		}

		gladiator->CheckHighestRankReward();

		int win_streak = gladiator->GetColosseumParam(COLOSSEUN_PARAM_TYPE_4);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_4, win_streak + 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_5, win_streak + 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_2, 1);
		if (gladiator->m_total_capability < target_gladiator->m_total_capability)
		{
			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_7, 1);
		}
		if (stat.round_num >= 8)
		{
			gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_11, 1);
		}
		if (!stat.has_woman_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_12, 1);
		if (!stat.has_man_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_13, 1);
		if (stat.has_swordsman_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_14, 1);
		if (stat.has_start_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_15, 1);
		if (stat.has_lich_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_16, 1);
		if (stat.has_berserk_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_17, 1);
		if (stat.has_arrower_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_18, 1);
		if (stat.has_doctor_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_19, 1);
		if (stat.has_tiewei_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_20, 1);
		if (stat.has_boxer_fight)gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_21, 1);
	}
	else
	{
		target_gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_3, 1);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_4, 0);
		gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_8, 1);
	}

	//竞技场成就
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_1, 1);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_10, stat.use_medicine_times);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_9, stat.realive_times);
	gladiator->SetColosseumParam(COLOSSEUN_PARAM_TYPE_6, (int)stat.total_damage_all);


	crossgameprotocal::GameHiddenColosseumSyncNewRankAfterSysReport sync_newrank;
	sync_newrank.is_in_pool = is_in_pool ? 1 : 0;
	sync_newrank.is_win = is_win ? 1 : 0;
	sync_newrank.new_rank = target_pos;
	sync_newrank.old_rank = pos;
	sync_newrank.is_sweep = is_sweep ? 1 : 0;
	sync_newrank.role_id = role_id;
	sync_newrank.target_uid = target_uid;
	sync_newrank.target_is_in_pool = target_is_in_pool ? 1 : 0;
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&sync_newrank, sizeof(sync_newrank));

	this->SendRoleInfoToHidden(role_id);
}

void ColosseumManager::SyncNewRankFromGameAfterSysReportFromHidden(int role_id, int target_uid, bool is_win, bool is_in_pool, bool target_is_in_pool, int old_rank, int new_rank, bool is_sweep)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role) return;

	if (is_win)
	{
		if (!is_in_pool && !target_is_in_pool &&
			old_rank > new_rank)	// 如果我的排名在目标排名的后面
		{
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COLOSSEUM, new_rank + 1);

			EventHandler::Instance().OnRoleTopRank(role, new_rank);
		}
	}

	this->SendFightResultNotice(role, old_rank, new_rank, is_win, 0, is_sweep);
}

void ColosseumManager::OnRoleGetAchievement(Role * role, int seq)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	Gladiator* user_gladiator = this->GetUserGladiator(role->GetUserId());
	if (user_gladiator == NULL)return;
	CommonDataParam &common_data = role->GetRoleModuleManager()->GetCommonData();
	const ColosseumAchievementCfg* cfg = LOGIC_CONFIG->GetColosseumConfig()->GetAchievementCfg(seq);
	if (NULL != cfg)
	{
		int param = user_gladiator->GetColosseumParam(cfg->achieve_type);
		bool ret_achievement = false;
		switch (cfg->achieve_type)
		{
		case COLOSSEUN_PARAM_TYPE_0://排名第一是：0，进入的值预先+1，这个是倒序的
			if (param == 0 || cfg->ach_param1 < param)return;
			ret_achievement = true;
			break;
		case COLOSSEUN_PARAM_TYPE_1:
		case COLOSSEUN_PARAM_TYPE_2:
		case COLOSSEUN_PARAM_TYPE_3:
		case COLOSSEUN_PARAM_TYPE_4:
		case COLOSSEUN_PARAM_TYPE_5:
		case COLOSSEUN_PARAM_TYPE_6:
		case COLOSSEUN_PARAM_TYPE_7:
		case COLOSSEUN_PARAM_TYPE_8:
		case COLOSSEUN_PARAM_TYPE_9:
		case COLOSSEUN_PARAM_TYPE_10:
		case COLOSSEUN_PARAM_TYPE_11:
		case COLOSSEUN_PARAM_TYPE_12:
		case COLOSSEUN_PARAM_TYPE_13:
		case COLOSSEUN_PARAM_TYPE_14:
		case COLOSSEUN_PARAM_TYPE_15:
		case COLOSSEUN_PARAM_TYPE_16:
		case COLOSSEUN_PARAM_TYPE_17:
		case COLOSSEUN_PARAM_TYPE_18:
		case COLOSSEUN_PARAM_TYPE_19:
		case COLOSSEUN_PARAM_TYPE_20:
		case COLOSSEUN_PARAM_TYPE_21:
			if (cfg->ach_param1 > param)return;
			ret_achievement = true;
			break;
		default:
			break;
		}
		if (ret_achievement)
		{
			//检查是否已经完成过了
			if (common_data.colosseum_task_flag.IsBitSet(cfg->sn))
			{
				role->NoticeNum(errornum::EN_COLOSSEUM_ACHIEVEMENT_HAS_GET);
				return;
			}
			common_data.colosseum_task_flag.SetBit(cfg->sn);
			if (cfg->price_num > 0)
			{
				switch (cfg->price_type)
				{
				case MONEY_TYPE_COLOSSEUM:
					role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->price_num, MONEY_TYPE_COLOSSEUM, "OnRoleGetAchievement");
					break;
				default:
					break;
				}
			}

			//获得奖励
			if (cfg->item_list.size() > 0)
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)cfg->item_list.size(), &cfg->item_list[0]))
				{
					role->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
					return;
				}
				
				if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->item_list.size(), &cfg->item_list[0], PUT_REASON_COLOSSEUM))
				{
					gamelog::g_log_colosseum.printf(LL_INFO, "ColosseumManager::OnRoleGetAchievement user[%s %d] putlist_err sn[%d]", role->GetName(), role->GetUID(), cfg->sn);
					return;
				}
				std::string log_str = STRING_SPRINTF("ColosseumManager::OnRoleGetAchievement user[%s %d] rewards:{", role->GetName(), role->GetUID());
				for (int item_nun = 0; item_nun < (short)cfg->item_list.size(); item_nun++)
				{
					log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d] ", cfg->item_list[item_nun].item_id, cfg->item_list[item_nun].num, cfg->item_list[item_nun].is_bind);
				}
				log_str += "}";
				gamelog::g_log_colosseum.printf(LL_INFO, log_str.c_str());
			}
		}
		else
		{
			role->NoticeNum(errornum::EN_COLOSSEUM_ACHIEVEMENT_NO_COMPLETE);
		}
	}
	this->SendRoleAchievement(role);
}

void ColosseumManager::SendRoleAchievement(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	CommonDataParam& comon_data = role->GetRoleModuleManager()->GetCommonData();
	static Protocol::SCColosseumAchievementInfo info;
	info.next_refresh_tamp = WorldStatus::Instance().GetColosseumRefreshTamp();
	info.colosseum_task_flag = comon_data.colosseum_task_flag;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void ColosseumManager::GameHiddenSendRoleInfo(int role_id)
{
	static Protocol::SCColosseumRoleInfo info;

	int pos = 0;
	bool is_in_pool = false;
	Gladiator * gladiator = this->GetUserGladiator(role_id, &pos, &is_in_pool);
	if (NULL == gladiator) return;

	info.rank = gladiator->GetPos() + 1;
	info.today_fight_count = gladiator->m_today_fight_count;
	info.fetch_exp_flag = gladiator->m_fetch_exp_flag;

	info.player_num_in_pool = m_pool_user_num;
	info.is_out_of_rank_list = (is_in_pool ? 1 : 0);

	info.capability = gladiator->m_total_capability;

	const ColosseumOtherCfg & colo_other = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
	int total_coin = info.player_num_in_pool <= colo_other.min_pool_num ? colo_other.bottom_pool_coin : colo_other.reward_pool_coin;
	info.can_get_coin = info.player_num_in_pool > 0 ? total_coin / info.player_num_in_pool : 0;

	int colosseum_pool_credits = (info.player_num_in_pool <= colo_other.min_pool_num ? colo_other.min_colosseum_pool_credits : colo_other.max_colosseum_pool_credits);
	info.get_colosseum_credits = info.player_num_in_pool > 0 ? colosseum_pool_credits / info.player_num_in_pool : 0;

	info.highest_rank = gladiator->m_highest_rank + 1;

	InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (char *)&info, sizeof(info));
	this->GameHiddenSendRankRewardInfo(role_id);
}

void ColosseumManager::GameHiddenSendRankRewardInfo(int role_id)
{
	int pos = 0;
	bool is_in_pool = false;
	Gladiator * gladiator = this->GetUserGladiator(role_id, &pos, &is_in_pool);
	if (NULL == gladiator) return;

	Protocol::SCColosseumRankRewardInfo crri;
	crri.history_highest_rank = gladiator->m_highest_rank + 1;
	crri.rank_reward_fetch_flag = gladiator->m_rank_award_fetch_flag;

	InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&crri, sizeof(crri));
}

void ColosseumManager::SendRoleInfo(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (NULL == role) return;

	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
	if (NULL == gladiator) return;

	static Protocol::SCColosseumRoleInfo info;
	this->AssembleProtocolRoleInfo(gladiator, &info);

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	this->SendRankRewardInfo(role);
}

void ColosseumManager::SendTopInfo(Role * role)
{
	if (NULL == role) return;

	static Protocol::SCColosseumTopInfo info;
	info.count = 0;

	this->AssembleProtocolTopInfo(&info);

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void ColosseumManager::SendTargetInfo(int uid)
{
	static Protocol::SCColosseumTargetInfo info;
	info.count = 0;

	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid));
	if (NULL == gladiator) return;

	this->AssembleProtocolTargetInfo(gladiator, &info);

	World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(uid), &info, sizeof(info));
}

void ColosseumManager::SendRankInfo(Role * role, int rank_count)
{
	if (NULL == role) return;

	static Protocol::SCColosseumRankInfo info;
	info.count = 0;

	this->AssembleProtocolRankInfo(&info, rank_count);

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void ColosseumManager::SendRankTopSimpleInfo(Role * role)
{
	if(NULL == role) return;	
	Gladiator * gladiator = NULL;
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		gladiator = m_rank_list[i];
		break;
	}
	if(NULL == gladiator) return;

	if (UserCacheManager::Instance().SendRoleSimpleInfo(role, gladiator->GetUid()))
	{
		return;
	}

	Protocol::SCQueryRoleSimpleInfo info;
	info.role_id = gladiator->GetUid();
	F_STRNCPY(info.role_name, gladiator->GetName(), sizeof(info.role_name));
	info.level = gladiator->m_show_level;
	info.top_level = 0;
	info.capability = gladiator->m_total_capability;
	info.appearance_info = gladiator->m_appearance;
	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void ColosseumManager::FetchRankReward(Role * role, int award_seq)
{
	if (award_seq < 0 || award_seq >= BIT_COUNT_OF_LONG_64) return;

	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId(), &pos, &is_in_pool);
	if (NULL == gladiator) return;

	if (0 != (gladiator->m_rank_award_fetch_flag & (1LL << award_seq)))
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const ColosseumAwardCfg * award_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetAwardCfg(award_seq);
	if (NULL == award_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (gladiator->m_highest_rank < 0 || gladiator->m_highest_rank >= award_cfg->max_rank)
	{
		role->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	if (!award_cfg->award_list.empty() && !role->GetRoleModuleManager()->GetKnapsack()->PutList((short)award_cfg->award_list.size(), &award_cfg->award_list[0], PUT_REASON_COLOSSEUM))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(award_cfg->award_coin, __FUNCTION__);

	// 检测通过，给予
	gladiator->m_rank_award_fetch_flag |= (1LL << award_seq);
	this->SetPosDirty(pos, is_in_pool);

	this->SendRankRewardInfo(role);
}

void ColosseumManager::SendRankRewardInfo(Role * role)
{
	if (NULL == role) return;

	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
	if (NULL == gladiator) return;

	Protocol::SCColosseumRankRewardInfo crri;
	this->AssembleProtocolRankRewardInfo(gladiator, &crri);

	role->GetRoleModuleManager()->NetSend((const void *)&crri, sizeof(crri));
}

void ColosseumManager::FetchRankEveryDayReward(Role * role)
{
	if(NULL == role) return;

	CommonDataParam& com_data = role->GetRoleModuleManager()->GetCommonData();
	std::vector<ItemConfigData> real_put_vec;
	int real_coinbind = 0;
	int real_colosseum = 0;
	if (0 != com_data.gladiator_everyday_reward_count)
	{
		//累计3天奖励
		/*for (int i = com_data.gladiator_everyday_reward_count - 1; i < com_data.gladiator_everyday_reward_count && i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST; i++)
		{
			const ColosseumAwardCfg * award_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRandRewardCfg(com_data.gladiator_everyday_reward_list[i]);
			if (award_cfg != NULL)
			{
				real_coinbind += award_cfg->award_coin;
				real_colosseum += award_cfg->colosseum_credits;
				for (std::vector<ItemConfigData>::const_iterator it = award_cfg->award_list.begin();it != award_cfg->award_list.end();it++)
				{
					real_put_vec.push_back(*it);
				}
			}
		}*/
		//只累计一天
		const ColosseumAwardCfg * award_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRandRewardCfg(com_data.gladiator_everyday_reward_list[0]);
		if (award_cfg != NULL)
		{
			real_coinbind += award_cfg->award_coin;
			real_colosseum += award_cfg->colosseum_credits;
			for (std::vector<ItemConfigData>::const_iterator it = award_cfg->award_list.begin(); it != award_cfg->award_list.end(); it++)
			{
				real_put_vec.push_back(*it);
			}

		}

		if ((short)real_put_vec.size() > 0)
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)real_put_vec.size(), &real_put_vec[0]))
			{
				role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}

			if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)real_put_vec.size(), &real_put_vec[0], PUT_REASON_COLOSSEUM))
			{
				gamelog::g_log_colosseum.printf(LL_INFO, "ColosseumManager::FetchRankEveryDayReward err user[%d,%s] putlist err", role->GetUID(), role->GetName());
			}
		}

		if (real_coinbind > 0)role->GetRoleModuleManager()->GetMoney()->AddCoinBind(real_coinbind, "FetchRankEveryDayReward");
		if (real_colosseum > 0)role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(real_colosseum, MONEY_TYPE_COLOSSEUM, "FetchRankEveryDayReward");

		com_data.gladiator_everyday_reward_count = 0;
		memset(com_data.gladiator_everyday_reward_list, 0, sizeof(com_data.gladiator_everyday_reward_list));

		if (!CrossConfig::Instance().IsHiddenServer())
		{
			Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
			if (NULL != gladiator)
			{
				gladiator->SendEveryDayRankRewardInfo();
			}
		}
		else
		{
			Protocol::SCColosseumEveryDayRewardInfo info;
			info.count = 0;
			for (int i = 0; i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST; i++)
			{
				info.rank_pos_list[i] = com_data.gladiator_everyday_reward_list[i];
				info.count++;
			}
			World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(role->GetUID()), &info, sizeof(info));
		}
	}
}

void ColosseumManager::GmColosseumHandler(int op_type)
{
	enum ColosseumOrderType
	{
		COLO_ORDER_SETTLE_REWARDS_NOW = 0,		// 立刻结算古神宝库
		COLO_ORDER_ADD_IN_GODPOOL_NOW = 1,		// 立刻执行入库逻辑
		COLO_ORDER_PRINT_IN_POOL_LIST = 2,		// 输出入库玩家列表
		COLO_ORDER_PRINT_REFRESH_TIME = 3,		// 输出下次刷新时间
		COLO_ORDER_PRINT_IN_POOL_TIME = 4,		// 输出下次入库时间
		COLO_ORDER_PRINT_RANKING_LIST = 5,		// 输出当前排名列表
		COLO_ORDER_SAVE_USER_LIST_NOW = 6,		// 立刻执行一次保存
		COLO_ORDER_UPDATE_ROBOT_LEVEL = 7,		// 执行一次UpdateRobotLevel
	};

	time_t now_sec = EngineAdapter::Instance().Time();
	switch (op_type)
	{
	case COLO_ORDER_SETTLE_REWARDS_NOW:
		m_next_switch_time = now_sec;
		break;
	case COLO_ORDER_ADD_IN_GODPOOL_NOW:
		m_next_pool_time = now_sec;
		break;
	case COLO_ORDER_PRINT_IN_POOL_LIST:
		printf("\nColosseum Order::Print All Pool User -> Num: %d", m_pool_user_num);
		for (int i = 0; i < m_pool_user_num; ++i)
		{
			if (NULL != m_pool_user_list[i])
			{
				printf("\n\tPoolRank: %d | User[%d,%s]", i, m_pool_user_list[i]->m_uid, m_pool_user_list[i]->m_name);
			}
		}
		printf("\n");
		break;
	case COLO_ORDER_PRINT_REFRESH_TIME:
		printf("\nPrint Next Refresh Time -> %lld\n", (long long)m_next_switch_time);
		break;
	case COLO_ORDER_PRINT_IN_POOL_TIME:
		printf("\nPrint Next In Pool Time -> %lld\n", (long long)m_next_pool_time);
		break;
	case COLO_ORDER_PRINT_RANKING_LIST:
		printf("\nPrint Colosseum Rank List -> Num: %d", m_rank_user_num);
		for (int i = 0; i < m_rank_user_num; ++i)
		{
			if (NULL != m_rank_list[i])
			{
				printf("\n\tRank: %d | User[%d,%s]", i, m_rank_list[i]->m_uid, m_rank_list[i]->m_name);
			}
		}
		printf("\nEnd.\n");
		break;
	case COLO_ORDER_SAVE_USER_LIST_NOW:
		m_next_save_time = 0;
		break;

	case COLO_ORDER_UPDATE_ROBOT_LEVEL:
		{
			this->UpdateRobotLevel();
		}
		break;
	}
}

void ColosseumManager::GmColosseumHandler(Role * role, int op_type, int param1, int param2)
{
	enum ColosseumOrderType
	{
		COLO_ORDER_ADD_ROBOT = 0,		// 加机器人
		COLO_SIMULATE_RESULT = 1,		// 模拟胜利结果
		COLO_FIGHT_REQ = 2,				// 战斗
	};

	if (NULL == role) return;

	switch (op_type)
	{
	case COLO_ORDER_ADD_ROBOT:
		this->AddRobotData(param1);
		break;
	case COLO_SIMULATE_RESULT:
		this->SysReport(role, param1, 0 != param2);
		break;
	case COLO_FIGHT_REQ:
		this->OnFightReq(role, param1);
		break;
	}
}

void ColosseumManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	// 跨服不需要ChallengeField
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int server_open_day = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	memset(m_defender_buff_id_list, 0, sizeof(m_defender_buff_id_list));
	int buff_num = 0;
	LOGIC_CONFIG->GetColosseumConfig()->GetRandDefenderBuffId(COLOSSEUM_DEFENDER_BUFF_NUM, &buff_num, m_defender_buff_id_list, server_open_day);

	// 跨服不需要ChallengeField， 暂时先把存储数据的CheckSave屏蔽掉
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->CheckGladiatorUnique();

	for (int i = 0; i < m_rank_user_num; ++i)
	{
		Gladiator * user = m_rank_list[i];
		if (NULL != user)
		{
			user->OnResetColosseunmModuleData(false);
			this->SetRankListDirty(i);
		}
	}

	for (int i = 0; i < m_pool_user_num; ++i)
	{
		Gladiator * user = m_pool_user_list[i];
		if (NULL != user)
		{
			user->OnResetColosseunmModuleData(true);
			this->SetPoolListDirty(i);
		}
	}

	m_is_change = true;
}

void ColosseumManager::GetDefenderBuffid(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_buff_list)
{
	if (NULL == out_num || NULL == out_buff_list) return;

	*out_num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_defender_buff_id_list) && *out_num < max_num; ++i)
	{
		out_buff_list[*out_num] = m_defender_buff_id_list[i];
		*out_num += 1;
	}
}

void ColosseumManager::InitDefenderBuffId(int buff_num, const int* buff_id_list)
{
	if (0 == buff_num)
	{
		int server_open_day = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
		memset(m_defender_buff_id_list, 0, sizeof(m_defender_buff_id_list));
		LOGIC_CONFIG->GetColosseumConfig()->GetRandDefenderBuffId(COLOSSEUM_DEFENDER_BUFF_NUM, &m_defender_buff_num, m_defender_buff_id_list, server_open_day);
	}
	else
	{
		for (int i = 0; i < buff_num; ++i)
		{
			m_defender_buff_id_list[i] = buff_id_list[i];
		}
	}
}

void ColosseumManager::SendReportInfo(Role * role)
{
	if (NULL == role) return;

	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
	if (NULL == gladiator) return;

	static Protocol::SCColosseumReportInfo info;
	info.count = 0;

	this->AssembleProtocolReportInfo(gladiator, &info);

	int len = sizeof(info) - sizeof(info.report_list) + info.count * sizeof(Report);

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, len);
}

ColosseumManager::ColosseumManager() : m_data_state(CHALLENGE_FIELD_DATA_STATE_INVALID),
m_is_first(false), m_is_change(false), m_last_save_list_count(-1), m_next_save_time(0u),
m_next_pool_time(0u), m_next_switch_time(0u), m_rank_user_num(0), m_rank_user_capacity(0),
m_rank_list(NULL), m_dirty_mark(NULL), m_old_state(NULL), m_pool_user_num(0), m_pool_user_capacity(0),
m_pool_user_list(NULL), m_pool_dirty_mark(NULL), m_pool_old_state(NULL), m_robot_id_generator(0),
m_defender_buff_num(0)
{
	m_user_rank_map.clear();
	m_user_pool_map.clear();

	{	// 设定初状态与时间戳
		time_t now_sec = EngineAdapter::Instance().Time();
		m_next_pool_time = now_sec + EngineAdapter::Instance().NextHourInterval(0, 0);					// 每小时入库一次；
		m_next_switch_time = now_sec + EngineAdapter::Instance().NextWeekInterval(MONDAY, 0, 0, 0);		// 每周一零点结算；
	}

	this->ResizeRankList(CHALLENGE_FIELD_ONCE_OPER_USER_COUNT);
	this->ResizePoolList(CHALLENGE_FIELD_ONCE_OPER_USER_COUNT);

	memset(m_defender_buff_id_list, 0, sizeof(m_defender_buff_id_list));
}

ColosseumManager::~ColosseumManager()
{
	this->ReleaseRankList();
	this->ReleasePoolList();
}

void ColosseumManager::ReleaseRankList()
{
	if (NULL != m_rank_list)
	{
		for (int i = 0; i < m_rank_user_num; ++i)
		{
			if (NULL != m_rank_list[i])
			{
				delete m_rank_list[i];
				m_rank_list[i] = NULL;
			}
		}
	}

	if (NULL != m_rank_list)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)m_rank_list;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_dirty_mark;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_old_state;
		delete[]tmp;
	}

	m_rank_user_num = 0;
	m_rank_user_capacity = 0;
}

void ColosseumManager::ReleasePoolList()
{
	if (NULL != m_pool_user_list)
	{
		for (int i = 0; i < m_pool_user_num; ++i)
		{
			if (NULL != m_pool_user_list[i])
			{
				delete m_pool_user_list[i];
				m_pool_user_list[i] = NULL;
			}
		}
	}

	if (NULL != m_pool_user_list)
	{
		ServerMsgMem *tmp = (ServerMsgMem*)m_pool_user_list;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_pool_dirty_mark;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_pool_old_state;
		delete[]tmp;
	}

	m_pool_user_num = 0;
	m_pool_user_capacity = 0;
}

void ColosseumManager::ResizeRankList(int size)
{
	if (m_rank_user_capacity >= size) return; // 不允许缩小List

#ifdef _DEBUG
	assert(m_rank_user_num <= m_rank_user_capacity && m_rank_user_num >= 0);
#endif

	Gladiator** rank_list_new = (Gladiator**)(new ServerMsgMem[sizeof(Gladiator*) * size]);
	bool *dirty_mark_new = (bool *)(new ServerMsgMem[sizeof(bool) * size]);
	bool *old_state_new = (bool *)(new ServerMsgMem[sizeof(bool) * size]);

	memset(rank_list_new, 0, sizeof(Gladiator *) * size);
	memset(dirty_mark_new, 0, sizeof(bool) * size);
	memset(old_state_new, 0, sizeof(bool) * size);

	if (NULL != m_rank_list)
	{
		memcpy(rank_list_new, m_rank_list, sizeof(Gladiator*) * m_rank_user_num);
		memcpy(dirty_mark_new, m_dirty_mark, sizeof(bool) * m_rank_user_num);
		memcpy(old_state_new, m_old_state, sizeof(bool) * m_rank_user_num);

		ServerMsgMem *tmp = (ServerMsgMem*)m_rank_list;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_dirty_mark;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_old_state;
		delete[]tmp;
	}

	m_rank_list = rank_list_new;
	m_dirty_mark = dirty_mark_new;
	m_old_state = old_state_new;

	m_rank_user_capacity = size;
}

void ColosseumManager::ResizePoolList(int size)
{
	if (m_pool_user_capacity >= size) return; // 不允许缩小List

	Gladiator** pool_list_new = (Gladiator**)(new ServerMsgMem[sizeof(Gladiator*) * size]);
	bool *dirty_mark_new = (bool *)(new ServerMsgMem[sizeof(bool) * size]);
	bool *old_state_new = (bool *)(new ServerMsgMem[sizeof(bool) * size]);

	memset(pool_list_new, 0, sizeof(Gladiator *) * size);
	memset(dirty_mark_new, 0, sizeof(bool) * size);
	memset(old_state_new, 0, sizeof(bool) * size);

	if (NULL != m_pool_user_list)
	{
		memcpy(pool_list_new, m_pool_user_list, sizeof(Gladiator*) * m_pool_user_num);
		memcpy(dirty_mark_new, m_pool_dirty_mark, sizeof(bool) * m_pool_user_num);
		memcpy(old_state_new, m_pool_old_state, sizeof(bool) * m_pool_user_num);

		ServerMsgMem *tmp = (ServerMsgMem*)m_pool_user_list;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_pool_dirty_mark;
		delete[]tmp;
		tmp = (ServerMsgMem*)m_pool_old_state;
		delete[]tmp;
	}

	m_pool_user_list = pool_list_new;
	m_pool_dirty_mark = dirty_mark_new;
	m_pool_old_state = old_state_new;

	m_pool_user_capacity = size;
}

void ColosseumManager::SetRankUser(int rankpos, Gladiator *gladiator, bool is_init /*= false*/)
{
	if (rankpos >= 0 && rankpos < CHALLENGE_FIELD_MAX_USER)
	{
		if (rankpos >= m_rank_user_num)
		{
			if (rankpos >= m_rank_user_capacity)
			{
				UNSTD_STATIC_CHECK(CHALLENGE_FIELD_INCREASE_STEP > 0);

				int new_size = (rankpos / CHALLENGE_FIELD_INCREASE_STEP + 1) * CHALLENGE_FIELD_INCREASE_STEP;
				if (new_size > CHALLENGE_FIELD_MAX_USER) new_size = CHALLENGE_FIELD_MAX_USER;
				this->ResizeRankList(new_size);
			}
			m_rank_user_num = rankpos + 1;
		}

		m_rank_list[rankpos] = gladiator;

		if (NULL != gladiator)
		{
			gladiator->SetPos(rankpos);
			gladiator->SetIsInPool(false);
			m_user_rank_map[gladiator->GetUserID()] = rankpos;
			m_user_pool_map.erase(gladiator->GetUserID());

			gamelog::g_log_colosseum.printf(LL_INFO, "SetRankUser gladiator[%d,%s] pos[%d] history_pos[%d]", gladiator->GetUid(), gladiator->GetName(), rankpos, gladiator->m_highest_rank);
		}

		if (!is_init) this->SetRankListDirty(rankpos);
	}
}

void ColosseumManager::SetPoolUser(int poolpos, Gladiator *gladiator, bool is_init /*= false*/)
{
	if (poolpos >= 0 && poolpos < CHALLENGE_FIELD_MAX_USER)
	{
		if (poolpos >= m_pool_user_num)
		{
			if (poolpos >= m_pool_user_capacity)
			{
				UNSTD_STATIC_CHECK(CHALLENGE_FIELD_INCREASE_STEP > 0);

				int new_size = (poolpos / CHALLENGE_FIELD_INCREASE_STEP + 1) * CHALLENGE_FIELD_INCREASE_STEP;
				if (new_size > CHALLENGE_FIELD_MAX_USER) new_size = CHALLENGE_FIELD_MAX_USER;
				this->ResizePoolList(new_size);
			}
			m_pool_user_num = poolpos + 1;
		}

		m_pool_user_list[poolpos] = gladiator;

		if (NULL != gladiator)
		{
			gladiator->SetPos(poolpos);
			gladiator->SetIsInPool(false);
			m_user_pool_map[gladiator->GetUserID()] = poolpos;
			m_user_rank_map.erase(gladiator->GetUserID());
			gamelog::g_log_colosseum.printf(LL_INFO, "SetPoolUser gladiator[%d,%s] pos[%d]", gladiator->GetUid(), gladiator->GetName(), poolpos);
		}

		if (!is_init) this->SetPoolListDirty(poolpos);
	}
}

void ColosseumManager::AddRankUser(Gladiator* gladiator)
{
	if (m_rank_user_num >= CHALLENGE_FIELD_MAX_USER) return;

	if (m_rank_user_num >= m_rank_user_capacity)
	{
		UNSTD_STATIC_CHECK(CHALLENGE_FIELD_INCREASE_STEP > 0);

		int new_size = m_rank_user_capacity + CHALLENGE_FIELD_INCREASE_STEP;
		if (new_size > CHALLENGE_FIELD_MAX_USER) new_size = CHALLENGE_FIELD_MAX_USER;
		this->ResizeRankList(new_size);
	}

	int add_pos = m_rank_user_num++;
	m_rank_list[add_pos] = gladiator;

	if (NULL != gladiator)
	{
		gladiator->SetPos(add_pos);
		gladiator->SetIsInPool(false);
		if (-1 == gladiator->m_highest_rank)
		{
			gladiator->m_highest_rank = add_pos;
		}
		else
		{
			gladiator->m_highest_rank = GetMin(gladiator->m_highest_rank, add_pos);
		}
		m_user_rank_map[gladiator->GetUserID()] = add_pos;
		m_user_pool_map.erase(gladiator->GetUserID());

		gamelog::g_log_colosseum.printf(LL_INFO, "AddRankUser gladiator[%d,%s] pos[%d] history_pos[%d]", gladiator->GetUid(), gladiator->GetName(), add_pos, gladiator->m_highest_rank);
	}

	this->SetRankListDirty(add_pos);
}

void ColosseumManager::AddPoolUser(Gladiator* gladiator)
{
	if (m_pool_user_num + 1 > CHALLENGE_FIELD_MAX_USER) return;

	if (m_pool_user_num + 1 > m_pool_user_capacity)
	{
		UNSTD_STATIC_CHECK(CHALLENGE_FIELD_INCREASE_STEP > 0);

		int new_size = m_pool_user_capacity + CHALLENGE_FIELD_INCREASE_STEP;
		if (new_size > CHALLENGE_FIELD_MAX_USER) new_size = CHALLENGE_FIELD_MAX_USER;
		this->ResizePoolList(new_size);
	}

	int add_pos = m_pool_user_num++;
	m_pool_user_list[add_pos] = gladiator;

	if (NULL != gladiator)
	{
		gladiator->SetPos(add_pos);
		gladiator->SetIsInPool(true);

		m_user_pool_map[gladiator->GetUserID()] = add_pos;
		m_user_rank_map.erase(gladiator->GetUserID());
		gamelog::g_log_colosseum.printf(LL_INFO, "AddPoolUser gladiator[%d,%s] pos[%d]", gladiator->GetUid(), gladiator->GetName(), add_pos);

		gladiator->OnAddInPool();
	}

	this->SetPoolListDirty(add_pos);

	if (NULL != gladiator)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(gladiator->GetUserID());
		if (NULL != role)
		{
			this->SendRoleInfo(role);
		}
	}
}

void ColosseumManager::SetRankListDirty(int rankpos)
{
	if (rankpos >= 0 && rankpos < m_rank_user_capacity)
	{
		m_dirty_mark[rankpos] = true;
		m_is_change = true;
	}
}

void ColosseumManager::SetPoolListDirty(int poolpos)
{
	if (poolpos >= 0 && poolpos < m_pool_user_capacity)
	{
		m_pool_dirty_mark[poolpos] = true;
		m_is_change = true;
	}
}

void ColosseumManager::SetPosDirty(int pos, bool is_in_pool)
{
	if (!is_in_pool)
	{
		this->SetRankListDirty(pos);
	}
	else
	{
		this->SetPoolListDirty(pos);
	}
}

void ColosseumManager::ClearRankListDirtyMark(int begin_pos, int end_pos)
{
	if (m_is_change)
	{
		if (begin_pos >= 0 && begin_pos < m_rank_user_capacity && end_pos >= 0 && end_pos <= m_rank_user_capacity)
		{
			for (int i = begin_pos; i < end_pos; ++i)
			{
				m_dirty_mark[i] = false;
				m_old_state[i] = (NULL != m_rank_list[i]);
			}
		}
	}
}

void ColosseumManager::ClearPoolListDirtyMark(int begin_pos, int end_pos)
{
	if (m_is_change)
	{
		if (begin_pos >= 0 && begin_pos < m_pool_user_capacity && end_pos >= 0 && end_pos <= m_pool_user_capacity)
		{
			for (int i = begin_pos; i < end_pos; ++i)
			{
				m_pool_dirty_mark[i] = false;
				m_pool_old_state[i] = (NULL != m_pool_user_list[i]);
			}
		}
	}
}

Gladiator* ColosseumManager::GetUserGladiator(UserID user_id, int* out_pos/* = NULL */, bool* out_is_in_pool/* = NULL */)
{
	RankPosMap::iterator it = m_user_rank_map.find(user_id);
	if (m_user_rank_map.end() != it)
	{
		if (NULL != out_pos) *out_pos = it->second;
		if (NULL != out_is_in_pool) *out_is_in_pool = false;

		return m_rank_list[it->second];
	}

	it = m_user_pool_map.find(user_id);
	if (m_user_pool_map.end() != it)
	{
		if (NULL != out_pos) *out_pos = it->second;
		if (NULL != out_is_in_pool) *out_is_in_pool = true;

		return m_pool_user_list[it->second];
	}

	return NULL;
}

int ColosseumManager::GetRoleRank(int role_id)
{
	Gladiator * gladiator = this->GetUserGladiator(role_id);
	if (gladiator == NULL)
	{
		return 99999;
	}

	return gladiator->GetPos() + 1;
}

bool ColosseumManager::LoadUser(long long next_id_from)
{
	if (this->IsLoadFinish()) return false;

	m_data_state = CHALLENGE_FIELD_DATA_STATE_LOADING;

	RMIInitChallengeUserObjectImpl *impl = new RMIInitChallengeUserObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.InitChallengeUserAsyn(next_id_from, impl);
}

void RMIInitChallengeUserObjectImpl::InitChallengeUserRet(int ret, const ChallengeUserListParam & challenge_user_p)
{
	if (0 != ret)
	{
		printf("ChallengeField::Init Failed ret:%d \n", ret);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitChallengeUserObjectImpl::InitChallengeUserRet_1");
		return;
	}

	std::set<int> uid_list;
	for (int i = 0; i < challenge_user_p.count && i < CHALLENGE_FIELD_ONCE_OPER_USER_COUNT; ++i)
	{
		if (uid_list.end() != uid_list.find(challenge_user_p.user_list[i].role_id)) continue;
		uid_list.insert(challenge_user_p.user_list[i].role_id);

		if (!ColosseumManager::Instance().InitChallengeUser(challenge_user_p.user_list[i]))
		{
			ServerLogic::GetInstServerLogic()->StopServer("RMIInitChallengeUserObjectImpl::InitChallengeUserRet_2");
			return;
		}
	}

	if (challenge_user_p.count > 0)
	{
		ColosseumManager::Instance().LoadUser(challenge_user_p.next_id_from);
	}
	else
	{
		ColosseumManager::Instance().LoadUserSucc();
	}
}

void ColosseumManager::LoadUserSucc()
{
	m_data_state = CHALLENGE_FIELD_DATA_STATE_LOAD_FINISH;
	m_next_save_time = EngineAdapter::Instance().Time() + 60;

	if (0 == m_rank_user_num)
	{
		this->InitRobot();
	}
	else
	{
		for (int i = 0; i < m_rank_user_num; ++i)
		{
			if (NULL == m_rank_list[i])
			{
				this->AddARobotData(i);
			}
		}
	}
}

bool ColosseumManager::InitChallengeUser(const ChallengeUserListParam::ChallengeUserAttr & user_attr)
{
	int rankpos = user_attr.rankpos;
	if (rankpos < -CHALLENGE_FIELD_MAX_USER || rankpos >= CHALLENGE_FIELD_MAX_USER)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "[ChallengeField::Init][rankpos:%d role_id:%d is invalid]",
			rankpos, user_attr.role_id);
		return false;
	}

	if (rankpos >= 0)
	{
		if (rankpos < m_rank_user_num && NULL != m_rank_list[rankpos])
		{
			if (Gladiator::IsRobot(user_attr.role_id))
			{
				return true; // 机器人就无所谓了
			}

			if (m_rank_list[rankpos]->GetUid() == user_attr.role_id)
			{
				return true;
			}

			// rankpos重了，需要人为干涉
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d repeat]", rankpos);
			return false;
		}

		if (m_user_rank_map.end() == m_user_rank_map.find(IntToUid(user_attr.role_id)))
		{
			Gladiator* user = new Gladiator();
			if (Gladiator::IsRobot(user_attr.role_id))
			{
				this->FillRobotData(user, rankpos, user_attr.role_id);
			}
			else
			{
				user->m_uid = user_attr.role_id;
				user->m_today_fight_count = user_attr.today_fight_count;
				user->m_fetch_exp_flag = user_attr.fetch_exp_flag;
				user->m_worship_count = user_attr.worship_count;
				user->m_rank_award_fetch_flag = user_attr.rank_award_fetch_flag;
				user->m_level = user_attr.level;
				user->m_profession = user_attr.profession;
				user->m_avatar_type = user_attr.avatar_type;
				user->m_highest_rank = user_attr.highest_rank;
				user->m_total_capability = user_attr.total_capability;
				user->SetAppearance(user_attr.appearance);
				F_STRNCPY(user->m_name, user_attr.name, sizeof(GameName));
				Gladiator::AssignShowLevel(user, rankpos);
				user->colosseum_param = user_attr.colosseum_param;
				user->colosseum_clear_check = user_attr.colosseum_clear_check;
				{
					PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
					if (NULL == pb_data)
					{
						delete user; user = NULL;
						return false;
					}
					if (!pb_data->ParseFromArray(user_attr.battle_data_buff, user_attr.battle_data_length))
					{
						gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d repeat protobuf parse failed]", rankpos);
						delete user; user = NULL;
						return false;
					}
					if (!user->m_battle_data.SetDataFromPB(*pb_data))
					{
						gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d repeat pb_data set data failed]", rankpos);
						delete user; user = NULL;
						return false;
					}
				}
			}
		
			this->SetRankUser(rankpos, user, true);

			if (user->IsRobot())
			{
				m_robot_id_generator = GetMax(m_robot_id_generator, (IntToUid(user->m_uid).role_id + 1) % MAX_ROLE_ID);
			}
		}
		else
		{
			// 容错处理，防止数据库里边一个role_id有2个以上排名
			this->SetRankUser(rankpos, NULL, false);

			int old_rankpos = m_user_rank_map[IntToUid(user_attr.role_id)];
			if (old_rankpos > rankpos && NULL != m_rank_list[old_rankpos] && user_attr.role_id == m_rank_list[old_rankpos]->GetUid())
			{
				gamelog::g_log_colosseum.printf(LL_WARNING, "ColosseumManager::InitChallengeUser role[%d,%s] old_rankpos[%d] new_rankpos[%d]",
					user_attr.role_id, user_attr.name, old_rankpos, rankpos);

				// 容错处理，外网存在同一玩家处于两个不同排名的情况，这里把玩家更换到更前的排名
				Gladiator* gladiator = m_rank_list[old_rankpos];
				this->SetRankUser(old_rankpos, NULL, false);
				this->SetRankUser(rankpos, gladiator, false);
			}
		}

		m_old_state[rankpos] = true;
	}
	else
	{
		int poolpos = -(rankpos + 1);
		if (poolpos < m_pool_user_num && m_pool_user_list[poolpos] != NULL)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d, poolpos:%d repeat]", rankpos, poolpos);
			return false;
		}

		if (m_user_pool_map.end() == m_user_pool_map.find(IntToUid(user_attr.role_id)))
		{
			Gladiator* user = new Gladiator();
			user->m_uid = user_attr.role_id;
			user->m_today_fight_count = user_attr.today_fight_count;
			user->m_fetch_exp_flag = user_attr.fetch_exp_flag;
			user->m_worship_count = user_attr.worship_count;
			user->m_rank_award_fetch_flag = user_attr.rank_award_fetch_flag;
			user->m_level = user_attr.level;
			user->m_profession = user_attr.profession;
			user->m_avatar_type = user_attr.avatar_type;
			user->m_highest_rank = user_attr.highest_rank;
			user->m_total_capability = user_attr.total_capability;
			user->SetAppearance(user_attr.appearance);
			F_STRNCPY(user->m_name, user_attr.name, sizeof(GameName));
			Gladiator::AssignShowLevel(user, 0);

			{
				PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
				if (NULL == pb_data)
				{
					delete user; user = NULL;
					return false;
				}
				if (!pb_data->ParseFromArray(user_attr.battle_data_buff, user_attr.battle_data_length))
				{
					gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d poolpos:%d repeat protobuf parse failed]", rankpos, poolpos);
					delete user; user = NULL;
					return false;
				}
				if (!user->m_battle_data.SetDataFromPB(*pb_data))
				{
					gamelog::g_log_serious_error.printf(LL_CRITICAL, "ChallengeField::Init][rankpos:%d poolpos:%d repeat pb_data set data failed]", rankpos, poolpos);
					delete user; user = NULL;
					return false;
				}
			}

			this->SetPoolUser(poolpos, user, true);
		}
		else
		{
			// 容错处理，防止数据库里边一个role_id有2个以上排名
			this->SetPoolUser(poolpos, NULL, false);
		}

		m_pool_old_state[poolpos] = true;
	}

	return true;
}

void ColosseumManager::GetChangeRankList(ChallengeUserListParam *p, int pos_begin, int* out_pos_end)
{
	if (!m_is_change || pos_begin < 0 || pos_begin >= m_rank_user_capacity)
	{
		p->count = 0;
		return;
	}

	int count = 0;
	int i = pos_begin;
	for (; i < m_rank_user_capacity && count < CHALLENGE_FIELD_ONCE_OPER_USER_COUNT; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_rank_list[i])
			{
				if (!m_old_state[i]) continue;

				p->user_list[count].change_state = structcommon::CS_DELETE;
				p->user_list[count].rankpos = i;
				p->user_list[count].role_id = 0;
			}
			else
			{
				p->user_list[count].change_state = m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				p->user_list[count].rankpos = i;
				p->user_list[count].role_id = UidToInt(m_rank_list[i]->GetUserID());

				ChallengeUserListParam::ChallengeUserAttr &data = p->user_list[count];
				Gladiator* gladiator = m_rank_list[i];
				data.role_id = gladiator->m_uid;
				data.today_fight_count = gladiator->m_today_fight_count;
				data.fetch_exp_flag = gladiator->m_fetch_exp_flag;
				data.worship_count = gladiator->m_worship_count;
				data.rank_award_fetch_flag = gladiator->m_rank_award_fetch_flag;
				data.highest_rank = gladiator->m_highest_rank;
				data.total_capability = gladiator->m_total_capability;

				{
					PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
					if (NULL == pb_data)
					{
						continue;
					}

					gladiator->m_battle_data.GetPBData(pb_data);
					if (!pb_data->SerializeToArray(data.battle_data_buff, sizeof(data.battle_data_buff)))
					{
#ifdef _DEBUG
						assert(0);
#endif
						continue;
					}
					data.battle_data_length = pb_data->GetCachedSize();

#ifdef _DEBUG
					assert(data.battle_data_length == pb_data->ByteSize());
#endif
				}

				data.level = gladiator->m_level;
				data.profession = gladiator->m_profession;
				data.avatar_type = gladiator->m_avatar_type;
				data.appearance = gladiator->GetAppearance();
				F_STRNCPY(data.name, gladiator->m_name, sizeof(GameName));
				data.colosseum_param = gladiator->colosseum_param;
				data.colosseum_clear_check = gladiator->colosseum_clear_check;
			}

			++count;
		}
	}
	p->count = count;
	if (NULL != out_pos_end) *out_pos_end = i;
}

void ColosseumManager::GetChangePoolList(ChallengeUserListParam *p, int pos_begin, int* out_pos_end)
{
	if (!m_is_change || pos_begin < 0 || pos_begin >= m_pool_user_capacity)
	{
		p->count = 0;
		return;
	}

	int count = 0;
	int i = pos_begin;
	for (; i < m_pool_user_capacity && count < CHALLENGE_FIELD_ONCE_OPER_USER_COUNT; ++i)
	{
		if (m_pool_dirty_mark[i])
		{
			int poolpos_in_db = -(i + 1);
			if (NULL == m_pool_user_list[i])
			{
				if (!m_pool_old_state[i]) continue;

				p->user_list[count].change_state = structcommon::CS_DELETE;
				p->user_list[count].rankpos = poolpos_in_db;
				p->user_list[count].role_id = 0;
			}
			else
			{
				p->user_list[count].change_state = m_pool_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				p->user_list[count].rankpos = poolpos_in_db;
				p->user_list[count].role_id = UidToInt(m_pool_user_list[i]->GetUserID());

				ChallengeUserListParam::ChallengeUserAttr &data = p->user_list[count];
				Gladiator* gladiator = m_pool_user_list[i];
				data.role_id = gladiator->m_uid;
				data.today_fight_count = gladiator->m_today_fight_count;
				data.fetch_exp_flag = gladiator->m_fetch_exp_flag;
				data.worship_count = gladiator->m_worship_count;
				data.rank_award_fetch_flag = gladiator->m_rank_award_fetch_flag;
				data.highest_rank = gladiator->m_highest_rank;
				data.total_capability = gladiator->m_total_capability;

				{
					PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
					if (NULL == pb_data)
					{
						continue;
					}

					gladiator->m_battle_data.GetPBData(pb_data);
					if (!pb_data->SerializeToArray(data.battle_data_buff, sizeof(data.battle_data_buff)))
					{
#ifdef _DEBUG
						assert(0);
#endif
						continue;
					}
					data.battle_data_length = pb_data->GetCachedSize();
				}

				data.level = gladiator->m_level;
				data.profession = gladiator->m_profession;
				data.avatar_type = gladiator->m_avatar_type;
				data.appearance = gladiator->GetAppearance();
				F_STRNCPY(data.name, gladiator->m_name, sizeof(GameName));
			}

			++count;
		}
	}
	p->count = count;
	if (NULL != out_pos_end) *out_pos_end = i;
}

void ColosseumManager::CheckSave(time_t now_second, bool force_save /* = false */)
{
	if (now_second >= m_next_save_time)
	{
		if (m_is_change || force_save)
		{
			this->SaveRankList();
			this->SavePoolList();
			m_is_change = false;
		}

		m_next_save_time = now_second + 60;
	}
}

void ColosseumManager::SaveRankList()
{
	static ChallengeUserListParam p;
	int rank_begin = 0, out_rank_end = 0;

	int loop_count = 0;
	while (++loop_count <= 128)
	{
		this->GetChangeRankList(&p, rank_begin, &out_rank_end);

		if (p.count > 0)
		{
			RMISaveChallengeUserObjectImpl *impl = new RMISaveChallengeUserObjectImpl();

			RMIGlobalClient gc;
			gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			if (gc.SaveChallengeUserAsyn(p, impl))
			{
				this->ClearRankListDirtyMark(rank_begin, out_rank_end);
				rank_begin = out_rank_end;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

void ColosseumManager::SavePoolList()
{
	static ChallengeUserListParam p;
	int rank_begin = 0, out_rank_end = 0;

	int loop_count = 0;
	while (++loop_count <= 128)
	{
		this->GetChangePoolList(&p, rank_begin, &out_rank_end);

		if (p.count > 0)
		{
			RMISaveChallengeUserObjectImpl *impl = new RMISaveChallengeUserObjectImpl();

			RMIGlobalClient gc;
			gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			if (gc.SaveChallengeUserAsyn(p, impl))
			{
				this->ClearPoolListDirtyMark(rank_begin, out_rank_end);
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

bool ColosseumManager::IsLevelLimit(int level)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_COLOSSEUM);
	if (NULL != cfg && level < cfg->limit_level)
	{
		return true;
	}

	return false;
}

int ColosseumManager::GetFreeRankPosSlot()
{
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) return i;
	}

	return -1;
}

void ColosseumManager::RefreshTarget(Gladiator* gladiator, int assign_target_index /* = 0 */, bool is_send_to_hidden /* = false */)
{
	if (NULL == gladiator) return;

	// 前10名时 只选相邻的
	if (gladiator->GetPos() < 10)
	{
		int target_count = 0;
		for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX && target_count < CHALLENGE_FIELD_OPPONENT_MAX; ++i)
		{
			int target_rank = gladiator->GetPos() - i - 1;
			if (target_rank < 0)
			{
				break;
			}

			gladiator->m_target_list[target_count++] = target_rank;
		}

		for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX && target_count < CHALLENGE_FIELD_OPPONENT_MAX; i++)
		{
			gladiator->m_target_list[target_count++] = gladiator->GetPos() + i + 1;
		}
	}
	else
	{
		bool flag = true;
		const ColosseumRefreshCfg * refresh_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRefreshCfg(gladiator->GetPos() + 1);
		if (refresh_cfg != NULL)
		{
			flag = false;
			for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX; ++i)
			{
				const ColosseumRefreshCfg::ItemCfg * item_cfg = refresh_cfg->GetItemCfg(i);
				if (item_cfg == NULL)
				{
					flag = true;
					break;
				}

				gladiator->m_target_list[i] = RandomNum(gladiator->GetPos() * item_cfg->section_left, gladiator->GetPos() * item_cfg->section_right);
			}
		}

		if (flag)
		{
			if (gladiator->GetPos() > 200)
			{
				int rank_param = 160 / 4;
				for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX; ++i)
				{
					gladiator->m_target_list[i] = gladiator->GetPos() - (rank_param * i + RandomNum(rank_param));

					int cant_over = 0;		// 限定循环 100次 防止丝死循环
					while (gladiator->m_target_list[i] == gladiator->GetPos() && cant_over < 100)// 防止刷到自己
					{
						gladiator->m_target_list[i] = gladiator->GetPos() - (rank_param * i + RandomNum(rank_param));
						++cant_over;
					}
				}
			}
			else
			{
				int rank_param = gladiator->GetPos() / 4;
				for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX; ++i)
				{
					gladiator->m_target_list[i] = rank_param * i + RandomNum(rank_param);

					int cant_over = 0;		// 限定循环 100次 防止丝死循环
					while (gladiator->m_target_list[i] == gladiator->GetPos() && cant_over < 100)// 防止刷到自己
					{
						gladiator->m_target_list[i] = rank_param * i + RandomNum(rank_param);
						++cant_over;
					}
				}
			}
		}

		if (assign_target_index > 0)
		{
			gladiator->m_target_list[CHALLENGE_FIELD_OPPONENT_MAX - 1] = assign_target_index;
		}
	}

	if (is_send_to_hidden)
	{
		this->SendTargetInfoToHidden(gladiator->GetUid());
	}
	else
	{
		this->SendTargetInfo(gladiator->GetUid());
	}

#ifdef _DEBUG
		gamelog::g_log_colosseum.buff_printf("RefreshTarget user[%d,%s] cur_rank[%d] assigned_target[%d] target_list[", 
			gladiator->GetUid(), gladiator->GetName(), gladiator->GetPos(), assign_target_index);
		for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX; ++i)
		{
			gamelog::g_log_colosseum.buff_printf("%d,", gladiator->m_target_list[i]);
		}
		gamelog::g_log_colosseum.buff_printf("]");
		gamelog::g_log_colosseum.commit_buff(LL_INFO);
#endif
	
}

void ColosseumManager::IssueCoinPool()
{
	int role_num = m_pool_user_num;
	if (role_num <= 0) return;		// 为防止除 0，所以宝库人数要大于 0 才发放奖励；

	const ColosseumOtherCfg & colo_other = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
	int pool_coin = (role_num <= colo_other.min_pool_num ? colo_other.bottom_pool_coin : colo_other.reward_pool_coin);
	int coin_get = pool_coin / role_num;		// 向下取整

	int colosseum_pool_credits = (role_num <= colo_other.min_pool_num ? colo_other.min_colosseum_pool_credits : colo_other.max_colosseum_pool_credits);
	int avg_colosseum_credits = colosseum_pool_credits / role_num;		// 向下取整

	if (coin_get <= 0 && avg_colosseum_credits <= 0)
	{
		return;
	}

	static MailContentParam mail; mail.Reset();
	mail.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = coin_get;
	mail.virtual_item_list.virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS] = avg_colosseum_credits;

	int length1 = SNPRINTF(mail.subject, sizeof(mail.subject), gamestring::g_colosseum_subject);
	int length2 = SNPRINTF(mail.contenttxt, sizeof(mail.contenttxt), gamestring::g_colosseum_coin_reward);
	if (length1 <= 0 || length2 <= 0) return;

	for (int i = 0; i < m_pool_user_num; ++i)
	{
		if (NULL == m_pool_user_list[i]) continue;

		MailRoute::MailToUser(m_pool_user_list[i]->GetUserID(), MAIL_REASON_DEFAULT, mail);
	}
}

void ColosseumManager::AddPlayerInPool()
{
	return;//去掉宝库的概念，前面5名的玩家不再进入宝库


	if (m_pool_user_num >= CHALLENGE_FIELD_POOL_PLAYER_MAX_NUM) return;		// 如果宝库人数达到上限，返回

	int original_size = m_pool_user_num;
	for (int i = 0; i < m_rank_user_num && i < 5; ++i) // 入库
	{
		if (NULL == m_rank_list[i])
		{
			this->AddARobotData(i);
		}
		else
		{
			Gladiator* gladiator = m_rank_list[i];
			if (!gladiator->IsRobot())
			{
				Gladiator* clone_gladiator = gladiator->Clone(); // 克隆玩家的数据并制作为机器人
				clone_gladiator->m_uid = this->GenerateValidRobotId();
				NameConfig::Instance()->GetRandomName(rand() % SEX_MAX, clone_gladiator->m_name);
				F_STRNCPY(clone_gladiator->m_battle_data.role_character_data.name, clone_gladiator->m_name, sizeof(GameName));
				this->AddPoolUser(gladiator);
				this->SetRankUser(i, clone_gladiator);
			}
		}
	}

	int after_size = m_pool_user_num;
	if (original_size != after_size && original_size < COLOSSEUM_IN_POOL_RANK_LIST_NUM) this->SendInPoolRankList(REFRESH_TYPE_PUSH, original_size);

}

void ColosseumManager::SendFightResultNotice(Role * role, int old_rank, int new_rank, bool is_win, int add_score, bool is_sweep)
{
	if (NULL == role) return;

	Protocol::SCColosseumPvPsResultInfo cpri;
	cpri.up_rank = is_win ? old_rank - new_rank : 0;
	cpri.new_rank = (!is_win || new_rank >= old_rank) ? old_rank : new_rank;// 如果没赢，或者排名比旧排名大，则名次不变；
	cpri.add_score = add_score;
	cpri.score = role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_COLOSSEUM);
	cpri.is_sweep = is_sweep ? 1 : 0;
	role->GetRoleModuleManager()->NetSend((const void *)&cpri, sizeof(cpri));
}

void ColosseumManager::SendOpenStatusNotice(Role * role)
{
	Protocol::SCColosseumOpenStatusInfo cosi;
	cosi.open_status = 1;		// 改为永久开启
	cosi.next_switch_timestamp = (long long)m_next_switch_time;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&cosi, sizeof(cosi));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&cosi, sizeof(cosi));
	}
}

void ColosseumManager::InitRobot()
{
	this->AddRobotData(INIT_ROBOT_NUM);
}

void ColosseumManager::StopAllColosseumBattle()
{
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_SINGLE_ARENA);
}

void ColosseumManager::OnSeasonSettle()
{
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		Gladiator * user = m_rank_list[i];
		user->OnSeasonChange();

		if (!user->IsRobot())
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user->GetUserID());
			if (NULL != role)
			{
				this->SendRankRewardInfo(role);
			}
		}
	}

	for (int i = 0; i < m_pool_user_num; ++i)
	{
		if (NULL == m_pool_user_list[i]) continue;

		Gladiator * user = m_pool_user_list[i];
		user->OnSeasonChange();

		if (!user->IsRobot())
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user->GetUserID());
			if (NULL != role)
			{
				this->SendRankRewardInfo(role);
			}
		}
	}

	std::vector<Gladiator*> role_rank_vec;
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		if (m_rank_list[i]->IsRobot())
		{
			delete m_rank_list[i];
			this->SetRankUser(i, NULL);
			continue;
		}

		// 把玩家的gladiator暂时转移
		role_rank_vec.push_back(m_rank_list[i]);
		this->SetRankUser(i, NULL);
	}

	m_rank_user_num = 0;

	// 1. 加300机器人
	this->AddRobotData(INIT_ROBOT_NUM);

	// 2. 把宝库里的玩家加进来
	for (int i = 0; i < m_pool_user_num; ++i)
	{
		if (NULL == m_pool_user_list[i]) continue;

		UserID user_id = m_pool_user_list[i]->GetUserID();

		this->AddRankUser(m_pool_user_list[i]); // 转移到RankList， 这里就不用delete了
		this->SetPoolUser(i, NULL);

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		if (NULL != role)
		{
			this->SendRoleInfo(role);
		}
	}
	m_pool_user_num = 0; // 宝库已清空

						 // 3. 把原有排行榜里的加进来
	for (size_t i = 0; i < role_rank_vec.size(); ++i)
	{
		this->AddRankUser(role_rank_vec[i]);

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_rank_vec[i]->GetUserID());
		if (NULL != role)
		{
			this->SendRoleInfo(role);
		}
	}

	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i] || m_rank_list[i]->IsRobot()) continue;

		this->RefreshTarget(m_rank_list[i]);
	}

	m_is_change = true;
	m_next_save_time = EngineAdapter::Instance().Time(); // 立即保存
}

void ColosseumManager::AddRobotData(int add_num)
{
	for (int num = 0; num < add_num; ++num)
	{
		this->AddARobotData();
	}
}

void ColosseumManager::AddARobotData(int insert_idx /* = -1 */)
{
	Gladiator * user = new Gladiator();
	int robot_uid = this->GenerateValidRobotId();
	if (0 == robot_uid)
	{
		delete user; user = NULL;
		return;
	}

	int rank = insert_idx + 1;
	if (insert_idx < 0)
	{
		rank = m_rank_user_num + 1;
	}

	this->FillRobotData(user, rank, robot_uid);

	if (insert_idx >= 0)
	{
		this->SetRankUser(insert_idx, user);
	}
	else
	{
		this->SetRankUser(m_rank_user_num, user);
	}

	m_is_change = true;
}

void ColosseumManager::FillRobotData(Gladiator* gladiator, int rank, int robot_id)
{
	if (NULL == gladiator) return;

	int open_server_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	int robot_level = LOGIC_CONFIG->GetColosseumConfig()->RandRobotLevelByRank(open_server_days, rank);

	gladiator->m_uid = robot_id;

	RoleBattleData & battle_data = gladiator->m_battle_data;
	battle_data.is_auto = 1;
	battle_data.role_stand_row = BATTLE_ROW_TYPE_BACK;

	const RobotRoleCfg* robot_role = ROBOTCONFIG->RandRobotByLevel(robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_role)
	{
		return;
	}

	gladiator->m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	gladiator->m_avatar_type = AppearanceConfig::Instance().RandAvatarByProfession(gladiator->m_profession);

	GameName robot_name;
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[gladiator->m_avatar_type], robot_name, false);
	F_STRNCPY(gladiator->m_name, robot_name, sizeof(GameName));
	gladiator->m_level = robot_role->level;
	Gladiator::AssignShowLevel(gladiator, rank);

	static Appearance appearance;
	appearance.Reset();
	appearance.advance_times = robot_role->advance_times;
	appearance.avatar_type = gladiator->m_avatar_type;
	appearance.headshot_id = 0;
	appearance.color = rand() % INIT_APPREARANCE_COLOR_NUM;
	appearance.prof_base = robot_role->base_prof;
	appearance.weapon_id = robot_role->weapon_id;
	appearance.sub_weapon_id = 0;

	gladiator->SetAppearance(appearance);

	battle_data.appearance = appearance;

	BattleCharacterData & role_data = battle_data.role_character_data;
	role_data.character_id = gladiator->m_uid;
	role_data.character_type = BATTLE_CHARACTER_TYPE_ROLE;
	role_data.level = gladiator->m_show_level;
	F_STRNCPY(role_data.name, robot_name, sizeof(GameName));
	role_data.profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	role_data.weapon_type = robot_role->weapon_type;

	const RobotAttrCon & robot_attr = robot_role->attr_con;
	role_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_attr.max_hp;
	role_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_attr.max_mp;
	memcpy(role_data.attr_list, robot_attr.basic_attr_list, sizeof(robot_attr.basic_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_attr.amend_attr_list, sizeof(robot_attr.amend_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_attr.resis_attr_list, sizeof(robot_attr.resis_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ELEMENT_BEGIN], robot_attr.element_list, sizeof(robot_attr.element_list));

	//const RobotOtherCfg& other_cfg = ROBOTCONFIG->GetOtherCfg();
	//if (other_cfg.jjc_robot_weaken_attr_percent > 0)
	//{
	//	role_data.attr_list[BATTLE_ATTR_ATTACK] = static_cast<BattleAttr>(1.0 * role_data.attr_list[BATTLE_ATTR_ATTACK] * (1.0 - 0.01 * other_cfg.jjc_robot_weaken_attr_percent));
	//	role_data.attr_list[BATTLE_ATTR_ATTACK] = GetMax(1, role_data.attr_list[BATTLE_ATTR_ATTACK]);
	//
	//	role_data.attr_list[BATTLE_ATTR_MENTAL] = static_cast<BattleAttr>(1.0 * role_data.attr_list[BATTLE_ATTR_MENTAL] * (1.0 - 0.01 * other_cfg.jjc_robot_weaken_attr_percent));
	//	role_data.attr_list[BATTLE_ATTR_MENTAL] = GetMax(1, role_data.attr_list[BATTLE_ATTR_MENTAL]);
	//}

	for (int i = 0; i < ROBOT_ROLE_SKILL_MAX && role_data.skill_num < MAX_BATTLE_SKILL_NUM; ++i)
	{
		const RobotSkillCon & robot_skill = robot_attr.skill_list[i];
		if (NULL == SkillPool::Instance().GetSkillCfg(robot_skill.skill_id) || robot_skill.skill_lv <= 0) continue;

		BattleCharacterData::SkillItem & skill = role_data.skill_list[role_data.skill_num++];
		skill.skill_id = robot_skill.skill_id;
		skill.skill_level = robot_skill.skill_lv;
	}

	// 宠物数据
	const RobotPetCfg* robot_pet = ROBOTCONFIG->RandRobotPetByLevel(robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_pet) return;

	battle_data.pet_num = 1;
	BattleCharacterData & pet_data = battle_data.pet_character_data[0].battle_data;
	pet_data.character_id = robot_pet->pet_id;
	pet_data.character_type = BATTLE_CHARACTER_TYPE_PET;
	pet_data.level = gladiator->m_show_level;
	pet_data.param = robot_pet->attr_con.strengthen_level;
	F_STRNCPY(pet_data.name, robot_pet->name, sizeof(GameName));
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(robot_pet->pet_id);
	if (NULL != pet_cfg)
	{
		pet_data.race = pet_cfg->race;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_WATER] = pet_cfg->element_water;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = pet_cfg->element_fire;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_WIND] = pet_cfg->element_wind;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = pet_cfg->element_land;
	}

	// 属性用robot_pet_attr的
	pet_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_pet->attr_con.max_mp;
	pet_data.attr_list[BATTLE_ATTR_CUR_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_CUR_MP] = robot_pet->attr_con.max_mp;
	memcpy(pet_data.attr_list, robot_pet->attr_con.basic_attr_list, sizeof(robot_pet->attr_con.basic_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_pet->attr_con.amend_attr_list, sizeof(robot_pet->attr_con.amend_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_pet->attr_con.resis_attr_list, sizeof(robot_pet->attr_con.resis_attr_list));

	//if (other_cfg.jjc_robot_weaken_attr_percent > 0)
	//{
	//	pet_data.attr_list[BATTLE_ATTR_ATTACK] = static_cast<BattleAttr>(1.0 * pet_data.attr_list[BATTLE_ATTR_ATTACK] * (1.0 - 0.01 * other_cfg.jjc_robot_weaken_attr_percent));
	//	pet_data.attr_list[BATTLE_ATTR_ATTACK] = GetMax(1, pet_data.attr_list[BATTLE_ATTR_ATTACK]);
	//
	//	pet_data.attr_list[BATTLE_ATTR_MENTAL] = static_cast<BattleAttr>(1.0 * pet_data.attr_list[BATTLE_ATTR_MENTAL] * (1.0 - 0.01 * other_cfg.jjc_robot_weaken_attr_percent));
	//	pet_data.attr_list[BATTLE_ATTR_MENTAL] = GetMax(1, pet_data.attr_list[BATTLE_ATTR_MENTAL]);
	//}

	pet_data.skill_num = 0; // 宠物技能在战斗服加载
							// 技能用robot_pet的
							//int skill_num = ARRAY_ITEM_COUNT(robot_pet->attr_con.skill_list);
							//for (int i = 0; i < skill_num; ++i)
							//{
							//	if (robot_pet->attr_con.skill_list[i].skill_id > 0 && robot_pet->attr_con.skill_list[i].skill_lv > 0)
							//	{
							//		pet_data.skill_list[pet_data.skill_num].skill_id = robot_pet->attr_con.skill_list[i].skill_id;
							//		pet_data.skill_list[pet_data.skill_num].skill_level = robot_pet->attr_con.skill_list[i].skill_lv;
							//		++pet_data.skill_num;
							//	}
							//}

	int partner_list[ROW_CHARACTER_NUM - 1] = { 0 };
	int partner_num = 0;
	battle_data.partner_num = 0;
	ROBOTCONFIG->RandRobotPartnerList(robot_level, ROW_CHARACTER_NUM - 1, &partner_num, partner_list);
	for (int i = 0; i < partner_num; ++i)
	{
		int partner_id = partner_list[i];
		const RobotPartnerCfg* robot_partner = ROBOTCONFIG->GetPartner(partner_id, robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
		if (NULL == robot_partner) continue;

		const PartnerBasicCfg* partner_basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
		if (NULL == partner_basic_cfg) continue;

		BattleCharacterData& bcd = battle_data.partner_character_data[battle_data.partner_num++].battle_data;
		bcd.Reset();
		bcd.level = gladiator->m_level - (robot_level - robot_partner->level);
		bcd.character_id = partner_id;
		bcd.profession = partner_basic_cfg->profession;
		bcd.character_type = BATTLE_CHARACTER_TYPE_PARTNER;
		F_STRNCPY(bcd.name, robot_partner->name, sizeof(bcd.name));
		bcd.race = partner_basic_cfg->race_type;
		bcd.is_long_range_unit = static_cast<char>(partner_basic_cfg->is_long_range_unit);

		CLEAR_PARTNER_CRYSTAL(bcd.attr_list);
		PARTNER_CRYSTAL_PUT(partner_basic_cfg->crystal_item, bcd.attr_list);

		// 属性用robot_pet_attr的
		bcd.attr_list[BATTLE_ATTR_MAX_HP] = robot_partner->attr_con.max_hp;
		bcd.attr_list[BATTLE_ATTR_MAX_MP] = robot_partner->attr_con.max_mp;
		bcd.attr_list[BATTLE_ATTR_CUR_HP] = robot_partner->attr_con.max_hp;
		bcd.attr_list[BATTLE_ATTR_CUR_MP] = robot_partner->attr_con.max_mp;
		memcpy(bcd.attr_list, robot_partner->attr_con.basic_attr_list, sizeof(robot_partner->attr_con.basic_attr_list));
		memcpy(&bcd.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_partner->attr_con.amend_attr_list, sizeof(robot_partner->attr_con.amend_attr_list));
		memcpy(&bcd.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_partner->attr_con.resis_attr_list, sizeof(robot_partner->attr_con.resis_attr_list));

		// 技能用robot_pet的
		int skill_num = ARRAY_ITEM_COUNT(robot_partner->attr_con.skill_list);
		bcd.skill_num = 0;
		for (int skill_idx = 0; skill_idx < skill_num; ++skill_idx)
		{
			if (robot_partner->attr_con.skill_list[skill_idx].skill_id > 0 && robot_partner->attr_con.skill_list[skill_idx].skill_lv > 0)
			{
				bcd.skill_list[bcd.skill_num].skill_id = robot_partner->attr_con.skill_list[skill_idx].skill_id;
				bcd.skill_list[bcd.skill_num].skill_level = robot_partner->attr_con.skill_list[skill_idx].skill_lv;
				++bcd.skill_num;
			}
		}
	}
}

void ColosseumManager::SendInPoolRankList(int reason, int from_rank, Role * role)
{
	if (reason < 0 || reason >= REFRESH_TYPE_MAX) return;
	if (NULL != role) from_rank = 0;	// 如果 role 不为 NULL，强制此参数为 0；

	Protocol::SCColosseumInPoolListInfo cipli;
	cipli.count = 0;
	cipli.reason = reason;

	int list_size = GetMin(m_pool_user_num, COLOSSEUM_IN_POOL_RANK_LIST_NUM);
	for (int idx = from_rank; idx < list_size && cipli.count < COLOSSEUM_IN_POOL_RANK_LIST_NUM; ++idx)
	{
		if (NULL == m_pool_user_list[idx]) continue;

		UserCacheNode * user_cache = UserCacheManager::Instance().GetUserNode(m_pool_user_list[idx]->GetUid());
		if (NULL == user_cache) continue;

		Protocol::SCColosseumInPoolListInfo::Top & node = cipli.top_list[cipli.count++];

		node.level = user_cache->level;
		node.profession = user_cache->profession;
		user_cache->GetName(node.name);
		node.top_level = user_cache->top_level;
	}

	if (REFRESH_TYPE_INIT != reason && 0 == cipli.count) return;	// 空数据就没必要下发了；

	int len = sizeof(cipli) - (COLOSSEUM_IN_POOL_RANK_LIST_NUM - cipli.count) * sizeof(cipli.top_list[0]);
	if (len <= 0) return;

	if (NULL == role)
	{
		//cipli.reason = Protocol::SCColosseumInPoolListInfo::REFRESH_TYPE_PUSH;
		World::GetInstWorld()->SendToAllGateway((const char *)&cipli, len);
	}
	else
	{
		cipli.reason = REFRESH_TYPE_INIT;	// 发给个人的必须为初始化
		role->GetRoleModuleManager()->NetSend((const void *)&cipli, len);
	}
}

void ColosseumManager::ClearCurPoolList()
{
	for (int i = 0; i < m_pool_user_num; ++i)
	{
		if (NULL != m_pool_user_list[i])
		{
			delete m_pool_user_list[i];
			m_pool_user_list[i] = NULL;

			this->SetPoolListDirty(i);
		}
	}

	m_pool_user_num = 0;
}

int ColosseumManager::GenerateValidRobotId()
{
	int count = 0;
	do
	{
		int id = m_robot_id_generator;
		m_robot_id_generator = (m_robot_id_generator + 1) % MAX_ROLE_ID;
		UserID robot_uid(COLOSSEUM_ROBOT_SERVER_ID, id);
		if (m_user_rank_map.find(robot_uid) == m_user_rank_map.end())
		{
			return UidToInt(robot_uid);
		}

		++count;
	} while (count < 1000);

	return 0;
}

bool ColosseumManager::CheckGladiatorUnique()
{
#ifdef _DEBUG

	std::set<int> role_id_set;
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		Gladiator* gladiator = m_rank_list[i];
		if (NULL == gladiator) continue;

		if (role_id_set.find(gladiator->GetUid()) != role_id_set.end())
		{

			assert(0);

			return false;
		}
		role_id_set.insert(gladiator->GetUid());
	}

	for (int i = 0; i < m_pool_user_num; ++i)
	{
		Gladiator* gladiator = m_pool_user_list[i];
		if (NULL == gladiator) continue;

		if (role_id_set.find(gladiator->GetUid()) != role_id_set.end())
		{
			assert(0);

			return false;
		}
		role_id_set.insert(gladiator->GetUid());
	}

#endif
	return true;
}

void ColosseumManager::UpdateRobotLevel()
{
	int server_open_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	gamelog::g_log_colosseum.printf(LL_INFO, "UpdateRobotLevel start open_days[%d]", server_open_days);

	for (int i = 0; i < m_rank_user_num; ++i)
	{
		Gladiator* gladiator = m_rank_list[i];
		if (NULL == gladiator || !gladiator->IsRobot()) continue;

		const ColosseumRobotLevelCfg* level_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRobotLevelCfgByRankAndOpenServerDays(i + 1, server_open_days);
		if (NULL == level_cfg) continue;

		gladiator->m_level = RandomNum(level_cfg->level_min, level_cfg->level_max + 1);
		gladiator->m_show_level = level_cfg->show_level;

		gamelog::g_log_colosseum.printf(LL_INFO, "UpdateRobotLevel robot[%d,%s] rank[%d] level[%d] show_level[%d] open_days[%d] pos+1[%d]",
			gladiator->m_uid, gladiator->m_name, i + 1, gladiator->m_level, gladiator->m_show_level, server_open_days, gladiator->m_pos + 1);
	}
	gamelog::g_log_colosseum.printf(LL_INFO, "UpdateRobotLevel end");

	Protocol::SCColosseumClientRefreshTargetNotice notice;
	World::GetInstWorld()->SendToAllGateway((const char*)&notice, sizeof(notice));
}

void ColosseumManager::AssembleProtocolTargetInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumTargetInfo* info)
{
	if (NULL == gladiator || NULL == info) return;

	info->count = 0;
	for (int i = 0; i < CHALLENGE_FIELD_OPPONENT_MAX && info->count < ARRAY_LENGTH(info->target_list); ++i)
	{
		int target_rank = gladiator->m_target_list[i];
		if (target_rank < 0 || target_rank >= m_rank_user_num) continue;

		Gladiator* target = m_rank_list[target_rank];
		if (NULL == target) continue;

		Protocol::SCColosseumTargetInfo::Target &node = info->target_list[info->count++];

		UNSTD_STATIC_CHECK(120 == MAX_ROLE_LEVEL);
		node.rank = (short)target_rank + 1;
		node.level = (short)target->m_show_level;
		node.appearance = target->GetAppearance();
		for (int k = 0; k < COLOSSEUM_DEFENDER_BUFF_NUM; ++k)
		{
			node.buff_id_list[k] = m_defender_buff_id_list[k];
		}
		F_STRNCPY(node.name, target->m_name, sizeof(GameName));
		node.capability = target->m_total_capability;
		node.top_level = UserCacheManager::Instance().GetUserTopLevel(target->m_uid);

		node.is_can_sweep = 0;
		if (gladiator->m_level >= LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().open_level)
		{
			node.is_can_sweep = (gladiator->m_total_capability * LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().raiding_battle / 100) >= target->m_total_capability ? 1 : 0;
		}
		node.reserve_sh = 0;
	}
}

void ColosseumManager::AssembleProtocolRoleInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumRoleInfo* info)
{
	if (NULL == gladiator || NULL == info) return;

	info->rank = gladiator->GetPos() + 1;
	info->today_fight_count = gladiator->m_today_fight_count;
	info->fetch_exp_flag = gladiator->m_fetch_exp_flag;

	info->player_num_in_pool = m_pool_user_num;
	info->is_out_of_rank_list = (gladiator->IsInPool() ? 1 : 0);

	info->capability = gladiator->m_total_capability;

	const ColosseumOtherCfg & colo_other = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
	int total_coin = info->player_num_in_pool <= colo_other.min_pool_num ? colo_other.bottom_pool_coin : colo_other.reward_pool_coin;
	info->can_get_coin = info->player_num_in_pool > 0 ? total_coin / info->player_num_in_pool : 0;

	int colosseum_pool_credits = (info->player_num_in_pool <= colo_other.min_pool_num ? colo_other.min_colosseum_pool_credits : colo_other.max_colosseum_pool_credits);
	info->get_colosseum_credits = info->player_num_in_pool > 0 ? colosseum_pool_credits / info->player_num_in_pool : 0;

	info->highest_rank = gladiator->m_highest_rank + 1;
}

void ColosseumManager::AssembleProtocolRankRewardInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumRankRewardInfo* info)
{
	if (NULL == gladiator || NULL == info) return;

	info->history_highest_rank = gladiator->m_highest_rank + 1;
	info->rank_reward_fetch_flag = gladiator->m_rank_award_fetch_flag;
}

void ColosseumManager::AssembleProtocolTopInfo(ARG_OUT Protocol::SCColosseumTopInfo* info)
{
	if (NULL == info) return;

	info->count = 0;

	for (int i = 0; i < m_rank_user_num && info->count < CHALLENGE_FIELD_TOP_NUM; ++i) // 前4的信息
	{
		if (NULL == m_rank_list[i]) continue;
		Gladiator* gladiator = m_rank_list[i];

		Protocol::SCColosseumTopInfo::Top &node = info->top_list[info->count++];
		node.level = gladiator->m_show_level;
		node.profession = gladiator->m_profession;
		node.avatar_type = gladiator->m_avatar_type;
		F_STRNCPY(node.name, gladiator->m_name, sizeof(node.name));
		node.worship_count = gladiator->m_worship_count;
		node.top_level = UserCacheManager::Instance().GetUserTopLevel(gladiator->m_uid);
	}
}

void ColosseumManager::AssembleProtocolRankInfo(ARG_OUT Protocol::SCColosseumRankInfo* info, int rank_count)
{
	info->count = 0;
	if (rank_count <= 0 || rank_count > Protocol::COLOSSEUM_RANK_NUM)
	{
		rank_count = Protocol::COLOSSEUM_RANK_NUM;
	}
	for (int i = 0; i < m_rank_user_num && info->count < Protocol::COLOSSEUM_RANK_NUM && info->count < rank_count; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		Gladiator* gladiator = m_rank_list[i];

		Protocol::SCColosseumRankInfo::Top & node = info->top_list[info->count];
		node.level = gladiator->m_show_level;
		node.profession = gladiator->m_profession;
		F_STRNCPY(node.name, gladiator->m_name, sizeof(GameName));
		node.top_level = UserCacheManager::Instance().GetUserTopLevel(gladiator->m_uid);
		node.headshot_id = gladiator->m_appearance.headshot_id;
		node.avatar_type = gladiator->m_avatar_type;
		if (0 >= gladiator->m_total_capability)
		{
			UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(gladiator->m_uid);
			if (NULL != user_cache_node)
			{
				node.capability = user_cache_node->capability;
			}
		}
		else
		{
			node.capability = gladiator->m_total_capability;
		}

		++info->count;
	}
}

void ColosseumManager::AssembleProtocolReportInfo(Gladiator* gladiator, ARG_OUT Protocol::SCColosseumReportInfo* info)
{
	if (NULL == gladiator || NULL == info) return;

	info->count = 0;

	std::list<Report>::const_iterator it = gladiator->m_report.begin();
	for (; it != gladiator->m_report.end(); ++it)
	{
		if (info->count >= Protocol::COLOSSEUM_REPORT_NUM) break;

		Report & node = info->report_list[info->count++];
		node.is_win = it->is_win;
		node.level = it->level;
		node.profession = it->profession;
		node.rank = it->rank + 1;
		memcpy(node.name, it->name, sizeof(GameName));
		node.avatar_type = it->avatar_type;
		node.is_attacker = it->is_attacker;
		node.headshot_id = it->headshot_id;
		node.reserve_sh = it->reserve_sh;
	}
}

void ColosseumManager::CalculateFightStat(ARG_OUT ColosseumFightStatistic& stat, BattleData_StatisticRole* role_statistic_data, BattleData_Statistic* statistic_data, int pet_stat_num, BattleData_StatisticRole* pet_stat_list)
{
	if (role_statistic_data != NULL && pet_stat_num > 0 && pet_stat_list != NULL && statistic_data != NULL)
	{
		stat.total_damage_all = role_statistic_data->total_damage_to_enemy;
		for (int pet_idx = 0; pet_idx < pet_stat_num && pet_idx < ROLE_BATTLE_PET_NUM; pet_idx++)
		{
			stat.total_damage_all += pet_stat_list[pet_idx].total_damage_to_enemy;
		}
		stat.total_damage_all += statistic_data->total_attacker_partner_damage_to_enemy;

		for (int partner_idx = 0; partner_idx < statistic_data->attacker_partner_num && partner_idx < BATTLE_MAX_ROLE_NUM - 1; partner_idx++)
		{
			int partner_id = statistic_data->attacker_parner_id_list[partner_idx];
			const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
			if (partner_cfg != NULL)
			{
				switch (partner_cfg->profession)
				{
				case PARTNER_PROF_PUGILIST:
					stat.has_boxer_fight = 1;
					break;
				case PARTNER_PROF_SWORDMAN:
					stat.has_swordsman_fight = 1;
					break;
				case PARTNER_PROF_ROMAHAWK:
					stat.has_berserk_fight = 1;
					break;
				case PARTNER_PROF_HORSEMAN:
					stat.has_tiewei_fight = 1;
					break;
				case PARTNER_PROF_MARKSMAN:
					stat.has_arrower_fight = 1;
					break;
				case PARTNER_PROF_MAGICIAN:
					stat.has_start_fight = 1;
					break;
				case PARTNER_PROF_MINISTER:
					stat.has_doctor_fight = 1;
					break;
				case PARTNER_PROF_SORCERER:
					stat.has_lich_fight = 1;
					break;
				default:
					break;
				}

				if (partner_cfg->sex == FEMALE) stat.has_woman_fight = 1;
				else if (partner_cfg->sex == MALE) stat.has_man_fight = 1;
			}
		}
	}

	if (NULL != statistic_data)
	{
		stat.round_num = statistic_data->round_num;
		stat.use_medicine_times = statistic_data->attacker_use_medicine_times;
		stat.realive_times = statistic_data->attacker_realive_times;
	}
}

void ColosseumManager::OnRandActivityEnd()
{
	const RandActivitySportsConfig * config = static_cast<const RandActivitySportsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SPORTS));
	if (NULL == config)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())return;

	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		Gladiator * user = m_rank_list[i];
		int pos = user->GetPos();
		if (!user->IsRobot())
		{
			//活动结束没领奖励的立刻发放
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user->GetUserID());
			if (NULL == role)
			{
				OfflineRegisterManager::Instance().OnRandActivityEnd(user->GetUserID(), RAND_ACTIVITY_TYPE_SPORTS);
			}
			else
			{
				RoleActivitySports * role_activity = static_cast<RoleActivitySports *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPORTS));
				if (NULL != role_activity)
				{
					role_activity->OnRaSportsHistroyEnd();
				}
			}
			//活动结算奖励

			const RASportsSettlementCfg *cfg = config->GetSportsSettlementCfg(pos);
			if (cfg == NULL)continue;
			static MailContentParam contentparam; contentparam.Reset();
			if (cfg->itemlist.size() > 0)
			{
				int real_item = 0;
				for (std::vector<ItemConfigData>::const_iterator it = cfg->itemlist.begin(); it != cfg->itemlist.end(); it++)
				{
					contentparam.item_list[real_item].item_id = it->item_id;
					contentparam.item_list[real_item].num = it->num;
					contentparam.item_list[real_item].is_bind = (it->is_bind) ? 1 : 0;
					const ItemBase *item_base = ITEMPOOL->GetItem(it->item_id);
					if (item_base != NULL)contentparam.item_list[real_item].invalid_time = item_base->CalInvalidTime();
					real_item++;
				}

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_sports_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_sports_settlement_content, RAND_ACTIVITY_TYPE_SPORTS, pos + 1);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(user->GetUserID(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}
	}
}

void ColosseumManager::OnSweepReq(Role * role, int target_index)
{
	if (NULL == role) return;

	if (role->GetLevel() < LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().open_level)
	{
		role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameQueryTargetGladiatorBeforeFightReq req;
		req.role_id = role->GetUID();
		req.target_index = target_index;
		req.is_sweep = 1;
		InternalComm::Instance().SendToGameThroughCross(role->GetUniqueServerID().plat_type, role->GetUniqueServerID().server_id, (const char*)&req, sizeof(req));

		return;
	}

	Protocol::SCSweepResultNotice msg;
	msg.is_sweep_success = 0;

	do
	{
		int pos = 0;
		bool is_in_pool = false;
		Gladiator* gladiator = this->GetUserGladiator(role->GetUserId(), &pos, &is_in_pool);

		if (NULL == gladiator)
		{
			role->NoticeNum(errornum::EN_COLOSSEUM_NOT_IN_RANK_LIST);
			break;
		}

		if (is_in_pool)
		{
			role->NoticeNum(errornum::EN_COLOSSEUM_CAN_NOT_FIGHT);
			break;
		}

		const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();

		// 判断次数
		if (gladiator->m_today_fight_count >= other_cfg.challenge_times)
		{
			role->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
			break;
		}

		int target_rank = gladiator->GetTargetRank(target_index);
		if (target_rank < 0 || target_rank >= (int)m_rank_user_num || NULL == m_rank_list[target_rank])
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			break;
		}

		// 获取对手的竞技场数据 (需要做属性调整)
		Gladiator * target = m_rank_list[target_rank];

		uint64_t temp_total_capability = (uint64_t)gladiator->m_total_capability;
		bool is_can_sweep = (temp_total_capability * LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().raiding_battle / 100) >= target->m_total_capability ? true : false;
		
		if (!is_can_sweep)	// 战力不够扫荡, 刷新对手并返回
		{
			this->OnRefreshTarget(role->GetUID());
			break;
		}
		else
		{
			this->SysReport(role, target->m_uid, true, NULL, NULL, NULL, 0, NULL, true);
			EventHandler::Instance().OnRoleChallengeColosseum(role);
			msg.is_sweep_success = 1;

			WorldStatus::Instance().LogFunctionStats(role, "ColosseumManager");
		}
	} while (0);

	role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
}

void ColosseumManager::OnGameHiddenQueryTargetGladiatorBeforeSweepResp(int role_id, int target_index, int target_uid, int user_total_capability, int target_total_capability)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role) return;

	Protocol::SCSweepResultNotice msg;
	msg.is_sweep_success = 0;

	do
	{
		uint64_t temp_total_capability = (uint64_t)user_total_capability;
		bool is_can_sweep = (temp_total_capability * LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().raiding_battle / 100) >= target_total_capability ? true : false;

		if (!is_can_sweep)	// 战力不够扫荡, 刷新对手并返回
		{
			crossgameprotocal::HiddenGameRoleColosseumReqRouter router;
			router.uid = role->GetUID();
			router.req_type = Protocol::CSColosseumReq::REQ_TYPE_REFRESH_TARGET;
			router.param = 0;

			int plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
			int server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
			InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&router, sizeof(router));

			gamelog::g_log_colosseum.printf(LL_INFO, "%s sweep_failed... role[%d, %s] target_index[%d] target_uid[%d] user_total_capability[%d] target_total_capability[%d]", __FUNCTION__,
				role->GetUID(), role->GetName(), target_index, target_uid, user_total_capability, target_total_capability);

			break;
		}
		else
		{
			this->SysReport(role, target_uid, true, NULL, NULL, NULL, 0, NULL, true);
			EventHandler::Instance().OnRoleChallengeColosseum(role);
			msg.is_sweep_success = 1;

			WorldStatus::Instance().LogFunctionStats(role, "ColosseumManager");
		}
	} while (0);

	role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
}

void ColosseumManager::SendRoleInfoToHidden(int uid)
{
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid));
	if (NULL == gladiator) return;

	static Protocol::SCColosseumRoleInfo info;
	this->AssembleProtocolRoleInfo(gladiator, &info);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, sizeof(info));

	this->SendRankRewardInfoToHidden(uid);
}

void ColosseumManager::SendRankRewardInfoToHidden(int uid)
{
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid));
	if (NULL == gladiator) return;

	Protocol::SCColosseumRankRewardInfo crri;
	this->AssembleProtocolRankRewardInfo(gladiator, &crri);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&crri, sizeof(crri));
}

void ColosseumManager::SendTargetInfoToHidden(int uid)
{
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid));
	if (NULL == gladiator) return;

	static Protocol::SCColosseumTargetInfo info;
	info.count = 0;
	this->AssembleProtocolTargetInfo(gladiator, &info);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, sizeof(info));
}

void ColosseumManager::SendTopInfoToHidden(int uid)
{
	static Protocol::SCColosseumTopInfo info;
	info.count = 0;

	this->AssembleProtocolTopInfo(&info);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, sizeof(info));
}

void ColosseumManager::SendRankInfoToHidden(int uid, int rank_count)
{
	static Protocol::SCColosseumRankInfo info;
	info.count = 0;

	this->AssembleProtocolRankInfo(&info, rank_count);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, sizeof(info));
}

void ColosseumManager::SendReportInfoToHidden(int uid)
{
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid));
	if (NULL == gladiator) return;

	static Protocol::SCColosseumReportInfo info;
	info.count = 0;

	this->AssembleProtocolReportInfo(gladiator, &info);

	int len = sizeof(info) - sizeof(info.report_list) + info.count * sizeof(Report);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, len);
}

void ColosseumManager::SendRankTopSimpleInfoToHidden(int uid)
{
	Gladiator * gladiator = NULL;
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;

		gladiator = m_rank_list[i];
		break;
	}
	if (NULL == gladiator) return;

	if (UserCacheManager::Instance().SendRoleSimpleInfoToHidden(uid, gladiator->GetUid()))
	{
		return;
	}

	Protocol::SCQueryRoleSimpleInfo info;
	info.role_id = gladiator->GetUid();
	F_STRNCPY(info.role_name, gladiator->GetName(), sizeof(info.role_name));
	info.level = gladiator->m_show_level;
	info.top_level = 0;
	info.capability = gladiator->m_total_capability;
	info.appearance_info = gladiator->m_appearance;
	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, sizeof(info));
}

void ColosseumManager::RoleRefreshTargetOnHidden(int uid)
{
	int pos = 0;
	bool is_in_pool = false;
	Gladiator* gladiator = this->GetUserGladiator(IntToUid(uid), &pos, &is_in_pool);
	if (NULL == gladiator || is_in_pool) return;

	time_t now_sec = EngineAdapter::Instance().Time();
	int client_refresh_interval_s = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg().client_refresh_interval_s;
	if (now_sec < gladiator->last_refresh_target_time + client_refresh_interval_s)
	{
		Protocol::SCNoticeNum err;
		err.notice_num = errornum::EN_YOUR_CLICK_IS_TOO_OFTEN;

		InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&err, sizeof(err));

		return;
	}

	gladiator->last_refresh_target_time = now_sec;

	if (!is_in_pool)
	{
		this->SetRankListDirty(pos);
	}
	else
	{
		this->SetPoolListDirty(pos);
	}

	this->RefreshTarget(gladiator, 0, true);
}

void ColosseumManager::OnRoleChangeFormation(Role* role)
{
	if (NULL == role) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
		if (NULL == gladiator) return;

		gladiator->SyncRoleData(role);

		this->SendRoleInfo(role);
	}
	else
	{
		crossgameprotocal::HiddenGameSyncRoleBattleData data;
		data.role_id = role->GetUID();
		data.business_type = crossgameprotocal::HiddenGameSyncRoleBattleData::BUSINESS_TYPE_COLOSSEUM;

		if (!role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(BATTLE_MODE_SINGLE_ARENA, data.role_battle_data, false))
		{
			role->GetRoleModuleManager()->CollectRoleBattleData(&data.role_battle_data, true, true);
		}

		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&data, role->GetUserId().db_index);
	}
}

std::set<int> ColosseumManager::GetAllUserUidSet() const
{
	std::set<int> all_user_uid_set;
	for (int i = 0; i < m_rank_user_num; ++i)
	{
		if (NULL == m_rank_list[i]) continue;
		if (m_rank_list[i]->IsRobot()) continue;

		all_user_uid_set.insert(m_rank_list[i]->GetUid());
	}

	return all_user_uid_set;
}

void ColosseumManager::OnUserLogin(Role * role)
{
	if (NULL == role) return;
	if (CrossConfig::Instance().IsHiddenServer()) return;

	if (this->IsLevelLimit(role->GetLevel())) 
	{
		if (2 != role->GetRoleModuleManager()->GetCommonData().colosseum_set_flag)
		{
			role->GetRoleModuleManager()->GetCommonData().colosseum_set_flag = 2;
		}
		return;
	}

	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
	if (NULL == gladiator)
	{
		this->SyncUserData(role, &gladiator);
		if (NULL != gladiator)
		{
			gladiator->UserLogin(role);
		}
	}
	else
	{
		this->SyncUserData(role);
		gladiator->UserLogin(role);
	}

	if (gladiator != NULL)gladiator->SendColosseumInfo(role);
	
	if (2 != role->GetRoleModuleManager()->GetCommonData().colosseum_set_flag)
	{
		if (NULL != gladiator)
		{
			int old_history_rank = gladiator->m_highest_rank;
			gladiator->m_highest_rank = gladiator->GetPos();
			role->GetRoleModuleManager()->GetCommonData().colosseum_set_flag = 2;
			gamelog::g_log_colosseum.printf(LL_INFO, "%s gladiator[%d,%s] old_history_pos[%d] history_pos[%d]", __FUNCTION__, gladiator->GetUid(), gladiator->GetName(), old_history_rank, gladiator->m_highest_rank);
		}
	}
	this->SendRoleAchievement(role);
	this->SendRoleInfo(role);
	this->SendOpenStatusNotice(role);
	this->SendInPoolRankList(REFRESH_TYPE_INIT, 0, role);
}

void ColosseumManager::OnUserLogout(Role* role)
{
	// 每次登出时，应把玩家最新数据更新到竞技场数据中
	if (NULL == role) return;
	if (this->IsLevelLimit(role->GetLevel())) return;

	Gladiator* gladiator = this->GetUserGladiator(role->GetUserId());
	if (NULL != gladiator)
	{
		this->SyncUserData(role);
	}
}

void ColosseumManager::OnLevelUp(Role * role, int to_level)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_COLOSSEUM);

	if (NULL != cfg && cfg->limit_level == to_level)
	{
		if (m_rank_user_num < 5000) // 避免数据量太大
		{
			this->AddARobotData();
		}

		int target = m_rank_user_num - 1;
		this->SyncUserData(role);
		this->OnRefreshTarget(role->GetUID(), target);
		this->SendOpenStatusNotice(role);
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COLOSSEUM, this->GetRoleRank(role->GetUID()) + 1);
		}
	}
	else if (NULL != cfg && cfg->limit_level < to_level)
	{
		this->SyncUserData(role);
		this->OnRefreshTarget(role->GetUID());
		this->SendOpenStatusNotice(role);
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			role->GetRoleModuleManager()->GetMysteriousTrial()->OnInfoChange(MSC_CON_COLOSSEUM, this->GetRoleRank(role->GetUID()) + 1);
		}
	}
}

void ColosseumManager::OnUserInfoChange(Role * role)
{
	if (NULL == role) return;
	if (this->IsLevelLimit(role->GetLevel())) return;

	//玩家等级满足,即使此时找不到应该也得加入竞技场
	this->SyncUserData(role);
}

void ColosseumManager::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsLoadFinish())
	{
		return;
	}

	// 跨服不需要ChallengeField， 暂时先把存储数据的CheckSave屏蔽掉
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	// 时间到，将玩家踢进宝库
	if (now_second >= m_next_pool_time)
	{
		this->AddPlayerInPool();
		m_next_pool_time = now_second + EngineAdapter::Instance().NextHourInterval(0, 0);
	}

	//检查成就刷新时间
	unsigned int refresh_tamp = WorldStatus::Instance().GetColosseumRefreshTamp();
	if (refresh_tamp <= 0)
	{
		time_t server_zero_tamp = SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
		const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
		if (other_cfg.refresh_intervar != 0)
		{
			//检查实际刷新次数差
			int real_times_discount = 0;
			time_t now_second = EngineAdapter::Instance().Time();

			time_t discount_second = now_second - server_zero_tamp;//开服了多久

			real_times_discount = (discount_second / (other_cfg.refresh_intervar * PER_DAY_SECOND)) + 1;//计算差距刷新次数
			unsigned int real_refresh_tamp = static_cast<unsigned int>(server_zero_tamp + other_cfg.refresh_intervar * PER_DAY_SECOND * real_times_discount);
			WorldStatus::Instance().SetColosseumRefreshTamp(real_refresh_tamp);
		}
	}

	if (refresh_tamp != 0 && refresh_tamp < now_second)
	{
		const ColosseumOtherCfg & other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();
		unsigned int new_tamp = refresh_tamp + PER_DAY_SECOND * other_cfg.refresh_intervar;
		WorldStatus::Instance().SetColosseumRefreshTamp(new_tamp);
		//标记所有竞技场玩家，清空成就完成标记，和累积次数
		for (int i = 0; i < m_rank_user_num; ++i)
		{
			if (NULL == m_rank_list[i]) continue;

			if (!m_rank_list[i]->IsRobot())
			{
				Gladiator* user = m_rank_list[i];
				user->SetClearCheck();
			}
		}
	}

	this->CheckSave(now_second);
}

bool ColosseumManager::OnServerStart()
{
	return this->LoadUser(0);
}

void ColosseumManager::OnServerStop()
{
	// 跨服不需要ChallengeField， 暂时先把存储数据的CheckSave屏蔽掉
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	m_next_save_time = 0;
	this->CheckSave(EngineAdapter::Instance().Time(), true);
}

void ColosseumManager::OnResetGlobalModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	this->UpdateRobotLevel();

	m_is_change = true;
}