#include "roleactivityyunzeterritory.hpp"
#include "protocol/randactivity/msgrayunzeterritory.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "gamelog.h"

#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeterritoryconfig.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"

RoleActivityYunZeTerritory::RoleActivityYunZeTerritory(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeTerritory::~RoleActivityYunZeTerritory()
{

}

void RoleActivityYunZeTerritory::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeTerritory::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}


void RoleActivityYunZeTerritory::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1:
		{
			this->Explore(raor->param_1);
		}
		break;
	default:
		break;
	}

	return;
}

void RoleActivityYunZeTerritory::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY))
	{
		return;
	}
	this->SendInfo();
}


void RoleActivityYunZeTerritory::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY != rand_act_type)
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

void RoleActivityYunZeTerritory::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY))
	{
		return;
	}

	m_param.OnDayReset();
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityYunZeTerritory::SendInfo()
{
	const RandActivityYunZeTerritoryConfig * config = static_cast<const RandActivityYunZeTerritoryConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int frequency = config->GetBasicsCfg().frequency;
	if (frequency<0 || frequency>PICTURE_FRAGMENTS_NUM)
	{
		return;
	}

	Protocol::SCRAYunZeTerritoryInfo msg;
	msg.linght_up_length = frequency;
	msg.times = m_param.times;
	for (int i = 0; i < PICTURE_FRAGMENTS_NUM; ++i)
	{
		msg.light_up[i] = m_param.light_up[i];
	}

	m_role_module_mgr->NetSend(&msg,sizeof(msg));
}

void RoleActivityYunZeTerritory::Explore(int picture_piece)
{
	if (picture_piece < 0 || picture_piece > PICTURE_FRAGMENTS_NUM-1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RandActivityYunZeTerritoryConfig * config = static_cast<const RandActivityYunZeTerritoryConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.times >= config->GetBasicsCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	if (m_param.light_up[picture_piece] != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	m_param.light_up[picture_piece] = 1;
	m_param.times++;

	this->SendInfo();

	int frequency = config->GetBasicsCfg().frequency;
	if (frequency<0 || frequency>PICTURE_FRAGMENTS_NUM)
	{
		return;
	}

	bool is_all_linght_up = true;
	for (int i = 0; i < frequency; ++i)
	{
		is_all_linght_up = is_all_linght_up && m_param.light_up[i];	
	}
	 
	if (is_all_linght_up)
	{
		this->SendReward();
	}

	this->SetRoleDataChange();

	{
		RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEHUAJING);
		}
	}
	{
		RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAColorfulDouble::TSAK_TYPE_YUN_ZE_TERRITORY, 1);
		}
	}
	{
		RoleActivityYunZeCutePet * role_activity = static_cast<RoleActivityYunZeCutePet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAYZCutePet::TASK_TYPE_YZ_TERRITORY, 1);
		}
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeTerritory::SendReward()
{
	if (REWARD_STATUS_LACK == m_param.is_get_reward)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityYunZeTerritoryConfig * config = static_cast<const RandActivityYunZeTerritoryConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZTerritoryTaskRewardCfg * reward_cfg = config->GetTaskRewardCfg(ramgr);
	if (NULL == reward_cfg)
	{
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->item_list.size(), &reward_cfg->item_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->item_list.size(), &reward_cfg->item_list[0], PUT_REASON_YUN_ZE_TERRITORY);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->item_list[0], (int)reward_cfg->item_list.size(), SEND_MAIL_TYPE_YUN_ZE_TERRITORY, true);
	}

	std::string gamelog_str;
	for (int i = 0; i < reward_cfg->item_list.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d, %d]} ", reward_cfg->item_list[i].item_id, reward_cfg->item_list[i].is_bind, reward_cfg->item_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeTerritory::%s role[%d, %s] reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	for (int i = 0; i < PICTURE_FRAGMENTS_NUM; ++i)
	{
		m_param.light_up[i] = 0;
	}
	m_param.is_get_reward = 1;
	
	this->SetRoleDataChange();
	this->SendInfo();
}