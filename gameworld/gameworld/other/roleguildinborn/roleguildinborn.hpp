#ifndef __ROLE_GUILD_INBORN_HPP__
#define __ROLE_GUILD_INBORN_HPP__

#include "servercommon/roleguildinborndef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class RoleGuildInborn
{
public:
	RoleGuildInborn();
	~RoleGuildInborn();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const RoleGuildInbornParam & param);
	void GetInitParam(RoleGuildInbornParam * param);

	void OnRoleLogin();
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc, AttributeList &skill_attr_list);
	int GetSkillList(std::vector<SkillPro> * skill_list, int add_target_type);
	int GetSkillScroe(int add_target_type);

	void OnLearnSkill(int inborn_type, int type_seq);
	void OnResetSkill(int inborn_type, int type_seq);

	void OnSetGuildLevel(int guild_level);

	void GetPassiveSkillByType(int add_target_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);	//收集灵脉被动特效

	void SendAllInfo();
	void SendSingleInfo(int inborn_type, int type_seq, int skill_level);
	void SendSingleInfoByType(int inborn_type);

private:

	bool IsInvaild(int inborn_type) {  return inborn_type < 0 || inborn_type >= GUILD_INBORN_SKILL_TYPE_MAX; }
	bool IsInvaild(int inborn_type, int type_seq);
	void OnSkillChange(int inborn_type, int add_target_type);

	RoleModuleManager * m_module_mgr;
	RoleGuildInbornParam m_param;
	AttributeList m_attrs_add;

};


#endif