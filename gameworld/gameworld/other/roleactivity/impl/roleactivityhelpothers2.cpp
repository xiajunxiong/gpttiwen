#include "roleactivityhelpothers2.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "npc/npcmanager.h"
#include "obj/character/role.h"

#include "other/pet/pet.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgrahelpothers2.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhelpothersconfig2.hpp"

RoleActivityHelpOthers2::RoleActivityHelpOthers2(int type)
	:RoleActivity(type)
{
}

RoleActivityHelpOthers2::~RoleActivityHelpOthers2()
{
}

void RoleActivityHelpOthers2::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityHelpOthers2::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityHelpOthers2::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HELP_OTHERS_2))
	{
		return;
	}
	this->SendInfo();
}

void RoleActivityHelpOthers2::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_HELP_OTHERS_2 != rand_act_type)
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
		this->SetRoleDataChange();
	}
}

void RoleActivityHelpOthers2::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HELP_OTHERS_2))
	{
		return;
	}

	m_param.DayReset();
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityHelpOthers2::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HELP_OTHERS_2))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HELP_OTHERS_2);

	switch (raor->opera_type)
	{
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_START:
		{
			this->OnStart();
		}
		break;
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_ACCEPT:
		{
			this->OnAccept();
		}
		break;
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_DELIVERY:
		{
			this->OnDelivery(raor->param_1, raor->param_2);
		}
		break;
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_TALK:
		{
			this->OnTalkNPC();
		}
		break;
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_DELIVERY_PET:
		{
			this->OnCommitPet(raor->param_1);
		}
		break;
		case Protocol::RA_HELP_OTHERS_REQ_TYPE_2_BATTLE_FINISH:
		{
			this->OnBattleFinish();
		}
	default:
		break;
	}

}

void RoleActivityHelpOthers2::AddTaskProgress(int num)
{
	if(num <= 0 || HELP_OTHERS_TASK_TYPE_2_BATTLE != m_param.task_type) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HELP_OTHERS_2))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HELP_OTHERS_2);

	m_param.progress += num;
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityHelpOthers2::SendInfo()
{
	Protocol::SCRAHelpOthersInfo2 info;
	info.pass_count = m_param.pass_count;
	info.task_id = m_param.task_id;
	info.task_status = m_param.task_status;
	info.progress = m_param.progress;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityHelpOthers2::OnStart()
{
	if (m_param.pass_count != 0 || m_param.task_id != 0)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHelpOthersConfig2 * config = static_cast<const RandActivityHelpOthersConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HELP_OTHERS_2));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (m_role_module_mgr->GetRole()->GetLevel() < config->GetOtherCfg().level_open)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	const RAHelpOthersTaskInfoCfg2 * task_cfg = config->GetRankTaskCfg(m_param.task_id, true, true);
	if (NULL == task_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	m_param.task_id = task_cfg->task_id;
	m_param.task_status = HELP_OTHERS_TASK_STATUS_2_NOT_ACCEPT;
	m_param.task_type = task_cfg->task_type;
	m_param.accept_npc_seq = task_cfg->accept_npc_seq;
	m_param.commit_npc_seq = task_cfg->commit_npc_seq;
	m_param.param1 = task_cfg->param1;
	m_param.param2 = task_cfg->param2;
	if (HELP_OTHERS_TASK_TYPE_2_DELIVERY_PET == m_param.task_type)
	{
		m_param.next_rank_pet_task_type = m_param.pass_count + 1 + config->GetOtherCfg().space_times;
	}
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityHelpOthers2::OnAccept()
{
	if (m_param.task_id <= 0 || m_param.task_status != HELP_OTHERS_TASK_STATUS_2_NOT_ACCEPT)
	{
		return;
	}
	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.accept_npc_seq)) return;

	if (HELP_OTHERS_TASK_TYPE_2_INSTEAD_OF_SEND == m_param.task_type)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(m_param.param1, m_param.param2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(m_param.param1, true, m_param.param2), PUT_REASON_HELP_OTHERS_2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.task_status = HELP_OTHERS_TASK_STATUS_2_ACCEPT;
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityHelpOthers2::OnDelivery(int column, int index)
{
	if (HELP_OTHERS_TASK_TYPE_2_DELIVERY != m_param.task_type && HELP_OTHERS_TASK_TYPE_2_INSTEAD_OF_SEND != m_param.task_type) return;
	if (HELP_OTHERS_TASK_STATUS_2_ACCEPT != m_param.task_status) return;
	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.commit_npc_seq)) return;
	 
	static ItemGridData item;
	item.Reset();
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(column, index, &item))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (item.item_id != m_param.param1)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, m_param.param2, __FUNCTION__))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_ITEM_ENOUGH_COMMIT_ERROR);
		return;
	}

	this->OnCommitTask();
}

void RoleActivityHelpOthers2::OnTalkNPC()
{
	if (HELP_OTHERS_TASK_TYPE_2_TALK != m_param.task_type) return;
	if (HELP_OTHERS_TASK_STATUS_2_ACCEPT != m_param.task_status) return;

	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.commit_npc_seq)) return;

	this->OnCommitTask();
}

void RoleActivityHelpOthers2::OnCommitPet(int pet_index)
{
	if (HELP_OTHERS_TASK_TYPE_2_DELIVERY_PET != m_param.task_type) return;
	if (HELP_OTHERS_TASK_STATUS_2_ACCEPT != m_param.task_status) return;

	const PetParam * pet_param = m_role_module_mgr->GetPet()->GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (pet_param->level != 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_CHAIN_COMMIT_PET_ERROR);
		return;
	}
	if (!m_role_module_mgr->GetPet()->SystemTakeBackPet(pet_index, m_param.param1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_CHAIN_PET_ERR);
		return;
	}
	this->OnCommitTask();
}

void RoleActivityHelpOthers2::OnBattleFinish()
{
	if (HELP_OTHERS_TASK_TYPE_2_BATTLE != m_param.task_type) return;
	if (HELP_OTHERS_TASK_STATUS_2_ACCEPT != m_param.task_status) return;

	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.commit_npc_seq)) return;

	if (m_param.progress < m_param.param2)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_FINISH_BATTLE_TASK);
		return;
	}

	this->OnCommitTask();
}

void RoleActivityHelpOthers2::OnCommitTask()		
{
	//判断都在外部完成
	if (m_param.pass_count < 0)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHelpOthersConfig2 * config = static_cast<const RandActivityHelpOthersConfig2 *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HELP_OTHERS_2));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAHelpOthersOtherCfg2 & other_cfg = config->GetOtherCfg();
	if (m_param.pass_count >= other_cfg.ring_up)
	{
		return;
	}

	const RAHelpOthersTaskRewardCfg2* reward_cfg = config->GetTaskRewardCfg(ramgr, m_param.pass_count + 1);
	if (NULL == reward_cfg)
	{
		return;
	}
	const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == level_cfg) return;

	int add_exp = static_cast<int>((level_cfg->exp * reward_cfg->exp) / RAHelpOthersTaskRewardCfg2::EXP_BASE_NUM);

	if (0 != reward_cfg->group_id)
	{
		const RAHelpOthersRewardCfg2 * reward_group_cfg = config->GetRewardCfg(reward_cfg->group_id);
		if (NULL == reward_group_cfg)
		{
			return;
		}
		if (!reward_group_cfg->reward_list.empty())
		{
			bool is_pus_succ = false;
			if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_group_cfg->reward_list.size(), &reward_group_cfg->reward_list[0]))
			{
				if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_group_cfg->reward_list.size(), &reward_group_cfg->reward_list[0], PUT_REASON_HELP_OTHERS_2))
				{
					is_pus_succ = true;
				}
			}
			if (!is_pus_succ)
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&reward_group_cfg->reward_list[0], (int)reward_group_cfg->reward_list.size(), SEND_MAIL_TYPE_HELP_OTHERS_2, true);
			}
		}
	}
	if (add_exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_HELP_OTHERS_2, __FUNCTION__);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], task_id[%d]", __FUNCTION__, 
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), 
		RAND_ACTIVITY_TYPE_HELP_OTHERS_2, m_param.task_id);
	int old_task_id = m_param.task_id;
	m_param.ClearTaskInfo();
	m_param.pass_count += 1;
	if (m_param.pass_count == other_cfg.ring_up)
	{
		m_param.task_status = HELP_OTHERS_TASK_STATUS_2_FINISH;
	}
	else
	{
		bool is_can_rand_pet_task = (0 != m_param.next_rank_pet_task_type) && m_param.pass_count + 1 < m_param.next_rank_pet_task_type ? false : true;
		const RAHelpOthersTaskInfoCfg2 * task_cfg = config->GetRankTaskCfg(old_task_id, is_can_rand_pet_task);
		if (NULL != task_cfg)
		{
			m_param.task_id = task_cfg->task_id;
			m_param.task_status = HELP_OTHERS_TASK_STATUS_2_NOT_ACCEPT;
			m_param.task_type = task_cfg->task_type;
			m_param.accept_npc_seq = task_cfg->accept_npc_seq;
			m_param.commit_npc_seq = task_cfg->commit_npc_seq;
			m_param.param1 = task_cfg->param1;
			m_param.param2 = task_cfg->param2;
			if (HELP_OTHERS_TASK_TYPE_2_DELIVERY_PET == m_param.task_type)
			{
				m_param.next_rank_pet_task_type = m_param.pass_count + 1 + config->GetOtherCfg().space_times;
			}
		}
	}
	this->SetRoleDataChange();
	this->SendInfo();
}
