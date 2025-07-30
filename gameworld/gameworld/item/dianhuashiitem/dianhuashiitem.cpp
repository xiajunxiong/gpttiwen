#include "dianhuashiitem.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/medal/medalconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"


DianHuaShiItem::DianHuaShiItem(short equip_type) : ItemBase(I_TYPE_DIAN_HUA_SHI)
{
	effect_type = 0;
	effect_level = 0;
}

int DianHuaShiItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	return 0;
}

void DianHuaShiItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data)
	{
		return;
	}
	
	DianHuaShiParam & data = param_data->dian_hua_shi_param;
	if (0 != data.has_net_value)
	{
		return;
	}
	data.has_net_value = 1;
	data.effect_id = effect_type;
	data.effect_level = effect_level;
}
