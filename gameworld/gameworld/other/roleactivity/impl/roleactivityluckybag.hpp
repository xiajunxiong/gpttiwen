#ifndef __ROLE_ACTIVITY_LUCKY_BAG_HPP__
#define __ROLE_ACTIVITY_LUCKY_BAG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raluckybagparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityLuckyBag : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_LUCKY_BAG_NEW;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG;
	
	static const int BATCH_BUY_TYPE = BUY_TIME_TYPE_LUCKY_BAG_BATCH;
	static const int  BATCH_COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH;

	RoleActivityLuckyBag(int type);
	virtual ~RoleActivityLuckyBag();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);
	int OnBuyGiftItem2(int seq, int ARG_OUT * price);	//batch
	bool OnRandActivityLuckyBagBuy(int seq, int chong_zhi_gold);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyItemCheck2(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem2(int chong_zhi_gold, long long unique_id);
public:
	void RandActivityLuckyBagSendInfo();
	void OnRandActivityLuckyBagFetchDayReward();
	void CheckBatchBuyReward();

	RALuckyBagParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__
