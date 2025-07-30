#ifndef __ORDER_IMPL_UPSHELVE_HPP__
#define __ORDER_IMPL_UPSHELVE_HPP__

#include "global/trademarket/orderbase.hpp"

class OrderImplUpshelve : public OrderBase
{
public:
	OrderImplUpshelve(int relate_role_id);

	virtual void OnOrderFailed(bool is_return_resource);	// 判定了订单失败，执行订单失败逻辑（返还资源）
	virtual void OnOrderFinish();	// 判定了订单成功，执行订单成功逻辑
};

#endif