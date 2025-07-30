#ifndef __ROLE_ACTIVITY_DAN_BI_FAN_LI_HPP__
#define __ROLE_ACTIVITY_DAN_BI_FAN_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/radanbifanliparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityDanBiFanLi : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_DAN_BI_FAN_LI;			//µ¥±Ê·µÀû
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI;

	RoleActivityDanBiFanLi(int type);
	virtual ~RoleActivityDanBiFanLi();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	bool OnBuyItemCheck(int pool_seq, int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int pool_seq, int seq, ARG_OUT int * price);

private:
	void OnSendAllInfo();
	void OnSendSingleInfo(int pool_seq);


	RADanBiFanLiParam m_param;
};

#endif	//__ROLE_ACTIVITY_DAN_BI_FAN_LI_HPP__

