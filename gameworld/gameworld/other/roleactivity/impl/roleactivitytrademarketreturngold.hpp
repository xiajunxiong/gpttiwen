#ifndef __ROLE_ACTIVITY_TRADE_MARKET_RETURN_GOLD_HPP__
#define __ROLE_ACTIVITY_TRADE_MARKET_RETURN_GOLD_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratrademarketreturngoldparam.hpp"

class RoleActivityTradeMarketReturnGold : public RoleActivity
{
public:
	RoleActivityTradeMarketReturnGold(int activity_type);
	virtual ~RoleActivityTradeMarketReturnGold();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnTradeMarketBuyItemSucc(int consume_gold);
private:
	void ReturnGoldByMail(const char* reason);

	void SendInfo();
	void QueryHistoryTopPriceItemList();
	void ClaimReturnGold();

	RATradeMarketReturnGoldParam m_param;	
};

#endif