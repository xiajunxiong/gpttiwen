#ifndef __ROLE_ACTIVITY_BO_ZHONG_YOU_LI_HPP__
#define __ROLE_ACTIVITY_BO_ZHONG_YOU_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

class RoleActivityBoZhongYouLi : public RoleActivity
{
public:
	RoleActivityBoZhongYouLi(int type);
	virtual ~RoleActivityBoZhongYouLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void Sowing();		// ≤•÷÷
	void Watering();	// π‡∏»
	void ReceiveReward(int seq);	// ’ªÒ

private:
	void RASendInfo();
	void OnJoinActivity();

	RABoZhongYouLiParam m_param;
};

#endif
