/*	星图系统
 *
 */
#ifndef __START_CHART_H__
#define __START_CHART_H__

#include "servercommon/starchartdef.h"
#include "protocol/msgstarchart.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"

class RoleModuleManager;

class StarChart
{
public:
	StarChart();
	~StarChart();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const StarChartParam & param);
	void GetInitParam(StarChartParam * param);

	// 客户端请求
	void OnClientRequest(Protocol::CSStarChartRequest& req);

	void OnRoleLogin();

	// 星图星位升级
	void StarChartLevelUp(int star_chart_type, int star_id, int star_stage);
	void StarChartOneKeyLevelUp(int star_chart_type, int star_id, int star_stage);
	// 超星系星位升级
	void SuperGalaxiesLevelUp(int super_star_id);
	void SuperGalaxiesOneKeyLevelUp(int super_star_id);
	// 重置超星系星位
	void ResetSuperGalaxies();
	// 解锁技能孔位
	void UnlockSkillHole(int star_chart_type, int star_id);
	// 装备技能
	void InstallSkill(int star_chart_type, int skill_hole_seq, int super_star_id);

	// 转职
	void ChangeProfession(int old_prof, int new_prof);

	// 计算属性 角色/宠物/伙伴
	const AttributeList& ReCalcAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList& ReCalcPetAttr(AttributeList &base_add);
	const AttributeList& ReCalcPartnerAttr(AttributeList &base_add);

	//收集角色星图特效
	void OnGetRoleBattleEffect(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);
	//收集宠物星图特效
	void GetPetEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);
	//收集伙伴星图特效
	void GetPartnerBattleEffect(int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);
public:
	// 判断星图/超星系星位是否有效, stage可以直接传0, 代表第一阶的小星图, 处理技能时会用到
	bool ValidStarChartPoint(int star_chart_type, int stage, int star_id) const;
	bool ValidSuperGalaxiesPoint(int star_id) const;

	void GmAllLigthUp();
	void GmResetStarChart();
private:
	// 重算星图属性
	void ReCalStarTypeAttr(int star_chart_type);
	// 重算超星系属性
	void ReCalSuperGalaxiesAttr();

	// 计算小星图当前阶段是否已全部点亮
	bool IsStarChartAllLightUp(int star_chart_type, int star_chart_stage);

	// 重算 角色/宠物/伙伴/全部 属性
	void NoticeStarAttrChange(int star_chart_type = STAR_CHART_TYPE_MAX);

	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	AttributeList m_star_chart_attrs_add[STAR_CHART_TYPE_MAX];		// 星图属性
	AttributeList m_super_galaxies_attrs_add[STAR_CHART_TYPE_MAX];	// 超星系有三类属性,必须分开计算

	StarChartParam m_param;
};

#endif
