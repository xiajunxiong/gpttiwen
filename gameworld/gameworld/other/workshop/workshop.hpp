#ifndef __WORK_SHOP_HPP__
#define __WORK_SHOP_HPP__

#include "servercommon/workshopdef.h"
#include "servercommon/configcommon.h"
#include "gameworld/other/workshop/godequipupconfig.hpp"

class RoleModuleManager;
class Workshop				// ����ϵͳ
{
public:
	Workshop();
	~Workshop();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager, const WorkShopParam & p_workshop_param);
	void GetWorkshopParam(WorkShopParam * p_workshop_param);

	//��������
	void NormalUltimate(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);//��ͨ����
	void StrongUltimate(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);//ǿ������

	//**************************************ʵ����Ǩ��

	//װ������
	void EquipUpStar(int seq, int equip_type, int equip_posi, int equip_grid, int cost_grid, int cost_grid_2);

	//������ҫ����װ����
	void UpEquipGrade(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);
	void BreakThroughGarde(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);

	void EquipGemMounting(int stone_type, int equip_posi, int equip_grid, int stone_grid);	// ��ʯ��Ƕ
	void EquipGemDismount(int stone_type, int equip_posi, int equip_grid, int stone_slot);	// ��ʯ��ж
	void EquipGemUpgrades(int stone_type, int equip_posi, int equip_grid, int stone_slot);	// ��ʯ����
	void ConfirmGemUpgrade();												// ȷ��������ʯ

	//װ���㻯/����/��ħ/�ֽ�
	void DianHuaEquip(int equip_posi, int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid);
	void SaveDianHuaEquip(int equip_posi, int equip_grid);
	void XiFuEquip(int equip_posi, int equip_grid);
	void FuMoEquip(int equip_posi, int equip_grid, int *new_spid = NULL, int *new_level = NULL);
	void ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2);

	//-----------------------------�ȴ�Ǩ��
	//����װ��������� _1�ǽ��ܴ���װ�� _2�Ǳ�ժ������װ��
	void ChangeEquipVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type = ROLE_TYPE_BENZUN);
	void ChangeJewelryVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type = ROLE_TYPE_BENZUN);
	void VocUltimate(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type);

	void OnActiveEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type);//����װ����������
	void OnRefreshEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int lock_voc_flag, int role_type, bool use_gold = false);//����װ����������
	void OnSaveEquipVocAttr(int item_type, int equip_posi, int equip_grid, int role_type);//��������

	void RefreshPetEquipMent(int compound_seq, int bag_index1);								// ϴ������װ��
	void SaveRefreshPetEquipment(int bag_index1);											// �������װ��ϴ����Ϣ

	void RefreshPetEquipMentInPet(int compound_seq, int pet_index);							// ϴ������װ��(�������ϵ�)
	void SaveRefreshPetEquipmentInPet(int pet_index);										// �������װ��ϴ����Ϣ(�������ϵ�)
	//**************************************ʵ����Ǩ��

	void DisComposeDianHuaShi(int dianhuashi_coloumn, int dianhuashi_grid);//�ֽ�㻯ʯ

	void CheckTurnBack();

	void GMRandEnd(int equip_posi, int equip_grid, int level);

	//�����������װ/��ҫ��װ��ְҵת��������װ��
	bool ChnageGodEquip(ItemGridData * change_item);

	//�ֽ�ϳɳ�����װ��
	void DeComposeItem(int grid_num, int *grid_idx_list);

	void CompoundItem(int compound_seq, int compound_num, int is_craft, bool is_huashen_buy = false);		// ������Ʒ
	bool DiscompoundItem(int compound_seq, int compound_num,int backpace_index);		// �ֽ���Ʒ
	bool ActiveNewRecipe(int active_id);												// �����䷽
	bool AutoActiveNewRecipe(int active_id, int item_id, bool is_auto_use = true, bool is_notice = true);		// �Զ������䷽
	void BatchCompoundItem(int compound_seq, int compound_num);							// �����ϳ�װ��
	bool RecipeIsActive(int active_id);

	void ExchangePetSkillBook(int exc_seq, std::vector<ItemPairCon> & grid_list);	// �һ����＼����
	void ExchangeTarget(int exc_seq/*���*/, int exc_times/*�ϳɴ���*/);					// �һ������ӡ�� �ϳ�Կ�ס�ϴ��¶
	
	//void SynthesisTarget(int _exc_seq,ItemID _target_id, ItemID _need_id,int _synthesis_type);						//�ϳ���Ʒ

	void CraftJewelry(int list_id);											// ���δ���
	void ReforgeJewelry(int mate_grid_1, int mate_grid_2);					// �����ض�
	void ChooseReforgeRetain(int list_idx);									// ѡ���ضͽ��

	void CompoundPetEquipMent(int compound_seq, int bag_index1, int bag_index2);				// ����װ���ϳ�
	void BreakPetEquipMent(int bag_index);

	void SendAllWorkshopInfo();												// ����������Ϣ
	void OnRoleLogin();		// ��ҵ�½ʱ�����ض���Ϣ��Ϊ�գ������ض���Ϣ

	void GmUnlockAllRecipe(bool is_unlock);			// ���������䷽
	void GmSetGodEquipChange() { m_workshop_param.god_equip_flag = 0; }
	void GmSetGodEquipChangeLevelFlag() { m_workshop_param.god_equip_change_flag = 0; }
	void GmSetGodEquipFixFlag() { m_workshop_param.check_god_equip_base_voc_flag = 0; }
	void GodEquipReback(); //��װ����
	bool CheckGodEquipItemDataReturn(const ItemGridData * item_data);
	bool CheckGodEquipItemDataChangeLevel(const ItemGridData * item_data);
	bool CheckGodEquipItemDataReturnBreak(ItemGridData * item_data, const GodEquipTurnBackNode * cfg);
	void GodEquipChange();

	//��װУ��
	void FixGodEquipBaseVoc();
private:
	RoleModuleManager * m_role_module_mgr;
	WorkShopParam m_workshop_param;

	void SendRecipeActiveFlag(int active_id, bool is_notice = true);		// ���͵����䷽����
	void SendJewelryTimesInfo();					// �·����κϳ�ʧ�ܴ���
	void SendJewelryReforgeOutcome();
	void SendComposeResult(bool is_succ, const ItemDataWrapper & get_item);


};

#endif