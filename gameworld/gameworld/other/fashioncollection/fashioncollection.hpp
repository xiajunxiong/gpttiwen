#ifndef __FASHION_COLLECTION_HPP__
#define __FASHION_COLLECTION_HPP__

#include "servercommon/fashioncollectiondef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class FashionCollection
{
public:
	FashionCollection();
	~FashionCollection();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const FashionCollectionParam &param);
	void GetOtherInitParam(FashionCollectionParam *param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	// 转职
	void ChangeProfession(int old_prof, int new_prof);

	void OnClientRequest(const void* msg);

	void OnRoleLogin();

	// 激活
	void OnActivation(int surface_id);
	// 时装过期
	void Expired(int surface_id);
	//背包按钮分解时装升级对应道具
	int DecomposeSurface(short column, short index, int num);
private:
	// 升级
	void OnLevelUp(int surface_id);
	// 专精激活
	void OnSpecializeActivation();

	void SendInfo();
private:
	RoleModuleManager* m_role_module;

	AttributeList m_attrs_add;

	FashionCollectionParam m_param;
};

#endif
