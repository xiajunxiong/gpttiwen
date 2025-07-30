#include "roleactivitybaozilaile.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitybaozilaileconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"


RoleActivityBaoZiLaiLe::RoleActivityBaoZiLaiLe(int type)
	:RoleActivity(type)
{
}

RoleActivityBaoZiLaiLe::~RoleActivityBaoZiLaiLe()
{
}

void RoleActivityBaoZiLaiLe::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RABaoZiLaiLeParam));
}

void RoleActivityBaoZiLaiLe::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RABaoZiLaiLeParam));
}

void RoleActivityBaoZiLaiLe::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityBaoZiLaiLe::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();
	this->RASendInfo();
}

void RoleActivityBaoZiLaiLe::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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

void RoleActivityBaoZiLaiLe::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:			// 领取奖励,p1-难度,p2-是否完成挑战(1-胜利,0-失败)
		{
			this->ReceiveReward(raor->param_1, raor->param_2);
		}
		break;
	}
}


void RoleActivityBaoZiLaiLe::ReceiveReward(int model, int is_win)
{
	if (is_win != 0 && is_win != 1)return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityBaoZiLaiLeConfig * config = static_cast<const RandActivityBaoZiLaiLeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	if (m_param.join_count >= config->GetOtherCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	const std::vector<ItemConfigData>* reward_list = config->GetRewardCfg(ramgr, model, (is_win == 1 ? true : false));
	if (reward_list == NULL) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(*reward_list))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward_list, PUT_REASON_RA_BAO_ZI_LAI_LE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward_list, SEND_MAIL_TYPE_BAO_ZI_LAI_LE);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(*reward_list, GET_REASON_BAO_ZI_LAI_LE, is_win);

	std::string log_str;
	for (size_t i = 0; i < reward_list->size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d] ", (*reward_list)[i].item_id, (*reward_list)[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] reward_list[%s] cur_times[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str(), m_param.join_count);

	m_param.join_count += 1;
	this->RASendInfo();

	this->OnJoinActivity();
}

void RoleActivityBaoZiLaiLe::RASendInfo()
{
	Protocol::SCRABaoZiLaiLeInfo msg;
	msg.join_count = m_param.join_count;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityBaoZiLaiLe::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_BAOZI_LAILE);
		}
	}
}
