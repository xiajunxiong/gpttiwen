#include "roleactivitytrademarketreturngold.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/randactivity/msgratrademarketreturngold.hpp"
#include "gamelog.h"
#include "config/randactivityconfig/impl/randactivitytrademarketreturngoldconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/trademarket/trademarketmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivityTradeMarketReturnGold::RoleActivityTradeMarketReturnGold(int activity_type) : RoleActivity(activity_type)
{

}

RoleActivityTradeMarketReturnGold::~RoleActivityTradeMarketReturnGold()
{

}

void RoleActivityTradeMarketReturnGold::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RATradeMarketReturnGoldParam));

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type) && m_param.remain_claimable_gold > 0)
	{
		this->ReturnGoldByMail(__FUNCTION__);
	}
}

void RoleActivityTradeMarketReturnGold::GetInitParam(RoleActivityRoleData *p)
{
	UNSTD_STATIC_CHECK(sizeof(p->base_data) > sizeof(RATradeMarketReturnGoldParam));
	memcpy(p->base_data, &m_param, sizeof(RATradeMarketReturnGoldParam));
}

void RoleActivityTradeMarketReturnGold::OnLoginSendInfo()
{
	this->SendInfo();
	this->QueryHistoryTopPriceItemList();
}

void RoleActivityTradeMarketReturnGold::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (m_role_activity_type != rand_act_type)
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
		if (m_param.remain_claimable_gold > 0)
		{
			this->ReturnGoldByMail(__FUNCTION__);
		}

		m_param.total_buy_item_consume_gold = 0LL;
		m_param.total_claimable_gold = 0LL;
		m_param.remain_claimable_gold = 0LL;
		m_param.remain_claimable_gold_decimal = 0.0;
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();

		gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityTradeMarketReturnGold::CheckInitRandActivity role[%d,%s] total_consume_gold[%lld] total_claimable_gold[%lld] remain_claimable_gold[%lld]",
			m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_param.total_buy_item_consume_gold, m_param.total_claimable_gold, m_param.remain_claimable_gold);
	}
}

void RoleActivityTradeMarketReturnGold::OnRAClose()
{
	this->ReturnGoldByMail(__FUNCTION__);
}

void RoleActivityTradeMarketReturnGold::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(this->GetRoleActivityType()))
	{
		return;
	}
	this->CheckInitRandActivity(this->GetRoleActivityType());

	switch (raor->opera_type)
	{
	case Protocol::RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_INFO:
		{
			this->SendInfo();
			this->QueryHistoryTopPriceItemList();
		}
		break;
	case Protocol::RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_CALIM_GOLD:
		{
			this->ClaimReturnGold();
		}
		break;
	}
}

void RoleActivityTradeMarketReturnGold::OnTradeMarketBuyItemSucc(int consume_gold)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(this->GetRoleActivityType()))
	{
		return;
	}

	double add_claimable_gold = 0.0;
	const RandActivityTradeMarketReturnGoldConfig* cfg = static_cast<const RandActivityTradeMarketReturnGoldConfig *>(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(this->GetRoleActivityType()));
	if (NULL != cfg)
	{
		add_claimable_gold = cfg->GetReturnGoldNum(RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID()), consume_gold);
	}

	int add_claimable_gold_int = static_cast<int>(add_claimable_gold);
	double add_claimable_gold_decimal = add_claimable_gold - add_claimable_gold_int;

	m_param.total_buy_item_consume_gold += consume_gold;
	m_param.total_claimable_gold += add_claimable_gold_int;
	m_param.remain_claimable_gold += add_claimable_gold_int;
	m_param.remain_claimable_gold_decimal += add_claimable_gold_decimal;
	if (m_param.remain_claimable_gold_decimal > 1.0)
	{
		int temp = (int)m_param.remain_claimable_gold_decimal;
		m_param.total_claimable_gold += temp;
		m_param.remain_claimable_gold += temp;
		m_param.remain_claimable_gold_decimal -= temp;
	}

	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityTradeMarketReturnGold::OnTradeMarketBuyItem role[%d,%s] consume_gold[%d] total_consume_gold[%lld] add_claimable_gold[%d] total_claimable_gold[%lld] remain_claimable_gold[%lld] decimal[%.2f]",
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), consume_gold, m_param.total_buy_item_consume_gold, add_claimable_gold_int, m_param.total_claimable_gold, m_param.remain_claimable_gold, m_param.remain_claimable_gold_decimal);
}

void RoleActivityTradeMarketReturnGold::ReturnGoldByMail(const char* reason)
{
	if (m_param.remain_claimable_gold <= 0) return;

	static MailContentParam contentparam; contentparam.Reset();
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = (int)m_param.remain_claimable_gold;

	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_activity_trade_return_gold_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_activity_trade_return_gold_content, (int)m_param.total_buy_item_consume_gold, (int)m_param.total_claimable_gold - (int)m_param.remain_claimable_gold, (int)m_param.remain_claimable_gold);

	MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRoleUID()), MAIL_REASON_DEFAULT, contentparam);

	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityTradeMarketReturnGold::ReturnGoldByMail role[%d,%s] \
		total_consume_gold[%lld] total_claimable_gold[%lld] remain_claimable_gold[%lld] decimal[%.2f] reason[%s] ra_begin_timestamp[%u]",
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_param.total_buy_item_consume_gold, 
		m_param.total_claimable_gold, m_param.remain_claimable_gold, m_param.remain_claimable_gold_decimal,
		reason ? reason : "NULL", m_param.ra_begin_timestamp);

	m_param.remain_claimable_gold = 0LL;
	m_param.remain_claimable_gold_decimal = 0.0;
	m_param.total_buy_item_consume_gold = 0LL;
	m_param.total_claimable_gold = 0LL;
}

void RoleActivityTradeMarketReturnGold::SendInfo()
{
	Protocol::SCRATradeMarketReturnGoldInfo info;
	info.remain_calaimable_gold = m_param.remain_claimable_gold;
	info.total_buy_item_consume_gold = m_param.total_buy_item_consume_gold;
	info.total_calaimable_gold = m_param.total_claimable_gold;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityTradeMarketReturnGold::QueryHistoryTopPriceItemList()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenTradeMarketHistoryTopPriceReq req;
		req.role_id = m_role_module_mgr->GetRoleUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
	}
	else
	{
		Protocol::SCRATradeMarketReturnHotItemInfo resp;
		memset(resp.hot_item_list, 0, sizeof(resp.hot_item_list));
		int num = 0;
		TradeMarketManager::Instance().GetHistoryTopPriceItemList(ARRAY_LENGTH(resp.hot_item_list), &num, resp.hot_item_list);
		
		EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &resp);
	}
}

void RoleActivityTradeMarketReturnGold::ClaimReturnGold()
{
	if (m_param.remain_claimable_gold <= 0)
	{
		this->SendInfo();
		return;
	}

	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityTradeMarketReturnGold::ClaimReturnGold role[%d,%s] claim_gold[%lld]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.remain_claimable_gold);

	long long add_gold = m_param.remain_claimable_gold;
	m_param.remain_claimable_gold = 0;

	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	this->SendInfo();
}

