#include "skillpool.hpp"
#include "servercommon/serverconfig/configstruct.h"
#include "servercommon/attributedef.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/professionconfig.hpp"

SkillPool *g_skill_pool = NULL;

SkillPool::SkillPool()
{
	memset(m_skill_list, 0, sizeof(m_skill_list));
	memset(m_attr_type_rand_map, 0, sizeof(m_attr_type_rand_map));
}

SkillPool::~SkillPool()
{
	static const int SKILL_LIST_SIZE = ARRAY_ITEM_COUNT(m_skill_list);
	for (int i = 0; i < SKILL_LIST_SIZE; ++i)
	{
		if (NULL != m_skill_list[i])
		{
			delete m_skill_list[i];
			m_skill_list[i] = NULL;
		}
	}
}

SkillPool& SkillPool::Instance()
{
	if (NULL == g_skill_pool)
	{
		g_skill_pool = new SkillPool();
	}
	return *g_skill_pool;
}

bool SkillPool::Init(const std::string &skill_dir_name, std::string *err)
{
	if (!this->InitSkill(skill_dir_name + "single_skill.xml", err, BATTLE_SKILL_TYPE_SINGLE) ||
		!this->InitSkill(skill_dir_name + "combo_skill.xml", err, BATTLE_SKILL_TYPE_COMBO) ||
		!this->InitSkill(skill_dir_name + "power_skill.xml", err, BATTLE_SKILL_TYPE_POWER) ||
		!this->InitSkill(skill_dir_name + "super_power_skill.xml", err, BATTLE_SKILL_TYPE_SUPER_POWER) ||
		!this->InitSkill(skill_dir_name + "random_skill.xml", err, BATTLE_SKILL_TYPE_RANDOM) || 
		!this->InitSkill(skill_dir_name + "line_skill.xml", err, BATTLE_SKILL_TYPE_LINE))
	{
		return false;
	}

	if (!this->InitLifeSkill(skill_dir_name + "life_skill.xml", err))
	{
		return false;
	}

	if (!this->InitPassiveSkill(skill_dir_name + "passive_skill.xml", err))
	{
		return false;
	}

	return true;
}

bool SkillPool::Reload(const std::string &skill_dir_name, std::string *err)
{
	SkillPool *temp = new SkillPool();

	if (!temp->Init(skill_dir_name.c_str(), err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_skill_pool) delete g_skill_pool;

	g_skill_pool = temp;

	return true;
}

bool SkillPool::RandAttr(int life_skill_type, int skill_lv, std::vector<AttrInfo> & out_list)
{
	if (life_skill_type < 0 || life_skill_type >= LIFE_SKILL_TYPE_MAX || skill_lv <= 0) return false;

	const LifeSkillCfg & skill_info = m_life_skill_list[life_skill_type];
	if (skill_lv > skill_info.max_level || skill_lv > MAX_LIFE_SKILL_LEVEL) return false;

	const LifeSkillCfg::SkillParam & skill_param = skill_info.skill_param[skill_lv];

	int attr_num = 0, num_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	std::map<int, std::map<int, int> >::iterator num_group_it = m_attr_numb_rand_map.find(skill_param.param3);
	if (m_attr_numb_rand_map.end() == num_group_it) return false;

	for (std::map<int, int>::iterator num_loop = num_group_it->second.begin(); num_loop != num_group_it->second.end(); ++num_loop)
	{
		if (num_prob < num_loop->second)
		{
			attr_num = num_loop->first;
			break;
		}

		num_prob -= num_loop->second;
	}

	for (int rand_times = 0; rand_times < attr_num; ++rand_times)
	{
		AttrInfo attr_pair; attr_pair.attr_type = attr_pair.attr_value = 0;
		if (!this->RandSingleAttr(skill_param.param4, skill_lv, &attr_pair)) return false;
		out_list.push_back(attr_pair);
	}

	return true;
}

const SkillCfg* SkillPool::GetSkillCfg(UInt16 skill_id)
{
	if (skill_id >= MAX_BATTLE_SKILL_ID)
	{
		return NULL;
	}

	return m_skill_list[skill_id];
}

const LifeSkillCfg* SkillPool::GetLifeSkillCfg(int life_skill_type)
{
	UNSTD_STATIC_CHECK(LIFE_SKILL_TYPE_MIN >= 0);
	if (life_skill_type < LIFE_SKILL_TYPE_MIN || life_skill_type >= LIFE_SKILL_TYPE_MAX)
	{
		return NULL;
	}

	return &m_life_skill_list[life_skill_type];
}

const LifeSkillCfg* SkillPool::GetLifeSkillById(UInt16 life_skill_id)
{
	UNSTD_STATIC_CHECK(LIFE_SKILL_TYPE_MIN >= 0);
	for (int i = LIFE_SKILL_TYPE_MIN; i < LIFE_SKILL_TYPE_MAX; ++i)
	{
		if (m_life_skill_list[i].life_skill_id == life_skill_id)
		{
			return &m_life_skill_list[i];
		}
	}

	return NULL;
}

const GamePassiveSkillCfg * SkillPool::GetPassiveSkillCfg(short skill_id)
{
	if (skill_id < 0 || skill_id >= MAX_BATTLE_SKILL_ID) return NULL;
	GamePassiveSkillCfgMap::iterator it = m_passive_skill_cfg_map.find(skill_id);
	if (it == m_passive_skill_cfg_map.end()) return NULL;

	return &it->second;
}

long long SkillPool::GetSkillGoBackRetCoin(int skill_id, int skill_level, int cur_level_exp, int profession, int advance_sub_level, int go_back_level /* = 1 */)
{
	//0->1不需要消耗铜币
	if(go_back_level < 1) return 0;

	const SkillCfg *skill_cfg = this->GetSkillCfg(skill_id);
	if(NULL == skill_cfg) return 0;

	/*bool is_init_skill = false;
	const CareerAdvanceCfg* prof_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(profession, advance_sub_level);
	if (NULL == prof_cfg) return 0;

	for (int i = 0; i < prof_cfg->init_skill_num && i < PROFESSION_SKILL_INIT_MAX_NUM; i++)
	{
		if (skill_cfg->skill_id == prof_cfg->init_skill_id[i])
		{
			is_init_skill = true;
		}
	}*/

	//int back_money = 0;
	//int cur_up_count = 0;
	//经验体系屏蔽,改用直接升级
	long long back_money = skill_cfg->GetCoinConsumeCount(skill_level, go_back_level);

	/*for (int cur_up_level = skill_level + 1; cur_up_level >= go_back_level; --cur_up_level)
	{
		if (cur_up_level == skill_level + 1)
		{
			if (0 != skill_cfg->GetAddExp(cur_up_level))
			{
				cur_up_count = cur_level_exp / skill_cfg->GetAddExp(cur_up_level);
				back_money = cur_up_count *  skill_cfg->GetBindCoinConsume(cur_up_level);
			}
		}
		else
		{
			if (is_init_skill && cur_up_level == 1)		//转职赠送的初始技能，1级为赠送，不予返还
			{
				continue;
			}
			if (skill_cfg->GetAddExp(cur_up_level) != 0)
			{
				cur_up_count = skill_cfg->GetUplevelNeedExp(cur_up_level) / skill_cfg->GetAddExp(cur_up_level);
				back_money += cur_up_count * skill_cfg->GetBindCoinConsume(cur_up_level);
			}
		}
	}*/

	return back_money;
}

bool SkillPool::InitSkill(const std::string &configname, std::string *err, short skill_type)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load Skill Error,\n %s.", configname.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	{
		// skilllist
		TiXmlElement *element = RootElement->FirstChildElement("skilllist");
		if (NULL == element)
		{
			*err = configname + ": no [skilllist].";
			return false;
		}

		std::string error_reason;
		iRet = InitSkillListCfg(element, skill_type, &error_reason);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSkillListCfg failed %d with reason[%s]", configname.c_str(), iRet, error_reason.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

bool SkillPool::InitLifeSkill(const std::string& configname, std::string* err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("rand_group", InitAttrTypeRandGroup);
	LOAD_CONFIG("att_value", InitAttrSegeRandGroup);
	LOAD_CONFIG("num_group", InitAttrNumbRandGroup);
	LOAD_CONFIG("life_skill", InitLifeSkillCfg);
	LOAD_CONFIG("other", InitLifeSkillOtherCfg);
	LOAD_CONFIG("create_exp", InitMakeEquipExpCfg);
	
	return true;
}

bool SkillPool::InitPassiveSkill(const std::string & config_name, std::string * err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (config_name == "" || !document.LoadFile(config_name))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load PassiveSkill Error,\n %s.", config_name.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = config_name + ": xml root node error.";
		return false;
	}

	{
		// passive_cfg
		TiXmlElement *element = RootElement->FirstChildElement("passive_cfg");
		if (NULL == element)
		{
			*err = config_name + ": no [passive_cfg].";
			return false;
		}

		iRet = InitPassiveSkillCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitPassiveSkillCfg failed ret[%d]", config_name.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// passive_level_cfg
		TiXmlElement *element = RootElement->FirstChildElement("passive_level_cfg");
		if (NULL == element)
		{
			*err = config_name + ": no [passive_level_cfg].";
			return false;
		}

		iRet = InitPassiveSkillLevelCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitPassiveSkillLevelCfg failed ret[%d]", config_name.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

int SkillPool::InitSkillListCfg(TiXmlElement *RootElement, short skill_type, std::string *err)
{
	char errinfo[1024] = { 0 };

	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] no data node", skill_type);
		*err = errinfo;
		return -10000;
	}

	short last_skill_id = -1;
	short last_level = 0;
	while (NULL != dataElement)
	{
		short skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id) || skill_id < 0 || skill_id >= MAX_BATTLE_SKILL_ID)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] id invalid", skill_type, skill_id);
			*err = errinfo;
			return -1;
		}

		if (last_skill_id != skill_id)
		{
			last_level = 0;
			last_skill_id = skill_id;
		}

		SkillCfg* skill_cfg = m_skill_list[skill_id];
		if (NULL == skill_cfg)
		{
			skill_cfg = new SkillCfg();
			skill_cfg->skill_id = skill_id;
			m_skill_list[skill_id] = skill_cfg;
		}

		int level = 0;
		if (!GetSubNodeValue(dataElement, "skill_level", level) || level <= 0 || level > MAX_BATTLE_SKILL_LEVEL || level != last_level + 1)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] level invalid", skill_type, skill_id);
			*err = errinfo;
			return -2;
		}
		if (level < skill_cfg->max_level)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] level[%d] invalid, maybe repeat ", skill_type, skill_id, level);
			*err = errinfo;
			return -22;
		}
		last_level = level;
		skill_cfg->max_level = level;

		int need_exp = 0;
		if (!GetSubNodeValue(dataElement, "up_level_need_exp", need_exp) || need_exp < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] up_level_need_exp<0 invalid", skill_type, skill_id);
			*err = errinfo;
			return -3;
		}
		skill_cfg->level_cfg_list[level].uplevel_need_exp = need_exp;

		int add_exp = 0;
		if (!GetSubNodeValue(dataElement, "add_exp", add_exp) || add_exp <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] add_exp<=0 invalid", skill_type, skill_id);
			*err = errinfo;
			return -4;
		}
		skill_cfg->level_cfg_list[level].add_exp = add_exp;

		int bind_coin_consume = 0;
		if (!GetSubNodeValue(dataElement, "bind_coin_consume", bind_coin_consume) || bind_coin_consume <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] bind_coin_consume<=0 invalid", skill_type, skill_id);
			*err = errinfo;
			return -5;
		}
		skill_cfg->level_cfg_list[level].bind_coin_consume = bind_coin_consume;

		int skill_learn_level = 0;
		if (!GetSubNodeValue(dataElement, "skill_learn_level", skill_learn_level) || skill_learn_level < 0 || skill_learn_level > MAX_ROLE_LEVEL)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] skill_learn_level<=0 invalid", skill_type, skill_id);
			*err = errinfo;
			return -6;
		}
		skill_cfg->level_cfg_list[level].skill_learn_level = skill_learn_level;

		int coin_consume = 0;
		if (!GetSubNodeValue(dataElement, "coin_consume", coin_consume) || coin_consume <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "SKILL_TYPE[%d] SKILL_ID[%d] coin_consume<=0 invalid", skill_type, skill_id);
			*err = errinfo;
			return -5;
		}
		skill_cfg->level_cfg_list[level].coin_consume = coin_consume;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SkillPool::InitLifeSkillCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_level = -1;
	int last_type = -1;
	while (dataElement)
	{
		int type = 0;
		if (!GetSubNodeValue(dataElement, "skill_type", type) || type < LIFE_SKILL_TYPE_MIN || type >= LIFE_SKILL_TYPE_MAX || (type != last_type && type != last_type + 1))
		{
			return -1;
		}

		if (last_type != type)
		{
			last_type = type;
			last_level = -1;
		}

		LifeSkillCfg & cfg = m_life_skill_list[type];
		cfg.life_skill_type = type;

		UInt16 skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id) || (cfg.life_skill_id > 0 && skill_id != cfg.life_skill_id) || 0 == skill_id)
		{
			return -2;
		}
		cfg.life_skill_id = skill_id;

		int level = 0;
		if (!GetSubNodeValue(dataElement, "skill_level", level) || level < 0 || level > MAX_LIFE_SKILL_LEVEL || level != last_level + 1)
		{
			return -3;
		}
		last_level = level;
		cfg.max_level = level;

		if (!GetSubNodeValue(dataElement, "life_type", cfg.life_skill_class) || cfg.life_skill_class <= LIFE_SKILL_CLASS_INVALID || cfg.life_skill_class >= LIFE_SKILL_CLASS_MAX)
		{
			return -4;
		}

		LifeSkillCfg::SkillParam & param = cfg.skill_param[level];
		if (!GetSubNodeValue(dataElement, "up_level_need_exp", param.upgrade_need_exp) || param.upgrade_need_exp <= 0)
		{
			return -5;
		}
		
		if (!GetSubNodeValue(dataElement, "param_1", param.param1))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "param_2", param.param2))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "param_3", param.param3))
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "param_4", param.param4))
		{
			return -9;
		}

		switch (cfg.life_skill_class)
		{
		case LIFE_SKILL_CLASS_GATHER:
			break;
		case LIFE_SKILL_CLASS_CRAFTS:
			if (level > 0 && m_attr_numb_rand_map.end() == m_attr_numb_rand_map.find(param.param3)) return -50;
			//if (level > 0 && m_attr_type_rand_map.end() == m_attr_type_rand_map.find(param.param4)) return -51;
			break;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SkillPool::InitLifeSkillOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	if (!GetSubNodeValue(dataElement, "dig_add_exp", m_life_skill_other_cfg.dig_add_exp) || m_life_skill_other_cfg.dig_add_exp < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "fell_add_exp", m_life_skill_other_cfg.lumber_add_exp) || m_life_skill_other_cfg.lumber_add_exp < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "collect_add_exp", m_life_skill_other_cfg.pick_flower_add_exp) || m_life_skill_other_cfg.pick_flower_add_exp < 0)
	{
		return -3;
	}

	return 0;
}

int SkillPool::InitMakeEquipExpCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -1000;
	}

	m_life_skill_other_cfg.make_equip_add_exp_list_size = 0;
	int& size = m_life_skill_other_cfg.make_equip_add_exp_list_size;

	int last_level = 0;
	while (dataElement != NULL)
	{
		int equip_level = 0;
		if (!GetSubNodeValue(dataElement, "equipment_level", equip_level) || equip_level <= 0 || last_level + 1 != equip_level)
		{
			return -1;
		}
		last_level = equip_level;

		int add_exp = 0;
		if (!GetSubNodeValue(dataElement, "add_exp", add_exp) || add_exp < 0)
		{
			return -2;
		}

		static MakeEquipExpInfo info;
		info.equip_level = equip_level;
		info.add_exp = add_exp;

		m_life_skill_other_cfg.make_equip_add_exp_list[size++] = info;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SkillPool::InitAttrTypeRandGroup(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int group_id = 0, attr_type = 0, rand_prob = 0;
		if (!GetSubNodeValue(dataElement, "att_group", group_id) || group_id < 0 || group_id >= RAND_GROUP_NUM)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "att_type", attr_type) || attr_type < 0 || attr_type >= ADD_POINT_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "rate", rand_prob) || rand_prob <= 0 || rand_prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -3;
		}

		m_attr_type_rand_map[group_id][attr_type] = rand_prob;

		

		dataElement = dataElement->NextSiblingElement();
	}

	// 检查每一组的概率之和是否为 RAND_UNIVERSAL_RATIO_BASE_NUM
	for (int i = 0; i < RAND_GROUP_NUM; ++i)
	{
		int total_prob = 0;
		for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
		{
			total_prob += m_attr_type_rand_map[i][k];
		}

		if (total_prob == 0) continue;

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != total_prob) return -(10000 + i);
	}

	return 0;
}

int SkillPool::InitAttrSegeRandGroup(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int skill_lv = 0, attr_type = 0; AttrSegment tmp_sege;
		if (!GetSubNodeValue(dataElement, "skill_level", skill_lv) || skill_lv <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "att_type", attr_type) || attr_type < 0 || attr_type >= ADD_POINT_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "min", tmp_sege.min_val) || tmp_sege.min_val < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "max", tmp_sege.max_val) || tmp_sege.max_val < tmp_sege.min_val)
		{
			return -4;
		}

		m_attr_segement_map[skill_lv][attr_type] = tmp_sege;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int SkillPool::InitAttrNumbRandGroup(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int group_id = 0, number = 0, prob = 0;
		if (!GetSubNodeValue(dataElement, "num_group", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "num", number) || number < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "rate", prob) || prob <= 0 || prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -3;
		}

		m_attr_numb_rand_map[group_id][number] = prob;
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::map<int, std::map<int, int> >::iterator loop = m_attr_numb_rand_map.begin(); loop != m_attr_numb_rand_map.end(); ++loop)
	{
		int prob_total = 0;
		for (std::map<int, int>::iterator num_loop = loop->second.begin(); num_loop != loop->second.end(); ++num_loop)
		{
			prob_total += num_loop->second;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != prob_total) return -50;
	}

	return 0;
}

int SkillPool::InitPassiveSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	while (dataElement != NULL)
	{
		int skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id) || skill_id <= 0)
		{
			return -1;
		}

		GamePassiveSkillCfgMap::iterator it = m_passive_skill_cfg_map.find(skill_id);
		if (m_passive_skill_cfg_map.end() != it)
		{
			return -10;
		}

		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id) || sp_id < 0)
		{
			return -2;
		}

		int skill_type = 0;
		if (!GetSubNodeValue(dataElement, "skill_type", skill_type) || skill_type <= 0)
		{
			return -3;
		}

		int skill_priority = 0;
		if (!GetSubNodeValue(dataElement, "is_same_type", skill_priority) || skill_priority <= GAME_PASSIVE_SKILL_PRIORITY_TYPE_INVALID ||
			skill_priority >= GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX)
		{
			return -4;
		}

		GamePassiveSkillCfg cfg;
		cfg.skill_id = skill_id;
		cfg.sp_id = sp_id;
		cfg.skill_type = skill_type;
		cfg.skill_priority = skill_priority;

		m_passive_skill_cfg_map.insert(GamePassiveSkillCfgMap::value_type(skill_id, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool SkillPool::RandSingleAttr(int group_id, int skill_lv, AttrInfo * out_info)
{
	if (group_id < 0 || group_id >= RAND_GROUP_NUM) return false;

	/*
	int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (int k = 0; k < ADD_POINT_TYPE_MAX; ++k)
	{
		if (rand_prob < m_attr_type_rand_map[group_id][k])
		{
			out_info->attr_type = k;
			break;
		}

		rand_prob -= m_attr_type_rand_map[group_id][k];
	}
	*/

	std::map<int, std::map<int, AttrSegment> >::iterator skill_lv_it = m_attr_segement_map.find(skill_lv);
	if (m_attr_segement_map.end() == skill_lv_it) return false;

	std::map<int, AttrSegment>::iterator sege_it = skill_lv_it->second.find(out_info->attr_type);
	if (skill_lv_it->second.end() == sege_it) return false;

	AttrSegment & segement = sege_it->second;
	out_info->attr_value = rand() % (segement.max_val - segement.min_val + 1) + segement.min_val;
	return true;
}

int SkillCfg::GetUplevelNeedExp(int _level) const
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	return level_cfg_list[_level].uplevel_need_exp;
}

int SkillCfg::GetAddExp(int _level) const 
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return 0;
	}

	return level_cfg_list[_level].add_exp;
}

int SkillCfg::GetBindCoinConsume(int _level) const
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	return level_cfg_list[_level].bind_coin_consume;
}

int SkillCfg::GetCoinConsume(int _level) const
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	return level_cfg_list[_level].coin_consume;
}

long long SkillCfg::GetCoinConsumeCount(int cur_level, int to_level) const
{
	if (cur_level < 0 || cur_level > max_level || cur_level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	if (to_level < 0 || to_level > max_level || to_level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	if (cur_level == to_level)
	{
		return 0;
	}

	if (cur_level > to_level)
	{
		std::swap(cur_level, to_level);
	}
	//获得升级或回退所需
	long long coin_consume_count = 0;
	for (int i = cur_level + 1; i <= to_level; ++i)
	{
		coin_consume_count += level_cfg_list[i].coin_consume;
	}

	return coin_consume_count;
}

int SkillCfg::GetRoleLevelLimit(int _level) const
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return MAX_INT;
	}

	return level_cfg_list[_level].skill_learn_level;
}

int SkillCfg::GetMaxLevel() const
{
	return GetMin(MAX_BATTLE_SKILL_LEVEL, max_level);
}

int SkillCfg::GetMaxLevelForRoleLevel(int role_level) const
{
	for (int i = 1; i < max_level; ++i)
	{
		if (role_level <= level_cfg_list[i].skill_learn_level)
		{
			return i;
		}
	}

	return max_level;
}

const SkillLevelCfg* SkillCfg::GetLevelCfg(int _level) const
{
	if (_level < 0 || _level > max_level || _level >= MAX_BATTLE_SKILL_LEVEL + 1)
	{
		return NULL;
	}

	return &level_cfg_list[_level];
}

int SkillPool::InitPassiveSkillLevelCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	while (dataElement != NULL)
	{
		int skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id) || skill_id <= 0)
		{
			return -1;
		}

		GamePassiveSkillCfgMap::iterator it = m_passive_skill_cfg_map.find(skill_id);
		if (m_passive_skill_cfg_map.end() == it)
		{
			return -100;
		}

		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id) || sp_id < 0)
		{
			return -2;
		}

		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level) || level <= 0 || level > MAX_BATTLE_SKILL_LEVEL)
		{
			return -3;
		}

		it->second.level_spid_list[level] = sp_id;
		it->second.is_level_spid = true;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
