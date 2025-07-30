#ifndef __PRESTIGE_HPP__
#define __PRESTIGE_HPP__

#include "servercommon/prestigedef.hpp"

class RoleModuleManager;
class Prestige
{
public:
	Prestige();
	~Prestige();
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const NewPrestigeParam &param);
	void GetOtherInitParam(NewPrestigeParam *param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnCheckPartnerUpgradeTask();

	bool AddPrestige(int village_id, int add, const char* reason = NULL, short *use_num = NULL, int item_id = -1);
	bool CheckPrestige(int village_id, int level);
	int GetVillageLevel(int village_id);
	int GetMaxVillageLevel();
	void OnShoping(int index, int num);
	int GetVillageCountByLevelLower(int level);
	int GetMaxPrestigeVillageId();

	void OnDonate(int village_id, int column, int index);

	void SendAllInfo();
	void SendSingleVillageInfo(int village_id);
	void SendSingleShopInfo(int index);
	void SendDonateInfo();
private:
	void CheckBuyCountVersion();
	void ResetBuyCount(int limit_type);
	void ShowLog(const PrestigeParam& param, const char* source);
	void ShowLog(const NewPrestigeParam& param, const char* source);
	void CheckParamData(const char* source);

	RoleModuleManager *m_role_module_mgr;
	NewPrestigeParam m_param;
};

#endif // !__PRESTIGE_HPP__
