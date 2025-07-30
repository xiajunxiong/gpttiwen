#ifndef __ROLE_ACTIVITY_ZU_HE_HE_LI_HPP__
#define __ROLE_ACTIVITY_ZU_HE_HE_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

const static int ZU_HE_HE_LI_WU_CHA = 10;

class RoleActivityZuHeHeLi: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ZU_HE_HE_LI_2;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI;

	RoleActivityZuHeHeLi(int type);
	virtual ~RoleActivityZuHeHeLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnBuyItem(unsigned int buy_flag, int chong_zhi_gold, time_t buy_time, int ARG_OUT & err_num);
	int OnBuyGiftItem(int buy_seq1, int buy_seq2, ARG_OUT int * price);
	int BuyItemByGold(int buy_flag);

	int OnBuyItemCheck(unsigned int buy_flag,long long ARG_OUT & unique_key);
	bool OnBuyItem2(int chong_zhi_gold, long long unique_id);
	int Cancel();
public:
	void SendInfo();
	int OnBuyItem2Helper(int chong_zhi_gold, long long unique_id);

	RAZuHeHeLiParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
