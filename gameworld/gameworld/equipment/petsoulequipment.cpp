#include <assert.h>
#include "petsoulequipment.h"
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
#include "other/workshop/petsoulequipmentconfig.hpp"
#include "gamelog.h"

PetSoulEquipment::PetSoulEquipment(short equip_type) : ItemBase(I_TYPE_PET_SOUL_EQUIP), m_equipment_type(equip_type),m_equipment_class(0)
{
	
}

int PetSoulEquipment::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void PetSoulEquipment::RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const
{
	if (NULL == param_data) return;

	PetSoulEquipMentParam & data = param_data->pet_soul_equipment_param;
	if (0 == data.has_random_net_value)
	{
		data.has_random_net_value = 1;
		data.pet_equipment_id = this->GetItemId();
		data.level = 1;
		const AttributeUplevelCfg * level_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipUpLevelCfg(m_color, data.level);
		if (level_cfg != NULL)
		{
			data.m_max_exp = level_cfg->exp;
		}
		//获取魂器基础属性
		const PetSoulEquipBaseCfg * base_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipBaseCfg(pet_weapon_seq);
		if (NULL != base_cfg)
		{
			//随机属性条数
			const AttributeNumGroup * attr_num_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipAttrNumCfg(base_cfg->attribute_num);
			if (NULL != attr_num_cfg)
			{
				int attr_real_num = 0;
				if ((attr_num_cfg->num_max - attr_num_cfg->num_min + 1) > 0)
				{
					attr_real_num = (rand() % (attr_num_cfg->num_max - attr_num_cfg->num_min + 1)) + attr_num_cfg->num_min;
				}
				else
				{
					attr_real_num = attr_num_cfg->num_min;
				}
				
				for (int attr_idx = 0; attr_idx < attr_real_num && attr_idx < MAX_PET_SOUL_EQUIPMENT_ATTR_NUM; attr_idx++)
				{
					const AttributeTypeValueDetailGroup * attr_value_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->RandAttrTypeValueCfg(base_cfg->attribute_group, attr_idx + 1);
					if (NULL != attr_value_cfg)
					{
						int rand_attr_value = (attr_value_cfg->att_max - attr_value_cfg->att_min) + 1;
						if (rand_attr_value > 0)
						{
							int value_attr = (rand() % rand_attr_value) + attr_value_cfg->att_min;
							data.add_attr_list[attr_idx].attr_type = attr_value_cfg->att_type;
							data.add_attr_list[attr_idx].attr_value = value_attr;
						}
						else
						{
							data.add_attr_list[attr_idx].attr_type = attr_value_cfg->att_type;
							data.add_attr_list[attr_idx].attr_value = attr_value_cfg->att_min;
						}
						data.attr_num++;
					}
				}
			}

			//随机特效
			const PetSoulEquipSpidCfg * pet_soul_equip_spid_cfg = LOGIC_CONFIG->GetPetSoulEquipMentConfig()->GetPetSoulEquipSpidCfg(base_cfg->pet_weapon_skill);
			if (NULL != pet_soul_equip_spid_cfg)
			{
				data.sp_id = pet_soul_equip_spid_cfg->sp_id;
			}
		}
	}
}