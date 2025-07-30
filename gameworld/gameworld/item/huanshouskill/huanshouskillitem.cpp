#include "huanshouskillitem.hpp"

#include "obj/character/role.h"
#include "engineadapter.h"
#include "servercommon/errornum.h"

int HuanShouSkillItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}
