#ifndef __ROLE_ACTIVITY_CONSUME_CAROUSEL_HPP__
#define __ROLE_ACTIVITY_CONSUME_CAROUSEL_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"

class RoleActivityConsumeCarousel : public RoleActivity
{
public:
	RoleActivityConsumeCarousel(int type);
	virtual ~RoleActivityConsumeCarousel();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnConsumeGold(long long consume_gold);

public:
	void SendConsumeCarouselAllInfo();
	void SendConsumeCarouselInfo();
	void SendConsumeCarouselShow();
	void SendConsumeCarouselReward();

	void OnRAStartConsumeCarousel(int turn_type);
	bool OnRAGetConsumeCarouselReward(int seq, bool is_single = true);
	void PrintLog(const char* func);

	RAConsumerCarouselParam m_param;
};

#endif	//__ROLE_ACTIVITY_CONSUME_CAROUSEL_HPP__
