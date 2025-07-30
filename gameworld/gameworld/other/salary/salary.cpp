#include "salary.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "global/team/team.hpp"
#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "npc/npcmanager.h"
#include "scene/scenemanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/mentor/shitu.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "task/taskrecorder.h"
#include "task/taskpool.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/noticenum.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "other/miyi/rolemiyi.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/wanlingfb/wanlingfbconfig.hpp"
#include "gamelog.h"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "scene/fbmanager.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "global/team/teamconfig.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "global/guild/guild.hpp"

Salary::Salary() : m_module_mgr(NULL)
{

}

Salary::~Salary()
{

}

void Salary::Init(RoleModuleManager* mgr, const SalaryParam& param)
{
	m_module_mgr = mgr;
	m_param = param;
}

void Salary::GetParam(SalaryParam* out_param)
{
	if (NULL != out_param)
	{
		*out_param = m_param;
	}
}

void Salary::CheckSalaryVersion()
{
	const SundryConfig::ChivarousSalaryCfgMap& cfg_map = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfgMap();
	for (SundryConfig::ChivarousSalaryCfgMap::const_iterator it = cfg_map.begin(); it != cfg_map.end(); ++it)
	{
		const ChivalrousSalaryCfg& cfg = it->second;
		if (0 <= cfg.seq && cfg.seq < ARRAY_LENGTH(m_param.chivalrous_reward_list) && m_param.chivalrous_reward_list[cfg.seq].version != cfg.version)
		{
			m_param.chivalrous_reward_list[cfg.seq].Reset();
			m_param.chivalrous_reward_list[cfg.seq].version = cfg.version;
		}
	}

	this->SendAllInfo(SALARY_TYPE_CHIVAROUS_REWARD);
}

void Salary::OnWeekChange(bool type)
{
	if (type == true)
	{
		for (int i = 0; i < MAX_SALARY_INFO_NUM; ++i)
		{
			m_param.chivalrous_reward_list[i].accumulate_times = 0;
			m_param.chivalrous_reward_list[i].total_salary = 0;
		}
	}

	this->SendAllInfo(SALARY_TYPE_CHIVAROUS_REWARD);
	EventHandler::Instance().OnRoleChivalrousChange(m_module_mgr->GetRole(),0);
}

void Salary::OnWeekChange()
{
	for (int i = 0; i < MAX_SALARY_INFO_NUM; ++i)
	{
		m_param.captain_salary_list[i].Reset();
	}

	this->SendAllInfo(SALARY_TYPE_CAPTAIN_SALARY);
}

int Salary::OnRoleWinBattle(int battle_mode, int monster_group_id, bool is_stat_calc_salary)
{
	Role* role = m_module_mgr->GetRole();

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL == team) return 0;

	if (!is_stat_calc_salary)	//统筹侠义值,跳过队长工资
	{
		if (team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
		{
			const CaptainSalaryCfg* captain_cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_WIN_BATTLE, battle_mode);
			if (NULL != captain_cfg)
			{
				this->AddSalary(captain_cfg);
			}
		}
	}

	if (team->GetMemberCount() <= 1) return 0;

	// 检查侠义值
	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(SALARY_REWARD_TYPE_WIN_BATTLE, battle_mode);
	if (NULL == cfg) return 0;
	int times = 0;

	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (role->GetLevel() >= need_level && role->GetLevel() >= team->GetFollowMemberMinLevel() + level_diff)
	{
		TeamMember* member_info = team->GetMemberInfo(role);
		if (NULL != member_info && member_info->is_follow)
		{
			times++;
		}
	}

	switch (battle_mode)
	{
	case BATTLE_MODE_SHAN_HAI_BOSS:
	{
		if (role->GetRoleModuleManager()->GetShanHaiBossChallengeTime() >= LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().day_time)
		{
			times++;
		}
	}
	break;
	case BATTLE_MODE_MI_YI:
	{
		if (m_module_mgr->GetMiYi()->IsWeekKilled(monster_group_id))
		{
			times++;
		}
	}
	break;
	case BATTLE_MODE_FIELD_BOSS:
	{
		if (m_module_mgr->GetRole()->GetWildBossRewardTimes() >= LOGIC_CONFIG->GetWildBossConfig()->GetOtherCfg().get_reward_times)
		{
			times++;
		}
	}
	break;
	case BATTLE_MODE_BRAVE_GROUND:	
	{
		const SalaryCfgKey * key_cfg = LOGIC_CONFIG->GetSundryConfig()->GetSalaryCfgKeyByMonsterGroupID(monster_group_id);
		if ((NULL != key_cfg && m_module_mgr->GetCommonData().brave_ground_sao_dang_reward_level >= key_cfg->param2) || m_module_mgr->GetRoleFirstKill()->HasWeekKiilMonsterGroup(monster_group_id))
		{
			times++;
		}
	}
	break;
	case BATTLE_MODE_WAN_LING_FB:
	{
		//击杀过怪物组并且今天通关奖励次数用完
		if (m_module_mgr->GetRoleFirstKill()->HasKillMonsterGroup(monster_group_id) && m_module_mgr->GetRoleWanLingFB()->GetFBPassRewardTimes() >= LOGIC_CONFIG->GetWanLingFBConfig()->GetOtherCfg().day_pass_reward_time)
		{
			times++;
		}
	}
	break;
	default:
	{
		if (m_module_mgr->GetRoleFirstKill()->HasWeekKiilMonsterGroup(monster_group_id))
		{
			times++;
		}
	}
	break;
	}

	bool is_first_1 = true;
	for (int i = 0; i < team->GetMemberCount(); i++)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == role->GetUID()) continue;

		/*if (is_killed && is_first)
		{
			//帮助未击杀过该怪物组的玩家可以加侠义值
			if (!member->GetRoleModuleManager()->GetRoleFirstKill()->HasKillMonsterGroup(business_data.monster_group_id))
			{
				times++;
				is_first = false;
			}
		}*/

		if (is_first_1 && m_module_mgr->GetShiTu()->IsApprentice(member->GetUID()))
		{
			times++;
			is_first_1 = false;
		}
	}

	if (times > 0)
	{
		times = 1;//目前策划要求满足多个条件也是1倍
		if (is_stat_calc_salary)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
			{
				if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
				{
					times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
				}
				return cfg->reward_chivarous * times;
			}
			return 0;
		}
		this->AddSalary(cfg, times);
	}
	return 0;
}

int Salary::OnRoleClearFB(int fb_type, int param /* = 0 */, bool is_stat_calc_salary /* = false*/)
{
	Role* role = m_module_mgr->GetRole();

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL == team) return 0;

	if (!is_stat_calc_salary)
	{
		if (team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
		{
			const CaptainSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_CLEAR_FB, fb_type, param);
			if (NULL != cfg)
			{
				this->AddSalary(cfg);
			}
		}
	}

	if (team->GetMemberCount() <= 1) return 0;

	// 检查侠义值
	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(SALARY_REWARD_TYPE_CLEAR_FB, fb_type, param);
	if (NULL == cfg) return 0;

	int times = 0;
	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (role->GetLevel() >= need_level && role->GetLevel() >= team->GetFollowMemberMinLevel() + level_diff)
	{
		TeamMember* member_info = team->GetMemberInfo(role);
		if (NULL != member_info && member_info->is_follow)
		{
			times++;
		}
	}
	bool is_first_1 = true;
	for (int i = 0; i < team->GetMemberCount(); i++)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == role->GetUID()) continue;

		/*if (is_killed && is_first)
		{
		//帮助未击杀过该怪物组的玩家可以加侠义值
		if (!member->GetRoleModuleManager()->GetRoleFirstKill()->HasKillMonsterGroup(business_data.monster_group_id))
		{
		times++;
		is_first = false;
		}
		}*/

		if (is_first_1 && m_module_mgr->GetShiTu()->IsApprentice(member->GetUID()))
		{
			times++;
			is_first_1 = false;
		}
	}

	if (fb_type == FBManager::FB_TYPE_CRYSTAL_FB)
	{
		if (ROLE_CRYSTAL_FB_TYPE_JING_YING == param)
		{
			//int normal_reward_times = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().normal_reward_time;
			//if (m_module_mgr->GetRoleCrystalFb()->GetJingYingPlayTimes() > normal_reward_times)
			//{
			//	// 已用完高额次数，可以加侠义值
			//	times++;
			//}
			//条件2由次数修改为没有消耗封印石头的玩家
			SpecialLogic * special_logic = m_module_mgr->GetRole()->GetScene()->GetSpecialLogic();
			if (special_logic != NULL)
			{
				if (special_logic->GetSceneType() == Scene::SCENE_TYPE_CRYSTAL_FB)
				{
					SpecialLogicCrystalFB * crystal_logic = (SpecialLogicCrystalFB *)special_logic;

					const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(crystal_logic->GetLevelSeq());
					if (cfg != NULL && cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING)
					{
						if(!crystal_logic->IsHasConsume(role->GetUID()))
						{
							times++;
						}
					}
				}
			}
		}
		else if (ROLE_CRYSTAL_FB_TYPE_XI_YOU == param)
		{
			if (!m_module_mgr->GetRoleCrystalFb()->HasUseTimes())
			{
				//奖励次数用完,可以加侠义值
				times++;
			}
		}
	}

	if (times > 0)
	{
		times = 1;//目前策划要求满足多个条件也是1倍
		if (is_stat_calc_salary)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
			{
				if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
				{
					times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
				}
				return cfg->reward_chivarous * times;
			}
			return 0;
		}
		this->AddSalary(cfg, times);
	}

	return 0;
}

int Salary::PreCalculateChivalrousOnWinBattle(int battle_mode, int monster_group_id, const TeamItem& ti)
{
	//不需要判断队伍人数，因为是假设自己不在队伍里的
	//if (ti.cur_member_num <= 1) return 0;

	// 检查侠义值
	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(SALARY_REWARD_TYPE_WIN_BATTLE, battle_mode);
	if (NULL == cfg) return 0;

	int times = 0;
	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (m_module_mgr->GetRole()->GetLevel() >= need_level && m_module_mgr->GetRole()->GetLevel() >= ti.GetMinLevel() + level_diff)
	{
		++times;
	}

	switch (battle_mode)
	{
	case BATTLE_MODE_FIELD_BOSS:
	{
		if (m_module_mgr->GetRole()->GetWildBossRewardTimes() >= LOGIC_CONFIG->GetWildBossConfig()->GetOtherCfg().get_reward_times)
		{
			++times;
		}
	}
	break;

	case BATTLE_MODE_SHAN_HAI_BOSS:
	{
		if (m_module_mgr->GetShanHaiBossChallengeTime() >= LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().day_time)
		{
			++times;
		}
	}
	break;

	case BATTLE_MODE_MI_YI:
	{
		if (m_module_mgr->GetMiYi()->IsWeekKilled(monster_group_id))
		{
			++times;
		}
	}
	break;

	case BATTLE_MODE_BRAVE_GROUND:
	{
		const SalaryCfgKey * key_cfg = LOGIC_CONFIG->GetSundryConfig()->GetSalaryCfgKeyByMonsterGroupID(monster_group_id);
		if ((NULL != key_cfg && m_module_mgr->GetCommonData().brave_ground_sao_dang_reward_level >= key_cfg->param2) || m_module_mgr->GetRoleFirstKill()->HasWeekKiilMonsterGroup(monster_group_id))
		{
			times++;
		}
	}
	break;

	case BATTLE_MODE_WAN_LING_FB:
	{
		//击杀过怪物组并且今天通关奖励次数用完
		if (m_module_mgr->GetRoleFirstKill()->HasKillMonsterGroup(monster_group_id) && m_module_mgr->GetRoleWanLingFB()->GetFBPassRewardTimes() >= LOGIC_CONFIG->GetWanLingFBConfig()->GetOtherCfg().day_pass_reward_time)
		{
			times++;
		}
	}
	break;

	//case BATTLE_MODE_CRYSTAL_FB:
	//{
	//	int team_type_target_type = LOGIC_CONFIG->GetTeamConfig()->GetTeamTypeTargetType(ti.team_type);
	//	if (team_type_target_type == TEAM_TYPE_TARGET_TYPE_ELITE_TRAIL && !m_module_mgr->GetRoleCrystalFb()->HasUseTimes())
	//	{
	//		times++;
	//	}
	//}
	//break;

	default:
	{
		if (m_module_mgr->GetRoleFirstKill()->HasWeekKiilMonsterGroup(monster_group_id))
		{
			++times;
		}
	}
	break;
	}

	// 判断师徒关系
	for (int i = 0; i < ti.cur_member_num && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_module_mgr->GetShiTu()->IsApprentice(ti.role_info[i].uid))
		{
			++times;
			break;
		}
	}

	if (times > 0)
	{
		times = 1;//目前策划要求满足多个条件也是1倍

		if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
			{
				times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			}
			return cfg->reward_chivarous * times;
		}
	}
	return 0;
}

int Salary::PreCalculateChivalrousOnClearFB(int fb_type, int param, const TeamItem& ti, int param1)
{
	//不需要判断队伍人数，因为是假设自己不在队伍里的
	//if (ti.cur_member_num <= 1) return 0;

	// 检查侠义值
	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(SALARY_REWARD_TYPE_CLEAR_FB, fb_type, param);
	if (NULL == cfg) return 0;

	int times = 0;
	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (m_module_mgr->GetRole()->GetLevel() >= need_level && m_module_mgr->GetRole()->GetLevel() >= ti.GetMinLevel() + level_diff)
	{
		++times;
	}

	// 判断师徒关系
	for (int i = 0; i < ti.cur_member_num && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_module_mgr->GetShiTu()->IsApprentice(ti.role_info[i].uid))
		{
			++times;
			break;
		}
	}

	if (fb_type == FBManager::FB_TYPE_CRYSTAL_FB)
	{
		if (ROLE_CRYSTAL_FB_TYPE_XI_YOU == param && !m_module_mgr->GetRoleCrystalFb()->HasUseTimes())
		{
			// 已用完高额次数，可以加侠义值
			times++;
		}

		//没有对应封印石头,可以加侠义值
		if (ROLE_CRYSTAL_FB_TYPE_JING_YING == param && !m_module_mgr->GetKnapsack()->HasItem(param1, 1, false))
		{
			times++;
		}
		
		//int normal_reward_times = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().normal_reward_time;
		//if (ROLE_CRYSTAL_FB_TYPE_JING_YING == param && m_module_mgr->GetRoleCrystalFb()->GetJingYingPlayTimes() >= normal_reward_times)
		//{
		//	// 已用完高额次数，可以加侠义值
		//	times++;
		//}
	}

	if (times > 0)
	{
		times = 1;//目前策划要求满足多个条件也是1倍

		if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
			{
				times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			}
			return cfg->reward_chivarous * times;
		}
	}
	return 0;
}

int Salary::PreCalculateTeamUserCurBattleGetChivSalary(int monster_group_id, const TeamItem& ti)
{
	//不需要判断队伍人数，因为是假设自己不在队伍里的
	//if (ti.cur_member_num <= 1) return 0;

	const MonsterGroupLinkageTaskCfg* monster_cfg = LOGIC_CONFIG->GetSundryConfig()->GetMonsterGroupLinkageTaskCfg(monster_group_id);
	if (NULL == monster_cfg) return 0;

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(monster_cfg->task_id);
	if (NULL == task_cfg) return 0;

	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(monster_cfg->reward_type, task_cfg->task_type);
	if (NULL == cfg) return 0;

	int times = 0;
	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (m_module_mgr->GetRole()->GetLevel() >= need_level && m_module_mgr->GetRole()->GetLevel() >= ti.GetMinLevel() + level_diff)
	{
		++times;
	}

	// 判断师徒关系
	for (int i = 0; i < ti.cur_member_num && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		if (m_module_mgr->GetShiTu()->IsApprentice(ti.role_info[i].uid))
		{
			++times;
			break;
		}
	}

	bool is_finish = m_module_mgr->GetTaskRecorder()->HaveFinished(monster_cfg->task_id); // 简化逻辑，仅判断自己有没有完成任务
	if (is_finish)
	{
		++times;
	}

	if (times > 0)
	{
		times = 1;//目前策划要求满足多个条件也是1倍

		if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
			{
				times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			}
			return cfg->reward_chivarous * times;
		}
	}
	return 0;
}

void Salary::OnRoleWinTaskMonsterGroup(int monster_group_id)
{
	Role* role = m_module_mgr->GetRole();

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
	{
		const CaptainSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_KILL_TASK_MONSTER_GROUP, monster_group_id);
		if (NULL != cfg)
		{
			this->AddSalary(cfg);
		}
	}
}

void Salary::OnRolePlayDemonCave(int beat_wave)
{
	Role* role = m_module_mgr->GetRole();
	if (beat_wave <= 0) return;	//通过第一波就给

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
	{
		const CaptainSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_DEMON_CAVE_WAVE, 0, 0);
		if (NULL != cfg)
		{
			this->AddSalary(cfg, 1);
		}
	}
}

void Salary::OnRoleClearFBOneLayer(int fb_type, int param)
{
	Role* role = m_module_mgr->GetRole();

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
	{
		const CaptainSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_CLEAR_ONE_FB_LAYER, fb_type, param);
		if (NULL != cfg)
		{
			this->AddSalary(cfg);
		}
	}
}

void Salary::OnRoleKillWildBoss()
{
	Role* role = m_module_mgr->GetRole();

	// 检查队长工资
	Team* team = role->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == role->GetUID() && team->GetMemberCount() >= SALARY_MIN_TEAM_MEMBER)
	{
		const CaptainSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfg(SALARY_REWARD_TYPE_KILL_WILD_BOSS, 0, 0);
		if (NULL != cfg)
		{
			this->AddSalary(cfg);
		}
	}
}

void Salary::OnRoleKillMonsterGroup(int monster_group_id)
{
	const MonsterGroupLinkageTaskCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetMonsterGroupLinkageTaskCfg(monster_group_id);
	if (NULL == cfg) return;

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(cfg->task_id);
	if (NULL == task_cfg) return;

	this->OnRoleTaskMonsterSucc(cfg->task_id, cfg->reward_type, task_cfg->task_type);
}

void Salary::OnRoleTaskMonsterSucc(int task_id, int reward_type, int task_type)
{
	Role* role = m_module_mgr->GetRole();
	if (NULL == role) return;

	// 检查侠义值
	int times = 0;
	Team* team = role->GetMyTeam();
	if (NULL == team) return;

	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(reward_type, task_type);
	if (NULL == cfg) return;

	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (role->GetLevel() >= need_level && role->GetLevel() >= team->GetFollowMemberMinLevel() + level_diff)
	{
		TeamMember* member_info = team->GetMemberInfo(role);
		if (NULL != member_info && member_info->is_follow)
		{
			times++;
		}
	}

	bool is_finish = m_module_mgr->GetTaskRecorder()->HaveFinished(task_id);
	bool is_have_role_not_finish = false;
	bool is_first = true;
	for (int i = 0; i < team->GetMemberCount(); i++)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == role->GetUID()) continue;

		if (is_first && m_module_mgr->GetShiTu()->IsApprentice(member->GetUID()))
		{
			times++;
			is_first = false;
		}
		if (is_finish)
		{
			if (!member->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(task_id))
			{
				is_have_role_not_finish = true;
			}
		}
	}
	if (is_have_role_not_finish)
	{
		times++;
	}
	if (times > 0)
	{
		this->AddSalary(cfg, 1);
	}
}

bool Salary::IsTaskMonsterGroup(int monster_group_id, int reward_type, int* task_id)
{
	const MonsterGroupLinkageTaskCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetMonsterGroupLinkageTaskCfg(monster_group_id);
	if (NULL == cfg || reward_type != cfg->reward_type) return false;

	if (NULL != task_id)
	{
		*task_id = cfg->task_id;
	}
	return true;
}

int Salary::CalcTeamUserCurBattleGetChivSalary(int monster_group_id)
{
	const MonsterGroupLinkageTaskCfg* monster_cfg = LOGIC_CONFIG->GetSundryConfig()->GetMonsterGroupLinkageTaskCfg(monster_group_id);
	if (NULL == monster_cfg) return 0;

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(monster_cfg->task_id);
	if (NULL == task_cfg) return 0;

	Team* team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team || team->GetMemberCount() <= 1) return 0;

	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(monster_cfg->reward_type, task_cfg->task_type);
	if (NULL == cfg) return 0;

	int times = 0;
	int need_level = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level;
	int level_diff = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().chivarous_salary_need_level_diff;
	if (m_module_mgr->GetRole()->GetLevel() >= need_level && m_module_mgr->GetRole()->GetLevel() >= team->GetFollowMemberMinLevel() + level_diff)
	{
		TeamMember* member_info = team->GetMemberInfo(m_module_mgr->GetRole());
		if (NULL != member_info && member_info->is_follow)
		{
			times++;
		}
	}
	bool is_finish = m_module_mgr->GetTaskRecorder()->HaveFinished(monster_cfg->task_id);
	bool is_have_role_not_finish = false;
	bool is_first = true;
	for (int i = 0; i < team->GetMemberCount(); i++)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member || member->GetUID() == m_module_mgr->GetRole()->GetUID()) continue;

		if (is_first && m_module_mgr->GetShiTu()->IsApprentice(member->GetUID()))
		{
			times++;
			is_first = false;
		}
		if (is_finish)
		{
			if (!member->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(monster_cfg->task_id))
			{
				is_have_role_not_finish = true;
			}
		}
	}
	if (is_have_role_not_finish)
	{
		times++;
	}
	if (times > 0)
	{
		//目前策划要求满足多个条件也是1倍
		times = 1;
		if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
		{
			if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
			{
				times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			}
			return cfg->reward_chivarous * times;
		}
	}
	return 0;
}

void Salary::SendDaySaveSalary(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)return;

	m_param.chivalrous_salary = 0;
	m_param.captain_salary = 0;

	Protocol::SCRoleSalaryDay info;
	info.salary_type = 0;
	info.day_value = m_param.captain_salary;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));

	info.salary_type = 1;
	info.day_value = m_param.chivalrous_salary;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}

void Salary::SendTodaySalary()
{
	Protocol::SCRoleSalaryDay info;
	info.salary_type = 0;
	info.day_value = m_param.captain_salary;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));

	info.salary_type = 1;
	info.day_value = m_param.chivalrous_salary;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}

void Salary::OnBattleStartStatSalaryInfo(Protocol::SCSalaryBattleStatInfo& info, int monster_group_id)
{
	const ChivalrousSalaryCfg* chivalrous_salary_cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfgByMonsterGroup(monster_group_id);
	if (NULL == chivalrous_salary_cfg) return;

	info.seq = chivalrous_salary_cfg->seq;
	info.salary = this->CalculateChivalrousByMonsterGroupID(monster_group_id);
}

int Salary::CalculateChivalrousByMonsterGroupID(int monster_group_id)
{
	const ChivalrousSalaryCfg* chivalrous_salary_cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfgByMonsterGroup(monster_group_id);
	if (NULL == chivalrous_salary_cfg) return 0;

	//目前给侠义值的只有奖励类型0, 1, 6, 7, 8,
	switch (chivalrous_salary_cfg->reward_type)
	{
	case SALARY_REWARD_TYPE_WIN_BATTLE:
	{
		return this->OnRoleWinBattle(chivalrous_salary_cfg->param, monster_group_id, true);
	}
	break;
	case SALARY_REWARD_TYPE_CLEAR_FB:
	{
		return this->OnRoleClearFB(chivalrous_salary_cfg->param, chivalrous_salary_cfg->param2, true);
	}
	break;
	case SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC:
	case SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC:
	case SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC:
	{
		return this->CalcTeamUserCurBattleGetChivSalary(monster_group_id);
	}
	break;
	default:
		break;
	}

	return 0;
}

int Salary::CalculateChivalrousByTeamType(int team_type, const TeamItem& ti)
{
	const ChivalrousTeamInfoCfg * chivalrous_team_cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousMonsterGroupByTeamType(team_type);
	if (NULL == chivalrous_team_cfg)
	{
		return 0;
	}

	const ChivalrousSalaryCfg* chivalrous_salary_cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfgByMonsterGroup(chivalrous_team_cfg->monster_group_id);
	if (NULL == chivalrous_salary_cfg) return 0;

	//目前给侠义值的只有奖励类型0, 1, 6, 7, 8,
	switch (chivalrous_salary_cfg->reward_type)
	{
	case SALARY_REWARD_TYPE_WIN_BATTLE:
	{
		return this->PreCalculateChivalrousOnWinBattle(chivalrous_salary_cfg->param, chivalrous_team_cfg->monster_group_id, ti);
	}
	break;
	case SALARY_REWARD_TYPE_CLEAR_FB:
	{
		return this->PreCalculateChivalrousOnClearFB(chivalrous_salary_cfg->param, chivalrous_salary_cfg->param2, ti, chivalrous_team_cfg->need_item_id);
	}
	break;
	case SALARY_REWARD_TYPE_KILL_CHALLENGE_TASK_MONSTER_SUCC:
	case SALARY_REWARD_TYPE_KILL_BRANCH_TASK_MONSTER_SUCC:
	case SALARY_REWARD_TYPE_KILL_ADVANCED_TASK_MONSTER_SUCC:
	{
		return this->PreCalculateTeamUserCurBattleGetChivSalary(chivalrous_team_cfg->monster_group_id, ti);
	}
	break;
	default:
		break;
	}

	return 0;
}

void Salary::SendAllInfo(int salary_type)
{
	if (salary_type != SALARY_TYPE_CAPTAIN_SALARY && salary_type != SALARY_TYPE_CHIVAROUS_REWARD) return;

	static Protocol::SCRoleSalaryInfo info;
	info.reason = Protocol::SCRoleSalaryInfo::REASON_TYPE_NORMAL;
	info.salary_type = salary_type;
	info.salary_list_size = 0;
	info.total_value = 0;

	if (SALARY_TYPE_CAPTAIN_SALARY == salary_type)
	{
		const SundryConfig::CaptainSalaryCfgMap& cfg_map = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfgMap();
		SundryConfig::CaptainSalaryCfgMap::const_iterator it = cfg_map.begin();

		for (; it != cfg_map.end() && info.salary_list_size < MAX_SALARY_INFO_NUM; ++it)
		{
			const CaptainSalaryCfg* cfg = &it->second;
			info.salary_list[info.salary_list_size].seq = cfg->seq;
			info.salary_list[info.salary_list_size].times = m_param.captain_salary_list[cfg->seq].accumulate_times;
			info.salary_list[info.salary_list_size].total_salary = m_param.captain_salary_list[cfg->seq].total_salary;
			info.salary_list_size += 1;

			info.total_value += m_param.captain_salary_list[cfg->seq].total_salary;
		}
	}
	else if (SALARY_TYPE_CHIVAROUS_REWARD == salary_type)
	{
		const SundryConfig::ChivarousSalaryCfgMap& cfg_map = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfgMap();
		SundryConfig::ChivarousSalaryCfgMap::const_iterator it = cfg_map.begin();

		for (; it != cfg_map.end() && info.salary_list_size < MAX_SALARY_INFO_NUM; ++it)
		{
			const ChivalrousSalaryCfg* cfg = &it->second;
			info.salary_list[info.salary_list_size].seq = cfg->seq;
			info.salary_list[info.salary_list_size].times = m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			info.salary_list[info.salary_list_size].total_salary = m_param.chivalrous_reward_list[cfg->seq].total_salary;
			info.salary_list_size += 1;
		}

		info.total_value = m_module_mgr->GetMoney()->GetOtherCoin(MONEY_TYPE_CHIVALROUS);
	}

	int send_len = sizeof(info) - sizeof(info.salary_list) + info.salary_list_size * sizeof(info.salary_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, send_len);
}

void Salary::GmAddSalary(Int64 count)
{
	m_module_mgr->GetMoney()->AddOtherCoin(count, MONEY_TYPE_CHIVALROUS, __FUNCTION__);
	this->SendAllInfo(SALARY_TYPE_CHIVAROUS_REWARD);
}

void Salary::GMChangeSalaryProgress(int reward_type, int param, int param2, int times)
{
	const ChivalrousSalaryCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousSalaryCfg(reward_type, param, param2);
	if (NULL == cfg) return;

	this->AddSalary(cfg, times);
}

int Salary::GetWeekTotalChivalrous()
{
	int total_chivalrous = 0;
	for (int i = 0; i < MAX_SALARY_INFO_NUM; ++i)
	{
		total_chivalrous += m_param.chivalrous_reward_list[i].total_salary;
	}
	return total_chivalrous;
}

void Salary::OnFetchHistoryChivalrousReward(int reward_seq)
{
	const ChivalrousAchievementCfg* cfg = LOGIC_CONFIG->GetSundryConfig()->GetChivalrousAchieveCfg(reward_seq);
	if (NULL == cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != (m_param.history_chivalrous_reward_flag & (1 << reward_seq)))
	{
		m_module_mgr->NoticeNum(errornum::EN_CHIVALROUS_REWARD_ALREADY_FETCHED);
	}

	if (m_param.history_chivalrous < cfg->need_chivalrous_value)
	{
		m_module_mgr->NoticeNum(errornum::EN_HISTORY_CHIVALROUS_NOT_ENOUGH);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_num, cfg->item_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->PutList(cfg->reward_num, cfg->item_list, PUT_REASON_CHIVALROUS_ACHIEVE))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.history_chivalrous_reward_flag |= (1 << reward_seq);
	this->SendHistoryChivalrousInfo();

	gamelog::g_log_salary.printf(LL_INFO, "Salary::OnFetchHistoryChivalrousReward role[%d,%s] reward_seq[%d]", m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.history_chivalrous_reward_flag);
}

void Salary::SendHistoryChivalrousInfo()
{
	Protocol::SCRoleHistoryChivalrous msg;
	msg.chivalrous_reward_flag = m_param.history_chivalrous_reward_flag;
	msg.history_chivalrous = m_param.history_chivalrous;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}

void Salary::SendUpdateInfo(int salary_type, int seq)
{
	if (salary_type != SALARY_TYPE_CAPTAIN_SALARY && salary_type != SALARY_TYPE_CHIVAROUS_REWARD) return;
	if (seq < 0 || seq >= MAX_SALARY_INFO_NUM) return;

	static Protocol::SCRoleSalaryInfo info;
	info.reason = Protocol::SCRoleSalaryInfo::REASON_TYPE_UPDATE;
	info.salary_type = salary_type;
	info.salary_list_size = 1;
	info.total_value = 0;

	if (SALARY_TYPE_CAPTAIN_SALARY == salary_type)
	{
		info.salary_list[0].seq = seq;
		info.salary_list[0].times = m_param.captain_salary_list[seq].accumulate_times;
		info.salary_list[0].total_salary = m_param.captain_salary_list[seq].total_salary;

		const SundryConfig::CaptainSalaryCfgMap& cfg_map = LOGIC_CONFIG->GetSundryConfig()->GetCaptainSalaryCfgMap();
		SundryConfig::CaptainSalaryCfgMap::const_iterator it = cfg_map.begin();

		for (; it != cfg_map.end() && info.salary_list_size < MAX_SALARY_INFO_NUM; ++it)
		{
			const CaptainSalaryCfg* cfg = &it->second;
			info.total_value += m_param.captain_salary_list[cfg->seq].total_salary;
		}
	}
	else if (SALARY_TYPE_CHIVAROUS_REWARD == salary_type)
	{
		info.salary_list[0].seq = seq;
		info.salary_list[0].times = m_param.chivalrous_reward_list[seq].accumulate_times;
		info.salary_list[0].total_salary = m_param.chivalrous_reward_list[seq].total_salary;

		info.total_value = m_module_mgr->GetMoney()->GetOtherCoin(MONEY_TYPE_CHIVALROUS);
	}

	int send_len = sizeof(info) - sizeof(info.salary_list) + info.salary_list_size * sizeof(info.salary_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&info, send_len);
}

void Salary::AddSalary(const CaptainSalaryCfg* cfg, int times /* = 1 */)
{
	if (NULL == cfg || times <= 0) return;

	if (m_param.captain_salary_list[cfg->seq].accumulate_times < cfg->week_limit_times)
	{
		if (m_param.captain_salary_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
		{
			times = cfg->week_limit_times - m_param.captain_salary_list[cfg->seq].accumulate_times;
			m_param.captain_salary_list[cfg->seq].accumulate_times = cfg->week_limit_times;
		}
		else
		{
			m_param.captain_salary_list[cfg->seq].accumulate_times += times;
		}
		m_param.captain_salary_list[cfg->seq].total_salary += cfg->reward_coin * times;
		m_param.captain_salary += cfg->reward_coin * times;

		//直接给
		m_module_mgr->GetMoney()->AddOtherCoin((Int64)cfg->reward_coin * times, cfg->reward_coin_type, __FUNCTION__, false, true);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_salary_captain_salary, cfg->reward_coin * times);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void*)&sm, sendlen);
			}
		}
		m_module_mgr->SendSpecialNotice(Protocol::SCSpecialNotice::SPECIAL_NOTICE_TYPE_CAPTAIN_SALARY, cfg->reward_coin * times);

		this->SendTodaySalary();
		this->SendUpdateInfo(SALARY_TYPE_CAPTAIN_SALARY, cfg->seq);
	}
}

void Salary::AddSalary(const ChivalrousSalaryCfg* cfg, int times /* = 1 */)
{
	if (NULL == cfg || times <= 0) return;

	if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times < cfg->week_limit_times)
	{
		if (m_param.chivalrous_reward_list[cfg->seq].accumulate_times + times > cfg->week_limit_times)
		{
			times = cfg->week_limit_times - m_param.chivalrous_reward_list[cfg->seq].accumulate_times;
			m_param.chivalrous_reward_list[cfg->seq].accumulate_times = cfg->week_limit_times;
		}
		else
		{
			m_param.chivalrous_reward_list[cfg->seq].accumulate_times += times;
		}

		m_param.chivalrous_reward_list[cfg->seq].total_salary += cfg->reward_chivarous * times;
		m_param.chivalrous_salary += cfg->reward_chivarous * times;
		m_param.history_chivalrous += cfg->reward_chivarous * times;
		//直接给
		m_module_mgr->GetMoney()->AddOtherCoin(cfg->reward_chivarous * times, MONEY_TYPE_CHIVALROUS, __FUNCTION__, false, true);
		Role * role = m_module_mgr->GetRole();
		Team* team = m_module_mgr->GetRole()->GetMyTeam();
		if (NULL != role && NULL != team)
		{
			if (team->GetMemberCountExcludeRobot() > 1 && role->GetGuildID() == team->GetAllTeamMemberGuildID())
			{
				Guild * m_guild = role->GetGuild();
				if (NULL != m_guild)
				{
					m_guild->OnRankValueAdd(role, Guild::HALLOFFAME_TYPE_KINDER, cfg->reward_chivarous * times);
				}
			}
		}
		
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_salary_chivalrous_salary, cfg->reward_chivarous * times);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void*)&sm, sendlen);
			}
		}

		this->SendTodaySalary();
		this->SendUpdateInfo(SALARY_TYPE_CHIVAROUS_REWARD, cfg->seq);
		this->SendHistoryChivalrousInfo();
		EventHandler::Instance().OnRoleChivalrousChange(m_module_mgr->GetRole(),times);
	}
}
