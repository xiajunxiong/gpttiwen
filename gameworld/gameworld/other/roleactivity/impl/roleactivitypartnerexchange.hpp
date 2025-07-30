#ifndef __ROLE_ACTIVITY_PARTNER_EXCHANGE_HPP__
#define __ROLE_ACTIVITY_PARTNER_EXCHANGE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"

class RoleActivityPartnerExchange : public RoleActivity
{
public:
	RoleActivityPartnerExchange(int type);
	virtual ~RoleActivityPartnerExchange();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void SendRAPartnerExchangeInfo();

	void OnActivityPartnerExchange(int seq);

	RAPartnerExchangeParam m_param;
};

#endif	//__ROLE_ACTIVITY_PARTNER_EXCHANGE_HPP__
