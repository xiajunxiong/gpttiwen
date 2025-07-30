#ifndef __ROLE_ACTIVITY_SUMMER_STORE_HPP__
#define __ROLE_ACTIVITY_SUMMER_STORE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

class RoleActivitySummerStore : public RoleActivity
{
public:
	RoleActivitySummerStore(int type);
	virtual ~RoleActivitySummerStore();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnCmdBuyItem(long long unique_id, int chong_zhi_gold);
private:
	void SendInfo(bool is_recal_phase = false);	
	void SendSingleInfo(short seq, short index);
	void BuyItem(int seq);
	void OnBuyItemChecek(int seq);
	void SetOpenFlag();

	RASummerStoreParam m_param;
};



#endif