#ifndef __ROLE_ACTIVITY_MO_ZU_ZAI_NA_LI_HPP__
#define __ROLE_ACTIVITY_MO_ZU_ZAI_NA_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ramozuzainaliparam.hpp"

class RoleActivityMoZuZaiNaLi : public RoleActivity
{
public:
	RoleActivityMoZuZaiNaLi(int type);
	virtual ~RoleActivityMoZuZaiNaLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnExpelMonster();
	int GetExpelMonsterNum() { return m_param.ra_frequency_num; }
public:
	void RASendInfo();

	RAMoZuZaiNaLiParam m_param;
};

#endif	//__ROLE_ACTIVITY_MO_ZU_ZAI_NA_LI_HPP__
