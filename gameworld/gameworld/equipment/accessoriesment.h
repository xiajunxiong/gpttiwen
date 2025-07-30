#ifndef __ACCESSORIESMENT_H__
#define __ACCESSORIESMENT_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/peishi/peishiconfig.hpp"

// ------------------------------------------------------------------------------------------------------------

class Accessoriesment : public ItemBase
{
public:
	enum ACCESSORIES_TYPE												// 装备类型
	{
		A_TYPE_MIN = 0,

		A_TYPE_1 = A_TYPE_MIN,									// 护镜
		A_TYPE_2,												// 花钿
		A_TYPE_3,												// 锦带
		A_TYPE_4,												// 香囊
		A_TYPE_5,												// 袖箭

		A_TYPE_MAX,
	};

	const static int ACCESSORIESMENT_TYPE_COUNT = A_TYPE_MAX - A_TYPE_MIN;// 装备类型数量

	enum ACCESSORIES_INDEX											// 装备部位
	{
		A_INDEX_MIN = 0,
		A_INDEX_1 = A_INDEX_MIN,								// 护镜 0
		A_INDEX_2,												// 花钿 1
		A_INDEX_3,												// 锦带 2
		A_INDEX_4,												// 香囊 3
		A_INDEX_5,												// 袖箭 4

		A_INDEX_MAX,
	};
	UNSTD_STATIC_CHECK(A_INDEX_MAX == ACCESSORIESMENT_TYPE_COUNT);

	Accessoriesment(short equip_type);
	virtual ~Accessoriesment() {}

	short GetEquipType() const { return m_equipment_type; }
	short GetEquipClass() const { return m_equipment_class; }
	short GetAccessories() const { return m_accessories_index; }
	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	void GetBaseAttr(AttrInfo base_attr_list[MAX_ACCESSORIES_BASE_VOC_NUM])const;

	short GetSuitType() const { return m_suit_type; }
	int GetFenJieTianMing() const { return fenjie_tianming; };
	short GetPeiShiType()const { return m_equip_type; }
	short GetMaxEntryNum()const { return m_entry_num; }

	int GetFlyFlag()const { return m_fly_flag; }
protected:
	const short	m_equipment_type;
	short m_equipment_class;
	short m_accessories_index;
	short m_equip_type;
	short m_suit_type;
	short m_entry_num;

	AttrInfo base_attr_list[MAX_ACCESSORIES_BASE_VOC_NUM];

	int fenjie_tianming;
	int m_fly_flag;
};

#endif