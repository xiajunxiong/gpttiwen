#include "speciallogicworldteamarena.hpp"
#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/team/team.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"

#include "battle/battle_manager_local.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "protocol/msgworldteamarena.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"

#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"

SpecialLogicWorldTeamArena::SpecialLogicWorldTeamArena(Scene * scene)
	:SpecialLogic(scene), m_need_refresh_rank(false), m_next_refresh_rank_timestamp(0), m_is_arena_close(true), force_to_next_time(0), 
	m_delay_close_arena_time(0), is_send_fight_special_info(0)
{
}

void SpecialLogicWorldTeamArena::OnRoleEnterScene(Role * role)
{
	if(NULL == role) return; 

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

	//检测战队
	long long team_unique_id = WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID());
	if (0 == team_unique_id)
	{
		role->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_NOT_HAS_TEAM);
		this->DelayKickOutRole(role);
		return;
	}
	//玩家进来需要拥有队伍
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		this->DelayKickOutRole(role);
		return;
	}
	//不是队长则需要查看是否跟队长处于同一个战队
	if (team->GetLeaderUID() != role->GetUID())
	{
		Role* leader = team->GetLeaderRole();
		if (NULL != leader && WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(leader->GetUID()) != team_unique_id)
		{
			role->NoticeNum(errornum::EN_GUILD_FIGHT_NOT_SAME_GUILD_WITH_LEADER);
			this->DelayKickOutRole(role);
			return;
		}
	}

	//检测是否还有资格留在场景中
	std::map<long long, WorldTeamArenaRankItem>::iterator find_it = m_team_arena_info_map.find(team_unique_id);
	if (find_it != m_team_arena_info_map.end())
	{
		const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
		if (find_it->second.failures_times >= (UInt32)other_cfg.max_fail_num)
		{
			this->DelayKickOutRole(role);
			return;
		}
	}
	int old_team_num = (int)m_scene_team_map.size();

	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA));
	if (NULL == activity || !activity->IsActivityOpen())
	{
		this->DelayKickOutRole(role);
		return;
	}
	else
	{
		//记录一些和下发场景所需信息
		m_scene_team_map[team_unique_id].insert(role->GetUID());
		if (!activity->IsActivityEnterTime())
		{
			this->SendWorldTeamArenaRank(role, true);
		}
		if (old_team_num <= 1 && (int)(m_scene_team_map.size()) > 1 && m_delay_close_arena_time > 0)
		{
			m_delay_close_arena_time = 0;
		}
		is_send_fight_special_info = true;
	}
	
	this->SendWorldTeamArenaSpecialInfo(role);
	this->SendAdvanceFetchInfo(role);
}

void SpecialLogicWorldTeamArena::OnRoleLeaveScene(Role * role, bool is_logout)
{
	std::map<long long, std::set<int> >::iterator team_it = m_scene_team_map.find(WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID()));
	if (m_scene_team_map.end() != team_it)
	{
		team_it->second.erase(role->GetUID());
		
		if(team_it->second.empty()) m_scene_team_map.erase(team_it);

		if (is_logout && m_scene_team_map.size() <= 1 && BattleManagerLocal::GetInstance().IsRoleInBattle(role->GetUID()))
		{
			// 两人PK，在战斗中掉线一人  等3分钟
			m_delay_close_arena_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 180;
		}
	}
	is_send_fight_special_info = true;
	this->OnCloseRank(role);
}

void SpecialLogicWorldTeamArena::OnRoleLeaveTeam(Role * role)
{
	if (NULL == role) return;

	this->DelayKickOutRole(role);
}

void SpecialLogicWorldTeamArena::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA));
	if (NULL == activity || !activity->IsActivityOpen())
	{
		return;
	}

	if (is_send_fight_special_info)
	{
		is_send_fight_special_info = false;
		this->SendWorldTeamArenaSpecialInfo();
	}

	//入场时间不检测
	if (activity->IsActivityEnterTime()) return;

	// 刷新排行榜
	if (m_need_refresh_rank)
	{
		m_need_refresh_rank = false;
		this->RefresTeamArenaRank();
		this->SendInSceneAllRoleInfo();
		this->SendOpenRankInfoUser();
	}

	// 如果场景内的活跃战队数量不大于1时，延时一下关闭
	if ((int)m_scene_team_map.size() <= 1 && (0u == m_delay_close_arena_time || (now_second > m_delay_close_arena_time)))
	{
		if (force_to_next_time != 0 && now_second >= force_to_next_time)
		{
			ActivityManager::Instance().OnForceToNextState(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
		}
		// 延时3秒	
		if (force_to_next_time == 0)
		{
			force_to_next_time = now_second + 3;
			m_is_arena_close = true;
		}
	}
}

bool SpecialLogicWorldTeamArena::CanChallengeOthers(Role * role, Role * target)
{
	if (NULL == role || NULL == target) return false;
	
	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
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

	std::map<long long, time_t>::iterator iter = m_defeat_protect_time.find(WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(target->GetUID()));
	if (iter != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= iter->second)
	{
		role->NoticeNum(errornum::EN_TARGET_FIGHT_PROTECT);
		return false;
	}

	std::map<long long, time_t>::iterator it = m_defeat_protect_time.find(WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID()));
	if (it != m_defeat_protect_time.end() && EngineAdapter::Instance().Time() <= it->second)
	{
		role->NoticeNum(errornum::EN_MY_FIGHT_PROTECT);
		return false;
	}

	return true;
}

bool SpecialLogicWorldTeamArena::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL != activity && activity->IsActivityOpen())
	{
		if (NULL != role && !activity->IsActivityEnterTime())
		{
			role->NoticeNum(errornum::EN_ACTIVITY_OPEN_OPER_FAIL);
			return false;
		}
	}
	//队伍只能在外部创建
	if (NULL == team)
	{
		return false;
	}
	
	return true;
}

bool SpecialLogicWorldTeamArena::CanTeamMemberGotoLeaderFromOtherScene(Role * member, Role * leader)
{
	/*ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL != activity && activity->IsActivityOpen())
	{
		if (NULL != member && !activity->IsActivityEnterTime())
		{
			member->NoticeNum(errornum::EN_ACTIVITY_OPEN_OPER_FAIL);
			return false;
		}
	}*/
	//需要让玩家回归
	return true;
}

void SpecialLogicWorldTeamArena::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_WORLD_TEAM_ARENA != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_TEAM_ARENA)) return;

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
		case RESULT_TYPE_EXCEED_MAX_ROUND:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_DRAW);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_DRAW);
		break;
		case RESULT_TYPE_FORCED_FINISH:		
		case RESULT_TYPE_EXCEED_MAX_TIME:
		this->OnGuildFightOver(tmp_atk_uid_list, GF_TYPE_LOSE);
		this->OnGuildFightOver(tmp_def_uid_list, GF_TYPE_LOSE);
		break;
	}

	m_need_refresh_rank = true;
}

void SpecialLogicWorldTeamArena::OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_WORLD_TEAM_ARENA != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_TEAM_ARENA)) return;

	//战队组队赛,自己逃跑不影响其他还在战斗的成员
}

void SpecialLogicWorldTeamArena::OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (BATTLE_MODE_WORLD_TEAM_ARENA != ack->battle_mode || !ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_WORLD_TEAM_ARENA)) return;

	//强制结束则双方都失败
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

int SpecialLogicWorldTeamArena::GetTeamIndexByUnqiueId(long long team_unique_id)
{
	for (int i = 0; i < m_scene->RoleNum(); i++)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if(NULL == role) continue;

		Team * team = role->GetMyTeam();
		if(NULL == team) continue;

		if (team_unique_id == WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID()))
		{
			return team->GetTeamIndex();
		}
	}
	
	return -1;
}

void SpecialLogicWorldTeamArena::OnActivityStandy()
{
	m_need_refresh_rank = false;
	m_next_refresh_rank_timestamp = 0;
	m_scene_team_map.clear();
	m_team_arena_info_map.clear();
	m_rank_list.clear();
	m_participate_user_list.clear();
	m_scene_open_rank_list.clear();
	force_to_next_time = 0;
	m_is_arena_close = true;
	m_delay_close_arena_time = 0u;
	is_send_fight_special_info = false;
}

void SpecialLogicWorldTeamArena::OnActivityEnterTime()
{
	m_is_arena_close = false;
}

void SpecialLogicWorldTeamArena::OnActivityOpen()
{
	this->InitFightingHall();					// 先初始化后再发送,否则查找不到参与人员数据
}

void SpecialLogicWorldTeamArena::OnActivityEnd()
{
	this->ForceStopWorldTeamArenaBattle();		// 强制中断英雄会武的战斗
	this->DelayKickOutAllRole();				// 把玩家都踢出场景
	this->RefresTeamArenaRank();				// 最后一次整理排名
	this->SendAllParticipNotice();				// 最后发送给全部	参与人员结算界面	
	this->WorldTeamArenaParticipAward();		// 发放参与奖励
	this->WorldTeamArenaRankAward();			// 发放活动排名奖励

	WorldTeamArenaRankManager::Instance().OnActivityEnd(m_rank_list);	//排名列表存储至管理

	m_is_arena_close = true;
	m_scene_team_map.clear();
}

void SpecialLogicWorldTeamArena::RequestFightRankInfo(Role * role)
{
	if (NULL == role) return;

	this->SendWorldTeamArenaRank(role, true);
	this->OnOpenRank(role);
}

void SpecialLogicWorldTeamArena::OnRoleLogin(Role * role)
{
	if (NULL == role) return;

	this->SendWorldTeamArenaSpecialInfo(role);
}

void SpecialLogicWorldTeamArena::OnInitiateFight(Role * role, int target_uid)
{
	if(NULL == role) return;

	Role * target_role = m_scene->GetRoleByUID(target_uid);
	if (NULL == target_role)
	{
		return;
	}

	//需要双方都有队伍(只判断发起人是否是队长)
	Team * attacker_team = role->GetMyTeam();
	Team * defender_team = target_role->GetMyTeam();
	if (NULL == attacker_team || NULL == defender_team || !attacker_team->IsLeader(role) || !this->CanChallengeOthers(role, target_role))
	{
		return;
	}
	Role * defender_leader_role = defender_team->GetLeaderRole();
	if (NULL == defender_leader_role || role->GetUID() == defender_leader_role->GetUID())	
	{
		return;
	}
	const WorldTeamArenaTeamSignUpData * attacker_sign_data = WorldTeamArenaSignUpManager::Instance().GetWorldTeamArenaSignUpDataById(role->GetUID());
	const WorldTeamArenaTeamSignUpData * defender_sign_data = WorldTeamArenaSignUpManager::Instance().GetWorldTeamArenaSignUpDataById(target_role->GetUID());
	if (NULL == attacker_sign_data || NULL == defender_sign_data)
	{
		return;
	}

	ActivityWorldTeamArenaFightInfo fight_info;
	int role_num = 0;
	Role * role_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM * 2];
	for (int mem_idx = 0; mem_idx < attacker_team->GetMemberCount() && mem_idx < MAX_TEAM_MEMBER_NUM && 
		fight_info.attacker_team.member_count < ARRAY_LENGTH(fight_info.attacker_team.member_list); ++mem_idx)
	{
		TeamMember* member = attacker_team->GetMemberInfoByIndex(mem_idx);
		if (NULL == member || !member->IsFollow()) continue;

		Role* attacker_role = attacker_team->GetMemberRoleByIndex(mem_idx);
		if (NULL == attacker_role)
		{
			continue;
		}
		if (!attacker_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_WORLD_TEAM_ARENA))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
			return;
		}
		WorldTeamArenaSignUpManager::Instance().FillMemberInfo(attacker_role, fight_info.attacker_team.member_list[fight_info.attacker_team.member_count]);
		//队长放首位
		if (role->GetUID() == attacker_role->GetUID() && 0 != mem_idx)
		{
			std::swap(fight_info.attacker_team.member_list[0], fight_info.attacker_team.member_list[fight_info.attacker_team.member_count]);
		}
		fight_info.attacker_team.member_count++;
		if (role_num < ARRAY_LENGTH(role_list))
		{
			role_list[role_num++] = attacker_role;
		}
	}

	for (int mem_idx = 0; mem_idx < defender_team->GetMemberCount() && mem_idx < MAX_TEAM_MEMBER_NUM &&
		fight_info.defender_team.member_count < ARRAY_LENGTH(fight_info.defender_team.member_list); ++mem_idx)
	{
		TeamMember* member = defender_team->GetMemberInfoByIndex(mem_idx);
		if (NULL == member || !member->IsFollow()) continue;

		Role* defender_role = defender_team->GetMemberRoleByIndex(mem_idx);
		if (NULL == defender_role)
		{
			continue;
		}
		if (!defender_role->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_WORLD_TEAM_ARENA))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
			return;
		}
		WorldTeamArenaSignUpManager::Instance().FillMemberInfo(defender_role, fight_info.defender_team.member_list[fight_info.defender_team.member_count]);
		//队长放首位
		if (defender_leader_role->GetUID() == defender_role->GetUID() && 0 != mem_idx)
		{
			std::swap(fight_info.attacker_team.member_list[0], fight_info.attacker_team.member_list[fight_info.attacker_team.member_count]);
		}
		fight_info.defender_team.member_count++;
		if (role_num < ARRAY_LENGTH(role_list))
		{
			role_list[role_num++] = defender_role;
		}
	}
	if (fight_info.attacker_team.member_count <= 0 || fight_info.defender_team.member_count <= 0)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_SUIT_FOR_BATTLE);
		return;
	}
	memcpy(fight_info.attacker_team.team_name, attacker_sign_data->team_name, sizeof(fight_info.attacker_team.team_name));
	memcpy(fight_info.defender_team.team_name, defender_sign_data->team_name, sizeof(fight_info.defender_team.team_name));
	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		if (m_rank_list[i].team_unique_id == attacker_sign_data->team_unique_id)
		{
			fight_info.attacker_team.rank_posi = i;
		}
		else if(m_rank_list[i].team_unique_id == defender_sign_data->team_unique_id)
		{
			fight_info.defender_team.rank_posi = i;
		}
	}

	fight_info.start_fight_timestamp = (unsigned int)EngineAdapter::Instance().Time() + ACTIVITY_WORLD_TEAM_ARENA_LOADING_FIGHT_TIME;
	if (!BattleManagerLocal::GetInstance().StartChallengeOthersReq(role, defender_leader_role->GetUID(), BATTLE_MODE_WORLD_TEAM_ARENA))
	{
		return;
	}

	Protocol::SCActivityWorldTeamArenaFightInfo info;
	info.start_fight_timestamp = fight_info.start_fight_timestamp;
	info.fight_info = fight_info;
	
	for (int i = 0; i < role_num && i < ARRAY_LENGTH(role_list); ++i)
	{
		if (NULL != role_list[i])
		{
			EngineAdapter::Instance().NetSendSerializeMsg(role_list[i]->GetNetId(), &info);
		}
	}
}

void SpecialLogicWorldTeamArena::OnOpenRank(Role * role)
{
	if (NULL == role) return;

	m_scene_open_rank_list.insert(role->GetUID());
}

void SpecialLogicWorldTeamArena::OnCloseRank(Role * role)
{
	if (NULL == role) return;

	m_scene_open_rank_list.erase(role->GetUID());
}

void SpecialLogicWorldTeamArena::OnAdavanceFetch(Role * role, short op_type, int param1)
{
	if (NULL == role) return;

	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
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
	std::map<int, WorldTeamArenaSceneUserInfo>::iterator it = m_participate_user_list.find(role->GetUID());
	if (it == m_participate_user_list.end()) return;

	//该活动只有参与能提前领取
	if (Protocol::CSActivityAdvanceFetchReq::ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD != op_type)
	{
		return;
	}

	if (0 != it->second.particip_award_flag)
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const ActivityWorldTeamArenaParRewardCfg * par_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetParRewardCfg(it->second.level);
	if (NULL == par_cfg) return;

	if (!par_cfg->reward_list.empty())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)par_cfg->reward_list.size(), &par_cfg->reward_list[0], PUT_REASON_WORLD_TEAM_ARENA))
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendMail(&par_cfg->reward_list[0], (int)par_cfg->reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	role->AddExp(par_cfg->exp, ADD_EXP_TYPE_ONLY_FIGHT, __FUNCTION__);
	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(par_cfg->coin, __FUNCTION__);
	it->second.particip_award_flag = 1;
	this->SendAdvanceFetchInfo(role);
	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s role_id[%d] level[%d]", __FUNCTION__, role->GetUID(), it->second.level);
}

void SpecialLogicWorldTeamArena::SendAdvanceFetchInfo(Role * role)
{
	if (NULL == role) return;

	Protocol::SCActivityAdvanceFetchInfo info;
	info.activity_type = ACTIVITY_TYPE_WORLD_TEAM_ARENA;
	std::map<int, WorldTeamArenaSceneUserInfo>::iterator it = m_participate_user_list.find(role->GetUID());
	if (it == m_participate_user_list.end())
	{
		info.level = role->GetLevel();
		info.particip_award_flag = 0;
		info.survival_award_flag = 0;
	}
	else
	{
		info.level = it->second.level;
		info.particip_award_flag = it->second.particip_award_flag;
		info.survival_award_flag = 0;
	}

	info.reserve_sh = 0;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void SpecialLogicWorldTeamArena::InitFightingHall()
{
	// 记录场景内的所有玩家为参与玩家
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;

		m_participate_user_list[role->GetUID()] = WorldTeamArenaSceneUserInfo(role->GetLevel());

		long long team_unique_id = WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID());
		m_scene_team_map[team_unique_id].insert(role->GetUID());
		EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_WORLD_TEAM_ARENA, __FUNCTION__);
	}
	//策划让即使一场没打也有排名
	for (std::map<long long, std::set<int> >::const_iterator it = m_scene_team_map.begin(); it != m_scene_team_map.end(); ++it)
	{
		WorldTeamArenaRankItem team_rank_item;
		team_rank_item.team_unique_id = it->first;
		WorldTeamArenaSignUpManager::Instance().GetTeamNameById(it->first, team_rank_item.team_name);

		m_team_arena_info_map[it->first] = team_rank_item;
	}
	this->RefresTeamArenaRank();
	this->SendWorldTeamArenaRank();
}

void SpecialLogicWorldTeamArena::OnGuildFightOver(std::set<int> uid_list, int result_type)
{
	if(uid_list.empty()) return;

	//按理说不会出现多个战队的情况
	std::set<long long> team_unique_list;
	for (std::set<int>::const_iterator it = uid_list.begin(); it != uid_list.end(); it++)
	{
		team_unique_list.insert(WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(*it));
	}

	if(team_unique_list.empty()) return;

	const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
	for (std::set<long long>::const_iterator it = team_unique_list.begin(); it != team_unique_list.end(); ++it)
	{
		m_defeat_protect_time[(*it)] = EngineAdapter::Instance().Time() + other_cfg.battle_protect;
		std::map<long long, WorldTeamArenaRankItem>::iterator team_it = m_team_arena_info_map.find(*it);
		if (team_it == m_team_arena_info_map.end())
		{
			//不知道怎么进来打的.
			continue;
		}
		team_it->second.score_change_time = EngineAdapter::Instance().Time();
		team_it->second.fight_times++;
		switch (result_type)
		{
			case WTA_TYPE_LOSE:
			team_it->second.total_score += other_cfg.lose_score;
			team_it->second.failures_times++;
			break;
			case WTA_TYPE_VICT:
			team_it->second.total_score += other_cfg.win_score;
			team_it->second.victory_times++;
			break;
			case WTA_TYPE_DRAW:
			team_it->second.total_score += other_cfg.draw_score;
			break;
		}
		UNSTD_STATIC_CHECK(3 == WTA_TYPE_MAX);
		if (team_it->second.failures_times >= (unsigned int)other_cfg.max_fail_num)
		{	
			//不能直接用这个uid_list,中途逃跑
			std::map<long long, std::set<int> >::iterator scene_team_it = m_scene_team_map.find(team_it->first);
			if (m_scene_team_map.end() != scene_team_it)
			{
				for (std::set<int>::iterator uid_it = scene_team_it->second.begin(); uid_it != scene_team_it->second.end(); ++uid_it)
				{
					this->DelayKickOutRole(*uid_it);
				}
				m_scene_team_map.erase(scene_team_it);
			}
		}
	}
}

bool SortRank(const WorldTeamArenaRankItem & a, const WorldTeamArenaRankItem & b)
{
	if (a.total_score != b.total_score) return a.total_score > b.total_score;

	int our_win_rate = a.fight_times <= 0 ? 0 : a.victory_times * 1000 / a.fight_times;
	int their_win_rate = b.fight_times <= 0 ? 0 : b.victory_times * 1000 / b.fight_times;
	if (our_win_rate != their_win_rate) return our_win_rate > their_win_rate;

	if (a.score_change_time != b.score_change_time) return a.score_change_time < b.score_change_time;
	return false;
}

void SpecialLogicWorldTeamArena::RefresTeamArenaRank()
{
	m_rank_list.clear();
	for (std::map<long long, WorldTeamArenaRankItem>::const_iterator it = m_team_arena_info_map.begin(); it != m_team_arena_info_map.end(); ++it)
	{
		m_rank_list.push_back(it->second);
	}

	std::sort(m_rank_list.begin(), m_rank_list.end(), SortRank);
}

void SpecialLogicWorldTeamArena::SendWorldTeamArenaRank(Role * role, bool send_my_info)
{
	if(send_my_info && NULL == role) return;

	Protocol::SCActivityWorldTeamArenaRankAllInfo all_info;
	Protocol::SCActivityWorldTeamArenaMyInfo my_info;

	all_info.resi_team_num = (int)m_scene_team_map.size();
	all_info.list_num = 0;
	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		if(all_info.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;

		WorldTeamArenaRankInfo & rank_info = all_info.rank_list[all_info.list_num++];
		rank_info = m_rank_list[i];

		if (NULL != role && send_my_info && WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(role->GetUID()) == rank_info.team_unique_id)
		{
			my_info.m_info = rank_info;
			my_info.rank_posi = all_info.list_num; 
		}
	}
	int length = sizeof(all_info) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - all_info.list_num) * sizeof(all_info.rank_list[0]);
	if (length < 0) return;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&all_info, length);
		if (send_my_info)
		{
			my_info.type = Protocol::SCActivityWorldTeamArenaMyInfo::WORLD_TEAM_ARENA_MY_INFO_TYPE_SEND;
			role->GetRoleModuleManager()->NetSend((const void *)&my_info, sizeof(my_info));
		}
	}
	else
	{
		m_scene->SendToRole((const char *)&all_info, length);
	}
}

void SpecialLogicWorldTeamArena::SendInSceneAllRoleInfo()
{
	Protocol::SCActivityWorldTeamArenaMyInfo my_info;
	my_info.type = Protocol::SCActivityWorldTeamArenaMyInfo::WORLD_TEAM_ARENA_MY_INFO_TYPE_SEND;
	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		my_info.rank_posi = (i + 1) > ACTIVITY_RANK_DEFAULT_NUM_MAX ? 0 : i + 1;
		my_info.m_info = m_rank_list[i];
		std::set<int> team_user_id_list;
		WorldTeamArenaSignUpManager::Instance().GetQueryTeamUidList(m_rank_list[i].team_unique_id, team_user_id_list);
		for (std::set<int>::const_iterator it = team_user_id_list.begin(); it != team_user_id_list.end(); ++it)
		{
			Role * role = m_scene->GetRoleByUID(*it);
			if(NULL == role) continue;

			role->GetRoleModuleManager()->NetSend((const void *)&my_info, sizeof(my_info));
		}
	}
}

void SpecialLogicWorldTeamArena::SendAllParticipNotice()
{
	Protocol::SCActivityWorldTeamArenaRankAllInfo all_info;
	Protocol::SCActivityWorldTeamArenaMyInfo my_info;

	std::map<long long, std::pair<int, WorldTeamArenaRankInfo> > team_info_map;	//战队信息暂存
	std::pair<int, WorldTeamArenaRankInfo> rank_info;
	all_info.resi_team_num = (int)m_scene_team_map.size();
	all_info.list_num = 0;
	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		rank_info.second = m_rank_list[i];
		if (all_info.list_num < ACTIVITY_RANK_DEFAULT_NUM_MAX)
		{
			all_info.rank_list[all_info.list_num++] = rank_info.second;
			rank_info.first = all_info.list_num;
		}
		else
		{
			rank_info.first = 0;
		}
		
		team_info_map[m_rank_list[i].team_unique_id] = rank_info;
	}

	int length = sizeof(all_info) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - all_info.list_num) * sizeof(all_info.rank_list[0]);
	if (length < 0) return;

	my_info.type = Protocol::SCActivityWorldTeamArenaMyInfo::WORLD_TEAM_ARENA_MY_INFO_TYPE_NOTICE;
	for (std::map<int, WorldTeamArenaSceneUserInfo>::const_iterator it = m_participate_user_list.begin(); it != m_participate_user_list.end(); ++it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		long long team_unique_id = WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(it->first);
		std::map<long long, std::pair<int, WorldTeamArenaRankInfo> >::iterator rank_it = team_info_map.find(team_unique_id);
		if (rank_it != team_info_map.end())
		{
			my_info.rank_posi = rank_it->second.first;
			my_info.m_info = rank_it->second.second;
		}
		else
		{
			//该战队没有排名信息(进比赛场景参与,一场没打)
			my_info.rank_posi = 0;
			my_info.m_info.Reset();
			my_info.m_info.team_unique_id = team_unique_id;
			WorldTeamArenaSignUpManager::Instance().GetTeamNameById(team_unique_id, my_info.m_info.team_name);
		}
		if (NULL == role)
		{
			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index,
				(char *)&rank_info, length, it->first);

			InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(it->first).db_index,
				(char *)&my_info, sizeof(my_info), it->first);
		}
		else
		{
			role->GetRoleModuleManager()->NetSend((const void *)&rank_info, length);
			role->GetRoleModuleManager()->NetSend((const void *)&my_info, sizeof(my_info));
		}
	}
}

void SpecialLogicWorldTeamArena::SendOpenRankInfoUser()
{
	if (m_scene_open_rank_list.empty()) return;

	Protocol::SCActivityWorldTeamArenaRankAllInfo all_info;
	all_info.resi_team_num = (int)m_scene_team_map.size();
	all_info.list_num = 0;

	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		if (all_info.list_num >= ACTIVITY_RANK_DEFAULT_NUM_MAX) break;

		all_info.rank_list[all_info.list_num++] = m_rank_list[i];
	}

	int length = sizeof(all_info) - (ACTIVITY_RANK_DEFAULT_NUM_MAX - all_info.list_num) * sizeof(all_info.rank_list[0]);
	if (length < 0) return;

	for (std::set<int>::iterator it = m_scene_open_rank_list.begin(); it != m_scene_open_rank_list.end(); it++)
	{
	
		Role * role = m_scene->GetRoleByUID(*it);
		if (NULL == role) continue;

		role->GetRoleModuleManager()->NetSend(&all_info, length);
	}
}

void SpecialLogicWorldTeamArena::SendWorldTeamArenaSpecialInfo(Role * role)
{
	Protocol::SCActivityWorldTeamArenaSpecialInfo info;
	info.resi_team_num = (int)m_scene_team_map.size();

	if (NULL == role)
	{
		for (int i = 0; i < (int)m_scene->RoleNum(); i++)
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

void SpecialLogicWorldTeamArena::WorldTeamArenaParticipAward()
{
	if(m_participate_user_list.empty()) return;

	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s ready", __FUNCTION__);
	for (std::map<int, WorldTeamArenaSceneUserInfo>::iterator it = m_participate_user_list.begin(); it != m_participate_user_list.end(); ++it)
	{
		if(0 != it->second.particip_award_flag) continue;

		gamelog::g_log_world_team_arena.printf(LL_INFO, "%s role_id[%d] ready", __FUNCTION__, it->first);
		const ActivityWorldTeamArenaParRewardCfg * par_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetParRewardCfg(it->second.level);
		if (NULL == par_cfg) return;

		static MailContentParam contentparam; contentparam.Reset();
		if (!par_cfg->reward_list.empty())
		{
			int index = 0;
			for (int award_idx = 0; award_idx < (int)par_cfg->reward_list.size() && award_idx < MAX_ATTACHMENT_ITEM_NUM; ++award_idx)
			{
				//邮件物品附件满了,先发送一封.
				if (index >= MAX_ATTACHMENT_ITEM_NUM)
				{
					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_team_arena_user_particip_subject);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_team_arena_user_particip_content);
					if (lensub > 0 && length > 0)
					{
						MailRoute::MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);
					}
					index = 0;
				}

				const ItemConfigData & award_obj = par_cfg->reward_list[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}
		}
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = par_cfg->coin;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = par_cfg->exp;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_team_arena_user_particip_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_team_arena_user_particip_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::Instance().MailToUser(it->first, MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_world_team_arena.printf(LL_INFO, "%s role_id[%d] level[%d] finish", __FUNCTION__, it->first, it->second.level);
		}
		it->second.particip_award_flag = 1;
	}

	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s finish", __FUNCTION__);
}

void SpecialLogicWorldTeamArena::WorldTeamArenaRankAward()
{
	if(m_rank_list.empty()) return;

	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s give rank reward ready", __FUNCTION__);
	for (int i = 0; i < (int)m_rank_list.size(); ++i)
	{
		int rank_posi = i + 1;
		gamelog::g_log_world_team_arena.printf(LL_INFO, "%s team[%lld, %s] rank[%d] ready", __FUNCTION__, m_rank_list[i].team_unique_id, m_rank_list[i].team_name, rank_posi);
		std::set<std::pair<int, int> > team_user_level_list;
		WorldTeamArenaSignUpManager::Instance().GetQueryTeamLevelList(m_rank_list[i].team_unique_id, team_user_level_list);
		if (team_user_level_list.empty())
		{
			continue;
		}
		static MailContentParam contentparam;
		for (std::set<std::pair<int, int> >::const_iterator it = team_user_level_list.begin(); it != team_user_level_list.end(); ++it)
		{
			const ActivityWorldTeamArenaRankRewardCfg * award_con = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetRankRewardCfg(rank_posi, (*it).second);
			if (NULL == award_con) continue;

			contentparam.Reset();
			int index = 0;
			for (int award_idx = 0; award_idx < (int)award_con->reward_list.size(); ++award_idx)
			{
				//邮件物品附件满了,先发送一封.
				if (index >= MAX_ATTACHMENT_ITEM_NUM)
				{
					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_team_arena_team_rank_subject);
					int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_team_arena_team_rank_content, 
						m_rank_list[i].team_name, rank_posi);
					if (lensub > 0 && length > 0)
					{
						MailRoute::MailToUser((*it).first, MAIL_REASON_DEFAULT, contentparam);
					}
					index = 0;
				}

				const ItemConfigData & award_obj = award_con->reward_list[award_idx];
				contentparam.item_list[index++] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}

			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_con->coin;
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_team_arena_team_rank_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_team_arena_team_rank_content, 
				m_rank_list[i].team_name, rank_posi);
				
			if (award_con->title_id > 0)
			{
				WorldTeamArenaRankManager::Instance().OnRoleTitleInfoChange((*it).first, __FUNCTION__, rank_posi, award_con->title_id);
			}
			//通过邮件发送奖励给玩家
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser((*it).first, MAIL_REASON_DEFAULT, contentparam);
				gamelog::g_log_world_team_arena.printf(LL_INFO, "%s team[%lld, %s] rank[%d] role_id[%d] finish", __FUNCTION__, m_rank_list[i].team_unique_id, 
					m_rank_list[i].team_name, rank_posi, (*it).first);
			}
		}

		//排名前十则发送通知邮件提醒玩家获得天下第一大会资格
		if (rank_posi <= ACTIVITY_WORLD_TEAM_ARENA_RANK_TOP_NUM)
		{
			contentparam.Reset();
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_team_arena_top_team_subjcet);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_team_arena_top_team_content,
				m_rank_list[i].team_name, rank_posi);
			if (lensub > 0 && length > 0)
			{
				for (std::set<std::pair<int, int> >::const_iterator it = team_user_level_list.begin(); it != team_user_level_list.end(); ++it)
				{
					MailRoute::MailToUser((*it).first, MAIL_REASON_DEFAULT, contentparam);
				}
			}
			
		}
		gamelog::g_log_world_team_arena.printf(LL_INFO, "%s team[%lld, %s] rank[%d] score[%d] Win/Battle: %d/%dfinish", __FUNCTION__, m_rank_list[i].team_unique_id, m_rank_list[i].team_name,	
			rank_posi, m_rank_list[i].total_score, m_rank_list[i].victory_times, m_rank_list[i].fight_times);
	}
	gamelog::g_log_world_team_arena.printf(LL_INFO, "%s give rank reward finish", __FUNCTION__);
}

void SpecialLogicWorldTeamArena::ForceStopWorldTeamArenaBattle()
{
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_WORLD_TEAM_ARENA);
}
