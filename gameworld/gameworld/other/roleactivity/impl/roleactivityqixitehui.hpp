#ifndef __ROLE_ACTIVITY_QI_XI_TE_HUI_HPP__
#define __ROLE_ACTIVITY_QI_XI_TE_HUI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raqixitehuiparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityQiXiTeHui : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_QI_XI_TE_HUI;			//七夕特惠
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI;

	RoleActivityQiXiTeHui(int type);
	virtual ~RoleActivityQiXiTeHui();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	// 领取阶段奖励
	void OnFetchPhaseReward(int seq);
	// 取消红点
	void OnCancelRedDot();

	void OnBuyItemCheck(int seq);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

private:
	void OnSendAllInfo();
	void OnSendSingleInfo(int seq);
	void OnSendOtherInfo();

	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	RAQiXiTeHuiParam m_param;
};

#endif	//__ROLE_ACTIVITY_QI_XI_TE_HUI_HPP__

