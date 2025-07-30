#include "roleexpconfig.hpp"
#include "servercommon/commondata.hpp"

RoleExpConfig::RoleExpConfig()
{

}

RoleExpConfig::~RoleExpConfig()
{
	std::vector<RoleTopLevelCfg>().swap(m_top_level_reward_vec);
	std::vector<RoleTopLevelCfg>().swap(m_loop_top_level_reward_vec);
	std::vector<RoleTopLevelAchieveCfg>().swap(m_top_level_achieve_vec);
}

bool RoleExpConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("exp_config", InitExpInfoCfg);
	LOAD_CONFIG("base_att", InitBaseAttrCfg);
	LOAD_CONFIG("base_point", InitBaseAttrPointDistributeCfg);
	LOAD_CONFIG("best_add_point", InitRecommendAddPointCfg);
	LOAD_CONFIG("attributes_exchange", InitConvertAddPointCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("world_level", InitWorldLevelCfg);
	LOAD_CONFIG("world_level_addition", InitWorldLevelUpCfg);
	LOAD_CONFIG("world_level_divide", InitWorldLevelUpDivideCfg);
	LOAD_CONFIG("top_level", InitTopLevelCfg);
	LOAD_CONFIG("top_achieve", InitTopLevelAchieveCfg);
	LOAD_CONFIG("world_level_decay", InitLevelExpDecayCfg);

	return true;
}

const RoleLevelCfg* RoleExpConfig::GetLevelCfg(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL)
	{
		return NULL;
	}

	return &m_level_cfg_list[level];
}

const AttrPointCfg* RoleExpConfig::GetBasePointDistributeCfg(int profession)
{
	int base_prof = profession / PROFESSION_BASE;
	static const int ARRAY_SIZE = ARRAY_ITEM_COUNT(m_base_point_distribute_cfg);
	for (int i = 0; i < m_base_point_distribute_cfg_num && i < ARRAY_SIZE; ++i)
	{
		if (m_base_point_distribute_cfg[i].prof == base_prof)
		{
			return &m_base_point_distribute_cfg[i];
		}
	}

	return NULL;
}

const AttrPointCfg* RoleExpConfig::GetRecommendAddPointCfg(int profession)
{
	int base_prof = profession / PROFESSION_BASE;
	static const int ARRAY_SIZE = ARRAY_ITEM_COUNT(m_recommend_add_point_cfg);
	for (int i = 0; i < m_recommend_add_point_cfg_num && i < ARRAY_SIZE; ++i)
	{
		if (m_recommend_add_point_cfg[i].prof == base_prof)
		{
			return &m_recommend_add_point_cfg[i];
		}
	}

	return NULL;
}

const ConvertAddPointCfg* RoleExpConfig::GetConvertCfg(int add_point_type)
{
	if (add_point_type < ADD_POINT_TYPE_MIN || add_point_type >= ADD_POINT_TYPE_MAX) return NULL;

	return &m_convert_list[add_point_type];
}

void RoleExpConfig::ConvertPointToAttribute(const AttributePlan& ap, AttributeList& out_attr_list)
{
	//计算好总数值再取整
	double maxhp = 0;
	double maxmp = 0;
	double attack = 0;
	double defence = 0;
	double agility = 0;
	double mentality = 0;
	double recovery = 0;

	for (int i = ADD_POINT_TYPE_MIN; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const ConvertAddPointCfg* cfg = this->GetConvertCfg(i);
		if (NULL != cfg)
		{
			maxhp += cfg->maxhp_rate * ap.add_point_list[i];
			maxmp += cfg->maxmp_rate * ap.add_point_list[i];
			attack += cfg->attack_rate * ap.add_point_list[i];
			defence += cfg->defense_rate * ap.add_point_list[i];
			agility += cfg->agility_rate * ap.add_point_list[i];
			mentality += cfg->mentality_rate * ap.add_point_list[i];
			recovery += cfg->recovery_rate * ap.add_point_list[i];
		}
	}

	// 加1e-4是避免浮点数比实际数小而导致取整后出现的误差
	if (maxhp >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(maxhp + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(maxhp - 1e-4);
	}
	if (maxmp >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(maxmp + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(maxmp - 1e-4);
	}
	if (attack >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(attack + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(attack - 1e-4);
	}
	if (defence >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(defence + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(defence - 1e-4);
	}
	if (agility >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(agility + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(agility - 1e-4);
	}
	if (mentality >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(mentality + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(mentality - 1e-4);
	}
	if (recovery >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(recovery + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(recovery - 1e-4);
	}

	/*out_attr_list.m_attrs[BATTLE_ATTR_MAX_HP]	+= static_cast<Attribute>(maxhp + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_MAX_MP]	+= static_cast<Attribute>(maxmp + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_ATTACK]	+= static_cast<Attribute>(attack + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_DEFENSE]	+= static_cast<Attribute>(defence + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_AGILITY]	+= static_cast<Attribute>(agility + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_MENTAL]	+= static_cast<Attribute>(mentality + 1e-4);
	out_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(recovery + 1e-4);*/
}

void RoleExpConfig::ConvertPointToAttribute2(AttributeList & ARG_IN_AND_OUT out_attr_list)
{
	//计算好总数值再取整
	double maxhp = 0;
	double maxmp = 0;
	double attack = 0;
	double defence = 0;
	double agility = 0;
	double mentality = 0;
	double recovery = 0;

	int add_point_list[ADD_POINT_TYPE_MAX] = { 0 };
	out_attr_list.GetAddPointTypeValue(add_point_list);
	out_attr_list.ResetAddPointTypeValue();

	for (int i = ADD_POINT_TYPE_MIN; i < ADD_POINT_TYPE_MAX; ++i)
	{
		const ConvertAddPointCfg * cfg = this->GetConvertCfg(i);
		if (NULL != cfg)
		{
			maxhp += cfg->maxhp_rate * add_point_list[i];
			maxmp += cfg->maxmp_rate * add_point_list[i];
			attack += cfg->attack_rate * add_point_list[i];
			defence += cfg->defense_rate * add_point_list[i];
			agility += cfg->agility_rate * add_point_list[i];
			mentality += cfg->mentality_rate * add_point_list[i];
			recovery += cfg->recovery_rate * add_point_list[i];
		}
	}

	// 加1e-4是避免浮点数比实际数小而导致取整后出现的误差
	if (maxhp >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(maxhp + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(maxhp - 1e-4);
	}
	if (maxmp >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(maxmp + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(maxmp - 1e-4);
	}
	if (attack >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(attack + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(attack - 1e-4);
	}
	if (defence >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(defence + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(defence - 1e-4);
	}
	if (agility >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(agility + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(agility - 1e-4);
	}
	if (mentality >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(mentality + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(mentality - 1e-4);
	}
	if (recovery >= 0)
	{
		out_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(recovery + 1e-4);
	}
	else
	{
		out_attr_list.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(recovery - 1e-4);
	}
}

int RoleExpConfig::GetWorldLevel(int open_server_day)
{
	if (open_server_day <= 0)
	{
		open_server_day = 1;
	}
	for (std::map<int, int>::reverse_iterator it = m_world_level_map.rbegin(); it != m_world_level_map.rend(); ++it)
	{
		if (open_server_day >= it->first)
		{	
			return it->second;
		}
	}

	return 1;
}

int RoleExpConfig::GetWorldLevelUp(int role_level, int world_level)
{
	int level_diff = 0;
	if (role_level < world_level)
	{
		level_diff = world_level - role_level;
	}
	int level_addition = BASE_EXP_PERCENTAGE_NUM;
	for (std::map<int, int>::iterator it = m_world_level_up_map.begin(); it != m_world_level_up_map.end(); ++it)
	{
		if (it->first / 1000 <= level_diff && it->first % 1000 >= level_diff)
		{
			level_addition = it->second;
			break;
		}
	}
	int level_addition_limit = this->GetWorldLevelUpLimit(role_level);
	if (level_addition > level_addition_limit)
	{
		level_addition = level_addition_limit;
	}
	int pass_level = role_level - world_level;
	if (pass_level >= 0)
	{
		level_addition = (int)(level_addition * (1.0 * (BASE_EXP_PERCENTAGE_NUM - this->GetExpDecayScaleByPassLevel(pass_level)) / BASE_EXP_PERCENTAGE_NUM));
	}

	return level_addition;
}

int RoleExpConfig::GetWorldLevelUpLimit(int role_level)const
{
	for (std::map<int, int>::const_iterator it = m_world_level_divide_map.begin(); it != m_world_level_divide_map.end(); ++it)
	{
		if (it->first / 1000 <= role_level && it->first % 1000 >= role_level)
		{
			return it->second;
		}
	}

	return 0;
}

int RoleExpConfig::GetExpDecayScaleByPassLevel(int pass_level) const
{
	for (std::map<int, int>::const_reverse_iterator it = m_level_exp_decay_map.rbegin(); it != m_level_exp_decay_map.rend(); ++it)
	{
		if (pass_level >= it->first)
		{
			return it->second;
		}
	}

	return 0;
}

long long RoleExpConfig::GetNeedExpByLevel(int begin_level, int end_level)
{
	if(begin_level <= 0 || begin_level > MAX_ROLE_LEVEL || end_level <= 0 || end_level > MAX_ROLE_LEVEL) return 0;

	if (begin_level > end_level)
	{
		int tmp_level = begin_level;
		begin_level = end_level;
		end_level = tmp_level;
	}
	
	long long need_exp = 0;
	for (int i = begin_level; i <= end_level && i < ARRAY_LENGTH(m_level_cfg_list); ++i)
	{
		need_exp += m_level_cfg_list[i].exp;
	}

	return need_exp;
}

const RoleTopLevelCfg * RoleExpConfig::GetTopLevelCfg(int top_level)
{
	if(top_level <= 0 || top_level > MAX_ROLE_TOP_LEVEL_NUM) return NULL;

	if (top_level <= (int)m_top_level_reward_vec.size())
	{
		return &m_top_level_reward_vec[top_level - 1];
	}

	if(m_loop_top_level_reward_vec.empty()) return NULL;

	top_level = (top_level - (int)m_top_level_reward_vec.size()) % (int)m_loop_top_level_reward_vec.size();

	if (0 == top_level)
	{
		return &m_loop_top_level_reward_vec[m_loop_top_level_reward_vec.size() - 1];
	}
	return &m_loop_top_level_reward_vec[top_level - 1];
}

const RoleTopLevelAchieveCfg * RoleExpConfig::GetToplevelAchieveCfg(int seq)
{
	if(seq < 0 || seq >= MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM || seq >= (int)m_top_level_achieve_vec.size()) return NULL;

 	return &m_top_level_achieve_vec[seq];
}

int RoleExpConfig::InitExpInfoCfg(TiXmlElement *RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_level = 0;
	while (dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level) || level != last_level + 1 || level <= 0 || level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		RoleLevelCfg& cfg = m_level_cfg_list[level];
		last_level = level;

		if (!GetSubNodeValue(dataElement, "exp", cfg.exp) || cfg.exp < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "att_point", cfg.total_attr_point) || cfg.total_attr_point < 0)
		{
			return -3;
		}

		if (level > 0)
		{
			RoleLevelCfg& last_level_cfg = m_level_cfg_list[level - 1];
			if (cfg.total_attr_point < last_level_cfg.total_attr_point)
			{
				return -4;
			}
			cfg.add_attr_point = cfg.total_attr_point - last_level_cfg.total_attr_point;

			if (cfg.exp < last_level_cfg.exp)
			{
				return -5;
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitBaseAttrCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "maxhp", m_base_attr_cfg.maxhp) || m_base_attr_cfg.maxhp < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "maxmp", m_base_attr_cfg.maxmp) || m_base_attr_cfg.maxmp < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "gongji", m_base_attr_cfg.attack) || m_base_attr_cfg.attack < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "fangyu", m_base_attr_cfg.defence) || m_base_attr_cfg.defence < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "minjie", m_base_attr_cfg.agility) || m_base_attr_cfg.agility < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "jingshen", m_base_attr_cfg.mentality) || m_base_attr_cfg.mentality < 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "huifu", m_base_attr_cfg.recovery) || m_base_attr_cfg.recovery < 0)
	{
		return -7;
	}

	return 0;
}

int RoleExpConfig::InitBaseAttrPointDistributeCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_job = -1;
	m_base_point_distribute_cfg_num = 0;

	while (dataElement)
	{
		int job = 0;
		if (!GetSubNodeValue(dataElement, "job", job) || job < 0 || job >= PROF_TYPE_MAX / PROFESSION_BASE)
		{
			return -1;
		}
		last_job = job;

		AttrPointCfg& cfg = m_base_point_distribute_cfg[m_base_point_distribute_cfg_num++];
		cfg.prof = job;

		if (!GetSubNodeValue(dataElement, "con", cfg.con) || cfg.con < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "str", cfg.str) || cfg.str < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "def", cfg.def) || cfg.def < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "agi", cfg.agi) || cfg.agi < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "magic", cfg.mag) || cfg.mag < 0)
		{
			return -6;
		}
		if (cfg.con + cfg.str + cfg.def + cfg.agi + cfg.mag != ATTR_BASE_POINT_MAX_NUM)
		{
			return -7;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitRecommendAddPointCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_job = -1;
	m_recommend_add_point_cfg_num = 0;

	while (dataElement)
	{
		int job = 0;
		if (!GetSubNodeValue(dataElement, "job", job) || job < 0 || job >= PROF_TYPE_MAX / PROFESSION_BASE)
		{
			return -1;
		}
		last_job = job;

		AttrPointCfg& cfg = m_recommend_add_point_cfg[m_recommend_add_point_cfg_num++];
		cfg.prof = job;

		if (!GetSubNodeValue(dataElement, "con", cfg.con) || cfg.con < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "str", cfg.str) || cfg.str < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "def", cfg.def) || cfg.def < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "agi", cfg.agi) || cfg.agi < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "magic", cfg.mag) || cfg.mag < 0)
		{
			return -6;
		}

		if (cfg.agi + cfg.con + cfg.def + cfg.mag + cfg.str != UPLEVEL_ADD_ATTR_POINT)
		{
			return -7;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitConvertAddPointCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_id = -1;
	while (dataElement)
	{ 
		int id = 0;
		if (!GetSubNodeValue(dataElement, "id", id) || id != last_id + 1)
		{
			return -1;
		}
		last_id = id;
		if (id < ADD_POINT_TYPE_MIN || id >= ADD_POINT_TYPE_MAX)
		{
			return -1000;
		}

		ConvertAddPointCfg& cfg = m_convert_list[id];
		int val = 0;
		if (!GetSubNodeValue(dataElement, "maxhp", val))
		{
			return -2;
		}
		cfg.maxhp_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "maxmp", val))
		{
			return -2;
		}
		cfg.maxmp_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "gongji", val))
		{
			return -2;
		}
		cfg.attack_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "fangyu", val))
		{
			return -2;
		}
		cfg.defense_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "minjie", val))
		{
			return -2;
		}
		cfg.agility_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "jingshen", val))
		{
			return -2;
		}
		cfg.mentality_rate = 0.01f * val;

		if (!GetSubNodeValue(dataElement, "huifu", val))
		{
			return -2;
		}
		cfg.recovery_rate = 0.01f * val;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		if (!GetSubNodeValue(dataElement, "wash_point_item", m_other_cfg.wash_point_item))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "wash_point_num", m_other_cfg.wash_point_num))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "auto_pro_add", m_other_cfg.auto_add_attr_level_upbound))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "world_level_effect", m_other_cfg.world_level_effect) || m_other_cfg.world_level_effect <= 0 || m_other_cfg.world_level_effect > MAX_ROLE_LEVEL)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "world_level_open", m_other_cfg.world_level_open_day) || m_other_cfg.world_level_open_day < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "world_level_overflow", m_other_cfg.world_level_overflow) || m_other_cfg.world_level_overflow <= 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "reset_time", m_other_cfg.top_level_reset_day) || m_other_cfg.top_level_reset_day <= 0)	
		{
			return -7;
		}

		int exp_coefficient = 0;
		if (!GetSubNodeValue(dataElement, "exp_coefficient", exp_coefficient) || exp_coefficient <= 0)
		{
			return -8;
		}
		m_other_cfg.exp_coefficient = 1.0 * exp_coefficient / 1000;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitWorldLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (NULL != dataElement)
	{
		int start_server_day = 0;
		if (!GetSubNodeValue(dataElement, "day", start_server_day) || start_server_day <= 0 || m_world_level_map.find(start_server_day) != m_world_level_map.end())
		{
			return -1;
		}
		int world_level = 0;
		if (!GetSubNodeValue(dataElement, "world_level", world_level) || world_level <= 0 || world_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		m_world_level_map[start_server_day] = world_level;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitWorldLevelUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (NULL != dataElement)
	{
		int min_level = 0, max_level = 0, level_addition = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL) return -2;

		int level_key = min_level * 1000 + max_level;
		if (m_world_level_up_map.end() != m_world_level_up_map.find(level_key))
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "level_addition", level_addition) || level_addition < 0) return -4;

		m_world_level_up_map[level_key] = level_addition;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitWorldLevelUpDivideCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (NULL != dataElement)
	{
		int min_level = 0, max_level = 0, level_addition_limit = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL) return -2;

		int level_key = min_level * 1000 + max_level;
		if (m_world_level_divide_map.end() != m_world_level_divide_map.find(level_key))
		{
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "level_addition_limit", level_addition_limit) || level_addition_limit < 0) return -4;

		m_world_level_divide_map[level_key] = level_addition_limit;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitTopLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int next_peak_level = 1;
	bool is_start_loop_flag = false;
	while (dataElement)
	{
		int peak_level = 0;
		if (!GetSubNodeValue(dataElement, "peak_level", peak_level) || peak_level != next_peak_level)
		{
			return -1;
		}
		next_peak_level += 1;
		RoleTopLevelCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "up_exp", node_cfg.up_exp) || node_cfg.up_exp <= 0)
		{
			return -2;
		}
		TiXmlElement * list_node = dataElement->FirstChildElement("item_list_list");
		if (NULL == list_node)
		{
			return -3;
		}
		TiXmlElement * item_node = list_node->FirstChildElement("item_list");
		if (NULL == item_node)
		{
			 return -4;
		}
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -5;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}

		int reward_group = 0;
		if (!GetSubNodeValue(dataElement, "reward_group", reward_group) || reward_group <= RoleTopLevelCfg::REWARD_GROUP_TYPE_BEGIN || reward_group >= RoleTopLevelCfg::REWARD_GROUP_TYPE_MAX)
		{
			return -6;
		}
		if (reward_group == RoleTopLevelCfg::REWARD_GROUP_TYPE_NORMAL)
		{
			if(is_start_loop_flag) return -7;
			m_top_level_reward_vec.push_back(node_cfg);
		}
		else
		{
			is_start_loop_flag = true;
			m_loop_top_level_reward_vec.push_back(node_cfg);
		}
		dataElement = dataElement->NextSiblingElement();
	}
	if (m_top_level_reward_vec.empty() && m_loop_top_level_reward_vec.empty())
	{
		return -8;
	}

	return 0;
}

int RoleExpConfig::InitTopLevelAchieveCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int next_seq = 0;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != next_seq || seq >= MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM) return -1;
		next_seq++;

		RoleTopLevelAchieveCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "achieve_type", node_cfg.achieve_type) || node_cfg.achieve_type <= RoleTopLevelAchieveCfg::ACHIEVE_TYPE_INVALID || 
			node_cfg.achieve_type >= RoleTopLevelAchieveCfg::ACHIEVE_TYPE_MAX) return -2;
	
		if (!GetSubNodeValue(dataElement, "param", node_cfg.param) || node_cfg.param < 0) return -3;

		TiXmlElement * list_node = dataElement->FirstChildElement("item_list");
		if (NULL == list_node)
		{
			return -4;
		}
		TiXmlElement * item_node = list_node->FirstChildElement("item");
		if (NULL == item_node)
		{
			return -5;
		}
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -6;

			node_cfg.item_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}
		
		m_top_level_achieve_vec.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RoleExpConfig::InitLevelExpDecayCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_pass_level = -1;
	while (NULL != dataElement)
	{
		int pass_level = 0;
		if (!GetSubNodeValue(dataElement, "pass_level", pass_level) || pass_level < 0 || pass_level <= last_pass_level) 
		{
			return -1;
		}
		last_pass_level = pass_level;

		int exp_decay_scale = 0;
		if (!GetSubNodeValue(dataElement, "decay_scale", exp_decay_scale) || exp_decay_scale <= 0 || exp_decay_scale >= BASE_EXP_PERCENTAGE_NUM)
		{
			return -2;
		}

		m_level_exp_decay_map[pass_level] = exp_decay_scale;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

