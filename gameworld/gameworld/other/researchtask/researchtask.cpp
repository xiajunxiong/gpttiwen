#include "researchtask.h"
#include "researchtaskconfig.hpp"
#include "world.h"
#include "protocol/msgresearchtask.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "other/dayreward/dayreward.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "protocol/msgtask.h"
#include "gamelog.h"
#include "global/worldstatus/worldstatus.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"

ResearchTask::ResearchTask()
{
}

ResearchTask::~ResearchTask()
{
}

void ResearchTask::Init(RoleModuleManager * mgr, const ResearchTaskParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void ResearchTask::GetInitParam(ResearchTaskParam * param)
{
	*param = m_param;
}

void ResearchTask::Update(time_t now_second)
{
}

void ResearchTask::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;
	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHENZHONG_RESERCH)) return;
	this->OnFindDataReset();
	m_param.today_finish_times = 0;
	m_param.m_add_exp_per_notice = 0;
	m_param.today_reward_fetch = 0;
	this->SendResearchTaskInfo();

	//m_mgr->GetDayReward()->SendFindRewardAndNotifysInfo();
}

void ResearchTask::OnFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_ZHEN_ZHONG);
	if (NULL == cfg || m_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	int role_create_day = m_mgr->GetCreateDays();

	const FunOpenCfg* fun_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_ZHENZHONG_RESERCH);
	if (role_create_day - 1 < fun_cfg->limit_open_day)return;//前一天三界除魔未开启

	if (m_mgr->GetCommonData().offine_day < 2)
	{
		if (rt_other.max_daily_task_times >= m_param.today_finish_times)
		{
			m_param.residual_reward_back_times = rt_other.max_daily_task_times - m_param.today_finish_times;
		}
		else
		{
			m_param.residual_reward_back_times = 0;
		}
	}
	else
	{
		m_param.residual_reward_back_times = rt_other.max_daily_task_times;
	}
}

void ResearchTask::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void ResearchTask::OnResearchTaskRoleLogin()
{
	Team * team = m_mgr->GetRole()->GetMyTeam();	// 如果玩家没有队伍，或者队长没有真中任务，则刷新任务
	if (NULL != team)
	{
		Role * leader = team->GetLeaderRole();
		if (NULL != leader && !leader->GetRoleModuleManager()->GetResearchTask()->IsHaveResearchTask())
		{
			m_param.task_info.Reset();
		}
	}
	else
	{
		m_param.task_info.Reset();
	}

	this->SendResearchTaskInfo();
}

bool ResearchTask::OnResearchTaskFindReward(int consume_type, int count)
{
	if (count > m_param.residual_reward_back_times)
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const AwardBackCon * reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->GetRewardBack(m_mgr->GetRole()->GetLevel(), consume_type);
	if (NULL == reward_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
		return false;
	}

	// 摇道具
	std::vector<ItemConfigData> item_list;
	for (int i = 0; i < count; ++i)		// 在这里写循环是为了实现『每次找回获取的道具都有可能不同』的逻辑；
	{
		const ItemConfigData * item = LOGIC_CONFIG->GetResearchTaskConfig()->RandBackItem(reward_cfg->item_group);
		if (NULL != item)
		{
			item_list.push_back(*item);
		}
	}
	if (!item_list.empty() && reward_cfg->item_group != -1)
	{
		if (!m_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
		if (!m_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}
	if (NULL != m_mgr->GetRole()->GetGuild())
	{
		m_mgr->GetMoney()->AddOtherCoin(count * reward_cfg->family_contribution, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_mgr->GetMoney()->AddCoinBind(count * reward_cfg->coin_bind, __FUNCTION__);
	m_mgr->GetRole()->AddExp(count * reward_cfg->experience, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);
	m_mgr->GetLevelComplement()->OnAddExp(count * reward_cfg->experience);

	m_param.residual_reward_back_times -= count;
	m_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_ZHEN_ZHONG] += count;
	m_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_ZHEN_ZHONG);
	EventHandler::Instance().OnResearchTask(m_mgr->GetRole(), count, true);
	return true;
}

void ResearchTask::OnRoleEnterTeam()
{
	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	if (m_param.today_finish_times >= rt_other.max_daily_task_times) return;

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHENZHONG_RESERCH)) return;

	Team * team = TeamManager::Instance().GetTeamByUid(m_mgr->GetRole()->GetUID());
	if (NULL == team) return;
	
	Role * leader = team->GetLeaderRole();
	if (NULL == leader || !leader->GetRoleModuleManager()->GetResearchTask()->IsHaveResearchTask()) return;
	
	leader->GetRoleModuleManager()->GetResearchTask()->OnGainTaskInfo(&m_param.task_info);
	this->SendResearchTaskInfo();
}

void ResearchTask::OnRoleRejoinTeam()
{
	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	if (m_param.today_finish_times >= rt_other.max_daily_task_times) return;

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHENZHONG_RESERCH)) return;

	Team * team = TeamManager::Instance().GetTeamByUid(m_mgr->GetRole()->GetUID());
	if (NULL == team) return;

	Role * leader = team->GetLeaderRole();
	if (NULL == leader || !leader->GetRoleModuleManager()->GetResearchTask()->IsHaveResearchTask()) return;

	leader->GetRoleModuleManager()->GetResearchTask()->OnGainTaskInfo(&m_param.task_info);
	this->SendResearchTaskInfo();
}

void ResearchTask::OnRoleLeaveTeam()
{
	if (!this->IsHaveResearchTask()) return;

	m_param.task_info.Reset();
	this->SendResearchTaskInfo();
}

void ResearchTask::OnGainTaskInfo(ResearchTaskInfo * task_con)
{
	*task_con = m_param.task_info;
}

void ResearchTask::OnSyncTaskInfo(const ResearchTaskInfo & task_con)
{
	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHENZHONG_RESERCH))
	{
		return;
	}
	if (0 != task_con.task_id && m_param.today_finish_times >= rt_other.max_daily_task_times)
	{
		if(m_param.task_info.task_id != 0) m_param.task_info.Reset();
		this->SendResearchTaskInfo();
		return;
	}

	m_param.task_info = task_con;
	this->SendResearchTaskInfo(true);
}

void ResearchTask::OnAcceptResearchTask()
{
	if (this->IsHaveResearchTask())
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
		return;
	}

	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();

	Team * team = TeamManager::Instance().GetTeamByUid(m_mgr->GetRole()->GetUID());
	if (NULL == team || team->GetMemberCount() < rt_other.team_need_member_min)
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_TASK_CONDI_NEED_MORES);
		return;
	}

	if (m_mgr->GetRole()->GetUID() != team->GetLeaderUID())
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}

	if (m_param.today_finish_times >= rt_other.max_daily_task_times)
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_TASK_TIME_ALREADY_MAX);
		return;
	}

	int average_level = 0, true_mem_num = 0, mem_count = team->GetMemberCount();
	for (int mem_cur = 0; mem_cur < mem_count; ++mem_cur)
	{
		Role * mem_role = team->GetMemberRoleByIndex(mem_cur);
		if (NULL == mem_role) continue;

		int role_level = mem_role->GetLevel();
		if (!FunOpen::IsFunOpen(mem_role, FUN_TYPE_ZHENZHONG_RESERCH, false))
		{
			m_mgr->GetRole()->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
			return;
		}

		++true_mem_num;
		average_level += role_level;
	}

	if (true_mem_num <= 0) return;

	average_level /= true_mem_num;
	if (!this->RandTaskInfoOnce(average_level, &m_param.task_info))
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	
	for (int i = 0; i < mem_count; ++i)
	{
		Role * mem_role = team->GetMemberRoleByIndex(i);
		if (NULL == mem_role) continue;

		mem_role->GetRoleModuleManager()->GetResearchTask()->OnSyncTaskInfo(m_param.task_info);
	}
}

void ResearchTask::OnCommitResearchTask()
{
	if (!this->IsHaveResearchTask()) return;

	Team * team = TeamManager::Instance().GetTeamByUid(m_mgr->GetRole()->GetUID());
	if (NULL == team) return;	// 离开队伍任务就会被刷掉，所以这里无队伍时认为其无任务而不可提交。

	if (m_mgr->GetRole()->GetUID() != team->GetLeaderUID())
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return;
	}
	
	if (m_param.task_info.progress < RESEARCH_TASK_TIMES_PER_ROUND)
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
		return;
	}
	
	int mem_count = team->GetMemberCount(); m_param.task_info.Reset();
	for (int i = 0; i < mem_count; ++i)
	{
		Role * mem_role = team->GetMemberRoleByIndex(i);
		if (NULL == mem_role) continue;

		mem_role->GetRoleModuleManager()->GetResearchTask()->OnSyncTaskInfo(m_param.task_info);
	}
}

void ResearchTask::OnOneKeyFinishZhenZhong()
{
	Money * money = m_mgr->GetMoney();
	if (NULL == money) return;

	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	int resi_times = rt_other.max_daily_task_times - m_param.today_finish_times;
	int buy_times = GetMin(resi_times, rt_other.one_key_auto_max_times);
	if (buy_times <= 0) return;
	
	const  AwardBackCon *abc = LOGIC_CONFIG->GetResearchTaskConfig()->GetFindType(m_mgr->GetRole()->GetLevel());if (!abc) return;
	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_ZHEN_ZHONG); if (!dr_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_ZHEN_ZHONG, m_mgr->GetRole()->GetLevel());
	if (NULL == price_cfg) return;

	int consume_money = 0;
	int consume_type = abc->find_type;
	if (consume_type != RB_TYPE_COIN) return;

	switch (consume_type)
	{
	case RB_TYPE_COIN:
		consume_money = (price_cfg->coin_price * buy_times * price_cfg->coin_scale) / 100;
		if (!money->CoinBindMoreThan(consume_money))		// 检测
		{
			m_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}

		if (!money->UseCoinBind(consume_money, __FUNCTION__)) return;	// 消耗
		break;

/*	case RB_TYPE_GOLD:
		consume_money = buy_times * dr_cfg.gold_price;

		if (!money->GoldMoreThan(consume_money))		// 检测
		{
			m_mgr->GetRole()->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}

		if (!money->UseGold(consume_money, __FUNCTION__)) return;	// 消耗

		break;*/
	default:
		break;
	}


	
	if (this->IsHaveResearchTask())		// 如果有任务，先把任务刷掉
	{
		m_param.task_info.Reset();
	}

	m_param.today_finish_times += buy_times;

	const ResearchRewardCfg * reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->GetTaskReward(m_mgr->GetRole()->GetLevel());
	if (NULL != reward_cfg)
	{
		if (reward_cfg->experience > 0)
		{
			m_mgr->GetRole()->AddExp(buy_times * reward_cfg->experience, ADD_EXP_TYPE_TASK, "ResearchTaskExpReward");
			m_mgr->GetLevelComplement()->OnAddExp(buy_times * reward_cfg->experience);
			EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), buy_times * reward_cfg->experience, __FUNCTION__);
		}

		//if (reward_cfg->coin_bind > 0) m_mgr->GetMoney()->AddCoinBind(buy_times * reward_cfg->coin_bind, "ResearchTaskCoinReward");
	}

	// 奖励活力值
	/*int gather_award = buy_times * rt_other.gather_power_award;
	m_mgr->GetRoleGatherFb()->AddGatherPower(gather_award);*/

	this->CheckCanGainAward(true);
	this->SendResearchTaskInfo();
	EventHandler::Instance().OnResearchTask(m_mgr->GetRole(), buy_times);
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "ResearchTask");
	m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_RESEARCH_TASK);
}

void ResearchTask::OnDefeatMonsterGroup()
{
	if (!this->IsHaveResearchTask()) return;
	
	const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	if (m_param.today_finish_times >= rt_other.max_daily_task_times) return;

	++m_param.today_finish_times;
	++m_param.task_info.progress;
	m_param.task_info.Reset();

	const ResearchRewardCfg * reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->GetTaskReward(m_mgr->GetRole()->GetLevel());
	if (NULL != reward_cfg)
	{
		if (reward_cfg->experience > 0)
		{
			m_param.m_add_exp_per_notice += m_mgr->GetRole()->AddExp(reward_cfg->experience, ADD_EXP_TYPE_TASK, "ResearchTaskExpReward");
			m_mgr->GetLevelComplement()->OnAddExp(reward_cfg->experience);
			EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), reward_cfg->experience, __FUNCTION__);
		}
		if (reward_cfg->coin_bind > 0) m_mgr->GetMoney()->AddCoinBind(reward_cfg->coin_bind, "ResearchTaskCoinReward");
	}

	const std::vector<RTRoundRewardCon> * round_reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->RandRewardListRoundCfg(m_param.today_finish_times, m_mgr->GetRole()->GetLevel());
	if (round_reward_cfg != NULL)
	{
		Protocol::SCResearchTaskNoticInfo info;
		info.reward_num = 0;
		for (std::vector<RTRoundRewardCon>::const_iterator it = round_reward_cfg->begin(); it != round_reward_cfg->end(); it++)
		{
			int rate_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			if (rate_num <= it->rate)//奖励给
			{
				const RTRoundRewardCfg *  round_reward_cfg2 = LOGIC_CONFIG->GetResearchTaskConfig()->RandRoundRewardCfg(it->reward_group);
				if (round_reward_cfg2 != NULL)
				{
					if (m_mgr->GetKnapsack()->CheckForPut(round_reward_cfg2->reward.item_id, round_reward_cfg2->reward.num))
					{
						m_mgr->GetKnapsack()->Put(round_reward_cfg2->reward, PUT_REASON_RESEARCH_TASK_REWARD);
					}
					else
					{
						m_mgr->GetKnapsack()->SendMail(&round_reward_cfg2->reward, 1, SNED_MAIL_TYPE_DEFAULT);
					}

					if (info.reward_num < ARRAY_ITEM_COUNT(info.reward_list))
					{
						info.reward_list[info.reward_num].item_id = round_reward_cfg2->reward.item_id;
						info.reward_list[info.reward_num].is_bind = round_reward_cfg2->reward.is_bind ? 1 : 0;
						info.reward_list[info.reward_num].num = (unsigned short)round_reward_cfg2->reward.num;
						info.reward_num += 1;
					}
				}
			}
		}

		info.add_exp = m_param.m_add_exp_per_notice;
		m_mgr->NetSend((const char*)&info, sizeof(info));
		m_param.m_add_exp_per_notice = 0;
	}

	const ResearchTaskOtherCfg & zzt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
	m_mgr->GetRoleGatherFb()->AddGatherPower(zzt_other.gather_power_award);

	this->CheckCanGainAward();
	if (m_param.task_info.progress >= RESEARCH_TASK_TIMES_PER_ROUND) return;	// 如果进度满 10 次，就得找真中提交任务了

	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "ResearchTask");

	if (1 == m_param.today_finish_times) // 今天第一次则记录
	{
		m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_RESEARCH_TASK);
	}
}

void ResearchTask::TeamLeaderSyncTaskAfterBattle()
{
	Team * team = TeamManager::Instance().GetTeamByUid(m_mgr->GetRole()->GetUID());	// 随机下一次任务
	if (NULL != team && m_mgr->GetRole()->GetUID() == team->GetLeaderUID())
	{
		int average_level = 0, true_mem_num = 0, mem_count = team->GetMemberCount();
		for (int mem_cur = 0; mem_cur < mem_count; ++mem_cur)
		{
			Role * mem_role = team->GetMemberRoleByIndex(mem_cur);
			if (NULL == mem_role) continue;

			int role_level = mem_role->GetLevel();
			if (!FunOpen::IsFunOpen(mem_role, FUN_TYPE_ZHENZHONG_RESERCH, false))
			{
				m_mgr->GetRole()->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
				continue;
			}

			++true_mem_num;
			average_level += role_level;
		}

		if (true_mem_num <= 0)
		{
			return;
		}

		const ResearchTaskOtherCfg & rt_other = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig();
		average_level /= true_mem_num;
		if (m_param.today_finish_times >= rt_other.max_daily_task_times || !this->RandTaskInfoOnce(average_level, &m_param.task_info))
		{
			m_param.task_info.Reset();
		}

		for (int i = 0; i < mem_count; ++i)
		{
			Role * mem_role = team->GetMemberRoleByIndex(i);
			if (NULL == mem_role) continue;

			mem_role->GetRoleModuleManager()->GetResearchTask()->OnSyncTaskInfo(m_param.task_info);
		}
	}
}

bool ResearchTask::HasFinishAllToday() const
{
	return m_param.today_finish_times >= 
		LOGIC_CONFIG->GetResearchTaskConfig()
		->GetOtherConfig().max_daily_task_times;
}

void ResearchTask::OnEnterScene(int scene_id)
{
	if (this->IsHaveResearchTask() && scene_id == m_param.task_info.scene_id) this->SendResearchTaskInfo();
}

void ResearchTask::SendResearchTaskInfo(bool is_new)
{
	static Protocol::SCResearchTaskMessage rtm;

	rtm.today_finish_times = m_param.today_finish_times;
	rtm.is_new = is_new ? 1 : 0;
	rtm.task_info = m_param.task_info;

	m_mgr->NetSend((const char *)&rtm, sizeof(rtm));
}

bool ResearchTask::IsCanOneClick()
{
	return m_param.today_reward_fetch == 0 && m_param.today_finish_times == 0;
}

void ResearchTask::OneClickFinishTask(unsigned int& exp, int & coin, int & jade, std::vector<ItemConfigData>& rewards)
{
	const std::vector<RTRoundRewardCon> * round_reward1_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->RandRewardListRoundCfg(10, m_mgr->GetRole()->GetLevel());
	if (round_reward1_cfg != NULL)
	{
		for (std::vector<RTRoundRewardCon>::const_iterator it = round_reward1_cfg->begin(); it != round_reward1_cfg->end(); it++)
		{
			int rate_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			if (rate_num <= it->rate)//奖励给
			{
				const RTRoundRewardCfg *  reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->RandRoundRewardCfg(it->reward_group);
				if (reward_cfg != NULL)
				{
					RoleJoinActivityReward::AddRewards(rewards, reward_cfg->reward);
				}
			}
		}
	}

	const std::vector<RTRoundRewardCon> * round_reward2_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->RandRewardListRoundCfg(20, m_mgr->GetRole()->GetLevel());
	if (round_reward2_cfg != NULL)
	{
		for (std::vector<RTRoundRewardCon>::const_iterator it = round_reward2_cfg->begin(); it != round_reward2_cfg->end(); it++)
		{
			int rate_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			if (rate_num <= it->rate)//奖励给
			{
				const RTRoundRewardCfg *  reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->RandRoundRewardCfg(it->reward_group);
				if (reward_cfg != NULL)
				{
					RoleJoinActivityReward::AddRewards(rewards, reward_cfg->reward);
				}
			}
		}
	}

	if (this->IsHaveResearchTask())		// 如果有任务，先把任务刷掉
	{
		m_param.task_info.Reset();
	}

	m_param.today_finish_times = LOGIC_CONFIG->GetResearchTaskConfig()->GetOtherConfig().max_daily_task_times;

	const ResearchRewardCfg * reward_cfg = LOGIC_CONFIG->GetResearchTaskConfig()->GetTaskReward(m_mgr->GetRole()->GetLevel());
	if (NULL != reward_cfg)
	{
		if (reward_cfg->experience > 0)
		{
			exp += m_param.today_finish_times * reward_cfg->experience;
		}
	}

	this->SendResearchTaskInfo();
	EventHandler::Instance().OnResearchTask(m_mgr->GetRole(), m_param.today_finish_times);
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "ResearchTask");
	m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_RESEARCH_TASK);
}

void ResearchTask::GmResearchTaskHandle(int operator_type, int param)
{
	enum GM_RT_TYPE
	{
		RT_SET_TODAY_COMPLETE_TIMES = 0,	// 设置今天完成真中的次数
		RT_SET_CUR_TASKS_PROGRESSES = 1,	// 设置当前任务进度
		RT_REFRESH_TASK_IMMEDIATELY = 2,	// 立刻刷新任务次数
	};

	switch (operator_type)
	{
	case RT_SET_TODAY_COMPLETE_TIMES:
		m_param.today_finish_times = param;
		break;
	case RT_SET_CUR_TASKS_PROGRESSES:
		m_param.task_info.progress = param;
		break;
	case RT_REFRESH_TASK_IMMEDIATELY:
		m_param.next_refresh_timestamp = 0;
		break;
	default:
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}
}

void ResearchTask::CheckCanGainAward(bool _is_onekey_finish)
{
	std::set<int> award_sign_list;
	//LOGIC_CONFIG->GetResearchTaskConfig()->GetAwardTimesListByFinishTimes(m_param.today_finish_times, award_sign_list);

	for (std::set<int>::iterator loop = award_sign_list.begin(); loop != award_sign_list.end(); ++loop)
	{
		int flag_seq = LOGIC_CONFIG->GetResearchTaskConfig()->GetAwardSeqByAwardNeedTimes(*loop);		// 如果已经领过了，继续
		if (flag_seq < 0 || flag_seq >= RESEARCH_TASK_AWARD_MAX_SEQID || (0 != (m_param.today_reward_fetch & (1 << flag_seq)))) continue;

		int award_num = 0; ItemConfigData rand_award_list[RESEARCH_TASK_AWARD_MAX_COUNT];
		if (!LOGIC_CONFIG->GetResearchTaskConfig()->RandAwardList(m_mgr->GetRole()->GetLevel(), *loop, &award_num, rand_award_list) || award_num <= 0) continue;

		// 规则：如果玩家背包空间不足导致道具无法放入背包，则相当于玩家放弃本次奖励，但是能放下的道具还是要发放的。
		Knapsack * role_bag = m_mgr->GetKnapsack();
		if (NULL == role_bag) continue;

		if (!_is_onekey_finish)
		{
			for (int award_idx = 0; award_idx < award_num; ++award_idx)		// 放得下的才放，放不下的作废
			{
				const ItemConfigData & putin_obj = rand_award_list[award_idx];
				if (role_bag->CheckForPut(putin_obj.item_id, putin_obj.num)) role_bag->Put(putin_obj, PUT_REASON_TASK_REWARD);
			}
		}

		m_param.today_reward_fetch |= (1 << flag_seq);
	}
}

bool ResearchTask::RandTaskInfoOnce(int team_average_level, ResearchTaskInfo * out_con)
{
	if (NULL == out_con || team_average_level <= 0 || team_average_level > MAX_ROLE_LEVEL) return false;

	int r_scene_id = 0, group_id = 0, image_id = 0;
	out_con->Reset();
	Posi posi;
	const ResearchTaskCfg * basic_info = LOGIC_CONFIG->GetResearchTaskConfig()->GetTaskByLevel(team_average_level);
	if (NULL == basic_info || !LOGIC_CONFIG->GetResearchTaskConfig()->GetRandSceneAndDemonByTaskID(basic_info->task_id, &r_scene_id, &group_id, &image_id, &posi))
	{
		return false;
	}

	Scene * r_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(r_scene_id, COMMON_SCENE_KEY);
	if (NULL == r_scene) return false;

	if (posi.x == 0 && posi.y == 0)
	{
		posi = r_scene->GetARandPosi();
	}

	out_con->task_id = basic_info->task_id;
	out_con->progress = m_param.task_info.progress;//task_prog;
	out_con->image_monster_id = image_id;
	out_con->battle_group_id = group_id;
	out_con->scene_id = r_scene_id;
	out_con->posi_x = posi.x;
	out_con->posi_y = posi.y;
	
	return true;
}