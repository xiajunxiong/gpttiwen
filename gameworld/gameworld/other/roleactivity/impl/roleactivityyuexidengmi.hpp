#ifndef __ROLE_ACTIVITY_YUE_XI_DENG_MI_HPP__
#define __ROLE_ACTIVITY_YUE_XI_DENG_MI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "protocol/randactivity/msgrayuexidengmi.hpp"

class RoleActivityYueXiDengMi : public RoleActivity
{
public:
	RoleActivityYueXiDengMi(int type);
	virtual ~RoleActivityYueXiDengMi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	void OnRandActivityOpRequest(Protocol::CSRAYueXiDengMiRequest *raor);

private:
	void RandQuestion();
	void Answer(const char * user_answer);

	void RASendInfo();
private:
	RAYueXiDengMiParam m_param;
};

#endif