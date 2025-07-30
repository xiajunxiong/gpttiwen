#ifndef __ZHOU_QI_SHOP_HPP__
#define __ZHOU_QI_SHOP_HPP__

#include "servercommon/cycleactivity/zhouqishopparam.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class ZhouQiShop
{
public:
	ZhouQiShop();
	~ZhouQiShop();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const ZhouQiShopParam & param);
	void GetInitParam(ZhouQiShopParam * param);

	void OnZhouQiInfoChange(int old_zhou_qi, unsigned int old_zhou_end_time);

	void Buy(int seq);
	void SendInfo();
private:
	void OnBuyStore(int seq);


	RoleModuleManager * m_mgr;
	ZhouQiShopParam m_param;
};

#endif
