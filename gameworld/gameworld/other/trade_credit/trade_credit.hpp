#ifndef __TRADE_CREDIT_HPP__
#define __TRADE_CREDIT_HPP__

#include "servercommon/trade_credit_def.hpp"

class RoleModuleManager;
class TradeCredit
{
public:
	TradeCredit();
	~TradeCredit();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const TradeCreditParam& param);
	void GetInitParam(TradeCreditParam* out_param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnRoleLevelUp(int old_level, int now_level);
	void OnChongzhi(int add_gold_num);
	void OnAddActive(int add_point);
	void OnKillFieldBoss();
	void OnBuyFromTradeMarket(int consume_gold);
	bool UseCredit(long long credit_num, const char* reason);
	bool CreditMorThan(long long credit_num);
	bool AddCredit(long long credit_num, const char* reason, bool is_notice = true);

	inline long long GetTotalTradeCredit() const { return m_param.total_credit_num; }
	void SendCreditInfo();

private:
	void OnCreditEventHappen(int credit_type, int happen_times, const char* reason);

	RoleModuleManager* m_mgr;
	TradeCreditParam m_param;
};

#endif