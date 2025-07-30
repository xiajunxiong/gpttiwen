#ifndef __ROLE_HUNDRED_GOD_BOSS_HPP__
#define __ROLE_HUNDRED_GOD_BOSS_HPP__

#include "servercommon/servercommon.h"

class RoleModuleManager;
class RoleHundredGodBoss
{
public:
	RoleHundredGodBoss();
	~RoleHundredGodBoss();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr);

	void Update(unsigned int now_second);

	void OnRoleLogin();
	void OnFinishBounty();

	void OnStart();
	void OnFetchTargetReward(int target_id);
	void OnFecthFinishReward();

	void SendAllInfo();

private:

	bool IsAllFinish();
	bool IsFinishCond(int cond_type, int param);
	void TimeOver();

	RoleModuleManager * m_role_module_mgr;

};


#endif
