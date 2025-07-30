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

	void OrderStart();			// ������ʼ����ʼ��������

	void OnOrderCostGold(long long gold_cost);
	void OnOrderCostCoin(long long coin_cost);
	void OnOrderConsumeItem(const ItemDataWrapper& wrapper);
	void OnOrderCostTradeCredit(long long credit);

	void SetOrderStatus(int order_status);
	virtual void OnOrderFailed(bool is_return_resource) = 0;	// �ж��˶���ʧ�ܣ�ִ�ж���ʧ���߼���������Դ��
	virtual void OnOrderFinish() = 0;	// �ж��˶����ɹ���ִ�ж����ɹ��߼�

	inline long long GetOrderId() const { return m_order_id; }
	inline unsigned int GetOrderCreateTime() const { return m_create_time; }

private:
	OrderBase();

protected:
	int m_order_type;			// ��������
	int m_order_status;			// ����״̬
	int m_relate_role_id;		// ������ɫID
	long long m_order_id;		// ����ID
	unsigned int m_create_time;	// ��������ʱ��
	OrderDetail m_order_detail;	// ��������
};

#endif