#ifndef __LOVERS_MARKET_HPP__
#define __LOVERS_MARKET_HPP__

#include "servercommon/cycleactivity/loversmarketparam.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class LoversMarket
{
public:
	LoversMarket();
	~LoversMarket();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const LoversMarketParam & param);
	void GetInitParam(LoversMarketParam * param);

	void OnClientReq(void * msg);

	void OnDayChange();
	void SendInfo();
private:
	void OnBuyStore(int seq);


	RoleModuleManager * m_mgr;
	LoversMarketParam m_param;
};

#endif
