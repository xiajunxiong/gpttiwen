#include "expcrystal.hpp"
#include "obj/character/role.h"
#include "item/itemgriddata.h"
#include "other/rolemodulemanager.hpp"
#include "equipment/equipmentmanager.h"

ExpCrystal::ExpCrystal() : ItemBase(I_TYPE_EXP_CRYSTAL)
{

}

ExpCrystal::~ExpCrystal()
{

}

int ExpCrystal::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (NULL == item_data.param) return -1;

	// 开启则关闭， 关闭则开启
	item_data.param->exp_crystal_param.is_turn_on ^= 1;

	num = 0;
	return 0;
}

