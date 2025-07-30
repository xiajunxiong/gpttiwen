#include "elementcrystal.hpp"
#include "obj/character/role.h"
#include "item/itemgriddata.h"
#include "other/rolemodulemanager.hpp"
#include "equipment/equipmentmanager.h"

ElementCrystal::ElementCrystal() : ItemBase(I_TYPE_ELEMENT_CRYSTAL), m_element_fire(0), m_element_water(0), m_element_wind(0), m_element_earth(0), m_limit_num(0), m_element_type(ELEMENT_TYPE_INVALID)
{

}

ElementCrystal::~ElementCrystal()
{

}

int ElementCrystal::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (!role->GetRoleModuleManager()->GetEquipmentManager()->PutOnElementCrystal(item_data))
	{
		return -1;
	}

	return 0;
}

void ElementCrystal::RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const
{
	param_data->element_crystal_param.used_times = m_limit_num;
}

