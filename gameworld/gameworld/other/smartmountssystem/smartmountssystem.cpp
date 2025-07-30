#include "smartmountssystem.hpp"
#include "gamelog.h"

#include "protocol/msgsmartmountssystem.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"


#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/smartmountssystemconfig.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "item/expense/norexitem.hpp"
#include "skill/skillpool.hpp"
#include "obj/character/role.h"
#include "scene/scenemanager.hpp"

#include "other/pet/pet.hpp"
#include "global/team/team.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/surface/surface.hpp"
#include "other/event/eventhandler.hpp"
#include "other/capability/capability.hpp"


SmartMountsSystem::SmartMountsSystem() : m_module_mgr(NULL)
{
	memset(pet_attr_capability, 0, sizeof(pet_attr_capability));
	memset(attr_list, 0, sizeof(attr_list));
	m_pet_dominance_map.clear();
}

SmartMountsSystem::~SmartMountsSystem()
{
}

void SmartMountsSystem::Init(RoleModuleManager * module_mgr, const SmartMountsSystemParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	//检测配置,给玩家解锁技能
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		this->OnActiveOrLevelUpUnlockSkill(i, false, false);
	}
}

void SmartMountsSystem::GetInitParam(SmartMountsSystemParam * param)
{
	*param = m_param;
}

void SmartMountsSystem::InitEnd()
{
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_NUM; k++)
		{
			if (-1 != m_param.smart_mounts_list[i].dominance_pet_index_list[k])
			{
				unsigned long long item_unique_id = m_module_mgr->GetPet()->GetPetUniqueId(m_param.smart_mounts_list[i].dominance_pet_index_list[k]);
				if(0 == item_unique_id) continue;

				m_pet_dominance_map[item_unique_id] = i;
			}
		}
	}
	this->ReCalcPetAttr();
}

void SmartMountsSystem::OnLogin()
{
}

void SmartMountsSystem::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM) || old_dayid == now_dayid) return;

	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		if (0 != m_param.smart_mounts_list[i].jewelry_data.is_active_flag)
		{
			m_param.smart_mounts_list[i].energy_value += base_cfg->wear_nornaments_day_give;
			if (m_param.smart_mounts_list[i].energy_value > base_cfg->bicycle_energy_max)
			{
				m_param.smart_mounts_list[i].energy_value = base_cfg->bicycle_energy_max;
			}
			this->SendRelatedInfo(i, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_ENERGY_CHANGE, m_param.smart_mounts_list[i].energy_value);
		}
	}
}

const AttributeList & SmartMountsSystem::ReCalcAttr(AttributeList & base_add, bool is_recalc, AttributeList &skill_attr_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM)) return m_attrs_add;
	
	//人物享受装备栏+圣物
	if (is_recalc)
	{
		m_attrs_add.Reset();
		m_skill_attr_list.Reset();
		const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
		{
			const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
			if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

			SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[i];
			AttributeList base_list;
			//技能
			std::vector<SmartMountsSystemBaseSkillParam> skill_list;
			smart_mounts_param.capability = pet_attr_capability[i];
			for (int z = 0; z < MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM; z++)
			{
				//装备数据
				if (0 == smart_mounts_param.equip_list[z].is_active_flag || 0 >= smart_mounts_param.equip_list[z].strength_level)
				{
					continue;
				}
				const SmartMountsSystemEquipCfg * equip_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipCfg(base_cfg->equip_id_list[z], smart_mounts_param.equip_list[z].strength_level);
				if (NULL == equip_cfg)
				{
					continue;
				}
				smart_mounts_param.capability += equip_cfg->equip_score;						//灵骑本身评分需要加入
				if (smart_mounts_param.energy_value < other_cfg.energy_limit_skill) continue;	//精力值不足,技能不生效

				int spid = equip_cfg->equip_sp_id;
				/*	int skill_id = unlock_cfg->exclusive_skill_id[z];
					int skill_level = smart_mounts_param.exclusive_level_list[z];
					if (skill_level > MAX_BATTLE_SKILL_LEVEL || skill_level <= 0) continue;

					const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
					if (NULL == passive_skill) continue;

					skill_list.push_back(SmartMountsSystemBaseSkillParam(skill_id, skill_level));
					if (smart_mounts_param.energy_value < other_cfg.energy_limit_skill) continue;

					int spid = passive_skill->sp_id;
					if (passive_skill->is_level_spid)
					{
						spid = passive_skill->level_spid_list[skill_level];
					}*/

				const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
				if (NULL == spe_cfg) continue;

				for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
				{
					const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
					if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

					const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
					if (NULL == detail_cfg) continue;

					if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
						&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
					{
						m_skill_attr_list.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
					}

					if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
						&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
					{
						BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
						m_skill_attr_list.m_attrs[detail_cfg->param_list[0]] += add_attr;
					}

					if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
						&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
						&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
						&& detail_cfg->param_list[2] > 0)
					{
						int attr_type0 = detail_cfg->param_list[0];
						int attr_type1 = detail_cfg->param_list[1];
						m_skill_attr_list.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
					}
				}
			}
			const SmartMountsSystemHolyCfg * holy_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetHolyCfg(base_cfg->bicycle_holy, smart_mounts_param.holy_strength_level);
			if (NULL != holy_cfg && holy_cfg->attr_type >= BATTLE_ATTR_MIN && holy_cfg->attr_type < BATTLE_ATTR_MAX && holy_cfg->attr_value > 0)
			{
				m_attrs_add.m_attrs[holy_cfg->attr_type] += holy_cfg->attr_value;
				base_list.m_attrs[holy_cfg->attr_type] += holy_cfg->attr_value;
			}

			for (int z = 0; z < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; z++)
			{
				if (smart_mounts_param.base_skill_list[z].skill_level > MAX_BATTLE_SKILL_LEVEL || smart_mounts_param.base_skill_list[z].skill_level <= 0) continue;

				const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(smart_mounts_param.base_skill_list[z].skill_id);
				if (NULL == passive_skill) continue;

				skill_list.push_back(SmartMountsSystemBaseSkillParam(smart_mounts_param.base_skill_list[z].skill_id, smart_mounts_param.base_skill_list[z].skill_level));
			}
			if (!skill_list.empty())
			{
				for (int z = 0; z < (int)skill_list.size(); z++)
				{
					smart_mounts_param.capability += LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillScoreCfgByIdAndLevel(skill_list[z].skill_id, skill_list[z].skill_level);
				}
			} 
			smart_mounts_param.capability += m_module_mgr->GetCapability()->CalcCapabityByAttrList(base_list);
		}
		m_attrs_add.Add(m_skill_attr_list.m_attrs);
	}

	base_add.Add(m_attrs_add.m_attrs);
	skill_attr_list.Add(m_skill_attr_list.m_attrs);
	return m_attrs_add;
}

void SmartMountsSystem::ReCalcPetAttr(int index)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM)) return;
	
	//宠物享受灵骑属性和普通技能,点化
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		if(index != -1 && i != index) continue;		//-1表示全计算

		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[i];
		const SmartMountsSystemLevelCfg * level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level);
		if(NULL == level_cfg) continue;

		memset(attr_list[i], 0, sizeof(attr_list[i]));
		AttributeList base_list;
		//计算当前灵性值
		int smart_nature_value = base_cfg->bicycle_grow_begin;
		if (0 != smart_mounts_param.jewelry_data.is_active_flag && smart_mounts_param.jewelry_data.strength_level > 0)
		{
			const SmartMountsSystemJewelryCfg * jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, smart_mounts_param.jewelry_data.strength_level);
			if (NULL != jewelry_cfg)
			{
				smart_nature_value = jewelry_cfg->bicycle_grow;
			}
		}

		const std::map<int, SmartMountsSystemBreachCfg>* breach_cfg_map = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBreachCfg(smart_mounts_param.bicycle_id);
		const SmartMountsSystemBreachCfg * breach_cfg = NULL;
		if (NULL != breach_cfg_map)
		{
			for (std::map<int, SmartMountsSystemBreachCfg>::const_iterator it = breach_cfg_map->begin(); it != breach_cfg_map->end(); ++it)
			{
				if (smart_mounts_param.level < it->first || (smart_mounts_param.level == it->first && 0 == smart_mounts_param.is_breach_flag)) break;

				breach_cfg = &it->second;
			}
		}
		std::map<int, int> attr_map;
		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; k++)
		{
			if (NULL != breach_cfg && breach_cfg->attr_value[k] > 0 && breach_cfg->attr_type[k] >= BATTLE_ATTR_MIN && breach_cfg->attr_type[k] < BATTLE_ATTR_MAX)
			{
				attr_map[breach_cfg->attr_type[k]] += breach_cfg->attr_value[k];
			}
			if (level_cfg->attr_value[k] > 0 && level_cfg->attr_type[k] >= BATTLE_ATTR_MIN && level_cfg->attr_type[k] < BATTLE_ATTR_MAX)
			{
				attr_map[level_cfg->attr_type[k]] += level_cfg->attr_value[k];
			}
		}
		if ((int)attr_map.size() > MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM)
		{
			gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s index[%d] level[%d] attr_num[%d] exceed the limit", __FUNCTION__, i, smart_mounts_param.level, (int)attr_map.size());
		}

		const SmartMountsSystemJewelryCfg * max_jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryMaxLevelById(base_cfg->jewelry_id));
		if (NULL != max_jewelry_cfg && max_jewelry_cfg->bicycle_grow > base_cfg->bicycle_grow_begin)
		{
			//当前属性(it->second) = 升级属性+突破属性,当前属性*(1 - 投放比例) + 当前属性*投放比例*((当前灵性 - 初始)/(最大灵性 - 初始)) 
			double attr_scale = 100 - base_cfg->grow_min_proportion;
			double nature_scale = base_cfg->grow_min_proportion * ((smart_nature_value - base_cfg->bicycle_grow_begin) * 1.0 / (max_jewelry_cfg->bicycle_grow - base_cfg->bicycle_grow_begin));
			int idx = 0;
			for (std::map<int, int>::iterator it = attr_map.begin(); it != attr_map.end() && idx < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; ++it, idx++)
			{
				attr_list[i][idx].attr_type = it->first; 
				attr_list[i][idx].attr_value += static_cast<short>((it->second * attr_scale + it->second * nature_scale) / 100);
				base_list.m_attrs[it->first] += attr_list[i][idx].attr_value;
				/*	if (smart_mounts_param.energy_value >= other_cfg.energy_limit_base)
				{
				m_attrs_add.m_attrs[it->first] += attr_list[i][idx].attr_value;
				}*/
			}
		}

		// 点化属性不加入基础属性列表,但需要加入总评分
		const SmartMountsSystemRevealCfg * reveal_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSmartMountsSystemRevealCfg(base_cfg->bicycle_id, smart_mounts_param.reveal_level);
		if(NULL != reveal_cfg)
		{
			for(int i = 0; i < ARRAY_ITEM_COUNT(reveal_cfg->attr_list); ++i)
			{
				if (reveal_cfg->attr_list[i].attr_value > 0 && reveal_cfg->attr_list[i].attr_type >= BATTLE_ATTR_MIN && reveal_cfg->attr_list[i].attr_type < BATTLE_ATTR_MAX)
				{
					base_list.m_attrs[reveal_cfg->attr_list[i].attr_type] += reveal_cfg->attr_list[i].attr_value;
				}
			}
		}

		smart_mounts_param.capability -= pet_attr_capability[i];		//减去旧的属性计算的评分
		smart_mounts_param.capability += pet_attr_capability[i] = m_module_mgr->GetCapability()->CalcCapabityByAttrList(base_list);
	}
}

bool SmartMountsSystem::IsPetDomination(unsigned long long item_unique_id)
{
	if (m_pet_dominance_map.find(item_unique_id) == m_pet_dominance_map.end())
	{
		return false;
	}
	return true;
}

void SmartMountsSystem::GetDominationBaseAttr(unsigned long long item_unique_id, AttributeList & base_add)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM)) return;

	std::map<unsigned long long, int>::iterator it = m_pet_dominance_map.find(item_unique_id);
	if (it == m_pet_dominance_map.end()) return;

	if (!this->IsValid(it->second))
	{
		m_pet_dominance_map.erase(it);
		return;
	}
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	 const SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[it->second];
	if (smart_mounts_param.energy_value < other_cfg.energy_limit_base) return;

	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; i++)
	{
		if (attr_list[it->second][i].attr_value > 0 && attr_list[it->second][i].attr_type >= BATTLE_ATTR_MIN && attr_list[it->second][i].attr_type < BATTLE_ATTR_MAX)
		{
			base_add.m_attrs[attr_list[it->second][i].attr_type] += attr_list[it->second][i].attr_value;
		}
	}
	const SmartMountsSystemRevealCfg * reveal_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSmartMountsSystemRevealCfg(smart_mounts_param.bicycle_id, smart_mounts_param.reveal_level);
	if (NULL != reveal_cfg)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(reveal_cfg->attr_list); ++i)
		{
			if (reveal_cfg->attr_list[i].attr_value > 0 && reveal_cfg->attr_list[i].attr_type >= BATTLE_ATTR_MIN && reveal_cfg->attr_list[i].attr_type < BATTLE_ATTR_MAX)
			{
				base_add.m_attrs[reveal_cfg->attr_list[i].attr_type] += reveal_cfg->attr_list[i].attr_value;
			}
		}
	}
}

void SmartMountsSystem::GetBaseSkillList(unsigned long long item_unique_id, std::vector<SkillPro>* base_skill_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM)) return;

	std::map<unsigned long long, int>::iterator it= m_pet_dominance_map.find(item_unique_id);
	if(it == m_pet_dominance_map.end() || NULL == base_skill_list) return;

	if(!this->IsValid(it->second))
	{
		m_pet_dominance_map.erase(it);
		return;
	}
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	if (m_param.smart_mounts_list[it->second].energy_value < other_cfg.energy_limit_skill) return;

	for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
	{
		if (m_param.smart_mounts_list[it->second].base_skill_list[i].skill_level <= 0 || m_param.smart_mounts_list[it->second].base_skill_list[i].skill_id == 0)
		{
			continue;
		}
		base_skill_list->push_back(SkillPro(m_param.smart_mounts_list[it->second].base_skill_list[i].skill_id, m_param.smart_mounts_list[it->second].base_skill_list[i].skill_level));
	}
}

int SmartMountsSystem::GetSkillOrSpCap()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM)) return 0;

	int equip_sp_capability = 0;
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[i];
		if (smart_mounts_param.energy_value < other_cfg.energy_limit_skill) continue;

		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM; k++)
		{
			if (0 == smart_mounts_param.equip_list[k].is_active_flag || 0 >= smart_mounts_param.equip_list[k].strength_level) continue;

			const SmartMountsSystemEquipCfg * equip_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipCfg(base_cfg->equip_id_list[k], smart_mounts_param.equip_list[k].strength_level);
			if(NULL == equip_cfg) continue;

			equip_sp_capability += equip_cfg->equip_score;
		}
	}

	/*std::vector<SmartMountsSystemBaseSkillParam> smart_mounts_exclusive_skill_list;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[i];
		if (smart_mounts_param.energy_value < other_cfg.energy_limit_skill) continue;

		const SmartMountsSkillUnlockCfg * unlock_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetUnlockCfg(smart_mounts_param.bicycle_id);
		if (NULL == unlock_cfg) continue;

		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM; k++)
		{
			int skill_id = unlock_cfg->exclusive_skill_id[k];
			int skill_level = smart_mounts_param.exclusive_level_list[k];
			if (skill_level > MAX_BATTLE_SKILL_LEVEL || skill_level <= 0) continue;

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
			if (NULL == passive_skill) continue;

			smart_mounts_exclusive_skill_list.push_back(SmartMountsSystemBaseSkillParam(skill_id, skill_level));
		}
	}
	int skill_capability = 0;
	for (int i = 0; i < (int)smart_mounts_exclusive_skill_list.size(); i++)
	{
		skill_capability += LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillScoreCfgByIdAndLevel(smart_mounts_exclusive_skill_list[i].skill_id, smart_mounts_exclusive_skill_list[i].skill_level);
	}*/

	return equip_sp_capability;
}

void SmartMountsSystem::OnActiveSmartMounts(int index)
{
	if(!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level > 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_UNLOCKED);
		return;
	}

	const SmartMountsSystemLevelCfg * level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(base_cfg->bicycle_id, 1);
	if (NULL == level_cfg)
	{
		return;
	}

	const SmartMountsSkillUnlockCfg * skill_unlock_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetUnlockCfg(base_cfg->bicycle_id);
	if (NULL == skill_unlock_cfg)
	{
		return;
	}
	if (!base_cfg->gain_way_list.empty())
	{
		for (int i = 0; i < (int)base_cfg->gain_way_list.size(); i++)
		{
			const SmartMountsSystemGainWayCfg * gain_way_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetGainWayCfg(base_cfg->gain_way_list[i]);
			if(NULL == gain_way_cfg) continue;

			UNSTD_STATIC_CHECK(SmartMountsSystemGainWayCfg::MAX_SMART_MOUNTS_SYSTEM_GAIN_WAY_TYPE_NUM == SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_MAX);
			bool is_faild = false;
			switch (gain_way_cfg->way_type)
			{
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_SMART_MOUNTS_LEVEL:
			{
				if (m_module_mgr->GetRole()->GetLevel() < gain_way_cfg->param2)
				{
					is_faild =  true;
				}
			}
			break;
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_NUM_LEVEL_SMART_MOUNTS:
			{
				int meet_cond_count = 0;
				for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_NUM; k++)
				{
					if(k == index) continue;
					
					if (m_param.smart_mounts_list[k].level >= gain_way_cfg->param2)
					{
						meet_cond_count++;
					}
				}
				if (meet_cond_count < gain_way_cfg->param1)
				{
					is_faild = true;
				}
			}
			break;
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_SMART_MOUNTS_SKILL_LEVEL:
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_MAX_SMART_MOUNTS_SKILL_LEVEL:
			{
				int meet_cond_count = 0;
				for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_NUM; k++)
				{
					if (k == index) continue;

					meet_cond_count += m_param.smart_mounts_list[k].GetReachSkillLevelCount(gain_way_cfg->param2);
				}
				if (meet_cond_count < gain_way_cfg->param1)
				{
					is_faild = true;
				}
			}
			break;
			default:
				break;
			}
			
			if (is_faild)
			{
				m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_UNLOCK_COND_NOT_MEET);
				return;
			}
		}
	}
	if (0 != base_cfg->active_use_item_id)
	{
		if (!m_module_mgr->GetKnapsack()->ConsumeItem(base_cfg->active_use_item_id, base_cfg->active_use_item_num, __FUNCTION__))
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_ACTIVE_ITEM_NOT_ENOUGH);
			return;
		}
	}
	smart_mounts_param.bicycle_id = base_cfg->bicycle_id;
	smart_mounts_param.level = 1;
	smart_mounts_param.energy_value = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().init_energy;
	smart_mounts_param.remain_skill_point += level_cfg->skill_point_gain;
	/*int min_grow = base_cfg->bicycle_grow_min * 100;
	int max_grow = base_cfg->bicycle_grow_max * 100 + 1;
	smart_mounts_param.smart_nature_value = RandomNum(min_grow, max_grow);*/

	if (this->OnActiveOrLevelUpUnlockSkill(index, true))//有新解锁技能才需重算
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	}
	this->ReCalcPetAttr(index);
	EventHandler::Instance().OnRoleActiveSmartMounts(m_module_mgr->GetRole(), index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill_point[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, smart_mounts_param.remain_skill_point);
}

void SmartMountsSystem::OnLevelUpSmartMounts(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	int max_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxLevelById(smart_mounts_param.bicycle_id);
	if (smart_mounts_param.level >= max_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LEVEL_FULL);
		return;
	}
	const SmartMountsSystemLevelCfg * level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level);
	const SmartMountsSystemLevelCfg * new_level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level + 1);
	if (NULL == level_cfg || NULL == new_level_cfg)
	{
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < new_level_cfg->role_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_level_not_engough, new_level_cfg->role_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;
	}

	if (0 != level_cfg->is_need_breach && 1 != smart_mounts_param.is_breach_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_FINISH_BREACH);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItem(level_cfg->level_up_consume_item_id, level_cfg->level_up_consume_item_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	smart_mounts_param.level = new_level_cfg->level;
	smart_mounts_param.remain_skill_point += new_level_cfg->skill_point_gain;
	smart_mounts_param.is_breach_flag = 0;
	if(this->OnActiveOrLevelUpUnlockSkill(index, false))	//有新解锁技能才需重算
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	}
	this->ReCalcPetAttr(index);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_base || smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill_point[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, smart_mounts_param.remain_skill_point);
}

void SmartMountsSystem::OnBreakLevelSmartMounts(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	int max_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxLevelById(smart_mounts_param.bicycle_id);
	if (smart_mounts_param.level >= max_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LEVEL_FULL);
		return;
	}
	const SmartMountsSystemLevelCfg * level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level);
	const SmartMountsSystemLevelCfg * new_level_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetLevelCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level + 1);
	const SmartMountsSystemBreachCfg * breach_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBreachCfg(smart_mounts_param.bicycle_id, smart_mounts_param.level);
	if (NULL == level_cfg || NULL == new_level_cfg || NULL == breach_cfg || 0 == level_cfg->is_need_breach)
	{
		return;
	}
	if (1 == smart_mounts_param.is_breach_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_ALREADY_BRAECH);
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < new_level_cfg->role_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_level_not_engough, new_level_cfg->role_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;
	}
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(breach_cfg->breach_consume_item_id, breach_cfg->breach_consume_item_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	int rate_num = RandomNum(SmartMountsSystemBreachCfg::MAX_BREACH_RATE_COUNT);
	if (rate_num >= breach_cfg->breach_rate)
	{
		if(breach_cfg->consume_return > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_breach_faild_notice, breach_cfg->consume_return, breach_cfg->breach_consume_item_id);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					m_module_mgr->NetSend((const void *)&sm, sendlen);
				}
			}
			//突破消耗数量会比返还大
			m_module_mgr->GetKnapsack()->Put(ItemConfigData(breach_cfg->breach_consume_item_id, true, breach_cfg->consume_return), PUT_REASON_SMART_MOUNTS_SYSTEM);
		}
		else
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_BREACH_FAILD);
		}
		return;
	}

	smart_mounts_param.is_breach_flag = 1;
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_BREAK_SUCC, 0, 0);
	//m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	this->ReCalcPetAttr(index);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_base)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s succ ! user[%d, %s] index[%d, %d] skill_point[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, smart_mounts_param.remain_skill_point);
}

void SmartMountsSystem::OnRefineNatureSmartMounts(int index)
{
	//if (!this->IsValid(index)) return;

	//const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	//if (NULL == base_cfg)
	//{
	//	return;
	//}
	//if (m_param.smart_mounts_list[index].level <= 0)
	//{
	//	m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
	//	return;
	//}
	//const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	//if (m_param.smart_mounts_list[index].smart_nature_value >= base_cfg->bicycle_grow_max * 100)
	//{
	//	m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NATURE_LIMIT);
	//	return;
	//}

	//if (!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.grow_reset_use, 1, __FUNCTION__))
	//{
	//	m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
	//	return;
	//}

	//int min_grow = base_cfg->bicycle_grow_min * 100;
	//int max_grow = base_cfg->bicycle_grow_max * 100 + 1;	
	//m_param.smart_mounts_list[index].smart_nature_value = RandomNum(min_grow, max_grow);
	//m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	//this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_NATURE_CHANGE, m_param.smart_mounts_list[index].smart_nature_value);
	//this->SendSingleAttr(index);
}

void SmartMountsSystem::OnSkillUpSmartMounts(int index, bool is_base_skill, int skill_index)
{
	//技能改版,老版技能升级接口屏蔽
	return;
	if (!this->IsValid(index) || !is_base_skill) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	const SmartMountsSkillUpCfg * skill_up_cfg = NULL;
	if (is_base_skill)
	{
		if(skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return;

		if (0 == smart_mounts_param.base_skill_list[skill_index].skill_id || smart_mounts_param.base_skill_list[skill_index].skill_level <= 0)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
			return;
		}
		int max_skill_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxSkillLevel(smart_mounts_param.bicycle_id, skill_index, is_base_skill);
		if (smart_mounts_param.base_skill_list[skill_index].skill_level >= max_skill_level)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL);
			return;
		}

		skill_up_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillUpCfg(smart_mounts_param.bicycle_id, skill_index, is_base_skill, smart_mounts_param.base_skill_list[skill_index].skill_level);
	}
	/*else
	{
		if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM) return;

		if (smart_mounts_param.exclusive_level_list[skill_index] <= 0)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
			return;
		}

		int max_skill_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxSkillLevel(smart_mounts_param.bicycle_id, skill_index, is_base_skill);
		if (smart_mounts_param.exclusive_level_list[skill_index] >= max_skill_level)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL);
			return;
		}
		skill_up_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillUpCfg(smart_mounts_param.bicycle_id, skill_index, is_base_skill, smart_mounts_param.exclusive_level_list[skill_index]);
	}*/
	if (NULL == skill_up_cfg)
	{
		return;
	}
	if (smart_mounts_param.level < skill_up_cfg->bicycle_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_skill_up_level_limit, skill_up_cfg->bicycle_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;
	}
	if (smart_mounts_param.remain_skill_point < skill_up_cfg->need_consume_skill_point)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_POINT_NOT_ENOUGH);
		return;
	}

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < (int)skill_up_cfg->consume_list.size() && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
	{
		stuff_list[stuff_count].item_id = skill_up_cfg->consume_list[i].item_id;
		stuff_list[stuff_count].num = skill_up_cfg->consume_list[i].num;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0)
		{
			if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}
		}
	}
	smart_mounts_param.remain_skill_point -= skill_up_cfg->need_consume_skill_point;
	int skill_level = 0;
	if (is_base_skill)
	{
		skill_level = smart_mounts_param.base_skill_list[skill_index].skill_level += 1;
		this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_SKILL_UP_BASE, skill_level, skill_index);
	}
	/*else
	{
		skill_level = smart_mounts_param.exclusive_level_list[skill_index] += 1;
		this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_SKILL_UP_EXCLUSIVE, skill_level, skill_index);
	}*/
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill_point[%d] skill[%d, %d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, smart_mounts_param.remain_skill_point, skill_index, skill_level);
}

void SmartMountsSystem::OnRefineSkillSmartMounts(int index, int lock_flag, bool is_auto_buy)
{
	return;//洗练逻辑去除
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}

	UInt16 lock_skill_id_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM] = { 0 };
	short count = 0;
	int lock_num = 0;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
	{
		SmartMountsSystemBaseSkillParam & skill_param = smart_mounts_param.base_skill_list[i];
		if(skill_param.skill_level <= 0) continue;

		if (lock_flag & (1 << i))
		{
			skill_param.is_unlock = 1;
			lock_skill_id_list[i] = skill_param.skill_id;
			lock_num++;
		}
		else
		{
			skill_param.is_unlock = 0;
		}
		count++;
	}
	if (lock_num == count)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_REFINE);
		return;
	}
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	int need_consume_gold = 0;
	switch (lock_num)
	{
		case 1:
		{
			need_consume_gold = other_cfg.lock_skill_1;
		}
		break;
		case 2:
		{
			need_consume_gold = other_cfg.lock_skill_2;
		}
		break;
	default:
		break;
	}

	ItemExtern::ItemConsumeList consume_list;
	consume_list.Reset();

	// 获取可消耗的背包材料列表
	short stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	stuff_list[stuff_count].item_id = other_cfg.skill_reset_use;
	stuff_list[stuff_count].num = other_cfg.skill_reset_number;
	stuff_list[stuff_count].buyable = is_auto_buy;
	stuff_count += 1;

	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
	{
		return;
	}
	consume_list.need_gold_num += need_consume_gold;
	if (consume_list.need_gold_num > 0 && !m_module_mgr->GetMoney()->GoldMoreThan(consume_list.need_gold_num))
	{
		m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	UInt16 new_base_skill_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM] = { 0 };
	if (!LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetRefineBaseSkill(smart_mounts_param.bicycle_id, lock_skill_id_list, count, new_base_skill_list))
	{
		return;
	}

	//消耗
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}

	if (consume_list.need_gold_num > 0)
	{
		m_module_mgr->GetMoney()->UseGold(consume_list.need_gold_num, __FUNCTION__);
	}
	
	memcpy(smart_mounts_param.new_base_skill_list, new_base_skill_list, sizeof(UInt16) * MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM);
	smart_mounts_param.is_refine_flag = 1;
	this->SendRefineInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] lock_flag[%d] is_auto_buy[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, lock_flag, is_auto_buy ? 1 : 0);
}

void SmartMountsSystem::OnSaveRefineSkillSmartMounts(int index)
{
	return;//洗练逻辑去除
	if (!this->IsValid(index)) return;
	
	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (1 != smart_mounts_param.is_refine_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NO_REFINING);
		return;
	}
	for (int i = 0; i < ARRAY_LENGTH(smart_mounts_param.new_base_skill_list); i++)
	{
		if(0 == smart_mounts_param.new_base_skill_list[i]) continue;	//防止出现洗练不保存后升级解锁新技能再保存的情况

		smart_mounts_param.base_skill_list[i].skill_id = smart_mounts_param.new_base_skill_list[i];
	}
	memset(smart_mounts_param.new_base_skill_list, 0, sizeof(smart_mounts_param.new_base_skill_list));
	smart_mounts_param.is_refine_flag = 0;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendRefineInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level);
}

void SmartMountsSystem::OnCancelRefineSmartMounts(int index)
{
	return;//洗练逻辑去除
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	memset(smart_mounts_param.new_base_skill_list, 0, sizeof(smart_mounts_param.new_base_skill_list));
	smart_mounts_param.is_refine_flag = 0;
	this->SendRefineInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level);
}

void SmartMountsSystem::OnAddEnergySmartMounts(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (base_cfg->bicycle_energy_max <= smart_mounts.energy_value)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_ENERGY_FULL);
		return;
	}
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.energy_up, 1, __FUNCTION__))
	{
		if (!m_module_mgr->GetMoney()->UseCoinBind(other_cfg.energy_up_coin, __FUNCTION__))
		{
			m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}
	int last_energy_value = smart_mounts.energy_value;
	smart_mounts.energy_value += other_cfg.energy_up_number;
	if (smart_mounts.energy_value > base_cfg->bicycle_energy_max)
	{
		smart_mounts.energy_value = base_cfg->bicycle_energy_max;
	}
	if ((last_energy_value < other_cfg.energy_limit_base && smart_mounts.energy_value >= other_cfg.energy_limit_base) || (last_energy_value < other_cfg.energy_limit_skill && smart_mounts.energy_value >= other_cfg.energy_limit_skill))
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_ENERGY_CHANGE, smart_mounts.energy_value);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] last_energy_value[%d] cur_energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), 
		m_module_mgr->GetRole()->GetName(), index, smart_mounts.level, last_energy_value, smart_mounts.energy_value);
}

void SmartMountsSystem::OnResetSkillSmartMounts(int index, bool is_base_skill, int skill_index)
{
	//技能改版,老版技能升级接口屏蔽
	return;
	if (!this->IsValid(index) || !is_base_skill) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	int skill_level = 0;
	if (is_base_skill)
	{
		if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return;

		if (0 == smart_mounts.base_skill_list[skill_index].skill_id || smart_mounts.base_skill_list[skill_index].skill_level <= 0)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
			return;
		}
		if (smart_mounts.base_skill_list[skill_index].skill_level == 1)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_MIN);
			return;
		}
		skill_level = smart_mounts.base_skill_list[skill_index].skill_level;
	}
	/*else
	{
		if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM) return;

		if (smart_mounts.exclusive_level_list[skill_index] <= 0)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
			return;
		}
		if (smart_mounts.exclusive_level_list[skill_index] == 1)
		{
			m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_MIN);
			return;
		}
		skill_level = smart_mounts.exclusive_level_list[skill_index];
	}*/
	const std::map<int, SmartMountsSkillUpCfg> * skill_up_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSkillUpCfg(smart_mounts.bicycle_id, skill_index, is_base_skill);
	if (NULL == skill_up_cfg)
	{
		return;
	}
	int return_skill_point = 0;
	for (std::map<int, SmartMountsSkillUpCfg>::const_iterator it = skill_up_cfg->begin(); it != skill_up_cfg->end(); ++it)
	{
		if (skill_level <= it->first) continue;

		return_skill_point += it->second.need_consume_skill_point;
	}
	smart_mounts.remain_skill_point += return_skill_point;
	if (is_base_skill)
	{
		smart_mounts.base_skill_list[skill_index].skill_level = 1;
	}
	/*else
	{
		smart_mounts.exclusive_level_list[skill_index] = 1;
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	}*/
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendSingleInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill_point[%d] skill[%d, %d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.remain_skill_point, skill_index, skill_level);
}

void SmartMountsSystem::OnDominancePetSmartMounts(int index, int pet_index, int dominance_index)
{
	if (!this->IsValid(index) || m_module_mgr->GetPet()->PetInvalid(pet_index) || dominance_index < 0 || dominance_index >= MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM) return;

	unsigned long long item_unique_id = m_module_mgr->GetPet()->GetPetUniqueId(pet_index);
	if(0 == item_unique_id) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (this->IsPetDomination(item_unique_id))
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
		return;
	}
	smart_mounts.dominance_pet_index_list[dominance_index] = pet_index;
	m_pet_dominance_map[item_unique_id] = index;
	m_module_mgr->GetPet()->OnDominanceChange(pet_index);
	this->SendDominanceInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] pet_index[%d] dominance_index[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, pet_index, dominance_index);
}

void SmartMountsSystem::OnCancelDominancePet(int index, int dominance_index)
{
	if (!this->IsValid(index) || dominance_index < 0 || dominance_index >= MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	int pet_index = smart_mounts.dominance_pet_index_list[dominance_index];
	unsigned long long item_unique_id = m_module_mgr->GetPet()->GetPetUniqueId(pet_index);
	if (0 == item_unique_id) return;

	m_pet_dominance_map.erase(item_unique_id);
	smart_mounts.dominance_pet_index_list[dominance_index] = -1;
	m_module_mgr->GetPet()->OnDominanceChange(pet_index);
	this->SendDominanceInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] pet_index[%d] dominance_index[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, pet_index, dominance_index);
}

bool SmartMountsSystem::OnCancelDominancePet(int pet_index)
{
	unsigned long long item_unique_id = m_module_mgr->GetPet()->GetPetUniqueId(pet_index);

	const int * smart_mounts_index_ptr = MapValuesConstPtr(m_pet_dominance_map, item_unique_id);
	if (!smart_mounts_index_ptr)
	{
		return true;
	}

	int index = *smart_mounts_index_ptr;

	if (!this->IsValid(index))
	{
		return true;
	}

	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	for (int i = 0; i < ARRAY_LENGTH(smart_mounts.dominance_pet_index_list); ++i)
	{
		if (smart_mounts.dominance_pet_index_list[i] == pet_index)
		{
			smart_mounts.dominance_pet_index_list[i] = -1;
		}
	}

	this->SendDominanceInfo(index);
	m_module_mgr->GetPet()->OnDominanceChange(pet_index);

	return true;
}

void SmartMountsSystem::OnRevealSmartMounts(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}

	const SmartMountsSystemRevealCfg * reveal_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetSmartMountsSystemRevealCfg(base_cfg->bicycle_id, smart_mounts_param.reveal_level + 1);
	if (NULL == reveal_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_FULL);
		return;
	}

	if(m_module_mgr->GetRole()->GetLevel() < reveal_cfg->role_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_level_not_engough, reveal_cfg->role_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;	
	}
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(reveal_cfg->consume_item_id, reveal_cfg->consume_item_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	smart_mounts_param.reveal_level += 1;
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_REVEAL_UP, smart_mounts_param.reveal_level, 0);
	this->ReCalcPetAttr(index);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_base || smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] reveal_level[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, smart_mounts_param.reveal_level);
}

int SmartMountsSystem::GetUnlockOrMaxJewelryLevel(bool is_unlock)
{
	int max_num = 0;
	if (is_unlock)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; ++i)
		{
			if(m_param.smart_mounts_list[i].level <= 0) continue;

			max_num++;
		}
	}
	else
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; ++i)
		{
			if (m_param.smart_mounts_list[i].level <= 0) continue;

			if (max_num < m_param.smart_mounts_list[i].jewelry_data.strength_level)
			{
				max_num = m_param.smart_mounts_list[i].jewelry_data.strength_level;
			}
		}
	}

	return max_num;
}

char SmartMountsSystem::GetCurRideSmartMountsImage()
{
	if (m_param.ride_index >= 0 && m_param.ride_index < MAX_SMART_MOUNTS_SYSTEM_NUM)
	{
		return m_param.smart_mounts_list[m_param.ride_index].is_use_advanced_flag;
	}
	return 0;
}

void SmartMountsSystem::OnFinishFunGuide()
{
	m_param.is_finish_fun_guide_flag = 1;
	this->SendOtherInfo();
}

void SmartMountsSystem::OnLearnSkillSmartMounts(int index, int skill_index, int column, int bag_index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return;

	SmartMountsSystemBaseSkillParam & skill_info = smart_mounts_param.base_skill_list[skill_index];
	if (0 == skill_info.is_unlock)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
		return;
	}
	if(0 != skill_info.skill_id || skill_info.skill_level > 0)
	{
		return;
	}
	const ItemGridData* grid_data = m_module_mgr->GetKnapsack()->GetItemGridData(column, bag_index);
	if(NULL == grid_data)
	{
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if(NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType())
	{
		return;
	}

	const NorexItem * norex_item = (const NorexItem*)item_base;
	if(NorexItem::I_NOREX_ITEM_SMART_MOUNTS_SKILL != norex_item->GetUseType())
	{
		return;
	}
	//孔位不对应
	if (norex_item->GetParam2() != skill_index)
	{
		return;
	}

	int max_skill_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxSkillLevel(smart_mounts_param.bicycle_id, skill_index, true);
	if (norex_item->GetParam3() > max_skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	
	skill_info.skill_id = norex_item->GetParam1();
	skill_info.skill_level = norex_item->GetParam3();
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_LEARN_SKILL, skill_index, skill_info.skill_id, skill_info.skill_level);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill[idx:%d, lv:%d, id:%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, skill_index, skill_info.skill_level, skill_info.skill_id);
}

void SmartMountsSystem::OnSkillUpSmartMounts(int index, int skill_index, int column, int bag_index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return;

	SmartMountsSystemBaseSkillParam & skill_info = smart_mounts_param.base_skill_list[skill_index];
	if (0 == skill_info.is_unlock)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
		return;
	}
	if (0 == skill_info.skill_id || skill_info.skill_level <= 0)
	{
		return;
	}
	const ItemGridData* grid_data = m_module_mgr->GetKnapsack()->GetItemGridData(column, bag_index);
	if (NULL == grid_data)
	{
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType())
	{
		return;
	}

	const NorexItem * norex_item = (const NorexItem*)item_base;
	if (NorexItem::I_NOREX_ITEM_SMART_MOUNTS_SKILL != norex_item->GetUseType())
	{
		return;
	}
	//孔位不对应 
	if (norex_item->GetParam2() != skill_index)
	{
		return;
	}

	int max_skill_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxSkillLevel(smart_mounts_param.bicycle_id, skill_index, true);
	if(skill_info.skill_level >= max_skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL);
		return;
	}

	if (norex_item->GetParam3() > max_skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}
	if(norex_item->GetParam3() < skill_info.skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_UP_FAILED);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	int old_skill_level = skill_info.skill_level;
	if(skill_info.skill_level == norex_item->GetParam3())
	{
		skill_info.skill_level += 1;
	}
	else
	{
		skill_info.skill_level = norex_item->GetParam3();
	}

	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_SKILL_UP_BASE, skill_info.skill_level, skill_index);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d]  skill[idx:%d, lv:%d, id:%d] old_level[%d]", __FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), index, smart_mounts_param.level, skill_index, skill_info.skill_level, 
		skill_info.skill_id, old_skill_level);
}

void SmartMountsSystem::OnSkillReplaceSmartMounts(int index, int skill_index, int column, int bag_index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return;

	SmartMountsSystemBaseSkillParam & skill_info = smart_mounts_param.base_skill_list[skill_index];
	if (0 == skill_info.is_unlock)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
		return;
	}
	if (0 == skill_info.skill_id || skill_info.skill_level <= 0)
	{
		return;
	}
	const ItemGridData* grid_data = m_module_mgr->GetKnapsack()->GetItemGridData(column, bag_index);
	if (NULL == grid_data)
	{
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType())
	{
		return;
	}

	const NorexItem * norex_item = (const NorexItem*)item_base;
	if (NorexItem::I_NOREX_ITEM_SMART_MOUNTS_SKILL != norex_item->GetUseType())
	{
		return;
	}
	//孔位不对应 
	if (norex_item->GetParam2() != skill_index)
	{
		return;
	}

	int max_skill_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetMaxSkillLevel(smart_mounts_param.bicycle_id, skill_index, true);
	if (norex_item->GetParam3() > max_skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}
	if (skill_info.skill_id == norex_item->GetParam1() && norex_item->GetParam3() <= skill_info.skill_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_SKILL_UP_FAILED);
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	SmartMountsSystemBaseSkillParam old_skill_info = skill_info;
	skill_info.skill_id = norex_item->GetParam1();
	skill_info.skill_level = GetMax(skill_info.skill_level, (char)norex_item->GetParam3());

	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_REPLACE_SKILL, skill_index, skill_info.skill_id, skill_info.skill_level);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	if (smart_mounts_param.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_skill)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts_param.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts_param.dominance_pet_index_list[i]);
			}
		}
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d]  skill[idx:%d, lv:%d, id:%d] old[id:%d, lv%d]", __FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), index, smart_mounts_param.level, skill_index, skill_info.skill_level, skill_info.skill_id, 
		old_skill_info.skill_id, old_skill_info.skill_level);
}

void SmartMountsSystem::OnSkillItemComposeItem(int main_item_id, int stuff_item_id, int target_item_id)
{
	const NorexItem * main_item = this->GetSkillItem(main_item_id);
	const NorexItem * stuff_item = this->GetSkillItem(stuff_item_id);
	const NorexItem * target_item = this->GetSkillItem(target_item_id);
	if(NULL == main_item || NULL == stuff_item || NULL == target_item)
	{
		return;
	}
	//检测是否是同格子(同品质)
	if(main_item->GetParam2() != stuff_item->GetParam2() || main_item->GetParam2() != target_item->GetParam2())
	{
		return;
	}
	//检测等级
	if(main_item->GetParam3() != stuff_item->GetParam3() || main_item->GetParam3() + 1 != target_item->GetParam3())
	{
		return;
	}
	//检测技能
	if (main_item->GetParam1() != target_item->GetParam1())
	{
		return;
	}

	if (!m_module_mgr->GetKnapsack()->CheckForPut(target_item_id, 1, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	{
		stuff_list[stuff_count].item_id = main_item_id;
		stuff_list[stuff_count].num = (main_item_id == stuff_item_id) ? 2 : 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}
	if(main_item_id != stuff_item_id)
	{
		stuff_list[stuff_count].item_id = stuff_item_id;
		stuff_list[stuff_count].num = 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}

	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0)
		{
			if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}
		}
	}
	m_module_mgr->GetKnapsack()->Put(ItemConfigData(target_item_id, true, 1), PUT_REASON_SMART_MOUNTS_SKILL_COMPOSE, NULL, NULL, 0, 0, false);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] convert[old:%d, new:%d] stuff[%d]", __FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), main_item_id, target_item_id, stuff_item_id);
}

void SmartMountsSystem::OnWearOrnaments(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.smart_mounts_list[index].level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}

	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (0 != smart_mounts.jewelry_data.is_active_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_FACE_SCORE_NOT_REPEAT_WEAR);
		return;
	}
	const SmartMountsSystemJewelryCfg * jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, 1);
	if(NULL == jewelry_cfg) return;

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)jewelry_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = jewelry_cfg->consume_list[i].item_id;
		stuff_con.num = jewelry_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}

	smart_mounts.jewelry_data.is_active_flag = 1;
	smart_mounts.jewelry_data.strength_level = 1;
	smart_mounts.is_use_advanced_flag = 1;		//穿戴时默认切换到进阶
	m_module_mgr->NoticeNum(noticenum::NT_SMART_MOUNTS_SYSTEM_WEAR_SUCC);
	this->ReCalcPetAttr(index);
	if (smart_mounts.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_base)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_WEAR_JEWELRY, smart_mounts.jewelry_data.strength_level);
	//此时骑乘的是该灵骑,则需要通知附近玩家
	if (m_param.ride_index == index)
	{
		EventHandler::Instance().OnRoleWearOrTakeOffSurface(m_module_mgr->GetRole());
		this->SendOtherInfo();
	}
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] jewelry_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.jewelry_data.strength_level, smart_mounts.energy_value);
}

bool SmartMountsSystem::OnTakeOffOrnaments(int index, bool is_notice)
{
	/*if (!this->IsValid(index)) return false;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return false;
	}
	if (m_param.smart_mounts_list[index].level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return false;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(m_param.smart_mounts_list[index].wear_ornaments);
	if (NULL == item_base)
	{
		m_param.smart_mounts_list[index].wear_ornaments = 0;
		return false;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPut(m_param.smart_mounts_list[index].wear_ornaments, 1))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	m_module_mgr->GetKnapsack()->Put(ItemConfigData(m_param.smart_mounts_list[index].wear_ornaments, true, 1), PUT_REASON_SMART_MOUNTS_SYSTEM);
	m_param.smart_mounts_list[index].wear_ornaments = 0;
	if (is_notice)
	{
		m_module_mgr->NoticeNum(noticenum::NT_SMART_MOUNTS_SYSTEM_ORNAMENTS_TAKE_OFF);
		this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_ORNAMENTS_CHANGE, m_param.smart_mounts_list[index].wear_ornaments);
	}*/
	return true;
}

void SmartMountsSystem::OnSetImage(int index, bool is_advance_image)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.smart_mounts_list[index].level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (0 == m_param.smart_mounts_list[index].jewelry_data.is_active_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_JEWELRY_NOT_ACTIVE);
		return;
	}
	m_param.smart_mounts_list[index].is_use_advanced_flag = is_advance_image ? 1 : 0;
	this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_SET_IMAGE, m_param.smart_mounts_list[index].is_use_advanced_flag);
	//此时骑乘的是该灵骑,则需要通知附近玩家
	if (m_param.ride_index == index)
	{
		EventHandler::Instance().OnRoleWearOrTakeOffSurface(m_module_mgr->GetRole());
		this->SendOtherInfo();
	}
}

void SmartMountsSystem::OnActiveEquip(int index, int equip_index)
{
	if (!this->IsValid(index) || equip_index < 0 || equip_index >= MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (0 != smart_mounts.equip_list[equip_index].is_active_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_EQUIP_ACTIVED);
		return;
	}

	//消耗道具
	const SmartMountsSystemEquipCfg * equip_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipCfg(base_cfg->equip_id_list[equip_index], 1);
	if (NULL == equip_cfg)
	{
		return;
	}
	if (m_module_mgr->GetRole()->GetLevel() < equip_cfg->role_level_limit)
	{	
		m_module_mgr->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
		return;
	}
	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)equip_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = equip_cfg->consume_list[i].item_id;
		stuff_con.num = equip_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	smart_mounts.equip_list[equip_index].is_active_flag = 1;
	smart_mounts.equip_list[equip_index].strength_level = 1;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_ACTIVE_EQUIP, equip_index, smart_mounts.equip_list[equip_index].strength_level);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] equip_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.equip_list[equip_index].strength_level, smart_mounts.energy_value);
}

void SmartMountsSystem::OnEquipLevelUp(int index, int equip_index)
{
	if (!this->IsValid(index) || equip_index < 0 || equip_index >= MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (0 == smart_mounts.equip_list[equip_index].is_active_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_EQUIP_NOT_ACTIVE);
		return;
	}

	//判断等级
	const SmartMountsSystemEquipCfg * equip_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipCfg(base_cfg->equip_id_list[equip_index], smart_mounts.equip_list[equip_index].strength_level + 1);
	int max_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipMaxLevelById(base_cfg->equip_id_list[equip_index]);
	if (smart_mounts.equip_list[equip_index].strength_level >= max_level || NULL == equip_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_EQUIP_MAX_LEVEL);
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < equip_cfg->role_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_level_not_engough, equip_cfg->role_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;
	}

	//消耗道具
	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)equip_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = equip_cfg->consume_list[i].item_id;
		stuff_con.num = equip_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}
	smart_mounts.equip_list[equip_index].strength_level++;
	//计算属性
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	//下发协议
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_EQUIP_LEVEL_UP, equip_index, smart_mounts.equip_list[equip_index].strength_level);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] equip_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.equip_list[equip_index].strength_level, smart_mounts.energy_value);
}

void SmartMountsSystem::OnJewelryStrength(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (0 == smart_mounts.jewelry_data.is_active_flag)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_JEWELRY_NOT_ACTIVE);
		return;
	}
	const SmartMountsSystemJewelryCfg * jewelry_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryCfg(base_cfg->jewelry_id, smart_mounts.jewelry_data.strength_level + 1);
	if (NULL == jewelry_cfg)
	{
		return;
	}
	int max_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetJewelryMaxLevelById(base_cfg->jewelry_id);
	if (smart_mounts.jewelry_data.strength_level >= max_level)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_JEWELRY_MAX_LEVEL);
		return;
	}

	//消耗道具
	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)jewelry_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = jewelry_cfg->consume_list[i].item_id;
		stuff_con.num = jewelry_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}

	smart_mounts.jewelry_data.strength_level++;
	this->ReCalcPetAttr(index);
	if (smart_mounts.energy_value >= LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg().energy_limit_base)
	{
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_JEWELRY_LEVEL_UP, smart_mounts.jewelry_data.strength_level);
	EventHandler::Instance().OnRoleStrSmartMountsJewelry(m_module_mgr->GetRole(), index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] jewelry_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.jewelry_data.strength_level, smart_mounts.energy_value);
}

void SmartMountsSystem::OnActiveHoly(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if(smart_mounts.holy_strength_level > 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_HOLY_ACTIVED);
		return;
	}
	const SmartMountsSystemHolyCfg * holy_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetHolyCfg(base_cfg->bicycle_holy, 1);
	if (NULL == holy_cfg)
	{
		return;
	}
	if (m_module_mgr->GetRole()->GetLevel() < holy_cfg->role_level_limit)
	{
		m_module_mgr->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
		return;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)holy_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = holy_cfg->consume_list[i].item_id;
		stuff_con.num = holy_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	smart_mounts.holy_strength_level = 1;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_ACTIVE_HOLY, smart_mounts.holy_strength_level, 0);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] holy_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.holy_strength_level, smart_mounts.energy_value);
}

void SmartMountsSystem::OnHolyLevelUp(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if(smart_mounts.holy_strength_level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_HOLY_NOT_ACTIVE);
		return;
	}
	const SmartMountsSystemHolyCfg * holy_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetHolyCfg(base_cfg->bicycle_holy, smart_mounts.holy_strength_level + 1);
	int max_holy_level = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetHolyMaxLevelById(base_cfg->bicycle_holy);
	if (smart_mounts.holy_strength_level >= max_holy_level || NULL == holy_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_HOLY_MAX_LEVEL);
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < holy_cfg->role_level_limit)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_smart_mounts_system_level_not_engough, holy_cfg->role_level_limit);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
		return;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)holy_cfg->consume_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = holy_cfg->consume_list[i].item_id;
		stuff_con.num = holy_cfg->consume_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}
	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (consume_list.count > 0 && !m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
		consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	smart_mounts.holy_strength_level += 1;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
	this->SendShowInfo(index, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_HOLY_LEVEL_UP, smart_mounts.holy_strength_level, 0);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] holy_level[%d] energy_value[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts.level, smart_mounts.holy_strength_level, smart_mounts.energy_value);
}

void SmartMountsSystem::OnRideSmartMounts(int index)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.smart_mounts_list[index].level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	m_param.ride_index = index;
	m_module_mgr->GetSurface()->WearSurface(SURFACE_TYPE_MOUNT, 0);
	this->SendOtherInfo();
}

void SmartMountsSystem::OnCancelRideSmartMounts()
{
	m_param.ride_index = -1;
	EventHandler::Instance().OnRoleWearOrTakeOffSurface(m_module_mgr->GetRole());
	this->SendOtherInfo();
}

void SmartMountsSystem::OnConvertItem(int need_convert_item_id, int target_item_id)
{
	const SmartMountsItemConvertCfg * convert_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetItemConvertCfg(need_convert_item_id);
	if(NULL == convert_cfg)
	{
		return;
	}

	if (NULL == ITEMPOOL->GetItem(target_item_id))
	{
		return;
	}
	if (convert_cfg->convert_item_list.end() == convert_cfg->convert_item_list.find(target_item_id) || need_convert_item_id == target_item_id)
	{
		return;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPut(target_item_id, 1, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	{
		stuff_list[stuff_count].item_id = convert_cfg->consume_item_id;
		stuff_list[stuff_count].num = 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}

	{
		stuff_list[stuff_count].item_id = need_convert_item_id;
		stuff_list[stuff_count].num = 1;
		stuff_list[stuff_count].buyable = false;
		stuff_count += 1;
	}

	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0)
		{
			if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}
		}
	}
	m_module_mgr->GetKnapsack()->Put(ItemConfigData(target_item_id, true, 1), PUT_REASON_SMART_MOUNTS_CONVERT_ITEM, NULL, NULL, 0, 0, false);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] convert[old:%d, new:%d] stuff[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), 
		need_convert_item_id, target_item_id, convert_cfg->consume_item_id);
}

void SmartMountsSystem::OnActiveConvertItem(int active_item_bag_column, int active_item_bag_index)
{
	const ItemGridData* item_grid = m_module_mgr->GetKnapsack()->GetItemGridData(active_item_bag_column, active_item_bag_index);
	if (NULL == item_grid)
	{
		return;
	}

	int need_item_id = item_grid->item_id;
	const SmartMountsActiveConvertCfg * convert_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetActiveConvertCfg(item_grid->item_id);
	if (NULL == convert_cfg)
	{
		return;
	}

	if (NULL == ITEMPOOL->GetItem(item_grid->item_id) || !this->IsValid(convert_cfg->smart_mounts_index))
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[convert_cfg->smart_mounts_index];
	if (smart_mounts_param.level <= 0)
	{
		return;
	}

	if (!m_module_mgr->GetKnapsack()->CheckForPut(convert_cfg->target_item_id, 1, false))
	{
		m_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	if(!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(active_item_bag_column, active_item_bag_index, 1, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	ItemConfigData target_item = ItemConfigData(convert_cfg->target_item_id, true, 1);
	m_module_mgr->GetKnapsack()->Put(target_item, PUT_REASON_SMART_MOUNTS_ACTIVE_CONVERT);
	m_module_mgr->GetKnapsack()->NotifyGetItemList(1, &target_item, GET_REASON_SMART_MOUNTS_ACTIVE_CONVERT);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] convert[%d] stuff[%d column:%d index:%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), 
		m_module_mgr->GetRole()->GetName(), convert_cfg->target_item_id, need_item_id, active_item_bag_column, active_item_bag_index);
}

void SmartMountsSystem::OnFinishBattle(int battle_mode, int pet_stat_num, long long * pet_unique_id_list)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM) || pet_stat_num <= 0 || NULL == pet_unique_id_list) return;

	//判断模式,以下这些不会消耗精力值
	switch (battle_mode)
	{
		//case BATTLE_MODE_NORMAL_MONSTER:
		case BATTLE_MODE_MINE_MONSTER:
		{	
			Team * m_team = m_module_mgr->GetRole()->GetMyTeam();
			if (NULL == m_team)
			{
				if (m_module_mgr->GetRole()->GetScene()->IsInStaticScene() && m_module_mgr->IsPatrolling())
				{
					return;
				}
			}
			else
			{
				Role * leader = m_team->GetLeaderRole();
				if (NULL != leader && leader->GetScene()->IsInStaticScene() && leader->GetRoleModuleManager()->IsPatrolling())
				{
					return;
				}
			}
		}
		break;
		case BATTLE_MODE_SINGLE_PVP_PRACTICE:
		case BATTLE_MODE_SINGLE_ARENA:
		case BATTLE_MODE_NORMAL_PRACTICE:
		case BATTLE_MODE_PLATFORM_BATTLE:
		case BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI:
		{
			return;
		}
		break;
	default:
		break;
	}

	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	for (int i = 0; i < pet_stat_num && i < ROLE_BATTLE_PET_NUM; i++)
	{
		std::map<unsigned long long, int >::iterator it = m_pet_dominance_map.find(pet_unique_id_list[i]);
		if(it == m_pet_dominance_map.end() || !this->IsValid(it->second)) continue;

		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(it->second);
		if (NULL == base_cfg) continue;

		SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[it->second];

		int last_energy_value = smart_mounts.energy_value;
		smart_mounts.energy_value -= base_cfg->energy_use;
		if (smart_mounts.energy_value < 0)
		{
			smart_mounts.energy_value = 0;
		}
		if ((last_energy_value >= other_cfg.energy_limit_skill && smart_mounts.energy_value < other_cfg.energy_limit_skill) || (last_energy_value >= other_cfg.energy_limit_base && smart_mounts.energy_value < other_cfg.energy_limit_base))
		{
			this->SendShowInfo(it->second, Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_ENERGY_VALUE, smart_mounts.energy_value, 0);
			m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
			this->ReCalcPetAttr(it->second);
			for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; k++)
			{
				if (-1 != smart_mounts.dominance_pet_index_list[k])
				{
					m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[k]);
				}
			}
		}
		else
		{
			this->SendRelatedInfo(it->second, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_ENERGY_CHANGE, smart_mounts.energy_value);
		}
	}

}

void SmartMountsSystem::GetUserSpecialEffectList(int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM) || NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(i);
		if (NULL == base_cfg || m_param.smart_mounts_list[i].level <= 0) continue;

		SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[i];
		if (smart_mounts_param.energy_value < other_cfg.energy_limit_skill) continue;

		/*const SmartMountsSkillUnlockCfg * unlock_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetUnlockCfg(smart_mounts_param.bicycle_id);
		if (NULL == unlock_cfg) continue;*/

		for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM; k++)
		{		
			if (0 == smart_mounts_param.equip_list[k].is_active_flag || 0 >= smart_mounts_param.equip_list[k].strength_level) continue;

			const SmartMountsSystemEquipCfg * equip_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetEquipCfg(base_cfg->equip_id_list[k], smart_mounts_param.equip_list[k].strength_level);
			if(NULL == equip_cfg) continue;

			//加面板的特效不收集入战斗
			const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(equip_cfg->equip_sp_id);
			if (NULL == spe_cfg) continue;

			bool is_has_battle_effect = false;
			for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
			{
				const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
				if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

				is_has_battle_effect = true;
				break;
			}

			if (!is_has_battle_effect)
			{
				continue;
			}

			if (*out_other_sp_num < max_other_sp_num)
			{
				out_other_sp_list[(*out_other_sp_num)++] = equip_cfg->equip_sp_id;
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#else
				gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
				return;
			}
		}

		/*for (int k = 0; k < MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM; k++)
		{
			int skill_id = unlock_cfg->exclusive_skill_id[k];
			int skill_level = smart_mounts_param.exclusive_level_list[k];
			if (skill_level > MAX_BATTLE_SKILL_LEVEL || skill_level <= 0) continue;

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
			if (NULL == passive_skill) continue;

			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid)
			{
				spid = passive_skill->level_spid_list[skill_level];
			}
			if (*out_other_sp_num < max_other_sp_num)
			{
				out_other_sp_list[(*out_other_sp_num)++] = spid;
			}
			else
			{
				gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_num[%d] ", __FUNCTION__, max_other_sp_num);
				return;
			}
		}*/
	}
}

void SmartMountsSystem::GetBaseSpecialEffectList(unsigned long long item_unique_id, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_SMART_MOUNTS_SYSTEM) || NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	std::vector<SkillPro> base_skill_list;
	this->GetBaseSkillList(item_unique_id, &base_skill_list);
	for (int i = 0; i < (int)base_skill_list.size(); i++)
	{
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(base_skill_list[i].skill_id);
		if (NULL == passive_skill) continue;

		// 灵骑技能是有技能等级的，注意！
		int skill_level = base_skill_list[i].skill_level;
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
		{
			spid = passive_skill->level_spid_list[skill_level];
		}
		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = spid;
		}
		else
		{
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_num[%d] ", __FUNCTION__, max_other_sp_num);
			return;
		}
	}
}

void SmartMountsSystem::SendAllInfo()
{
	Protocol::SCSmartMountsSystemAllInfo info;
	info.count = 0;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_NUM; i++)
	{
		if(m_param.smart_mounts_list[i].bicycle_id <= 0 || m_param.smart_mounts_list[i].level <= 0) continue;

		info.smart_mounts_list[info.count++] = m_param.smart_mounts_list[i];
	}

	int length = sizeof(info) - (MAX_SMART_MOUNTS_SYSTEM_NUM - info.count) * sizeof(info.smart_mounts_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void SmartMountsSystem::SendSingleInfo(int index)
{
	if(!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemSingleInfo info;
	info.index = index;
	info.smart_mounts = m_param.smart_mounts_list[index];

	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendSingleAttr(int index)
{
	if (!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemSingleAttr info;
	info.index = index;
	info.reserve_sh = 0;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; i++)
	{
		info.attr_list[i] = attr_list[index][i];
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendDominanceInfo(int index)
{
	if (!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemDominanceInfo info;
	info.index = index;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
	{
		info.dominance_pet_index_list[i] = m_param.smart_mounts_list[index].dominance_pet_index_list[i];
	}
	
	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendShowInfo(int index, int notice_type, short param1, short param2,  short param3 /*= 0*/, short param4 /*= 0*/)
{
	if (!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemShowInfo info;
	info.noitce_type = notice_type;
	info.index = index;
	info.param1 = param1;
	info.param2 = param2;
	info.param3 = param3;
	info.param4 = param4;
	
	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendRefineInfo(int index)
{
	if (!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemRefineSkillInfo info;
	info.index = index;
	info.is_refine_flag = m_param.smart_mounts_list[index].is_refine_flag;
	memcpy(info.new_base_skill_list, m_param.smart_mounts_list[index].new_base_skill_list, sizeof(info.new_base_skill_list));
	memcpy(info.base_skill_list, m_param.smart_mounts_list[index].base_skill_list, sizeof(info.base_skill_list));

	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendOtherInfo()
{
	Protocol::SCSmartMountsSystemOtherInfo info;
	info.is_finish_fun_guide_flag = m_param.is_finish_fun_guide_flag;
	info.ride_index = m_param.ride_index;
	if (info.ride_index >= 0 && info.ride_index < MAX_SMART_MOUNTS_SYSTEM_NUM)
	{
		info.is_use_advanced_flag = m_param.smart_mounts_list[info.ride_index].is_use_advanced_flag;
	}
	else
	{
		info.is_use_advanced_flag = 0;
	}
	info.reserve_ch = 0;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::SendRelatedInfo(int index, int notice_type, int param1)
{
	if (!this->IsValid(index)) return;

	Protocol::SCSmartMountsSystemRelatedInfo info;
	info.index = index;
	info.op_type = notice_type;
	info.param1 = param1;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void SmartMountsSystem::GmSetBaseSkill(int index, int skill_index, int skill_id)
{
	if (!this->IsValid(index)) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}

	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	if (smart_mounts_param.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM)
	{
		return;
	}
	if (smart_mounts_param.base_skill_list[skill_index].skill_level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_LOCK);
		return;
	}

	if (NULL == SkillPool::Instance().GetPassiveSkillCfg(skill_id))
	{
		return;
	}
	smart_mounts_param.base_skill_list[skill_index].skill_id = skill_id;
	this->SendSingleInfo(index);
	gamelog::g_Log_smart_mounts_system.printf(LL_INFO, "%s user[%d, %s] index[%d, %d] skill_id[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		index, smart_mounts_param.level, skill_id);
}

void SmartMountsSystem::GmSetEnergy(int index, int energy)
{
	if (!this->IsValid(index) || energy < 0) return;

	const SmartMountsSystemBaseCfg * base_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetBaseCfg(index);
	if (NULL == base_cfg)
	{
		return;
	}
	SmartMountsSystemSingleParam & smart_mounts = m_param.smart_mounts_list[index];
	if (smart_mounts.level <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED);
		return;
	}
	const SmartMountsSystemOtherCfg & other_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetOtherCfg();
	int last_energy_value = smart_mounts.energy_value;
	smart_mounts.energy_value = energy;
	if (smart_mounts.energy_value > base_cfg->bicycle_energy_max)
	{
		smart_mounts.energy_value = base_cfg->bicycle_energy_max;
	}
	if ((last_energy_value < other_cfg.energy_limit_base && smart_mounts.energy_value >= other_cfg.energy_limit_base) || (last_energy_value < other_cfg.energy_limit_skill && smart_mounts.energy_value >= other_cfg.energy_limit_skill))
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SMART_MOUNTS_SYSTEM, __FUNCTION__);
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM; i++)
		{
			if (-1 != smart_mounts.dominance_pet_index_list[i])
			{
				m_module_mgr->GetPet()->OnDominanceChange(smart_mounts.dominance_pet_index_list[i]);
			}
		}
	}
	this->SendRelatedInfo(index, Protocol::SCSmartMountsSystemRelatedInfo::INFO_TYPE_ENERGY_CHANGE, smart_mounts.energy_value);
}

const NorexItem * SmartMountsSystem::GetSkillItem(int item_id) const
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType())
	{
		return NULL;
	}

	const NorexItem * norex_item = (const NorexItem*)item_base;
	if (NorexItem::I_NOREX_ITEM_SMART_MOUNTS_SKILL != norex_item->GetUseType())
	{
		return NULL;
	}

	return norex_item;
}

bool SmartMountsSystem::OnActiveOrLevelUpUnlockSkill(int index, bool is_active, bool is_notice)
{
	if (!this->IsValid(index)) return false;

	SmartMountsSystemSingleParam & smart_mounts_param = m_param.smart_mounts_list[index];
	const SmartMountsSkillUnlockCfg * skill_unlock_cfg = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetUnlockCfg(smart_mounts_param.bicycle_id);
	if (NULL == skill_unlock_cfg)
	{
		return false;
	}
	bool ret = false;
	//技能改版,去除洗练.改成孔位解锁
	//short unlock_skill_id_list[MAX_SMART_MOUNTS_SYSTEM_CUR_SKILL_NUM] = { 0 };
	//short count = 0;
	for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_CUR_SKILL_NUM; i++)
	{
		if (i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM)
		{
			if (0 == smart_mounts_param.base_skill_list[i].is_unlock && smart_mounts_param.level >= skill_unlock_cfg->base_skill_unlock_level[i])
			{
				//UInt16 base_skill_id_list[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM] = { 0 };
				//this->GetSmartMountsBaseSkillIdList(index, base_skill_id_list, MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM);
				//smart_mounts_param.base_skill_list[i].skill_id = LOGIC_CONFIG->GetSmartMountsSystemConfig()->GetUnlockBaseSkill(skill_unlock_cfg->base_skill_group[i], base_skill_id_list, MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM);
				smart_mounts_param.base_skill_list[i].is_unlock = 1;
				//unlock_skill_id_list[count++] = smart_mounts_param.base_skill_list[i].skill_id;
				ret = true;
			}
		}
		/*else
		{
			int index = i - MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM;
			if (0 >= smart_mounts_param.exclusive_level_list[index] && smart_mounts_param.level >= skill_unlock_cfg->exclusive_skill_unlock_level[index])
			{
				smart_mounts_param.exclusive_level_list[index] = 1;
				unlock_skill_id_list[count++] = skill_unlock_cfg->exclusive_skill_id[index];
			}
		}*/
	}
	if (is_notice)
	{
		this->SendShowInfo(index, is_active ? Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_ACTIVE : Protocol::SCSmartMountsSystemShowInfo::SHOW_NOTICE_TYPE_LEVEL_UP, 
			smart_mounts_param.level, is_active ? 0 : smart_mounts_param.remain_skill_point/*, count, unlock_skill_id_list*/);
	}
	return ret;
}

void SmartMountsSystem::GetSmartMountsBaseSkillIdList(int index, UInt16 * base_skill_id_list, int count)
{
	if (!this->IsValid(index) || NULL == base_skill_id_list) return;

	for (int i = 0; i < count && i < MAX_SMART_MOUNTS_SYSTEM_CUR_SKILL_NUM; i++)
	{
		base_skill_id_list[i] = m_param.smart_mounts_list[index].base_skill_list[i].skill_id;
	}
}
