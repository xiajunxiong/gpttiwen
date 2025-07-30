#include "roleactivityzongxiangmancheng.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityzongxiangmanchengconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/activesystem/activesystem.hpp"

RoleActivityZongXiangManCheng::RoleActivityZongXiangManCheng(int type) :RoleActivity(type)
{
}

RoleActivityZongXiangManCheng::~RoleActivityZongXiangManCheng()
{
}

void RoleActivityZongXiangManCheng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAZongXiangManChengParam));
}

void RoleActivityZongXiangManCheng::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAZongXiangManChengParam));
}


void RoleActivityZongXiangManCheng::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityZongXiangManChengConfig * activity = (RandActivityZongXiangManChengConfig *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	this->RASendInfo();
}

void RoleActivityZongXiangManCheng::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();
	this->RASendInfo();
}


void RoleActivityZongXiangManCheng::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityZongXiangManCheng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 1:	// 激活
		{
			this->ActiveSeq(raor->param_1);
		}
		break;
	}
}


void RoleActivityZongXiangManCheng::ActiveSeq(int seq)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityZongXiangManCheng * activity = (RandActivityZongXiangManCheng *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	const RandActivityZongXiangManChengConfig * config = static_cast<const RandActivityZongXiangManChengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG));
	if (NULL == config) return;

	const ZongXiangManChengBaseConfig * base_cfg = config->GetBaseCfg(config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG), seq);
	if (NULL == base_cfg)return;


	//激活
	if (m_param.light_type <= 0)m_param.light_type = base_cfg->light_type;
	else
	{
		if (m_param.light_type != base_cfg->light_type)
		{
			return;
		}
	}

	switch (base_cfg->light_condition)
	{
	case ZongXiangManChengBaseConfig::ZONG_XIANG_CONDITION_0:
		{
			int active_num = m_role_module_mgr->GetActiveSystem()->GetActiveNum();
			if (active_num < base_cfg->parameter)
			{
				return;
			}
		}
		break;
	case ZongXiangManChengBaseConfig::ZONG_XIANG_CONDITION_1:
		{
			int bit_count = 0;
			for (int i = 0; i < MAX_ZONG_XIANG_MAN_CHENG_FLAG;i++)
			{
				if (m_param.m_flag_map.IsBitSet(i))
				{
					bit_count += 1;
				}
			}
			if (bit_count < base_cfg->parameter)
			{
				return;
			}
		}
		break;
	default:
		break;
	}

	if (m_param.m_flag_map.IsBitSet(seq))return;
	m_param.m_flag_map.SetBit(seq);
	//是否有奖励

	if (base_cfg->reward_group > 0)
	{
		const ZongXiangRewardConfig * reward_cfg = config->GetRewardCfg(base_cfg->reward_group);
		if (NULL != reward_cfg)
		{
			m_role_module_mgr->GetKnapsack()->PutList((int)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_RA_ZONG_XIANG_REWARD);
		}
	}

	/*if (base_cfg->reward.item_id > 0)
	{
		m_role_module_mgr->GetKnapsack()->Put(base_cfg->reward, PUT_REASON_ACHIEVEMENT_PROG_REWARD);
	}*/

	this->RASendInfo();
}

void RoleActivityZongXiangManCheng::RASendInfo()
{
	Protocol::SCRAZongXiangManChengInfo msg;
	msg.light_type = m_param.light_type;
	msg.m_flag_map = m_param.m_flag_map;
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}