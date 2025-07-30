#include "roleactivitypartnerexchange.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitypartnerexchangeconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"
#include "protocol/randactivity/msgrapartnerexchange.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityPartnerExchange::RoleActivityPartnerExchange(int type)
	:RoleActivity(type)
{

}

RoleActivityPartnerExchange::~RoleActivityPartnerExchange()
{

}

void RoleActivityPartnerExchange::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAPartnerExchangeParam));
}

void RoleActivityPartnerExchange::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAPartnerExchangeParam));
}

void RoleActivityPartnerExchange::OnLoginSendInfo()
{
	this->SendRAPartnerExchangeInfo();
}

void RoleActivityPartnerExchange::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_partner_exchange_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_partner_exchange_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		memset(m_param.ra_partner_exchange_times, 0, sizeof(m_param.ra_partner_exchange_times));

		this->SetRoleDataChange();
	}
}

void RoleActivityPartnerExchange::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityPartnerExchangeConfig * config = static_cast<const RandActivityPartnerExchangeConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE));
	if (NULL == config)
	{
		return;
	}

	//伙伴兑换重置每日限购次数
	for (int i = 0; i < MAX_RA_PARTNER_NUM; i++)
	{
		const RAPartnerExchangeCfg* cfg = config->GetPartnerExchangeBySeq(ramgr, i);
		if (cfg == NULL) continue;
		if (cfg->buy_tepy == RAPartnerExchangeCfg::BUY_TEPY_TODAY)
		{
			m_param.ra_partner_exchange_times[i] = 0;
		}
	}
	this->SendRAPartnerExchangeInfo();

	this->SetRoleDataChange();
}

void RoleActivityPartnerExchange::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE);

	switch (raor->opera_type)
	{
	case Protocol::RA_PARTNER_EXCHANGE_OPERA_TYPE_GET_INFO:
		{
			this->SendRAPartnerExchangeInfo();
		}
		break;
	case Protocol::RA_PARTNER_EXCHANGE_OPERA_TYPE_EXCHANGE:
		{
			this->OnActivityPartnerExchange(raor->param_1);
		}
		break;
	}
}

void RoleActivityPartnerExchange::SendRAPartnerExchangeInfo()
{
	Protocol::SCRAActivityPartnerExchangeInfo info;

	memcpy(info.ra_partner_exchange_times, m_param.ra_partner_exchange_times, sizeof(m_param.ra_partner_exchange_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityPartnerExchange::OnActivityPartnerExchange(int seq)
{
	if (seq < 0 || seq >= MAX_RA_PARTNER_NUM)return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityPartnerExchangeConfig * config = static_cast<const RandActivityPartnerExchangeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE));
	if (NULL == config)
	{
		return;
	}

	const RAPartnerExchangeCfg* cfg = config->GetPartnerExchangeBySeq(ramgr, seq);
	if (cfg == NULL)return;
	switch (cfg->buy_tepy)
	{
	case RAPartnerExchangeCfg::BUY_TEPY_ALL:
		if (m_param.ra_partner_exchange_times[seq] >= cfg->number)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
			return;
		}
		break;
	case RAPartnerExchangeCfg::BUY_TEPY_TODAY:
		if (m_param.ra_partner_exchange_times[seq] >= cfg->buy_seccond_day)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
			return;
		}
		break;
	default:
		return;
	}

	bool pass1_check = false;
	if (cfg->condition_id == 0 || m_role_module_mgr->GetPartner()->IsPartnerAlreadyActive(cfg->condition_id))
	{
		pass1_check = true;
	}

	bool pass2_check = false;
	if (cfg->item_id == 0 || m_role_module_mgr->GetKnapsack()->HasItem(cfg->item_id, cfg->condition_num))
	{
		pass2_check = true;
	}

	if (!pass1_check && !pass2_check)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_UNLOCK);
		return;
	}

	if (cfg->item1.item_id != 0 && cfg->item1.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->item1.item_id, cfg->item1.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
			return;
		}
	}
	if (cfg->item2.item_id != 0 && cfg->item2.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->item2.item_id, cfg->item2.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
			return;
		}
	}
	if (cfg->item3.item_id != 0 && cfg->item3.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->item3.item_id, cfg->item3.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
			return;
		}
	}
	if (cfg->reward_item.item_id != 0 && cfg->reward_item.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward_item.item_id, cfg->reward_item.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}
	}

	if (cfg->item1.item_id != 0 && cfg->item1.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item1.item_id, cfg->item1.num, __FUNCTION__))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnActivityPartnerExchange err[uid:%d,name:%s] consumeitem1[item_id:%d,num:%d,is_bind:%d]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				cfg->item1.item_id, cfg->item1.num, cfg->item1.is_bind ? 1 : 0);
			return;
		}
	}
	if (cfg->item2.item_id != 0 && cfg->item2.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item2.item_id, cfg->item2.num, __FUNCTION__))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnActivityPartnerExchange err[uid:%d,name:%s] consumeitem2[item_id:%d,num:%d,is_bind:%d]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				cfg->item2.item_id, cfg->item2.num, cfg->item2.is_bind ? 1 : 0);
			return;
		}
	}
	if (cfg->item3.item_id != 0 && cfg->item3.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item3.item_id, cfg->item3.num, __FUNCTION__))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnActivityPartnerExchange err[uid:%d,name:%s] consumeitem3[item_id:%d,num:%d,is_bind:%d]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				cfg->item3.item_id, cfg->item3.num, cfg->item3.is_bind ? 1 : 0);
			return;
		}
	}
	if (cfg->reward_item.item_id != 0 && cfg->reward_item.num != 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(cfg->reward_item, PUT_REASON_RA_PARTNER_EXCHANGE))
		{
			gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnActivityPartnerExchange err[uid:%d,name:%s] Put[item_id:%d,num:%d,is_bind:%d]", m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				cfg->reward_item.item_id, cfg->reward_item.num, cfg->reward_item.is_bind ? 1 : 0);
			return;
		}
	}

	m_param.ra_partner_exchange_times[seq] += 1;
	this->SendRAPartnerExchangeInfo();
	if (cfg->hearsay == 1 && !CrossConfig::Instance().IsHiddenServer())
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_partner_exchange_notice,
			m_role_module_mgr->GetRole()->GetName(), cfg->reward_item.item_id, RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
	}

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}
