#include "obj/character/role.h"
#include "servercommon/errornum.h"

#include "itemconvertbyjobitem.hpp"

ItemConvertByJobItem::ItemConvertByJobItem() : ItemBase(I_TYPE_CONVERT_ITEM)
{
}

int ItemConvertByJobItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}
