#ifndef __ORDER_BASE_HPP__
#define __ORDER_BASE_HPP__

#include "servercommon/struct/itemlistparam.h"

struct OrderDetail
{
	OrderDetail() : total_cost_gold(0LL), total_cost_coin(0LL), total_cost_credit(0LL) 
	{
		consume_item.Reset();
	}

	long long total_cost_gold;
	long long total_cost_coin;
	long long total_cost_credit;
	ItemDataWrapper consume_item;
};

class OrderBase
{
public:
	OrderBase(int order_type, int relate_role_id);
	virtual ~OrderBase();

	void OrderStart();			// 订单开始（初始化订单）

	void OnOrderCostGold(long long gold_cost);
	void OnOrderCostCoin(long long coin_cost);
	void OnOrderConsumeItem(const ItemDataWrapper& wrapper);
	void OnOrderCostTradeCredit(long long credit);

	void SetOrderStatus(int order_status);
	virtual void OnOrderFailed(bool is_return_resource) = 0;	// 判定了订单失败，执行订单失败逻辑（返还资源）
	virtual void OnOrderFinish() = 0;	// 判定了订单成功，执行订单成功逻辑

	inline long long GetOrderId() const { return m_order_id; }
	inline unsigned int GetOrderCreateTime() const { return m_create_time; }

private:
	OrderBase();

protected:
	int m_order_type;			// 订单类型
	int m_order_status;			// 订单状态
	int m_relate_role_id;		// 关联角色ID
	long long m_order_id;		// 订单ID
	unsigned int m_create_time;	// 订单创建时间
	OrderDetail m_order_detail;	// 订单详情
};

#endif