#include "roleactivityelementcards.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityelementcardsconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/randactivity/msgraelementcards.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityElementCards::RoleActivityElementCards(int type)
	:RoleActivity(type)
{

}

RoleActivityElementCards::~RoleActivityElementCards()
{

}

void RoleActivityElementCards::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAElementCardsParam));
}

void RoleActivityElementCards::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAElementCardsParam));
}

void RoleActivityElementCards::OnLogin()
{
	this->OnRAElementCardsDayReset();
}

void RoleActivityElementCards::OnLoginSendInfo()
{
	this->SendRAElementCardsInfo();
}

void RoleActivityElementCards::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());

	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ELEMENT_CARDS))
	{
		this->OnRAElementCardsDayReset();
	}
}

void RoleActivityElementCards::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_ELEMENT_CARDS != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_element_cards_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_element_cards_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityElementCards::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ELEMENT_CARDS)) return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ELEMENT_CARDS);

	switch (raor->opera_type)
	{
	case Protocol::RA_ELEMENT_CARDS_OP_TYPE_INFO:
		{
			this->SendRAElementCardsInfo();
		}
		break;
	case Protocol::RA_ELEMENT_CARDS_OP_TYPE_FLOP:
		{
			this->OnRAElementCardsFlop();
		}
		break;
	}
}

void RoleActivityElementCards::OnRAOpenOnlineSend()
{
	this->OnRAElementCardsDayReset();
}

void RoleActivityElementCards::SendRAElementCardsInfo()
{
	Protocol::SCRAElementCardsInfo info;
	info.flop_seq = m_param.ra_element_cards_last_seq;
	info.flop_count = m_param.ra_element_cards_count;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityElementCards::OnRAElementCardsDayReset()
{
	unsigned int day_id = EngineAdapter::Instance().DayID();
	if (day_id == m_param.ra_element_cards_day_id) return;

	const RandActivityElementCardsConfig * config = static_cast<const RandActivityElementCardsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ELEMENT_CARDS));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAElementCardsOtherCfg & cfg = config->GetElementCardsOtherCfg();

	m_param.ra_element_cards_count += cfg.login_add_count;
	m_param.ra_element_cards_day_id = day_id;
	this->SendRAElementCardsInfo();
	this->SetRoleDataChange();
}

void RoleActivityElementCards::OnRAElementCardsFlop()
{
	if (0 >= m_param.ra_element_cards_count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityElementCardsConfig * config = static_cast<const RandActivityElementCardsConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ELEMENT_CARDS));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAElementCardsCfg * cfg = config->GetElementCardsCfg(ramgr);
	if (NULL == cfg) return;

	if (0 == cfg->item.size())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->item.size(), &cfg->item[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)cfg->item.size(), &cfg->item[0], PUT_REASON_RA_ELEMENT_CARDS))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_element_cards_count--;
	m_param.ra_element_cards_last_seq = cfg->seq;
	this->SendRAElementCardsInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAElementCardsFlop user[%d, %s], level[%d], act_type[%d], seq[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_ELEMENT_CARDS, cfg->seq);

	this->OnParticipateRA(__FUNCTION__);
}
