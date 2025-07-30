#include "roleactivitysignincontinuity.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysignincontinuityconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgrasignincontinuity.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivitySignInContinuity::RoleActivitySignInContinuity(int type)
	:RoleActivity(type)
{

}

RoleActivitySignInContinuity::~RoleActivitySignInContinuity()
{

}

void RoleActivitySignInContinuity::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASignInContinuityParam));
}

void RoleActivitySignInContinuity::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RASignInContinuityParam));
}

void RoleActivitySignInContinuity::OnLogin()
{
	this->OnRASignInContinuityDayReset();
}

void RoleActivitySignInContinuity::OnLoginSendInfo()
{
	this->SendRASignInContinuityInfo();
}

void RoleActivitySignInContinuity::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());

	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY))
	{
		this->OnRASignInContinuityDayReset();
	}
}

void RoleActivitySignInContinuity::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_sign_in_continuity_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_sign_in_continuity_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivitySignInContinuity::OnRAOpenOnlineSend()
{
	this->OnRASignInContinuityDayReset();
}

void RoleActivitySignInContinuity::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY)) return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY);

	switch (raor->opera_type)
	{
	case Protocol::RA_SIGN_IN_CONTINUITY_OP_TYPE_INFO:
		{
			this->SendRASignInContinuityInfo();
		}
		break;
	case Protocol::RA_SIGN_IN_CONTINUITY_OP_TYPE_RECEIVE:
		{
			this->OnRASignInContinuityReceive(raor->param_1);
		}
		break;
	}
}

void RoleActivitySignInContinuity::OnRASignInContinuityDayReset()
{
	unsigned int day_id = EngineAdapter::Instance().DayID();
	if (day_id == m_param.ra_sign_in_continuity_day_id) return;

	m_param.ra_sign_in_continuity_login_day++;
	m_param.ra_sign_in_continuity_day_id = day_id;
	this->SendRASignInContinuityInfo();
	this->SetRoleDataChange();
}

void RoleActivitySignInContinuity::SendRASignInContinuityInfo()
{
	Protocol::SCRASignInContinuityInfo info;
	info.login_day = m_param.ra_sign_in_continuity_login_day;
	info.receive_flag = m_param.ra_sign_in_continuity_receive_flag;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySignInContinuity::OnRASignInContinuityReceive(int seq)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivitySignInContinuityConfig * config = static_cast<const RandActivitySignInContinuityConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY));
	if (NULL == config)
	{
		return;
	}

	const RASignInContinuityCfg * cfg = config->GetSignInContinuityCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 == cfg->item.size())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.ra_sign_in_continuity_login_day < cfg->day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LOGIN_COUNT_ERR);
		return;
	}

	if (m_param.ra_sign_in_continuity_receive_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->item.size(), &cfg->item[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)cfg->item.size(), &cfg->item[0], PUT_REASON_RA_SIGN_IN_CONTINUITY))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_sign_in_continuity_receive_flag.SetBit(seq);
	this->SendRASignInContinuityInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRASignInContinuityReceive user[%d, %s], level[%d], act_type[%d], seq[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY, seq);

	this->OnParticipateRA(__FUNCTION__);
}
