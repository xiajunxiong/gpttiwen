#include "smartmountssystemconfig.hpp"

#include "item/itempool.h"
#include "skill/skillpool.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

SmartMountsSystemConfig::SmartMountsSystemConfig()
{
	memset(m_reveal_list, 0, sizeof(m_reveal_list));
}

SmartMountsSystemConfig::~SmartMountsSystemConfig()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_reveal_list); i++)
	{
		for (int k = 0; k < ARRAY_ITEM_COUNT(m_reveal_list[i]); ++k)
		{
			if (NULL != m_reveal_list[i][k])
			{
				delete m_reveal_list[i][k];
			}
		}
	}
	memset(m_reveal_list, 0, sizeof(m_reveal_list));

	std::vector<SmartMountsItemConvertCfg>().swap(m_item_convert_list);
}

bool SmartMountsSystemConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("gain_way", InitGainWayCfg);				//获取途径配置读取需要放在基础配置读取之前
	LOAD_CONFIG("bicycle_equip", InitEquipCfg);	
	LOAD_CONFIG("bicycle_jewelry", InitJewelryCfg);			//装备与饰品需要在基础配置读取之前
	LOAD_CONFIG("bicycle_holy", InitHolyCfg);				//圣物需要在基础配置读取之前
	LOAD_CONFIG("bicycle_base", InitbaseCfg);				//基础配置读取需要放在其它配置读取之前
	LOAD_CONFIG("breach_attribute", InitBreachCfg);
	LOAD_CONFIG("bicycle_level", InitLevelCfg);				//需要放在突破属性配置读取之后
	LOAD_CONFIG("skill_score", InitSkillScoreCfg);			
	LOAD_CONFIG("skill_group", InitSkillGroupCfg);			//需要放在技能评分配置读取之后
	LOAD_CONFIG("Skill_hole_up", InitSkillUpCfg);
	LOAD_CONFIG("Skill_hole_open", InitSkillUnlockCfg);		//需要放在技能组与技能升级读取之后		
	LOAD_CONFIG("bicycle_reveal", InitRevealCfg);			//需要放在基础配置读取之后		
	LOAD_CONFIG("item_change", InitItemConvertCfg);			

	return true;
}

const SmartMountsSystemGainWayCfg * SmartMountsSystemConfig::GetGainWayCfg(int seq)
{
	std::map<int, SmartMountsSystemGainWayCfg>::iterator it = m_gain_way_map.find(seq);
	if(it == m_gain_way_map.end()) return NULL;

	return &it->second;
}

const SmartMountsSystemBaseCfg * SmartMountsSystemConfig::GetBaseCfg(int index)
{
	if(index < 0 || index >= MAX_SMART_MOUNTS_SYSTEM_NUM) return NULL;

	return &m_smart_mounts_cfg_list[index];
}

const SmartMountsSystemBreachCfg * SmartMountsSystemConfig::GetBreachCfg(int bicycle_id, int level)
{
	std::map<int, std::map<int, SmartMountsSystemBreachCfg> >::iterator it = breach_map.find(bicycle_id);
	if (it == breach_map.end())
	{
		return NULL;
	}
	std::map<int, SmartMountsSystemBreachCfg>::iterator level_it = it->second.find(level);
	if (level_it == it->second.end())
	{
		return NULL;
	}
	return &level_it->second;
}

const std::map<int, SmartMountsSystemBreachCfg>* SmartMountsSystemConfig::GetBreachCfg(int bicycle_id)
{
	std::map<int, std::map<int, SmartMountsSystemBreachCfg> >::iterator it = breach_map.find(bicycle_id);
	if (it == breach_map.end())
	{
		return NULL;
	}

	return &it->second;
}

int SmartMountsSystemConfig::GetMaxLevelById(int bicycle_id)
{
	std::map<int, std::map<int, SmartMountsSystemLevelCfg> >::iterator it = level_map.find(bicycle_id);
	if (it == level_map.end())
	{
		return 0;
	}
	if (it->second.empty())
	{
		return 0;
	}

	return (*it->second.rbegin()).first;
}

const SmartMountsSystemLevelCfg * SmartMountsSystemConfig::GetLevelCfg(int bicycle_id, int level)
{
	std::map<int, std::map<int, SmartMountsSystemLevelCfg> >::iterator it = level_map.find(bicycle_id);
	if (it == level_map.end()) return NULL;

	std::map<int, SmartMountsSystemLevelCfg>::iterator level_it = it->second.find(level);
	if(level_it == it->second.end()) return NULL;

	return &level_it->second;		
}

const SmartMountsSkillUnlockCfg * SmartMountsSystemConfig::GetUnlockCfg(int bicycle_id)
{
	std::map<int, SmartMountsSkillUnlockCfg>::iterator it = skill_unlock_map.find(bicycle_id);
	if(it == skill_unlock_map.end()) return NULL;

	return &it->second;
}

int SmartMountsSystemConfig::GetMaxSkillLevel(int bicycle_id, int skill_index, bool is_base_skill)
{
	int skill_up_key = this->GetSkillUpKeyCfg(bicycle_id, skill_index, is_base_skill ? SMART_MOUNTS_SKILL_TYPE_BASE : SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE);
	std::map<int, std::map<int, SmartMountsSkillUpCfg> >::iterator it = skill_up_map.find(skill_up_key);
	if (it == skill_up_map.end()) return NULL;

	if (it->second.empty())
	{
		return 0;
	}

	return (*it->second.rbegin()).first;
}

const SmartMountsSkillUpCfg * SmartMountsSystemConfig::GetSkillUpCfg(int bicycle_id, int skill_index, bool is_base_skill, int skill_level)
{
	int skill_up_key = this->GetSkillUpKeyCfg(bicycle_id, skill_index, is_base_skill ? SMART_MOUNTS_SKILL_TYPE_BASE : SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE);
	std::map<int, std::map<int, SmartMountsSkillUpCfg> >::iterator it = skill_up_map.find(skill_up_key);
	if(it == skill_up_map.end()) return NULL;

	std::map<int, SmartMountsSkillUpCfg>::iterator skill_level_it = it->second.find(skill_level);
	if(skill_level_it != it->second.end()) return &skill_level_it->second;

	return NULL;
}

const std::map<int, SmartMountsSkillUpCfg>* SmartMountsSystemConfig::GetSkillUpCfg(int bicycle_id, int skill_index, bool is_base_skill)
{
	int skill_up_key = this->GetSkillUpKeyCfg(bicycle_id, skill_index, is_base_skill ? SMART_MOUNTS_SKILL_TYPE_BASE : SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE);
	std::map<int, std::map<int, SmartMountsSkillUpCfg> >::iterator it = skill_up_map.find(skill_up_key);
	if (it == skill_up_map.end()) return NULL;

	return &it->second;
}

int SmartMountsSystemConfig::GetUnlockBaseSkill(int skill_group_id, UInt16 * now_has_skill_list, int count)
{
	std::map<int, SmartMountsSystemSkillGroupCfg>::iterator it = skill_map.find(skill_group_id);
	if(it == skill_map.end() || NULL == now_has_skill_list) return 0;

	int rate_count = it->second.rate_count;
	std::vector<SmartMountsSystemSkillGroupCfg::ItemCfg> rand_list(it->second.skill_list);
	for (int i = 0; i < count; i++)
	{
		if(0 == now_has_skill_list[i]) continue;

		for (std::vector<SmartMountsSystemSkillGroupCfg::ItemCfg>::iterator item_it = rand_list.begin(); item_it != rand_list.end();)
		{
			if (now_has_skill_list[i] == item_it->skill_id)
			{
				rate_count -= item_it->rate;
				item_it = rand_list.erase(item_it);
			}
			else
			{
				item_it++;
			}
		}
	}
	
	int rate_num = RandomNum(rate_count);
	for (int i = 0; i < (int)rand_list.size(); i++)
	{
		if (rate_num < rand_list[i].rate)
		{
			return rand_list[i].skill_id;
		}
		rate_num -= rand_list[i].rate;
	}

	return 0;
}

bool SmartMountsSystemConfig::GetRefineBaseSkill(int bicycle_id, UInt16 * lock_skill_id_list, int count, UInt16 * new_base_skill_list)
{
	const SmartMountsSkillUnlockCfg * unlock_cfg = this->GetUnlockCfg(bicycle_id);
	if(NULL == unlock_cfg || NULL == new_base_skill_list) return false;

	std::set<int> lock_skill_id_set;
	for (int i = 0; i < count && i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
	{
		if (0 != lock_skill_id_list[i])
		{
			lock_skill_id_set.insert(lock_skill_id_list[i]);
			new_base_skill_list[i] = lock_skill_id_list[i];
		}
	}
	for (int i = 0; i < count && i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM; i++)
	{
		std::map<int, SmartMountsSystemSkillGroupCfg>::iterator it = skill_map.find(unlock_cfg->base_skill_group[i]);
		if(it == skill_map.end()) return false;

		if(0 != new_base_skill_list[i]) continue;

		int rate_count = it->second.rate_count;
		std::vector<SmartMountsSystemSkillGroupCfg::ItemCfg> rand_list(it->second.skill_list);
		if (!lock_skill_id_set.empty())
		{
			for (std::vector<SmartMountsSystemSkillGroupCfg::ItemCfg>::iterator item_it = rand_list.begin(); item_it != rand_list.end();)
			{
				if (lock_skill_id_set.find(item_it->skill_id) != lock_skill_id_set.end())
				{
					rate_count -= item_it->rate;
					item_it = rand_list.erase(item_it);
				}
				else
				{
					item_it++;
				}
			}
		}

		int rate_num = RandomNum(rate_count);
		for (int k = 0; k < (int)rand_list.size(); k++)
		{
			if (rate_num < rand_list[k].rate)
			{
				new_base_skill_list[i] = rand_list[k].skill_id;
				lock_skill_id_set.insert(rand_list[k].skill_id);
				break;
			}
			rate_num -= rand_list[k].rate;
		}
	}
	return true;
}

const SmartMountsSystemEquipCfg * SmartMountsSystemConfig::GetEquipCfg(int equip_id, int equip_level)
{
	if(equip_level <= 0) return NULL;

	std::map<int, std::map<int, SmartMountsSystemEquipCfg> >::iterator it = m_equip_cfg.find(equip_id);
	if(m_equip_cfg.end() == it) return NULL;

	std::map<int, SmartMountsSystemEquipCfg>::iterator level_it = it->second.find(equip_level);
	if(level_it != it->second.end()) return &level_it->second;

	//找不到则使用就近配置
	for (std::map<int, SmartMountsSystemEquipCfg>::reverse_iterator reverse_it = it->second.rbegin(); reverse_it != it->second.rend(); reverse_it++)
	{
		if (equip_level >= reverse_it->first)
		{
			return &reverse_it->second;
		}
	}

	return NULL;
}

int SmartMountsSystemConfig::GetEquipMaxLevelById(int equip_id)
{
	std::map<int, std::map<int, SmartMountsSystemEquipCfg> >::iterator it = m_equip_cfg.find(equip_id);
	if (m_equip_cfg.end() == it) return 0;

	if (it->second.empty())
	{
		return 0;
	}

	return it->second.rbegin()->first;
}

const SmartMountsSystemJewelryCfg * SmartMountsSystemConfig::GetJewelryCfg(int jewelry_id, int jewelry_level)
{
	if (jewelry_level <= 0) return NULL;

	std::map<int, std::map<int, SmartMountsSystemJewelryCfg> >::iterator it = m_jewelry_cfg.find(jewelry_id);
	if (m_jewelry_cfg.end() == it) return NULL;

	std::map<int, SmartMountsSystemJewelryCfg>::iterator level_it = it->second.find(jewelry_level);
	if (level_it != it->second.end()) return &level_it->second;

	//找不到则使用就近配置
	for (std::map<int, SmartMountsSystemJewelryCfg>::reverse_iterator reverse_it = it->second.rbegin(); reverse_it != it->second.rend(); reverse_it++)
	{
		if (jewelry_level >= reverse_it->first)
		{
			return &reverse_it->second;
		}
	}

	return NULL;
}

int SmartMountsSystemConfig::GetJewelryMaxLevelById(int jewelry_id)
{
	std::map<int, std::map<int, SmartMountsSystemJewelryCfg> >::iterator it = m_jewelry_cfg.find(jewelry_id);
	if (m_jewelry_cfg.end() == it) return 0;

	if (it->second.empty())
	{
		return 0;
	}

	return it->second.rbegin()->first;
}

const SmartMountsSystemHolyCfg * SmartMountsSystemConfig::GetHolyCfg(int holy_id, int holy_level)
{
	if (holy_level <= 0) return NULL;

	std::map<int, std::map<int, SmartMountsSystemHolyCfg> >::iterator it = m_holy_cfg.find(holy_id);
	if (m_holy_cfg.end() == it) return NULL;

	std::map<int, SmartMountsSystemHolyCfg>::iterator level_it = it->second.find(holy_level);
	if (level_it != it->second.end()) return &level_it->second;

	//找不到则使用就近配置
	for (std::map<int, SmartMountsSystemHolyCfg>::reverse_iterator reverse_it = it->second.rbegin(); reverse_it != it->second.rend(); reverse_it++)
	{
		if (holy_level >= reverse_it->first)
		{
			return &reverse_it->second;
		}
	}

	return NULL;
}

int SmartMountsSystemConfig::GetHolyMaxLevelById(int holy_id)
{
	std::map<int, std::map<int, SmartMountsSystemHolyCfg> >::const_iterator it = m_holy_cfg.find(holy_id);
	if(it == m_holy_cfg.end() || it->second.empty()) return 0;

	return it->second.rbegin()->first;
}

const SmartMountsSystemRevealCfg * SmartMountsSystemConfig::GetSmartMountsSystemRevealCfg(int bicycle_id, int reveal_level)
{
	if (bicycle_id <= 0 || bicycle_id > MAX_SMART_MOUNTS_SYSTEM_NUM || reveal_level <= 0 || reveal_level > MAX_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_NUM)
	{
		return NULL;
	}

	return m_reveal_list[bicycle_id - 1][reveal_level - 1];
}

int SmartMountsSystemConfig::GetQualityById(int skill_id)
{
	std::map<int, int>::iterator it = m_skill_quality_map.find(skill_id);
	if(it == m_skill_quality_map.end()) return 0;

	return it->second;
}

int SmartMountsSystemConfig::GetSkillScoreCfg(int quality, int skill_level)
{
	std::map<int, int>::iterator it = m_skill_score_map.find(this->GetSkillQualityKey(quality, skill_level));
	if(it == m_skill_score_map.end()) return 0;

	return it->second;
}

int SmartMountsSystemConfig::GetSkillScoreCfgByIdAndLevel(int skill_id, int skill_level)
{
	return this->GetSkillScoreCfg(this->GetQualityById(skill_id), skill_level);;
}

const SmartMountsItemConvertCfg * SmartMountsSystemConfig::GetItemConvertCfg(int convert_item_id)const
{
	for (int i = 0; i < (int)m_item_convert_list.size(); ++i)
	{
		if (m_item_convert_list[i].convert_item_list.end() != m_item_convert_list[i].convert_item_list.find(convert_item_id))
		{
			return &m_item_convert_list[i];
		}
	}

	return NULL;
}

const SmartMountsActiveConvertCfg * SmartMountsSystemConfig::GetActiveConvertCfg(int active_item_id) const
{
	std::map<int, SmartMountsActiveConvertCfg>::const_iterator it = m_active_convert_map.find(active_item_id);
	if (it == m_active_convert_map.end())
	{
		return NULL;
	}

	return &it->second;
}

int SmartMountsSystemConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	if (!GetSubNodeValue(root_element, "energy_limit_base", m_other_cfg.energy_limit_base) || m_other_cfg.energy_limit_base < 0) return -1;
	if (!GetSubNodeValue(root_element, "energy_limit_skill", m_other_cfg.energy_limit_skill) || m_other_cfg.energy_limit_skill < 0) return -2;
	if (!GetSubNodeValue(root_element, "energy_up_coin", m_other_cfg.energy_up_coin) || m_other_cfg.energy_up_coin <= 0) return -3;
	if (!GetSubNodeValue(root_element, "energy_up", m_other_cfg.energy_up) || NULL == ITEMPOOL->GetItem(m_other_cfg.energy_up)) return -4;
	if (!GetSubNodeValue(root_element, "energy_up_number", m_other_cfg.energy_up_number) || m_other_cfg.energy_up_number <= 0) return -5;
	if (!GetSubNodeValue(root_element, "grow_reset_use", m_other_cfg.grow_reset_use) || NULL == ITEMPOOL->GetItem(m_other_cfg.grow_reset_use)) return -6;
	if (!GetSubNodeValue(root_element, "original_energy", m_other_cfg.init_energy) || m_other_cfg.init_energy < 0) return -7;
	if (!GetSubNodeValue(root_element, "skill_reset_use", m_other_cfg.skill_reset_use) || NULL == ITEMPOOL->GetItem(m_other_cfg.skill_reset_use)) return -8;
	if (!GetSubNodeValue(root_element, "skill_reset_number", m_other_cfg.skill_reset_number) || m_other_cfg.skill_reset_number <= 0) return -9;
	if (!GetSubNodeValue(root_element, "lock_skill_1", m_other_cfg.lock_skill_1) || m_other_cfg.lock_skill_1 <= 0) return -10;
	if (!GetSubNodeValue(root_element, "lock_skill_2", m_other_cfg.lock_skill_2) || m_other_cfg.lock_skill_2 <= 0) return -11;

	return 0;
}

int SmartMountsSystemConfig::InitGainWayCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0) return -1;
		if (NULL != this->GetGainWayCfg(seq))
		{
			return -2;
		}
		SmartMountsSystemGainWayCfg node_cfg;
		if (!GetSubNodeValue(root_element, "way_type", node_cfg.way_type) || node_cfg.way_type <= SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_BEGIN || node_cfg.way_type >= SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_MAX) return -3;
		if (!GetSubNodeValue(root_element, "param1", node_cfg.param1)) return -4;
		if (!GetSubNodeValue(root_element, "param2", node_cfg.param2)) return -5;

		UNSTD_STATIC_CHECK(SmartMountsSystemGainWayCfg::MAX_SMART_MOUNTS_SYSTEM_GAIN_WAY_TYPE_NUM == SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_MAX);
		switch (node_cfg.way_type)
		{
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_SMART_MOUNTS_LEVEL:
			{
				if(node_cfg.param1 < 0) return -6;
				if(node_cfg.param2 < 0 || node_cfg.param2 > MAX_ROLE_LEVEL) return -7;
			}
			break;
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_NUM_LEVEL_SMART_MOUNTS:
			{
				if (node_cfg.param1 <= 0 || node_cfg.param1 > MAX_SMART_MOUNTS_SYSTEM_NUM) return -8;
				if (node_cfg.param2 <= 0) return -9;
			}
			break;
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_SMART_MOUNTS_SKILL_LEVEL:
			case SmartMountsSystemGainWayCfg::GAIN_WAY_TYPE_HAS_MAX_SMART_MOUNTS_SKILL_LEVEL:
			{
				if (node_cfg.param1 <= 0 || node_cfg.param1 > MAX_SMART_MOUNTS_SYSTEM_NUM) return -10;
				if (node_cfg.param2 <= 0 || node_cfg.param2 > MAX_BATTLE_SKILL_LEVEL) return -11;
			}
			break;
		default:
			break;
		}
		m_gain_way_map[seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitbaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_index = -1;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= MAX_SMART_MOUNTS_SYSTEM_NUM) return -33;
		if (last_index + 1 != seq)
		{
			return -44;
		}
		last_index = seq;

		SmartMountsSystemBaseCfg node_cfg;
		if (!GetSubNodeValue(root_element, "bicycle_id", node_cfg.bicycle_id) || node_cfg.bicycle_id <= 0 || node_cfg.bicycle_id > MAX_SMART_MOUNTS_SYSTEM_NUM) return -1;
		if (!GetSubNodeValue(root_element, "activate_use", node_cfg.active_use_item_id) || (0 != node_cfg.active_use_item_id && NULL == ITEMPOOL->GetItem(node_cfg.active_use_item_id))) return -2;
		if (!GetSubNodeValue(root_element, "use_number", node_cfg.active_use_item_num) || node_cfg.active_use_item_num < 0) return -3;
		if (!GetSubNodeValue(root_element, "jewelry_id", node_cfg.jewelry_id)) return -4;
		if (!GetSubNodeValue(root_element, "give_energy", node_cfg.wear_nornaments_day_give) || node_cfg.wear_nornaments_day_give <= 0) return -5;
		if (!GetSubNodeValue(root_element, "bicycle_grow_begin", node_cfg.bicycle_grow_begin) || node_cfg.bicycle_grow_begin <= 0) return -6;
		if (!GetSubNodeValue(root_element, "grow_min_proportion", node_cfg.grow_min_proportion) || node_cfg.grow_min_proportion < 0 || node_cfg.grow_min_proportion > 100) return -7;
		if (!GetSubNodeValue(root_element, "bicycle_energy", node_cfg.bicycle_energy_max) || node_cfg.bicycle_energy_max <= 0 || node_cfg.bicycle_energy_max >= MAX_INT16) return -8;
		if (!GetSubNodeValue(root_element, "energy_use", node_cfg.energy_use) || node_cfg.energy_use <= 0) return -9;
		if (!GetSubNodeValue(root_element, "control_number", node_cfg.control_num) || node_cfg.control_num != MAX_SMART_MOUNTS_SYSTEM_DOMINANCE_PET_CUR_NUM) return -10;
		if (!GetSubNodeValue(root_element, "bicycle_hol", node_cfg.bicycle_holy)) return -20;

		std::map<int, std::map<int, SmartMountsSystemJewelryCfg> >::iterator it = m_jewelry_cfg.find(node_cfg.jewelry_id);
		if (it == m_jewelry_cfg.end() || it->second.empty())
		{
			return -11;
		}
		for (std::map<int, SmartMountsSystemJewelryCfg>::iterator jewelry_level_it = it->second.begin(); jewelry_level_it != it->second.end(); jewelry_level_it++)
		{
			if (jewelry_level_it->second.bicycle_grow <= node_cfg.bicycle_grow_begin)
			{
				return -12;
			}
		}
		std::map<int, std::map<int, SmartMountsSystemHolyCfg> >::iterator holy_it = m_holy_cfg.find(node_cfg.bicycle_holy);
		if (holy_it == m_holy_cfg.end() || holy_it->second.empty())
		{
			return -13;
		}

		if (node_cfg.active_use_item_id > 0)
		{	
			SmartMountsActiveConvertCfg convert_cfg;
			if (!GetSubNodeValue(root_element, "bicycle_jewelry", convert_cfg.target_item_id) ||  NULL == ITEMPOOL->GetItem(convert_cfg.target_item_id))
			{
				return -20000;
			}
			convert_cfg.smart_mounts_index = seq;
			m_active_convert_map[node_cfg.active_use_item_id] = convert_cfg;
		}

		TiXmlElement * gain_way_list = root_element->FirstChildElement("gain_way");
		if (NULL != gain_way_list)
		{
			TiXmlElement *node_element = gain_way_list->FirstChildElement("node");
			while (node_element)
			{
				int gain_way_seq = 0;
				if (!GetNodeValue(node_element, gain_way_seq) || NULL == this->GetGainWayCfg(gain_way_seq))
				{
					return -20002;
				}
				node_cfg.gain_way_list.push_back(gain_way_seq);
				node_element = node_element->NextSiblingElement();
			}
		}

		int count = 0;
		TiXmlElement * bicycle_equip_list = root_element->FirstChildElement("bicycle_equip");
		if (NULL != bicycle_equip_list)
		{
			TiXmlElement *node_element = bicycle_equip_list->FirstChildElement("node");
			while (node_element)
			{
				int equip_id = 0;
				if (!GetNodeValue(node_element, equip_id) || NULL == this->GetEquipCfg(equip_id, 1) || count >= MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM)
				{
					return -20003;
				}
				node_cfg.equip_id_list[count++] = equip_id;
				node_element = node_element->NextSiblingElement();
			}
		}
		if(count != MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM) return -66;

		if (m_bicycle_set.find(node_cfg.bicycle_id) != m_bicycle_set.end())
		{
			return -77;
		}

		m_smart_mounts_cfg_list[seq] = node_cfg;
		m_bicycle_set.insert(node_cfg.bicycle_id);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitBreachCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int bicycle_id = 0;
		if (!GetSubNodeValue(root_element, "bicycle_id", bicycle_id) || m_bicycle_set.find(bicycle_id) == m_bicycle_set.end()) return -1;
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level <= 0) return -2;

		if (NULL != this->GetBreachCfg(bicycle_id, level))
		{
			return -3;
		}

		SmartMountsSystemBreachCfg node_cfg;
		if (!GetSubNodeValue(root_element, "breach_consume", node_cfg.breach_consume_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.breach_consume_item_id)) return -4;
		if (!GetSubNodeValue(root_element, "breach_consume_num", node_cfg.breach_consume_item_num) || node_cfg.breach_consume_item_num <= 0) return -5;
		if (!GetSubNodeValue(root_element, "breach_chance", node_cfg.breach_rate) || node_cfg.breach_rate <= 0 || node_cfg.breach_rate > SmartMountsSystemBreachCfg::MAX_BREACH_RATE_COUNT) return -6;
		if (!GetSubNodeValue(root_element, "consume_return", node_cfg.consume_return) || node_cfg.consume_return <= 0 || node_cfg.consume_return >= node_cfg.breach_consume_item_num) return -7;

		char buf_type[64] = { 0 };
		char buf_value[64] = { 0 };
		int index = 0;
		for (int i = 1; i <= MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; i++, index++)
		{
			SNPRINTF(buf_type, sizeof(buf_type), "attr_type_%d", i);
			SNPRINTF(buf_value, sizeof(buf_value), "attr_add_%d", i);
			if (!GetSubNodeValue(root_element, buf_type, node_cfg.attr_type[index]) || node_cfg.attr_type[index] < BATTLE_ATTR_MIN || node_cfg.attr_type[index] >= BATTLE_ATTR_MAX) return -(100 + i);
			if (!GetSubNodeValue(root_element, buf_value, node_cfg.attr_value[index]) || node_cfg.attr_value[index] < 0) return -(150 + i);
		}

		breach_map[bicycle_id][level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int bicycle_id = 0;
		if (!GetSubNodeValue(root_element, "bicycle_id", bicycle_id) || m_bicycle_set.find(bicycle_id) == m_bicycle_set.end()) return -1;
	
		SmartMountsSystemLevelCfg node_cfg;
		if (!GetSubNodeValue(root_element, "level", node_cfg.level) || node_cfg.level <= 0) return -2;

		if (NULL != this->GetLevelCfg(bicycle_id, node_cfg.level))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "level_up_use", node_cfg.level_up_consume_item_id) || (0 != node_cfg.level_up_consume_item_id && NULL == ITEMPOOL->GetItem(node_cfg.level_up_consume_item_id))) return -4;
		if (!GetSubNodeValue(root_element, "use_number", node_cfg.level_up_consume_item_num) || (0 != node_cfg.level_up_consume_item_id && node_cfg.level_up_consume_item_num <= 0)) return -5;
		if (!GetSubNodeValue(root_element, "is_need_breach", node_cfg.is_need_breach)) return -6;
		if (!GetSubNodeValue(root_element, "rolelevel_limit", node_cfg.role_level_limit) || node_cfg.role_level_limit < 0) return -7;
		if (!GetSubNodeValue(root_element, "skill_point_gain", node_cfg.skill_point_gain) || node_cfg.skill_point_gain < 0) return -8;

		if (0 != node_cfg.is_need_breach)
		{
			if (NULL == this->GetBreachCfg(bicycle_id, node_cfg.level))
			{
				return -9;
			}
		}

		const char * attr_type_name[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM] = { "attr_type_1", "attr_type_2", "attr_type_3", "attr_type_4", "attr_type_5" };
		const char * attr_value_name[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM] =  { "attr_add_1", "attr_add_2", "attr_add_3", "attr_add_4", "attr_add_5" };
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM; i++)
		{
			if (!GetSubNodeValue(root_element, attr_type_name[i], node_cfg.attr_type[i]) || node_cfg.attr_type[i] < BATTLE_ATTR_MIN || node_cfg.attr_type[i] >= BATTLE_ATTR_MAX)
			{
				return -(100 + i);
			}
			if (!GetSubNodeValue(root_element, attr_value_name[i], node_cfg.attr_value[i]) || node_cfg.attr_value[i] < 0)
			{
				return -(150 + i);
			}
		}

		level_map[bicycle_id][node_cfg.level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitSkillUnlockCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int bicycle_id = 0;
		if (!GetSubNodeValue(root_element, "bicycle_id", bicycle_id) || m_bicycle_set.find(bicycle_id) == m_bicycle_set.end()) return -1;

		const char * open_level[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM/* + MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM*/] = { "openlevel_1", "openlevel_2", "openlevel_3"/*,
			 "openlevel_4", "openlevel_5" */ };
		const char * skill_group[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM/* + MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM*/] = { "skill_group_1", "skill_group_2", "skill_group_3"/*,
			"special_skill_1", "special_skill_2"*/ };
		SmartMountsSkillUnlockCfg node_cfg;
		for (int i = 0; i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM/* + MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM*/; i++)
		{
			//if (i < MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM)
			{
				if (!GetSubNodeValue(root_element, open_level[i], node_cfg.base_skill_unlock_level[i]) || node_cfg.base_skill_unlock_level[i] > this->GetMaxLevelById(bicycle_id))
				{
					return -(100 + i);
				}

				if (!GetSubNodeValue(root_element, skill_group[i], node_cfg.base_skill_group[i]) || skill_map.find(node_cfg.base_skill_group[i]) == skill_map.end())
				{
					return -(150 + i);
				}
			}
			/*else
			{
				int index = i - MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM;
				if (!GetSubNodeValue(root_element, open_level[i], node_cfg.exclusive_skill_unlock_level[index]) || node_cfg.exclusive_skill_unlock_level[index] > this->GetMaxLevelById(bicycle_id))
				{
					return -(100 + i);
				}
				if (!GetSubNodeValue(root_element, skill_group[i], node_cfg.exclusive_skill_id[index]) || NULL == SkillPool::Instance().GetPassiveSkillCfg(node_cfg.exclusive_skill_id[index]))
				{
					return -(150 + i);
				}
			}*/
		}

		skill_unlock_map[bicycle_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitSkillUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int bicycle_id = 0, skill_type = 0, skill_index = 0, skill_level = 0;
		if (!GetSubNodeValue(root_element, "bicycle_id", bicycle_id) || m_bicycle_set.find(bicycle_id) == m_bicycle_set.end()) return -1;
		if (!GetSubNodeValue(root_element, "type", skill_type) || skill_type <= SMART_MOUNTS_SKILL_TYPE_BEGIN || skill_type >= SMART_MOUNTS_SKILL_TYPE_MAX) return -2;
		if (!GetSubNodeValue(root_element, "Skill_hole_index", skill_index)) return -3;

		switch (skill_type)
		{
			case SMART_MOUNTS_SKILL_TYPE_BASE:
			{
				if(skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM) return - 4;
			}
			break;
			case SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE : 
				root_element = root_element->NextSiblingElement();
			continue;
			/*case SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE:
			{
				if (skill_index < 0 || skill_index >= MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM) return -5;
			}
			break;*/
		default:
			return -22;
		}

		if (!GetSubNodeValue(root_element, "Skill_hole_level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL) return -6;
		if (NULL != this->GetSkillUpCfg(bicycle_id, skill_index, skill_type == SMART_MOUNTS_SKILL_TYPE_BASE ? true : false, skill_level))
		{
			return -33;
		}

		SmartMountsSkillUpCfg node_cfg;
		if (!GetSubNodeValue(root_element, "skill_point_consume", node_cfg.need_consume_skill_point) || node_cfg.need_consume_skill_point < 0) return -7;
		if (!GetSubNodeValue(root_element, "bicycle_level_limit", node_cfg.bicycle_level_limit) || node_cfg.bicycle_level_limit <= 0 || 
			node_cfg.bicycle_level_limit > this->GetMaxLevelById(bicycle_id)) return -8;

		TiXmlElement * list_node = root_element->FirstChildElement("skillup_consume_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("skillup_consume_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				node_cfg.consume_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}
		int kill_up_key = this->GetSkillUpKeyCfg(bicycle_id, skill_index, skill_type);
		skill_up_map[kill_up_key][skill_level] = node_cfg;;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SmartMountsSystemConfig::InitSkillGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int skill_group_id;
		if (!GetSubNodeValue(root_element, "skill_group", skill_group_id) || skill_group_id <= 0) return -1;

		SmartMountsSystemSkillGroupCfg::ItemCfg node_cfg;
		if (!GetSubNodeValue(root_element, "skill_id", node_cfg.skill_id) || NULL == SkillPool::Instance().GetPassiveSkillCfg(node_cfg.skill_id)) return -2;
		if (!GetSubNodeValue(root_element, "value", node_cfg.rate) || node_cfg.rate <= 0) return -3;
		if (!GetSubNodeValue(root_element, "color", node_cfg.quality) || m_skill_quality_set.find(node_cfg.quality) == m_skill_quality_set.end()) return -4;
		
		skill_map[skill_group_id].rate_count += node_cfg.rate;
		skill_map[skill_group_id].skill_list.push_back(node_cfg);
		m_skill_quality_map[node_cfg.skill_id] = node_cfg.quality;
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int SmartMountsSystemConfig::InitSkillScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int quality = 0, skill_level = 0, skill_score = 0;
		if (!GetSubNodeValue(root_element, "color", quality) || quality <= 0) return -1;
		if (!GetSubNodeValue(root_element, "skill_level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "skill_score", skill_score) || skill_score <= 0) return -3;

		m_skill_score_map[this->GetSkillQualityKey(quality, skill_level)] = skill_score;
		m_skill_quality_set.insert(quality);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SmartMountsSystemConfig::InitEquipCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int equip_id = 0, equip_level = 0;
		if (!GetSubNodeValue(root_element, "equip_id", equip_id) || equip_id <= 0 || equip_id > MAX_SMART_MOUNTS_SYSTEM_EQUIP_NUM * MAX_SMART_MOUNTS_SYSTEM_NUM) return -1;
		if (!GetSubNodeValue(root_element, "equip_level", equip_level) || equip_level <= 0 || equip_level >= 1000) return -2;

		std::map<int, std::map<int, SmartMountsSystemEquipCfg> >::iterator it = m_equip_cfg.find(equip_id);
		if (it != m_equip_cfg.end() && it->second.end() != it->second.find(equip_level))
		{
			return - 3;
		}

		SmartMountsSystemEquipCfg & node_cfg = m_equip_cfg[equip_id][equip_level];
		if (!GetSubNodeValue(root_element, "equip_skill_id", node_cfg.equip_sp_id) || NULL == BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(node_cfg.equip_sp_id)) return -4;
		if (!GetSubNodeValue(root_element, "role_level_limit", node_cfg.role_level_limit) || node_cfg.role_level_limit <= 0 || node_cfg.role_level_limit > MAX_ROLE_LEVEL) return -5;
		if (!GetSubNodeValue(root_element, "equip_score", node_cfg.equip_score) || node_cfg.equip_score < 0) return -6;

		TiXmlElement * list_node = root_element->FirstChildElement("equip_consume_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("equip_consume_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				node_cfg.consume_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SmartMountsSystemConfig::InitJewelryCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int jewelry_id = 0, jewelry_level = 0;
		if (!GetSubNodeValue(root_element, "jewelry_id", jewelry_id) || jewelry_id <= 0 || jewelry_id > MAX_SMART_MOUNTS_SYSTEM_NUM) return -1;
		if (!GetSubNodeValue(root_element, "jewelry_level", jewelry_level) || jewelry_level <= 0 || jewelry_level > MAX_SMART_MOUNTS_SYSTEM_JEWELRY_LEVEL_NUM) return -2;

		std::map<int, std::map<int, SmartMountsSystemJewelryCfg> >::iterator it = m_jewelry_cfg.find(jewelry_id);
		if (it != m_jewelry_cfg.end() && it->second.end() != it->second.find(jewelry_level))
		{
			return -3;
		}

		SmartMountsSystemJewelryCfg & node_cfg = m_jewelry_cfg[jewelry_id][jewelry_level];
		if (!GetSubNodeValue(root_element, "bicycle_grow", node_cfg.bicycle_grow) || node_cfg.bicycle_grow <= 0) return -4;

		TiXmlElement * list_node = root_element->FirstChildElement("jewelry_consume_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("jewelry_consume_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				node_cfg.consume_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SmartMountsSystemConfig::InitHolyCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int holy_id = 0, holy_level = 0;
		if (!GetSubNodeValue(root_element, "holy_id", holy_id) || holy_id <= 0) return -1;
		if (!GetSubNodeValue(root_element, "holy_level", holy_level) || holy_level <= 0 || holy_level >= MAX_INT8) return -2;

		std::map<int, std::map<int, SmartMountsSystemHolyCfg> >::iterator it = m_holy_cfg.find(holy_id);
		if (it != m_holy_cfg.end() && it->second.end() != it->second.find(holy_level))
		{
			return -3;
		}

		SmartMountsSystemHolyCfg & node_cfg = m_holy_cfg[holy_id][holy_level];
		if (!GetSubNodeValue(root_element, "att_type", node_cfg.attr_type) || node_cfg.attr_type < BATTLE_ATTR_MIN || node_cfg.attr_type >= BATTLE_ATTR_MAX) return -4;
		if (!GetSubNodeValue(root_element, "att_value", node_cfg.attr_value) || node_cfg.attr_value <= 0) return -5;
		if (!GetSubNodeValue(root_element, "role_level_limit", node_cfg.role_level_limit) || node_cfg.role_level_limit <= 0 || node_cfg.role_level_limit > MAX_ROLE_LEVEL) return -6;

		TiXmlElement * list_node = root_element->FirstChildElement("holy_consume_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("holy_consume_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				node_cfg.consume_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SmartMountsSystemConfig::InitRevealCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_bicycle_id = 0, last_reveal_level = 0;;
	while (NULL != root_element)
	{
		int bicycle_id = 0;
		if (!GetSubNodeValue(root_element, "bicycle_id", bicycle_id) || m_bicycle_set.find(bicycle_id) == m_bicycle_set.end()) return -1;

		if (last_bicycle_id != bicycle_id)
		{
			last_bicycle_id = bicycle_id;
			last_reveal_level = 0;
		}

		int reveal_level = 0;
		if (!GetSubNodeValue(root_element, "reveal_level", reveal_level) || reveal_level <= 0 || 
			reveal_level > MAX_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_NUM || reveal_level != last_reveal_level + 1) 
		{
			return -2;
		}

		last_reveal_level = reveal_level;
		SmartMountsSystemRevealCfg * node_cfg = m_reveal_list[bicycle_id - 1][reveal_level - 1];
		if (NULL != node_cfg)
		{
			return -3;
		}
		else
		{
			node_cfg = new SmartMountsSystemRevealCfg();
			m_reveal_list[bicycle_id - 1][reveal_level - 1] = node_cfg;
		}

		if (!GetSubNodeValue(root_element, "level_up_use", node_cfg->consume_item_id) || NULL == ITEMPOOL->GetItem(node_cfg->consume_item_id))
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "use_number", node_cfg->consume_item_num) || node_cfg->consume_item_num <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "rolelevel_limit", node_cfg->role_level_limit))
		{
			return -6;
		}
		for (int i = 0; i < ARRAY_ITEM_COUNT(node_cfg->attr_list); ++i)
		{
			std::string buf_type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string buf_value = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, buf_type, node_cfg->attr_list[i].attr_type) || node_cfg->attr_list[i].attr_type < BATTLE_ATTR_MIN ||
				node_cfg->attr_list[i].attr_value >= BATTLE_ATTR_MAX)
			{
				return -(10 + i);
			}
			if (!GetSubNodeValue(root_element, buf_value, node_cfg->attr_list[i].attr_value) || node_cfg->attr_list[i].attr_value < 0)
			{
				return -(20 + i);
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SmartMountsSystemConfig::InitItemConvertCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_type = 0;
	while (NULL != root_element)
	{
		int consume_item_id = 0, convert_item_id = 0, type = 0;
		if (!GetSubNodeValue(root_element, "consume_item", consume_item_id) || NULL == ITEMPOOL->GetItem(consume_item_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "convert_item", convert_item_id) || NULL == ITEMPOOL->GetItem(convert_item_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "change_type", type))
		{
			return -3;
		}
		if (m_item_convert_list.empty() || last_type != type)
		{
			SmartMountsItemConvertCfg node_cfg;
			node_cfg.consume_item_id = consume_item_id;
			node_cfg.convert_item_list.insert(convert_item_id);
			m_item_convert_list.push_back(node_cfg);
		}
		else
		{
			SmartMountsItemConvertCfg & node_cfg = m_item_convert_list[(int)m_item_convert_list.size() - 1];
			if (node_cfg.consume_item_id != consume_item_id)
			{
				return -4;
			}
			node_cfg.convert_item_list.insert(convert_item_id);
		}

		last_type = type;
		root_element = root_element->NextSiblingElement();
	}

	if (m_item_convert_list.empty())
	{
		return -110;
	} 
	for (int i = 0; i < (int)m_item_convert_list.size(); ++i)
	{
		if ((int)m_item_convert_list[i].convert_item_list.size() < 2)
		{
			return -120;
		}
	}

	return 0;
}

int SmartMountsSystemConfig::GetSkillUpKeyCfg(int bicycle_id, int skill_index, int skill_type)
{
	return bicycle_id * BICYCLE_ID_BASE_NUM + SKILL_INDEX_BASE_NUM * skill_index + skill_type;
}

int SmartMountsSystemConfig::GetSkillQualityKey(int quality, int skill_level)
{
	return 1000 * quality + skill_level;
}