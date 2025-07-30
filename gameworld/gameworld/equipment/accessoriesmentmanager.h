#ifndef __ACCESSORIES_MENT_MANAGER_H__
#define __ACCESSORIES_MENT_MANAGER_H__

#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/attributelist.hpp"
#include "gamedef.h"
#include "obj/character/character.h"
#include <set>
#include "gameworld/equipment/accessoriesment.h"
#include "protocol/msgaccessories.h"

class RoleModuleManager;
class AccessoriesmentManager
{
public:
	AccessoriesmentManager();
	~AccessoriesmentManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, int param_list_num, const ItemListParam::ItemListData * accessories_data_list[], int level_param[ACCESSORIES_SPECIAL_EQUIP_LEVEL]);
	void GetInitParam(int level_param[ACCESSORIES_SPECIAL_EQUIP_LEVEL]);
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc, int role_type = ROLE_TYPE_BENZUN, AttributeList * equip_special_attr = NULL);

	int PutOn(short column, short index, short equip_index);
	int TakeOff(short equip_index, bool is_from_puton = false, int * take_off_column = NULL, int * take_off_index = NULL);

	void Strengthen(short is_wear, short index);//强化
	void ChangeAccessories(short is_put_on, short column, short index, int new_item_id);

	void Discompose(short column, short index, int num = 1);//分解
	void ComposeVod(bool is_wear, short index, short replace_index, Protocol::CSAccessoriesMentVocComposeReq::ComposeVoc change_voc_list[MAX_ACCESSORIES_VOC_NUM]);

	void SendAccessoriesMentInfo();
	void SendAccessoriesmentInfoToRoleByIndex(short equip_index);

	void UpLevelSpecialEquip(int seq);
	int GetAccessoriesSpecialLevel() { return accessories_special_equip_list[PeiShiSpecialEquipCfg::SPECIAL_TYPE_2 - 1]; }
	void SendSpecialEquiplist();

	void SetAccessoriesGridDirty(short equip_index, bool to_notice_client = true);

	bool GetAccessoriesmentGridData(short equip_index, ItemGridDataGetter *equip_data);

	void GetAccessoriesmentList(int *count, QueryItemListParam::ItemListData *save_list);
	void GetChangeAccessoriesmentList(int *count, ItemListParam::ItemListData *save_list);
	void ClearDirtyMark();

	void GetUserSpecialEffectList(int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);
private:
	bool StrengthenGiveBack(const ItemGridData * back_item);//配置销毁强化退还

	RoleModuleManager* m_module_mgr;
	AttributeList	m_attrs_add;
	int m_capability;
	//自身装备
	ItemGridData	m_accessories_list[Accessoriesment::A_INDEX_MAX];

	bool			m_dirty_mark[Accessoriesment::A_INDEX_MAX];
	bool			m_old_state[Accessoriesment::A_INDEX_MAX];
	bool			m_is_change;

	int accessories_special_equip_list[ACCESSORIES_SPECIAL_EQUIP_LEVEL];
};

#endif

