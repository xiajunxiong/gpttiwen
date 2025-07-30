#include "servercommon/commonsavedef.h"

#include "activityhuanjiezhanchangconfig.hpp"
#include "servercommon/attributedef.hpp"

ActivityHuanJieZhanChangConfig::ActivityHuanJieZhanChangConfig()
{
	memset(m_rank_least_score_list, 0, sizeof(m_rank_least_score_list));
}

ActivityHuanJieZhanChangConfig::~ActivityHuanJieZhanChangConfig()
{
	std::vector<ActivityHuanJieZhanChangRankCfg>().swap(m_rank_cfg);
	std::vector<ActivityHuanJieZhanChangRankingListCfg>().swap(m_ranking_list_cfg);
	std::vector<HuanJieZhanChangRobotCfg>().swap(m_huan_jie_robot_cfg);
}

bool ActivityHuanJieZhanChangConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("activity_cfg", InitOtherCfg);
	LOAD_CONFIG("ranking_cfg", InitRankCfg);
	LOAD_CONFIG("ranking_reward", InitRankingListCfg);
	LOAD_CONFIG("equipment_limit", InitEquipAttrLimitListCfg);
	LOAD_CONFIG("attr_adjust_group", InitRobotAttrAdjustCfg);
	LOAD_CONFIG("robot_cfg", InitRobotCfg);
	LOAD_CONFIG("victory_box", InitTodayRewardCfg);
	LOAD_CONFIG("robot_limit", InitRobotAttrLimitListCfg);
	
	return true;
}

const ActivityHuanJieZhanChangOtherCfg & ActivityHuanJieZhanChangConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const std::vector<ActivityHuanJieZhanChangRankCfg> & ActivityHuanJieZhanChangConfig::GetAllRankCfg()
{
	return m_rank_cfg;
}

const ActivityHuanJieZhanChangRankCfg * ActivityHuanJieZhanChangConfig::GetActivityHuanJieZhanChangRankCfg(int score)
{
	for (std::vector<ActivityHuanJieZhanChangRankCfg> ::reverse_iterator iter = m_rank_cfg.rbegin(); iter != m_rank_cfg.rend(); ++iter)
	{
		if (score >= iter->need_rank_score)
		{
			return &(*iter);
		}
	}

	return NULL;
}

const ActivityHuanJieZhanChangRankingListCfg * ActivityHuanJieZhanChangConfig::GetActivityHuanJieZhanChangRankingListCfg(int rank_pos)
{
	for (int i = 0; i < (int)m_ranking_list_cfg.size(); ++i)
	{
		if (m_ranking_list_cfg[i].min_ranking_pos <= rank_pos && rank_pos <= m_ranking_list_cfg[i].max_ranking_pos)
		{
			return &m_ranking_list_cfg[i];
		}
	}

	return NULL;
}

int ActivityHuanJieZhanChangConfig::GetActivityHuanJieZhanChangRankingTitle(int rank_pos)
{
	for (int i = 0; i < (int)m_ranking_list_cfg.size(); ++i)
	{
		if (m_ranking_list_cfg[i].min_ranking_pos <= rank_pos && rank_pos <= m_ranking_list_cfg[i].max_ranking_pos)
		{
			return m_ranking_list_cfg[i].title_id;
		}
	}

	return 0;
}

bool ActivityHuanJieZhanChangConfig::IsRankProtect(int score)
{
	const  ActivityHuanJieZhanChangRankCfg * cfg = this->GetActivityHuanJieZhanChangRankCfg(score);
	if (cfg != NULL)
	{
		return cfg->rank <= m_other_cfg.rank_protect;
	}

	return false;
}

int ActivityHuanJieZhanChangConfig::GetExtraScore(int score, int opponent_score)
{
	bool find1 = false, find2 = false;
	int rank1 = 0, rank2 = 0;
	for (std::vector<ActivityHuanJieZhanChangRankCfg>::reverse_iterator iter = m_rank_cfg.rbegin(); iter != m_rank_cfg.rend(); ++iter)
	{
		if (!find1 && score >= iter->need_rank_score)
		{
			find1 = true;
			rank1 = iter->rank;
		}

		if (!find2 && opponent_score >= iter->need_rank_score)
		{
			find2 = true;
			rank2 = iter->rank;
		}
	}

	int differ_rank = rank1 - rank2;
	int rank = std::abs(differ_rank);
	int extra_score = 0;
	if (rank > m_other_cfg.rank_wave_section)
	{
		extra_score = m_other_cfg.fluctuation_value_1;
	}

	if (rank > m_other_cfg.rank_wave_section * 2)
	{
		extra_score = m_other_cfg.fluctuation_value_2;
	}

	return differ_rank < 0 ? extra_score : -extra_score;		//小于段位比对手小，加分。不然减分
}

const ActivityHuanJieZhanChangEquipAttrLimitCfg * ActivityHuanJieZhanChangConfig::GetEquipAttrLimitCfg(int limit_type, int app_prof_type, int main_attr_type)
{
	int limit_type_key = this->GetAttrLimitKey(limit_type, app_prof_type, main_attr_type);
	std::map<int, ActivityHuanJieZhanChangEquipAttrLimitCfg>::iterator it = m_attr_limit_cfg.find(limit_type_key);
	if(it == m_attr_limit_cfg.end()) return NULL;

	return &it->second;
}

const ActivityHuanJieZhanChangRobotAttrLimitCfg * ActivityHuanJieZhanChangConfig::GetActivityHuanJieZhanChangRobotAttrLimitCfg(int limit_type, int p1)
{
	ActivityHuanJieZhanChangRobotAttrLimitCfgKey key;
	key.type = limit_type;
	key.p1 = p1;

	return MapValuesConstPtr(m_robot_attr_limit_cfg, key);
}

const HuanJieZhanChangRobotCfg * ActivityHuanJieZhanChangConfig::GetHuanJieRobotCfg(int rank_score)
{
	for (int i = 0; i < (int)m_huan_jie_robot_cfg.size(); ++i)
	{
		if (m_huan_jie_robot_cfg[i].min_score <= rank_score && rank_score <= m_huan_jie_robot_cfg[i].max_score)
		{
			return &m_huan_jie_robot_cfg[i];
		}
	}

	if (!m_huan_jie_robot_cfg.empty())
	{
		return &m_huan_jie_robot_cfg[0];
	}

	return NULL;
}

const HuanJieZhanChangRobotAttrAdjustCfg * ActivityHuanJieZhanChangConfig::GetHuanJieRobotAttrAdjustCfg(int group_id)
{
	std::map<int, HuanJieZhanChangRobotAttrAdjustCfg>  ::iterator iter = m_huan_jie_robot_adjust_cfg.find(group_id);
	if (iter != m_huan_jie_robot_adjust_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

int ActivityHuanJieZhanChangConfig::GetBigRankType(int rank_score)
{
	for (int i = HUAN_JIE_BIG_RANK_TYPE_INVALID + 1; i < HUAN_JIE_BIG_RANK_TYPE_MAX; ++i)
	{
		if (rank_score >= m_rank_least_score_list[i].rank_min_score && rank_score <= m_rank_least_score_list[i].rank_max_score)
		{
			return i;
		}
	}

	return HUAN_JIE_BIG_RANK_TYPE_INVALID;
}

const HuanJieTodayWinRewardCfg * ActivityHuanJieZhanChangConfig::GetHuanJieTodayWinRewardCfg(int seq)
{
	return MapValuesConstPtr(m_huan_jie_today_win_reward_cfg, seq);
}

std::vector<HuanJieTodayWinRewardCfg> ActivityHuanJieZhanChangConfig::GetHuanJieAllTodayWinRewardCfg()
{
	return MapValuesAsVector(m_huan_jie_today_win_reward_cfg);
}

int ActivityHuanJieZhanChangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_rounds", m_other_cfg.max_battle_rounds) || m_other_cfg.max_battle_rounds < 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "basics_score", m_other_cfg.base_score) || m_other_cfg.base_score < 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "victory_score", m_other_cfg.win_add_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "defeat_score", m_other_cfg.lose_add_score))
	{
		return -__LINE__;
	}
	m_other_cfg.lose_add_score = -m_other_cfg.lose_add_score;

	if (!GetSubNodeValue(root_element, "winning_streak", m_other_cfg.win_streak_times) || m_other_cfg.win_streak_times <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "winning_streak_score", m_other_cfg.win_streak_add_score))
	{
		return -__LINE__;
	}


	if (!GetSubNodeValue(root_element, "victory_money", m_other_cfg.win_add_huan_jie_coin))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "defeat_money", m_other_cfg.lose_add_huan_jie_coin))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "play_even_money", m_other_cfg.draw_add_huan_jie_coin))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "positioning_race", m_other_cfg.ding_ji_sai_times))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "positioning_victory_score", m_other_cfg.ding_ji_win_add_score))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "positioning_defeat_score", m_other_cfg.ding_ji_lose_add_score))
	{
		return -__LINE__;
	}
	m_other_cfg.ding_ji_lose_add_score = -m_other_cfg.ding_ji_lose_add_score;

	if (!GetSubNodeValue(root_element, "day_money_limit", m_other_cfg.today_huan_jie_coin_limit))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "rank_protect_off", m_other_cfg.rank_protect))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "rank_wave_section", m_other_cfg.rank_wave_section))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "fluctuation_value_1", m_other_cfg.fluctuation_value_1))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "fluctuation_value_2", m_other_cfg.fluctuation_value_2))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "level_limit", m_other_cfg.level_limit) || m_other_cfg.level_limit <= 0 || m_other_cfg.level_limit > MAX_ROLE_LEVEL)
	{
		return -__LINE__;
	}

	return 0;
}

int ActivityHuanJieZhanChangConfig::InitRankCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_need_rank_score = -1;
	while (NULL != root_element)
	{
		ActivityHuanJieZhanChangRankCfg cfg;
		if (!GetSubNodeValue(root_element, "score", cfg.need_rank_score) || cfg.need_rank_score <= last_need_rank_score)
		{
			return -__LINE__;
		}
		last_need_rank_score = cfg.need_rank_score;

		if (!GetSubNodeValue(root_element, "season_down", cfg.season_reduce_score))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "ranking_reward_money", cfg.rank_reward_huan_jie_coin))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "ranking", cfg.rank))
		{
			return -__LINE__;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("ranking_reward_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("ranking_reward");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -__LINE__;
				}

				cfg.rank_reward.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		TiXmlElement * settlement_reward_element = root_element->FirstChildElement("settlement_ranking_reward_list");
		if (settlement_reward_element != NULL)
		{
			TiXmlElement * node_element = settlement_reward_element->FirstChildElement("settlement_ranking_reward");
			while (node_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(node_element))
				{
					return -__LINE__;
				}

				cfg.settlement_rank_reward.push_back(reward);
				node_element = node_element->NextSiblingElement();
			}
		}

		int big_rank_type = 0;
		if (!GetSubNodeValue(root_element, "rank", big_rank_type) || big_rank_type <= HUAN_JIE_BIG_RANK_TYPE_INVALID || big_rank_type >= HUAN_JIE_BIG_RANK_TYPE_MAX)
		{
			return -__LINE__;
		}
		if (big_rank_type != HUAN_JIE_BIG_RANK_TYPE_INVALID + 1 && m_rank_least_score_list[big_rank_type].rank_min_score == 0)
		{
			m_rank_least_score_list[big_rank_type].rank_min_score = cfg.need_rank_score;
			m_rank_least_score_list[big_rank_type - 1].rank_max_score = cfg.need_rank_score - 1;
		}
		

		m_rank_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	m_rank_least_score_list[HUAN_JIE_BIG_RANK_TYPE_MAX - 1].rank_max_score = MAX_INT;

	return 0;
}

int ActivityHuanJieZhanChangConfig::InitRankingListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_max_rank_pos = 0;
	while (NULL != root_element)
	{
		ActivityHuanJieZhanChangRankingListCfg cfg;
		if (!GetSubNodeValue(root_element, "min_rank_pos", cfg.min_ranking_pos) || cfg.min_ranking_pos <= last_max_rank_pos)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "max_rank_pos", cfg.max_ranking_pos) || cfg.max_ranking_pos < cfg.min_ranking_pos)
		{
			return -__LINE__;
		}
		last_max_rank_pos = cfg.max_ranking_pos;

		if (!GetSubNodeValue(root_element, "rank_demand", cfg.need_rank_score))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "title", cfg.title_id))
		{
			return -__LINE__;
		}

		if (cfg.title_id != 0 && cfg.max_ranking_pos > HUAN_JIE_RANK_TITLE_MAX_NUM)
		{
			return -__LINE__;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("ranking_reward_item_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("ranking_reward_item");
			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}

				cfg.ranking_reward.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_ranking_list_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityHuanJieZhanChangConfig::InitEquipAttrLimitListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int limit_type = 0, job_type = 0, main_attribute_type = 0;
		if (!GetSubNodeValue(root_element, "limit_type", limit_type) || limit_type < 0 || limit_type >= HUAN_JIE_HUA_SHEN_ATTR_LIMIT_TYPE_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "job_type", job_type) || job_type < 0 || job_type >= PROFESSION_TYPE_NUM)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "main_attribute", main_attribute_type))
		{
			return -__LINE__;
		}
		if (0 == job_type)
		{
			main_attribute_type -= 1;
		}
		if (main_attribute_type < ADD_POINT_TYPE_MIN || main_attribute_type >= ADD_POINT_TYPE_MAX)
		{
			return -__LINE__;
		}

		int limit_type_key = this->GetAttrLimitKey(limit_type, job_type, main_attribute_type);
		if (m_attr_limit_cfg.end() != m_attr_limit_cfg.find(limit_type_key))
		{
			return -__LINE__;
		}
		ActivityHuanJieZhanChangEquipAttrLimitCfg node;
		const char * attr_limit_name[HJZC_EQUIP_ATTR_LIMIT_MAX_NUM] = { "max_maxhp", "max_maxmp", "max_gongji", "max_fangyu", "max_minjie",
			"max_jingshen", "max_huifu", "max_bisha", "max_shanduo", "max_mingzhong", "max_fanji", "max_mokang", "max_mogong", "max_jianren", "max_bisha_dmg",
			"max_def_zhongdu", "max_def_shihua", "max_def_hunshui", "max_def_jiuzui", "max_def_hunluan", "max_def_chenmo", "max_def_zhongshang",
			"max_zhenshang"};
		int attr_limit_type_list[HJZC_EQUIP_ATTR_LIMIT_MAX_NUM] = { BATTLE_ATTR_MAX_HP , BATTLE_ATTR_MAX_MP , BATTLE_ATTR_ATTACK , BATTLE_ATTR_DEFENSE , BATTLE_ATTR_AGILITY ,
			BATTLE_ATTR_MENTAL , BATTLE_ATTR_RECOVERY, BATTLE_ATTR_CRITICAL, BATTLE_ATTR_DODGE , BATTLE_ATTR_HIT , BATTLE_ATTR_COUNTER_ATTACK , BATTLE_ATTR_MAGIC_DEFENSE , 
			BATTLE_ATTR_MAGIC_ATTACK, BATTLE_ATTR_CRITICAL_DEC_VALUE, BATTLE_ATTR_CRITICAL_INC_VALUE, BATTLE_ATTR_ANTI_POISON , BATTLE_ATTR_ANTI_PETRIFY, BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK, BATTLE_ATTR_ANTI_CHAOS, BATTLE_ATTR_ANTI_SILENCE, BATTLE_ATTR_ANTI_SEVERE_INJURE, BATTLE_ATTR_REAL_DAMAGE };
		for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
		{
			if (!GetSubNodeValue(root_element, attr_limit_name[i], node.attr_limit_list[i].attr_value))
			{
				return -__LINE__;
			}
			node.attr_limit_list[i].attr_type = attr_limit_type_list[i];
		}
		m_attr_limit_cfg[limit_type_key] = node;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityHuanJieZhanChangConfig::InitRobotAttrLimitListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ActivityHuanJieZhanChangRobotAttrLimitCfg node;
		if (!GetSubNodeValue(root_element, "limit_type", node.type))
		{
			return -__LINE__;
		}

		if (node.type== HUAN_JIE_HUA_SHEN_ROBOT_ATTR_LIMIT_TYPE_ROLE)
		{
			if (!GetSubNodeValue(root_element, "job_type", node.p1))
			{
				return -__LINE__;
			}
		}
		else
		{
			if (!GetSubNodeValue(root_element, "main_attribute", node.p1))
			{
				return -__LINE__;
			}
		}
	
		const char * attr_limit_name[HJZC_EQUIP_ATTR_LIMIT_MAX_NUM] = { "max_maxhp", "max_maxmp", "max_gongji", "max_fangyu", "max_minjie",
			"max_jingshen", "max_huifu", "max_bisha", "max_shanduo", "max_mingzhong", "max_fanji", "max_mokang", "max_mogong", "max_jianren", "max_bisha_dmg",
			"max_def_zhongdu", "max_def_shihua", "max_def_hunshui", "max_def_jiuzui", "max_def_hunluan", "max_def_chenmo", "max_def_zhongshang",
			"max_zhenshang" };
		int attr_limit_type_list[HJZC_EQUIP_ATTR_LIMIT_MAX_NUM] = { BATTLE_ATTR_MAX_HP , BATTLE_ATTR_MAX_MP , BATTLE_ATTR_ATTACK , BATTLE_ATTR_DEFENSE , BATTLE_ATTR_AGILITY ,
			BATTLE_ATTR_MENTAL , BATTLE_ATTR_RECOVERY, BATTLE_ATTR_CRITICAL, BATTLE_ATTR_DODGE , BATTLE_ATTR_HIT , BATTLE_ATTR_COUNTER_ATTACK , BATTLE_ATTR_MAGIC_DEFENSE ,
			BATTLE_ATTR_MAGIC_ATTACK, BATTLE_ATTR_CRITICAL_DEC_VALUE, BATTLE_ATTR_CRITICAL_INC_VALUE, BATTLE_ATTR_ANTI_POISON , BATTLE_ATTR_ANTI_PETRIFY, BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK, BATTLE_ATTR_ANTI_CHAOS, BATTLE_ATTR_ANTI_SILENCE, BATTLE_ATTR_ANTI_SEVERE_INJURE, BATTLE_ATTR_REAL_DAMAGE };
		for (int i = 0; i < HJZC_EQUIP_ATTR_LIMIT_MAX_NUM; i++)
		{
			AttrItemCfg item_cfg;
			item_cfg.attr_type = attr_limit_type_list[i];
			if (!GetSubNodeValue(root_element, attr_limit_name[i], item_cfg.attr_value))
			{
				return -__LINE__;
			}

			node.max_attr_cfg.push_back(item_cfg);
		}

		ActivityHuanJieZhanChangRobotAttrLimitCfgKey key;
		key.type = node.type;
		key.p1 = node.p1;

		m_robot_attr_limit_cfg[key] = node;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityHuanJieZhanChangConfig::InitRobotAttrAdjustCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -__LINE__;

	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "attr_adjust_group", group_id))
		{
			return -__LINE__;
		}

		HuanJieZhanChangRobotAttrAdjustCfg & cfg = m_huan_jie_robot_adjust_cfg[group_id];
		cfg.group_id = group_id;

		static int ADJUST_MIN = -100000;
		static int ADJUST_MAX = 100000;

		if (!GetSubNodeValue(dataElement, "role_gongji_adjust", cfg.role_attack_adjust) || cfg.role_attack_adjust <= ADJUST_MIN || cfg.role_attack_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "role_jingshen_adjust", cfg.role_mental_adjust) || cfg.role_mental_adjust <= ADJUST_MIN || cfg.role_mental_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "role_huifu_adjust", cfg.role_recover_adjust) || cfg.role_recover_adjust <= ADJUST_MIN || cfg.role_recover_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "role_maxhp_adjust", cfg.role_maxhp_adjust) || cfg.role_maxhp_adjust <= ADJUST_MIN || cfg.role_maxhp_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "pet_gongji_adjust", cfg.pet_attack_adjust) || cfg.pet_attack_adjust <= ADJUST_MIN || cfg.pet_attack_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "pet_jingshen_adjust", cfg.pet_mental_adjust) || cfg.pet_mental_adjust <= ADJUST_MIN || cfg.pet_mental_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "pet_huifu_adjust", cfg.pet_recover_adjust) || cfg.pet_recover_adjust <= ADJUST_MIN || cfg.pet_recover_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "pet_maxhp_adjust", cfg.pet_maxhp_adjust) || cfg.pet_maxhp_adjust <= ADJUST_MIN || cfg.pet_maxhp_adjust > ADJUST_MAX)
		{
			return -__LINE__;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityHuanJieZhanChangConfig::InitRobotCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -__LINE__;
	}

	int last_robot_id = 0;
	while (dataElement)
	{
		HuanJieZhanChangRobotCfg  cfg;
		if (!GetSubNodeValue(dataElement, "id", cfg.id) || cfg.id != last_robot_id + 1)
		{
			return -__LINE__;
		}
		last_robot_id = cfg.id;

		if (!GetSubNodeValue(dataElement, "min_score", cfg.min_score))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "max_score", cfg.max_score))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "level_max", cfg.level) || cfg.level <= 0 || cfg.level > MAX_ROLE_LEVEL)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "attr_adjust_group", cfg.attr_adjust_group) || NULL == this->GetHuanJieRobotAttrAdjustCfg(cfg.attr_adjust_group))
		{
			return -__LINE__;
		}

		m_huan_jie_robot_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	if (m_huan_jie_robot_cfg.empty())
	{
		return -__LINE__;
	}

	return 0;
}

int ActivityHuanJieZhanChangConfig::InitTodayRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -__LINE__;
	}

	int last_robot_id = 0;
	while (dataElement)
	{
		HuanJieTodayWinRewardCfg  cfg;
		if (!GetSubNodeValue(dataElement, "seq", cfg.seq) || cfg.seq < 0 || cfg.seq >= 30)
		{
			return -__LINE__;
		}


		if (!GetSubNodeValue(dataElement, "victory_field", cfg.need_win_times))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "exp", cfg.exp))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "coin", cfg.coin))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "huanjie_money", cfg.huan_jie_coin))
		{
			return -__LINE__;
		}

		ReadItemConfigDataList(dataElement,"reward",cfg.rewards);

		m_huan_jie_today_win_reward_cfg[cfg.seq] = cfg;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityHuanJieZhanChangConfig::GetAttrLimitKey(int limit_type, int job_type, int main_attr_type)
{
	return HZJC_ATTR_LIMIT_MAX_NUM * limit_type + HZJC_ATTR_JOB_LIMIT_MAX_NUM * job_type + main_attr_type;
}
