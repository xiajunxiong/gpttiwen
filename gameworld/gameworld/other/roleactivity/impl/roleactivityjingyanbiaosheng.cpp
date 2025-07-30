#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "protocol/randactivity/msgrajingyanbiaosheng.hpp"
#include "config/randactivityconfig/impl/randactivityjingyanbiaoshengconfig.hpp"
#include "roleactivityjingyanbiaosheng.hpp"


RoleActivityJingYanBiaoSheng::RoleActivityJingYanBiaoSheng(int type) :RoleActivity(type)
{
}

RoleActivityJingYanBiaoSheng::~RoleActivityJingYanBiaoSheng()
{
}

void RoleActivityJingYanBiaoSheng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityJingYanBiaoSheng::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityJingYanBiaoSheng::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityJingYanBiaoSheng::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.day_buy_times = 0;
	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityJingYanBiaoSheng::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
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

void RoleActivityJingYanBiaoSheng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * req)
{
	const RandActivityJingYanBiaoShengConfig * config = static_cast<const RandActivityJingYanBiaoShengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (role_level >= MAX_ROLE_LEVEL)
	{
		return;
	}

	const RAJingYanBiaoShengItemCfg * buy_cfg = config->GetRAJingYanBiaoShengCfg(role_level);
	if (buy_cfg == NULL)
	{
		return;
	}


	const RAJingYanBiaoShengOtherCfg & other_cfg = config->GetOtherCg();
	if (other_cfg.limit_type == 1)
	{
		if (m_param.buy_times >= other_cfg.limit_buy_times)
		{
			return;
		}
	}

	if (other_cfg.limit_type == 2)
	{
		if (m_param.day_buy_times >= other_cfg.limit_day_buy_times)
		{
			return;
		}
	}

	switch (req->param_1)
	{
		case 0:
			{
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(buy_cfg->comsume_item_id, buy_cfg->comsume_num, __FUNCTION__, 0, true))
				{
					return;
				}
			}
			break;
		case 1:
			{
				if (!m_role_module_mgr->GetMoney()->UseGold(buy_cfg->need_gold, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
			}
			break;
		default:
			return;
			break;
	}

	m_role_module_mgr->GetRole()->AddExp(buy_cfg->get_exp, ADD_EXP_TYPE_JING_YAN_BIAO_SHENG);

	if (other_cfg.limit_type == 1)
	{
		m_param.buy_times += 1;
	}

	if (other_cfg.limit_type == 2)
	{
		m_param.day_buy_times += 1;
	}

	this->SetRoleDataChange();
	this->SendInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityJingYanBiaoSheng::SendInfo()
{
	Protocol::SCRAJingYanBiaoShengInfo protocol;
	protocol.buy_times = m_param.buy_times;
	protocol.day_buy_times = m_param.day_buy_times;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

}
