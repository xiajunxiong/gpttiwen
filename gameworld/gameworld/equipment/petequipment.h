#ifndef __PET_EQUIPMENT_H__
#define __PET_EQUIPMENT_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"

// ------------------------------------------------------------------------------------------------------------

class PetEquipment : public ItemBase
{
public:
	enum PET_EQUIPMENT_TYPE		// 宠物装备类型
	{
		PET_EQUIPMENT_TYPE_0 = 0,		// 宠物装备1

		PET_EQUIPMENT_TYPE_MAX
	};
	PetEquipment(short equip_type);
	virtual ~PetEquipment() {}

	short GetEquipType() const { return m_equipment_type; }
	short GetEquipClass() const { return m_equipment_class; }
	short GetEquipLevel() const { return m_equip_level; }
	short GetSkillGroup() const { return m_skill_group; }
	short GetSkillGroup2() const { return m_skill_group_2; }
	int GetBreakItem()const { return m_break_item; }
	int GetBreakNum()const { return m_break_num; }

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	int GetEquipColor()const { return m_color; }
protected:
	const short	m_equipment_type;									// 装备类型EQUIPMENT_TYPE 武器 衣服等
	short m_equipment_class;										// 装备分类EQUIPMENT_CLASS 法杖 弓箭 布甲等
	short m_equip_level;

	int m_break_item;
	int m_break_num;

	short m_number_id;
	short m_skill_group;
	short m_skill_group_2;
};

#endif