#include "roleactivitymijingqiwen2.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "scene/fbmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"

RoleActivityMiJingQiWen2::RoleActivityMiJingQiWen2(int type)
	:RoleActivity(type)
{

}

RoleActivityMiJingQiWen2::~RoleActivityMiJingQiWen2()
{
}

void RoleActivityMiJingQiWen2::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ANECDOTE_2))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ANECDOTE_2);

	switch (raor->opera_type)
	{
	case 0:	//进入特殊副本
		{
			this->OnRoleEnterSpecialFb();
		}
		break;
	default:
		break;
	}

}

void RoleActivityMiJingQiWen2::OnRoleEnterSpecialFb()
{
	if (FBManager::Instance().EnterFB(m_role_module_mgr->GetRole(), FBManager::FB_TYPE_MI_JING_QI_WEN_2, 0, 0, true))
	{

	}

	this->OnParticipateRA(__FUNCTION__);
}