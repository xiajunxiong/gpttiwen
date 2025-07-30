#ifndef __ROLE_ACTIVITY_TIAN_YI_YAO_SHI_HPP__
#define __ROLE_ACTIVITY_TIAN_YI_YAO_SHI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityTianYiYaoShi : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_TIAN_YI_YAO_SHI;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI;

	RoleActivityTianYiYaoShi(int type);
	virtual ~RoleActivityTianYiYaoShi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);
private:
	// ³é½±
	void OnLottery(int lottery_times);
	// ¹ºÂòÀñ°ü
	void BuyGiftBox(int seq, int cfg_ver);

	void RASendInfo();
private:
	RATianYiYaoShiParam m_param;
};

#endif