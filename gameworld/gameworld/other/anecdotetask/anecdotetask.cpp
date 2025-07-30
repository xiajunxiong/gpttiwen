#include "anecdotetask.hpp"
#include "servercommon/servercommon.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgtask.h"
#include "task/taskmanager.h"
#include "obj/character/role.h"
#include "npc/npcmanager.h"
#include "item/itemgriddata.h"
#include "item/knapsack.h"
#include "protocol/msgitem.h"
#include "servercommon/errornum.h"
#include "servercommon/maildef.hpp"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "battle/battle_manager_local.hpp"
#include "item/knapsack.h"

AnecdoteTask::AnecdoteTask() : m_mgr(NULL)
{

}

AnecdoteTask::~AnecdoteTask()
{

}

void AnecdoteTask::Init(RoleModuleManager* mgr, const AnecdoteTaskParam& param, const AnecdoteTaskCommonParam& common_param, const SpecialAnecdoteParam& special_param)
{
	m_mgr = mgr;
	m_param = param;
	m_common_param = common_param;
	m_special_param = special_param;
	// 转移天下绝对的数据
	if (0 == m_common_param.transfer_tianxiajuedui_flag)
	{
		m_common_param.special_value[ANECDOTE_TASK_TYPE_TIANXIAJUEDUI] = m_common_param.tianxiajuedui;
		m_common_param.tianxiajuedui = 0;
		m_common_param.transfer_tianxiajuedui_flag = 1;
	}
}

void AnecdoteTask::GetInitParam(AnecdoteTaskParam* out_param, AnecdoteTaskCommonParam* out_common_param, SpecialAnecdoteParam* out_special_param)
{
	if (NULL == out_param || NULL == out_common_param) return;

	*out_param = m_param;
	*out_common_param = m_common_param;
	*out_special_param = m_special_param;
}

void AnecdoteTask::OnLogin()
{
	this->CheckAutoGiveReward();
	this->CheckAutoAcceptTask();
	this->CheckBigEventExpireAutoGiveMail();
	this->CheckBigEvent();
}

void AnecdoteTask::OnRoleLevelUp()
{
	this->CheckAutoAcceptTask();
	this->CheckBigEvent();
}

void AnecdoteTask::OnRoleClaimReward(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 == m_param.task_info_list[anecdote_task_type].cur_task_id)
	{
		// 未接任务
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_ACCEPTED);
		return;
	}

	if (0 == m_param.task_info_list[anecdote_task_type].succ_times)
	{
		// 未完成任务
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].has_claim_reward > 0)
	{
		// 已领取奖励
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_ALREADY_CLAIM_REWARD);
		return;
	}

	this->GiveFinalReward(anecdote_task_type, false);
}

void AnecdoteTask::OnFinishTask(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(m_param.task_info_list[anecdote_task_type].cur_task_id);
	if (NULL == task_cfg)
	{
		return;
	}

	if (task_cfg->task_condition != TASK_NEED_MEET_ANECDOTE_REQUIRES)
	{
		return;
	}

	// 未写任务进度控制模块时，暂时这样直接调用ProgressChange
	this->OnProgressChange(m_param.task_info_list[anecdote_task_type].cur_task_id,
		m_param.task_info_list[anecdote_task_type].progress,
		m_param.task_info_list[anecdote_task_type].progress + 1);
}

void AnecdoteTask::OnFailTask(int anecdote_task_type)
{
	// 暂时没有逻辑
}

void AnecdoteTask::OnBattleFail(int monster_group_id)
{
	const AnecdoteMonsterGroupCfg* cfg = TASKPOOL->GetAnecdoteMonsterGroupCfg(monster_group_id);
	if (NULL == cfg) return;

	int anecdote_task_type = cfg->anecdote_task_type;
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	Protocol::SCAnecdoteTaskBattleFail msg;
	msg.anecdote_task_type = anecdote_task_type;
	msg.task_id = m_param.task_info_list[anecdote_task_type].cur_task_id;

	m_mgr->NetSend(&msg, sizeof(msg));
}

void AnecdoteTask::AcceptTask(int task_id)
{

	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg || TASK_TYPE_ANECDOTE_TASK != task_cfg->task_type) return;

	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].succ_times > 0)
	{
		return;
	}

	m_param.task_info_list[anecdote_task_type].cur_task_id = task_id;
	m_param.task_info_list[anecdote_task_type].progress = 0;
	m_param.task_info_list[anecdote_task_type].cur_task_accept_status = ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED;

	this->SendTaskInfoChangeNotice(anecdote_task_type);

	gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::AcceptTask role[%d,%s] task_id[%d] anecdote_type[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_id, anecdote_task_type);

	
}

void AnecdoteTask::CommitTask(int task_id)
{
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg || TASK_TYPE_ANECDOTE_TASK != task_cfg->task_type) return;

	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].succ_times > 0)
	{
		return;
	}

	if (0 == m_param.task_info_list[anecdote_task_type].progress)
	{
		return;
	}

	// 发放小任务的奖励（有可能是一些任务物品）
	const CommonTaskReward * ctr_cfg = TASKPOOL->GetCommonTaskReward(task_id, task_role);
	if (NULL != ctr_cfg)	// 允许出现没有奖励的任务，所以当奖励为空时不返回。
	{
		if (ctr_cfg->reward_count > 0 && !m_mgr->GetKnapsack()->CheckForPutList(ctr_cfg->reward_count, ctr_cfg->reward_list))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return; // 没有足够位置发放任务报酬
		}

		if (ctr_cfg->reward_count > 0)
		{
			m_mgr->GetKnapsack()->PutList2(ctr_cfg->reward_count, ctr_cfg->reward_list, PUT_REASON_TASK_REWARD, GET_REASON_TASK, task_id);
		}

		m_mgr->GetMoney()->Add(ctr_cfg->gold, ctr_cfg->immortal, ctr_cfg->coin, "AnecdoteTaskCommonReward");

		if (ctr_cfg->exp > 0) m_mgr->GetRole()->AddExp(ctr_cfg->exp, ADD_EXP_TYPE_TASK);
		if (ctr_cfg->power > 0)
		{
			m_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(ctr_cfg->power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
		}
	}

	gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::CommitTask role[%d,%s] task_id[%d] anecdote_type[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_id, anecdote_task_type);

	if (0 == task_cfg->nex_task_id) 
	{
		// 没有后置任务，则该任务已全部完成
		m_param.task_info_list[anecdote_task_type].succ_times = 1;
		m_param.task_info_list[anecdote_task_type].has_claim_reward = 0;
		this->SendTaskInfoChangeNotice(anecdote_task_type);
		this->SendBigEventInfo();
		m_mgr->GetTaskManager()->OnFinishAnedoteGame(anecdote_task_type);
	}
	else
	{
		// 先提交当前子任务
		//this->SendTaskInfoChangeNotice(anecdote_task_type);

		// 有后置任务
		m_param.task_info_list[anecdote_task_type].cur_task_id = task_cfg->nex_task_id;
		m_param.task_info_list[anecdote_task_type].progress = 0;
		m_param.task_info_list[anecdote_task_type].cur_task_accept_status = ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTABLE;
		
		this->SendTaskInfoChangeNotice(anecdote_task_type);

		const TaskInfomaConfig* next_task = TASKPOOL->GetTaskCfgByID(task_cfg->nex_task_id);
		if (NULL != next_task && 0 == next_task->accept_npc)
		{
			gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::CommitTask AutoAcceptNextTask role[%d,%s] next_task_id[%d] anecdote_type[%d]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_cfg->nex_task_id, anecdote_task_type);
			
			this->AcceptTask(task_cfg->nex_task_id);
		}
	}
	
	// 先下发任务完成通知
	this->SendTaskCompleteNotice(anecdote_task_type, task_id);
}

void AnecdoteTask::OnProgressChange(int task_id, int old_value, int new_value)
{
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg || TASK_TYPE_ANECDOTE_TASK != task_cfg->task_type) return;

	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].cur_task_id != task_id)
	{
#ifdef _DEBUG
		assert(0); // 不应存在这种情况
#endif
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].progress > 0)
	{
		return;
	}

	m_param.task_info_list[anecdote_task_type].progress = new_value;
	
	gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::OnProgressChange role[%d,%s] task_id[%d] anecdote_type[%d] cur_progress[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_id, anecdote_task_type, new_value);

	if (m_param.task_info_list[anecdote_task_type].progress > 0
		&& task_cfg->can_auto_commit)
	{
		gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::OnProgressChange AutoCommitTask role[%d,%s] task_id[%d] anecdote_type[%d]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), task_id, anecdote_task_type);

		this->CommitTask(task_id); // 自动提交就不下发进度了
	}
	else
	{
		this->SendTaskInfoChangeNotice(anecdote_task_type);
	}
}

void AnecdoteTask::GiveFinalReward(int anecdote_task_type, bool is_by_mail)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].has_claim_reward > 0)
	{
		return;
	}

	const AnecdoteRewardCfg* reward_cfg = TASKPOOL->GetAnecdoteFinalReward(anecdote_task_type);
	if (NULL == reward_cfg)
	{
		return;
	}

	bool is_knapsack_full_send_mail = false;
	if (!is_by_mail)
	{
		if (reward_cfg->reward_count > 0)
		{
			if (!m_mgr->GetKnapsack()->CheckForPutList(reward_cfg->reward_count, reward_cfg->reward_list))
			{
				is_by_mail = true; // 没有空间的情况下，通过邮件发放
				is_knapsack_full_send_mail = true;
			}
		}
	}

	if (!is_by_mail)
	{
		if (reward_cfg->reward_count > 0)
		{
			if (!m_mgr->GetKnapsack()->CheckForPutList(reward_cfg->reward_count, reward_cfg->reward_list))
			{
				m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}

		if (reward_cfg->gold > 0)
		{
			m_mgr->GetMoney()->AddGold(reward_cfg->gold, __FUNCTION__);
		}

		if (reward_cfg->coin_bind > 0)
		{
			m_mgr->GetMoney()->AddCoinBind(reward_cfg->coin_bind, __FUNCTION__);
		}

		if (reward_cfg->exp > 0)
		{
			m_mgr->GetRole()->AddExp(reward_cfg->exp, ADD_EXP_TYPE_ANECDOTE_TASK, __FUNCTION__);
		}

		if (reward_cfg->reward_count > 0)
		{
			m_mgr->GetKnapsack()->PutList(reward_cfg->reward_count, reward_cfg->reward_list, PUT_REASON_ANECDOTE_TASK);
		}

		m_mgr->GetKnapsack()->NotifyGetItemList3(reward_cfg->gold, reward_cfg->coin_bind, reward_cfg->exp, reward_cfg->reward_count, reward_cfg->reward_list, GET_REASON_ANECDOTE_TASK);
	
		gamelog::g_log_anecdote_task.buff_printf("AnecdoteTask::GiveFinalReward ToKnapsack, role[%d,%s] gold[%d] coin[%d] exp[%d] items[", 
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), reward_cfg->gold, reward_cfg->coin_bind, reward_cfg->exp);
		for (int k = 0; k < reward_cfg->reward_count; ++k)
		{
			gamelog::g_log_anecdote_task.buff_printf("[item_id:%d, num:%d]", reward_cfg->reward_list[k].item_id, reward_cfg->reward_list[k].num);
		}
		gamelog::g_log_anecdote_task.buff_printf("]");
		gamelog::g_log_anecdote_task.commit_buff(LL_INFO);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		
		if (reward_cfg->gold > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = reward_cfg->gold;
		if (reward_cfg->coin_bind > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin_bind;
		if (reward_cfg->exp > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = reward_cfg->exp;

		for (int k = 0; k < reward_cfg->reward_count && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
		{
			const ItemConfigData & reward = reward_cfg->reward_list[k];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
			if (item_base == NULL)
			{
				continue;
			}

			contentparam.item_list[k].item_id = reward.item_id;
			contentparam.item_list[k].num = reward.num;
			contentparam.item_list[k].is_bind = (reward.is_bind) ? 1 : 0;
			contentparam.item_list[k].invalid_time = item_base->CalInvalidTime();
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_anecdote_task_reward_by_mail_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), is_knapsack_full_send_mail ? gamestring::g_anecdote_task_reward_by_mail_knapsack_full : gamestring::g_anecdote_task_reward_by_mail_content, anecdote_task_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		
			gamelog::g_log_anecdote_task.buff_printf("AnecdoteTask::GiveFinalReward ToKnapsack, role[%d,%s] gold[%d] coin[%d] exp[%d] items[", 
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), reward_cfg->gold, reward_cfg->coin_bind, reward_cfg->exp);
			for (int k = 0; k < reward_cfg->reward_count; ++k)
			{
				gamelog::g_log_anecdote_task.buff_printf("[item_id:%d, num:%d]", reward_cfg->reward_list[k].item_id, reward_cfg->reward_list[k].num);
			}
			gamelog::g_log_anecdote_task.buff_printf("]");
			gamelog::g_log_anecdote_task.commit_buff(LL_INFO);
		}
	}

	m_param.task_info_list[anecdote_task_type].has_claim_reward = 1;
	this->SendTaskInfoChangeNotice(anecdote_task_type);
	EventHandler::Instance().OnFinishAnecdoteFinalTask(m_mgr->GetRole(), anecdote_task_type);
}

void AnecdoteTask::SendTaskInfoChangeNotice(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	Protocol::SCAnecdoteTaskInfo info;
	info.task_info.accept_status = m_param.task_info_list[anecdote_task_type].cur_task_accept_status;
	info.task_info.cur_task_id = m_param.task_info_list[anecdote_task_type].cur_task_id;
	info.task_info.has_finish = m_param.task_info_list[anecdote_task_type].succ_times > 0 ? 1 : 0;
	info.task_info.progress = m_param.task_info_list[anecdote_task_type].progress;
	info.task_info.anecdote_task_type = anecdote_task_type;

	m_mgr->NetSend(&info, sizeof(info));
}

void AnecdoteTask::SendTaskCompleteNotice(int anecdote_task_type, int task_id)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	Protocol::SCAnecdoteTaskCompleteNotice msg;
	msg.anecdote_task_type = static_cast<short>(anecdote_task_type);
	msg.task_id = static_cast<unsigned short>(task_id);

	m_mgr->NetSend(&msg, sizeof(msg));
}

void AnecdoteTask::OnTalkToNpc(int npc_id)
{
	if (!NpcManager::Instance().CanTouchNpc(m_mgr->GetRole(), npc_id)) return;

	for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	{
		int task_id = m_param.task_info_list[type].cur_task_id;
		if (0 == task_id) continue;

		if (m_param.task_info_list[type].succ_times > 0) continue;
		if (m_param.task_info_list[type].cur_task_accept_status != ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED) continue;

		const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
		if (NULL == task_cfg) continue;

		if (task_cfg->task_condition != TASK_NEED_TALK_WITH_NPC) continue;
		if (task_cfg->param1 != npc_id) continue;
		
		m_param.task_info_list[type].progress += 1;

		this->SendTaskInfoChangeNotice(type);

		if (m_param.task_info_list[type].progress > task_cfg->param2
			&& task_cfg->can_auto_commit)
		{
			this->CommitTask(task_id);
		}
	}
}

void AnecdoteTask::OnCommitItem(int task_id, int item_id, int num)
{
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;
	if (task_cfg->task_condition != TASK_NEED_GIVE_PROPERTY) return;

	if (task_cfg->param1 != item_id)
	{
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_COMMIT_ITEM_MISMATCH);
		return;
	}

	if (!m_mgr->GetKnapsack()->HasItem(item_id, num))
	{
		static Protocol::SCTaskItemLackForCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);

		return; 
	}

	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.task_info_list[anecdote_task_type].succ_times > 0)
	{
		// 已完成
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_ALREADY_SUCC);
		return;
	}

	if (ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED != m_param.task_info_list[anecdote_task_type].cur_task_accept_status)
	{
		// 未接任务
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_ACCEPTED);
		return;
	}
	
	if (!m_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__))
	{
		static Protocol::SCTaskItemLackForCommitNotice notice;
		notice.count = 1;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);

		return;
	}

	{
		// 通知提交了物品
		static Protocol::SCTaskItemCommitNotice notice;
		notice.item_list[0].item_id = item_id;
		notice.item_list[0].num = num;
		notice.count = 1;

		int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);
	}

	m_param.task_info_list[anecdote_task_type].progress += 1;

	this->SendTaskInfoChangeNotice(anecdote_task_type);

	if (m_param.task_info_list[anecdote_task_type].progress > task_cfg->param2
		&& task_cfg->can_auto_commit)
	{
		this->CommitTask(task_id);
	}
}

bool GetConsumeStuffList(int item_num, ARG_IN const ItemContainer* item_list, ARG_OUT int* stuff_count, ARG_OUT ItemExtern::ItemConsumeConfig* stuff_list, int max_stuff_num)
{
	for (int i = 0; i < item_num && *stuff_count < max_stuff_num; ++i)
	{
		ItemExtern::ItemConsumeConfig & stuff_obj = stuff_list[(*stuff_count)++];
		stuff_obj.item_id = item_list[i].item_id;
		stuff_obj.num = item_list[i].com_num;
		stuff_obj.buyable = false;
	}

	return (*stuff_count) == item_num;
}

bool CheckGroupItemAllInList(const TaskInfomaConfig* task_cfg, int item_num, ARG_IN const ItemContainer* item_list)
{
	const std::set<ItemID>* group_items = TASKPOOL->GetItemGroupList(task_cfg->param1);
	if (NULL == group_items || group_items->size() != item_num) return false;

	// 保证配置中每个物品都被找到
	std::set<ItemID>::const_iterator it = group_items->begin();
	for (; it != group_items->end(); ++it)
	{
		bool is_found = false;
		for (int i = 0; i < item_num; ++i)
		{
			if ((*it) == item_list[i].item_id)
			{
				is_found = true;
				break;
			}
		}
		if (!is_found)
		{
			return false;
		}
	}
	
	return true;
}

void AnecdoteTask::OnCommitItemGroup(TaskID task_id, int item_num, ARG_IN const ItemContainer* item_list)
{
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg) return;
	if (task_cfg->task_condition != TASK_NEED_GIVE_PROGROUP) return;

	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!this->CheckTaskStatus(task_id))
	{
		return;
	}

	if (!CheckGroupItemAllInList(task_cfg, item_num, item_list))
	{
		m_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
		return;
	}

	int stuff_count = 0, com_num = 0; 
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];

	if (!GetConsumeStuffList(item_num, item_list, &stuff_count, stuff_list, ARRAY_LENGTH(stuff_list)))
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(task_role, false, stuff_count, stuff_list, &consume_list, true))
		{
			return;
		}

		// 消耗
		if (consume_list.count > 0 && !m_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "TaskGiveItem"))
		{
			return;
		}

		{
			// 通知提交了物品
			static Protocol::SCTaskItemCommitNotice notice;
			notice.count = 0;
			for (int i = 0; i < stuff_count; ++i)
			{
				notice.item_list[notice.count].item_id = stuff_list[i].item_id;
				notice.item_list[notice.count].num = stuff_list[i].num;
				notice.count += 1;
			}
			int send_len = sizeof(notice) - sizeof(notice.item_list) + notice.count * sizeof(notice.item_list[0]);
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&notice, send_len);
		}
	}
	
	// 改变数据
	m_param.task_info_list[anecdote_task_type].progress += 1;

	this->SendTaskInfoChangeNotice(anecdote_task_type);

	if (m_param.task_info_list[anecdote_task_type].progress > task_cfg->param2
		&& task_cfg->can_auto_commit)
	{
		this->CommitTask(task_id);
	}
}

void AnecdoteTask::OnChallengeAnecdoteSucc(int anecdote_type)
{
	//奖励微薄，小游戏全靠客户端自己实现，客户端说完成了就是完成了，这里不作过多限制(除了要战斗的)

	if (anecdote_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_type >= ANECDOTE_TASK_TYPE_MAX ||
		ANECDOTE_TASK_TYPE_THIEF_CAMP_RESCUE == anecdote_type ||
		ANECDOTE_TASK_TYPE_CATCH_FOX == anecdote_type)
	{
		return;
	}

	gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::OnChallengeAnecdoteSucc role[%d,%s] anecdote_type[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), anecdote_type);

	this->OnFinishTask(anecdote_type);
	
	UNSTD_STATIC_CHECK(ANECDOTE_TASK_TYPE_MAX == 43);
}

void AnecdoteTask::CheckAutoAcceptTask()
{
	// 对所有初始任务，判断是否能接受任务

	for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	{
		if (0 != m_param.task_info_list[type].cur_task_id) continue;
		if (m_param.task_info_list[type].succ_times > 0) continue;

		const AnecdoteInitialTaskCfg* cfg = TASKPOOL->GetAnecdoteTaskInitialCfg(type);
		if (NULL == cfg) continue;

		if (m_mgr->GetRole()->GetLevel() < cfg->level_limit) continue;

		const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(cfg->task_id);
		if (NULL == task_cfg) continue;

		m_param.task_info_list[type].cur_task_id = cfg->task_id;
		m_param.task_info_list[type].progress = 0;
		m_param.task_info_list[type].has_claim_reward = 0;
		m_param.task_info_list[type].cur_task_accept_status = ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTABLE;

		this->SendTaskInfoChangeNotice(type);

		if (0 == task_cfg->accept_npc) // 可以自动接任务
		{
			this->AcceptTask(cfg->task_id);
		}
	}
}

void AnecdoteTask::CheckAutoGiveReward()
{
	for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	{
		if (0 == m_param.task_info_list[type].cur_task_id) continue;
		if (m_param.task_info_list[type].has_claim_reward > 0) continue;

		int task_id = m_param.task_info_list[type].cur_task_id;
		const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
		if (NULL == task_cfg || TASK_TYPE_ANECDOTE_TASK != task_cfg->task_type) continue;
		if (task_cfg->nex_task_id > 0 || TASK_NEED_TALK_WITH_NPC != task_cfg->task_condition) continue;

		// 最后一个任务(且任务条件不是对话），无需进度，只要重登就发奖励

		// 设置接取任务状态为已接取
		if (m_param.task_info_list[type].cur_task_accept_status != ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED)
		{
			m_param.task_info_list[type].cur_task_accept_status = ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED;
		}

		// 设置任务进度为已完成
		if (0 == m_param.task_info_list[type].progress)
		{
			m_param.task_info_list[type].progress = 1;
		}
		
		if (0 == m_param.task_info_list[type].succ_times)
		{
			m_param.task_info_list[type].succ_times = 1;
		}

		this->GiveFinalReward(type, true);
	}
}

void AnecdoteTask::SendAllInfo()
{
	this->SendTaskInfo();
	this->SendSpecialInfo();
	this->SendBigEventInfo();
	this->SendSpecialAnecdoteTaskInfo();
}

void AnecdoteTask::SpecialSet(int req_type, int value)
{
	if (req_type < 0 || req_type >= ANECDOTE_TASK_TYPE_MAX) return;

	m_common_param.special_value[req_type] = value; // 仅作存储，并在登录时下发
}

bool AnecdoteTask::IsFinishTask(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return false;
	}

	return 0 != m_param.task_info_list[anecdote_task_type].has_claim_reward;
}

bool AnecdoteTask::IsAcceptOrFinish(int anecdote_task_type)
{
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return false;
	}

	if (0 != m_param.task_info_list[anecdote_task_type].succ_times || 
		0 != m_param.task_info_list[anecdote_task_type].has_claim_reward ||
		ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED == m_param.task_info_list[anecdote_task_type].cur_task_accept_status)
	{
		return true;
	}

	return false;
}

void AnecdoteTask::CheckSpecialTask(int task_type, int task_idx)
{
	if (task_type < 0 || task_type >= MAX_SPECIAL_ANECDOTE_TASK_TYPE)return;
	SpecialTaskInfo &task_info = m_special_param.task_list[task_type];

	if (task_info.IsBitSet(task_idx))return;

	this->GrantSpecialReward(task_type, task_idx);
}

void AnecdoteTask::StarSpecialTask(int task_type, int task_idx)
{
	if (m_mgr->GetRole()->InTeam())
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}
	//配置
	if (task_type < 0 || task_type >= MAX_SPECIAL_ANECDOTE_TASK_TYPE)return;
	SpecialTaskInfo &task_info = m_special_param.task_list[task_type];

	if (task_info.IsBitSet(task_idx))
	{
		return;// 该怪物已经打过了
	}

	const AnecdoteSpecialCfg * task_cfg = TASKPOOL->GetAnecdoteSpecialCfg(task_type, task_idx);
	if (NULL == task_cfg)return;
	
	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_mgr->GetRole(), BATTLE_MODE_SPECIAL_ANECDOTE_TASK, task_cfg->monster_id, task_type, task_idx, true);
}

void AnecdoteTask::GrantSpecialReward(int task_type, int task_idx)
{
	if (task_type < 0 || task_type >= MAX_SPECIAL_ANECDOTE_TASK_TYPE)return;
	SpecialTaskInfo &task_info = m_special_param.task_list[task_type];

	task_info.SetBit(task_idx);

	//配置奖励
	const AnecdoteSpecialCfg * task_cfg = TASKPOOL->GetAnecdoteSpecialCfg(task_type, task_idx);
	if (NULL == task_cfg)return;

	if ((int)task_cfg->reward_vec.size() > 0)
	{
		if (!m_mgr->GetKnapsack()->PutVec(task_cfg->reward_vec, PUT_REASON_SPECIAL_ANECDOTE))
		{
			//背包放不了，邮件补发
			m_mgr->GetKnapsack()->SendMail(&task_cfg->reward_vec[0], task_cfg->reward_vec.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}

		m_mgr->GetKnapsack()->NotifyGetItemList((short)task_cfg->reward_vec.size(), &task_cfg->reward_vec[0], GET_REASON_ANECDOTE_TASK);
	}

	this->SendSpecialAnecdoteTaskSingleInfo(task_type);
}

void AnecdoteTask::SendSpecialAnecdoteTaskInfo()
{
	Protocol::SCSpecialAnecdoteList info;
	for (int i = 0; i < MAX_SPECIAL_ANECDOTE_TASK_TYPE; i++)
	{
		info.task_list[i] = m_special_param.task_list[i];
	}
	m_mgr->NetSend(&info, sizeof(info));
}

void AnecdoteTask::SendSpecialAnecdoteTaskSingleInfo(int task_type)
{
	if (task_type < 0 || task_type >= MAX_SPECIAL_ANECDOTE_TASK_TYPE)return;
	
	Protocol::SCSpecialAnecdoteSingleList info;
	info.task_type = task_type;
	info.task_info = m_special_param.task_list[task_type];
	m_mgr->NetSend(&info, sizeof(info));
}

void AnecdoteTask::GmResetAnecdote(int anecdote_type)
{
	if (anecdote_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	m_param.task_info_list[anecdote_type].cur_task_accept_status = 0;
	m_param.task_info_list[anecdote_type].cur_task_id = 0;
	m_param.task_info_list[anecdote_type].fail_times = 0;
	m_param.task_info_list[anecdote_type].has_claim_reward = 0;
	m_param.task_info_list[anecdote_type].progress = 0;
	m_param.task_info_list[anecdote_type].succ_times = 0;

	this->SendTaskInfoChangeNotice(anecdote_type);

	gamelog::g_log_anecdote_task.printf(LL_INFO, "AnecdoteTask::GmResetAnecdote role[%d,%s] anecdote_type[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), anecdote_type);

	this->CheckAutoAcceptTask();
}

void AnecdoteTask::OnBattleSucc(int monster_group_id)
{
	const AnecdoteMonsterGroupCfg* cfg = TASKPOOL->GetAnecdoteMonsterGroupCfg(monster_group_id);
	if (NULL == cfg) return;

	int anecdote_task_type = cfg->anecdote_task_type;
	if (anecdote_task_type <= ANECDOTE_TASK_TYPE_INVALID || anecdote_task_type >= ANECDOTE_TASK_TYPE_MAX)
	{
		return;
	}

	int task_id = m_param.task_info_list[anecdote_task_type].cur_task_id;
	const TaskInfomaConfig* task_cfg = TASKPOOL->GetTaskCfgByID(task_id);
	if (NULL == task_cfg)
	{
		return;
	}

	if (task_cfg->task_condition != TASK_NEED_BEAT_MONGROUP)
	{
		return;
	}
	
	this->OnProgressChange(task_id, m_param.task_info_list[anecdote_task_type].progress, m_param.task_info_list[anecdote_task_type].progress + 1);
}

bool AnecdoteTask::CheckTaskStatus(int task_id)
{
	int anecdote_task_type = TASKPOOL->GetAnecdoteTaskType(task_id);
	if (ANECDOTE_TASK_TYPE_INVALID == anecdote_task_type)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (m_param.task_info_list[anecdote_task_type].succ_times > 0)
	{
		// 已完成
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_ALREADY_SUCC);
		return false;
	}

	if (ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED != m_param.task_info_list[anecdote_task_type].cur_task_accept_status)
	{
		// 未接任务
		m_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_ACCEPTED);
		return false;
	}

	return true;
}

int AnecdoteTask::CountFinishAnedoteTaskNum() const
{
	int num = 0;
	for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	{
		if (m_param.task_info_list[type].succ_times > 0)
		{
			num += 1;
		}
	}

	return num;
}

void AnecdoteTask::CheckBigEvent()
{
	const AnecdoteOtherCfg& cfg = TASKPOOL->GetAnecdoteOthetCfg();
	if (m_mgr->GetRole()->GetLevel() < cfg.big_event_open_level)
	{
		return;
	}

	if (m_param.big_event_version >= cfg.big_event_version)
	{
		return;
	}

	m_param.big_event_version = cfg.big_event_version;
	m_param.big_event_start_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_param.big_event_end_timestamp = m_param.big_event_start_timestamp + cfg.big_event_open_duration_hours * 60 * 60;
	m_param.big_event_reward_fetch_flag = 0;
	m_param.has_big_event_expire_send_reward_mail = 0;
	m_param.reward_version = ANECDOTE_BIG_EVENT_REWARD_VERSION;

	gamelog::g_log_anecdote_task.printf(LL_INFO, "CheckBigEvent reset data, role[%d,%s] version[%d] start[%u] end[%u] reward_version[%d]",
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.big_event_version, m_param.big_event_start_timestamp,
		m_param.big_event_end_timestamp, m_param.reward_version);

	this->SendBigEventInfo();
}

void AnecdoteTask::SendTaskInfo()
{
	Protocol::SCAnecdoteTaskInfoList list;
	list.count = 0;

	for (int type = ANECDOTE_TASK_TYPE_INVALID + 1; type < ANECDOTE_TASK_TYPE_MAX; ++type)
	{
		list.task_info_list[list.count].accept_status = m_param.task_info_list[type].cur_task_accept_status;
		list.task_info_list[list.count].anecdote_task_type = type;
		list.task_info_list[list.count].cur_task_id = m_param.task_info_list[type].cur_task_id;
		list.task_info_list[list.count].has_finish = m_param.task_info_list[type].succ_times > 0 ? 1 : 0;
		list.task_info_list[list.count].progress = m_param.task_info_list[type].progress;
		list.count += 1;
	}

	int send_len = sizeof(list) - sizeof(list.task_info_list) + list.count * sizeof(list.task_info_list[0]);
	m_mgr->NetSend(&list, send_len);	
}

void AnecdoteTask::SendBigEventInfo()
{
	Protocol::SCAnecdoteBigEventInfo info;
	info.reward_version = m_param.reward_version;
	info.anecdote_finish_num = this->CountFinishAnedoteTaskNum();
	info.is_big_event_open_now = this->IsBigEventOpenNow() ? 1 : 0;
	info.end_timestamp = m_param.big_event_end_timestamp;
	info.reward_fetch_status = m_param.big_event_reward_fetch_flag;

	m_mgr->NetSend(&info, sizeof(info));
}

void AnecdoteTask::SendSpecialInfo()
{
	Protocol::SCAnecdoteTaskSpecialInfo info;
	for (int i = 0; i < ANECDOTE_TASK_TYPE_MAX; ++i)
	{
		info.special_value[i] = m_common_param.special_value[i];
	}
	m_mgr->NetSend(&info, sizeof(info));
}

bool AnecdoteTask::IsBigEventOpenNow()
{
	const AnecdoteOtherCfg& cfg = TASKPOOL->GetAnecdoteOthetCfg();
	if (m_mgr->GetRole()->GetLevel() < cfg.big_event_open_level)
	{
		return false;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	return now >= m_param.big_event_start_timestamp && now <= m_param.big_event_end_timestamp;
}

void AnecdoteTask::OnRoleFetchBigEventReward(int reward_seq)
{
	const AnecdoteBigEventRewardCfg* cfg = TASKPOOL->GetAnecdoteBigEventRewardCfg(reward_seq, m_param.reward_version);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (this->CountFinishAnedoteTaskNum() < cfg->complete_anecdote_num)
	{
		m_mgr->NoticeNum(errornum::EN_COMPLETE_ANECDOTE_NUM_LIMIT);
		return;
	}

	if (!this->IsBigEventOpenNow())
	{
		m_mgr->NoticeNum(errornum::EN_BIG_EVENT_NOT_OPEN);
		return;
	}

	if (this->HasFetchBigEventReward(reward_seq))
	{
		m_mgr->NoticeNum(errornum::EN_BIG_EVENT_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList((short)cfg->item_num, cfg->item_list))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->PutList((short)cfg->item_num, cfg->item_list, PUT_REASON_ANECDOTE_BIG_EVENT))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.big_event_reward_fetch_flag |= (1u << reward_seq);
	
	this->SendBigEventInfo();

	gamelog::g_log_anecdote_task.printf(LL_INFO, "OnRoleFetchBigEventReward role[%d,%s] seq[%d] cur_flag[%u] reward_version[%d]",
		m_mgr->GetUid(), m_mgr->GetRoleName(), reward_seq, m_param.big_event_reward_fetch_flag, m_param.reward_version);
}

void AnecdoteTask::CheckBigEventExpireAutoGiveMail()
{
	if (m_param.has_big_event_expire_send_reward_mail > 0) return;
	if (m_param.big_event_version <= 0) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (0 == m_param.big_event_end_timestamp || now < m_param.big_event_end_timestamp)
	{
		return;
	}

	// 这里不检测有没有未领取的，因为即使没有未领取的，都要走下面赋值has_big_event_expire_send_reward_mail的逻辑
	//if (!this->HasUnfetchBigEventReward())
	//{
	//	return;
	//}

	m_param.has_big_event_expire_send_reward_mail = 1;
	this->SendBigEventRewardMail();

	gamelog::g_log_anecdote_task.printf(LL_INFO, "CheckBigEventExpireAutoGiveMail role[%d,%s] end_timestamp[%u] version[%d] reward_version[%d]",
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.big_event_end_timestamp, m_param.big_event_version, m_param.reward_version);
}

void AnecdoteTask::SendBigEventRewardMail()
{
	std::vector<int> seqs;
	this->GetUnfetchBigEventRewardSeqs(seqs);

	static MailContentParam contentparam; 
	contentparam.Reset();

	int reward_num = 0;
	for (size_t i = 0; i < seqs.size(); ++i)
	{
		const AnecdoteBigEventRewardCfg* cfg = TASKPOOL->GetAnecdoteBigEventRewardCfg(seqs[i], m_param.reward_version);
		if (NULL == cfg) continue;

		for (int k = 0; k < cfg->item_num && reward_num < MAX_ATTACHMENT_ITEM_NUM; ++k)
		{
			const ItemConfigData & reward = cfg->item_list[k];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
			if (item_base == NULL)
			{
				continue;
			}

			contentparam.item_list[reward_num].item_id = reward.item_id;
			contentparam.item_list[reward_num].num = reward.num;
			contentparam.item_list[reward_num].is_bind = (reward.is_bind) ? 1 : 0;
			contentparam.item_list[reward_num].invalid_time = item_base->CalInvalidTime();
			reward_num += 1;

			if (reward_num >= MAX_ATTACHMENT_ITEM_NUM)
			{
				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_anecdote_big_event_reward_by_mail_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_anecdote_big_event_reward_by_mail_content);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				}
				contentparam.Reset();
				reward_num = 0;
			}
		}
	}

	if (reward_num > 0)
	{
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_anecdote_big_event_reward_by_mail_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_anecdote_big_event_reward_by_mail_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void AnecdoteTask::GetUnfetchBigEventRewardSeqs(ARG_OUT std::vector<int>& seqs)
{
	int finish_num = this->CountFinishAnedoteTaskNum();
	int reward_version = m_param.reward_version;
	const std::vector<AnecdoteBigEventRewardCfg>* vec_ptr = TASKPOOL->GetAnecdoteBigEventRewardCfgVec(reward_version);
	if (NULL == vec_ptr)
	{
		return;
	}

	const std::vector<AnecdoteBigEventRewardCfg>& vec = *vec_ptr;
	seqs.reserve(vec.size());
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (finish_num < vec[i].complete_anecdote_num) continue;
		if (this->HasFetchBigEventReward(vec[i].seq)) continue;

		seqs.push_back(vec[i].seq);
	}
}

bool AnecdoteTask::HasFetchBigEventReward(int seq)
{
	return (m_param.big_event_reward_fetch_flag & (1u << seq)) > 0;
}

bool AnecdoteTask::HasUnfetchBigEventReward()
{
	std::vector<int> seqs;
	this->GetUnfetchBigEventRewardSeqs(seqs);
	return !seqs.empty();
}

void AnecdoteTask::Update(unsigned int now_second)
{
	this->CheckBigEventExpireAutoGiveMail();
}

