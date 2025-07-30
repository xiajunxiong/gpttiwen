#ifndef __ROLE_ACTIVITY_ZHEN_PIN_SHANG_PU_HPP__
#define __ROLE_ACTIVITY_ZHEN_PIN_SHANG_PU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityZhenPinShangPu: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU;

	RoleActivityZhenPinShangPu(int type);
	virtual ~RoleActivityZhenPinShangPu();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int GetDayBuyTimes(int type);
	int GetBuyTimes(int type);
	void AddDayBuyTimes(int type);
	void AddBuyTimes(int type);

	int OnBuyItemCheck(int seq, int select_flag, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int seq, int select_flag, ARG_OUT int * price);

public:
	void SendInfo();
	void SendOpenDayInfo();

	RAZhenPinShangPuParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
