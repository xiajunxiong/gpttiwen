#ifndef __ORDER_IMPL_BUY_HPP__
#define __ORDER_IMPL_BUY_HPP__

#include "global/trademarket/orderbase.hpp"

class OrderImplBuy : public OrderBase
{
public:
	OrderImplBuy(int relate_role_id);

	virtual void OnOrderFailed(bool is_return_resource);	// �ж��˶���ʧ�ܣ�ִ�ж���ʧ���߼���������Դ��
	virtual void OnOrderFinish();	// �ж��˶����ɹ���ִ�ж����ɹ��߼�
};

#endif