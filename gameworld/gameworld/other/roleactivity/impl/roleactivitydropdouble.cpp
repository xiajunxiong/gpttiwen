#include "roleactivitydropdouble.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitydropdoubleconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"


#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityDropDouble::RoleActivityDropDouble(int type)
	:RoleActivity(type)
{

}

RoleActivityDropDouble::~RoleActivityDropDouble()
{

}

void RoleActivityDropDouble::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RADropDoubleParam));
}

void RoleActivityDropDouble::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RADropDoubleParam));
}

void RoleActivityDropDouble::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_DROP_DOUBLE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_drop_double_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_drop_double_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

