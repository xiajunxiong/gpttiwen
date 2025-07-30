#include "roleactivitybackreward.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitybackrewardconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "other/partner/partner.hpp"
#include "other/pet/pet.hpp"
#include "other/medal/medal.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"


RoleActivityBackReward::RoleActivityBackReward(int type)
	:RoleActivity(type)
{
}

RoleActivityBackReward::~RoleActivityBackReward()
{
}

void RoleActivityBackReward::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RABackRewardParam));
}

void RoleActivityBackReward::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RABackRewardParam));
}

void RoleActivityBackReward::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_BACK_REWARD))
	{
		this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_BACK_REWARD);

		unsigned int logout_timestamp = m_role_module_mgr->GetCommonData().last_logout_timestamp;
		if (0 == m_param.huigui_timestamp && logout_timestamp > 0)
		{
			const RandActivityBackRewardConfig * config = static_cast<const RandActivityBackRewardConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_BACK_REWARD));
			if (NULL == config)
				return;

			int logout_day = GetDayIndex(logout_timestamp, EngineAdapter::Instance().Time());
			if (logout_day < 0) logout_day = 0;
			if (m_role_module_mgr->GetRole()->GetLevel() >= config->GetOtherCfg()->open_level &&
				logout_day >= config->GetOtherCfg()->back_days)
			{
				m_param.huigui_timestamp = (unsigned int)EngineAdapter::Instance().Time();
				//检查一些成就类任务
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PASS_LHWC, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PURPLE_TOKEN, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL, 0, false);

				this->RASendInfo();
			}
		}
	}

	if (m_param.huigui_timestamp > 0)
	{
		int day_index = GetDayIndex(m_param.huigui_timestamp, EngineAdapter::Instance().Time()) + 1;
		if (day_index <= 0) day_index = 1;

		if (day_index > 0 && day_index <= MAX_RA_BACK_REWARD_DAY)
		{
			m_param.day_flag[day_index - 1] = RA_BACK_REWARD_FLAG_TYPE_FINISH;

			this->RASendInfo();
		}
	}
}

void RoleActivityBackReward::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_BACK_REWARD))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityBackReward::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_BACK_REWARD != rand_act_type)
		return;

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
			return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityBackReward::OnReq(int type, int param)
{
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_BACK_REWARD);

	switch (type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->RAFetchDayReward(param);
		}
		break;
	case 2:
		{
			this->RAFetchTaskReward(param);
		}
		break;
	}
}

void RoleActivityBackReward::CheckTaskRate(int task_type, int add_param, bool is_send)
{
	if (!this->IsHaveActivity())
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityBackRewardConfig * config = static_cast<const RandActivityBackRewardConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL == config)
	{
		return;
	}

	const std::vector<RABackRewardTaskCfg> * list_cfg = config->GetTaskTypeListCfg(ramgr, task_type);
	if (NULL == list_cfg)
		return;

	bool is_new = false;
	for (int i = 0; i < (int)list_cfg->size(); i++)
	{
		const  RABackRewardTaskCfg * cfg = &(*list_cfg)[i];
		if (NULL == cfg)
			continue;

		int task_id = cfg->task_id - 1;
		if(m_param.task_list[task_id].flag != RA_BACK_REWARD_FLAG_TYPE_NOT_FINISH)
			continue;

		switch (cfg->task_type)
		{
		case RA_BACK_REWARD_TYPE_PET_INTENSI:
		case RA_BACK_REWARD_TYPE_PASS_FB:
		case RA_BACK_REWARD_TYPE_BIG_DIPPER:
		case RA_BACK_REWARD_TYPE_PASS_ELITEFB:
			{
				m_param.task_list[task_id].param += add_param;
			}
			break;
		case RA_BACK_REWARD_TYPE_PARTNER_UPGRADE:	
			{
				if(add_param == m_param.task_list[task_id].param)
					continue;

				m_param.task_list[task_id].param = add_param;
			}
			break;
		case RA_BACK_REWARD_TYPE_PASS_LHWC:
			{
				int num = (m_role_module_mgr->GetLunHuiWangChuan()->GetPassIDX() <= 0) ? 0 : m_role_module_mgr->GetLunHuiWangChuan()->GetPassIDX() - 1;
				if (num == m_param.task_list[task_id].param)
					continue;

				m_param.task_list[task_id].param = num;
			}
			break;
		case RA_BACK_REWARD_TYPE_PURPLE_TOKEN:
			{
				int num = m_role_module_mgr->GetMedal()->GetCurWearNumByColor(cfg->param2);
				if (num == m_param.task_list[task_id].param)
					continue;

				m_param.task_list[task_id].param = num;
			}
			break;
		case RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM:
			{
				int num = m_role_module_mgr->GetPartner()->GetWearSetEquipNum();
				if (num == m_param.task_list[task_id].param)
					continue;

				m_param.task_list[task_id].param = num;
			}
			break;
		case RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL:
			{
				int num = m_role_module_mgr->GetPet()->GetPetIntensifyMax();
				if (num == m_param.task_list[task_id].param)
					continue;

				m_param.task_list[task_id].param = num;
			}
			break;
		default:
			continue;
		}

		is_new = true;

		if (m_param.task_list[task_id].param >= cfg->param1)
		{
			m_param.task_list[task_id].flag = RA_BACK_REWARD_FLAG_TYPE_FINISH;
		}
	}

	if (is_send && is_new) this->RASendInfo();
}

void RoleActivityBackReward::GmSetActivity(int type, int param)
{
	switch (type)
	{
	case 0:
		{
			m_param.huigui_timestamp = (unsigned int)EngineAdapter::Instance().Time();
			m_param.day_flag[0] = RA_BACK_REWARD_FLAG_TYPE_FINISH;
			//检查一些成就类任务
			if (this->IsHaveActivity())
			{
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PASS_LHWC, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PURPLE_TOKEN, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM, 0, false);
				this->CheckTaskRate(RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL, 0, false);
			}
		}
		break;
	case 1:
		{
			if (!this->IsHaveActivity())
				return;

			m_param.day_flag[param - 1] = RA_BACK_REWARD_FLAG_TYPE_FINISH;
		}
		break;
	default:
		return;
	}

	this->RASendInfo();
}

void RoleActivityBackReward::RASendInfo()
{
	Protocol::SCBackRewardInfo info;
	info.huigui_timestamp = m_param.huigui_timestamp;
	memcpy(info.day_flag, m_param.day_flag, sizeof(info.day_flag));
	memcpy(info.task_list, m_param.task_list, sizeof(info.task_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityBackReward::RAFetchDayReward(int day_index)
{
	if (!this->IsHaveActivity())
		return;

	if (day_index <= 0 || day_index > MAX_RA_BACK_REWARD_DAY)
		return;

	if (m_param.day_flag[day_index - 1] != RA_BACK_REWARD_FLAG_TYPE_FINISH)
	{
		if (m_param.day_flag[day_index - 1] == RA_BACK_REWARD_FLAG_TYPE_NOT_FINISH)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		}
		return;
	}	

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityBackRewardConfig * config = static_cast<const RandActivityBackRewardConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RABackRewardDayCfg * cfg = config->GetDayCfg(ramgr, day_index);
	if (NULL == cfg)
		return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->count, cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, cfg->count, SEND_MAIL_TYPE_BACK_REWARD, true);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList(cfg->count, cfg->reward_item, PUT_REASON_RA_BACK_REWARD);
	}

	m_param.day_flag[day_index - 1] = RA_BACK_REWARD_FLAG_TYPE_FETCH;

	this->RASendInfo();
	this->OnParticipateRA(__FUNCTION__);

	//log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | user[%d, %s], ra_begin_timestamp[%ud], day_index[%d] ", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.ra_begin_timestamp, day_index);
}

void RoleActivityBackReward::RAFetchTaskReward(int task_index)
{
	if (!this->IsHaveActivity())
		return;

	if (task_index <= 0 || task_index > MAX_RA_BACK_REWARD_TASK)
		return;

	if (m_param.task_list[task_index - 1].flag != RA_BACK_REWARD_FLAG_TYPE_FINISH)
	{
		if (m_param.task_list[task_index - 1].flag == RA_BACK_REWARD_FLAG_TYPE_NOT_FINISH)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		}
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityBackRewardConfig * config = static_cast<const RandActivityBackRewardConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_BACK_REWARD));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RABackRewardTaskCfg * cfg = config->GetTaskCfg(ramgr, task_index);
	if (NULL == cfg)
		return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item, 1, SEND_MAIL_TYPE_BACK_REWARD, true);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_BACK_REWARD);
	}

	m_param.task_list[task_index - 1].flag = RA_BACK_REWARD_FLAG_TYPE_FETCH;

	this->RASendInfo();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | user[%d, %s], ra_begin_timestamp[%ud], task_index[%d] ", __FUNCTION__, __LINE__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.ra_begin_timestamp, task_index);
}

bool RoleActivityBackReward::IsHaveActivity()
{
	if (m_param.huigui_timestamp <= 0)
		return false;

	int day_index = GetDayIndex(m_param.huigui_timestamp, EngineAdapter::Instance().Time()) + 1;
	if (day_index <= 0) day_index = 1;

	return day_index > 0 && day_index <= MAX_RA_BACK_REWARD_DAY;
}
