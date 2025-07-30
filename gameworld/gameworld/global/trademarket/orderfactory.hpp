#ifndef __ORDER_FACTORY_HPP__
#define __ORDER_FACTORY_HPP__

class OrderBase;
class OrderFactory
{
public:
	static OrderBase* CreateOrder(int order_type, int relate_role_id);
};

#endif