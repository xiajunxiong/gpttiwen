#ifndef __EQUIPMENT_MANAGER_H__
#define __EQUIPMENT_MANAGER_H__

#include "item/itemgriddata.h"
#include "equipment.h"
#include "jewelry.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/attributelist.hpp"
#include "gamedef.h"
#include "obj/character/character.h"
#include <set>

class RoleModuleManager;
class EquipmentManager
{
public:
	EquipmentManager();
	~EquipmentManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const EquipmentOtherParam * other_param,
		int param_list_num, const ItemListParam::ItemListData * param_data_list[],
		int jewelry_list_num, const ItemListParam::ItemListData * jewelry_data_list[]);
	void GetInitParam(EquipmentOtherParam* other_param);
	void OnLogin();

	void Update(time_t now_second);
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc, int role_type = ROLE_TYPE_BENZUN, AttributeList * equip_special_attr = NULL);
	void ReCalcAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);
	int GetEquipDianHuaShiCap();

	//��ȡ��ǰ��Ƕ���ڵ��ڸõȼ��ı�ʯ����
	int GetEquipGemNumByGrade(int grade);
	//��ȡ��ǰ������װ��(װ��+����)���ڵ��ڸ�Ʒ�ʵ�����
	int GetWearEquipNumByColor(int color);

	void GetCurWeapon(ItemID* out_weapon_id, short* out_weapon_type, ItemID* out_sub_weapon_id = NULL, short role_type = ROLE_TYPE_BENZUN);
	ItemID GetCurWeaponId();
	void GetWeaponAttr(short max_attr_num, short* out_attr_num, AttrInfo* out_attr_list);
	void GetEquipSpecialEffect(int max_weapon_sp_num, short* out_weapon_sp_num, int* out_weapon_sp_list,
		int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);

	int PutOn(short column, short index, short equip_index, short role_type = ROLE_TYPE_BENZUN);
	int TakeOff(short equip_index, bool is_from_puton = false, short role_type = ROLE_TYPE_BENZUN, int * take_off_column = NULL, int * take_off_index = NULL);

	void EquipGemMounting(int stone_type, int equip_grid, int stone_grid);	// ��ʯ��Ƕ
	void EquipGemDismount(int stone_type, int equip_grid, int stone_slot);	// ��ʯ��ж
	void EquipGemUpgrades(int stone_type, int equip_grid, int stone_slot);	// ��ʯ����
	void ConfirmGemUpgrade();												// ȷ��������ʯ

	void CalcGemOneKeyUpgradeNeed(std::map<ItemID, int> & owned_list);
	void SendGemOneKeyUpgradeNeed();

	void LockEquipMentVoc(short equip_type, short equip_index, short voc_idx, short lock);//����װ��������

	bool PutOnElementCrystal(const ItemGridData &grid_data);
	bool TakeOffElementCrystal(bool is_from_puton = false);
	inline ItemID GetElementCrystalItemId() const { return m_other_param.element_crystal_id; }
	inline int GetElementCrystalRemainTimes() const { return m_other_param.element_crystal_used_times; }
	int ClearMixedElementCrystal();				//�������������õĻ��Ԫ��ˮ�����Զ����գ�����Ҫ����ÿ�����Ԫ��ˮ��ת��Ϊ5000ͭ��ͨ���ʼ����� ;  ��Ҫ���ĵط��б���,�ֿ�,��ɫ�������ˮ��,�Ѽ������������ˮ��

	void SendEquipmentInfoToRole();
	void SendEquipmentInfoToRoleByIndex(short equip_index);
	void SendElementCrystalChangeNotice();

	void SetEquipGridDirty(short equip_index, bool to_notice_client = true, short role_type = ROLE_TYPE_BENZUN);

	bool GetEquipmentGridData(short equip_index, ItemGridData *equip_data, short role_type = ROLE_TYPE_BENZUN);
	ItemGridData * GetEquipmentGridData2(int equip_index, short role_type = ROLE_TYPE_BENZUN);
	bool GetEquipmentGridData(short equip_index, ItemDataWrapper &equip_data, short role_type = ROLE_TYPE_BENZUN);
	bool GetEquipmentGridVocLock(short equip_index, short voc_idx);

	void GetEquipmentList(int *count, QueryItemListParam::ItemListData *save_list);
	void GetChangeEquipmentList(int *count, ItemListParam::ItemListData *save_list);
	void ClearDirtyMark();

	//����װ��
	bool HuaShenPutOnElementCrystal(int index);						//���Ԫ��ˮ��
	bool HuaShenTakeOffElementCrystal(bool is_from_puton = false);	//����Ԫ��ˮ��
	bool HuaShenCrystalUpdate(ItemDataWrapper *item_data);				//���»���Ԫ������
	bool HuaShenEquipmentUpdate(int equip_index, ItemDataWrapper * item_data);	//���»���װ����Ϣ
	bool HuaShenEquipmentReset(int equip_index, bool is_notice);			//���ø�װ����
	bool HuaShenCrystalReset(bool is_notice);								//����Ԫ��ˮ��
	inline ItemID GetHuaShenElementCrystalItemId() const { return m_other_param.huashen_element_crystal_id; }
	inline int GetHuaShenElementCrystalRemainTimes() const { return m_other_param.huashen_element_crystal_used_times; }

	void GetHuaShenWeaponAttr(short max_attr_num, short* out_attr_num, AttrInfo* out_attr_list);
	void GetHuaShenEquipSpecialEffect(int max_weapon_sp_num, short* out_weapon_sp_num, int* out_weapon_sp_list,
		int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);

	void SendHuaShenEquipmentInfo();
	void SendHuaShenEquipmentInfoByIndex(short equip_index);
	void SendHuaShenElementCrystalChangeNotice();

	////////////////////////////////////////// ���� //////////////////////////////////////////
	bool GetJewelryGridData(short jewelry_type, ItemGridData * carrier_data, int role_type = ROLE_TYPE_BENZUN);
	ItemGridData * GetJewelryGridData2(int jewelry_type, int role_type = ROLE_TYPE_BENZUN);
	bool GetJewelryGridData(short jewelry_type, ItemDataWrapper & carrier_data, int role_type = ROLE_TYPE_BENZUN);
	bool GetJewelryGridVocLock(short jewelry_type, short voc_idx);

	void GetJewelryList(int * count, QueryItemListParam::ItemListData * save_list);
	void GetChangeJewelryList(int * count, ItemListParam::ItemListData * save_list);

	void SendAllJewelryInfo();
	void SendSingleJewelryInfo(short jewelry_type, bool is_wear = true);

	bool WearJewelry(short grid_index);
	bool TakeOffJewelry(short jewelry_type, bool need_recalc = true, int * take_off_column = NULL, int * take_off_index = NULL);
	ItemGridData * GetJewalryListType(int type, short role_type = ROLE_TYPE_BENZUN);
	ItemGridData * GetEquipListType(int type, short role_type = ROLE_TYPE_BENZUN);

	//װ������
	void EquipUpStar(int seq, int equip_type, int equip_grid, int cost_grid, int cost_grid_2);

	//������ҫ����װ����
	void UpEquipGrade(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);
	void BreakThroughGarde(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);

	//�㻯ʯ
	void DianHuaEquip(int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid);
	void SaveDianHuaEquip(int equip_grid);
	void XiFuEquip(int equip_grid);
	void FuMoEquip(int equip_grid, int *new_spid = NULL, int *new_level = NULL);

	void SetJewelryGridDirty(short jewelry_type, bool notify = true, short role_type = ROLE_TYPE_BENZUN, bool is_wear = true);
	void ClearJewelryDirty();

	//��������
	bool HuaShenWearJewelry(short grid_index);
	bool HuaShenTakeOffJewelry(short jewelry_type, bool need_recalc = true, int * take_off_index = NULL);
	bool HuaShenJewelryUpdate(short jewelry_type, ItemDataWrapper * item_data);	//���»���������Ϣ
	bool HuaShenJewelryReset(short jewelry_type, bool is_notice);				//���ø�װ����

	void SendHuaShenAllJewelryInfo();
	void SendHuaShenSingleJewelryInfo(short jewelry_type);

	// ��ȡ�ﵽָ����ɫ��װ������
	int GetReachingColor(int level, int color);

	void OnChangeProfession();
	void OnRoleStartFight(int battle_mode);
	void OnRoleFinishFight();
	bool GetElementCrystal(ItemGridData * item_data, int role_type = ROLE_TYPE_BENZUN);	//����ʱ���ж��Ƿ񴩴���ˮ��

	//����
	void ClearHuaShenEquipAndJewelry();	//������ʼ,����װ��,��������
	void ClearHuaShenEquip();			//��������,�����������
	void SynHuaShenEquipAndJewelry(bool is_check = false);	//������ʼ,ͬ�����𴩴���Ϣ��������,��ͬ����ʱ������Ԫ��ˮ��
	void HuaShenKeySyn(std::set<long long> * item_set);						//����������,�������ý�ս��һ��ͬ��
	void CheckItemIsHuaShenPut(int item_type, int index, ItemGridData * grid);	//��⻯���ʱ�Ƿ񴩴��Ÿõ���,�������ͬ��
	void SynItemDataPut(int item_type, int index, const ItemGridData * grid, int role_type = ROLE_TYPE_BENZUN);	//һ���������ͬ��

	void OpenResonaceLevel();

	void SendDianHuaShiCap();

	void SendEquipMentLockInfo();

	int GetEquipJewelryCap() { return m_other_param.equip_jewelry_cap; }

	//  װ��ǿ��ϵͳ

	void UpGreade(int idx);//ǿ����Ӧ��λ
	void SendUpGreadeInfo();//�·�ǿ����Ϣ
	void SendResonaceInfo();
	int GetMinEquipStr();	//��ȡ��ǰװ�����ǿ���ȼ�

	//������װ�� -- ��װ����
	void ReturnGodEquip();
	void ReturnGodEquip2();
	void GodEquipReBack();
	void GodEquipBackErrUplevel();
	void GodEquipChangeLevel();
	void FixGodEquipBaseVoc();

	void CheckNewSuitChange();//���¼��װ����װ
	void ChangeNewSuit();
	void GmSetNewVocFlag() { m_other_param.new_voc_extra_flag = 0; }
	void CHeckNewVocEatra();//���������������
	void ChangeNewVocEatra();
private:
	RoleModuleManager* m_module_mgr;
	AttributeList	m_attrs_add;
	AttributeList	m_attrs_add_per;
	bool need_confirm;
	GemUpgradeInfo m_gem_upgrade_param;
	AttributeList   m_huashen_attr_add;			//��������
	AttributeList   m_huashen_speical_logic_attrs;	//������������

	int dianhuashi_id;
	int dianhuashi_num;
	int all_dianhuashi_level;

	int low_god_equip_level;//��װ���ǿ���ȼ�
	bool has_wear_all_equip;//�Ƿ�ȫ�����Ķ�����װ

	//����װ��
	ItemGridData	m_equipment_list[Equipment::E_INDEX_MAX];

	bool			m_dirty_mark[Equipment::E_INDEX_MAX];
	bool			m_old_state[Equipment::E_INDEX_MAX];
	bool			m_equipment_is_change;

	EquipmentOtherParam m_other_param;

	ItemGridData	m_jewelry_list[Jewelry::J_TYPE_MAX];

	bool			m_j_dirty_mark[Jewelry::J_TYPE_MAX];
	bool			m_j_old_state[Jewelry::J_TYPE_MAX];
	bool			m_jewelry_is_change;

	//����װ��
	ItemGridData	m_huashen_equipment_list[Equipment::E_INDEX_MAX];

	bool			m_huashen_dirty_mark[Equipment::E_INDEX_MAX];
	bool			m_huashen_old_state[Equipment::E_INDEX_MAX];

	ItemGridData	m_huashen_jewelry_list[Jewelry::J_TYPE_MAX];

	bool			m_huashen_j_dirty_mark[Jewelry::J_TYPE_MAX];
	bool			m_huashen_j_old_state[Jewelry::J_TYPE_MAX];

};

#endif

