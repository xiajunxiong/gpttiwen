#ifndef __ROLE_ACTIVITY_SPRING_FESTIVAL_STORE_HPP__
#define __ROLE_ACTIVITY_SPRING_FESTIVAL_STORE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raspringfestivalstoreparam.hpp"

class RoleActivitySpringFestivalStore : public RoleActivity
{
public:
	RoleActivitySpringFestivalStore(int type);
	virtual ~RoleActivitySpringFestivalStore();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second) {}
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	bool OnCmdBuyItem(int type_id, int real_pay);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);
public:
	void SendInfo();
	void BuyItem(int answer_id,int seq_id_check);

	RASpringFesticalStoreParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__

