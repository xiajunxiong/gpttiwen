#ifndef __LOGIN_SYSTEM_HPP__
#define __LOGIN_SYSTEM_HPP__

#include "servercommon/loginsystemdef.hpp"

class RoleModuleManager;
class LoginSystem
{
public:
	LoginSystem();
	~LoginSystem();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager,const LoginSystemParam &param);
	void GetOtherInitParam(LoginSystemParam *param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();
	void OnGetReward(int login_day);
	bool IsOpenActivity();
	void ResetDataByActivityOpen();
private:
	RoleModuleManager *m_role_module_mgr;

	void SendLoginSystemInfo();
	void OnRefresh();

	LoginSystemParam m_param;
};


#endif // !__TRAIN_HPP__
