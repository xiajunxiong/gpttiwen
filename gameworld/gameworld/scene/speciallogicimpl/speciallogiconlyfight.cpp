#include "speciallogiconlyfight.hpp"
#include "world.h"

#include "obj/character/role.h"
#include "obj/otherobj/treasurechest.h"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"

#include "gameworld/gamelog.h"
#include "gameworld/internalcomm.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolemodulemanager.hpp"
#include "gameworld/item/knapsack.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"

SpecialLogicOnlyFight::SpecialLogicOnlyFight(Scene * scene)
	:SpecialLogic(scene), m_need_refresh_rank(false), m_next_refresh_rank_timestamp(0), next_box_refresh_tamp(0), m_box_num(0), force_to_next_time(0), 
	is_refresh_box_flag(false), m_is_arena_close(true), m_delay_close_arena_time(0), is_send_fight_special_info(false)
{
}

void SpecialLogicOnlyFight::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->DelayKickOutRole(role);
		return;
	}
	//赛场处于关闭状态
	if (m_is_arena_close)
	{
		role->NoticeNum(errornum::EN_ACTIVITY_ARENA_IS_COLSE);
		this->DelayKickOutRole(role);
		return;
	}

	int old_num = (int)m_player_in_scene.size();

	ActivityOnlyFight * act_ref = (ActivityOnlyFight *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT));
	if (NULL == act_ref || !act_ref->IsActivityOpen())
	{
		this->DelayKickOutRole(role);
		return;
	}
	else 
	{
		m_player_in_scene.insert(role->GetUID());
		if (!act_ref->IsActivityEnterTime())
		{
			this->SendOnlyFightRank(role, true);
		}

		if (old_num <= 1 && (int)(m_player_in_scene.size()) > 1 && m_delay_close_arena_time > 0)
		{
			m_delay_close_arena_time = 0;
		}
		is_send_fight_special_info = true;
	}

	int role_id = role->GetUID();
	std::map<int, OnlyFightPrivateInfo>::iterator find_it = m_private_info_map.find(role_id);
	if (find_it != m_private_info_map.end())
	{
		OnlyFightPrivateInfo & info_con = find_it->second;
		const OnlyFightOtherCfg & of_other = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
		if (info_con.lose_time >= (UInt32)of_other.max_fail_times)
		{
			this->DelayKickOutRole(role);
			return;
		}
	}
	this->SendAdvanceFetchInfo(role);
}

void SpecialLogicOnlyFight::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	std::set<int>::iterator find_it = m_player_in_scene.find(role->GetUID());
	if (m_player_in_scene.end() != find_it)
	{
		m_player_in_scene.erase(find_it);

		if (is_logout && m_player_in_scene.size() <= 1 && BattleManagerLocal::GetInstance().IsRoleInBattle(role->GetUID()))
		{
			// 两人PK，在战斗中掉线一人  等3分钟
			m_delay_close_arena_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 180; 
		}
	}
	is_send_fight_special_info = true;
	this->OnCloseRank(role);
}

void SpecialLogicOnlyFight::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{	
		return;
	}

	ActivityOnlyFight * activity = (ActivityOnlyFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
	if(NULL == activity || !activity->IsActivityOpen()) return;

	// 刷新宝箱
	if (is_refresh_box_flag && now_second >= next_box_refresh_tamp)
	{
		const OnlyFightOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
		if (m_box_num < other_cfg.max_num)
		{
			this->OnRefreshChest();
		}

		next_box_refresh_tamp = now_second + other_cfg.time;
	}
	
	//入场时间不检测
	if(activity->IsActivityEnterTime()) return;

	// 刷新排行榜
	if (m_need_refresh_rank)
	{
		m_need_refresh_rank = false;
		this->RefreshOnlyFightRank();
		this->SendInSceneAllRoleInfo();
		this->SendOpenRankInfoUser();
	}
	if (is_send_fight_special_info)
	{
		is_send_fight_special_info = false;
		this->SendOnlyFightSpecialInfo();
	}

	/*if (m_need_refresh_rank && now_second >= m_next_refresh_rank_timestamp)
	{
		const OnlyFightOtherCfg & gf_other_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
		m_need_refresh_rank = false;
		m_next_refresh_rank_timestamp = now_second + gf_other_cfg.rank_refresh_interval;
		this->RefreshOnlyFightRank();
		this->SendOnlyFightRank();
		this->SendInSceneAllRoleInfo();
	}*/

	// 存活奖励  当少于xx人的时候可以领一份,存活到最后阶段可以领取到全部存活奖励(应要求由达到条件发放改为活动结束)
	if (!m_survival_award_need_check_num.empty() && NULL != m_scene)
	{ 
		//bool is_true = false;
		for (std::set<int>::iterator it = m_survival_award_need_check_num.begin(); it != m_survival_award_need_check_num.end();)
		{
			if (m_scene->RoleNum() <= *it)
			{
				int role_num = m_scene->RoleNum();
				for (int i = 0; i < role_num; i++)
				{
					Role *role = m_scene->GetRoleByIndex(i);
					if(NULL == role) continue;

					m_survival_map[*it].push_back(OnlyFightPraInfo(role->GetLevel(), role->GetUserId()));
				}
				m_survival_award_need_check_num.erase(it++);
				continue;
			//	is_true = this->OnlyFightIssueSurvivalAward(*it);
			}
			++it;
		}
	}

	// 如果场景内的活跃人数不大于1人时，把玩家T出场景,并提示
	if ((int)m_player_in_scene.size() <= 1 && (0u == m_delay_close_arena_time || (now_second > m_delay_close_arena_time))) 
	{
		if (force_to_next_time != 0 && now_second >= force_to_next_time)
		{
			this->DelayKickOutAllRole();
		}
		// 延时3秒	
		if (force_to_next_time == 0)
		{
			force_to_next_time = now_second + 3;
			m_is_arena_close = true;
			if (!m_player_in_scene.empty())
			{
				Role * role = this->GetScene()->GetRoleByUID(*m_player_in_scene.begin());
				if (NULL != role)
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_only_fight_notice);
					if (length > 0)
					{
						Protocol::SCSystemMsg sm;
						int sendlen = 0;
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
						{
							role->GetRoleModuleManager()->NetSend((const void *)&sm, sendlen);
						}
					}
				}
			}
		}
	}
}

bool SpecialLogicOnlyFight::CanChallengeOthers(Role * role, Role * target)
{
	if(NULL == role || NULL == target) return false;

	ActivityOnlyFight * activity = (ActivityOnlyFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
	if (NULL == activity) return false;

	if (!activity->IsActivityOpen())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}

	if (activity->IsActivityEnterTime())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_WAITING_FOR_COUNTDOWN);
		return false;
	}

	std::map<int, time_t>::iterator iter = m_defeat_protect_time.find(target->GetUID());
	if (iter != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= iter->second)
	{
		role->NoticeNum(errornum::EN_TARGET_FIGHT_PROTECT);
		return false;
	}

	std::map<int, time_t>::iterator it = m_defeat_protect_time.find(role->GetUID());
	if (it != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= it->second)
	{
		role->NoticeNum(errornum::EN_MY_FIGHT_PROTECT);
		return false;
	}

	return true;
}

void SpecialLogicOnlyFight::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_ONLY_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_ONLY_FIGHT)) return;

	if(ack->attacker_list.role_count > 1 || ack->defender_list.role_count > 1) return;		//该活动不可组队,PVP单挑. 出现0的情况为对手逃跑

	int tmp_atk_uid = ack->attacker_list.role_list[0].uid;
	int tmp_def_uid = ack->defender_list.role_list[0].uid;

	switch (ack->battle_result)
	{
	case RESULT_TYPE_ATTACKER_WIN:
		this->OnOnlyFightOver(tmp_atk_uid, OF_TYPE_VICT);
		this->OnOnlyFightOver(tmp_def_uid, OF_TYPE_LOSE);
		break;
	case RESULT_TYPE_DEFENDER_WIN:
		this->OnOnlyFightOver(tmp_atk_uid, OF_TYPE_LOSE);
		this->OnOnlyFightOver(tmp_def_uid, OF_TYPE_VICT);
		break;
	case RESULT_TYPE_DRAW:
		this->OnOnlyFightOver(tmp_atk_uid, OF_TYPE_DRAW);
		this->OnOnlyFightOver(tmp_def_uid, OF_TYPE_DRAW);
		break;
	case RESULT_TYPE_FORCED_FINISH:		// 平局或战斗中断，算双方均失败；
	case RESULT_TYPE_EXCEED_MAX_ROUND:
	case RESULT_TYPE_EXCEED_MAX_TIME:
		this->OnOnlyFightOver(tmp_atk_uid, OF_TYPE_LOSE);
		this->OnOnlyFightOver(tmp_def_uid, OF_TYPE_LOSE);
		break;
	}

	m_need_refresh_rank = true;
}

void SpecialLogicOnlyFight::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_ONLY_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_ONLY_FIGHT)) return;

	//逃跑默认失败
	this->OnOnlyFightOver(ack->uid, OF_TYPE_LOSE);
	m_need_refresh_rank = true;
}

void SpecialLogicOnlyFight::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_ONLY_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_ONLY_FIGHT)) return;

	if (ack->attacker_list.role_count > 1 || ack->defender_list.role_count > 1) return;		//该活动不可组队,PVP单挑.

	int tmp_atk_uid = ack->attacker_list.role_list[0].uid;
	int tmp_def_uid = ack->defender_list.role_list[0].uid;

	this->OnOnlyFightOver(tmp_atk_uid, OF_TYPE_LOSE);
	this->OnOnlyFightOver(tmp_def_uid, OF_TYPE_LOSE);
	m_need_refresh_rank = true;
}

bool SpecialLogicOnlyFight::OnOpenTreasureChest(Role * role, TreasureChest * chest_obj)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (NULL == role || NULL == chest_obj) return false;

	// 该活动为单人模式,不允许组队,自己随机一次奖励
	{
		if (!this->RandAndPutChestAwardInBag(role, chest_obj->GetTreasureChestid()))
		{
			return false;
		}
	}

	if (m_box_num > 0)m_box_num -= 1;
	return true;
}

void SpecialLogicOnlyFight::OnActivityStandy()
{
	m_need_refresh_rank = false;
	m_next_refresh_rank_timestamp = 0;
	m_survival_award_need_check_num.clear();
	m_survival_map.clear();
	m_next_chest_refersh_time.clear();
	m_participate_player.clear();
	m_private_info_map.clear();
	m_of_rank_list.clear();
	m_player_in_scene.clear();
	next_box_refresh_tamp = 0;
	force_to_next_time = 0;
	m_is_arena_close = true;
	m_delay_close_arena_time = 0u;
	is_send_fight_special_info = false;
}

void SpecialLogicOnlyFight::OnActivityOpen()
{
	this->InitFightingHall();					// 先初始化后再发送,否则查找不到参与人员数据
}

void SpecialLogicOnlyFight::OnActivityEnterTime()
{
	m_chest_list.clear();
	is_refresh_box_flag = true;
	m_is_arena_close = false;
}

bool SpecialLogicOnlyFight::SceneRoleNumEmpty()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (this->GetScene()->RoleNum() == 0)
	{
		return true;
	}

	return false;
}

void SpecialLogicOnlyFight::RequestOnlyFightRankInfo(Role * role)
{
	if(NULL == role) return;

	this->SendOnlyFightRank(role, true);
	this->OnOpenRank(role);
}

void SpecialLogicOnlyFight::OnActivityEnd()
{
	m_survival_award_need_check_num.clear();	// 清空存活奖检测表

	this->ForceStopOnlyFightBattle();			// 强制中断家族大乱斗的战斗
	this->DelayKickOutAllRole();				// 把玩家都踢出场景
	this->ClearAllChest();						// 清理场景内的宝箱
	this->RefreshOnlyFightRank();				// 最后一次整理排名
	this->SendAllParticipNotice();				// 通知所有参与玩家排行榜结果
	this->OnlyFightIssueActivityAward();		// 发放活动奖励
	this->OnlyFightIssueSurvivalAward();		// 发放存活奖励
	this->OnlyFightIssueParticipAward();		// 发放参与奖励
	m_participate_player.clear();				// 清空参与人员
	m_survival_map.clear();						// 清空存活奖励人员
	is_refresh_box_flag = false;
	m_is_arena_close = true;
}

void SpecialLogicOnlyFight::OnRoleLogin(Role * role)
{
	if(NULL == role) return;

	this->SendOnlyFightSpecialInfo();
}

void SpecialLogicOnlyFight::OnOpenRank(Role * role)
{
	if (NULL == role) return;

	m_scene_open_rank_list.insert(role->GetUID());
}

void SpecialLogicOnlyFight::OnCloseRank(Role * role)
{
	if (NULL == role) return;

	m_scene_open_rank_list.erase(role->GetUID());
}

void SpecialLogicOnlyFight::OnAdavanceFetch(Role * role, short op_type, int param1)
{
	if(NULL == role) return;

	ActivityOnlyFight * activity = (ActivityOnlyFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
	if (NULL == activity) return;

	if (!activity->IsActivityOpen())
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	if (activity->IsActivityEnterTime())
	{
		return;
	}
	std::map<int, OnlyFightUserInfo>::iterator it = m_participate_player.find(role->GetUID());
	if(it == m_participate_player.end()) return;

	switch (op_type)
	{
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD:
		{
			if (0 != it->second.particip_award_flag)
			{
				role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				return;
			}
			
			const OnlyFightParticipationAward * cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightParCfg(it->second.level);
			if (NULL == cfg) return;

			if (!cfg->item_reward.empty())
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->item_reward.size(), &cfg->item_reward[0], PUT_REASON_ONLY_FIGHT))
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&cfg->item_reward[0], (int)cfg->item_reward.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
			}

			role->AddExp(cfg->experience, ADD_EXP_TYPE_ONLY_FIGHT, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin_bind, __FUNCTION__);
			it->second.particip_award_flag = 1;
			this->SendAdvanceFetchInfo(role);

			unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
			if (NULL != role)
			{
				role->GetRoleModuleManager()->GetCommonData().fetch_only_fight_join_reward_flag = 1;
				role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT] = fetch_reward_zero_timestamp;
			}
			else
			{
				crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
				hgslais.role_id = it->first;
				hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT;
				hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index, (const char *)&hgslais, sizeof(hgslais));
			}
		}
		break;
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_SURVIVAL_AWARD:
		{
			std::map<int, std::vector<OnlyFightPraInfo> >::iterator sur_it = m_survival_map.find(param1);
			if(sur_it == m_survival_map.end()) return;

			for (int i = 0; i < (int)sur_it->second.size(); i++)
			{
				if (it->first != UidToInt(sur_it->second[i].user_id))
				{
					continue;
				}
				const OnlyFightSurvAward * award_con = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSurvAward(it->second.level, sur_it->first);
				if (NULL == award_con) continue;

				if (it->second.survival_award_flag & (1 << award_con->seq))
				{
					role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
					return;
				}

				if (!award_con->item_reward.empty())
				{
					if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)award_con->item_reward.size(), &award_con->item_reward[0], PUT_REASON_ONLY_FIGHT))
					{
						role->GetRoleModuleManager()->GetKnapsack()->SendMail(&award_con->item_reward[0], (int)award_con->item_reward.size(), SNED_MAIL_TYPE_DEFAULT, true);
					}
				}
				
				role->AddExp(award_con->experience, ADD_EXP_TYPE_ONLY_FIGHT, __FUNCTION__);
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(award_con->coin_bind, __FUNCTION__);
				it->second.survival_award_flag |= (1 << award_con->seq);
				this->SendAdvanceFetchInfo(role);
			}
		}
		break;
	default:
		break;
	}
}

void SpecialLogicOnlyFight::SendAdvanceFetchInfo(Role * role)
{
	if(NULL == role) return;
	Protocol::SCActivityAdvanceFetchInfo info;
	info.activity_type = ACTIVITY_TYPE_ONLY_FIGHT;
	std::map<int, OnlyFightUserInfo>::iterator it = m_participate_player.find(role->GetUID());
	if (it == m_participate_player.end())
	{
		info.level = role->GetLevel();
		info.particip_award_flag = 0;
		info.survival_award_flag = 0;
	}
	else
	{
		info.level = it->second.level;
		info.particip_award_flag = it->second.particip_award_flag;
		info.survival_award_flag = it->second.survival_award_flag;
	}
	
	info.reserve_sh = 0;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void SpecialLogicOnlyFight::InitFightingHall()
{
	// 初始化存活奖励检测表
	LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetSurvivalCheckNumList(m_survival_award_need_check_num);

	// 记录所有场景内的玩家为参与玩家UID
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;

		m_participate_player[role->GetUID()] = OnlyFightUserInfo(role->GetLevel());

		std::set<int>::iterator find_it = m_player_in_scene.find(role->GetUID());
		if (m_player_in_scene.end() == find_it)
		{
			m_player_in_scene.insert(role->GetUID());
		}
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_ONLY_FIGHT, __FUNCTION__);
	}

	this->SendOnlyFightRank();
}

void SpecialLogicOnlyFight::OnOnlyFightOver(int uid, int result_type)
{
	const OnlyFightOtherCfg & of_other = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
	
	int add_score = 0;
	switch (result_type)
	{
		case OF_TYPE_LOSE:
			add_score = of_other.lose_score;
			break;
		case OF_TYPE_VICT:
			add_score = of_other.wins_score;
			break;
		case OF_TYPE_DRAW:
			add_score = of_other.draw_score;
			break;
	default:
		break;
	}

	m_defeat_protect_time[uid] = EngineAdapter::Instance().Time() + 30;

	UNSTD_STATIC_CHECK(3 == OF_TYPE_MAX);
	std::map<int, OnlyFightPrivateInfo>::iterator find_it = m_private_info_map.find(uid);
	if (find_it != m_private_info_map.end())
	{
		OnlyFightPrivateInfo & info_con = find_it->second;
		info_con.fight_times++;
		info_con.total_score += add_score;

		if(OF_TYPE_VICT == result_type) 
		{
			++info_con.victory_times;
		}
		else if(OF_TYPE_LOSE == result_type) 
		{
			++info_con.lose_time;
			if(info_con.lose_time >= (UInt32)of_other.max_fail_times) 
			{
				Role * role = m_scene->GetRoleByUID(uid);
				if (NULL != role)
				{
					this->DelayKickOutRole(role);
				}
			}
		}
		if(add_score != of_other.draw_score) info_con.score_change_timestamp = (UInt32)EngineAdapter::Instance().Time();
	}
	else
	{
		m_private_info_map[uid] = OnlyFightPrivateInfo(add_score, OF_TYPE_VICT == result_type, uid);
	}
}

bool SortRank(const OnlyFightRankInfo& a, const OnlyFightRankInfo& b)
{
	const OnlyFightPrivateInfo & a_info = a.info_con;
	const OnlyFightPrivateInfo & b_info = b.info_con;

	if(a_info.total_score != b_info.total_score) return a_info.total_score > b_info.total_score;

	int our_win_rate = a_info.fight_times <= 0 ? 0 : a_info.victory_times * 1000 / a_info.fight_times;
	int their_win_rate = b_info.fight_times <= 0 ? 0 : b_info.victory_times * 1000 / b_info.fight_times;
	if (our_win_rate != their_win_rate) return our_win_rate > their_win_rate;

	if (a_info.score_change_timestamp != b_info.score_change_timestamp) return a_info.score_change_timestamp < b_info.score_change_timestamp;

	return false;
}

void SpecialLogicOnlyFight::RefreshOnlyFightRank()
{
	m_of_rank_list.clear();
	for (std::map<int, OnlyFightPrivateInfo>::iterator it = m_private_info_map.begin(); it != m_private_info_map.end(); it++)
	{
		m_of_rank_list.push_back(OnlyFightRankInfo(it->first, it->second));
	}

	std::sort(m_of_rank_list.begin(), m_of_rank_list.end(), SortRank);
}

void SpecialLogicOnlyFight::SendOnlyFightRank(Role * role, bool send_my_info)
{
	if(send_my_info && NULL == role) return;

	Protocol::SCActivityOnlyFightMyInfo aofmi;
	Protocol::SCActivityOnlyFightRankAllInfo aofrai;
	
	aofrai.resi_role_num = m_scene->RoleNum();
	aofrai.list_num = 0;
	bool is_find = false;
	for (int i = 0; i < (int)m_of_rank_list.size(); i++)
	{
		if(aofrai.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;
		Protocol::OnlyFightRankItem & rank_item = aofrai.list_info[aofrai.list_num++];
		rank_item.rank_posi = aofrai.list_num;
		rank_item.total_score = m_of_rank_list[i].info_con.total_score;
		rank_item.win_times = m_of_rank_list[i].info_con.victory_times;
		rank_item.battle_times = m_of_rank_list[i].info_con.fight_times;
		UserCacheNode * user = UserCacheManager::Instance().GetUserNode(m_of_rank_list[i].uid);
		if (NULL != user) user->GetName(rank_item.role_name);

		if (NULL != role && send_my_info && role->GetUID() == m_of_rank_list[i].uid)
		{
			aofmi.m_info = rank_item;
			is_find = true;
		}
	}
	int length = sizeof(aofrai) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - aofrai.list_num) * sizeof(aofrai.list_info[0]);
	if(length < 0) return;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&aofrai, length);
		if(send_my_info) 
		{
			aofmi.type = Protocol::SCActivityOnlyFightMyInfo::ONLY_FIGHT_MY_INFO_TYPE_SEND;
			if (!is_find)
			{
				role->GetName(aofmi.m_info.role_name);
				std::map<int, OnlyFightPrivateInfo>::iterator it = m_private_info_map.find(role->GetUID());
				if (it != m_private_info_map.end())
				{
					aofmi.m_info.total_score = it->second.total_score;
					aofmi.m_info.win_times = it->second.victory_times;
					aofmi.m_info.battle_times = it->second.fight_times;
				}
			}
			role->GetRoleModuleManager()->NetSend((const void *)&aofmi, sizeof(aofmi));
		}
	}
	else
	{
		m_scene->SendToRole((const char *)&aofrai, length);
	}
}

void SpecialLogicOnlyFight::SendInSceneAllRoleInfo()
{
	Protocol::SCActivityOnlyFightMyInfo info;
	info.type = Protocol::SCActivityOnlyFightMyInfo::ONLY_FIGHT_MY_INFO_TYPE_SEND;
	for (int i = 0; i < (int)m_of_rank_list.size(); i++)
	{
		Role * role  = m_scene->GetRoleByUID(m_of_rank_list[i].uid);
		if(NULL == role) continue;

		int rank_posi = (i + 1) > ACTIVITY_RANK_DEFAULT_NUM_MAX ? 0 : i + 1;
		info.m_info.rank_posi = rank_posi;
		info.m_info.total_score = m_of_rank_list[i].info_con.total_score;
		info.m_info.win_times = m_of_rank_list[i].info_con.victory_times;
		info.m_info.battle_times = m_of_rank_list[i].info_con.fight_times;
		role->GetName(info.m_info.role_name);

		role->GetRoleModuleManager()->NetSend((const void *)&info, sizeof(info));
	}
}

void SpecialLogicOnlyFight::SendAllParticipNotice()
{
	Protocol::SCActivityOnlyFightMyInfo info;
	Protocol::SCActivityOnlyFightRankAllInfo rank_info;
	rank_info.resi_role_num = m_scene->RoleNum();
	rank_info.list_num = 0;
	std::map<int, Protocol::OnlyFightRankItem> m_info_map;	//暂存玩家排行信息
	Protocol::OnlyFightRankItem rank_item;
	int rank_posi = 1;
	for (int i = 0; i < (int)m_of_rank_list.size(); i++)
	{
		rank_item.rank_posi = rank_posi++;
		rank_item.total_score = m_of_rank_list[i].info_con.total_score;
		rank_item.win_times = m_of_rank_list[i].info_con.victory_times;
		rank_item.battle_times = m_of_rank_list[i].info_con.fight_times;

		UserCacheNode * user = UserCacheManager::Instance().GetUserNode(m_of_rank_list[i].uid);
		if (NULL != user) user->GetName(rank_item.role_name);

		if (rank_info.list_num < ACTIVITY_RANK_DEFAULT_NUM_MAX)
		{
			rank_info.list_info[rank_info.list_num++] = rank_item;
		}
		else
		{
			rank_item.rank_posi = 0;
		}

		m_info_map[m_of_rank_list[i].uid] = rank_item;
		rank_item.Reset();
	}
	int length = sizeof(rank_info) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - rank_info.list_num) * sizeof(rank_info.list_info[0]);
	if (length < 0) return;

	info.type = Protocol::SCActivityOnlyFightMyInfo::ONLY_FIGHT_MY_INFO_TYPE_NOTICE;
	for (std::map<int, OnlyFightUserInfo>::iterator par_it = m_participate_player.begin(); par_it != m_participate_player.end(); par_it++)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(par_it->first);
		std::map<int, Protocol::OnlyFightRankItem>::iterator it = m_info_map.find(par_it->first);
		if (it != m_info_map.end())
		{
			info.m_info = it->second;
		}
		else
		{
			info.m_info.Reset();
		}
		if (NULL == role)
		{
			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(par_it->first).db_index,
				(char *)&rank_info, length, par_it->first);

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(par_it->first).db_index,
				(char *)&info, sizeof(info), par_it->first);
		}
		else
		{
			role->GetRoleModuleManager()->NetSend((const void *)&rank_info, length);
			role->GetRoleModuleManager()->NetSend((const void *)&info, sizeof(info));
		}

	}
}

void SpecialLogicOnlyFight::SendOpenRankInfoUser()
{
	if (m_scene_open_rank_list.empty()) return;

	Protocol::SCActivityOnlyFightRankAllInfo aofrai;
	aofrai.resi_role_num = m_scene->RoleNum();
	aofrai.list_num = 0;
	for (int i = 0; i < (int)m_of_rank_list.size(); i++)
	{
		if (aofrai.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;

		Protocol::OnlyFightRankItem & rank_item = aofrai.list_info[aofrai.list_num++];
		rank_item.rank_posi = aofrai.list_num;
		rank_item.total_score = m_of_rank_list[i].info_con.total_score;
		rank_item.win_times = m_of_rank_list[i].info_con.victory_times;
		rank_item.battle_times = m_of_rank_list[i].info_con.fight_times;
		UserCacheNode * user = UserCacheManager::Instance().GetUserNode(m_of_rank_list[i].uid);
		if (NULL != user) user->GetName(rank_item.role_name);
	}
	int length = sizeof(aofrai) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - aofrai.list_num) * sizeof(aofrai.list_info[0]);
	if (length < 0) return;
	for (std::set<int>::iterator it = m_scene_open_rank_list.begin(); it != m_scene_open_rank_list.end(); it++)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*it);
		if (NULL == role) continue;

		role->GetRoleModuleManager()->NetSend(&aofrai, length);
	}
}

void SpecialLogicOnlyFight::OnRefreshChest()
{
	// 先清理掉之前已经被开启过的宝箱
	std::vector<OnlyFightDeleteData> delete_vec;
	for (std::map<int, std::set<ObjID> >::iterator chest_obj_it = m_chest_list.begin();chest_obj_it != m_chest_list.end();chest_obj_it++)
	{
		for (std::set<ObjID>::iterator delete_obj_id_it = chest_obj_it->second.begin(); delete_obj_id_it != chest_obj_it->second.end(); ++delete_obj_id_it)
		{
			Obj * target_obj = m_scene->GetObj(*delete_obj_id_it);
			if (target_obj != NULL)
			{
				if (target_obj->GetObjType() == Obj::OBJ_TYPE_TREASURE_CHEST)
				{
					TreasureChest *chest_obj = (TreasureChest *)target_obj;
					if (chest_obj->IsOpen())
					{
						m_scene->DeleteObj(*delete_obj_id_it);
						delete_vec.push_back(OnlyFightDeleteData(chest_obj_it->first, *delete_obj_id_it));
					}
				}
			}

		}
	}

	if (!delete_vec.empty())
	{
		for (int i = 0; i < delete_vec.size(); i++)
		{
			std::map<int, std::set<ObjID> >::iterator chest_obj_it = m_chest_list.find(delete_vec[i].box_id);
			if (chest_obj_it != m_chest_list.end())
			{
				std::set<ObjID>::iterator it = chest_obj_it->second.find(delete_vec[i].obj_id);
				if (it != chest_obj_it->second.end())
				{
					chest_obj_it->second.erase(*it);
				}
			}
		}
	}

	// 再生成宝箱
	const OnlyFightOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
	for (int i = 0; i < other_cfg.num && m_box_num < other_cfg.max_num; i++)
	{
		const OnlyFightBoxCfg* cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightBoxCfg();
		if (cfg == NULL) return;

		TreasureChest * chest_obj = new TreasureChest(cfg->box_id);
		chest_obj->SetPos(m_scene->GetARandPosi());
		m_scene->AddObj(chest_obj);
		m_chest_list[cfg->box_id].insert(chest_obj->GetId());
		m_box_num += 1;
	}
}

void SpecialLogicOnlyFight::ClearAllChest()
{
	for (std::map<int, std::set<ObjID> >::iterator ext_loop = m_chest_list.begin(); ext_loop != m_chest_list.end(); ++ext_loop)
	{
		for (std::set<ObjID>::iterator obj_id_it = ext_loop->second.begin(); obj_id_it != ext_loop->second.end(); ++obj_id_it)
		{
			Obj * target_obj = m_scene->GetObj(*obj_id_it);
			if (target_obj != NULL)
			{
				if (target_obj->GetObjType() == Obj::OBJ_TYPE_TREASURE_CHEST)
				{
					m_scene->DeleteObj(*obj_id_it);		
				}
			}
		}
	}

	m_chest_list.clear();
	m_box_num = 0;
}

bool SpecialLogicOnlyFight::RandAndPutChestAwardInBag(Role * role, int chest_id)
{
	std::vector<ItemConfigData> tmp_award_list;
	if (LOGIC_CONFIG->GetActivityOnlyFightConfig()->RandAwardListByBoxID(chest_id, tmp_award_list))
	{
		if (!tmp_award_list.empty()) 
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)tmp_award_list.size(), &tmp_award_list[0], PUT_REASON_ONLY_FIGHT))
			{
				role->NoticeNum(errornum::EN_HAS_NO_SPACING);
				return false;
			}
			gamelog::g_log_only_fight.printf(LL_INFO, "open treasure chest: item[id:%d, num:%d] now_time:%lld reason[%s]", tmp_award_list[0].item_id,
				tmp_award_list[0].num, (long long)EngineAdapter::Instance().Time(), __FUNCTION__);
		}
	}
	return true;
}

void SpecialLogicOnlyFight::OnlyFightIssueActivityAward()
{
	int rank_list[ONLY_FIGHT_RANK_TITLE_MAX_NUM] = {0};
	int count = 0;
	gamelog::g_log_only_fight.printf(LL_INFO, "ActivityOnlyFight End, Give Rank Reward -> NowSec: %lld reason[%s]", (long long)EngineAdapter::Instance().Time(), __FUNCTION__);
	for (std::vector<OnlyFightRankInfo>::iterator loop_it = m_of_rank_list.begin(); loop_it != m_of_rank_list.end(); ++loop_it)
	{
		int rank_posi = std::distance(m_of_rank_list.begin(), loop_it) + 1;
		if (rank_posi <= ONLY_FIGHT_RANK_TITLE_MAX_NUM)
		{
			rank_list[count++] = loop_it->uid;
		}

		int role_id = loop_it->uid;
		int role_level = 1;
		const OnlyFightUserInfo * user_info = MapValuesPtr(m_participate_player, role_id);
		if (user_info)
		{
			role_level = user_info->level;
		}

		const OnlyFightRankAward * award_con = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightRankCfg(rank_posi, role_level);
		if (NULL == award_con) continue;

		//发送称号给玩家
		if (award_con->title > 0)
		{
			WorldStatus::Instance().SendTitleToRole(award_con->title, loop_it->uid, __FUNCTION__);
		}

		static MailContentParam contentparam; contentparam.Reset();
		int index = 0;
		for (int award_idx = 0; award_idx < (int)award_con->item_reward.size() && award_idx < ONLY_FIGHT_AWARD_MAX_COUNT; ++award_idx)
		{
			//邮件物品附件满了,先发送一封.
			if (index >= MAX_ATTACHMENT_ITEM_NUM)
			{	
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_rank_reward_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_rank_reward_content, rank_posi);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(loop_it->uid, MAIL_REASON_DEFAULT, contentparam);
				}
				index = 0;
			}

			const ItemConfigData & award_obj = award_con->item_reward[award_idx];
			contentparam.item_list[index++] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
		}

		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_rank_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_rank_reward_content, rank_posi);

		//通过邮件发送奖励给玩家
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(loop_it->uid, MAIL_REASON_DEFAULT, contentparam);
		}

		const OnlyFightPrivateInfo & info = loop_it->info_con;
		float win_rate = info.victory_times * 100.0f / info.fight_times;
		gamelog::g_log_only_fight.printf(LL_INFO, "role_uid[%d] Rank: %d, Score: %d, Win/Battle: %d%d, WinRate: %.2f%%, LastScoreChangeTime: %u reason[%s]",
			 loop_it->uid, rank_posi, info.total_score, info.victory_times, info.fight_times, win_rate, info.score_change_timestamp, __FUNCTION__);
	}
	gamelog::g_log_only_fight.printf(LL_INFO, "ActivityOnlyFight Rank Reward Give Finished.");
	if (count > 0)
	{
		WorldStatus::Instance().AddOnlyFightRecord(ONLY_FIGHT_RANK_TITLE_MAX_NUM, rank_list);
	}
}

bool SpecialLogicOnlyFight::OnlyFightIssueSurvivalAward(int need_check_num)
{
	int role_num = m_scene->RoleNum();
	for (int i = 0; i < role_num; ++i)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		const OnlyFightSurvAward * award_con = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSurvAward(role->GetLevel(), need_check_num);
		if (NULL == award_con) continue;

		static MailContentParam contentparam; contentparam.Reset();

		int index = 0;
		for (int award_idx = 0; award_idx < (int)award_con->item_reward.size() && award_idx < ONLY_FIGHT_AWARD_MAX_COUNT; ++award_idx)
		{
			//邮件物品附件满了,先发送一封.
			if (index >= MAX_ATTACHMENT_ITEM_NUM)
			{
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_survival_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_survival_reward, need_check_num);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				}
				index = 0;
			}
			const ItemConfigData & award_obj = award_con->item_reward[award_idx];
			contentparam.item_list[index++] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
		}

		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = award_con->experience;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin_bind;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_survival_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_survival_reward, need_check_num);
		if (lensub > 0 && length > 0) MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}
	return true;
}

void SpecialLogicOnlyFight::OnlyFightIssueSurvivalAward()
{
	for (std::map<int, std::vector<OnlyFightPraInfo> >::iterator it = m_survival_map.begin(); it != m_survival_map.end(); it++)
	{
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			std::map<int, OnlyFightUserInfo>::iterator user_it = m_participate_player.find(UidToInt(it->second[i].user_id));
			if (user_it == m_participate_player.end()) continue;

			const OnlyFightSurvAward * award_con = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSurvAward(user_it->second.level, it->first);
			if (NULL == award_con) continue;

			if (user_it->second.survival_award_flag & (1 << award_con->seq))
			{
				continue;
			}

			static MailContentParam contentparam; contentparam.Reset();

			int index = 0;
			for (int award_idx = 0; award_idx < (int)award_con->item_reward.size() && award_idx < ONLY_FIGHT_AWARD_MAX_COUNT; ++award_idx)
			{
				//邮件物品附件满了,先发送一封.
				if (index >= MAX_ATTACHMENT_ITEM_NUM)
				{
					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_survival_subject);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_survival_reward, it->first);
					if (lensub > 0 && length > 0)
					{
						MailRoute::MailToUser(it->second[i].user_id, MAIL_REASON_DEFAULT, contentparam);
					}
					index = 0;
				}
				const ItemConfigData & award_obj = award_con->item_reward[award_idx];
				contentparam.item_list[index++] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}

			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = award_con->experience;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin_bind;
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_survival_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_survival_reward, it->first);
			if (lensub > 0 && length > 0) MailRoute::Instance().MailToUser(it->second[i].user_id, MAIL_REASON_DEFAULT, contentparam);

			user_it->second.survival_award_flag |= (1 << award_con->seq);
		}
	}
}

void SpecialLogicOnlyFight::OnlyFightIssueParticipAward()
{
	if (m_participate_player.empty()) return;

	static MailContentParam contentparam; 
	for (std::map<int, OnlyFightUserInfo>::iterator it = m_participate_player.begin(); it != m_participate_player.end(); it++)
	{
		if(0 != it->second.particip_award_flag) continue;

		contentparam.Reset();
		const OnlyFightParticipationAward * cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightParCfg(it->second.level);
		if (NULL == cfg) continue;

		int index = 0;
		for (int award_idx = 0; award_idx < (int)cfg->item_reward.size() && award_idx < ONLY_FIGHT_AWARD_MAX_COUNT; ++award_idx)
		{
			//邮件物品附件满了,先发送一封.
			if (index >= MAX_ATTACHMENT_ITEM_NUM)
			{
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_participation_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_participation_award);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);
				}
				index = 0;
			}
			const ItemConfigData & award_obj = cfg->item_reward[award_idx];
			contentparam.item_list[index++] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
		}
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = cfg->experience;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = cfg->coin_bind;

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_only_fight_participation_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_only_fight_participation_award);
		if (lensub > 0 && length > 0) MailRoute::MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);

		it->second.particip_award_flag = 1;

		unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetCommonData().fetch_only_fight_join_reward_flag = 1;
			role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT] = fetch_reward_zero_timestamp;
		}
		else
		{
			crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
			hgslais.role_id = it->first;
			hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT;
			hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index, (const char *)&hgslais, sizeof(hgslais));
		}

	}
}

void SpecialLogicOnlyFight::SendOnlyFightSpecialInfo(Role * role)
{
	Protocol::SCActivityOnlyFightSpecialInfo info;
	info.resi_role = m_scene->RoleNum();

	if (NULL == role)
	{
		for (int i = 0; i < info.resi_role; i++)
		{
			Role * user = m_scene->GetRoleByIndex(i);
			if (NULL == user) continue;

			user->GetRoleModuleManager()->NetSend(&info, sizeof(info));
		}
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

void SpecialLogicOnlyFight::ForceStopOnlyFightBattle()
{
	battlegameprotocol::GameBattleForceFinishBattleModeReq ffbmr;
	ffbmr.battle_mode = BATTLE_MODE_ONLY_FIGHT;

	World::GetInstWorld()->SendMsgToAllBattle((const char *)&ffbmr, sizeof(ffbmr));
}
