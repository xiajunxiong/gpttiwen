#ifndef __ORDER_MANAGER_HPP__
#define __ORDER_MANAGER_HPP__

#include <map>
#include <set>
#include "servercommon/servercommon.h"

// ����ģ�飨ԭ�������ط������ϼܡ�����ʱʹ�ã�
// Ϊ�˽����ԭ���ȿ���Դ�ٵ�������в���ʱ����Ͽ��������ԭ�������ʧ������
// ϣ������һ�������ҵ�TradeMarket�У�����̫���߼������
class OrderBase;
struct ItemDataWrapper;
class OrderManager
{
public:
	typedef std::map<long long, OrderBase*> OrderMap;
	typedef std::set<long long> OrderRecordSet;

public:
	static OrderManager& Instance();

	void OnConnectCrossSucc();
	void OnDisconnectCross();

	void Update(unsigned int now);
	long long RegisterOrder(int order_type, int relate_role_id);   // �Ѷ���ע�ᵽ�������У����ض���ID

	void OnOrderCostGold(long long order_id, long long gold_cost);
	void OnOrderCostCoin(long long order_id, long long coin_cost);
	void OnOrderConsumeItem(long long order_id, const ItemDataWrapper& wrapper);
	void OnOrderCostTradeCredit(long long order_id, long long credit);

	void OnOrderFinish(long long order_id);
	void OnOrderFailed(long long order_id, bool is_return_resource_by_order, const char* reason = NULL);
	void OnOrderSwitchToStatus(long long order_id, int order_status);

	inline bool IsOpen() const { return m_is_open; }
	bool IsOrderIdValid(long long order_id);
	void SystemSetIsOpen(bool is_open);

	void AddFailCount(const char* reason);
private:
	OrderManager();
	~OrderManager();

	OrderBase* CreateOrder(int order_type, int relate_role_id);	   // ���ݶ������ʹ�������ʵ��

	//��ֹ�������ƣ��벻Ҫ���л����������������� ������������� ��Ϊ������ʱ��delete��ĳЩ��Ա������
	OrderManager(const OrderManager&);
	OrderManager& operator=(const OrderManager &);

	void Release();
	OrderBase* GetOrderPtr(long long order_id);

	void ClearFailCount();
	void TriggerFailSave(const char* reason);
	void ResetFailSave();

	OrderMap m_order_map;

	bool m_is_open;
	int m_fail_count;
};

#endif