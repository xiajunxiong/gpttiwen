#ifndef __OFFLINE_EXP_HPP__
#define __OFFLINE_EXP_HPP__

#include "other/attributelist.hpp"

class RoleModuleManager;
class OfflineExp
{
public:
	OfflineExp();
	~OfflineExp();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager,unsigned int offline_exp,int offline_min);
	void GetOtherInitParam(unsigned int *offline_exp,int *offline_min);

	void OnRoleLogin(long long last_save_time);
	void ReceiveExp();

private:
	RoleModuleManager *m_role_module_mgr;

	void SendOfflineExpInfo(int offline_m);

	unsigned int m_offline_exp_addition;
	int m_offline_min;
};


#endif // !__TRAIN_HPP__
