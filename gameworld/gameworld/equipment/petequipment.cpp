#include <assert.h>
#include "petequipment.h"
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
#include "other/workshop/petequipmentconfig.hpp"
#include "gamelog.h"

PetEquipment::PetEquipment(short equip_type) : ItemBase(I_TYPE_PET_EQUIPMENT), m_equipment_type(equip_type),m_equipment_class(0),
		m_equip_level(0), m_break_item(0),m_break_num(0), m_number_id(0), m_skill_group(0), m_skill_group_2(0)
{
	
}

int PetEquipment::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	/*
	if (NULL == role || NULL == item_data.param) return -1;

	num = 1;  // 装备的消耗数量肯定为1

	ItemDataWrapper item_wrapper(m_item_id, 1, 1);
	item_wrapper.invalid_time = item_data.invalid_time;
	item_data.param->GetData(item_wrapper.param_data);
	item_wrapper.has_param = 1;

	const ItemBase *item = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (NULL == item || I_TYPE_EQUIPMENT != item->GetItemType()) return -1;

	//TODO return role->GetEquipmentManager()->PutOn(item_wrapper, equip_index);
	*/

	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void PetEquipment::RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const
{
	if (NULL == param_data) return;

	PetEquipMentParam & data = param_data->pet_equipment_param;
	if (1 != data.has_random_net_value)
	{
		data.has_random_net_value = 1;
		data.pet_equipment_id = this->GetItemId();
		//赋予技能
		const PetEquipMentNumber * number_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetPetEquipMentRankSkillNumber(m_number_id);
		if (number_cfg != NULL)
		{
			if (number_cfg->number > 0)
			{

				if(number_cfg->number == 2)
				{
					int real_skill_num = 0;
					//第一个技能
					const  PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(m_skill_group);
					if (skill_cfg != NULL)
					{
						
						std::vector<PetEquipMentSkill> new_rand_vec;
						new_rand_vec.assign(skill_cfg->rand_vec.begin(), skill_cfg->rand_vec.end());
						if (skill_cfg->weight_count > 0)
						{
							int new_rand_num = skill_cfg->weight_count;
							int rand_num = rand() % new_rand_num;
							for (std::vector<PetEquipMentSkill>::iterator itor = new_rand_vec.begin(); itor != new_rand_vec.end(); itor++)
							{
								if (itor->weight > rand_num)
								{
									data.equipment_skill_list[real_skill_num] = itor->gift_id;
									new_rand_num -= itor->weight;
									new_rand_vec.erase(itor);
									real_skill_num++;
									break;
								}
								else
								{
									rand_num -= itor->weight;
								}
							}
						}
					}
					else
					{
						gamelog::g_log_serious_error.printf(LL_INFO, "PetEquipment::RandomNetValueOnPutItem err [PetEquipMentSkill] m_skill_group[%d]", m_skill_group);
					}

					//第二个技能
					const  PetEquipMentSkillGroup * skill_cfg2 = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(m_skill_group_2);
					if (skill_cfg2 != NULL)
					{
						std::vector<PetEquipMentSkill> new_rand_vec;
						new_rand_vec.assign(skill_cfg2->rand_vec.begin(), skill_cfg2->rand_vec.end());
						if (skill_cfg2->weight_count > 0)
						{
							int new_rand_num = skill_cfg2->weight_count;
							int rand_num = rand() % new_rand_num;
							for (std::vector<PetEquipMentSkill>::iterator itor = new_rand_vec.begin(); itor != new_rand_vec.end(); itor++)
							{
								if (itor->weight >= rand_num)
								{
									data.equipment_skill_list[real_skill_num] = itor->gift_id;
									new_rand_num -= itor->weight;
									new_rand_vec.erase(itor);
									real_skill_num++;
									break;
								}
								else
								{
									rand_num -= itor->weight;
								}
							}
						}
					}
					else
					{
						gamelog::g_log_serious_error.printf(LL_INFO, "PetEquipment::RandomNetValueOnPutItem err [PetEquipMentSkill] m_skill_group[%d]", m_skill_group);
					}
					data.equipment_skill_num = real_skill_num;
				}
				else
				{
					const  PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(m_skill_group);
					if (skill_cfg != NULL)
					{
						int real_skill_num = 0;
						std::vector<PetEquipMentSkill> new_rand_vec;
						new_rand_vec.assign(skill_cfg->rand_vec.begin(), skill_cfg->rand_vec.end());
						if (skill_cfg->weight_count > 0)
						{
							int new_rand_num = skill_cfg->weight_count;
							for (int i = 0; i < number_cfg->number && i < MAX_PET_EQUIPMENT_SKILL_NUM; i++)
							{
								int rand_num = rand() % new_rand_num;
								for (std::vector<PetEquipMentSkill>::iterator itor = new_rand_vec.begin(); itor != new_rand_vec.end(); itor++)
								{
									if (itor->weight >= rand_num)
									{
										data.equipment_skill_list[i] = itor->gift_id;
										new_rand_num -= itor->weight;
										new_rand_vec.erase(itor);
										real_skill_num++;
										break;
									}
									else
									{
										rand_num -= itor->weight;
									}
								}
							}
						}
						data.equipment_skill_num = real_skill_num;
					}
					else
					{
						gamelog::g_log_serious_error.printf(LL_INFO, "PetEquipment::RandomNetValueOnPutItem err [PetEquipMentSkill] m_skill_group[%d]", m_skill_group);
					}
				}
			}
			else
			{
				gamelog::g_log_serious_error.printf(LL_INFO, "PetEquipment::RandomNetValueOnPutItem err [PetEquipMentNumber] m_number_id[%d] number_cfg->number[%d]",m_number_id, number_cfg->number);
			}
			
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "PetEquipment::RandomNetValueOnPutItem err [no PetEquipMentNumber] m_number_id[%d]", m_number_id);
		}
	}

	//记录宠物装备的详细净值
	if (role != NULL)
	{
		gamelog::g_log_rand_item_param.printf(LL_INFO, "[%s] user[%d,%s] itembase[itemid:%d,%s] put_reason[%d] m_number_id[%d] m_skill_group[%d] m_skill_group_2[%d] m_ param_log[%s]", __FUNCTION__,
			role->GetUID(), role->GetName(), this->GetItemId(),this->GetItemName(), put_reason, m_number_id, m_skill_group, m_skill_group_2, param_data->GetParamLog(I_TYPE_PET_EQUIPMENT));
	}
}