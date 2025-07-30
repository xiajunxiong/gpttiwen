#ifndef __PET_SOUL_EQUIPMENT_H__
#define __PET_SOUL_EQUIPMENT_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"

// ------------------------------------------------------------------------------------------------------------

class PetSoulEquipment : public ItemBase
{
public:
	enum PET_SOUL_EQUIPMENT_TYPE		// ≥ËŒÔŒ‰∆˜¿‡–Õ
	{
		PET_SOUL_EQUIPMENT_TYPE_0 = 0,		// ªÍ∆˜

		PET_EQUIPMENT_TYPE_MAX
	};
	PetSoulEquipment(short equip_type);
	virtual ~PetSoulEquipment() {}

	short GetEquipType() const { return m_equipment_type; }
	short GetEquipClass() const { return m_equipment_class; }

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;

	int GetEquipColor()const { return m_color; }
	int GetPetWeaponSeq()const { return pet_weapon_seq; }
protected:
	const short	m_equipment_type;
	short m_equipment_class;

	int pet_weapon_seq;

};

#endif