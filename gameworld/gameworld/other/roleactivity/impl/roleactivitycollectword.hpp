#ifndef __ROLE_ACTIVITY_COLLECT_WORD_HPP__
#define __ROLE_ACTIVITY_COLLECT_WORD_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racollectwordparam.hpp"

class RoleActivityCollectWord : public RoleActivity
{
public:
	RoleActivityCollectWord(int type);
	virtual ~RoleActivityCollectWord();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void RaCollectWordSendInfo();
	int RaCollectWordExchange(int seq);
	int RaCollectWordGetExchangeTimes(int seq);
	void RaCollectWordAddExchangeTimes(int seq);
	void RaCollectWordFetchDayGift();
	int RaCollectWordGetTotalExchangeTimes(int seq);

	RACollectWordParam m_param;
};

#endif	//__ROLE_ACTIVITY_COLLECT_WORD_HPP__
