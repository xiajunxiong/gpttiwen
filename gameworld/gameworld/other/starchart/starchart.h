/*	��ͼϵͳ
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

	// �ͻ�������
	void OnClientRequest(Protocol::CSStarChartRequest& req);

	void OnRoleLogin();

	// ��ͼ��λ����
	void StarChartLevelUp(int star_chart_type, int star_id, int star_stage);
	void StarChartOneKeyLevelUp(int star_chart_type, int star_id, int star_stage);
	// ����ϵ��λ����
	void SuperGalaxiesLevelUp(int super_star_id);
	void SuperGalaxiesOneKeyLevelUp(int super_star_id);
	// ���ó���ϵ��λ
	void ResetSuperGalaxies();
	// �������ܿ�λ
	void UnlockSkillHole(int star_chart_type, int star_id);
	// װ������
	void InstallSkill(int star_chart_type, int skill_hole_seq, int super_star_id);

	// תְ
	void ChangeProfession(int old_prof, int new_prof);

	// �������� ��ɫ/����/���
	const AttributeList& ReCalcAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList& ReCalcPetAttr(AttributeList &base_add);
	const AttributeList& ReCalcPartnerAttr(AttributeList &base_add);

	//�ռ���ɫ��ͼ��Ч
	void OnGetRoleBattleEffect(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);
	//�ռ�������ͼ��Ч
	void GetPetEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);
	//�ռ������ͼ��Ч
	void GetPartnerBattleEffect(int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);
public:
	// �ж���ͼ/����ϵ��λ�Ƿ���Ч, stage����ֱ�Ӵ�0, �����һ�׵�С��ͼ, ������ʱ���õ�
	bool ValidStarChartPoint(int star_chart_type, int stage, int star_id) const;
	bool ValidSuperGalaxiesPoint(int star_id) const;

	void GmAllLigthUp();
	void GmResetStarChart();
private:
	// ������ͼ����
	void ReCalStarTypeAttr(int star_chart_type);
	// ���㳬��ϵ����
	void ReCalSuperGalaxiesAttr();

	// ����С��ͼ��ǰ�׶��Ƿ���ȫ������
	bool IsStarChartAllLightUp(int star_chart_type, int star_chart_stage);

	// ���� ��ɫ/����/���/ȫ�� ����
	void NoticeStarAttrChange(int star_chart_type = STAR_CHART_TYPE_MAX);

	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	AttributeList m_star_chart_attrs_add[STAR_CHART_TYPE_MAX];		// ��ͼ����
	AttributeList m_super_galaxies_attrs_add[STAR_CHART_TYPE_MAX];	// ����ϵ����������,����ֿ�����

	StarChartParam m_param;
};

#endif
