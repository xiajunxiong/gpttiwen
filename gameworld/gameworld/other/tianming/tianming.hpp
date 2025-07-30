#ifndef __TIAN_MING_HPP__
#define __TIAN_MING_HPP__

#include "servercommon/struct/tianmingparam.h"

class RoleModuleManager;
class Role;

class TianMing				//ÌìÃü
{
public:
	TianMing();
	~TianMing();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const TianMingParam & param);
	void GetInitParam(TianMingParam * param);

	void OnLogin();

	int GetAttrPer(int _sys_attr_type);
	void AddExp(int _exp , const char* _add_reason );

private:
	void SendInfo();
	void UpLevel(int _level);
	void ReCalcAttr(int _sys_type);

	RoleModuleManager * m_mgr;
	TianMingParam m_param;
};

#endif
