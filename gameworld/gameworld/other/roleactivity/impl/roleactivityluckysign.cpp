#include "roleactivityluckysign.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgraburningsummer.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityluckysignconfig.hpp"

RoleActivityLuckySign::RoleActivityLuckySign(int type)
	:RoleActivity(type)
{
}

RoleActivityLuckySign::~RoleActivityLuckySign()
{
}

void RoleActivityLuckySign::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RALuckySignParam));
}

void RoleActivityLuckySign::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RALuckySignParam));
}

void RoleActivityLuckySign::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_SIGN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_SIGN);
	this->SendInfo();
}

void RoleActivityLuckySign::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_LUCKY_SIGN != rand_act_type)
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
		const RandActivityLuckySignConfig * config = static_cast<const RandActivityLuckySignConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_SIGN));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityLuckySign::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_SIGN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_SIGN);

	m_param.DayReset();
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityLuckySign::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_SIGN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LUCKY_SIGN);
	switch (raor->opera_type)
	{
		case Protocol::RA_LUCKY_SIGN_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_LUCKY_SIGN_REQ_TYPE_SET_SHOW_FLAG:
		{
			this->OnSetShowFlag(raor->param_1);
		}
		break;
		case Protocol::RA_LUCKY_SIGN_REQ_TYPE_DAY_SIGN:
		{
			this->OnDaySign();
		}
		break;
	}	
}

void RoleActivityLuckySign::SendInfo()
{
	Protocol::SCRALuckySignInfo info;
	info.is_base_show_flag = m_param.is_base_show_flag;
	info.is_fetch_reward_flag = m_param.is_fetch_reward_flag;
	info.sign_id = m_param.sign_id;
	info.reward_type_seq = m_param.reward_type_seq;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityLuckySign::OnSetShowFlag(int show_flag)
{
	//还未签到
	if (1 != m_param.is_fetch_reward_flag)
	{
		return;
	}
	m_param.is_base_show_flag = (0 != show_flag) ? 1 : 0;
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityLuckySign::OnDaySign()
{
	if (0 != m_param.is_fetch_reward_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LUCKY_SIGN_REWARD_FETCHED);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityLuckySignConfig * config = static_cast<const RandActivityLuckySignConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_SIGN));
	if (NULL == config)
	{
		return;
	}
	const RALuckySignCfg * cfg = config->GetRandCfg(ramgr);
	if (NULL == cfg)
	{
		return;
	}
	
	const RALuckySignSignCfg * sign_cfg = config->GetSignCfg(cfg->sign_group_id);
	const RALuckySignRewardCfg * reward_cfg = config->GetRewardCfg(cfg->reward_group_id);
	if (NULL == sign_cfg || NULL == reward_cfg)
	{
		return;
	}

	if (!reward_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_LUCKY_SIGN))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_LUCKY_SIGN);
		}
	}
	m_param.sign_id = sign_cfg->sign_id;
	m_param.reward_type_seq = reward_cfg->type_seq;
	m_param.is_fetch_reward_flag = 1;
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], sign[%d, %d], reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_LUCKY_SIGN, cfg->sign_group_id, sign_cfg->sign_id, cfg->reward_group_id, reward_cfg->type_seq);
}
