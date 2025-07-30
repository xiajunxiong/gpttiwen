#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/zaohuaequip/zaohuaequipconfig.hpp"

#include "yuanhunzhuitem.hpp"

YuanHunZhuItem::YuanHunZhuItem() : ItemBase(I_TYPE_YUAN_HUN_ZHU)
{

}

int YuanHunZhuItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void YuanHunZhuItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	YuanHunZhuParam & data = param_data->yuan_hun_zhu_param;
	data.yuan_hun_zhu_item_id = this->GetItemId();
	const YuanHunZhuCfg * cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetCfg(this->GetItemId());
	if (!cfg)
	{
		return;
	}

	for (int i = 0; i < YUAN_HUN_ZHU_MAX_ATTR_NUM; ++i)
	{

		const YuanHunZhuAttrGroupItemCfg * attr_cfg = LOGIC_CONFIG->GetZaoHuaEquipConfig()->YuanHunZhuGetRandAttrGroupItemCfg(cfg->slot_attr_rand_group_id[i]);
		if (attr_cfg)
		{
			data.attr_seq[i] = attr_cfg->seq;
			data.attrs[i] = attr_cfg->attr;
		}		
	}
}
