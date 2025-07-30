#ifndef __ROLE_ACTIVITY_YUN_ZE_CATCH_ELVES_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_CATCH_ELVES_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzecatchelvesparam.hpp"
#include "protocol/randactivity/msgrayunzecatchelves.hpp"

class RoleActivityYunZeCatchElves : public RoleActivity
{
public:
	RoleActivityYunZeCatchElves(int type);
	virtual ~RoleActivityYunZeCatchElves();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
 	void RASendInfo();
	void CatchElves();
	void BatchCatchElves();
	void JoinActivity(int times = 1);

	RAYunZeCatchElvesParam m_param;
};


#endif