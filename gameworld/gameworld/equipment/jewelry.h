#ifndef __JEWELRY_H__
#define __JEWELRY_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"

// ------------------------------------------------------------------------------------------------------------

class Jewelry : public ItemBase
{
public:
	enum JEWELRY_TYPE		// 灵饰类型 / 装备位置
	{
		J_TYPE_GEM_RING = 0,		// 戒指
		J_TYPE_BRACELET = 1,		// 手镯
		J_TYPE_PENDANTS = 2,		// 坠饰

		J_TYPE_MAX
	};
	UNSTD_STATIC_CHECK(J_TYPE_MAX <= JEWELRY_MAX_INDEX);

	enum SPECIAL_ATTR		// 灵饰 特殊属性枚举 - 有些灵饰所加的属性不是纯粹的战斗属性
	{
		S_ATTR_MIN = 100,						// 特殊属性从 100 号开始

		S_ATTR_ALL_RESISTANCE = S_ATTR_MIN,		// 所有特殊状态抗性提升

		S_ATTR_MAX
	};

	Jewelry(short jwewlry_type);
	virtual ~Jewelry() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;
	bool IsUnLimitLevel()const { return m_is_unlimit_level; }
	bool RandomNetValueParam(JewelryParam * out_param) const;
	bool RandomNetValueParamReforge(JewelryParam * out_param, int ly_id_1, int ly_id_2, int anima_1, int anima_2) const;
	int CheckEquipmentIsTreasure(const JewelryParam & p) const;		// 检查这样属性的装备是不是珍品
	bool CheckJewelryAttrValid(int type, Attribute value) const;
	bool CheckJewelryAttrMax(JewelryParam * param) const;				// 检查是否为满属性装备

	//精工检查
	bool CanStrongUltimate(const JewelryParam & p);
	bool GetNormalUltimateMaxValue(int ultimate_type, int *min_value, int * max_value);//获取精工上限
	bool GetUltimateMaxValue(int ultimate_type, int * max_value, int *min_value = NULL);//获取精工最大值
	bool GetNormalMinAndMaxUltimateValue(int * min_value, int *max_value);
	bool GetStrongMinAndMaxUltimateValue(int * min_value, int *max_value);

	short GetJewelryType() const { return m_jewelry_type; }
	short GetAttrAddType() const { return m_attr_add_type; }
	short GetAttrAddType2() const { return m_attr_add_type2; }
	short GetJewelryLevel() const { return m_equipment_level; }

	short GetEntryNum()const { return m_entry_num; }
	short GetEntryGroup()const { return m_entry_group; }
	int GetEquipColor()const { return m_color; }
	int GetJewelryOrnamentType()const { return m_ornament_type; }
	int GetSpecialEquip()const { return m_special_equip; }
	int GetSuitRandomGroup() const { return m_suit_random_group; }
protected:
	
	const short m_jewelry_type;				// 灵饰类型 / 装备位置

	short m_equipment_level;				// 装备阶级
	short m_ornament_type;					// 灵饰类型
	bool m_is_unlimit_level;
	short m_attr_add_type;					// 属性附加类型
	AttrSegment m_default_attr_range;		// 默认属性范围

	short m_attr_add_type2;					// 属性附加类型
	AttrSegment m_default_attr_range2;		// 默认属性范围
	int m_entry_num;
	int m_entry_group;

	int m_attr_type[EQUIP_JEWELRY_ATTR_NUM];
	int m_min_value[EQUIP_JEWELRY_ATTR_NUM];
	int m_max_value[EQUIP_JEWELRY_ATTR_NUM];

	int m_special_equip;

	int m_min_jg_common;
	int m_max_jg_common;
	int	m_min_jg_added;
	int m_max_jg_added;

	int m_suit_random_group;
	int star_group;
};



#endif

