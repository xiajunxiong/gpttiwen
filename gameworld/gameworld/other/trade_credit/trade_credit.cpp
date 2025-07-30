#include "trade_credit.hpp"
#include "global/trademarket/trademarketconfig.hpp"
#include "protocol/msgtrademarket.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"
#include "obj/character/role.h"

TradeCredit::TradeCredit() : m_mgr(NULL)
{

}

TradeCredit::~TradeCredit()
{

}

void TradeCredit::Init(RoleModuleManager* mgr, const TradeCreditParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void TradeCredit::GetInitParam(TradeCreditParam* out_param)
{
	if (NULL == out_param) return;

	*out_param = m_param;
}

void TradeCredit::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = CREDIT_TYPE_INVALID + 1; i < CREDIT_TYPE_MAX; ++i)
	{
		m_param.today_credit_num[i] = 0;
	}
}

void TradeCredit::OnRoleLevelUp(int old_level, int now_level)
{
	if (old_level >= now_level) return;

	this->OnCreditEventHappen(CREDIT_TYPE_LEVEL, now_level - old_level, __FUNCTION__);
}

void TradeCredit::OnChongzhi(int add_gold_num)
{
	if (add_gold_num <= 0) return;

	this->OnCreditEventHappen(CREDIT_TYPE_CHONGZHI, add_gold_num, __FUNCTION__);
}

void TradeCredit::OnAddActive(int add_point)
{
	if (add_point <= 0) return;

	this->OnCreditEventHappen(CREDIT_TYPE_ACTIVE, add_point, __FUNCTION__);
}

void TradeCredit::OnKillFieldBoss()
{
	this->OnCreditEventHappen(CREDIT_TYPE_FIELD_BOSS, 1, __FUNCTION__);
}

void TradeCredit::OnBuyFromTradeMarket(int consume_gold)
{
	if (consume_gold <= 0) return;

	this->OnCreditEventHappen(CREDIT_TYPE_TRADEMARKET_BUY, consume_gold, __FUNCTION__);
}

bool TradeCredit::UseCredit(long long credit_num, const char* reason)
{
	if (credit_num < 0 || m_param.total_credit_num < credit_num) return false;

	if (0 == credit_num) return true;

	m_param.total_credit_num -= credit_num;

	this->SendCreditInfo();

	gamelog::g_log_trade_credit.printf(LL_INFO, "TradeCredit::UseCredit role[%d,%s] use_num[%lld] cur_num[%lld] reason[%s]\n",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), credit_num, m_param.total_credit_num, reason ? reason : "NULL");

	return true;
}

bool TradeCredit::CreditMorThan(long long credit_num)
{
	return m_param.total_credit_num >= credit_num;
}

bool TradeCredit::AddCredit(long long credit_num, const char* reason, bool is_notice /* = true */)
{
	if (credit_num < 0) return false;
	if (0 == credit_num) return true;

	if (MAX_INT64 - credit_num < m_param.total_credit_num)
	{
		return false; //超出64位整型的限制
	}

	m_param.total_credit_num += credit_num;

	if (is_notice)
	{
		this->SendCreditInfo();
	}

	gamelog::g_log_trade_credit.printf(LL_INFO, "TradeCredit::AddCredit role[%d,%s] add_num[%lld] cur_num[%lld] reason[%s]\n",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), credit_num, m_param.total_credit_num, reason ? reason : "NULL");
	
	return true;
}

void TradeCredit::SendCreditInfo()
{
	Protocol::SCTradeMarketCreditInfo info;
	info.total_credit = this->GetTotalTradeCredit();

	m_mgr->NetSend(&info, sizeof(info));
}

void TradeCredit::OnCreditEventHappen(int credit_type, int happen_times, const char* reason)
{
	if (credit_type <= CREDIT_TYPE_INVALID || credit_type >= CREDIT_TYPE_MAX)
	{
		return;
	}

	const TradeCreditCfg* cfg = TradeMarketConfig::Instance().GetTradeCreditCfg(credit_type);
	if (NULL == cfg)
	{
		return;
	}

	if (cfg->day_credit_limit > 0 && m_param.today_credit_num[credit_type] >= cfg->day_credit_limit)
	{
		return;
	}

	int add_credit = cfg->credit_num_per_time * happen_times;
	int day_limit = cfg->day_credit_limit;
	if (0 == day_limit)
	{
		day_limit = MAX_INT;
	}
	if (day_limit - add_credit < m_param.today_credit_num[credit_type])
	{
		add_credit = day_limit - m_param.today_credit_num[credit_type];
		m_param.today_credit_num[credit_type] = day_limit;
	}
	else
	{
		m_param.today_credit_num[credit_type] += add_credit;
	}

	this->AddCredit(add_credit, reason, false);
	this->SendCreditInfo();
}

