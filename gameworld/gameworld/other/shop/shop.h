#ifndef __SHOP_H__
#define __SHOP_H__

#include <map>
#include "servercommon/shopdef.h"

class RoleModuleManager;
class Shop
{
public:
	Shop();
	~Shop();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const ShopParam& param);
	void GetOtherInitParam(ShopParam* param);

	void OnUserLogin();
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResetModuleDataWeek();

	void OnBuy(int index, int num);
	void OnGoldShopBuy(int index, int num);
	void OnFastBuy(int item_id, int num);
	bool GetFastBuyGoldUnitPrice(int item_id, ARG_OUT int* price);
	void OnBuyNewShop(int index, int num, short is_huashen_buy);

	void GmReserBuyCount(int type);

	void SendAllInfo();
	void SendSingleInfo(int index);
	void SendGoldSingleInfo(int index);
	void CancelSingleNotify(int index);
	void SendPurchaseNotify();

private:
	void CalcPurchaseNotify();

	RoleModuleManager * m_role_module_mgr;
	ShopParam m_param;
};

#endif // !__SHOP_H__