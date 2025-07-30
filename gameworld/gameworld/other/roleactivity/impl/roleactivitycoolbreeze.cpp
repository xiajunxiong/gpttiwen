#include "roleactivitycoolbreeze.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgracoolsummer.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycoolbreezeconfig.hpp"

RoleActivityCoolBreeze::RoleActivityCoolBreeze(int type)
	:RoleActivity(type)
{
}

RoleActivityCoolBreeze::~RoleActivityCoolBreeze()
{
}

void RoleActivityCoolBreeze::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACoolBreezeParam));
}

void RoleActivityCoolBreeze::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RACoolBreezeParam));
}

void RoleActivityCoolBreeze::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COOL_BREEZE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE);

	unsigned int now = static_cast<unsigned int>(now_second);
	if (now >= m_param.next_fixe_task_refresh_timestamp)
	{
		const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
		if (NULL == config)
		{
			return;
		}
		const RACoolBreezeOtherCfg & other_cfg = config->GetOtherCfg();
		int open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COOL_BREEZE);
		const tm * local_time = EngineAdapter::Instance().LocalTime();

		bool is_day_not_fixe_refresh = false;		//固定任务今天刷新时间是否还未过去
		if (local_time->tm_hour == other_cfg.fixe_refresh_hour)
		{
			if (local_time->tm_min < other_cfg.fixe_refresh_minute)	//今天刷新时间未过
			{
				is_day_not_fixe_refresh = true;
			}
		}
		else if (local_time->tm_hour < other_cfg.fixe_refresh_hour)
		{
			is_day_not_fixe_refresh = true;
		}
		if (is_day_not_fixe_refresh)
		{
			open_day -= 1;
		}

		if (open_day <= m_param.last_refresh_fixe_open_day)
		{
			return;
		}
		int refresh_num = open_day - m_param.last_refresh_fixe_open_day;
		if (refresh_num > MAX_RA_COOL_BREEZE_FIXE_TASK_NUM)
		{
			refresh_num = MAX_RA_COOL_BREEZE_FIXE_TASK_NUM;
		}
		m_param.last_refresh_fixe_open_day = open_day;
		m_param.next_fixe_task_refresh_timestamp = now + EngineAdapter::Instance().NextDayInterval(other_cfg.fixe_refresh_hour, other_cfg.fixe_refresh_minute, 0);
		std::set<int> index_list;
		for (int i = 0; i < refresh_num; i++)
		{
			short suit_refresh_index = 0;
			short min_open_day = MAX_INT16;
			bool is_find = false;
			for (int k = 0; k < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; k++)
			{
				if (index_list.find(k) != index_list.end()) continue;

				if (m_param.fixe_task_list[k].type <= 0)
				{
					index_list.insert(k);
					m_param.fixe_task_list[k].Reset();
					is_find = true;
					break;
				}
				if (m_param.fixe_task_list[k].open_day < min_open_day)
				{
					suit_refresh_index = i;
					min_open_day = m_param.fixe_task_list[k].open_day;
				}
			}
			if (!is_find)
			{
				index_list.insert(suit_refresh_index);
				this->OnTimeOutCheckTaskStatus(m_param.fixe_task_list[suit_refresh_index]);
				m_param.fixe_task_list[suit_refresh_index].Reset();
			}
		}

		//m_param.ResetSuitRefreshPos(refresh_num);
		std::vector<int> has_type_list;
		m_param.GetTypeList(&has_type_list);
		for (std::set<int>::iterator it = index_list.begin(); it != index_list.end(); ++it)
		{
			config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, m_param.fixe_task_list[*it], &has_type_list);
			m_param.fixe_task_list[*it].open_day = open_day--;
			has_type_list.push_back(m_param.fixe_task_list[*it].type);
		}
		/*	for (int i = 0; i < refresh_num; i++)
			{
				RACoolBreezeTaskInfo & task_info = m_param.GetSuitRefreshPosInfo();
				config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, task_info, &has_type_list);
				task_info.open_day = open_day--;
				has_type_list.push_back(task_info.type);
			}*/
		this->SetRoleDataChange();
		this->SendInfo();
	}

	if (now >= m_param.next_random_task_refresh_timestamp)
	{
		const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
		if (NULL == config)
		{
			return;
		}
		const RACoolBreezeOtherCfg & other_cfg = config->GetOtherCfg();
		int open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COOL_BREEZE);
		const tm * local_time = EngineAdapter::Instance().LocalTime();

		bool is_day_not_random_refresh = false;		//随机任务今天刷新时间是否还未过去
		if (local_time->tm_hour == other_cfg.random_refresh_hour)
		{
			if (local_time->tm_min < other_cfg.random_refresh_minute)	//今天刷新时间未过
			{
				is_day_not_random_refresh = true;
			}
		}
		else if (local_time->tm_hour < other_cfg.random_refresh_hour)
		{
			is_day_not_random_refresh = true;
		}
		if (is_day_not_random_refresh)
		{
			open_day -= 1;
		}

		if (open_day <= m_param.last_refresh_random_open_day)
		{
			return;
		}
		int refresh_num = open_day - m_param.last_refresh_random_open_day;
		if (refresh_num > MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM)
		{
			refresh_num = MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM;
		}
		m_param.last_refresh_random_open_day = open_day;
		m_param.next_random_task_refresh_timestamp = now + EngineAdapter::Instance().NextDayInterval(other_cfg.random_refresh_hour, other_cfg.random_refresh_minute, 0);

		//随机任务与固定任务区别在于,随机任务即使有任务栏,超2天还是会被刷新
		for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
		{
			if (m_param.random_task_list[i].type > 0 && m_param.last_refresh_random_open_day >= m_param.random_task_list[i].open_day + 2)
			{
				this->OnTimeOutCheckTaskStatus(m_param.random_task_list[i]);
				m_param.random_task_list[i].Reset();
			}
		}
		std::vector<int> has_type_list;
		m_param.GetTypeList(&has_type_list, false);
		for (int i = 0; i < refresh_num; i++)
		{
			RACoolBreezeTaskInfo & task_info = m_param.GetSuitRefreshPosInfo(false);
			config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM, task_info, &has_type_list);
			task_info.open_day = open_day--;
			has_type_list.push_back(task_info.type);
		}
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityCoolBreeze::OnConsumeGold(long long consume_gold)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);
	this->OnTaskEventInfoChange(RA_COOL_BREEZE_TASK_TYPE_CONSUME_GOLD, consume_gold, 0);
}

void RoleActivityCoolBreeze::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COOL_BREEZE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE);
	this->SendInfo();
}

void RoleActivityCoolBreeze::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_COOL_BREEZE != rand_act_type)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}
	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
		if (NULL == config)
		{
			return;
		}
		const RACoolBreezeOtherCfg & other_cfg = config->GetOtherCfg();
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COOL_BREEZE);
		const tm * local_time = EngineAdapter::Instance().LocalTime();
		unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();

		bool is_day_not_fixe_refresh = false;		//固定任务今天刷新时间是否还未过去
		if (local_time->tm_hour == other_cfg.fixe_refresh_hour)
		{
			if (local_time->tm_min < other_cfg.fixe_refresh_minute)	//今天刷新时间未过
			{
				is_day_not_fixe_refresh = true;
			}
		}
		else if (local_time->tm_hour < other_cfg.fixe_refresh_hour)
		{
			is_day_not_fixe_refresh = true;
		}
		m_param.next_fixe_task_refresh_timestamp = now_second + EngineAdapter::Instance().NextDayInterval(other_cfg.fixe_refresh_hour, other_cfg.fixe_refresh_minute, 0);

		//随机任务
		bool is_day_not_random_refresh = false;		//随机任务今天刷新时间是否还未过去
		if (local_time->tm_hour == other_cfg.random_refresh_hour)
		{
			if (local_time->tm_min < other_cfg.random_refresh_minute)	//今天刷新时间未过
			{
				is_day_not_random_refresh = true;
			}
		}
		else if (local_time->tm_hour < other_cfg.random_refresh_hour)
		{
			is_day_not_random_refresh = true;
		}
		m_param.next_random_task_refresh_timestamp = now_second + EngineAdapter::Instance().NextDayInterval(other_cfg.random_refresh_hour, other_cfg.random_refresh_minute, 0);

		if (1 == open_day)		//第一天是默认先刷固定和随机任务,下一次刷新时间要到明天
		{
			if (is_day_not_fixe_refresh)
			{
				m_param.next_fixe_task_refresh_timestamp += SECOND_PER_DAY;
			}
			if (is_day_not_random_refresh)
			{
				m_param.next_random_task_refresh_timestamp += SECOND_PER_DAY;
			}
			config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, m_param.fixe_task_list[0]);
			m_param.fixe_task_list[0].open_day = 1;
			m_param.last_refresh_fixe_open_day = 1;
			config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM, m_param.random_task_list[0]);
			m_param.random_task_list[0].open_day = 1;
			m_param.last_refresh_random_open_day = 1;
		}
		else
		{
			int last_fixe_open_day = open_day;
			int last_random_open_day = open_day;
			if (is_day_not_fixe_refresh)
			{
				last_fixe_open_day -= 1 ;
			}
			if (is_day_not_random_refresh)
			{
				last_random_open_day -= 1;
			}
			std::vector<int> has_type_list;
			m_param.last_refresh_fixe_open_day = last_fixe_open_day;
			for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM && last_fixe_open_day > 0; i++)
			{
				config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, m_param.fixe_task_list[i], &has_type_list);
				m_param.fixe_task_list[i].open_day = last_fixe_open_day--;
				has_type_list.push_back(m_param.fixe_task_list[i].type);
			}
			
			has_type_list.clear();
			m_param.last_refresh_random_open_day = last_random_open_day;
			for (int i = 0; i < MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM && last_random_open_day > 0; i++)
			{
				config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM, m_param.random_task_list[i], &has_type_list);
				m_param.random_task_list[i].open_day = last_random_open_day--;
				has_type_list.push_back(m_param.random_task_list[i].type);
			}
		}
		this->SetRoleDataChange();
	}

}

void RoleActivityCoolBreeze::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COOL_BREEZE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COOL_BREEZE);
	switch (raor->opera_type)
	{
		case Protocol::RA_COOL_BREEZE_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_COOL_BREEZE_REQ_TYPE_COMMIT_TASK:
		{
			this->OnCommitTask(raor->param_1, (0 != raor->param_2));
		}
		break;
		case Protocol::RA_COOL_BREEZE_REQ_TYPE_REFRESH_TASK:
		{
			this->OnRefreshFixeTask(raor->param_1);
		}
		break;
	}
}

void RoleActivityCoolBreeze::OnTaskEventInfoChange(int task_type, int param1, int param2)
{
	if(task_type <= RA_COOL_BREEZE_TASK_TYPE_BEGIN || task_type >= RA_COOL_BREEZE_TASK_TYPE_MAX) return;
	
	UNSTD_STATIC_CHECK(11 == RA_COOL_BREEZE_TASK_TYPE_MAX);
	for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM + MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM; i++)
	{
		RACoolBreezeTaskInfo & task_info = this->GetTaskInfoByIndex(i);
		if(task_info.task_type != task_type) continue;

		bool is_single_send = true;
		switch (task_type)
		{
		case RA_COOL_BREEZE_TASK_TYPE_PASS_CRYSTAL:
		case RA_COOL_BREEZE_TASK_TYPE_ACTIVE:
		case RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_NUM:
		case RA_COOL_BREEZE_TASK_TYPE_CONSUME_COIN:
		case RA_COOL_BREEZE_TASK_TYPE_CONSUME_GOLD:
		case RA_COOL_BREEZE_TASK_TYPE_CONSUME_ENERGY:
		case RA_COOL_BREEZE_TASK_TYPE_XUNBAO_NUM:
		case RA_COOL_BREEZE_TASK_TYPE_SEAL_PET:
		case RA_COOL_BREEZE_TASK_TYPE_PET_SKILL_LEARN:
		{
			task_info.schedule += param1;
		}
		break;
		case RA_COOL_BREEZE_TASK_TYPE_STRENGTHEN_LEVEL:
		{
			task_info.schedule = (param1 > task_info.schedule) ? param1 : task_info.schedule;
		}
		break;
		default:
			is_single_send = false;
			break;
		}
		if (is_single_send)
		{
			bool is_fixe = (i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM) ? true : false;
			this->OnSendSingleInfo(is_fixe ? i : i - MAX_RA_COOL_BREEZE_FIXE_TASK_NUM, is_fixe ? RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE : RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM, task_info);
			this->SetRoleDataChange();
		}
	}
}

void RoleActivityCoolBreeze::SendInfo()
{
	Protocol::SCRACoolBreezeInfo info;
	info.last_refresh_fixe_open_day = m_param.last_refresh_fixe_open_day;
	info.last_refresh_random_open_day = m_param.last_refresh_random_open_day;
	info.next_fixe_task_refresh_timestamp = m_param.next_fixe_task_refresh_timestamp;
	info.next_random_task_refresh_timestamp = m_param.next_random_task_refresh_timestamp;
	memcpy(info.fixe_task_list, m_param.fixe_task_list, sizeof(info.fixe_task_list));
	memcpy(info.random_task_list, m_param.random_task_list, sizeof(info.random_task_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCoolBreeze::OnSendSingleInfo(int index, short task_group_type, RACoolBreezeTaskInfo & task_info)
{
	Protocol::SCRACoolBreezeSingleInfo info;
	info.index = index;
	info.task_group_type = task_group_type;
	info.task_info = task_info;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCoolBreeze::OnCommitTask(int index, bool is_rand_task)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL == config)
	{
		return;
	}
	
	const RACoolBreezeTaskCfg * task_cfg = NULL;
	RACoolBreezeTaskInfo* task_info = NULL;
	if (is_rand_task)
	{	
		if (index < 0 || index >= MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM || m_param.random_task_list[index].type <= 0) return;

		task_cfg = config->GetTaskCfgByType(m_param.random_task_list[index].type);
		task_info = &m_param.random_task_list[index];
	}
	else
	{
		if (index < 0 || index >= MAX_RA_COOL_BREEZE_FIXE_TASK_NUM || m_param.fixe_task_list[index].type <= 0) return;

		task_cfg = config->GetTaskCfgByType(m_param.fixe_task_list[index].type);
		task_info = &m_param.fixe_task_list[index];
	}

	if (NULL == task_cfg || NULL == task_info)
	{
		return;
	}

	if (task_info->schedule < task_cfg->param1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
		return;
	}

	if (!task_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)task_cfg->reward_list.size(), &task_cfg->reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)task_cfg->reward_list.size(), &task_cfg->reward_list[0], PUT_REASON_COOL_BREEZE))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&task_cfg->reward_list[0], (int)task_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_COOL_BREEZE);
		}
	}
	task_info->Reset();
	this->SetRoleDataChange();
	this->OnSendSingleInfo(index, is_rand_task ? RA_COOL_BREEZE_TASK_GROUP_TYPE_RANDOM : RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, *task_info);
	this->OnParticipateRA(__FUNCTION__);
	
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COOL_BREEZE, index, task_cfg->type);
}

void RoleActivityCoolBreeze::OnRefreshFixeTask(int index)
{
	if (index < 0 || index >= MAX_RA_COOL_BREEZE_FIXE_TASK_NUM || m_param.fixe_task_list[index].type <= 0) return;
	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL == config)
	{
		return;
	}
	const RACoolBreezeOtherCfg & other_cfg = config->GetOtherCfg();
	if (m_param.fixe_task_list[index].refresh_num >= other_cfg.refresh_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COOL_BREEZE_NOT_REFRESH);
		return;
	}
	if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.gold, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	std::vector<int> has_type_list;
	for (int i = 0; i < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM; i++)
	{
		if (m_param.fixe_task_list[i].type > 0)
		{
			has_type_list.push_back(m_param.fixe_task_list[i].type);
		}
	}
	config->RandTask(RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, m_param.fixe_task_list[index], &has_type_list);
	m_param.fixe_task_list[index].schedule = 0;
	m_param.fixe_task_list[index].refresh_num += 1;

	this->SetRoleDataChange();
	this->OnSendSingleInfo(index, RA_COOL_BREEZE_TASK_GROUP_TYPE_FIXE, m_param.fixe_task_list[index]);
	this->OnParticipateRA(__FUNCTION__);
	
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], type[%d] task_day[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COOL_BREEZE, index, m_param.fixe_task_list[index].type, m_param.fixe_task_list[index].open_day);
}

void RoleActivityCoolBreeze::OnTimeOutCheckTaskStatus(const RACoolBreezeTaskInfo & task_info)
{
	if(task_info.task_type <= 0) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityCoolBreezeConfig * config = static_cast<const RandActivityCoolBreezeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COOL_BREEZE));
	if (NULL == config)
	{
		return;
	}

	const RACoolBreezeTaskCfg * task_cfg = config->GetTaskCfgByType(task_info.type);
	if(NULL == task_cfg	|| task_info.schedule < task_cfg->param1) return;

	if (!task_cfg->reward_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&task_cfg->reward_list[0], (int)task_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, false, RAND_ACTIVITY_TYPE_COOL_BREEZE);
	}
	this->SetRoleDataChange();
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], type[%d] open_day[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COOL_BREEZE, task_cfg->type, task_info.open_day);
}

//索引由外部调用检测
RACoolBreezeTaskInfo & RoleActivityCoolBreeze::GetTaskInfoByIndex(int index)
{
	if (index < MAX_RA_COOL_BREEZE_FIXE_TASK_NUM)
	{
		return m_param.fixe_task_list[index];
	}
	return m_param.random_task_list[index - MAX_RA_COOL_BREEZE_FIXE_TASK_NUM];
}
