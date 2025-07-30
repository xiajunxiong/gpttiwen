#include "roleactivityjoyseeksfavors.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjoyseeksfavorsconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityJoySeeksFavors::RoleActivityJoySeeksFavors(int type)
	:RoleActivity(type)
{

}

RoleActivityJoySeeksFavors::~RoleActivityJoySeeksFavors()
{

}

void RoleActivityJoySeeksFavors::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAJoySeeksFavorsParam));
}

void RoleActivityJoySeeksFavors::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAJoySeeksFavorsParam));
}

void RoleActivityJoySeeksFavors::OnLoginSendInfo()
{
	this->SendRAJoySeeksFavorsInfo();
}

void RoleActivityJoySeeksFavors::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS != rand_act_type)
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

	const RandActivityJoySeeksFavorsConfig * config = static_cast<const RandActivityJoySeeksFavorsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS));
	if (NULL == config)
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_joy_seeks_favors_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_joy_seeks_favors_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_joy_seeks_favors_role_level = m_role_module_mgr->GetRole()->GetLevel();

		this->SetRoleDataChange();
	}
}

int RoleActivityJoySeeksFavors::GetJoySeeksFavorsRoleLevel()
{
	return m_param.ra_joy_seeks_favors_role_level;
}

void RoleActivityJoySeeksFavors::SendRAJoySeeksFavorsInfo()
{

}
