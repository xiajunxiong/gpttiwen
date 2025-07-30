#include "roleactivityyuanqihelp.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqihelpconfig.hpp"
#include "roleactivityqiyuankongmingdeng.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"
#include "other/funopen/funopenconfig.hpp"

RoleActivityYuanQiHelp::RoleActivityYuanQiHelp(int type)
	:RoleActivity(type)
{

}

RoleActivityYuanQiHelp::~RoleActivityYuanQiHelp()
{

}

void RoleActivityYuanQiHelp::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYuanQiHelp::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYuanQiHelp::Update(time_t now_second)
{
	
}

void RoleActivityYuanQiHelp::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_HELP))
	{
		return;
	}
	this->SendTaskInfo();
}

void RoleActivityYuanQiHelp::OnRoleLevelUp(int old_level, int to_level)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_HELP))
	{
		return;
	}

	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_YUAN_QI_HELP);
	if (NULL == cfg)
	{
		return ;
	}

	if (old_level < cfg->limit_level && to_level >= cfg->limit_level)
	{
		this->SendTaskInfo();
	}
}

void RoleActivityYuanQiHelp::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUAN_QI_HELP != rand_act_type)
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
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->RandTask();
		this->SetRoleDataChange();
	}
}

void RoleActivityYuanQiHelp::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_HELP))
	{
		return;
	}

	m_param.DayReset();
	this->RandTask();
	this->SendTaskInfo();
}

void RoleActivityYuanQiHelp::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0://请求下发信息
		{
			this->SendTaskInfo();
		}
		break;
	case 1://提交任务 param1 是否完成
		{
			this->CommitTask(raor->param_1 == 1);
		}
		break;
	default:
		break;
	}
}

void RoleActivityYuanQiHelp::CommitTask(bool is_finish)
{
	if (!is_finish)return;
	const RandActivityYuanQiHelpConfig * config = static_cast<const RandActivityYuanQiHelpConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_HELP));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const YuanQiHelpOtherCfg & other_cfg = config->GetOtherCfg();
	if (other_cfg.frequency <= m_param.today_play_times)return;
	const YuanQiHelpTaskCfg * task_cfg = config->GetTaskCfg(m_param.task_id);
	if (NULL != task_cfg)
	{
		const YuanQiHelpRewardCfg *  reward_cfg = config->GetRewardCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_HELP), task_cfg->item_id);
		if (NULL != reward_cfg)
		{
			if ((int)reward_cfg->m_reward_vec.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((int)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], false))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (int)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_YUAN_QI_HELP);
				}
				else
				{
					m_role_module_mgr->GetKnapsack()->PutList((int)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_RA_YUAN_QI_HELP);
				}
			}
		}
	}
	m_param.pre_task_id = m_param.task_id;
	for (int i = 0; i < MAX_YUAN_QI_HELP_MAX_TASK_LIST; i++)
	{
		if (m_param.pre_task_id_list[i] == 0)
		{
			m_param.pre_task_id_list[i] = m_param.task_id;
			break;
		}
	}
	m_param.PlayEnd();
	m_param.today_play_times += 1;
	this->RandTask();
	this->SendTaskInfo();

	this->OnJoinActivity();
}

void RoleActivityYuanQiHelp::SendTaskInfo()
{
	Protocol::SCRAYuanQiHelpInfo msg;
	
	msg.today_play_times = m_param.today_play_times;
	msg.task_id = m_param.task_id;
	msg.pre_task_id = m_param.pre_task_id;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYuanQiHelp::RandTask()
{
	const RandActivityYuanQiHelpConfig * config = static_cast<const RandActivityYuanQiHelpConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_HELP));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const YuanQiHelpOtherCfg & other_cfg = config->GetOtherCfg();
	if (other_cfg.frequency <= m_param.today_play_times)return;

	const YuanQiHelpTaskCfg * task_cfg = config->RandTaskCfg(m_param.pre_task_id_list);
	if (NULL != task_cfg)
	{
		m_param.task_id = task_cfg->task_id;
	}
}

void RoleActivityYuanQiHelp::OnJoinActivity()
{
	{
		RoleActivityQiYuanKongMingDeng * role_activity = static_cast<RoleActivityQiYuanKongMingDeng *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_QIYUAN_KONGMINGDENG_TASK_TYPE_VITALITY_HELPS);
		}
	}

	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_VITALITY_HELPS);
		}
	}
}
