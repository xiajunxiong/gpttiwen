#include "holyitem.h"
#include "other/shengqi/holyequipmentconfig.hpp"

#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "servercommon/struct/battle/battle_def.hpp"

HolyItem::HolyItem()
	: ItemBase(I_TYPE_HOLY_EQUIP)
{
	m_shengqi_type = 0;
}

int HolyItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void HolyItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data) return;

	HolyEquipParam & data = param_data->holy_equip_param;
	if (0 != data.has_net_value) return;

	std::string error;
	if (!LOGIC_CONFIG->GetHolyEquipMentConfig()->RandHolyItemAttr(data, this->GetItemId(), this->GetColor(), m_shengqi_type, error))
	{
		if (NULL != role)
		{
			gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] item_id[%d] error_info:%s", __FUNCTION__, role->GetUID(), role->GetName(), this->GetItemId(), error.c_str());
		}
	}	
}

void HolyEquipParam::GetBattleHallowAttr(BattleHallowInfo& info) const
{
	info.attr_active_max = GetMax(active_max, 1u);

	for (int i = 0; i < ARRAY_ITEM_COUNT(total_attr_list); ++i)
	{
		if (HolyItem::HOLY_ATTR_TYPE_ACTIVENESS == total_attr_list[i].attr_type)
		{
			info.attr_active = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK == total_attr_list[i].attr_type)
		{
			info.attr_damage = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_SPEED == total_attr_list[i].attr_type)
		{
			info.attr_speed = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_RECOVER == total_attr_list[i].attr_type)
		{
			info.attr_recover = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_HELP == total_attr_list[i].attr_type)
		{
			info.attr_intervene = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_FALL == total_attr_list[i].attr_type)
		{
			info.attr_knock_off = total_attr_list[i].attr_value;
		}
		else if (HolyItem::HOLY_ATTR_TYPE_COPE_FALL == total_attr_list[i].attr_type)
		{
			info.attr_anti_knock_off = total_attr_list[i].attr_value;
		}
	}
}


