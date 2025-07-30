#include "roleactivitygivepetdraw.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitygivepetdrawconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgragivepetdraw.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityGivePetDraw::RoleActivityGivePetDraw(int type)
	:RoleActivity(type)
{

}

RoleActivityGivePetDraw::~RoleActivityGivePetDraw()
{

}

void RoleActivityGivePetDraw::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAGivePetDrawParam));
}

void RoleActivityGivePetDraw::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAGivePetDrawParam));
}

void RoleActivityGivePetDraw::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityGivePetDraw::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_GIVE_PET_DRAW != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.give_mark = 0;

		this->SetRoleDataChange();
	}
}

void RoleActivityGivePetDraw::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GIVE_PET_DRAW))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_GIVE_PET_DRAW);

	switch (raor->opera_type)
	{
	case Protocol::RA_GIVE_PET_DRAW_OPERA_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
	case Protocol::RA_GIVE_PET_DRAW_OPERA_TYPE_RECEIVE:
		{
			this->OnReveive();
		}
		break;
	}
}

void RoleActivityGivePetDraw::SendInfo()
{
	Protocol::SCRAGivePetDrawInfo info;
	info.give_mark = m_param.give_mark;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityGivePetDraw::OnReveive()
{
	const RandActivityGivePetDrawConfig * config = static_cast<const RandActivityGivePetDrawConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GIVE_PET_DRAW));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGivePetDrawOtherCfg * cfg = config->GetGivePetDrawOtherCfg();
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 < m_param.give_mark)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_GIVE_PET_DRAW))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.give_mark = 1;
	this->SendInfo();

	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnReveive user[%d, %s], level[%d], type[%d], receive_item[id:%d num:%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_GIVE_PET_DRAW, cfg->item.item_id, cfg->item.num);

	this->OnParticipateRA(__FUNCTION__);
}
