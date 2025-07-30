#ifndef __MYSTERIOUS_TRIAL_HPP__
#define __MYSTERIOUS_TRIAL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/mysterioustrialdef.hpp"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class MysteriousTrial
{
public:
	MysteriousTrial();
	~MysteriousTrial();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const MysteriousTrialParam &param);
	void GetInitParam(MysteriousTrialParam * param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();
	void OnInfoChange(int type, int num, int param = 0, int param2 = 0);
	void OnOpenTrial();

	void OnGetMysteriousTrialReward(int param1, int param2);
	void OnGetMysteriousTrialFinallyReward(int param1);
	bool OnBuyEveryDayDiscountReward(int day, int chong_zhi_gold, int type);
	void OnBuyItemCfgCheck(int type, int day);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int day, ARG_OUT int * price);

	void SendInfo();

	void GmAddMysteriousTrial(int day, int seq, int add_num);
	void GmSetDayId();
private:
	void SendSingleInfo(int day_index, int seq);
	void SendFinallyInfo();
	bool IsCheckTypeInvaild() { return m_param.type < MYSTERIOUS_TRIAL_TYPE_YUSHEN || m_param.type >= MYSTERIOUS_TRIAL_TYPE_MAX; }
	
	RoleModuleManager * m_module_mgr;
	MysteriousTrialParam m_param;
};



#endif