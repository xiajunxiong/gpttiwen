#include "roleactivitysignhappy.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysignhappyconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"


RoleActivitySignHappy::RoleActivitySignHappy(int type) :RoleActivity(type)
{
}

RoleActivitySignHappy::~RoleActivitySignHappy()
{
}

void RoleActivitySignHappy::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASignHappyParam));
}

void RoleActivitySignHappy::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASignHappyParam));
}

void RoleActivitySignHappy::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SIGN_HAPPY))
		return;

	m_param.is_add_day = 0;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SIGN_HAPPY);

	if (0 == m_param.is_add_day)
	{
		m_param.login_day += 1;
		m_param.is_add_day = 1;
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivitySignHappy::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SIGN_HAPPY))
		return;

	this->RASendInfo();
}

void RoleActivitySignHappy::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SIGN_HAPPY != rand_act_type)
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
		m_param.login_day = 1;
		m_param.is_add_day = 1;

		this->SetRoleDataChange();
	}
}

void RoleActivitySignHappy::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SIGN_HAPPY))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SIGN_HAPPY);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->OnFetchReward(raor->param_1);
		}
		break;
	case 2:
		{
			this->OnFetchAllReward();
		}
		break;
	}
}

void RoleActivitySignHappy::RASendInfo()
{
	Protocol::SCRASignHappyInfo info;
	info.login_day = m_param.login_day;
	info.fetch_mark = m_param.fetch_mark;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySignHappy::OnFetchReward(int index)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivitySignHappyConfig * config = static_cast<const RandActivitySignHappyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SIGN_HAPPY));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RASignHappyRewardCfg * cfg = config->GetRewardCfg(ramgr, index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != (m_param.fetch_mark & (1 << index)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (index >= m_param.login_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	// 基础奖励
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, cfg->reward_count, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SIGN_HAPPY);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward_item, PUT_REASON_RA_SIGN_HAPPY);
	}

	// 额外奖励
	if (cfg->reward_count_1 > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count_1, cfg->reward_item_1, false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item_1, cfg->reward_count_1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SIGN_HAPPY);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count_1, cfg->reward_item_1, PUT_REASON_RA_SIGN_HAPPY);
		}
	}

	m_param.fetch_mark |= (1 << index);

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_day[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
		index + 1);
}

void RoleActivitySignHappy::OnFetchAllReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivitySignHappyConfig * config = static_cast<const RandActivitySignHappyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SIGN_HAPPY));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	for (int i = 0; i < m_param.login_day; i++)
	{
		if (0 != (m_param.fetch_mark & (1 << i)))
			continue;

		const RASignHappyRewardCfg * cfg = config->GetRewardCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		// 基础奖励
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, cfg->reward_item, false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, cfg->reward_count, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SIGN_HAPPY);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward_item, PUT_REASON_RA_SIGN_HAPPY);
		}

		// 额外奖励
		if (cfg->reward_count_1 > 0)
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count_1, cfg->reward_item_1, false))
			{
				m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item_1, cfg->reward_count_1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SIGN_HAPPY);
			}
			else
			{
				m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count_1, cfg->reward_item_1, PUT_REASON_RA_SIGN_HAPPY);
			}
		}

		m_param.fetch_mark |= (1 << i);

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_day[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
			i + 1);
	}

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}
