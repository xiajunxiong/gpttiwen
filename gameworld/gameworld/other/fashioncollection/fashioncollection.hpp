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
	// תְ
	void ChangeProfession(int old_prof, int new_prof);

	void OnClientRequest(const void* msg);

	void OnRoleLogin();

	// ����
	void OnActivation(int surface_id);
	// ʱװ����
	void Expired(int surface_id);
	//������ť�ֽ�ʱװ������Ӧ����
	int DecomposeSurface(short column, short index, int num);
private:
	// ����
	void OnLevelUp(int surface_id);
	// ר������
	void OnSpecializeActivation();

	void SendInfo();
private:
	RoleModuleManager* m_role_module;

	AttributeList m_attrs_add;

	FashionCollectionParam m_param;
};

#endif
