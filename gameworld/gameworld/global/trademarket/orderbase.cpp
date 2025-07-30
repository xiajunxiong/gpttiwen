#include "orderbase.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "engineadapter.h"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "gamelog.h"

OrderBase::OrderBase(int order_type, int relate_role_id) : m_order_type(order_type),
	m_order_status(0), m_relate_role_id(relate_role_id), m_order_id(0LL), m_create_time(0u)
{
	UniqueIDGenerator::Instance()->GenUniqueID(&m_order_id);
	m_create_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

OrderBase::~OrderBase()
{

}

void OrderBase::OrderStart()
{
	this->SetOrderStatus(TRADEMARKET_ORDER_STATUS_BEGIN);
}

void OrderBase::OnOrderCostGold(long long gold_cost)
{
	m_order_detail.total_cost_gold += gold_cost;

	gamelog::g_log_trade_market.printf(LL_INFO, "OnOrderCostGold role[%d] order_id[%lld] add_gold_cost[%lld]",
		m_relate_role_id, m_order_id, gold_cost);
}

void OrderBase::OnOrderCostCoin(long long coin_cost)
{
	m_order_detail.total_cost_coin += coin_cost;

	gamelog::g_log_trade_market.printf(LL_INFO, "OnOrderCostCoin role[%d] order_id[%lld] coin_cost[%lld]",
		m_relate_role_id, m_order_id, coin_cost);
}

void OrderBase::OnOrderConsumeItem(const ItemDataWrapper& wrapper)
{
	m_order_detail.consume_item = wrapper;

	gamelog::g_log_trade_market.printf(LL_INFO, "OnOrderConsumeItem role[%d] order_id[%lld] consume_item_id[%d] item_num[%d] item_unique_id[%lld] paramlog[%s]",
		m_relate_role_id, m_order_id, wrapper.item_id, wrapper.num, wrapper.item_unique_id, NetValueItemParam::GetParamLog(wrapper.item_id, wrapper.param_data));	
}

void OrderBase::OnOrderCostTradeCredit(long long credit)
{
	m_order_detail.total_cost_credit += credit;

	gamelog::g_log_trade_market.printf(LL_INFO, "OnOrderCostTradeCredit role[%d] order_id[%lld] credit[%lld]",
		m_relate_role_id, m_order_id, credit);
}

const char* GetOrderStatusText(int order_status)
{
	const char* ret = "Invalid";
	switch (order_status)
	{
	case TRADEMARKET_ORDER_STATUS_BEGIN:
		{
			ret = "Begin";
		}
		break;

	case TRADEMARKET_ORDER_STATUS_QUERY:
		{
			ret = "Query";
		}
		break;

	case TRADEMARKET_ORDER_STATUS_CONSUME:
		{
			ret = "Consume";
		}
		break;
	}

	return ret;
}

void OrderBase::SetOrderStatus(int order_status)
{
	m_order_status = order_status;

	gamelog::g_log_trade_market.printf(LL_INFO, "SetOrderStatus role[%d] order_id[%lld] order_status[%s]",
		m_relate_role_id, m_order_id, GetOrderStatusText(order_status));
}

