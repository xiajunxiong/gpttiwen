#include "orderfactory.hpp"
#include "orderimpl/orderimpladvertise.hpp"
#include "orderimpl/orderimplbuy.hpp"
#include "orderimpl/orderimplupshelve.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"

OrderBase* OrderFactory::CreateOrder(int order_type, int relate_role_id)
{
	switch (order_type)
	{
	case TRADEMARKET_ORDER_TYPE_ADVERTISE:
		{
			return new OrderImplAdvertise(relate_role_id);
		}
		break;

	case TRADEMARKET_ORDER_TYPE_UPSHELVE:
		{
			return new OrderImplUpshelve(relate_role_id);
		}
		break;

	case TRADEMARKET_ORDER_TYPE_BUY:
		{
			return new OrderImplBuy(relate_role_id);
		}
		break;

	default:
		break;
	}

	return NULL;
}

