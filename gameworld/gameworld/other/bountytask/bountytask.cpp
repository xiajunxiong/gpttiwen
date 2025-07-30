#include "bountytask.hpp"
#include "task/taskpool.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "task/taskrecorder.h"
#include "task/taskmanager.h"
#include "protocol/msgtask.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopen.hpp"
#include "other/prestige/prestige.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/activesystem/activesystemconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/levelcomplement/levelcomplement.h"
#include "global/worldstatus/worldstatus.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"

BountyTask::BountyTask() : m_mgr(NULL), m_task_id(-1)
{
}

BountyTask::~BountyTask()
{

}

void BountyTask::Init(RoleModuleManager* mgr, const BountyTaskParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void BountyTask::GetInitParam(ARG_OUT BountyTaskParam* out_param)
{
	*out_param = m_param;
}

void BountyTask::OnTurnTableRewardFetchReq()
{
	if (0 != m_param.has_fetch_turn_table_reward) return;
	if (m_param.today_turn_table_reward_idx < 0)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_mail_bountytask_turntable);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*) &sm, sendlen);
			}
		}
		return;
	}

	const BountyTaskTurnTableCfg* cfg = TASKPOOL->GetBountyTaskTurnTableCfgByGroupId(m_param.today_turn_table_group_id);
	if (NULL == cfg) return;

	if (m_param.today_turn_table_reward_idx >= cfg->reward_num) return;

	if (!m_mgr->GetKnapsack()->CheckForPut(cfg->reward_list[m_param.today_turn_table_reward_idx].item_id, cfg->reward_list[m_param.today_turn_table_reward_idx].num))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->Put(cfg->reward_list[m_param.today_turn_table_reward_idx], PUT_REASON_BOUNTY_TASK_TURN_TABLE_REWARD))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (0 != cfg->is_hearsay[m_param.today_turn_table_reward_idx])
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_bounty_task_turn_table_reward_notice,
							  m_mgr->GetRoleName(), cfg->reward_list[m_param.today_turn_table_reward_idx].item_id);

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	this->DepositItem(1, &cfg->reward_list[m_param.today_turn_table_reward_idx]);

	std::vector<ItemConfigData> notice_list;
	this->FillItemVec(notice_list);
	m_mgr->GetKnapsack()->NotifyGetItemList3(0, m_param.day_add_coin, 0, (short)notice_list.size(), &notice_list[0], GET_REASON_BOUNTY_TASK_REWARD, m_param.day_add_exp);

	gamelog::g_log_task.printf(LL_INFO, "BountyTask fetch reward, role[%d,%s] reward_group_id[%d] reward_idx[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_param.today_turn_table_group_id, m_param.today_turn_table_reward_idx);

	m_param.has_fetch_turn_table_reward = 1;
	m_param.today_turn_table_reward_idx = -1;
	m_param.today_turn_table_group_id = 0;
	m_param.is_first_turn_table = 1;


	this->SendInfo();
}

void BountyTask::GMFastFinishTask()
{
	m_param.cur_finish_task_group_num = m_param.today_task_group_num;

	// 已完成今日份任务，送一个转盘奖励
	m_param.today_turn_table_reward_idx = TASKPOOL->RandomBountyTaskTurnTableReward(m_mgr->GetRole()->GetLevel(), 0 == m_param.is_first_turn_table);
	if (-1 != m_param.today_turn_table_reward_idx)
	{
		m_param.today_turn_table_group_id = TASKPOOL->GetBountyTaskGroupId(m_mgr->GetRole()->GetLevel());
		m_param.has_fetch_turn_table_reward = 0;

		this->SendInfo();
	}
}

void BountyTask::OnLevelUp()
{
	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_BOUNTY_TASK)) return;

	if (m_param.today_task_group_num <= 0 && 0 == m_param.today_first_task_id)
	{
		// 若等级大于配置指定的等级，或玩家已完成配置指定的任务（这是为了客户端的表现特殊做的逻辑 orz)
		if (m_mgr->GetRole()->GetLevel() >= TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_level 
			|| m_mgr->GetTaskRecorder()->HaveFinished(TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_task_id))
		{
			if (m_param.is_first_task == 0 && m_param.is_special_task == 0 && m_mgr->GetRole()->GetLevel() <= TASKPOOL->GetBountyTaskOtherCfg().first_task_level)
			{
				m_param.is_special_task = 1;
			}

			if (m_mgr->GetRole()->GetLevel() == TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_level)
			{
				this->RefreshTaskSkipFirstTask(__FUNCTION__);
			}
			else
			{
				this->RefreshTask(__FUNCTION__);
			}
		}
	}
}

void BountyTask::OnRoleFirstAdavanceCareer()
{
	if (m_param.today_task_group_num <= 0 && 0 == m_param.today_first_task_id)
	{
		this->RefreshTask(__FUNCTION__);
	}
}

void BountyTask::OnRoleChangeProf()
{
	//玩家转职时如果门派首任务还没完成,则删除之前首任务,接取新的首任务
	if (0 == m_param.has_finish_first_task && m_param.today_task_group_num <= 0 && 0 != m_param.today_first_task_id)
	{
		m_mgr->GetTaskManager()->EraseTask(m_param.today_first_task_id);
		this->RefreshTask(__FUNCTION__);
	}	
}

void BountyTask::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void BountyTask::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	m_param.is_special_task = 0;

	this->ResetTurnTable();
	this->RemoveYestodayTask();
	this->RefreshFindCount();
	this->RefreshTask(__FUNCTION__);
}

void BountyTask::OnTaskCommit(int task_id, bool _is_onekey_finish)
{
	if(m_param.last_task_id == task_id)	//任务奖励需要先存起来
	{
		const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(m_param.last_task_id, m_mgr->GetRole());
		if (NULL != ctr_cfg)
		{
			//经验在别的接口加
			if (ctr_cfg->coin > 0)
			{
				m_param.day_add_coin += ctr_cfg->coin;
			}
			if (ctr_cfg->reward_count > 0)
			{
				this->DepositItem(ctr_cfg->reward_count, ctr_cfg->reward_list);
			}
		}
	}

	if (task_id == TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_task_id &&
		m_mgr->GetRole()->GetLevel() >= TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_level &&
		m_param.today_first_task_id == 0)
	{
		// 若当前提交的任务为指定任务，且当前等级为指定等级，且当前未接门派任务
		this->RefreshTaskSkipFirstTask(__FUNCTION__);
	}

	if (task_id == m_param.today_first_task_id)
	{
		m_param.has_finish_first_task = 1;
		TaskID task_id = 0;
		if (m_param.is_special_task == 1)
		{
			m_param.first_task_count++;
			task_id = TASKPOOL->GetFirstTaskCfg(m_param.first_task_count);
			m_param.is_first_task = 1;
		}
		else
		{
			task_id = TASKPOOL->RandomBountyTask(m_mgr->GetRole()->GetLevel(), 0, m_param.today_task_list);
		}
		
		if (task_id > 0)
		{
			m_param.cur_task_idx = 0;
			m_param.today_task_list[m_param.cur_task_idx] = task_id;
			m_param.today_task_group_num = TASKPOOL->GetBountyTaskOtherCfg().tasknum_per_day;
			this->AcceptBountyTask(task_id, _is_onekey_finish);

			gamelog::g_log_bounty_task.printf(LL_INFO, "OnTaskCommit get_task_succ, role[%d,%s] lv[%d] finish_group_num[%d] cur_task_idx[%d] next_task_id[%d]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetLevel(),
				m_param.cur_finish_task_group_num, m_param.cur_task_idx, task_id);
		}
		else
		{
			// 居然获取不了任务，一定有问题
#ifdef _DEBUG
			assert(0);
#else
			gamelog::g_log_bounty_task.printf(LL_INFO, "OnTaskCommit role[%d,%s] rand task failed!",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName());
#endif
		}
		this->SendTaskInfo();

		m_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_BOUNTY_TASK);
		return;
	}

	if (m_param.cur_task_idx >= 0 && 
		m_param.cur_task_idx < BOUNTY_TASK_MAX_PER_DAY &&
		m_param.today_task_list[m_param.cur_task_idx] == task_id)
	{
		m_param.cur_task_idx += 1;

		TaskID next_task_id = TASKPOOL->GetNextBountyTaskID(task_id);
		if (0 == next_task_id)
		{
			m_param.cur_finish_task_group_num += 1;
			EventHandler::Instance().OnBountyTaskCommit(m_mgr->GetRole(), 1);
			if (m_param.cur_finish_task_group_num >= m_param.today_task_group_num)
			{
				EventHandler::Instance().OnBountyTaskFinish(m_mgr->GetRole());
				if (!_is_onekey_finish)
				{
					//正常完成才送最后道具奖励(等级不同奖励不同)
					const std::vector<ItemConfigData>* finish_reward_cfg = TASKPOOL->GetBountyTaskFinishRewardCfg(m_mgr->GetRole()->GetLevel());
					if (NULL != finish_reward_cfg && !finish_reward_cfg->empty())
					{
						bool is_send_mail = true;
						if (m_mgr->GetKnapsack()->CheckForPutList2(*finish_reward_cfg))
						{
							if (m_mgr->GetKnapsack()->PutVec(*finish_reward_cfg, PUT_REASON_BOUNTY_FINISH_REWARD))
							{
								is_send_mail = false;
							}
						}
						if (is_send_mail)
						{
							m_mgr->GetKnapsack()->SendMail(*finish_reward_cfg, SNED_MAIL_TYPE_DEFAULT, true);
						}
						this->DepositItem((int)finish_reward_cfg->size(), &(*finish_reward_cfg)[0]);
					}
					//已完成今日份任务，送一个转盘奖励
					m_param.today_turn_table_reward_idx = TASKPOOL->RandomBountyTaskTurnTableReward(m_mgr->GetRole()->GetLevel(), 0 == m_param.is_first_turn_table);
					if (-1 != m_param.today_turn_table_reward_idx)
					{
						m_param.today_turn_table_group_id = TASKPOOL->GetBountyTaskGroupId(m_mgr->GetRole()->GetLevel());
						m_param.has_fetch_turn_table_reward = 0;

						this->SendInfo();
					}
				}
			}
			else
			{
				if (m_param.is_special_task == 1)
				{
					m_param.first_task_count++;
					next_task_id = TASKPOOL->GetFirstTaskCfg(m_param.first_task_count);
				}
				else
				{
					next_task_id = TASKPOOL->RandomBountyTask(m_mgr->GetRole()->GetLevel(), m_param.cur_task_idx, m_param.today_task_list);
				}			
				if (0 == next_task_id)
				{
					gamelog::g_log_bounty_task.printf(LL_ERROR, "OnTaskCommit rand_task_failed, role[%d,%s] lv[%d] cur_task_idx[%d]",
						m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetLevel(), m_param.cur_task_idx);
				}
			}
		}

		if (next_task_id > 0)
		{
			if (m_param.cur_task_idx >= BOUNTY_TASK_MAX_PER_DAY)
			{
#ifdef _DEBUG
				assert(0); // 数组已经放不下了，内网要挂掉警告策划
#else
				gamelog::g_log_bounty_task.printf(LL_ERROR, "BountyTask::OnTaskCommit role[%d,%s] not enough space for next task[%d]",
					m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), next_task_id);
#endif
			}
			else
			{
				m_param.today_task_list[m_param.cur_task_idx] = next_task_id;
				this->AcceptBountyTask(next_task_id, _is_onekey_finish);

				gamelog::g_log_bounty_task.printf(LL_INFO, "OnTaskCommit get_task_succ, role[%d,%s] lv[%d] finish_group_num[%d] cur_task_idx[%d] next_task_id[%d]",
					m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetLevel(), 
					m_param.cur_finish_task_group_num, m_param.cur_task_idx, next_task_id);
			}
		}

		//EVENTHANDLE.OnCommitBountyTask(m_mgr->GetRole(), this->GetTodayFinish());
		this->SendTaskInfo();
		WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "BountyTask");
	}
}

void BountyTask::OnAddDayExp(int task_id, int add_exp)
{
	if(add_exp <= 0 || task_id != m_param.last_task_id) return;

	m_param.day_add_exp += add_exp;
}

void BountyTask::OneKeyFinishBountyTask()
{
	if (this->GetTodayFinish() >= this->GetTaskCount()) return;

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_BOUNTY_TASK)) return;
	const BountyTaskOtherCfg& other_cfg = TASKPOOL->GetBountyTaskOtherCfg();
	if (m_mgr->GetRole()->GetLevel() < other_cfg.level_limit) return;

	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_BOUNTY_TASK);if (!dr_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_BOUNTY_TASK, m_mgr->GetRole()->GetLevel());
	if (NULL == price_cfg) return;

	int shengyu = this->GetTaskCount() - this->GetTodayFinish();

	int consume_money = 0;
	int consume_type = TASKPOOL->GetFindType(m_mgr->GetRole()->GetLevel());
	if (consume_type != RB_TYPE_COIN) return;

	switch (consume_type)
	{
	case RB_TYPE_COIN:
		{
			consume_money = (price_cfg->coin_price * shengyu * price_cfg->coin_scale) / 100;
			if (!m_mgr->GetMoney()->CoinBindMoreThan(consume_money))
			{
				m_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_mgr->GetMoney()->UseCoinBind(consume_money, __FUNCTION__);
		}
		break;
/*	case RB_TYPE_GOLD:
		{
			consume_money = shengyu * dr_cfg->gold_price;
			if (!m_mgr->GetMoney()->GoldMoreThan(consume_money))
			{
				m_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_mgr->GetMoney()->UseGold(consume_money, __FUNCTION__);
		}
		break;*/
	default:
		return;
		break;
	}
	int count_x = 0;	//  防止死循环
	while (1 && count_x < 100)
	{
		++count_x;
		int cur_task_id = 0;
		if (m_param.cur_task_idx == -1)
		{
			cur_task_id = m_param.today_first_task_id;
		} else
		{
			cur_task_id = m_param.today_task_list[m_param.cur_task_idx];
		}
		int old_task_id = cur_task_id;
		m_task_id = cur_task_id;
		this->OnTaskCommit(cur_task_id,true);
		this->GetReward(true);
		m_mgr->GetTaskManager()->EraseTask(old_task_id);
		if ((m_param.cur_finish_task_group_num == m_param.today_task_group_num) && m_param.today_task_list[m_param.cur_task_idx] == 0) break;
	}
	this->SendTaskInfo();
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "BountyTask");
}

void BountyTask::GetReward(bool _is_onekey_finish)
{
	// 否则说明任务完成，开始发放任务报酬
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(m_task_id);
	if (NULL == task_cfg) return;

	// 发放任务报酬
	const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(m_task_id, task_role);
	if (NULL != ctr_cfg)	// 允许出现没有奖励的任务，所以当奖励为空时不返回。
	{
		if (ctr_cfg->reward_count > 0 && !m_mgr->GetKnapsack()->CheckForPutList(ctr_cfg->reward_count, ctr_cfg->reward_list))
		{
			return; // 没有足够位置发放任务报酬
		}
	}

	if (TASK_NEED_COMMIT_PROPERTY == task_cfg->task_condition)
	{
		if (!m_mgr->GetTaskManager()->OnCommitProperty(m_task_id, task_cfg->param1, task_cfg->param2))
		{
			return; // 如果提交物品失败，则返回
		}
	}

	if (NULL != ctr_cfg)
	{
		if (!_is_onekey_finish)
		{
			if (ctr_cfg->reward_count > 0)
			{
				m_mgr->GetKnapsack()->PutList(ctr_cfg->reward_count, ctr_cfg->reward_list, PUT_REASON_BOUNTY_TASK_TURN_TABLE_REWARD);
			}

			m_mgr->GetMoney()->Add(ctr_cfg->gold, ctr_cfg->immortal, ctr_cfg->coin, "CommonTaskReward");
		}
		
		if (ctr_cfg->exp > 0)
		{
			long long real_add_exp = task_role->AddExp(ctr_cfg->exp, ADD_EXP_TYPE_TASK);
			m_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
			EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), ctr_cfg->exp, __FUNCTION__);
		}
		if (ctr_cfg->power > 0)
		{
			m_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(ctr_cfg->power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
		}
	}
}

void BountyTask::GetReward(TaskID task_id, unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards)
{
	// 否则说明任务完成，开始发放任务报酬
	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;

	// 发放任务报酬
	const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(task_id, task_role);
	if (NULL == ctr_cfg) return;
	
	for (int i = 0; i < ctr_cfg->reward_count
		&& i < ARRAY_ITEM_COUNT(ctr_cfg->reward_list); ++i)
	{
		rewards.push_back(ctr_cfg->reward_list[i]);
	}

	exp += ctr_cfg->exp;
	coin += ctr_cfg->coin;

	gamelog::g_log_bounty_task.buff_printf("BountyTask::GetReward role[%d,%s] task_id[%d] add_exp[%d] add_coin[%d] add_jade[%d] reward_items[",
		m_mgr->GetUid(), m_mgr->GetRoleName(), task_id, ctr_cfg->exp, ctr_cfg->coin, 0);
	for (int i = 0; i < ctr_cfg->reward_count
		&& i < ARRAY_ITEM_COUNT(ctr_cfg->reward_list); ++i)
	{
		gamelog::g_log_bounty_task.buff_printf("{%d,%d,%s}",
			ctr_cfg->reward_list[i].item_id,
			ctr_cfg->reward_list[i].num,
			ctr_cfg->reward_list[i].is_bind ? "T" : "F");
	}
}

bool BountyTask::OnBountyTaskFindReward(int consume_type, int count)
{
	if (count > m_mgr->GetCommonData().bounty_task_find_num)
	{
		m_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const BountyTaskFindCfg * find_cfg = TASKPOOL->GetBountyTaskFindCfg(m_mgr->GetRole()->GetLevel(), consume_type);
	if(NULL == find_cfg) return false;
	
	if (find_cfg->group_id != -1)
	{
		const BountyTaskFindRewardCfg * reward_cfg = TASKPOOL->GetBountyTaskFindRewardCfgByGroupId(find_cfg->group_id);
		if (NULL == reward_cfg)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

	//const BountyTaskFindRewardCfg * reward_cfg = TASKPOOL->GetBountyTaskFindRewardCfgByGroupId(find_cfg->group_id);
	if (NULL == reward_cfg)
	{
		m_mgr->GetCommonData().bounty_task_find_num -= count;
		return false;
	}
		
	if (!m_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

		if (!m_mgr->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_FIND_REWARD))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}

	m_mgr->GetRole()->AddExp(find_cfg->exp * count, ADD_EXP_TYPE_FIND_REWARD);
	m_mgr->GetLevelComplement()->OnAddExp(count * find_cfg->exp);
	m_mgr->GetMoney()->AddCoinBind(find_cfg->coin * count, __FUNCTION__);
	if (NULL != m_mgr->GetRole()->GetGuild())
	{
		m_mgr->GetMoney()->AddOtherCoin(count * find_cfg->family_gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}

	m_mgr->GetCommonData().bounty_task_find_num -= count;
	m_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_BOUNTY_TASK] += count;
	m_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_BOUNTY_TASK);
	EventHandler::Instance().OnBountyTaskCommit(m_mgr->GetRole(), count, true);
	return true;
}

int BountyTask::GetTaskCount()
{
	return TASKPOOL->GetBountyTaskOtherCfg().tasknum_per_day;
}

void BountyTask::SendInfo()
{
	static Protocol::SCBountyTaskTurnTableRewardInfo info;
	info.group_id = m_param.today_turn_table_group_id;
	info.reward_idx = m_param.today_turn_table_reward_idx;

	m_mgr->NetSend(&info, sizeof(info));
}

void BountyTask::SendTaskInfo()
{
	static Protocol::SCBountyTaskInfo info;
	info.cur_finish_task_num = m_param.cur_finish_task_group_num;
	info.today_task_num = m_param.today_task_group_num;
	info.has_finish_first_task = m_param.has_finish_first_task;
	info.reserve_sh = 0;

	m_mgr->NetSend(&info, sizeof(info));
}

void BountyTask::OnRoleLogin()
{
	if (this->GetTodayFinish() >= this->GetTaskCount())
	{
		return;
	}

	if (m_mgr->GetRole()->GetLevel() > TASKPOOL->GetBountyTaskOtherCfg().first_task_level)
	{	// 超过等级也不触发特殊逻辑了
		m_param.is_first_task = 1;
	}

	bool is_one_key = (m_param.last_is_one_key == 1) ? true : false;
	this->AcceptBountyTask(m_param.last_task_id, is_one_key);

	if (m_param.today_task_group_num <= 0 && 0 == m_param.today_first_task_id)
	{
		// 若等级大于配置指定的等级，或玩家已完成配置指定的任务（这是为了客户端的表现特殊做的逻辑 orz)
		if (m_mgr->GetRole()->GetLevel() >= TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_level
			|| m_mgr->GetTaskRecorder()->HaveFinished(TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_task_id))
		{
			if (m_param.is_first_task == 0 && m_param.is_special_task == 0 && m_mgr->GetRole()->GetLevel() <= TASKPOOL->GetBountyTaskOtherCfg().first_task_level)
			{
				m_param.is_special_task = 1;
			}

			if (m_mgr->GetRole()->GetLevel() == TASKPOOL->GetBountyTaskOtherCfg().skip_first_task_level)
			{
				this->RefreshTaskSkipFirstTask(__FUNCTION__);
			}
			else
			{
				this->RefreshTask(__FUNCTION__);
			}
		}
	}
}

bool BountyTask::IsCanOneClick()
{
	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_BOUNTY_TASK)) return false;
	return !m_mgr->GetActiveSystem()->IsStartedActive(ACTIVE_SYSTEM_TYPE_BOUNTY_TASK);

	//return 0 == this->GetTodayFinish() && 0 == m_param.has_finish_first_task;
}

void BountyTask::OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards)
{
	int count_x = 0;	//  防止死循环
	while (1 && count_x < 100)
	{
		++count_x;
		int cur_task_id = 0;
		if (m_param.cur_task_idx == -1)
		{
			cur_task_id = m_param.today_first_task_id;
		}
		else
		{
			cur_task_id = m_param.today_task_list[m_param.cur_task_idx];
		}
		int old_task_id = cur_task_id;
		m_task_id = cur_task_id;
		this->OnTaskCommit(cur_task_id, true);
		{
			std::vector<ItemConfigData> reward_list;
			this->GetReward(cur_task_id, exp, coin, jade, reward_list);
			for (size_t i = 0; i < reward_list.size(); i++)
			{
				RoleJoinActivityReward::AddRewards(rewards, reward_list[i]);
			}
		}

		m_mgr->GetTaskManager()->EraseTask(old_task_id);
		if ((m_param.cur_finish_task_group_num == m_param.today_task_group_num) && m_param.today_task_list[m_param.cur_task_idx] == 0) break;
	}

	this->SendTaskInfo();
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "BountyTask");
	EventHandler::Instance().OnBountyTaskCommit(m_mgr->GetRole(), this->GetTodayFinish());	
	EventHandler::Instance().OnBountyTaskFinish(m_mgr->GetRole());
}

void BountyTask::RefreshTask(const char* caller)
{
	if(!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_BOUNTY_TASK)) return;

	const BountyTaskOtherCfg& other_cfg = TASKPOOL->GetBountyTaskOtherCfg();
	if (m_mgr->GetRole()->GetLevel() < other_cfg.level_limit)
	{
		m_param.cur_task_idx = -1;
		m_param.cur_finish_task_group_num = 0;
		m_param.today_task_group_num = 0;
	}
	else
	{
		m_param.cur_task_idx = -1;
		m_param.cur_finish_task_group_num = 0;
		m_param.today_task_group_num = 0;
		memset(m_param.today_task_list, 0, sizeof(m_param.today_task_list));
		m_param.has_finish_first_task = 0;
		m_param.today_first_task_id = 0;

		TaskID task_id = TASKPOOL->GetFirstBountyTaskID(m_mgr->GetRole()->GetAppearanceProf());
		if (task_id > 0)
		{
			this->AcceptBountyTask(task_id);
			m_param.today_first_task_id = task_id;

			if (m_param.is_first_task == 0 && m_param.is_special_task == 0 && m_mgr->GetRole()->GetLevel() <= TASKPOOL->GetBountyTaskOtherCfg().first_task_level)
			{
				m_param.is_special_task = 1;
			}

			gamelog::g_log_bounty_task.printf(LL_INFO, "RefreshTask get_task_succ, role[%d,%s] lv[%d] finish_group_num[%d] cur_task_idx[%d] next_task_id[%d] caller[%s]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetLevel(),
				m_param.cur_finish_task_group_num, m_param.cur_task_idx, task_id, caller);
		}
	}
	m_param.DayItemRewardReset();
	this->SendTaskInfo();
}

void BountyTask::RefreshTaskSkipFirstTask(const char* caller)
{
	// 特殊逻辑上覆盖的特殊逻辑,这个函数功能已经和RefreshTask差不多了,但我不敢直接删.....先这样用吧(ps:不要被函数名欺骗了,他已经没有跳过第一个任务的功能了)
	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_BOUNTY_TASK)) return;

	m_param.cur_task_idx = -1;
	m_param.cur_finish_task_group_num = 0;
	m_param.today_task_group_num = 0;
	memset(m_param.today_task_list, 0, sizeof(m_param.today_task_list));
	m_param.has_finish_first_task = 0;
	m_param.today_first_task_id = 0;

	TaskID task_id = TASKPOOL->GetFirstBountyTaskID(m_mgr->GetRole()->GetAppearanceProf());
	if (task_id > 0)
	{
		this->AcceptBountyTask(task_id);
		m_param.today_first_task_id = task_id;

		if (m_param.is_first_task == 0 && m_param.is_special_task == 0 && m_mgr->GetRole()->GetLevel() <= TASKPOOL->GetBountyTaskOtherCfg().first_task_level)
		{
			m_param.is_special_task = 1;
		}

		gamelog::g_log_bounty_task.printf(LL_INFO, "RefreshTaskSkipFirstTask get_task_succ, role[%d,%s] lv[%d] finish_group_num[%d] cur_task_idx[%d] next_task_id[%d] caller[%s]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetLevel(),
			m_param.cur_finish_task_group_num, m_param.cur_task_idx, task_id, caller);
	}

	m_param.DayItemRewardReset();
	this->SendTaskInfo();
}

void BountyTask::RefreshFindCount()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_BOUNTY_TASK);
	if (NULL == cfg || m_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	const BountyTaskOtherCfg& other_cfg = TASKPOOL->GetBountyTaskOtherCfg();

	CommonDataParam& m_commondata = m_mgr->GetCommonData();
	if (m_commondata.offine_day < 2)
	{
		if (other_cfg.tasknum_per_day >= m_param.cur_finish_task_group_num)
		{
			m_commondata.bounty_task_find_num = other_cfg.tasknum_per_day - m_param.cur_finish_task_group_num;
		}
		else
		{
			m_commondata.bounty_task_find_num = 0;
		}
	}
	else
	{
		m_commondata.bounty_task_find_num = other_cfg.tasknum_per_day;
	}

}

void BountyTask::RemoveYestodayTask()
{
	for (int i = 0; i < m_param.cur_task_idx; ++i)
	{
		m_mgr->GetTaskRecorder()->TaskRecordRemove(m_param.today_task_list[i]);
	}
	if (m_param.cur_task_idx >= 0 && m_param.cur_task_idx < BOUNTY_TASK_MAX_PER_DAY)
	{
		m_mgr->GetTaskManager()->EraseTask(m_param.today_task_list[m_param.cur_task_idx]);
	}
}

void BountyTask::ResetTurnTable()
{
	if (m_param.today_turn_table_reward_idx >= 0 && 0 == m_param.has_fetch_turn_table_reward)
	{
		m_param.has_fetch_turn_table_reward = 1;

		const BountyTaskTurnTableCfg* cfg = TASKPOOL->GetBountyTaskTurnTableCfgByGroupId(m_param.today_turn_table_group_id);
		if (NULL != cfg)
		{
			static MailContentParam contentparam; contentparam.Reset();
			const ItemConfigData& item = cfg->reward_list[m_param.today_turn_table_reward_idx];
			contentparam.item_list[0].item_id = item.item_id;
			contentparam.item_list[0].num = item.num;
			contentparam.item_list[0].is_bind = item.is_bind;

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_bounty_task_turn_table_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_bounty_task_turn_table_reward_content);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}

		}
	}

	m_param.today_turn_table_reward_idx = -1;
	m_param.has_fetch_turn_table_reward = 0;
	m_param.today_turn_table_group_id = 0;
}

void BountyTask::AcceptBountyTask(TaskID task_id, bool _is_onekey_finish)
{
	const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == cfg || cfg->task_type != TASK_TYPE_BOUNTY_TASK) return;

	// 确保这个任务没有被执行过或正在执行中
	if (m_mgr->GetTaskRecorder()->HaveFinished(task_id))
	{
		m_mgr->GetTaskRecorder()->TaskRecordRemove(task_id);
	}
	if (m_mgr->GetTaskManager()->CheckRoleHasTask(task_id))
	{
		m_mgr->GetTaskManager()->EraseTask(task_id);
	}

	if (!_is_onekey_finish) //一键完成就不用去跟npc对话了
	{
		// 接任务
		m_mgr->GetTaskManager()->AcceptTask(task_id);
	}

	m_param.last_task_id = task_id;
	m_param.last_is_one_key = _is_onekey_finish ? 1 : 0;
}

void BountyTask::FillItemVec(std::vector<ItemConfigData>& item_list)
{
	if (m_param.item_count <= 0) return;

	for (int i = 0; i < ARRAY_LENGTH(m_param.day_item_reward_list) && i < m_param.item_count; ++i)
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(m_param.day_item_reward_list[i].item_id);
		if (NULL == item_base) continue;

		ItemConfigData item_data(m_param.day_item_reward_list[i].item_id, (0 != m_param.day_item_reward_list[i].is_bind), m_param.day_item_reward_list[i].count);
		item_list.push_back(item_data);
	}
}

void BountyTask::DepositItem(int item_num, const ItemConfigData * item_list)
{
	if (item_num <= 0 || NULL == item_list || m_param.item_count < 0 || m_param.item_count >= BOUNTY_TASK_REWARD_COUNT) return;

	for (int i = 0; i < item_num; i++)
	{
		bool is_merge = false;
		for (int k = 0; k < m_param.item_count && k < ARRAY_LENGTH(m_param.day_item_reward_list); ++k)
		{
			if (m_param.day_item_reward_list[k].item_id == item_list[i].item_id && (0 != m_param.day_item_reward_list[k].is_bind) == item_list[i].is_bind)
			{
				is_merge = true;
				m_param.day_item_reward_list[k].count += item_list[i].num;
				break;
			}
		}
		if (!is_merge)
		{
			if (m_param.item_count < ARRAY_LENGTH(m_param.day_item_reward_list))
			{
				m_param.day_item_reward_list[m_param.item_count].item_id = item_list[i].item_id;
				m_param.day_item_reward_list[m_param.item_count].is_bind = item_list[i].is_bind;
				m_param.day_item_reward_list[m_param.item_count].count = item_list[i].num;
				m_param.item_count++;
			}
			else
			{
				break;
			}
		}
	}
	return;
}
