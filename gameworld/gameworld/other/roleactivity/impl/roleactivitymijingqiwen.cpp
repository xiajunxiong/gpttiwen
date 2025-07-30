#include "roleactivitymijingqiwen.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "scene/fbmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"

RoleActivityMiJingQiWen::RoleActivityMiJingQiWen(int type)
	:RoleActivity(type)
{

}

RoleActivityMiJingQiWen::~RoleActivityMiJingQiWen()
{
}

void RoleActivityMiJingQiWen::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ANECDOTE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ANECDOTE);

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

void RoleActivityMiJingQiWen::OnRoleEnterSpecialFb()
{
	if (FBManager::Instance().EnterFB(m_role_module_mgr->GetRole(), FBManager::FB_TYPE_MI_JING_QI_WEN, 0, 0, true))
	{

	}

	this->OnParticipateRA(__FUNCTION__);
}