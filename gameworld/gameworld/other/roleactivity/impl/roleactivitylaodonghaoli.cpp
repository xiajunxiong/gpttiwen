#include "roleactivitylaodonghaoli.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitylaodonghaoliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgratianjianghaoli.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/randactivity/randactivityimpl/randactivitylaodonghaoli.hpp"

RoleActivityLaoDongHaoLi::RoleActivityLaoDongHaoLi(int type)
	:RoleActivity(type)
{

}

RoleActivityLaoDongHaoLi::~RoleActivityLaoDongHaoLi()
{

}

void RoleActivityLaoDongHaoLi::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATianJiangHaoLiParam));
}

void RoleActivityLaoDongHaoLi::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RATianJiangHaoLiParam));
}

void RoleActivityLaoDongHaoLi::OnLoginSendInfo()
{
	this->SendRATianJiangHaoLiInfo();
}

void RoleActivityLaoDongHaoLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.ra_tian_jiang_hao_li_gather_count = 0;
	this->SendRATianJiangHaoLiInfo();

	this->SetRoleDataChange();
}

void RoleActivityLaoDongHaoLi::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_tian_jiang_hao_li_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_tian_jiang_hao_li_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityLaoDongHaoLi::RATianJiangHaoLiAddReceiveCount()
{
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);

	m_param.ra_tian_jiang_hao_li_gather_count++;
	this->SendRATianJiangHaoLiInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityLaoDongHaoLi::SendRATianJiangHaoLiInfo()
{
	bool is_refresh_box = false;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI))
	{
		RandActivityLaoDongHaoLi * tjhl = (RandActivityLaoDongHaoLi*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);
		if (NULL != tjhl)
		{
			if (tjhl->IsRefreshBoxTime()) is_refresh_box = true;
		}
	}
	Protocol::SCRALaoDongHaoLiInfo info;
	info.is_refresh_box = is_refresh_box ? 1 : 0;
	info.receive_count = (short)m_param.ra_tian_jiang_hao_li_gather_count;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

