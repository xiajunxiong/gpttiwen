#include "runestoneitem.h"
#include "other/shengqi/holyequipmentconfig.hpp"

#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"

RuneStoneItem::RuneStoneItem()
	: ItemBase(I_TYPE_RUNE_STONE)
{
	m_runestone_type = 0;
	m_runestone_level = 0;
}

int RuneStoneItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void RuneStoneItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data || NULL == role) return;

	RuneStoneParam & data = param_data->rune_stone_param;
	if (0 != data.has_net_value)
	{
		return;
	}

	if (!LOGIC_CONFIG->GetHolyEquipMentConfig()->RandRuneStoneAttr(role, data, m_runestone_type, this->GetItemId()))
	{
		gamelog::g_sheng_qi_system.printf(LL_INFO, "%s role[%d, %s] can not find fushi_type[%d] or fushi_id[%d]! RandomNetValueOnPutItem Failed!!!", __FUNCTION__, role->GetUID(), role->GetName(), m_runestone_type, this->GetItemId());
	}
}
