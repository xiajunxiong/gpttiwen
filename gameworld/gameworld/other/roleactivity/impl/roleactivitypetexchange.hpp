#ifndef __ROLE_ACTIVITY_PET_EXCHANGE_HPP__
#define __ROLE_ACTIVITY_PET_EXCHANGE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rapetexchangeparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityPetExchange : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_PET_EXCHANGE_SHOP;			//≥ËŒÔ∂“ªª-…ÃµÍ
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP;

	RoleActivityPetExchange(int type);
	virtual ~RoleActivityPetExchange();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

public:
	void SendRAPetExchangeInfo();
	void OnSendSingleInfo(int type, int seq, int param1, int param2 = 0);
	void OnActivityPetExchange(int seq);
	void OnBuyItem(int seq);
	void OnBuyItemChecek(int seq);

	RAPetExchangeParam m_param;
};

#endif	//__ROLE_ACTIVITY_PET_EXCHANGE_HPP__
