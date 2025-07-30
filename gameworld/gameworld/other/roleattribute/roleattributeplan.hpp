#ifndef __ROLE_ATTRIBUTE_HPP__
#define __ROLE_ATTRIBUTE_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/attributedef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class RoleAttributePlan
{
public:
	RoleAttributePlan();
	~RoleAttributePlan();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const AttributePlanParam& param);
	void GetOtherInitParam(AttributePlanParam *out_param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	void OnRoleLevelUp(int delta_level, int old_level);
	void CheckAutoAdd(bool is_send=true);
	void SetCustomAddPoint(int plan_index, short(&custom_add_point)[ADD_POINT_TYPE_MAX]);

	void CreateAttributePlanReq(const char* plan_name);
	void ChangeAttributePlanNameReq(short plan_index, const char* plan_name);
	void SwitchAttributePlanReq(short switch_to_index);
	void AddAttributePointReq(short plan_index, short vitality, short strength, short power, short speed, short magic);
	void ClearAttributePointReq(short plan_index, bool is_chagne_prof = false);
	void OnRoleChangeProf();			//玩家转职,自动洗点
	
	void OnRoleFirstTransfer();				// 首次转职自动推荐加点（洗点+自动加点）

	void SendPlanInfo();

	void GetAttributePlan(AttributePlan& plan) const;

	bool GetAttributeList(AttributeList* out_attrs_list);
private:
	RoleModuleManager* m_module_mgr;
	AttributePlanParam m_param;

	AttributeList m_attrs_add;
};

#endif