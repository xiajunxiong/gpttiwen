#ifndef __ROLE_ACTIVITY_ELEMENT_CARDS_HPP__
#define __ROLE_ACTIVITY_ELEMENT_CARDS_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raelementcardsparam.hpp"

class RoleActivityElementCards : public RoleActivity
{
public:
	RoleActivityElementCards(int type);
	virtual ~RoleActivityElementCards();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAOpenOnlineSend();

public:
	void SendRAElementCardsInfo();

	void OnRAElementCardsDayReset();
	void OnRAElementCardsFlop();


	RAElementCardsParam m_param;
};

#endif	//__ROLE_ACTIVITY_ELEMENT_CARDS_HPP__
