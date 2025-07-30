#ifndef __ZAO_HUA_ITEM_HPP__
#define __ZAO_HUA_ITEM_HPP__

#include "item/itembase.h"

class ZaoHuaItem : public ItemBase
{
public:
	enum ZAO_HUA_TYPE												// 装备类型
	{
		Z_TYPE_MIN = 0,

		Z_TYPE_1 = Z_TYPE_MIN,									// 
		Z_TYPE_2,												// 
		Z_TYPE_3,												// 
		Z_TYPE_4,												// 
		Z_TYPE_5,												// 
		Z_TYPE_6,												// 
		Z_TYPE_7,												// 
		Z_TYPE_8,												// 

		Z_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(Z_TYPE_MAX == ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM);
	ZaoHuaItem();
	const static int MAX_ZAO_HUA_BASE_VOC_NUM = 2;//造化装备基础词条
	virtual ~ZaoHuaItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;
	int GetEquipType() const { return m_equip_type; }
	int GetEquipLevel()const { return m_equip_level; }
	int GetSuitType() const { return m_suit_id; }
	void GetAttrList(AttrInfo attr_list[MAX_ZAO_HUA_BASE_VOC_NUM]) const;

	void GetReturn(int *ret_item, int *ret_num) const;
private:

	int m_equip_type;
	int m_equip_level;
	int m_suit_id;
	AttrInfo m_attr_list[MAX_ZAO_HUA_BASE_VOC_NUM];

	int return_item;
	int return_num;
};

#endif 

