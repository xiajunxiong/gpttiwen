#ifndef __INCOME_HPP__
#define __INCOME_HPP__


#include "servercommon/incomedef.hpp"

class RoleModuleManager;

class RoleIncome
{
public:
	RoleIncome();
	~RoleIncome();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const RoleIncomeParam & param);
	void GetInitParam(RoleIncomeParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();

	//��Ҳ���
	void OnReq(int type);

	//������Ϣ
	void SendInfo();

	//�¼���¼(ͬ��ʱ add_param = 0)
	void OnIncomeAdd(int income_type, long long add_param);


private:

	RoleModuleManager * m_module_mgr;

	RoleIncomeParam m_param;
};

#endif