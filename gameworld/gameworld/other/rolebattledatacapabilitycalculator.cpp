#include "rolebattledatacapabilitycalculator.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "skill/skillpool.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/professionconfig.hpp"

int RoleBattleDataCapabilityCalculator::CalcRoleCapability(const RoleBattleData& role_battle_data)
{
	int app_prof = role_battle_data.role_character_data.profession / PROFESSION_BASE;
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	const BattleAttr* attr_list = role_battle_data.role_character_data.attr_list;
	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < BATTLE_ATTR_MAX; i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_list[i] * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_list[i] * prof_cap_cfg->cap_base[i];
		}
	}

	int skill_capability = 0;
	const BattleCharacterData::SkillItem* skill_list = role_battle_data.role_character_data.skill_list;
	for (int i = 0; i < role_battle_data.role_character_data.skill_num; ++i)
	{
		//检测是否是该职业技能
		const ProfSkillCfg* prof_skill_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetProfSkillCfgByHashKey(ProfessionConfig::GetHashKey(app_prof, skill_list[i].skill_id));
		if (NULL == prof_skill_cfg) continue;

		int add_skill_score = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfSkillScore(ProfSkillScoreKey(app_prof, skill_list[i].skill_level));
		if (add_skill_score > 0)
		{
			skill_capability += add_skill_score;
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + skill_capability + special_capability;
}

int RoleBattleDataCapabilityCalculator::CalcPartnerCapability(const BattleCharacterData& partner_data)
{
	int app_prof = partner_data.profession; // 伙伴的prof不需要除以BASE_PROFESSION
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	double capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(partner_data.attr_list); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		capability += partner_data.attr_list[i] * 1.0 * prof_cap_cfg->cap_base[i];
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM);
}

int RoleBattleDataCapabilityCalculator::CalcPetCapability(const BattleCharacterData& pet_data)
{
	int pet_base_capability = 0;
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_data.character_id);
	if (NULL != pet_cfg)
	{
		pet_base_capability = pet_cfg->base_score;
	}

	double gongjixishu_1 = 1 + pet_data.attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.00081 + (pet_data.attr_list[BATTLE_ATTR_CRITICAL] + pet_data.attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE]) *
		0.000242 + pet_data.attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.0054;
	double gongjixishu_2 = 1 + (pet_data.attr_list[BATTLE_ATTR_HIT] + pet_data.attr_list[BATTLE_ATTR_DODGE]) * 0.001;
	double jingshenxishu_1 = 1 + 1.2 * pet_data.attr_list[BATTLE_ATTR_MAGIC_ATTACK] / (pet_data.attr_list[BATTLE_ATTR_MAGIC_ATTACK] + 220) + 0.8 * pet_data.attr_list[BATTLE_ATTR_MAGIC_DEFENSE] /
		(pet_data.attr_list[BATTLE_ATTR_MAGIC_DEFENSE] + 300);

	int attr_capability = (int)(pet_data.attr_list[BATTLE_ATTR_MAX_MP] * 1.6 + pet_data.attr_list[BATTLE_ATTR_MAX_HP] * 0.25 + pet_data.attr_list[BATTLE_ATTR_ATTACK] * 1.55 * gongjixishu_1 * gongjixishu_2 +
		pet_data.attr_list[BATTLE_ATTR_DEFENSE] * 1.2 + pet_data.attr_list[BATTLE_ATTR_MENTAL] * 1.95 * jingshenxishu_1 + pet_data.attr_list[BATTLE_ATTR_RECOVERY] * 1.85 +
		pet_data.attr_list[BATTLE_ATTR_AGILITY] * 2.2);

	int skill_capability = 0;
	const PetSkillAddCapabilityCfg * add_capability_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetSkillAddCapabilityCfg(pet_data.level);
	if (NULL != add_capability_cfg)
	{
		for (int i = 0; i < pet_data.skill_num; ++i)
		{
			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(pet_data.skill_list[i].skill_id);
			if (passive_skill == NULL)
			{
				continue;
			}
			switch (passive_skill->skill_priority)
			{
			case PET_SKILL_PRIORITY_TYPE_HIGH:
				{
					skill_capability += add_capability_cfg->high_skill_add;
				}
				break;
			case PET_SKILL_PRIORITY_TYPE_SUPER:
				{
					int skill_level = pet_data.skill_list[i].skill_level;
					if (0 <= skill_level && skill_level < add_capability_cfg->super_skill_add.size())
					{
						skill_capability += add_capability_cfg->super_skill_add[skill_level];
					}
				}
				break;
			default:
				{
					skill_capability += add_capability_cfg->low_skill_add;
				}
				break;
			}
		}
	}

	return pet_base_capability + attr_capability + skill_capability;
}

int RoleBattleDataCapabilityCalculator::CalcCapability(const RoleBattleData& role_battle_data)
{
	int total_capability = 0;
	total_capability += CalcRoleCapability(role_battle_data);
	total_capability += role_battle_data.role_extra_capability;

	if (role_battle_data.fight_pet_idx >= 0 && role_battle_data.fight_pet_idx < role_battle_data.pet_num && role_battle_data.fight_pet_idx < ARRAY_LENGTH(role_battle_data.pet_character_data))
	{
		// 宠物的战力计算会有所偏差，因为主角的宠物战力计算不是直接用属性来计算的
		total_capability += CalcPetCapability(role_battle_data.pet_character_data[role_battle_data.fight_pet_idx].battle_data);
	}

	for (int i = 0; i < role_battle_data.partner_num && i < ARRAY_ITEM_COUNT(role_battle_data.partner_character_data); ++i)
	{
		total_capability += CalcPartnerCapability(role_battle_data.partner_character_data[i].battle_data);
	}

	for (int i = 0; i < role_battle_data.pet_helper_num && i < ARRAY_ITEM_COUNT(role_battle_data.pet_helper_character_data); ++i)
	{
		total_capability += CalcPetCapability(role_battle_data.pet_helper_character_data[i].pet_data.battle_data);
	}

	return total_capability;
}

