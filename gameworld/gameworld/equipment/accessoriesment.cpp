#include <assert.h>
#include "accessoriesment.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "gamelog.h"

Accessoriesment::Accessoriesment(short equip_type) : ItemBase(I_TYPE_ACCESSORIES), m_equipment_type(equip_type)
{
	m_equipment_class = 0;
	m_accessories_index = 0;
	m_equip_type = 0;
	m_suit_type = 0;
	m_entry_num = 0;
	fenjie_tianming = 0;
	m_fly_flag = 0;
	memset(base_attr_list, 0, sizeof(base_attr_list));
}

int Accessoriesment::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void Accessoriesment::RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const
{
	if (NULL == param_data) return;

	const PeishiEntryCfg * peishi_cfg = LOGIC_CONFIG->GetPeiShiConfig()->GetEntryCfg(m_suit_type, this->GetColor());
	if (NULL == peishi_cfg)return;
	
	const EntryRewardCfg * entry_reward_cfg = LOGIC_CONFIG->GetPeiShiConfig()->RandEntryRewardCfg(peishi_cfg->seq);
	if (NULL == entry_reward_cfg)return;
	
	param_data->accessoriess_param.voc_list[0].attr_type = entry_reward_cfg->att_type;
	int rand_value = 0;
	if (entry_reward_cfg->att_max - entry_reward_cfg->att_min + 1 != 0)
	{
		rand_value = rand() % (entry_reward_cfg->att_max - entry_reward_cfg->att_min + 1)  + entry_reward_cfg->att_min;
	}
	else
	{
		rand_value = entry_reward_cfg->att_max;
	}

	param_data->accessoriess_param.voc_list[0].attr_value = rand_value;
}

void Accessoriesment::GetBaseAttr(AttrInfo base_list[MAX_ACCESSORIES_BASE_VOC_NUM]) const
{
	for (int i = 0; i < MAX_ACCESSORIES_BASE_VOC_NUM; i++)
	{
		base_list[i] = base_attr_list[i];
	}
}
