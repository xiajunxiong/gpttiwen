#ifndef __ROLE_TIAN_DI_DAO_HEN_HPP__
#define __ROLE_TIAN_DI_DAO_HEN_HPP__

#include "servercommon/tiandidaohendef.hpp"

class RoleModuleManager;
class RoleTianDiDaoHen
{
public:
	RoleTianDiDaoHen();
	~RoleTianDiDaoHen();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleTianDiDaoHenParam & param);
	void GetInitParam(RoleTianDiDaoHenParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void RandDropItem();
	void UseCrystalEnergy(int num);

	int ActiveItem(int item_index);
	int FetchFinalReward();
	void SendInfo();
private:
	RoleModuleManager * m_module_mgr;
	
	RoleTianDiDaoHenParam m_param;
};

#endif