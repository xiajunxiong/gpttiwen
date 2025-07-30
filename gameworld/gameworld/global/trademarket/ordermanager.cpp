#include "ordermanager.hpp"
#include "orderbase.hpp"
#include "orderfactory.hpp"
#include "gamelog.h"

OrderManager::OrderManager() : m_is_open(true), m_fail_count(0)
{

}

OrderManager::~OrderManager()
{
	this->Release();
}

OrderManager& OrderManager::Instance()
{
	static OrderManager inst;
	return inst;
}

void OrderManager::OnConnectCrossSucc()
{
	this->ResetFailSave();
}

void OrderManager::OnDisconnectCross()
{
	this->TriggerFailSave(__FUNCTION__);
}

void OrderManager::Update(unsigned int now)
{
	std::vector<long long> expire_list;

	OrderMap::iterator it = m_order_map.begin();
	for (; it != m_order_map.end(); ++it)
	{
		OrderBase* order = it->second;
		if (NULL == order) continue;

		if (now > order->GetOrderCreateTime() + 10)
		{
			expire_list.push_back(it->first);
		}
	}

	for (size_t i = 0; i < expire_list.size(); ++i)
	{
		this->OnOrderFailed(expire_list[i], true, "OrderExpire");
	}
}

OrderBase* OrderManager::CreateOrder(int order_type, int relate_role_id)
{
	return OrderFactory::CreateOrder(order_type, relate_role_id);
}

long long OrderManager::RegisterOrder(int order_type, int relate_role_id)
{
	OrderBase* order_base = this->CreateOrder(order_type, relate_role_id);
	if (NULL == order_base)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}

	if (0 == order_base->GetOrderId())
	{
#ifdef _DEBUG
		assert(0);
#endif
		return 0;
	}

	m_order_map.insert(OrderMap::value_type(order_base->GetOrderId(), order_base));
	order_base->OrderStart();

	return order_base->GetOrderId();
}

void OrderManager::OnOrderCostGold(long long order_id, long long gold_cost)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderCostGold(gold_cost);
}

void OrderManager::OnOrderCostCoin(long long order_id, long long coin_cost)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderCostCoin(coin_cost);
}

void OrderManager::OnOrderConsumeItem(long long order_id, const ItemDataWrapper& wrapper)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderConsumeItem(wrapper);
}

void OrderManager::OnOrderCostTradeCredit(long long order_id, long long credit)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderCostTradeCredit(credit);
}

void OrderManager::OnOrderFinish(long long order_id)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderFinish();

	delete order;
	m_order_map.erase(order_id);

	this->ClearFailCount();
}

void OrderManager::OnOrderFailed(long long order_id, bool is_return_resource_by_order, const char* reason /* = NULL */)
{
	gamelog::g_log_trade_market.printf(LL_WARNING, "OrderManager::OnOrderFailed order_id[%lld] reason[%s]", order_id, reason ? reason : "NULL");

	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->OnOrderFailed(is_return_resource_by_order);

	delete order;
	m_order_map.erase(order_id);
}

void OrderManager::ClearFailCount()
{
	m_fail_count = 0; // 成功一次则清0 fail count

	gamelog::g_log_trade_market.printf(LL_WARNING, "OrderManager::OnOrderFinish clear fail count");
}

void OrderManager::AddFailCount(const char* reason)
{
	m_fail_count += 1;
	if (m_fail_count > 5)
	{
		gamelog::g_log_trade_market.printf(LL_WARNING, "OrderManager::OnOrderFailed fail count > 5, close OrderManager");

		this->TriggerFailSave(reason);
	}
}

void OrderManager::TriggerFailSave(const char* reason)
{
	if (m_is_open)
	{
	m_fail_count = 0;
	m_is_open = false;

		gamelog::g_log_trade_market.printf(LL_ERROR, "OrderManager::TriggerFailSave reason:%s", reason);
	}

	//std::string stack_trace = PrintStackBackTrace(false);
	//gamelog::g_log_trade_market.printf(LL_ERROR, "OrderManager::TriggerFailSave stack_trace:\n%s", stack_trace.c_str());
}

void OrderManager::ResetFailSave()
{
	if (!m_is_open)
	{
		gamelog::g_log_trade_market.print(LL_INFO, "OrderManager::ResetFailSave");
		m_is_open = true;
	}
}

void OrderManager::OnOrderSwitchToStatus(long long order_id, int order_status)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return;

	order->SetOrderStatus(order_status);
}

bool OrderManager::IsOrderIdValid(long long order_id)
{
	OrderBase* order = this->GetOrderPtr(order_id);
	if (NULL == order) return false;

	return true;
}

void OrderManager::SystemSetIsOpen(bool is_open)
{
	m_fail_count = 0;
	m_is_open = is_open;

	gamelog::g_log_trade_market.printf(LL_INFO, "OrderManager::SystemSetIsOpen m_is_open[%s]", m_is_open ? "True" : "False");
}

void OrderManager::Release()
{
	OrderMap::iterator it = m_order_map.begin();
	for (; it != m_order_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}

	m_order_map.clear();
}

OrderBase* OrderManager::GetOrderPtr(long long order_id)
{
	OrderMap::iterator it = m_order_map.find(order_id);
	if (it != m_order_map.end())
	{
		return it->second;
	}
	return NULL;
}

