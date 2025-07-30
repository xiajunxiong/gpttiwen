#include "roleactivitymozuzainali.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitymozuzainaliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityMoZuZaiNaLi::RoleActivityMoZuZaiNaLi(int type)
	:RoleActivity(type)
{
}

RoleActivityMoZuZaiNaLi::~RoleActivityMoZuZaiNaLi()
{
}

void RoleActivityMoZuZaiNaLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAMoZuZaiNaLiParam));
}

void RoleActivityMoZuZaiNaLi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAMoZuZaiNaLiParam));
}

void RoleActivityMoZuZaiNaLi::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityMoZuZaiNaLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	int temp_frequency_num = m_param.ra_frequency_num;

	m_param.ra_frequency_num = 0;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] temp_frequency_num[%d] m_param.ra_frequency_num[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), temp_frequency_num, m_param.ra_frequency_num);
}

void RoleActivityMoZuZaiNaLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI != rand_act_type)
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

void RoleActivityMoZuZaiNaLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	}
}

void RoleActivityMoZuZaiNaLi::OnExpelMonster()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI))
		return;

	m_param.ra_frequency_num += 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityMoZuZaiNaLi::RASendInfo()
{
	Protocol::SCRAMoZuZaiNaLiInfo info;
	info.day_times = m_param.ra_frequency_num;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}