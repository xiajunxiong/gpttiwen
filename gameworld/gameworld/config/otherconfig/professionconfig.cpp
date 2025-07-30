#include "professionconfig.hpp"
#include "servercommon/servercommon.h"
#include "skill/skillpool.hpp"
#include "obj/character/role.h"
#include "task/taskpool.h"
#include "skill/skillpool.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/prestigedef.hpp"
#include "monster/monster_group_pool.hpp"
#include "item/itempool.h"


ProfessionConfig::ProfessionConfig()
{

}

ProfessionConfig::~ProfessionConfig()
{
	CareerAdvanceCfgMap().swap(m_career_advance_cfg_map);
	CareerAdvanceCfgMap().swap(m_advance_cond_cfg_map);

	for (std::map<int, std::vector<int> >::iterator it = m_prof_title_vec.begin(); it != m_prof_title_vec.end(); it++)
	{
		std::vector<int>().swap(it->second);
	}

	for (std::map<int, std::vector<CareerAdvanceGiveItemCfg> >::iterator it = m_change_prof_give_item_cfg.begin(); it != m_change_prof_give_item_cfg.end(); it++)
	{
		std::vector<CareerAdvanceGiveItemCfg>().swap(it->second);
	}

	ProfSkillListMap().swap(m_prof_skilllist_map);
}

bool ProfessionConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("career_skill_list", InitProfSkillCfg);
	LOAD_CONFIG("career_advancements", InitCareerAdvanceCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("career_change_equip", InitChangeProfGiveCfg);
	LOAD_CONFIG("skill_change", InitSkillChangeCfg);

	return true;
}

bool ProfessionConfig::CanLearnSkill(Role* role, UInt16 skill_id)
{
	int profession = role->GetProfession();
	if (profession <= 0)
	{
		return false;
	}

	int prof_base_type = profession / PROFESSION_BASE;
	int hash_key = GetHashKey(prof_base_type, skill_id);

	const ProfSkillCfg* cfg = this->GetProfSkillCfgByHashKey(hash_key);
	if (NULL == cfg)
	{
		return false;
	}

	if (role->GetLevel() < cfg->skill_learn_level)
	{
		return false;
	}

	return true;
}

int ProfessionConfig::GetMaxLearnSkillLevel(Role* role, UInt16 skill_id)
{
	int profession = role->GetProfession();
	if (profession <= 0)
	{
		return 0;
	}

	int prof_base_type = profession / PROFESSION_BASE;
	int hash_key = GetHashKey(prof_base_type, skill_id);

	const ProfSkillCfg* cfg = this->GetProfSkillCfgByHashKey(hash_key);
	if (NULL == cfg)
	{
		return 0;
	}

	return cfg->skill_level_limit;
}

int ProfessionConfig::GetSkillLeanLevel(Role * role, UInt16 skill)
{
	int profession = role->GetProfession();
	if (profession <= 0)
	{
		return 0;
	}

	int prof_base_type = profession / PROFESSION_BASE;
	int hash_key = GetHashKey(prof_base_type, skill);

	const ProfSkillCfg* cfg = this->GetProfSkillCfgByHashKey(hash_key);
	if (NULL == cfg)
	{
		return 0;
	}

	return cfg->skill_learn_level;
}

int ProfessionConfig::GetHashKey(int prof_base_type, UInt16 skill_id)
{
	return prof_base_type * PROF_HASH_BASE + skill_id;
}

void ProfessionConfig::GetAllTitleByBaseProf(int prof_base, std::vector<int> & out_list)
{
	std::map<int, std::vector<int> >::iterator prof_it = m_prof_title_vec.find(prof_base);
	if (m_prof_title_vec.end() != prof_it) out_list.assign(prof_it->second.begin(), prof_it->second.end());
}

std::vector<int> ProfessionConfig::GetUnlockSkillList(int prof_base, int role_level)
{
	std::vector<int> ret_vec;
	ProfSkillListMap::iterator it = m_prof_skilllist_map.find(prof_base);
	if (it == m_prof_skilllist_map.end()) return ret_vec;

	ret_vec.reserve(it->second.size());
	for (size_t i = 0; i < it->second.size(); ++i)
	{
		if (role_level >= it->second[i].learn_level)
		{
			ret_vec.push_back(it->second[i].skill_id);
		}
	}

	return ret_vec;
}

std::vector<int> ProfessionConfig::GetProfSkillList(int prof_base)
{
	std::vector<int> ret_vec;
	ProfSkillListMap::iterator it = m_prof_skilllist_map.find(prof_base);
	if (it == m_prof_skilllist_map.end()) return ret_vec;

	ret_vec.reserve(it->second.size());
	for (size_t i = 0; i < it->second.size(); ++i)
	{
		ret_vec.push_back(it->second[i].skill_id);	
	}

	return ret_vec;
}

std::set<int> ProfessionConfig::GetProfSkillSet(int prof_base)
{
	std::set<int> ret_set;
	ProfSkillListMap::iterator it = m_prof_skilllist_map.find(prof_base);
	if (it == m_prof_skilllist_map.end()) return ret_set;

	for (size_t i = 0; i < it->second.size(); ++i)
	{
		ret_set.insert(it->second[i].skill_id);
	}

	return ret_set;
}

const CareerAdvanceCfg* ProfessionConfig::GetCareerAdvanceCfg(int profession, int _order)
{
	CareerAdvanceCfgMap::iterator it = m_career_advance_cfg_map.find(profession);
	if (m_career_advance_cfg_map.end() == it)
	{
		return NULL;
	}

	if (_order < 0 || _order >= (int)it->second.size())
		return NULL;

	return &it->second[_order];
}

const std::vector<CareerAdvanceCfg> * ProfessionConfig::GetCareerAdvanceCfgByCondType(int _cond_type)
{
	CareerAdvanceCfgMap::iterator it = m_advance_cond_cfg_map.find(_cond_type);
	if (m_advance_cond_cfg_map.end() == it)
	{
		return NULL;
	}

	return &it->second;
}

int ProfessionConfig::GetCareerAdvanceStageSize(int _profession)
{
	CareerAdvanceCfgMap::iterator it = m_career_advance_cfg_map.find(_profession);
	if (m_career_advance_cfg_map.end() == it)
	{
		return -1;
	}

	return it->second.size();
}

const CareerAdvanceGiveItemCfg * ProfessionConfig::GetCareerAdvanceGiveItemCfg(int base_prof, int role_level)
{
	std::map<int, std::vector<CareerAdvanceGiveItemCfg> >::iterator iter = m_change_prof_give_item_cfg.find(base_prof);
	if (iter == m_change_prof_give_item_cfg.end())
	{
		return NULL;
	}

	const std::vector<CareerAdvanceGiveItemCfg> & vec = iter->second;

	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (vec[i].role_level_min <= role_level && role_level <= vec[i].role_level_max)
		{
			return &vec[i];
		}
	}

	return NULL;
}

const ProfOtherCfg& ProfessionConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int ProfessionConfig::InitProfSkillCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		short prof_base_type = 0;
		if (!GetSubNodeValue(dataElement, "job_type", prof_base_type) || prof_base_type <= 0 || prof_base_type >= PROFESSION_TYPE_NUM)
		{
			return -1;
		}

		UInt16 skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id))
		{
			return -2;
		}

		int hash_key = GetHashKey(prof_base_type, skill_id);
		if (m_prof_skill_map.end() != m_prof_skill_map.find(hash_key))
		{
			return -2000;
		}

		ProfSkillCfg cfg;
		cfg.prof_base_type = prof_base_type;
		cfg.skill_id = skill_id;

		short proficient_type = 0;
		if (!GetSubNodeValue(dataElement, "skill_type", proficient_type) || proficient_type < 0 || proficient_type > 2)
		{
			return -3;
		}
		cfg.proficient_type = proficient_type;


		int max_skill_level = 0;

		if (2 != proficient_type) // 若非生活技能
		{
			const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
			if (NULL == skill_cfg)
			{
				const GamePassiveSkillCfg* passive_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
				if (NULL == passive_cfg)
				{
					return -1001;
				}
				else
				{
					max_skill_level = 1;
				}
			}
			else
			{
				max_skill_level = skill_cfg->GetMaxLevel();
			}		
		}
		else // 若是生活技能
		{
			const LifeSkillCfg* life_skill_cfg = SkillPool::Instance().GetLifeSkillById(skill_id);
			if (NULL == life_skill_cfg)
			{
				return -1002;
			}
			max_skill_level = life_skill_cfg->max_level;
		}

		short skill_level_limit = 0;
		if (!GetSubNodeValue(dataElement, "skill_level_limit", skill_level_limit) || skill_level_limit <= 0)
		{
			return -4;
		}
#ifndef _DEBUG
		if (cfg.skill_level_limit > max_skill_level)
		{
			return -4333;
		}
#endif
		cfg.skill_level_limit = skill_level_limit;

		short skill_learn_level = 0;
		if (!GetSubNodeValue(dataElement, "skill_learn_level", skill_learn_level) || skill_learn_level < 0 || skill_learn_level > MAX_ROLE_LEVEL)
		{
			return -5;
		}
		cfg.skill_learn_level = skill_learn_level;

		m_prof_skill_map.insert(ProfSkillCfgMap::value_type(hash_key, cfg));

		if (0 == proficient_type)
		{
			ProfSkillLearnLevelCfg c;
			c.skill_id = cfg.skill_id;
			c.learn_level = cfg.skill_learn_level;
			m_prof_skilllist_map[cfg.prof_base_type].push_back(c);
		}	

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ProfessionConfig::InitCareerAdvanceCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_title_id = 0;
	while (dataElement)
	{
		int profession_id = 0;
		if (!GetSubNodeValue(dataElement, "job_id", profession_id) || profession_id <= 0)
		{
			return -1;
		}
		if (profession_id / PROFESSION_BASE <= 0 || profession_id / PROFESSION_BASE >= PROFESSION_TYPE_NUM)
		{
			return -2;
		}
		if (profession_id % PROFESSION_BASE <= 0 || profession_id % PROFESSION_BASE > PROFESSION_ADVANCE_TIMES)
		{
			return -3;
		}

		static CareerAdvanceCfg cfg;
		cfg.profession_id = profession_id;

		if (!GetSubNodeValue(dataElement, "pre_job", cfg.pre_profession) || cfg.pre_profession < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "order", cfg.order) || cfg.order < 0 || cfg.order >= MAX_ADVANCE_CAREER_SUB_COUNT)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "job_seq", cfg.job_seq) || cfg.job_seq <= 0)
		{
			return -9999;
		}

		if (!GetSubNodeValue(dataElement, "job_type", cfg.prof_base) || cfg.prof_base <= 0 || cfg.prof_base >= PROFESSION_TYPE_NUM)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "is_broadcast", cfg.is_broadcast))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "need_level", cfg.need_level) || cfg.need_level < 0 || cfg.need_level > MAX_ROLE_LEVEL)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "cond_type", cfg.cond_type) || cfg.cond_type < ADVANCE_COND_TYPE_NO || cfg.cond_type >= ADVANCE_COND_TYPE_MAX)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "cond_param1", cfg.cond_param1) || cfg.cond_param1 < 0)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "cond_param2", cfg.cond_param2) || cfg.cond_param2 < 0)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "pre_task", cfg.pre_task) || (cfg.pre_task > 0 && NULL == TASKPOOL->GetTaskCfgByID(cfg.pre_task)))
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "pre_title", cfg.pre_title))
		{
			return -13;
		}

		if (!GetSubNodeValue(dataElement, "pre_village", cfg.pre_village) || cfg.pre_village < 0 || cfg.pre_village >= PRESTIGE_VILLAGE_MAX_NUM)
		{
			return -1401;
		}

		if (!GetSubNodeValue(dataElement, "pre_relation", cfg.pre_relation) || cfg.pre_relation < 0)
		{
			return -1402;
		}

		if (!GetSubNodeValue(dataElement, "juanzeng_up", cfg.juanzeng_up_limit) || cfg.juanzeng_up_limit <= 0)
		{
			return -1403;
		}

		if (!GetSubNodeValue(dataElement, "shengwangka_up", cfg.shengwangka_up_limit) || cfg.shengwangka_up_limit <= 0)
		{
			return -1404;
		}

		if (!GetSubNodeValue(dataElement, "monster_group", cfg.monster_group) || (0 != cfg.monster_group && NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.monster_group)))
		{
			return -15;
		}

		if (0 != cfg.pre_title)
		{
			CheckResourceCenter::Instance().GetTitleCheck()->Add(cfg.pre_title, __FUNCTION__);
		}

		if (!GetSubNodeValue(dataElement, "pre_score", cfg.pre_score) || cfg.pre_score < 0)
		{
			return -16;
		}

		if (!GetSubNodeValue(dataElement, "need_bind_coin", cfg.need_bind_coin) || cfg.need_bind_coin < 0)
		{
			return -17;
		}

		if (!GetSubNodeValue(dataElement, "image", cfg.image) || cfg.image < 0)
		{
			return -18;
		}

		if (!GetSubNodeValue(dataElement, "get_title", cfg.reward_title))
		{
			return -19;
		}
		CheckResourceCenter::Instance().GetTitleCheck()->Add(cfg.reward_title, __FUNCTION__);

		if (!GetSubNodeValue(dataElement, "max_equip_level", cfg.max_equip_level) || cfg.max_equip_level <= 0)
		{
			return -20;
		}

		if (!GetSubNodeValue(dataElement, "max_skill_level", cfg.max_skill_level) || cfg.max_skill_level <= 0 || cfg.max_skill_level > MAX_BATTLE_SKILL_LEVEL)
		{
			return -21;
		}

		TiXmlElement* skill_id = dataElement->FirstChildElement("skill_id");
		if (NULL == skill_id) return -22;
		int count = 0;
		TiXmlElement * cfg_skill_id = skill_id->FirstChildElement("node");
		while (NULL != cfg_skill_id)
		{
			if (count >= PROFESSION_SKILL_INIT_MAX_NUM)
			{
				return -2201;
			}

			if (!GetNodeValue(cfg_skill_id, cfg.init_skill_id[count]))
			{
				return -(2202 + count);
			}
			++count;
			cfg_skill_id = cfg_skill_id->NextSiblingElement();
		}
		cfg.init_skill_num = count > PROFESSION_SKILL_INIT_MAX_NUM ? PROFESSION_SKILL_INIT_MAX_NUM : count;

		do
		{
			TiXmlElement* skill_id = dataElement->FirstChildElement("unlock_skill_id");
			if (NULL == skill_id) break;
			TiXmlElement * cfg_skill_id = skill_id->FirstChildElement("node");
			while (NULL != cfg_skill_id)
			{
				int skill = 0;
				if (!GetNodeValue(cfg_skill_id, skill))
				{
					return -(3303 + count);
				}
				cfg.unlock_skill_set.insert(skill);
				cfg_skill_id = cfg_skill_id->NextSiblingElement();
			}
		} while (0);

		if (!GetSubNodeValue(dataElement, "job_name", cfg.job_name))
		{
			return -23;
		}

		char str[20][20] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu",
			"bisha","jianren","fanji","mingzhong","shanduo",
			"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan",
			"mogong","mokang", "bisha_dmg" };
		int attr_id[20] = {
			BATTLE_ATTR_MAX_HP,
			BATTLE_ATTR_MAX_MP,
			BATTLE_ATTR_ATTACK,
			BATTLE_ATTR_DEFENSE,
			BATTLE_ATTR_AGILITY,
			BATTLE_ATTR_MENTAL,
			BATTLE_ATTR_RECOVERY,

			BATTLE_ATTR_CRITICAL,
			BATTLE_ATTR_CRITICAL_DEC_VALUE,
			BATTLE_ATTR_COUNTER_ATTACK,
			BATTLE_ATTR_HIT,
			BATTLE_ATTR_DODGE,

			BATTLE_ATTR_ANTI_POISON,
			BATTLE_ATTR_ANTI_PETRIFY,
			BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK,
			BATTLE_ATTR_ANTI_CHAOS,

			BATTLE_ATTR_MAGIC_ATTACK,
			BATTLE_ATTR_MAGIC_DEFENSE,
			BATTLE_ATTR_CRITICAL_INC_VALUE
		};

		for (int i = 0; i < ARRAY_LENGTH(attr_id); ++i)
		{
			if (!GetSubNodeValue(dataElement, str[i], cfg.m_attrs[attr_id[i]]) || cfg.m_attrs[attr_id[i]] < 0)
				return -(2400 + i);
		}

		if (!GetSubNodeValue(dataElement, "all_sp_bonus", cfg.all_sp_bonus), cfg.all_sp_bonus < 0)
		{
			return -25;
		}

		if (!GetSubNodeValue(dataElement, "change_color_coin", cfg.change_color_coin), cfg.change_color_coin < 0)
		{
			return -26;
		}

		if (!GetSubNodeValue(dataElement, "change_item", cfg.change_item_id), cfg.change_item_id < 0 || NULL == ITEMPOOL->GetItem(cfg.change_item_id))
		{
			return -27;
		}

		if (!GetSubNodeValue(dataElement, "change_item_num", cfg.change_item_num), cfg.change_item_num < 0)
		{
			return -28;
		}

		if (cfg.reward_title > 0 && last_title_id != cfg.reward_title)
		{
			int base_prof = profession_id / PROFESSION_BASE;
			std::map<int, std::vector<int> >::iterator prof_it = m_prof_title_vec.find(base_prof);
			if (m_prof_title_vec.end() != prof_it)
			{
				prof_it->second.push_back(cfg.reward_title);
			}
			else
			{
				std::vector<int> tmp_title; tmp_title.push_back(cfg.reward_title);
				m_prof_title_vec[base_prof] = tmp_title;
			}
			last_title_id = cfg.reward_title;
		}

		if (cfg.monster_group != 0 && cfg.pre_profession != 0)
			return -29;

		std::vector <CareerAdvanceCfg> & temp_vec = m_career_advance_cfg_map[profession_id];
		if (cfg.order != temp_vec.size())
		{
			return -30;
		}

		if (!temp_vec.empty())
		{
			const CareerAdvanceCfg& last_cfg = temp_vec[temp_vec.size() - 1];
			std::set<int>::const_iterator it = last_cfg.unlock_skill_set.begin();
			for (; it != last_cfg.unlock_skill_set.end(); ++it)
			{
				cfg.unlock_skill_set.insert(*it);
			}
		}
		temp_vec.push_back(cfg);	

		std::vector <CareerAdvanceCfg> & temp_cond_vec = m_advance_cond_cfg_map[cfg.cond_type];
		temp_cond_vec.push_back(cfg);

		if (0 != cfg.pre_title)
		{
			CheckResourceCenter::Instance().GetTitleCheck()->Add(cfg.pre_title, __FUNCTION__);
		}
		CheckResourceCenter::Instance().GetTitleCheck()->Add(cfg.reward_title, __FUNCTION__);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ProfessionConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	if (!GetSubNodeValue(dataElement, "transfer_cost_min", m_other_cfg.change_career_cost_min))
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "transfer_cost_max", m_other_cfg.change_career_cost_max))
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "magnification", m_other_cfg.change_cost_ratio_per_time))
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "reset", m_other_cfg.ver))
	{
		return -4;
	}

	return 0;
}

int ProfessionConfig::InitChangeProfGiveCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		int job_type = 0;
		if (!GetSubNodeValue(dataElement, "job_type", job_type))
		{
			return -__LINE__;
		}
		
		CareerAdvanceGiveItemCfg cfg;
		if (!GetSubNodeValue(dataElement, "min_level", cfg.role_level_min))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "max_level", cfg.role_level_max))
		{
			return -__LINE__;
		}

		TiXmlElement * gift_list = dataElement->FirstChildElement("transfer_gift_list");
		if (NULL == gift_list) return -101;
		TiXmlElement * item_list = gift_list->FirstChildElement("transfer_gift");
		while (NULL != item_list)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_list))
			{
				return -102;
			}

			cfg.change_prof_item.push_back(item);
			item_list = item_list->NextSiblingElement();
		}

		m_change_prof_give_item_cfg[job_type].push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ProfessionConfig::InitSkillChangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	while (dataElement)
	{
		int skill_id = 0, skill_level = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id) || NULL == SkillPool::Instance().GetSkillCfg(skill_id))
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(dataElement, "level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL)
		{
			return -__LINE__;
		}
		SkillChangeCfg::SkillChangeLevelCfg node_cfg; 
		if (!GetSubNodeValue(dataElement, "coin_consume_all", node_cfg.coin_consume_all) || node_cfg.coin_consume_all < 0)
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(dataElement, "bind_coin_consume", node_cfg.bind_coin_consume) || node_cfg.bind_coin_consume < 0)
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(dataElement, "add_exp", node_cfg.add_exp) || node_cfg.add_exp < 0)
		{
			return -__LINE__;
		}

		m_skill_change_map[skill_id].skill_list[skill_level] = node_cfg;
		m_skill_change_map[skill_id].max_level = GetMax(m_skill_change_map[skill_id].max_level, skill_level);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

const ProfSkillCfg* ProfessionConfig::GetProfSkillCfgByHashKey(int hash_key)
{
	ProfSkillCfgMap::iterator it = m_prof_skill_map.find(hash_key);
	if (it == m_prof_skill_map.end())
	{
		return NULL;
	}

	return &it->second;
}

bool ProfessionConfig::IsNeedReturn(int skill_id)
{
	if (m_skill_change_map.end() == m_skill_change_map.find(skill_id))
	{
		return false;
	}
	return true;
}

int ProfessionConfig::GetSkillChangeReturn(int skill_id, int skill_level, int skill_exp)
{
	if(skill_level <= 0) return 0;
	std::map<int, SkillChangeCfg>::const_iterator it = m_skill_change_map.find(skill_id);
	if(it == m_skill_change_map.end()) return 0;

	const SkillChangeCfg::SkillChangeLevelCfg & skill_level_cfg = it->second.skill_list[skill_level];
	int back_coin_count = skill_level_cfg.coin_consume_all;;
	if (skill_level != it->second.max_level && skill_exp > 0 && skill_level_cfg.add_exp > 0)
	{
		back_coin_count += (skill_exp / skill_level_cfg.add_exp) * skill_level_cfg.bind_coin_consume;
	}

	return back_coin_count;
}

