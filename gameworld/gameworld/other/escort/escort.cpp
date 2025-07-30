#include "escort.hpp"
#include "escortconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "npc/npcmanager.h"
#include "item/knapsack.h"
#include "item/money.h"
#include "protocol/msgescort.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "scene/scene.h"
#include "other/prestige/prestige.hpp"
#include "servercommon/activesystemdef.hpp"
#include "other/activesystem/activesystem.hpp"
#include "task/taskmanager.h"
#include "gamelog.h"
#include "global/worldstatus/worldstatus.hpp"

Escort::Escort()
{
}

Escort::~Escort()
{
}

void Escort::Init(RoleModuleManager * role_module_manager)
{
	m_role_module_mgr = role_module_manager;
}

void Escort::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void Escort::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_role_module_mgr->GetCommonData().escort_today_count = 0;
	this->SendAllInfo();
}

void Escort::OnRoleLogin(long long last_save_time)
{
	if (0 != m_role_module_mgr->GetCommonData().escort_task_seq)
	{
		const EscortTaskCfg* task_cfg = LOGIC_CONFIG->GetEscortConfig()->GetTaskCfg(m_role_module_mgr->GetCommonData().escort_task_seq);
		if (NULL == task_cfg)
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, "Escort::OnRoleLogin config NOT FOUND, role[%d,%s] seq[%d]",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetCommonData().escort_task_seq);
			return;
		}

		if (task_cfg->progress_desc != ESCORT_TYPE_OTHER_TASK)
		{
			long long now_time = EngineAdapter::Instance().Time();
			if (now_time - last_save_time >= LOGIC_CONFIG->GetEscortConfig()->GetOtherCfg().offline_protect_time)
			{
				this->GiveupTask();
				m_role_module_mgr->NoticeNum(errornum::EN_ESCORT_OVERTIME);
			}
		}		
	}
}

void Escort::AcceptTask(int task_type, int task_id)
{
	CommonDataParam & cdp_ref = m_role_module_mgr->GetCommonData();
	if (0 != cdp_ref.escort_task_seq)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ESCORT_TASK_ALREADY_ACCEPTED);
		this->SendAllInfo();
		return;
	}

	if (m_role_module_mgr->GetRole()->InTeam())
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
		return;
	}

	const EscortOtherCfg & escort_other = LOGIC_CONFIG->GetEscortConfig()->GetOtherCfg();
	if (ESCORT_TYPE_OTHER_TASK != task_type && !NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), escort_other.accept_npc))
	{
		return;
	} 
	
	if ((ESCORT_TYPE_NORMAL == task_type || ESCORT_TYPE_SENIOR == task_type) && cdp_ref.escort_today_count >= escort_other.daily_num)
	{
		return;
	}

	int task_seq = LOGIC_CONFIG->GetEscortConfig()->GetTaskSeq(m_role_module_mgr->GetRole()->GetLevel(), task_type, task_id);
	if (0 == task_seq) return;

	const EscortTaskCfg * cfg = LOGIC_CONFIG->GetEscortConfig()->GetTaskCfg(task_seq);
	if (NULL == cfg || task_id != cfg->task_id) return;

	if (ESCORT_TYPE_CHALLENGE_TASK == cfg->progress_desc)
	{
		if (!m_role_module_mgr->GetTaskManager()->CheckRoleHasTask(task_id))
		{
			return;
		}
	}
	else if (ESCORT_TYPE_OTHER_TASK == cfg->progress_desc)
	{
		if (!m_role_module_mgr->GetTaskManager()->CheckRoleHasTask(task_id))
		{
			return;
		}
	}

	int rand_npc_id = LOGIC_CONFIG->GetEscortConfig()->RandNpc(cfg->NPC_group);
	if (0 == rand_npc_id) return;

	int need_coin = cfg->deposit;
	if (!m_role_module_mgr->GetMoney()->UseCoinBind(need_coin, __FUNCTION__))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	cdp_ref.escort_task_seq = task_seq;
	cdp_ref.escort_npc_id = rand_npc_id;

	// 非任务运镖才加次数
	if (ESCORT_TYPE_NORMAL == cfg->progress_desc || ESCORT_TYPE_SENIOR == cfg->progress_desc)
	{
		++cdp_ref.escort_today_count;
		cdp_ref.escort_end_time = escort_other.time_limit + static_cast<unsigned int>(EngineAdapter::Instance().Time());
		EventHandler::Instance().OnEscortDayActivity(m_role_module_mgr->GetRole());
	}
	
	m_role_module_mgr->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_ESCORT, __FUNCTION__);
	int rand_ms = RandomNum(20000, 40000);
	cdp_ref.encounter_escort_monster_ms = rand_ms;
	this->SendAllInfo();
	this->SendEscortNoticeArea();

	EventHandler::Instance().OnEscortStart(m_role_module_mgr->GetRole(), task_seq);
	WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "Escort");
}

void Escort::CommitTask()
{
	CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();
	if (0 == param_ref.escort_task_seq)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_TASK_NOT_ACCEPTEND);
		return;
	}

	const EscortTaskCfg * cfg = LOGIC_CONFIG->GetEscortConfig()->GetTaskCfg(param_ref.escort_task_seq);
	if (NULL == cfg || !NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), param_ref.escort_npc_id)) return;

	if (ESCORT_TYPE_NORMAL == cfg->progress_desc || ESCORT_TYPE_SENIOR == cfg->progress_desc)
	{
		const EscortNpcCfg* npc_cfg = LOGIC_CONFIG->GetEscortConfig()->GetEscortNpcCfg(cfg->NPC_group, param_ref.escort_npc_id);
		if (NULL != npc_cfg)
		{
			//增加声望
			m_role_module_mgr->GetPrestige()->AddPrestige(npc_cfg->village_id, npc_cfg->reward_prestige, __FUNCTION__);
		}
		if (cfg->reward_count > 0) m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward_list, PUT_REASON_ESCORT);
		if (cfg->reward_coin > 0) m_role_module_mgr->GetMoney()->AddCoinBind(cfg->reward_coin, __FUNCTION__);
	}

	//返回接镖奖励
	if (cfg->deposit > 0)
	{
		m_role_module_mgr->GetMoney()->AddCoinBind(cfg->deposit, "Escort::CommitTask deposit back!");
	}
	
	this->SendResultNotic(true);

	EventHandler::Instance().OnEscortOneTime(m_role_module_mgr->GetRole(), true, param_ref.escort_task_seq);
	this->ResetEscortStatus();
}

void Escort::GiveupTask()
{
	if (!this->HaveEscortTask())
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_TASK_NOT_ACCEPTEND);
		return;
	}

	this->SendResultNotic(false);

	EventHandler::Instance().OnEscortOneTime(m_role_module_mgr->GetRole(), false, m_role_module_mgr->GetCommonData().escort_task_seq);
	this->ResetEscortStatus();
}

void Escort::GiveUpAdvTask(int task_id)
{
	CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();
	if (0 != param_ref.escort_task_seq && 0 != task_id)
	{
		const EscortTaskCfg* escort_cfg = LOGIC_CONFIG->GetEscortConfig()->GetTaskCfg(param_ref.escort_task_seq);
		if (NULL != escort_cfg && task_id != escort_cfg->task_id)
		{
			return;
		}
		this->SendResultNotic(false);
		EventHandler::Instance().OnEscortOneTime(m_role_module_mgr->GetRole(), false, m_role_module_mgr->GetCommonData().escort_task_seq);
		this->ResetEscortStatus();
	}
}

void Escort::OnRoleFinishFight(int is_win)
{
	if (this->HaveEscortTask() && !is_win)
	{
		this->SendResultNotic(false);

		EventHandler::Instance().OnEscortOneTime(m_role_module_mgr->GetRole(), false, m_role_module_mgr->GetCommonData().escort_task_seq);
		this->ResetEscortStatus();
	}
}

bool Escort::HaveEscortTask()
{
	CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();
	return 0 != param_ref.escort_task_seq;
}

int Escort::GetEscortSeq()
{
	return m_role_module_mgr->GetCommonData().escort_task_seq;
}

int Escort::GetSpeedChangeRate()
{
	const EscortOtherCfg & escort_other = LOGIC_CONFIG->GetEscortConfig()->GetOtherCfg();
	return (0 != m_role_module_mgr->GetCommonData().escort_task_seq) ? -escort_other.slow_down : 0;
}

void Escort::SendAllInfo()
{
	const CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();

	Protocol::SCEscortAllInfo info;
	info.task_seq = param_ref.escort_task_seq;
	info.today_count = param_ref.escort_today_count;
	info.npc_id = param_ref.escort_npc_id;
	info.end_time = param_ref.escort_end_time;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Escort::SendResultNotic(bool is_win)
{
	Protocol::SCEscortResultNotic info;
	info.is_win = is_win ? 1 : 0;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Escort::ResetEscortStatus()
{
	CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();

	param_ref.escort_task_seq = 0;
	param_ref.escort_npc_id = 0;
	param_ref.escort_end_time = 0;
	m_role_module_mgr->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_ESCORT, __FUNCTION__);

	this->SendAllInfo();
	this->SendEscortNoticeArea();
}

void Escort::SendEscortNoticeArea()
{
	const CommonDataParam & param_ref = m_role_module_mgr->GetCommonData();

	static Protocol::SCEscortStatusChangeNotice notice;
	notice.escort_seq = param_ref.escort_task_seq;
	notice.reserve_sh = 0;
	notice.role_obj_id = m_role_module_mgr->GetRole()->GetId();

	m_role_module_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_role_module_mgr->GetRole()->GetObserHandle(), &notice, sizeof(notice));
}
