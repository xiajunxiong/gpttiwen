#ifndef __FABAO_HPP__
#define __FABAO_HPP__

#include "servercommon/fabaodef.hpp"
#include "other/attributelist.hpp"
#include <set>
#include "fabaoconfig.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

class Role;
class RoleModuleManager;

class FaBao
{
public:
	static const int FA_BAO_ATTR_CAL_RATE = 1000;

	FaBao();
	~FaBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager* GetRoleModuleManager() { return m_role_module_mgr; }
	void Init(RoleModuleManager * role_module_manager, const FaBaoParam & param, const FaBaoDrawParam & draw_param);
	void GetOtherInitParam(FaBaoParam *param, FaBaoDrawParam * draw_param);
	
	const AttributeList & ReCalcAttr(AttributeList & base_add, bool is_recalc);
	const AttributeList & ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list);

	void OnRoleLogin();
	void OnWeekChange();
	void GMsetTranf(int tranf);

	void ActiveFaBao(short fabao_index, bool is_from_draw = false);								// �����
	bool UpLevelFaBao(short fabao_index, bool is_notie = true);			// ��������
	int AdvancedFaBao(short fabao_index);								// ���׷���

	bool ActiveFaBaoByItem(int fabao_index);
	int GetFaBaoNumByColorAndLevel(int color, int level);			//���ָ��Ʒ�ʲ����ڵȼ��ķ�������

	void FabaoGuideFight();

	void GetFaBaoList(short *fabao_num, BattleFabao out_data_fabao_info[BATTLE_FABAO_NUM_MAX]);//��ȡ��ǰ����������Ϣ,���ؿ��õ�������������
	int GetFaBaoMaxLevel();//��ȡ��ǰ����������Ϣ,���ؿ��õ�������������

	void SendFaBaoAllInfo();
	void SendSignalFaBaoInfo(int fabao_index,int reason_type);
	void SendEquipBarInfo();

	bool IsActiveFaBao(short fabao_index);
	const FaBaoParam & GetFaBaoParam();
	const FaBaoEquip * GetFaBaoEquip(int fabao_index);
	void ReCalcAttrByRoleLevelUp();

	void GmActiveAllFabao();

	//�����齱
	void OnLunHuiWangChuangChange(int seq);			//�ֻ����������仯
	int FaBaoSetDrawMaterial(int index, int(&item_id)[FA_BAO_DRAW_MATERIAL_MAX_NUM]);
	void FaBaoDrawAccelerate(int index, int times);
	void FaBaoDraw(int index);
	void SendFaBaoDrawInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	FaBaoParam m_param;
	AttributeList m_attrs_add;
	AttributeList m_attrs_add_per;

	FaBaoDrawParam m_draw_param;
};


#endif // !__FABAO_HPP__
