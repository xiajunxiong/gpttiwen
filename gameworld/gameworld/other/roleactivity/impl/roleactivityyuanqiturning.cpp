#include "roleactivityyuanqiturning.hpp"
#include "gamelog.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqiturningconfig.hpp"
#include "protocol/msgrandactivity.hpp"

RoleActivityYuanQiTurning::RoleActivityYuanQiTurning(int type)
	:RoleActivity(type)
{
}

RoleActivityYuanQiTurning::~RoleActivityYuanQiTurning()
{
}

void RoleActivityYuanQiTurning::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYuanQiTurningParam));
}

void RoleActivityYuanQiTurning::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYuanQiTurningParam));
}

void RoleActivityYuanQiTurning::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	this->SendInfo();
}

void RoleActivityYuanQiTurning::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUAN_QI_TURNING != rand_act_type)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityYuanQiTurning::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	m_param.OnDayChange();

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityYuanQiTurning::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	switch (raor->opera_type)
	{
		case 0://下发
		{
			this->SendInfo();
		}
		break;
		case 1://翻牌
		{
			this->Turning(raor->param_1);
		}
		break;
		default:
		break;
	}
}

void RoleActivityYuanQiTurning::Turning(int open_index)
{
	const RandActivityYuanQiTurningConfig * config = static_cast<const RandActivityYuanQiTurningConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	const RAYuanQiTurningInfoBank * cfg = config->RankInfoBank(open_day);
	if (cfg == NULL)return;
	
	const RAYuanQiTurningInfoDetail * detail_cfg = config->RankInfoDetail(cfg->sign_group);
	if (detail_cfg == NULL)return;
	const RAYuanQiTurningRewardGroup * reward_cfg = config->GetRewardCfg(cfg->reward_group);
	if (reward_cfg == NULL)return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(reward_cfg->reward_vec.size(), &reward_cfg->reward_vec[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_vec[0], reward_cfg->reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList(reward_cfg->reward_vec.size(), &reward_cfg->reward_vec[0], PUT_REASON_YUAN_QI_TURNING);
	}

	m_param.open_index = open_index;
	m_param.info_id = detail_cfg->sign_id;
	m_param.reward_id = reward_cfg->type;

	this->SendInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYuanQiTurning::SendInfo()
{
	Protocol::SCRAYuanQiTurningInfo info;
	
	info.info_id = m_param.info_id;
	info.open_index = m_param.open_index;
	info.reward_id = m_param.reward_id;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}