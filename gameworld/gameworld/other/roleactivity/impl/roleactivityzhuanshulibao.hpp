#ifndef __ROLE_ACTIVITY_ZHUAN_SHU_LI_BAO_HPP__
#define __ROLE_ACTIVITY_ZHUAN_SHU_LI_BAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityZhuanShuLiBao: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_NEW;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO;

	static const int BUY_TYPE_DAY = BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT;
	static const int COMMON_SAVE_TYPE_DAY = ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY;

	RoleActivityZhuanShuLiBao(int type);
	virtual ~RoleActivityZhuanShuLiBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRoleLevelUp(int old_level, int to_level);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnBuyItemOld(int type,int chong_zhi_gold,time_t time);
	int OnBuyGiftItem(int type, ARG_OUT int * price);

	int OnBuyItemCheck(int type, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	void OnBuyDayItemCheck(int phase, int type);
	int OnBuyDayItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftDayItem(int phase, int type, ARG_OUT int * price);

	void GmSetNextDayTimestamp();
public:

	void SendInfo();
	void SendDayInfo();
	void OnCheckDayGift(bool is_sort = false);
	bool OnCheckRefresh();

	RAZhuanShuLiBaoParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
