#include "roleactivityyunzecutepet.hpp"
#include "config/randactivityconfig/impl/randactivityyunzecutepetconfig.hpp"
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

RoleActivityYunZeCutePet::RoleActivityYunZeCutePet(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeCutePet::~RoleActivityYunZeCutePet()
{

}

void RoleActivityYunZeCutePet::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeCutePet::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeCutePet::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET))
	{
		return;
	}

	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config) return;

	std::vector<RAYZCutePetTaskCfg> task_list;
	if (config->GetTaskCfg(ramgr, ARG_OUT task_list) < 0) return;

	for (int i = 0; i < (int)task_list.size(); ++i)
	{
		if (task_list[i].task_id == RAYZCutePet::TASK_TYPE_LOGIN)
		{
			if (m_param.task_status.IsBitSet(task_list[i].seq))
			{
				return;
			}

			this->EventCutePet();
		}
	}
}

void RoleActivityYunZeCutePet::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET))
	{
		return;
	}

	Protocol::SCRAYunZeCutePetChallengeInfo cmsg;
	cmsg.is_challenge_status = -1;
	cmsg.challenge_status = m_param.challenge_status;

	m_role_module_mgr->NetSend(&cmsg, sizeof(cmsg));

	if(m_param.ra_login_timestamp ==0 || m_param.ra_login_timestamp)

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeCutePet::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET != rand_act_type)
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

void RoleActivityYunZeCutePet::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET))
	{
		return;
	}

	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config) return;

	m_param.DayReset();

	this->RASendInfo();    
	this->SetRoleDataChange();
}

void RoleActivityYunZeCutePet::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();			// 发协议
		}
		break;
	case 1:
		{
			this->Fighting(raor->param_1);		// 挑战 param1--宠物id
		}
		break;
	default:
		break;
	}
}

void RoleActivityYunZeCutePet::AddTaskParam(int task_type, int task_param)
{
	if (task_type <= RAYZCutePet::TASK_TYPE_BEGIN || task_type >= RAYZCutePet::TASK_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET))
		return;

	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::vector<RAYZCutePetTaskCfg> task_list;

	if (config->GetTaskCfg(ramgr, ARG_OUT task_list) < 0)
	{
		return;
	}

	int i = 0;
	for (; i < (int)task_list.size(); ++i)
	{
		if (task_list[i].task_id == task_type)
		{
			if (m_param.task_status.IsBitSet(task_list[i].seq))
			{
				return;
			}

			m_param.task_param_list[task_type] += task_param;
			break;
		}
	}
	if (i == (int)task_list.size()) return;

	this->CountTask(task_type);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeCutePet::RASendInfo()
{
	Protocol::SCRAYunZeCutePetInfo msg;
	memset(msg.today_fraction, 0, sizeof(msg.today_fraction));
	memset(msg.task_param_list, 0, sizeof(msg.task_param_list));

	msg.pet_combat_num = m_param.pet_combat_num;
	memcpy(msg.today_fraction, m_param.today_fraction, sizeof(msg.today_fraction));
	memcpy(msg.task_param_list,m_param.task_param_list,sizeof(msg.task_param_list));
	msg.task_status = m_param.task_status;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityYunZeCutePet::Fighting(int monster_id)
{
	if (monster_id < 0) return;

	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZCutePetBasicsCfg * monster_cfg = config->GetMonsterByIdCfg(monster_id);
	if (NULL == monster_cfg)
	{
		return;
	}

	if (m_param.challenge_status.IsBitSet(monster_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PET_CHALLENGE_SUCCESS);
		return;
	}

	int is_challenge_status = m_param.pet_combat_num > monster_cfg->combat ? 1 : (m_param.pet_combat_num == monster_cfg->combat ? 2 : 0);

	if (is_challenge_status == 1)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL == ramgr) return;

		const RAYZCutePetRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, monster_cfg->reward_group);
		if (NULL == reward_cfg) return;

		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward.size(), &reward_cfg->reward[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward.size(), &reward_cfg->reward[0], PUT_REASON_RA_YUNZE_CUTE_PET);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward[0], (int)reward_cfg->reward.size(), SEND_MAIL_TYPE_YUNZE_CUTE_PET, true);
		}

		m_param.challenge_status.SetBit(monster_id);

		std::string gamelog_str;
		for (int i = 0; i < (int)reward_cfg->reward.size(); ++i)
		{
			gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", reward_cfg->reward[i].item_id, reward_cfg->reward[i].num);
		}

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeCutePet::%s role[%d, %s] activity_type[%d] monster_id[%d] reward[%s] ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET, monster_id, gamelog_str.c_str());
	}
	else
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeCutePet::%s role[%d, %s] activity_type[%d] monster_id[%d] challenge_failed ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET, monster_id);
	}

	Protocol::SCRAYunZeCutePetChallengeInfo msg;
	msg.is_challenge_status = is_challenge_status;
	msg.challenge_status = m_param.challenge_status;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeCutePet::CountTask(int task_type)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::vector<RAYZCutePetTaskCfg> task_list;

	if (config->GetTaskCfg(ramgr,ARG_OUT task_list) < 0)
	{
		return;
	}

	int i = 0;
	for (; i < (int)task_list.size(); ++i)
	{
		if (task_list[i].task_id == task_type)
		{
			if (m_param.task_param_list[task_type] >= task_list[i].task_state)
			{
				m_param.task_param_list[task_type] = task_list[i].task_state;

				if (m_param.task_status.IsBitSet(task_list[i].seq))
				{
					return;
				}

				m_param.pet_combat_num += task_list[i].task_combat;
			
				m_param.task_status.SetBit(task_list[i].seq);
				break;
			}
		}
	}

	if (i == (int)task_list.size()) return;

	int act_open_day = config->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);
	if (act_open_day > RAYZCutePet::ACTIVITY_DAY_MAX || act_open_day <= 0)
	{
		return;
	}
	
	m_param.today_fraction[act_open_day - 1] = m_param.pet_combat_num;
	if (act_open_day > 1)
	{
		for (int j = 0; j < act_open_day-1; j++)
		{
			m_param.today_fraction[act_open_day - 1] -= m_param.today_fraction[j];
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeCutePet::%s role[%d, %s] activity_type[%d] task_type[%d] cur_pet_combat[%d] ", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET, task_type,m_param.pet_combat_num);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeCutePet::EventCutePet()
{
	const RandActivityYunZeCutePetConfig * config = static_cast<const RandActivityYunZeCutePetConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	std::vector<RAYZCutePetTaskCfg> task_list;

	if (config->GetTaskCfg(ramgr, ARG_OUT task_list) < 0)
	{
		return;
	}

	for (int i = 0; i < (int)task_list.size(); ++i)
	{
		if (task_list[i].task_id == RAYZCutePet::TASK_TYPE_LOGIN)
		{
			this->AddTaskParam(RAYZCutePet::TASK_TYPE_LOGIN);
		}
	}
}
