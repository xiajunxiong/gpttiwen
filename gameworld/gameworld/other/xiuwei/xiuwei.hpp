#ifndef  __XIU_WEI_HPP__
#define  __XIU_WEI_HPP__

#include "servercommon/struct/xiuweiparam.hpp"
#include "protocol/msgxiuwei.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;

class XiuWei
{
public:
	XiuWei();
	~XiuWei();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const XiuWeiParam &param);
	void GetOtherInitParam(XiuWeiParam *param);
	void OnLogin();
	void OnDayChange();

	void OnClientReq(Protocol::CSXiuWeiReq * _req);

	const AttributeList &  ReCalcAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList &  ReCalcPetAttr(AttributeList &base_add, int pet_level);
	const AttributeList &  ReCalcPartnerAttr(AttributeList &base_add, int partner_level);

	//收集修为特效
	void OnGetBattleEffect(int xiu_wei_character_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);	

private:
	void SendInfo();
	void SendInfoChange(int _xiu_wei_type);
	void SendDanYaoInfo();

	bool IsRoleType(int _xiu_wei_type);
	bool IsPetType(int _xiu_wei_type);
	bool IsParnerType(int _xiu_wei_type);

	void UpLevel(int _xiu_wei_type, int _dan_yao_type);
	void OneKeyUpLevel(int _xiu_wei_type, int _dan_yao_type);	//一键升级
	void ReCalcAttrByXiuWeiType(int _xiu_wei_type , AttributeList &base_add, const AttributeList base_attr_list, int level_by_type = 0);
	void GetBattleEffect(int _xiu_wei_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);

	RoleModuleManager *m_role_module_mgr;

	XiuWeiParam m_param;
	AttributeList m_attrs_add;
	AttributeList m_attrs_add_pet;
	AttributeList m_attrs_add_partner;
};

#endif	// __XIU_WEI_HPP__
