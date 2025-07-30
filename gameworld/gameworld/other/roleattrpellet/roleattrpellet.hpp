#ifndef __ROLE_ATTR_PELLET_HPP__
#define __ROLE_ATTR_PELLET_HPP__

#include "other/attributelist.hpp"
#include "servercommon/roleattrpelletdef.hpp"

class RoleModuleManager;
class RoleAttrPellet
{
public:
	RoleAttrPellet();
	~RoleAttrPellet();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const RoleAttrPelletParam& param);
	void GetOtherInitParam(RoleAttrPelletParam *out_param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void ReCalcAttrPartner(AttributeList & base_add, int partner_id, int profession);

	void GetRolePelletUseList(int max_num, short * role_use_pellet_count);

	void OnUseAttrPellet(int pellet_id, int num);
	void OnUsePetAttrPellet(int pellet_id, int num, int pet_index);
	void OnUsePartnerAttrPellet(int pellet_id, int num, int partner_id);

	void OnSendAllInfo();
	void OnSendSingleInfo(short role_attr_pellet_type, short use_count, int attr_pellet_type, int param1);

private:
	RoleModuleManager* m_module_mgr;
	RoleAttrPelletParam m_param;

	AttributeList m_attrs_add;
};



#endif