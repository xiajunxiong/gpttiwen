#ifndef __ORDER_IMPL_UPSHELVE_HPP__
#define __ORDER_IMPL_UPSHELVE_HPP__

#include "global/trademarket/orderbase.hpp"

class OrderImplUpshelve : public OrderBase
{
public:
	OrderImplUpshelve(int relate_role_id);

	virtual void OnOrderFailed(bool is_return_resource);	// �ж��˶���ʧ�ܣ�ִ�ж���ʧ���߼���������Դ��
	virtual void OnOrderFinish();	// �ж��˶����ɹ���ִ�ж����ɹ��߼�
};

#endif