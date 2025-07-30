#include <assert.h>
#include "godprintequipment.h"
#include "obj/character/role.h"
#include "globalconfig/globalconfig.h"
#include "item/itempool.h"
#include "other/workshop/petgodprintequipmentconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/other/pet/pet.hpp"
#include "gameworld/other/rolemodulemanager.hpp"

GodPrintEquipment::GodPrintEquipment(short equip_type) : ItemBase(I_TYPE_GOD_PRINT), m_param1(0), m_param2(0), m_param3(0)
{
	
}

int GodPrintEquipment::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	if (NULL == role->GetRoleModuleManager()->GetPet()->AddPetGodPrintByUse(&item_data))
	{
		return -1;
	}
	return 0;
}

void GodPrintEquipment::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	GodPrintItem data;
	{
		data.Reset();
		data.item_id = this->GetItemId();
		data.level = 1;
		data.color = (char)m_param3;
		data.m_posi = (short)m_param2 - 1;
		const PetGodPrintIntensifyInfoCfg *  exp_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetIntensifyInfoCfg(m_param3, data.level);
		if (exp_cfg != NULL)
		{
			data.exp = 0;
			data.m_max_exp = exp_cfg->exp;
		}

		const PetGodPrintEquipMentBaseQualityCfg *  level_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->GetBaseCfg(m_param2, (int)data.color);
		if (level_cfg != NULL)
		{
			// 随机主属性
			const PetGodPrintMainAttrBaseCfg * main_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandMainBaseAttrCfg(level_cfg->main_attribute_group);
			if (main_cfg != NULL)
			{
				if ((main_cfg->att_max - main_cfg->att_min + 1) != 0)
				{
					data.main_attr.attr_value = (rand() % (main_cfg->att_max - main_cfg->att_min + 1)) + main_cfg->att_min;
				}
				else
				{
					data.main_attr.attr_value = main_cfg->att_min;
				}
				data.main_attr.attr_type = main_cfg->att_type;
			}

			// 随机副属性

			const PetGodPrintEquipMentBaseAttrNumCfg * deputy_num_cfg = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrNum(level_cfg->deputy_attribute_num);
			if (deputy_num_cfg != NULL)
			{
				if (deputy_num_cfg->num > 0)
				{
					for (int attr_idx = 0; attr_idx < MAX_PET_GOD_PRONT_ATTR_NUM &&
						attr_idx < deputy_num_cfg->num; attr_idx++)
					{
						int attr_type[MAX_PET_GOD_PRONT_ATTR_NUM];
						memset(attr_type, -1, sizeof(attr_type));

						for (int i = 0; i < MAX_PET_GOD_PRONT_ATTR_NUM && i < data.attr_num; i++)
						{
							attr_type[i] = data.add_attr_list[i].attr_type;
						}

						const PetGodPrintdeputyAttrInfoCfg * attr_info = LOGIC_CONFIG->GetPetGodPrintEquipMentConfig()->RandDeputyAttrBaseInfo(level_cfg->deputy_attribute_att, attr_type);
						if (attr_info != NULL)
						{
							if ((attr_info->att_max - attr_info->att_min + 1) > 0)
							{
								data.add_attr_list[data.attr_num].attr_type = attr_info->att_type;
								data.add_attr_list[data.attr_num++].attr_value = (rand() % (attr_info->att_max - attr_info->att_min + 1)) + attr_info->att_min;
							}
							else
							{
								data.add_attr_list[data.attr_num].attr_type = attr_info->att_type;
								data.add_attr_list[data.attr_num++].attr_value = attr_info->att_min;
							}
						}
					}
				}
			}

			data.suit_id = m_param1;
		}
	}
	if (NULL != param_data)
	{
		param_data->god_print_param.god_print_data = data;
	}
}