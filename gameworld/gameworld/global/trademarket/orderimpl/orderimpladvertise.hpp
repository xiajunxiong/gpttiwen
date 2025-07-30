#ifndef __ORDER_IMPL_ADVERTISE_HPP__
#define __ORDER_IMPL_ADVERTISE_HPP__

#include "global/trademarket/orderbase.hpp"

class OrderImplAdvertise : public OrderBase
{
public:
	OrderImplAdvertise(int relate_role_id);

	virtual void OnOrderFailed(bool is_return_resource);	// �ж��˶���ʧ�ܣ�ִ�ж���ʧ���߼���������Դ��
	virtual void OnOrderFinish();	// �ж��˶����ɹ���ִ�ж����ɹ��߼�
};

#endif