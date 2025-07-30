#ifndef __ROLE_ACTIVITY_CAI_SHEN_JU_BAO_HPP__
#define __ROLE_ACTIVITY_CAI_SHEN_JU_BAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityCaiShenJuBao : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_CAI_SHEN_JU_BAO_NEW;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO;

	RoleActivityCaiShenJuBao(int type);
	virtual ~RoleActivityCaiShenJuBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual bool IsFinishBonusCond(int type = 0);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnBuyItem(int seq, int chong_zhi_gold);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);

	int OnBuyItemCheck(int seq,long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo();

	RACaiShenJuBaoParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
