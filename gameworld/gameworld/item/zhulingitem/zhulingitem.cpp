#include "zhulingitem.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/medal/medalconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"


ZhuLingItem::ZhuLingItem() : ItemBase(I_TYPE_ZHU_LING)
{

}

int ZhuLingItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}
