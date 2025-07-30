#include "roleactivityxunmenghudie.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityxunmenghudieconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityxunmenghudie.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"

RoleActivityXunMengHuDie::RoleActivityXunMengHuDie(int type)
	:RoleActivity(type)
{
}

RoleActivityXunMengHuDie::~RoleActivityXunMengHuDie()
{
}

void RoleActivityXunMengHuDie::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAXunMengHuDieParam));
}

void RoleActivityXunMengHuDie::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAXunMengHuDieParam));
}

void RoleActivityXunMengHuDie::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityXunMengHuDie::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	m_param.ra_frequency_num = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityXunMengHuDie::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityXunMengHuDie::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:
		{
			this->RAGrab(raor->param_1, raor->param_2);
		}
		break;
	}
}

void RoleActivityXunMengHuDie::OnComplete(bool is_win)
{
	if (is_win)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL == ramgr)
		{
			return;
		}

		const RandActivityXunMengHuDieConfig * config = static_cast<const RandActivityXunMengHuDieConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
			return;

		const RAXunMengHuDieRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
		if (NULL == reward_cfg)
			return;

		if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg->reward, false))
		{
			m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward, PUT_REASON_RA_XUNMENG_HUDIE);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward[0], (int)reward_cfg->reward.size(), SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
		}

		m_param.ra_frequency_num += 1;

		this->RASendInfo();
		this->SetRoleDataChange();

		this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_WIN, &reward_cfg->reward);

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] frequency_num[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			m_param.ra_frequency_num);

		this->OnJoinActivity();
	}
	else
	{
		this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_FAIL);
	}
}

void RoleActivityXunMengHuDie::RASendInfo()
{
	Protocol::SCRAXunMengHuDieInfo info;
	info.frequency_num = m_param.ra_frequency_num;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityXunMengHuDie::RAGrab(ObjID obj_id, int is_win)
{
	const RandActivityXunMengHuDieConfig * config = static_cast<const RandActivityXunMengHuDieConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	if (m_param.ra_frequency_num >= config->GetOtherCfg()->frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_ACTIVITY_NOT_TIMES);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(m_role_activity_type))
		{
			RandAcitivityXunMengHuDie * randactivity_info = (RandAcitivityXunMengHuDie *)ramgr->ForceGetRandActivity(m_role_activity_type);
			if (randactivity_info != NULL)
			{
				randactivity_info->OnComplete(m_role_module_mgr->GetRole(), is_win, obj_id);
			}
		}
	}
}

void RoleActivityXunMengHuDie::OnJoinActivity()
{
	{
		RoleActivityYuanQiJiShiBu * role_activity = static_cast<RoleActivityYuanQiJiShiBu *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_XUNMENG_HUDIE);
		}
	}
}
