#include "orderimplbuy.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "gamelog.h"
#include "global/trademarket/trademarketmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

OrderImplBuy::OrderImplBuy(int relate_role_id) : OrderBase(TRADEMARKET_ORDER_TYPE_BUY, relate_role_id)
{

}

void OrderImplBuy::OnOrderFailed(bool is_return_resource)
{
	gamelog::g_log_trade_market.printf(LL_WARNING, "OrderImplBuy::OnOrderFailed role_id[%d] \
		order_id[%lld] order_type[%d] order_status[%d] order_create_time[%u] return_coin[%lld] return_credit[%lld] return_item_id[%d] return_item_num[%d] \
		param_log[%s]",
		m_relate_role_id, m_order_id, m_order_type, m_order_status, m_create_time, 
		m_order_detail.total_cost_coin, m_order_detail.total_cost_credit,
		m_order_detail.consume_item.item_id, m_order_detail.consume_item.num,
		NetValueItemParam::GetParamLog(m_order_detail.consume_item.item_id, m_order_detail.consume_item.param_data));

	if (is_return_resource && m_order_status >= TRADEMARKET_ORDER_STATUS_CONSUME) // 消耗过的才能返还
	{
		//发邮件
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.item_list[0] = m_order_detail.consume_item;
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = static_cast<int>(m_order_detail.total_cost_coin);
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_TRADE_CREDIT] = static_cast<int>(m_order_detail.total_cost_credit);
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = static_cast<int>(m_order_detail.total_cost_gold);

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_order_return_resource_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_order_buy_return_resource);
		MailRoute::MailToUser(IntToUid(m_relate_role_id), MAIL_REASON_DEFAULT, contentparam);
	
		OrderManager::Instance().AddFailCount(__FUNCTION__);
	}
}

void OrderImplBuy::OnOrderFinish()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "OrderImplBuy::OnOrderFinish role_id[%d] order_id[%lld]",
		m_relate_role_id, m_order_id);
}

