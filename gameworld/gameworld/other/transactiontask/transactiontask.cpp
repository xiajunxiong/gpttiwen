#include "transactiontask.hpp"
#include "transactiontaskconfig.hpp"

#include "item/knapsack.h"
#include "task/taskpool.h"
#include "task/taskrecorder.h"
#include "task/taskmanager.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "gameworld/protocol/msgtask.h"

#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "other/dayreward/dayrewardconfig.hpp"

TransactionTask::TransactionTask()
{
}

TransactionTask::~TransactionTask()
{
}

void TransactionTask::Init(RoleModuleManager * module_mgr, const TransactionTaskParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void TransactionTask::GetInitParam(TransactionTaskParam * param)
{
	*param = m_param;
}

void TransactionTask::OnRoleLogin()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_TRANSACTION_TASK)) return;
	if(m_param.level == 0 || m_param.task_status == TRANSACTION_TASK_STATUS_FINISH) return;

	this->SendTaskInfo();
}

void TransactionTask::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void TransactionTask::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnFindDataReset();

	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_TRANSACTION_TASK)) return;

	this->RemoveYestodayTask();
	this->RefreshTask(m_module_mgr->GetRole()->GetLevel());
	this->SendTaskInfo();
}
	
void TransactionTask::OnFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TRANSACTION_TASK);
	if (NULL == cfg || m_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	if (m_module_mgr->GetCommonData().offine_day < 2)
	{
		if (TRANSACTION_TASK_STATUS_FINISH != m_param.task_status)
		{
			m_param.find_count = this->GetLastReferNum();
		}
		else
		{
			m_param.find_count = 0;
		}
	}
	else
	{
		m_param.find_count = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskOtherCfg().random;
	}

}

void TransactionTask::RoleLevelUp(int cur_level)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_TRANSACTION_TASK)) return;
	if(m_param.level != 0) return;

	this->RefreshTask(cur_level);
	this->SendTaskInfo();
}

void TransactionTask::CheckVersion()
{
	const TransactionTaskLevelGroupCfg* level_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskLevelGroupCfg(m_param.level);
	if(NULL == level_cfg) return;

	if(level_cfg->version_reset == m_param.version) return;
	if(m_param.task_status == TRANSACTION_TASK_STATUS_FINISH) return;

	int list[TRANSACTION_TASK_REWARD_MAX_NUM];
	if (!LOGIC_CONFIG->GetTransactionTaskConfig()->GetRewardListCfg(m_param.level, list, &m_param.type, &m_param.version)) return;

	for (int i = 0; i < TRANSACTION_TASK_REWARD_MAX_NUM; i++)
	{
		m_param.seq[i] = list[i];
	}
}

void TransactionTask::ReferItem(int index, int refer_num,bool _is_onekey_finish)
{
	if(index < 0 || index >= TRANSACTION_TASK_REWARD_MAX_NUM || refer_num <= 0) return;
	if (m_param.task_status != 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return;
	}
	if (m_param.concondition_flag.IsBitSet(index))
	{
		m_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	int group_id = LOGIC_CONFIG->GetTransactionTaskConfig()->GetGroupByIndex(m_param.type, index);
	if (group_id == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
		return;
	}

	const TransactionTaskSingleCfg* cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskSingleRewardCfg(group_id, m_param.seq[index]);
	if (NULL == cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (_is_onekey_finish == false)
	{
		int item_num = m_module_mgr->GetKnapsack()->Count(cfg->need_item.item_id);
		if (cfg->need_item.num > refer_num || refer_num > item_num || cfg->need_item.num > item_num)
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
		if (cfg->is_has_item_reward)
		{
			if (!m_module_mgr->GetKnapsack()->CheckForPut(cfg->reward.item_id, cfg->reward.num))
			{
				m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}
		if(!m_module_mgr->GetKnapsack()->ConsumeItem(cfg->need_item.item_id, cfg->need_item.num, __FUNCTION__))
		{
			m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
		if (cfg->is_has_item_reward)
		{
			if (!m_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_TRANSACTION_TASK_REWARD))
			{
				//返还提交道具
				m_module_mgr->GetKnapsack()->Put(cfg->need_item, PUT_REASON_TRANSACTION_TASK_REWARD);
				return;
			}
		}
		m_module_mgr->GetMoney()->AddOtherCoin(cfg->money_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	}

	m_param.concondition_flag.SetBit(index);
	if(this->CheckAllTaskFinish())
	{ 
		EventHandler::Instance().OnTransactionTask(m_module_mgr->GetRole());
	}

	this->SendTaskInfo();
}

void TransactionTask::GetFinallyReward(bool _is_onekey_finish)
{
	if (m_param.task_status != 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	if (!this->CheckAllTaskFinish())
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	const TransactionTaskRewardCfg * cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskRewardCfg(m_param.level);
	if(NULL == cfg)
	{ 
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPutList(cfg->count, &cfg->task_reward[0]))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!_is_onekey_finish)
	{
		if (!m_module_mgr->GetKnapsack()->PutList(cfg->count, &cfg->task_reward[0], PUT_REASON_TRANSACTION_TASK_REWARD))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		m_module_mgr->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
	}
	
	m_module_mgr->GetRole()->AddExp(cfg->exp, ADD_EXP_TYPE_TRANSACTION_TASK, __FUNCTION__);
	m_param.task_status = 1;
	this->SendTaskInfo();
	EventHandler::Instance().OnTransactionTaskCommit(m_module_mgr->GetRole());
}

int TransactionTask::GetTodayFinish()
{
	int count = 0;
	if (this->CheckAllTaskFinish())
	{
		count = 1;
	}
	return count;
}

void TransactionTask::OneKeyFinishTransactionTask()
{
	if (this->GetTodayFinish() == 1)
	{
		m_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return;
	}

	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TRANSACTION_TASK); if (!dr_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_TRANSACTION_TASK, m_module_mgr->GetRole()->GetLevel());
	if (NULL == price_cfg) return;
	const TransactionTaskOtherCfg & tto_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskOtherCfg();
	const TransactionTaskRewardCfg *ttr_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskRewardCfg(m_module_mgr->GetRole()->GetLevel()); if (!ttr_cfg) return;
	
	this->CheckVersion();
	this->SendTaskInfo();

	int shengyu = this->GetLastReferNum();

	//扣钱扣钱
	int consume_money = 0;
	int consume_type = LOGIC_CONFIG->GetTransactionTaskConfig()->GetRewardType(m_module_mgr->GetRole()->GetLevel());
	if (consume_type != RB_TYPE_COIN) return;

	switch (consume_type)
	{
	case RB_TYPE_COIN:
		{
			consume_money = (price_cfg->coin_price * shengyu * price_cfg->coin_scale) / 100;
			if (!m_module_mgr->GetMoney()->CoinBindMoreThan(consume_money))
			{
				m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_module_mgr->GetMoney()->UseCoinBind(consume_money, __FUNCTION__);
		}
		break;
/*	case RB_TYPE_GOLD:
		{
			consume_money = shengyu * dr_cfg->gold_price;
			if (!m_module_mgr->GetMoney()->GoldMoreThan(consume_money))
			{
				m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_module_mgr->GetMoney()->UseGold(consume_money, __FUNCTION__);
		}
		break;*/
	default:
		return;
		break;
	}

	for (int index = 0; index < tto_cfg.random;++index)
	{
		// 当前格子是否提交seq
		if (m_param.concondition_flag.IsBitSet(index)) continue;

		int group_id = LOGIC_CONFIG->GetTransactionTaskConfig()->GetGroupByIndex(m_param.type, index);
		if (group_id == 0)
		{
			m_module_mgr->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
			return;
		}

		const TransactionTaskSingleCfg* cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskSingleRewardCfg(group_id, m_param.seq[index]);
		if (NULL == cfg)
		{
			m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		int item_num = m_module_mgr->GetKnapsack()->Count(cfg->need_item.item_id);
		if (cfg->need_item.num > item_num ) // 如果数量不足直接给，反正钱也扣了
		{
			//m_module_mgr->GetKnapsack()->Put(cfg->need_item, PUT_REASON_NO_NOTICE);
		}
		item_num = cfg->need_item.num;
		//提交
		this->ReferItem(index, item_num,true);
	}

	this->GetFinallyReward(true);
}

bool TransactionTask::OnTransactionTaskFindReward(int consume_type, int count)
{
	if (m_param.find_count <= 0 || count != m_param.find_count)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const TransactionTaskRewardFindCfg * find_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetFindRewardCfg(consume_type, m_module_mgr->GetRole()->GetLevel());
	if (NULL == find_cfg) return false;

	std::vector<ItemConfigData> reward_list;
//	for (int i = 0; i < count; i++)
	{
		const TransactionTaskFindRewardGroupCfg * find_reward_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetFindRewardGroupCfg(find_cfg->group_id);
		if (NULL != find_reward_cfg && !find_reward_cfg->reward_list.empty())
		{
			reward_list.insert(reward_list.end(), find_reward_cfg->reward_list.begin(), find_reward_cfg->reward_list.end());
		}
	}

	if (!reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), &reward_list[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_TRANSACTION_TASK_REWARD))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}
	if (find_cfg->exp > 0)
	{
		m_module_mgr->GetRole()->AddExp(find_cfg->exp, ADD_EXP_TYPE_FIND_REWARD);
	}
	m_module_mgr->GetMoney()->AddCoinBind(find_cfg->coin, __FUNCTION__);
	if (NULL != m_module_mgr->GetRole()->GetGuild())
	{
		m_module_mgr->GetMoney()->AddOtherCoin(find_cfg->family_gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	
	m_param.find_count -= count;
	m_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_TRANSACTION_TASK] += count;
	m_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_TRANSACTION_TASK);
	return true;
}

void TransactionTask::SendTaskInfo()
{
	Protocol::SCTransactionTaskInfo info;
	info.task_id = m_param.task_id;
	info.task_status = m_param.task_status;
	info.version_reset = m_param.version;
	info.level = m_param.level;
	info.type = m_param.type;
	memcpy(info.seq, m_param.seq, sizeof(info.seq));
	info.reserve = 0;
	info.flag = m_param.concondition_flag;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void TransactionTask::TransactionTaskOp(Protocol::CSTransactionTaskReq * req)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_TRANSACTION_TASK)) return;

	switch (req->op_type)
	{
	case Protocol::CSTransactionTaskReq::TRANSACTION_TASK_INFO_TYPE:
		{
			this->CheckVersion();
			this->SendTaskInfo();
		}
		break;
	case Protocol::CSTransactionTaskReq::TRANSACTION_TASK_COMMON_TYPE:
		{
			this->ReferItem(req->index, req->refer_num);
		}
		break;
	case Protocol::CSTransactionTaskReq::TRANSACTION_TASK_FINALLY_TYPE:
		{
			this->GetFinallyReward();
		}
		break;
	}
}

bool TransactionTask::CheckAllTaskFinish()
{
	for (int i = 0; i < TRANSACTION_TASK_REWARD_MAX_NUM; i++)
	{
		if (!m_param.concondition_flag.IsBitSet(i)) return false;
	}
	return true;
}

int TransactionTask::GetLastReferNum()
{
	const TransactionTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskOtherCfg();
	int last_num = 0;
	for (int i = 0; i < other_cfg.random; i++)
	{
		if (!m_param.concondition_flag.IsBitSet(i))
		{
			last_num++;
		}
	}

	return last_num;
}

void TransactionTask::RefreshTask(int level)
{
	m_param.DayReset();

	int list[TRANSACTION_TASK_REWARD_MAX_NUM];
	short type = 0;
	short version = 0;
	if (!LOGIC_CONFIG->GetTransactionTaskConfig()->GetRewardListCfg(level, list, &type, &version)) return;

	const TransactionTaskOtherCfg & other_cfg = LOGIC_CONFIG->GetTransactionTaskConfig()->GetTaskOtherCfg();

	const TaskInfomaConfig * cfg = TASKPOOL->GetTaskCfgByID(other_cfg.task_id);
	if(NULL == cfg || cfg->level_min > level) return;

	m_param.task_id = other_cfg.task_id;
	m_param.level = level;
	m_param.type = type;
	m_param.version = version;

	for (int i = 0; i < TRANSACTION_TASK_REWARD_MAX_NUM; i++)
	{
		m_param.seq[i] = list[i];
	}

	m_module_mgr->GetTaskManager()->AcceptTask(m_param.task_id);
}

void TransactionTask::RemoveYestodayTask()
{
	if (m_param.task_id >= 0 && m_param.task_status == 0)
	{
		m_module_mgr->GetTaskManager()->EraseTask(m_param.task_id);
		//增加删除记录是为了容错,如数据库中删除了玩家丝绸之路数据,但是记录还存在的话,玩家无法在接到这个任务
		m_module_mgr->GetTaskRecorder()->TaskRecordRemove(m_param.task_id); 
	}
	if (m_param.task_id >= 0 && m_param.task_status == 1)
	{
		m_module_mgr->GetTaskRecorder()->TaskRecordRemove(m_param.task_id);
	}
}
