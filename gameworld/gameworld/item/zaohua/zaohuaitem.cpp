#include "zaohuaitem.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/medal/medalconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "other/zaohuaequip/zaohuaequipconfig.hpp"
#include "config/logicconfigmanager.hpp"


ZaoHuaItem::ZaoHuaItem() : ItemBase(I_TYPE_ZAO_HUA_EQUIP)
{

}

int ZaoHuaItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void ZaoHuaItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	ZaoHuaEquipParam &param = param_data->zao_hua_param;
	if (param.has_net_value == 1)return;
	param.has_net_value = 1;

	param.ling_li_value = LOGIC_CONFIG->GetZaoHuaEquipConfig()->GetRandLingXingValue();
}

void ZaoHuaItem::GetAttrList(AttrInfo attr_list[MAX_ZAO_HUA_BASE_VOC_NUM]) const
{
	for (int i = 0; i < MAX_ZAO_HUA_BASE_VOC_NUM; i++)
	{
		attr_list[i] = m_attr_list[i];
	}
}

void ZaoHuaItem::GetReturn(int * ret_item, int * ret_num) const
{
	if (ret_item != NULL)*ret_item = return_item;
	if (ret_num != NULL)*ret_num = return_num;
}
