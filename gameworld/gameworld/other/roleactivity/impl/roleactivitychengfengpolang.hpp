#ifndef __ROLE_ACTIVITY_CHENG_FENG_PO_LANG_HPP__
#define __ROLE_ACTIVITY_CHENG_FENG_PO_LANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"
#include "global/randactivity/randactivityimpl/randactivitychengfengpolang.hpp"

class RoleActivityChengFengPoLang : public RoleActivity
{
public:
	RoleActivityChengFengPoLang(int type);
	virtual ~RoleActivityChengFengPoLang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void RandQuestion();
	void AnswerReq(int answer);
	
private:
	void RASendInfo();
	void OnJoinActivity();

	RAChengFengPoLangParam m_param;
};

#endif