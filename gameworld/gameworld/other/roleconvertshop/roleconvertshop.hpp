#ifndef __ROLE_CONVERT_SHOP_HPP__
#define __ROLE_CONVERT_SHOP_HPP__

#include "servercommon/roleconvertshopparam.hpp"

class Role;
class RoleModuleManager;
class RoleConvertShop
{
public:
	RoleConvertShop();
	~RoleConvertShop();

	void* operator new(size_t c);
	void  operator delete(void* m);

	void Init(RoleModuleManager* mgr, const RoleConvertShopParam& param);
	void GetInitParam(RoleConvertShopParam* out_param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();
	void OnHuanJieChange();
	void OnCloudArenaSeasonChange();

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResetModuleDataWeek();
	void OnResetModuleDataMonth();

	void BuyReq(int convert_type, int seq, int buy_num, ItemID item_id);
	bool ConsumeSurfaceCoin(int money_type, int consume_num, ItemID item_id);

	void SendAllInfo();
	void SendRecordList();
	void SendRecordChange(int convert_type, int seq);

private:
	void ClearNonExistItemRecord(int convert_type);
	bool ConsumeResource(int money_type, int consume_num, ItemID item_id);
	void ResetConvertTimes(int limit_type);

	RoleModuleManager* m_mgr;
	RoleConvertShopParam m_param;
};

#endif