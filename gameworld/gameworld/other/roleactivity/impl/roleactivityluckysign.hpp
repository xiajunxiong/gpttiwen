#ifndef __ROLE_ACTIVITY_LUCKY_SIGN_HPP__
#define __ROLE_ACTIVITY_LUCKY_SIGN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

class RoleActivityLuckySign : public RoleActivity
{
public:
	RoleActivityLuckySign(int type);
	virtual ~RoleActivityLuckySign();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendInfo();
	void OnSetShowFlag(int show_flag);
	void OnDaySign();

	RALuckySignParam m_param;
};



#endif	//__ROLE_ACTIVITY_LUCKY_SIGN_HPP__
