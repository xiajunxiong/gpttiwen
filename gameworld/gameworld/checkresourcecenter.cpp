#include "checkresourcecenter.hpp"
#include "globalconfig/globalconfig.h"
#include "item/itempool.h"
#include "item/other/otheritem.hpp"
#include "item/expense/norexitem.hpp"
#include "world.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "monster/monsterpool.hpp"
#include "other/title/titleconfig.hpp"
#include "task/taskpool.h"
#include "other/partner/partnerconfig.hpp"
#include "monster/monster_group_pool.hpp"
#include "other/train/trainconfig.hpp"
#include "skill/skillpool.hpp"
#include "other/peaktoken/peaktokenconfig.hpp"

CheckResourceCenter::CheckResourceCenter()
{

}

CheckResourceCenter::~CheckResourceCenter()
{

}

CheckResourceCenter & CheckResourceCenter::Instance()
{
	static CheckResourceCenter check_res;
	return check_res;
}

bool CheckResourceCenter::Check(std::string *err)
{
	bool ret = this->CheckHelper(err);
	this->Clear();

	return ret;
}

bool CheckResourceCenter::CheckHelper(std::string *err)
{
	if (!m_item_valid_check.Check(err)) return false;
	if (!m_scene_check.Check(err)) return false;
	if (!m_norex_check.Check(err)) return false;
	if (!m_special_effect_check.Check(err)) return false;
	if (!m_monster_check.Check(err)) return false;
	if (!m_title_check.Check(err)) return false;
	if (!m_task_check.Check(err)) return false;
	if (!m_partner_check.Check(err)) return false;
	if (!m_pet_check.Check(err)) return false;
	if (!m_monster_group_check.Check(err)) return false;
	if (!m_growth_check.Check(err)) return false;
	if (!m_skill_check.Check(err)) return false;
	if (!m_passive_skill_check.Check(err)) return false;
	if (!m_task_condition_item_check.Check(err)) return false;
	if (!m_partner_pact_check.Check(err)) return false;
	if (!m_peak_token_check.Check(err)) return false;
	if (!m_uta_cfg_checker.Check(*err)) return false;

	return true;
}

void CheckResourceCenter::Clear()
{
	m_item_valid_check.Clear();
	m_scene_check.Clear();
	m_norex_check.Clear();
	m_special_effect_check.Clear();
	m_monster_check.Clear();
	m_title_check.Clear();
	m_task_check.Clear();
	m_partner_check.Clear();
	m_pet_check.Clear();
	m_monster_group_check.Clear();
	m_growth_check.Clear();
	m_skill_check.Clear();
	m_passive_skill_check.Clear();
	m_task_condition_item_check.Clear();
	m_partner_pact_check.Clear();
	m_peak_token_check.Clear();
}

//////////////////////////////////////////////////////////////////////////
void CheckResourceCenter::ItemValidCheckStruct::Add(ItemID item_owner, ItemID item_id, int num, const char* source_info)
{
	item_vec.push_back(CheckItem(item_owner, item_id, num, source_info));
}

bool CheckResourceCenter::ItemValidCheckStruct::Check(std::string *err)
{
	char errinfo[1024] = {0};

	for (std::vector<CheckItem>::iterator it = item_vec.begin(); it != item_vec.end(); ++it)
	{
		ItemID item_id = (*it).item_id;
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			//printf("[ERROR] CheckResourceCenter::ItemValidCheckStruct::Check item_id[%d] num[%d] item_owner[%d] source_info[%s] error\n", item_id, (*it).num, (*it).item_owner, (*it).source_info.c_str());
			snprintf(errinfo, sizeof(errinfo), "[ItemIdInvalid] item_id = %d num = %d item_owner = %d source_info[%s] !", item_id, (*it).num, (*it).item_owner, (*it).info.c_str());
			*err = errinfo;
			return false;
		}

		if (!item->CanMerge() && (*it).num > 1)
		{
			snprintf(errinfo, sizeof(errinfo), "[ItemNumError] item_id = %d  num = %d item_owner = %d  source_info[%s] !", item_id, (*it).num, (*it).item_owner, (*it).info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::ItemValidCheckStruct::Clear()
{
	std::vector<CheckItem>().swap(item_vec);
}

//////////////////////////////////////////////////////////////////////////
void CheckResourceCenter::SceneCheckStruct::Add(int scene_id, const char* source_info)
{
	static SceneCheckItem sci;
	sci.scene_id = scene_id;
	sci.info = source_info;
	scene_vec.push_back(sci);
}

bool CheckResourceCenter::SceneCheckStruct::Check(std::string *err)
{
	for (std::vector<SceneCheckItem>::iterator it = scene_vec.begin(); it != scene_vec.end(); ++it)
	{
		SceneCheckItem sci = *it;
		if (!World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(sci.scene_id))
		{
			char errinfo[1024] = {0};

			sprintf(errinfo, "[SceneCheckStruct::Check] scene_id = %d not exist, from[%s] !", sci.scene_id, sci.info.c_str());
			*err = errinfo;
			printf("CheckResourceCenter::SceneCheckStruct::Check scene_id[%d] from[%s] not exist, please check config\n", sci.scene_id, sci.info.c_str());
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::SceneCheckStruct::Clear()
{
	std::vector<SceneCheckItem>().swap(scene_vec);
}

void CheckResourceCenter::NorexItemCheckStruct::Add(ItemID item_id)
{
	item_vec.push_back(item_id);
}

bool CheckResourceCenter::NorexItemCheckStruct::Check(std::string *err)
{
	char errinfo[1024] = {0};

	for (std::vector<ItemID>::iterator it = item_vec.begin(); it != item_vec.end(); ++it)
	{
		ItemID item_id = *it;

		const ItemBase *itembase = ITEMPOOL->GetItem(item_id);
		if (NULL == itembase || I_TYPE_EXPENSE != itembase->GetItemType())
		{
			sprintf(errinfo, "[NorexItemCheckStruct::Check] item_id = %d not exist !", item_id);
			*err = errinfo;
			return false;
		}

		//const NorexItem *norex_item = (const NorexItem *)itembase;

		//short use_type = norex_item->GetUseType();
		//int param1 = norex_item->GetParam1();
		/*int param2 = norex_item->GetParam2();*/

		//switch (use_type)
		//{
		///*case NorexItem::I_NOREX_ITEM_MOUNT_CARD:
		//	{
		//		if (NULL == LOGIC_CONFIG->GetMountOpConfig().GetMountImage(param1))
		//		{
		//			sprintf(errinfo, "I_NOREX_ITEM_MOUNT_CARD item %d mountid = %d not exist !", item_id, param1);
		//			*err = errinfo;
		//			return false;
		//		}
		//	}
		//	break;*/

		///*case NorexItem::I_NOREX_ITEM_SKILLBOOK:
		//	{
		//		if (NULL == SKILLPOOL->GetSkill(param1))
		//		{
		//			sprintf(errinfo, "I_NOREX_ITEM_SKILLBOOK item %d skill %d not exist !", item_id, param1);
		//			*err = errinfo;
		//			return false;
		//		}
		//	}
		//	break;*/
		//	default:
		//		break;
		//}
	}

	return true;
}

void CheckResourceCenter::NorexItemCheckStruct::Clear()
{
	std::vector<ItemID>().swap(item_vec);
}

void CheckResourceCenter::PetValidCheckStruct::Add(int pet_id, const char * source_info)
{
	static PetCheckItem check;
	check.pet_id = pet_id;
	check.info = source_info;
	pet_vec.push_back(check);
}

void CheckResourceCenter::PetValidCheckStruct::AddAttrCheck(int pet_id, int attr_point, int attr_type, const char* source_info)
{
	static PetAttrCheckItem check;
	check.pet_id = pet_id;
	check.attr_type = attr_type;
	check.attr_point = attr_point;
	check.info = source_info;
	pet_attr_vec.push_back(check);
}

void CheckResourceCenter::PetValidCheckStruct::AddBenedictionCheck(int benediction_id, const char * source_info)
{
	static PetBenedictionCheckItem check;
	check.benediction_id = benediction_id;
	check.info = source_info;
	pet_benediction_vec.push_back(check);
}

bool CheckResourceCenter::PetValidCheckStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<PetCheckItem>::iterator it = pet_vec.begin(); it != pet_vec.end(); ++it)
	{
		int pet_id = it->pet_id;
		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL == pet_cfg)
		{
			sprintf(errinfo, "[PetValidCheckStruct::Check] pet_id = %d not exist ! source_info[%s]", pet_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	for (std::vector<PetAttrCheckItem>::iterator it = pet_attr_vec.begin(); it != pet_attr_vec.end(); ++it)
	{
		int pet_id = it->pet_id;
		const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL == pet_cfg)
		{
			sprintf(errinfo, "[PetValidCheckStruct::Check Attr] pet_id = %d not exist ! source_info[%s]", pet_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
		if (it->attr_point > pet_cfg->growth[it->attr_type] || it->attr_point < pet_cfg->growth[it->attr_type] - 6)
		{
			sprintf(errinfo, "[PetValidCheckStruct::Check Attr] pet_id = %d attr not ok! attr_type[%d] attr_point[%d] source_info[%s]", pet_id, it->attr_type, it->attr_point, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	for (std::vector<PetBenedictionCheckItem>::iterator it = pet_benediction_vec.begin(); it != pet_benediction_vec.end(); ++it)
	{
		int benediction_id = it->benediction_id;
		if (!LOGIC_CONFIG->GetPetConfig()->IsHasBenediction(benediction_id))
		{
			sprintf(errinfo, "[PetValidCheckStruct::Check] benediction_id = %d not exist ! source_info[%s]", benediction_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::PetValidCheckStruct::Clear()
{
	std::vector<PetCheckItem>().swap(pet_vec);
	std::vector<PetAttrCheckItem>().swap(pet_attr_vec);
}

void CheckResourceCenter::SpecialEffectCheckStruct::Add(int group_id, const char* source_info)
{
	static EffectCheckItem item;
	item.param1 = group_id;
	item.info = source_info;
	check_vec.push_back(item);
}

void CheckResourceCenter::SpecialEffectCheckStruct::AddSpecial(int sp_id, const char * source_info)
{
	static EffectCheckItem item;
	item.param1 = sp_id;
	item.info = source_info;
	special_check_vec.push_back(item);
}

bool CheckResourceCenter::SpecialEffectCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<EffectCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int group_id = it->param1;
		const EffectGroupCfg * cfg = BattleSpecialEffectPool::GetInstance().GetEffectGroupCfg(group_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[SpecialEffectCheckStruct::Check] group_id = %d not exist ! source_info[%s]", group_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	for (std::vector<EffectCheckItem>::iterator it = special_check_vec.begin(); it != special_check_vec.end(); ++it)
	{
		int sp_id = it->param1;
		const SpecialEffectCfg * cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[SpecialEffectCheckStruct::Check] sp_id = %d not exist ! source_info[%s]", sp_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::SpecialEffectCheckStruct::Clear()
{
	std::vector<EffectCheckItem>().swap(check_vec);
}

void CheckResourceCenter::MonsterCheckStruct::Add(int monster_id, const char* source_info)
{
	static MonsterCheckItem item;
	item.monster_id = monster_id;
	item.info = source_info;
	check_vec.push_back(item);	
}

bool CheckResourceCenter::MonsterCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<MonsterCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int monster_id = it->monster_id;
		const MonsterCfg * cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[MonsterCheckStruct::Check] monster_id = %d not exist ! source_info[%s]", monster_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::MonsterCheckStruct::Clear()
{
	std::vector<MonsterCheckItem>().swap(check_vec);
}

void CheckResourceCenter::TitleCheckStruct::Add(int title_id, const char * source_info)
{
	for (std::vector<TitleCheckItem>::const_iterator it = check_vec.begin(); check_vec.end() != it; ++it)
	{
		if (title_id == it->title_id) return;
	}
	check_vec.push_back(TitleCheckItem(title_id, source_info));
}

bool CheckResourceCenter::TitleCheckStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<TitleCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		const TitleCfg * cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(it->title_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[TitleCheckStruct::Check] titile_id = %d not exist ! source_info[%s]", it->title_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::TitleCheckStruct::Clear()
{
	std::vector<TitleCheckItem>().swap(check_vec);
}

void CheckResourceCenter::TaskCheckStruct::Add(int task_id, const char* source_info)
{
	static TaskCheckStruct::TaskCheckItem tcs;
	tcs.task_id = task_id;
	tcs.info = source_info;

	check_vec.push_back(tcs);
}

bool CheckResourceCenter::TaskCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<TaskCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		const TaskInfomaConfig* cfg = TASKPOOL->GetTaskCfgByID(it->task_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[TaskCheckStruct::Check] task_id = %d not exist ! source_info[%s]", it->task_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::TaskCheckStruct::Clear()
{
	std::vector<TaskCheckItem>().swap(check_vec);
}

void CheckResourceCenter::PartnerCheckStruct::Add(int partner_id, const char* source_info)
{
	for (std::vector<PartnerCheckItem>::const_iterator it = check_vec.begin(); check_vec.end() != it; ++it)
	{
		if (partner_id == it->partner_id) return;
	}
	check_vec.push_back(PartnerCheckItem(partner_id, source_info));
}

bool CheckResourceCenter::PartnerCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<PartnerCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		const PartnerBasicCfg * cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(it->partner_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[PartnerCheckStruct::Check] partner_id = %d not exist ! source_info[%s]", it->partner_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::PartnerCheckStruct::Clear()
{
	std::vector<PartnerCheckItem>().swap(check_vec);
}

void CheckResourceCenter::MonsterGroupCheckStruct::Add(int monster_group_id, const char* source_info)
{
	MonsterGroupCheckItem item;
	item.monster_group_id = monster_group_id;
	item.info = source_info;
	check_vec.push_back(item);
}

bool CheckResourceCenter::MonsterGroupCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<MonsterGroupCheckItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int monster_group_id = it->monster_group_id;
		const MonsterGroupCfg * cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
		if (NULL == cfg)
		{
			sprintf(errinfo, "[MonsterGroupCheckStruct::Check] group_id = %d not exist ! source_info[%s]", monster_group_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::MonsterGroupCheckStruct::Clear()
{
	std::vector<MonsterGroupCheckItem>().swap(check_vec);
}

void CheckResourceCenter::CourseOfGrowthStruct::Add(int param1, const char * source_info)
{
	static CourseOfGrowthCheckParam1 check;
	check.param1 = param1;
	check.info = source_info;
	check_vec.push_back(check);
}

bool CheckResourceCenter::CourseOfGrowthStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<CourseOfGrowthCheckParam1>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int param1 = it->param1;
		if (param1 > LOGIC_CONFIG->GetTrainConfig()->GetPetTrainMaxLevel())
		{
			sprintf(errinfo, "[CourseOfGrowthCheckParam1::Check] param1 = %d not exist ! source_info[%s]", param1, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::CourseOfGrowthStruct::Clear()
{
	std::vector<CourseOfGrowthCheckParam1>().swap(check_vec);
}

void CheckResourceCenter::SkillCheckStruct::Add(int skill_id, const char* source_info)
{
	static SkillCheckParam check;
	check.skill_id = skill_id;
	check.info = source_info;
	check_vec.push_back(check);
}

bool CheckResourceCenter::SkillCheckStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<SkillCheckParam>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int skill_id = it->skill_id;
		if (NULL == SkillPool::Instance().GetSkillCfg(skill_id))
		{
			sprintf(errinfo, "[SkillCheckStruct::Check] skill_id = %d not exist ! source_info[%s]", skill_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::SkillCheckStruct::Clear()
{
	std::vector<SkillCheckParam>().swap(check_vec);
}

void CheckResourceCenter::TaskConditionItemStruct::Add(ItemID item_id, const char* source_info)
{
	static TaskConditionItem item;
	item.item_id = item_id;
	item.info = source_info;
	check_vec.push_back(item);
}

bool CheckResourceCenter::TaskConditionItemStruct::Check(std::string* err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<TaskConditionItem>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		ItemID item_id = it->item_id;
		if (!TASKPOOL->IsConditionNeedItem(item_id))
		{
			sprintf(errinfo, "[TaskConditionItemStruct::Check] item_id = %d not in [task_to_item_type_3] config! source_info[%s]", item_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::TaskConditionItemStruct::Clear()
{
	std::vector<TaskConditionItem>().swap(check_vec);
}

void CheckResourceCenter::PartnerPactCheckStruct::Add(ItemID pact_item_id, const char * source_info)
{
	static PactCheckParam item;
	item.pact_item_id = pact_item_id;
	item.info = source_info;
	check_vec.push_back(item);
}

bool CheckResourceCenter::PartnerPactCheckStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<PactCheckParam>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		ItemID item_id = it->pact_item_id;
		if (-1 == LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(item_id))
		{
			sprintf(errinfo, "[PartnerPactCheckStruct::Check] item_id = %d not in [partner_pact_save_index] config! source_info[%s]", item_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::PartnerPactCheckStruct::Clear()
{
	std::vector<PactCheckParam>().swap(check_vec);
}

void CheckResourceCenter::PeakTokenCheckStruct::Add(ItemID item_id, int reward_group_id, const char * source_info)
{
	static PeakTokenCheckParam item;
	item.item_id = item_id;
	item.reward_group_id = reward_group_id;
	item.info = source_info;
	check_vec.push_back(item);
}

bool CheckResourceCenter::PeakTokenCheckStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<PeakTokenCheckParam>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		ItemID item_id = it->item_id;
		int reward_group_id = it->reward_group_id;

		if (NULL == LOGIC_CONFIG->GetPeakTokenConfig()->GetGroupRandCfg(reward_group_id))
		{
			sprintf(errinfo, "[PeakTokenCheckStruct::Check] item_id = %d no reward_group_id[%d] config! source_info[%s]",
				item_id, reward_group_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::PeakTokenCheckStruct::Clear()
{
	std::vector<PeakTokenCheckParam>().swap(check_vec);
}

void CheckResourceCenter::PassiveSkillCheckStruct::Add(int skill_id, const char * source_info)
{
	static SkillCheckParam check;
	check.skill_id = skill_id;
	check.info = source_info;
	check_vec.push_back(check);
}

bool CheckResourceCenter::PassiveSkillCheckStruct::Check(std::string * err)
{
	char errinfo[1024] = { 0 };

	for (std::vector<SkillCheckParam>::iterator it = check_vec.begin(); it != check_vec.end(); ++it)
	{
		int skill_id = it->skill_id;
		if (NULL == SkillPool::Instance().GetPassiveSkillCfg(skill_id))
		{
			sprintf(errinfo, "[SkillCheckStruct::Check] passive_skill_id = %d not exist ! source_info[%s]", skill_id, it->info.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

void CheckResourceCenter::PassiveSkillCheckStruct::Clear()
{
	std::vector<SkillCheckParam>().swap(check_vec);
}
