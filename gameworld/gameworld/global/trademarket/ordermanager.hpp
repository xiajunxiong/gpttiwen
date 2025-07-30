#ifndef __ORDER_MANAGER_HPP__
#define __ORDER_MANAGER_HPP__

#include <map>
#include <set>
#include "servercommon/servercommon.h"

// 订单模块（原服向隐藏服进行上架、购买时使用）
// 为了解决在原服先扣资源再到跨服进行操作时跨服断开连接造成原服玩家损失的问题
// 希望做成一个插件外挂到TradeMarket中，不对太多逻辑做耦合
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
	long long RegisterOrder(int order_type, int relate_role_id);   // 把订单注册到管理器中，返回订单ID

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

	OrderBase* CreateOrder(int order_type, int relate_role_id);	   // 根据订单类型创建订单实例

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
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