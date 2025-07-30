#ifndef __ROLE_ACTIVITY_COW_EXCHANGE_HPP__
#define __ROLE_ACTIVITY_COW_EXCHANGE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racowparam.hpp"

class RoleActivityCowExchange : public RoleActivity
{
public:
	RoleActivityCowExchange(int type);
	virtual ~RoleActivityCowExchange();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second) {}
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	bool OnNoreItemUse(int add_jifen);
	bool OnCmdBuyItem(int type_id, int real_pay);
public:
	void SendInfo();
	void BuyItem(int type_id,int seq_id_check);

	RACowExchangeParam m_param;
};

#endif	//__ROLE_ACTIVITY_COW_EXCHANGE_HPP__

