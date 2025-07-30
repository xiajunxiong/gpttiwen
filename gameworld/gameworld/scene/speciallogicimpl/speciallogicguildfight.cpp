#include "speciallogicguildfight.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/guild/guildmanager.hpp"
#include "world.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "obj/otherobj/treasurechest.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "servercommon/errornum.h"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "global/guild/guild.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/gamelog.h"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/guild/guildmember.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"

SpecialLogicGuildFight::SpecialLogicGuildFight(Scene * scene) : SpecialLogic(scene), m_need_refresh_rank(false), m_next_refresh_rank_timestamp(0),
next_box_refresh_tamp(0), m_box_num(0), force_to_next_time(0), is_refresh_box_flag(false), m_is_arena_close(true), m_delay_close_arena_time(0), is_send_fight_special_info(false)
{
}

void SpecialLogicGuildFight::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
	if (NULL == activity || !activity->IsActivityOpen()) return;

	// 刷新宝箱
	if (is_refresh_box_flag && now_second >= next_box_refresh_tamp)
	{
		const GuildFightOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
		if (m_box_num < other_cfg.max_num)
		{
			this->OnRefreshChest();
		}

		next_box_refresh_tamp = now_second + other_cfg.time;
	}

	//入场时间不检测
	if (activity->IsActivityEnterTime()) return;

	// 刷新排行榜
	if (m_need_refresh_rank)
	{
		m_need_refresh_rank = false;
		this->RefreshGuildFightRank();
		this->SendInSceneAllRoleInfo();
		this->SendOpenRankInfoUser();
	}
	if (is_send_fight_special_info)
	{
		is_send_fight_special_info = false;
		this->SendGuildFightSpecialInfo();
	}

	/*
	if (m_need_refresh_rank && now_second >= m_next_refresh_rank_timestamp)
	{
		const GuildFightOtherCfg & gf_other_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
		m_need_refresh_rank = false;
		m_next_refresh_rank_timestamp = now_second + gf_other_cfg.rank_refresh_interval;
		this->RefreshGuildFightRank();
		this->SendGuildFightRank();
		this->SendInSceneAllRoleInfo();
	}*/

	// 存活奖励
	if (!m_survival_award_need_check_num.empty() && NULL != m_scene)
	{
		for (std::set<int>::iterator it = m_survival_award_need_check_num.begin(); it != m_survival_award_need_check_num.end();)
		{
			if (m_scene->RoleNum() <= *it)
			{
				int role_num = m_scene->RoleNum();
				for (int i = 0; i < role_num; i++)
				{
					Role *role = m_scene->GetRoleByIndex(i);
					if (NULL == role) continue;

					m_survival_map[*it].push_back(GuildFightPraInfo(role->GetLevel(), role->GetUserId()));
				}
				m_survival_award_need_check_num.erase(it++);
				continue;
			}
			++it;
		}
	}

	// 如果场景内的活跃家族数不大于 1 个，强制结束活动
	if ((int)m_family_palyer_in_scene.size() <= 1 && (0u == m_delay_close_arena_time || (now_second > m_delay_close_arena_time)))
	{
		if (force_to_next_time != 0 && now_second >= force_to_next_time)
		{
			ActivityManager::Instance().OnForceToNextState(ACTIVITY_TYPE_GUILD_FIGHT);		
		}
		// 延时3秒关闭活动
		if (force_to_next_time == 0)
		{
			force_to_next_time = now_second + 3;
			m_is_arena_close =  true;
		}
	}
}

void SpecialLogicGuildFight::OnRoleEnterScene(Role * role)
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

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (team->GetLeaderUID() != role->GetUID())
		{
			Role* leader = team->GetLeaderRole();
			if (NULL != leader && leader->GetGuildID() != role->GetGuildID())
			{
				role->NoticeNum(errornum::EN_GUILD_FIGHT_NOT_SAME_GUILD_WITH_LEADER);
				this->DelayKickOutRole(role);
				return;
			}
		}
	}

	int role_id = role->GetUID();
	const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	if (m_private_info_map[role_id].failures >= gf_other.max_fail_times)
	{
		this->DelayKickOutRole(role);
		return;
	}

	int old_num = (int)m_family_palyer_in_scene.size();

	ActivityGuildFight * act_ref = (ActivityGuildFight *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT));
	if (NULL == act_ref || !act_ref->IsActivityOpen())
	{
		this->DelayKickOutRole(role);
		return;
	}
	else
	{
		m_family_palyer_in_scene[role->GetGuildID()].insert(role->GetUID());
		this->SendGuildFightRank(role);

		if (old_num <= 1 && (int)(m_family_palyer_in_scene.size()) > 1 && m_delay_close_arena_time > 0)
		{
			m_delay_close_arena_time = 0;
		}
		is_send_fight_special_info = true;
	}
	this->SendAdvanceFetchInfo(role);
}

void SpecialLogicGuildFight::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role) return;

	std::map<int, std::set<int> >::iterator find_it = m_family_palyer_in_scene.find(role->GetGuildID());
	if (m_family_palyer_in_scene.end() != find_it)
	{
		find_it->second.erase(role->GetUID());

		if (find_it->second.empty()) m_family_palyer_in_scene.erase(find_it);

		if (is_logout && m_family_palyer_in_scene.size() <= 1 && BattleManagerLocal::GetInstance().IsRoleInBattle(role->GetUID()))
		{
			// 两人PK，在战斗中掉线一人  等3分钟
			m_delay_close_arena_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 180;
		}
	}
	is_send_fight_special_info = true;
	this->OnCloseRank(role);
}

bool SpecialLogicGuildFight::CanChallengeOthers(Role * role, Role * target)
{
	if(NULL == role || NULL == target) return false;

	ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
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

	if (role->GetGuildID() == target->GetGuildID())
	{
		role->NoticeNum(errornum::EN_GUILD_IS_SAME_AS_YOURS);
		return false;
	}
	/*	// 就算是点到非队长的组队玩家，也一样进入战斗
	if (target->InTeam() && !target->IsLeader())
	{
		role->NoticeNum(errornum::EN_TARGET_IS_NOT_LEADER);
		return false;
	}
	*/

	//Team * team = target->GetMyTeam();
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

void SpecialLogicGuildFight::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_GUILD_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_FIGHT)) return;
	
	std::set<int> tmp_atk_uid_list, tmp_def_uid_list;
	for (int atk_idx = 0; atk_idx < ack->attacker_list.role_count; ++atk_idx)
	{
		tmp_atk_uid_list.insert(ack->attacker_list.role_list[atk_idx].uid);
	}

	for (int def_idx = 0; def_idx < ack->defender_list.role_count; ++def_idx)
	{
		tmp_def_uid_list.insert(ack->defender_list.role_list[def_idx].uid);
	}

	switch (ack->battle_result)
	{
	case RESULT_TYPE_ATTACKER_WIN:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_VICT);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_LOSE);
		break;
	case RESULT_TYPE_DEFENDER_WIN:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_LOSE);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_VICT);
		break;
	case RESULT_TYPE_DRAW:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_DRAW);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_DRAW);
		break;
	case RESULT_TYPE_FORCED_FINISH:		// 平局或战斗中断，算双方均失败；
	case RESULT_TYPE_EXCEED_MAX_ROUND:
	case RESULT_TYPE_EXCEED_MAX_TIME:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_LOSE);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_LOSE);
		break;
	}

	m_need_refresh_rank = true;
}

void SpecialLogicGuildFight::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_GUILD_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_FIGHT)) return;
	
	std::set<int> uid;
	uid.insert(ack->uid);

	//逃跑默认失败
	this->OnGuildFightOver(uid, GF_TYPE_LOSE);

	m_need_refresh_rank = true;
}

void SpecialLogicGuildFight::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_GUILD_FIGHT != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_FIGHT)) return;

	std::set<int> tmp_atk_uid_list, tmp_def_uid_list;
	for (int atk_idx = 0; atk_idx < ack->attacker_list.role_count; ++atk_idx)
	{
		tmp_atk_uid_list.insert(ack->attacker_list.role_list[atk_idx].uid);
	}

	for (int def_idx = 0; def_idx < ack->defender_list.role_count; ++def_idx)
	{
		tmp_def_uid_list.insert(ack->defender_list.role_list[def_idx].uid);
	}

	this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_LOSE);
	this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_LOSE);
	m_need_refresh_rank = true;
}

bool SpecialLogicGuildFight::OnOpenTreasureChest(Role * role, TreasureChest * chest_obj)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (NULL == role || NULL == chest_obj) return false;

	Team * team = role->GetMyTeam();
	if (NULL != team)
	{
		if (role->GetUID() != team->GetLeaderUID())
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return false;
		}

		// 所有队员随机一次奖励
		int mem_count = team->GetMemberCount();
		for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)
		{
			Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
			if (NULL != mem_role) this->RandAndPutChestAwardInBag(mem_role, chest_obj->GetTreasureChestid(),team->IsLeader(mem_role));
		}
	}
	else	// 如果是单人开箱，只自己随机一次奖励
	{
		if (!this->RandAndPutChestAwardInBag(role, chest_obj->GetTreasureChestid()))
		{
			return false;
		}
	}
	if(m_box_num > 0)m_box_num -= 1;
	return true;
}

bool SpecialLogicGuildFight::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	if (role == NULL) return false;

	//	活动开启后依旧可以组队
	//ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
	//if (NULL != activity && activity->IsActivityOpen())
	//{
	//	if (!activity->IsActivityEnterTime())
	//	{
	//		role->NoticeNum(errornum::EN_ACTIVITY_OPEN_OPER_FAIL);
	//		return false;
	//	}		
	//}

	if (team == NULL)
	{
		return is_from_create;
	}

	Role * leared_role = team->GetLeaderRole();
	if (leared_role != NULL)
	{
		if (role->GetGuildID() != leared_role->GetGuildID())
		{
			role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_GUILD_MEMBER);
			return false;//不一样的家族无法组队
		}
	}
	return true;
}

bool SpecialLogicGuildFight::CanInvitationRole(Role * role, int uid)
{
	Guild* guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
		return false;
	}

	if (NULL == guild->GetMemberManager()->GetMember(IntToUid(uid)))
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
		return false;
	}

	return true;
}

bool SpecialLogicGuildFight::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
	if (NULL != activity && activity->IsActivityOpen())
	{
		if (NULL != member && !activity->IsActivityEnterTime())
		{
			member->NoticeNum(errornum::EN_ACTIVITY_OPEN_OPER_FAIL);
			return false;
		}

	}

	return true;
}

void SpecialLogicGuildFight::OnActivityStandy()
{
	m_need_refresh_rank = false;
	m_next_refresh_rank_timestamp = 0;
	m_survival_award_need_check_num.clear();
	m_survival_map.clear();
	m_gf_rank_list.clear();
	m_next_chest_refersh_time.clear();
	m_participate_player.clear();
	m_private_info_map.clear();
	m_familys_info_map.clear();
	m_family_palyer_in_scene.clear();
	m_guild_participate_num.clear();
	next_box_refresh_tamp = 0;
	force_to_next_time = 0;
	m_is_arena_close = true;
	m_delay_close_arena_time = 0u;
	is_send_fight_special_info = false;
}

void SpecialLogicGuildFight::OnActivityOpen()
{
	//准备时间刷宝箱，正式开始后宝箱消失
	//this->ExtinctAllChest();

	this->InitFightingHall();
}

void SpecialLogicGuildFight::OnActivityEnterTime()
{
	m_chest_list.clear();
	is_refresh_box_flag = true;
	m_is_arena_close = false;
}

void SpecialLogicGuildFight::InitFightingHall()
{
	// 初始化存活奖励检测表
	LOGIC_CONFIG->GetActivityGuildFightConfig()->GetSurvivalCheckNumList(m_survival_award_need_check_num);

	// 记录所有场景内的玩家为参与玩家，记录活跃家族的玩家ID
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;

		m_participate_player[role->GetUID()] = GuildFightUserInfo(role->GetLevel());

		std::map<int, std::set<int> >::iterator find_it = m_family_palyer_in_scene.find(role->GetGuildID());
		if (m_family_palyer_in_scene.end() != find_it)
		{
			find_it->second.insert(role->GetUID());
		}
		else
		{
			std::set<int> tmp_player_set; tmp_player_set.insert(role->GetUID());
			m_family_palyer_in_scene[role->GetGuildID()] = tmp_player_set;
		}
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_GUILD_FIGHT, __FUNCTION__, role->GetGuildID());
	}

	// 记录各家族参与的人数
	for (std::map<int, std::set<int> >::iterator guild_id_it = m_family_palyer_in_scene.begin(); guild_id_it != m_family_palyer_in_scene.end(); ++guild_id_it)
	{
		int number = (int)guild_id_it->second.size();
		if (number > 0) m_guild_participate_num[guild_id_it->first] = number;
	}

	this->SendGuildParticipateRoleNum();		// 将各家族参与的人数信息发送给各家族；
	this->SendGuildFightRank();
}

void SpecialLogicGuildFight::RequestGuildFightRankInfo(Role * role)
{
	if (NULL == role) return;

	this->SendGuildFightRank(role, true);
	this->OnOpenRank(role);
}

void SpecialLogicGuildFight::OnActivityEnd()
{
	m_survival_award_need_check_num.clear();	// 清空存活奖检测表

	this->ForceStopGuildFightBattle();			// 强制中断家族大乱斗的战斗
	this->DelayKickOutAllRole();				// 把玩家都踢出场景
	this->ExtinctAllChest();					// 清理场景内的宝箱
	this->RefreshGuildFightRank();				// 最后一次整理排名
	this->SendActivityEndNoticeAndInfo();		// 最后发送给全部	参与人员结算界面	
	this->GuildFightIssueActivityAward();		// 发放活动奖励
	this->GuildFightIssueSurvivalAward();		// 发放存活奖励
	this->GuildFightIssueParticipAward();		// 发放参与奖励
	this->SetGuildHonorBattleNeedInfo();

	m_participate_player.clear();
	m_guild_participate_num.clear();
	m_survival_map.clear();						// 发完存活奖，清空m_survival_map
	is_refresh_box_flag = false;
	m_is_arena_close = true;
}

void SpecialLogicGuildFight::OnRoleLogin(Role * role)
{
	if (NULL == role) return;

	this->SendGuildParticipateRoleNum(role);
}

void SpecialLogicGuildFight::OnOpenRank(Role * role)
{
	if(NULL == role) return;

	m_scene_open_rank_list.insert(role->GetUID());
}

void SpecialLogicGuildFight::OnCloseRank(Role * role)
{
	if(NULL == role) return;

	m_scene_open_rank_list.erase(role->GetUID());
}

void SpecialLogicGuildFight::OnAdavanceFetch(Role * role, short op_type, int param1)
{
	if (NULL == role) return;

	ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
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
	std::map<int, GuildFightUserInfo>::iterator it = m_participate_player.find(role->GetUID());
	if (it == m_participate_player.end()) return;

	switch (op_type)
	{
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD:
		{
			if (0 != it->second.particip_award_flag)
			{
				role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
				return;
			}

			const GuildFightJoinAwardCfg * join_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightJoinAwardCfg(it->second.level);
			if (NULL == join_cfg) return;

			if (!join_cfg->reward_list.empty())
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)join_cfg->reward_list.size(), &join_cfg->reward_list[0], PUT_REASON_GUILD_FIGHT))
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&join_cfg->reward_list[0], (int)join_cfg->reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
			}

			role->AddExp(join_cfg->exp, ADD_EXP_TYPE_GUILD_FIGHT, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(join_cfg->coin, __FUNCTION__);
			it->second.particip_award_flag = 1;
			this->SendAdvanceFetchInfo(role);

			unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
			if (NULL != role)
			{
				role->GetRoleModuleManager()->GetCommonData().fetch_guild_fight_join_reward_flag = 1;
				role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT] = fetch_reward_zero_timestamp;
			}
			else
			{
				crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
				hgslais.role_id = it->first;
				hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT;
				hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index, (const char *)&hgslais, sizeof(hgslais));
			}
		}
		break;
		case Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_SURVIVAL_AWARD:
		{
			std::map<int, std::vector<GuildFightPraInfo> >::iterator sur_it = m_survival_map.find(param1);
			if (sur_it == m_survival_map.end()) return;

			for (int i = 0; i < (int)sur_it->second.size(); i++)
			{
				if (it->first != UidToInt(sur_it->second[i].user_id))
				{
					continue;
				}
				const GuildFightSurvAward * award_con = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightSurvivalAward(sur_it->first, it->second.level);
				if (NULL == award_con) continue;

				if (it->second.survival_award_flag & (1 << award_con->seq))
				{
					role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
					return;
				}

				if (award_con->item_reward.award_num > 0)
				{
					if (!role->GetRoleModuleManager()->GetKnapsack()->PutList(award_con->item_reward.award_num, &award_con->item_reward.award_list[0], PUT_REASON_GUILD_FIGHT))
					{
						role->GetRoleModuleManager()->GetKnapsack()->SendMail(&award_con->item_reward.award_list[0], (int)award_con->item_reward.award_num, SNED_MAIL_TYPE_DEFAULT, true);
					}
				}

				role->AddExp(award_con->experience, ADD_EXP_TYPE_GUILD_FIGHT, __FUNCTION__);
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

void SpecialLogicGuildFight::SendAdvanceFetchInfo(Role * role)
{
	if (NULL == role) return;
	Protocol::SCActivityAdvanceFetchInfo info;
	info.activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
	std::map<int, GuildFightUserInfo>::iterator it = m_participate_player.find(role->GetUID());
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

void SpecialLogicGuildFight::OnGuildFightOver(std::set<int> uid_list, int result_type)
{
	if (uid_list.empty()) return;

	int role_num = (int)uid_list.size();
	GuildID guild_id = 0; bool find_flag = false;
	for (std::set<int>::iterator loop = uid_list.begin(); loop != uid_list.end(); ++loop)
	{
		Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*loop));
		if (NULL == mem_role) continue;

		Guild * guild = GuildManager::Instance().GetGuild(mem_role->GetGuildID());
		if (NULL == guild) continue;

		guild_id = mem_role->GetGuildID();
		find_flag = true;
		break;
	}

	const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	if (find_flag)
	{
		int score_times = 0;
		switch (result_type)
		{
		case GF_TYPE_LOSE:
			score_times = gf_other.lose_score;
			break;
		case GF_TYPE_VICT:
			score_times = gf_other.wins_score;
			break;
		case GF_TYPE_DRAW:
			score_times = gf_other.draw_score;
			break;
		}
		UNSTD_STATIC_CHECK(3 == GF_TYPE_MAX)

		int score_additon = role_num * score_times;
		std::map<int, GuildFightFamilysInfo>::iterator find_it = m_familys_info_map.find(guild_id);
		if (m_familys_info_map.end() != find_it)		// 如果找到
		{
			GuildFightFamilysInfo & info_con = find_it->second;
			info_con.total_score += score_additon;
			if (GF_TYPE_VICT == result_type) ++info_con.victory_times;
			++info_con.fight_times;

			if (0 != score_additon) info_con.score_change_time = (UInt32)EngineAdapter::Instance().Time();
		}
		else		// 如果没找到
		{
			m_familys_info_map[guild_id] = GuildFightFamilysInfo(score_additon, GF_TYPE_VICT == result_type);
		}
	}


	for (std::set<int>::iterator loop = uid_list.begin(); loop != uid_list.end(); ++loop)
	{
		int role_id = *loop;

		m_defeat_protect_time[role_id] = EngineAdapter::Instance().Time() + 30;

		if (GF_TYPE_LOSE == result_type)
		{
			std::map<int, GuildFightPrivateInfo>::iterator find_it = m_private_info_map.find(role_id);
			if (m_private_info_map.end() != find_it)
			{
				GuildFightPrivateInfo & info_con = find_it->second;
				++info_con.failures;
			}
			else
			{
				m_private_info_map[role_id] = GuildFightPrivateInfo(false);
			}

			Role * loop_role = m_scene->GetRoleByUID(role_id);
			if (NULL != loop_role)
			{
				if (m_private_info_map[role_id].failures >= gf_other.max_fail_times) 
				{
					TeamManager::Instance().ExitTeam(loop_role);		
					this->DelayKickOutRole(loop_role);
				}
			}
		}
	}
}

bool SortRank(const GuildFightRankInfo & a, const GuildFightRankInfo & b)
{
	const GuildFightFamilysInfo & a_info = a.info_con;
	const GuildFightFamilysInfo & b_info = b.info_con;

	if (a_info.total_score != b_info.total_score) return a_info.total_score > b_info.total_score;

	int our_win_rate = a_info.fight_times <= 0 ? 0 : a_info.victory_times * 1000 / a_info.fight_times;
	int their_win_rate = b_info.fight_times <= 0 ? 0 : b_info.victory_times * 1000 / b_info.fight_times;
	if (our_win_rate != their_win_rate) return our_win_rate > their_win_rate;

	if (a_info.score_change_time != b_info.score_change_time) return a_info.score_change_time < b_info.score_change_time;
	return false;
}

void SpecialLogicGuildFight::RefreshGuildFightRank()
{
	m_gf_rank_list.clear();
	for (std::map<int, GuildFightFamilysInfo>::iterator loop = m_familys_info_map.begin(); loop != m_familys_info_map.end(); ++loop)
	{
		m_gf_rank_list.push_back(GuildFightRankInfo(loop->first, loop->second));
	}

	std::sort(m_gf_rank_list.begin(), m_gf_rank_list.end(), SortRank);
}

void SpecialLogicGuildFight::SendGuildFightRank(Role * role, bool send_my_family_info)
{
	if (send_my_family_info && NULL == role) return;

	Protocol::SCActivityGuildFightMyGuildInfo agfmgi;
	Protocol::SCActivityGuildFightRankAllInfo agfrai;

	agfrai.resi_role = m_scene->RoleNum();
	agfrai.resi_family = (int)m_family_palyer_in_scene.size();
	agfrai.list_num = 0;
	agfmgi.type = Protocol::SCActivityGuildFightMyGuildInfo::GUILD_FIGHT_TYPE_INFO;
	bool is_find = false;
	for (std::vector<GuildFightRankInfo>::iterator rank_loop = m_gf_rank_list.begin(); rank_loop != m_gf_rank_list.end(); ++rank_loop)
	{
		if (agfrai.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;
		Protocol::GuildFightRankItem & rank_item = agfrai.list_info[agfrai.list_num++];
		rank_item.rank_posi = agfrai.list_num;
		rank_item.total_score = rank_loop->info_con.total_score;
		rank_item.win_times = rank_loop->info_con.victory_times;
		rank_item.battle_times = rank_loop->info_con.fight_times;
		
		Guild * guild = GuildManager::Instance().GetGuild(rank_loop->guild_id);
		if (NULL != guild) guild->GetGuildName(rank_item.guild_name);

		if (send_my_family_info && NULL != role && role->GetGuildID() == rank_loop->guild_id) 
		{
			agfmgi.my_family_info = rank_item;
			is_find = true;
		}
	}

	int send_len = sizeof(agfrai) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - agfrai.list_num) * sizeof(agfrai.list_info[0]);
	if (send_len <= 0) return;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&agfrai, send_len);
		if (send_my_family_info) 
		{
			if (!is_find)	//没找到则代表家族没上榜
			{
				Guild * guild = GuildManager::Instance().GetGuild(role->GetGuildID());
				if (NULL == guild)
				{
					return;
				}
				std::map<int, GuildFightFamilysInfo>::iterator it = m_familys_info_map.find(guild->GetGuildID());
				if (it != m_familys_info_map.end())
				{
					agfmgi.my_family_info.total_score = it->second.total_score;
					agfmgi.my_family_info.win_times = it->second.victory_times;
					agfmgi.my_family_info.battle_times = it->second.fight_times;
				}
				agfmgi.my_family_info.rank_posi = 0;
				guild->GetGuildName(agfmgi.my_family_info.guild_name);

			}
			role->GetRoleModuleManager()->NetSend((const void *)&agfmgi, sizeof(agfmgi));
		}
	}
	else
	{
		m_scene->SendToRole((const char *)&agfrai, send_len);
	}
}

void SpecialLogicGuildFight::SendInSceneAllRoleInfo()
{
	Protocol::SCActivityGuildFightMyGuildInfo info;
	info.type = Protocol::SCActivityGuildFightMyGuildInfo::GUILD_FIGHT_TYPE_INFO;

	for (int i = 0; i < (int)m_gf_rank_list.size(); i++)
	{
		std::map<int, std::set<int> >::iterator it = m_family_palyer_in_scene.find(m_gf_rank_list[i].guild_id);
		if(it == m_family_palyer_in_scene.end()) continue;
		
		if((int)it->second.size() <= 0) continue;

		int rank_posi = (i + 1) > ACTIVITY_RANK_DEFAULT_NUM_MAX ? 0 : i + 1;
		info.my_family_info.rank_posi = rank_posi;
		info.my_family_info.battle_times = m_gf_rank_list[i].info_con.fight_times;
		info.my_family_info.total_score = m_gf_rank_list[i].info_con.total_score;
		info.my_family_info.win_times = m_gf_rank_list[i].info_con.victory_times;
		Guild * guild = GuildManager::Instance().GetGuild(m_gf_rank_list[i].guild_id);
		if(guild != NULL) guild->GetGuildName(info.my_family_info.guild_name);

		for (std::set<int>::iterator uid_it = it->second.begin(); uid_it != it->second.end(); uid_it++)
		{
			Role * role = m_scene->GetRoleByUID(*uid_it);
			if(role == NULL) continue;

			role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
		}
	}
}

void SpecialLogicGuildFight::SendActivityEndNoticeAndInfo()
{
	Protocol::SCActivityGuildFightMyGuildInfo info;
	Protocol::SCActivityGuildFightRankAllInfo agfrai;

	info.type = Protocol::SCActivityGuildFightMyGuildInfo::GUILD_FIGHT_TYPE_END_NOTICE_AND_INFO;
	agfrai.resi_role = m_scene->RoleNum();
	agfrai.resi_family = (int)m_family_palyer_in_scene.size();
	agfrai.list_num = 0;
	std::map<int, Protocol::GuildFightRankItem> m_guild_map;	//取出符合条件的信息暂存
	Protocol::GuildFightRankItem rank_item;
	int rank_posi = 1;
	for (std::vector<GuildFightRankInfo>::iterator rank_loop = m_gf_rank_list.begin(); rank_loop != m_gf_rank_list.end(); ++rank_loop)
	{
		rank_item.rank_posi = rank_posi++;
		rank_item.total_score = rank_loop->info_con.total_score;
		rank_item.win_times = rank_loop->info_con.victory_times;
		rank_item.battle_times = rank_loop->info_con.fight_times;

		Guild * guild = GuildManager::Instance().GetGuild(rank_loop->guild_id);
		if (NULL != guild) guild->GetGuildName(rank_item.guild_name);

		if (agfrai.list_num < ACTIVITY_RANK_DEFAULT_NUM_MAX)
		{
			agfrai.list_info[agfrai.list_num++] = rank_item;
		}
		else
		{
			rank_item.rank_posi = 0;		
		}

		// 暂存排名信息
		m_guild_map[rank_loop->guild_id] = rank_item;
		rank_item.Reset();
	}

	int send_len = sizeof(agfrai) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - agfrai.list_num) * sizeof(agfrai.list_info[0]);
	if (send_len <= 0) return;

	for (std::map<int, int>::iterator guild_it = m_guild_participate_num.begin(); guild_it != m_guild_participate_num.end(); ++guild_it)
	{
		Guild * guild = GuildManager::Instance().GetGuild(guild_it->first);
		if (NULL == guild) continue;

		guild->GetMemberManagerRef().SendInfoToAllMemberFromHidden((const char *)&agfrai, send_len);
		std::map<int, Protocol::GuildFightRankItem>::iterator it = m_guild_map.find(guild_it->first);
		if (it != m_guild_map.end())
		{
			info.my_family_info = it->second;
		}
		else
		{
			info.my_family_info.Reset();
		}
		guild->GetMemberManagerRef().SendInfoToAllMemberFromHidden((const char *)&info, sizeof(info));
	}
}

void SpecialLogicGuildFight::SendOpenRankInfoUser()
{
	if(m_scene_open_rank_list.empty()) return;

	Protocol::SCActivityGuildFightRankAllInfo agfrai;
	agfrai.resi_role = m_scene->RoleNum();
	agfrai.resi_family = (int)m_family_palyer_in_scene.size();
	agfrai.list_num = 0;
	for (std::vector<GuildFightRankInfo>::iterator rank_loop = m_gf_rank_list.begin(); rank_loop != m_gf_rank_list.end(); ++rank_loop)
	{
		if (agfrai.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;

		Protocol::GuildFightRankItem & rank_item = agfrai.list_info[agfrai.list_num++];
		rank_item.rank_posi = agfrai.list_num;
		rank_item.total_score = rank_loop->info_con.total_score;
		rank_item.win_times = rank_loop->info_con.victory_times;
		rank_item.battle_times = rank_loop->info_con.fight_times;

		Guild * guild = GuildManager::Instance().GetGuild(rank_loop->guild_id);
		if (NULL != guild) guild->GetGuildName(rank_item.guild_name);
	}
	int send_len = sizeof(agfrai) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - agfrai.list_num) * sizeof(agfrai.list_info[0]);
	if (send_len <= 0) return;
	for (std::set<int>::iterator it = m_scene_open_rank_list.begin(); it != m_scene_open_rank_list.end(); it++)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*it);
		if (NULL == role) continue;

		role->GetRoleModuleManager()->NetSend(&agfrai, send_len);
	}
}

void SpecialLogicGuildFight::OnRefreshChest()
{
	// 先清理掉之前已经被开启过的宝箱
	std::vector<GuildFightDeleteData> delete_vec;
	for(std::map<int, std::set<ObjID> >::iterator chest_obj_it = m_chest_list.begin();chest_obj_it != m_chest_list.end();chest_obj_it++)
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
						delete_vec.push_back(GuildFightDeleteData(chest_obj_it->first, *delete_obj_id_it));
					}
				}
			}	
		}
	}

	if (!delete_vec.empty())
	{
		for (int i = 0; i < (int)delete_vec.size(); i++)
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
	const GuildFightOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	for (int i = 0; i < other_cfg.num && m_box_num < other_cfg.max_num; i++)
	{
		const GuildFightChestCfg* cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetRandGuildFightChestCfg();
		if (cfg == NULL) return;

		TreasureChest * chest_obj = new TreasureChest(cfg->box_id);
		chest_obj->SetPos(m_scene->GetARandPosi());
		m_scene->AddObj(chest_obj);
		m_chest_list[cfg->box_id].insert(chest_obj->GetId());
		m_box_num += 1;
	}
}

void SpecialLogicGuildFight::ExtinctAllChest()
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

bool SpecialLogicGuildFight::RandAndPutChestAwardInBag(Role * role, int chest_id, bool is_leader)
{
	std::vector<ItemConfigData> tmp_award_list;
	if (LOGIC_CONFIG->GetActivityGuildFightConfig()->RandAwardListByChestID(chest_id, tmp_award_list, is_leader))
	{
		if (!tmp_award_list.empty()) 
		{
			if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)tmp_award_list.size(), &tmp_award_list[0], PUT_REASON_GUILD_FIGHT))
			{
				role->NoticeNum(errornum::EN_HAS_NO_SPACING);
				return false;
			}
			gamelog::g_log_guild_fight.printf(LL_INFO, "open treasure chest: item[id:%d, num:%d] now_time:%lld reason[%s]", tmp_award_list[0].item_id,
				tmp_award_list[0].num, (long long)EngineAdapter::Instance().Time(), __FUNCTION__);
		}
	}
	return true;
}

void SpecialLogicGuildFight::GuildFightIssueActivityAward()
{
	gamelog::g_log_guild_fight.printf(LL_INFO, "ActivityGuildFight End, Give Rank Reward -> NowSec: %lld", (long long)EngineAdapter::Instance().Time());
	for (std::vector<GuildFightRankInfo>::iterator loop_it = m_gf_rank_list.begin(); loop_it != m_gf_rank_list.end(); ++loop_it)
	{
		int rank_posi = std::distance(m_gf_rank_list.begin(), loop_it) + 1;
		Guild * guild = GuildManager::Instance().GetGuild(loop_it->guild_id);
		if (NULL == guild) continue;


		std::vector<GuildMember * > guild_member_arr = guild->GetMemberManagerRef().GetAllGuildMember();
		for (int i = 0; i < (int)guild_member_arr.size(); ++i)
		{
			GuildMember * curr = guild_member_arr[i];
			int role_id = curr->GetUid();
			int role_level = curr->GetLevel();
			const GuildFightUserInfo * user_info = MapValuesPtr(m_participate_player, role_id);
			if (user_info)
			{
				role_level = user_info->level;
			}
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (NULL != role)
			{
				role->GetRoleModuleManager()->OnSetGuildFightInfo(rank_posi);
			}
			else
			{
				crossgameprotocal::HiddenGameActivityEndInfo info;
				info.role_id = role_id;
				info.activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
				info.param1 = rank_posi;

				InternalComm::Instance().SendToGameThroughCross2((const void *)&info, sizeof(info), IntToUid(role_id).db_index);
			}

			const GuildFightRankAward * award_con = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightActivityAward(rank_posi, role_level);
			if (NULL == award_con) continue;

			if (award_con->title > 0)
			{
				//guild->GetMemberManager() ->SendTitleToAllMember(award_con->title, __FUNCTION__);
				guild->SetGuildFightTitle(award_con->title);
			}

			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = award_con->contribution;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin;
			for (int award_idx = 0; award_idx < award_con->item_reward.award_num && award_idx < GUILD_FIGHT_AWARD_MAX_COUNT; ++award_idx)
			{
				const ItemConfigData & award_obj = award_con->item_reward.award_list[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_fight_rank_reward_title);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_fight_familys_rank_reward, rank_posi);
			if (lensub > 0 && length > 0) MailRoute::Instance().MailToUser(curr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}

		const GuildFightFamilysInfo & info = loop_it->info_con;
		float win_rate = info.victory_times * 100.0f / info.fight_times;
		gamelog::g_log_guild_fight.printf(LL_INFO, "\tFamily[%d, %s], Rank: %d, Score: %d, Win/Battle: %d%d, WinRate: %.2f%%, LastScoreChangeTime: %u",
			guild->GetGuildID(), guild->GetName(), rank_posi, info.total_score, info.victory_times, info.fight_times, win_rate, info.score_change_time);
	}
	gamelog::g_log_guild_fight.printf(LL_INFO, "ActivityGuildFight Rank Reward Give Finished.");
}

bool SpecialLogicGuildFight::GuildFightIssueSurvivalAward(int need_check_num)
{
	int role_num = m_scene->RoleNum();
	for (int i = 0; i < role_num; ++i)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		const GuildFightSurvAward * award_con = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightSurvivalAward(need_check_num, role->GetLevel());
		if (NULL == award_con) continue;

		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = award_con->experience;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin_bind;
		for (int award_idx = 0; award_idx < award_con->item_reward.award_num && award_idx < GUILD_FIGHT_AWARD_MAX_COUNT; ++award_idx)
		{
			const ItemConfigData & award_obj = award_con->item_reward.award_list[award_idx];
			contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_fight_survival_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_fight_survival_reward, need_check_num);
		if (lensub > 0 && length > 0) MailRoute::Instance().MailToUser(role->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}

	return true;
}

void SpecialLogicGuildFight::GuildFightIssueSurvivalAward()
{
	for (std::map<int, std::vector<GuildFightPraInfo> >::iterator it = m_survival_map.begin(); it != m_survival_map.end(); it++)
	{
		std::set<int> uid_hash;
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			int role_id = UidToInt(it->second[i].user_id);
			std::map<int, GuildFightUserInfo>::iterator user_it = m_participate_player.find(role_id);
			if (user_it == m_participate_player.end()) continue;

			const GuildFightSurvAward * award_con = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightSurvivalAward(it->first, user_it->second.level);
			if (NULL == award_con) continue;
		
			if (user_it->second.survival_award_flag & (1 << award_con->seq))
			{
				continue;
			}
			if (uid_hash.find(role_id) != uid_hash.end())
			{
				gamelog::g_log_guild_fight.printf(LL_WARNING, "GuildFightIssueSurvivalAward key[%d] uid[%d] repeat!",
					it->first, role_id);

				continue;
			}

			uid_hash.insert(role_id);

			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = award_con->experience;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin_bind;
			for (int award_idx = 0; award_idx < award_con->item_reward.award_num && award_idx < GUILD_FIGHT_AWARD_MAX_COUNT; ++award_idx)
			{
				const ItemConfigData & award_obj = award_con->item_reward.award_list[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_fight_survival_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_fight_survival_reward, it->first);
			if (lensub > 0 && length > 0) MailRoute::Instance().MailToUser(it->second[i].user_id, MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_guild_fight.printf(LL_INFO, "GuildFightIssueSurvivalAward key[%d] uid[%d]", it->first, role_id);
			user_it->second.survival_award_flag |= (1 << award_con->seq);
		}
	}
}

void SpecialLogicGuildFight::GuildFightIssueParticipAward()
{
	if (m_participate_player.empty() || m_guild_participate_num.empty()) return;

	for (std::map<int, GuildFightUserInfo>::iterator it = m_participate_player.begin(); it != m_participate_player.end(); it++)
	{
		if (0 != it->second.particip_award_flag) continue;

		const GuildFightJoinAwardCfg * join_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightJoinAwardCfg(it->second.level);
		if(NULL == join_cfg) continue;

		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = join_cfg->coin;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = join_cfg->exp;
		if (!join_cfg->reward_list.empty())
		{
			for (int award_idx = 0; award_idx < (int)join_cfg->reward_list.size() && award_idx < MAX_ATTACHMENT_ITEM_NUM; ++award_idx)
			{
				const ItemConfigData & award_obj = join_cfg->reward_list[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_fight_participation_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_fight_participation_award);
		if (lensub > 0 && length > 0)
		{
			MailRoute::Instance().MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);
		}
		it->second.particip_award_flag = 1;

		unsigned int fetch_reward_zero_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetCommonData().fetch_guild_fight_join_reward_flag = 1;
			role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT] = fetch_reward_zero_timestamp;
		}
		else
		{	
			crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync hgslais;
			hgslais.role_id = it->first;
			hgslais.special_logic_activity_type = DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT;
			hgslais.fetch_reward_zero_timestamp = fetch_reward_zero_timestamp;

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index, (const char *)&hgslais, sizeof(hgslais));
		}
	}

	const GuildFightOtherCfg & gf_other_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	for (std::map<int, int>::iterator guild_it = m_guild_participate_num.begin(); guild_it != m_guild_participate_num.end(); ++guild_it)
	{
		Guild * guild = GuildManager::Instance().GetGuild(guild_it->first);
		if (NULL != guild) guild->AddExp(guild_it->second * gf_other_cfg.guild_fund);
	}

	//参与奖励换新，旧奖励屏蔽
	//const GuildFightOtherCfg & gf_other_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();

	//static MailContentParam contentparam; contentparam.Reset();
	//contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = gf_other_cfg.join_reward_coin;
	//contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = gf_other_cfg.join_reward_gongxian;
	//for (int award_idx = 0; award_idx < gf_other_cfg.pati_award.award_num && award_idx < GUILD_FIGHT_AWARD_MAX_COUNT; ++award_idx)
	//{
	//	const ItemConfigData & award_obj = gf_other_cfg.pati_award.award_list[award_idx];
	//	contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
	//}

	//int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_fight_participation_subject);
	//int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_fight_participation_award);
	//if (lensub > 0 && length > 0) MailRoute::MailToMultUser(&m_participate_player[0], (int)m_participate_player.size(), MAIL_REASON_DEFAULT, contentparam);

	//for (std::map<int, int>::iterator guild_it = m_guild_participate_num.begin(); guild_it != m_guild_participate_num.end(); ++guild_it)
	//{
	//	Guild * guild = GuildManager::Instance().GetGuild(guild_it->first);
	//	if (NULL != guild) guild->AddExp(guild_it->second * gf_other_cfg.guild_fund);
	//}

	/*for (std::vector<GuildFightPraInfo>::const_iterator player_it = m_participate_player.begin(); player_it != m_participate_player.end(); ++player_it)
	{
		EventHandler::Instance().OnRAGodTokenFinishLimitActivity(player_it->user_id, ACTIVITY_TYPE_GUILD_FIGHT);
	}*/
}

void SpecialLogicGuildFight::SendGuildParticipateRoleNum(Role * role)
{
	Protocol::SCActivityGuildFightParticipNum agfpn;
	agfpn.guild_participate_num = 0;

	if (NULL != role)
	{
		std::map<int, int>::iterator guild_it = m_guild_participate_num.find(role->GetGuildID());
		if (m_guild_participate_num.end() != guild_it)
		{
			agfpn.guild_participate_num = guild_it->second;
		}
		role->GetRoleModuleManager()->NetSend((const void *)&agfpn, sizeof(agfpn));
	}
	else
	{
		for (std::map<int, int>::iterator guild_it = m_guild_participate_num.begin(); guild_it != m_guild_participate_num.end(); ++guild_it)
		{
			Guild * guild = GuildManager::Instance().GetGuild(guild_it->first);
			if (NULL == guild) continue;

			agfpn.guild_participate_num = guild_it->second;

			guild->GetMemberManagerRef().SendInfoToAllMemberFromHidden((const char *)&agfpn, sizeof(agfpn));
		}
	}
}

void SpecialLogicGuildFight::SendGuildFightSpecialInfo(Role * role)
{
	Protocol::SCActivityGuildFightSpecialInfo info;
	info.resi_role = m_scene->RoleNum();
	info.resi_family = (int)m_family_palyer_in_scene.size();

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

void SpecialLogicGuildFight::ForceStopGuildFightBattle()
{
	battlegameprotocol::GameBattleForceFinishBattleModeReq ffbmr;
	ffbmr.battle_mode = BATTLE_MODE_GUILD_FIGHT;

	World::GetInstWorld()->SendMsgToAllBattle((const char *)&ffbmr, sizeof(ffbmr));
}

void SpecialLogicGuildFight::SetGuildHonorBattleNeedInfo()
{
	GuildHonorBattleManager::Instance().OnGuildFightHappen();

	for (int i = 0; i < (int)m_gf_rank_list.size(); ++i)
	{
		const GuildFightRankInfo & curr = m_gf_rank_list[i];
		GuildHonorBattleManager::Instance().SetGuildFightResult(curr.guild_id, curr.info_con.total_score, curr.info_con.score_change_time);
	}
}
