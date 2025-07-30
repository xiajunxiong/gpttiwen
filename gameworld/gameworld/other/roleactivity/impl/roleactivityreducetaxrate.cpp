#include "roleactivityreducetaxrate.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityreducetaxrateconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgrareducetaxrate.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityReduceTaxRate::RoleActivityReduceTaxRate(int type)
	:RoleActivity(type)
{

}

RoleActivityReduceTaxRate::~RoleActivityReduceTaxRate()
{

}

void RoleActivityReduceTaxRate::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAReduceTaxRateParam));
}

void RoleActivityReduceTaxRate::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAReduceTaxRateParam));
}

void RoleActivityReduceTaxRate::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_reduce_tax_rate_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_reduce_tax_rate_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}
