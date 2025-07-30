#include "cloudarenaconfig.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "checkresourcecenter.hpp"

CloudArenaConfig::CloudArenaConfig() : m_refresh_cost_cfg_num(0), m_buff_group_num(0), m_robot_cfg_num(0),
	m_robot_attr_adjust_cfg_num(0), m_rank_cfg_num(0), m_score_cfg_num(0), m_season_reward_cfg_num(0)
{
	
}

CloudArenaConfig::~CloudArenaConfig()
{
	std::vector<CloudArenaWorldLevelAttrAdjustGroupHash>().swap(m_world_level_attr_adjust_group_hash);
}

bool CloudArenaConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("others", InitOtherCfg);
	LOAD_CONFIG("coefficient_wave", InitFloatFactorCfg);
	LOAD_CONFIG("position", InitMatchScoreFactorCfg);
	LOAD_CONFIG("buff_group", InitBuffGroupCfg);
	LOAD_CONFIG("attr_adjust_group", InitRobotAttrAdjustCfg);
	LOAD_CONFIG("robot_cfg", InitRobotCfg);
	LOAD_CONFIG("ranking_cfg", InitRankingCfg);
	LOAD_CONFIG("challenge_integral", InitScoreCfg);
	LOAD_CONFIG("ranking_reward", InitSeasonRewardCfg);
	LOAD_CONFIG("achievement", InitAchievementCfg);

	//for (int score = 1000; score <= 5000; score += 100)
	//{
	//	printf("score=%d season_score=%d\n", score, this->GetSeasonReduceToScore(score));
	//}

	return true;
}

const CloudArenaOtherCfg& CloudArenaConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int CloudArenaConfig::GetPositionFactor(int opponent_idx)
{
	if (opponent_idx < 0 || opponent_idx >= CLOUD_ARENA_OPPONENT_NUM) return 0;

	return m_match_score_factor_cfg.position_factor_list[opponent_idx];
}

void CloudArenaConfig::CalculateMatchFactor(int opponent_idx, bool last_fight_win, bool last_fight_fail, int manual_refresh_times, ARG_OUT int* low_match_factor, ARG_OUT int* high_match_factor)
{
	int position_factor = this->GetPositionFactor(opponent_idx);

	int float_factor = 0;
	if (last_fight_win)
	{
		float_factor += m_float_factor_cfg.win_value;
	}

	if (last_fight_fail)
	{
		float_factor += m_float_factor_cfg.fail_value;
	}

	float_factor += m_float_factor_cfg.refresh_value * manual_refresh_times;

	if (abs(float_factor) >= m_other_cfg.match_wave_threshold)
	{
		if (float_factor > 0)
		{
			float_factor = m_other_cfg.match_wave_threshold;
		}
		else
		{
			float_factor = -m_other_cfg.match_wave_threshold;
		}
	}

	position_factor += float_factor;

	*low_match_factor = position_factor;
	*high_match_factor = position_factor + m_other_cfg.match_factor_range;
}

bool CloudArenaConfig::RandomDefenseBuff(ARG_OUT int* buff_id)
{
	const CloudArenaBuffGroupCfg* cfg = this->GetBuffGroupCfg(1);
	if (NULL == cfg) return false;

	int r = RandomNum(cfg->total_rate);
	int total_sum = 0;
	for (int i = 0; i < cfg->buff_num; ++i)
	{
		if (r < cfg->buff_list[i].rate + total_sum)
		{
			*buff_id = cfg->buff_list[i].buff_id;
			return true;
		}
		total_sum += cfg->buff_list[i].rate;
	}

	return false;
}

int CloudArenaConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "challenge_times", m_other_cfg.season_init_fight_times) || m_other_cfg.season_init_fight_times <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "max_challenge", m_other_cfg.max_store_fight_times) || m_other_cfg.max_store_fight_times < m_other_cfg.season_init_fight_times)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "challenge_expend", m_other_cfg.each_fight_consume_times) || m_other_cfg.each_fight_consume_times > m_other_cfg.max_store_fight_times)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "initial_score", m_other_cfg.initial_score) || m_other_cfg.initial_score < 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "min_score", m_other_cfg.min_score) || m_other_cfg.min_score < 0)
	{
		return -77;
	}

	if (!GetSubNodeValue(dataElement, "max_rounds", m_other_cfg.max_round) || m_other_cfg.max_round < 10)
	{
		return -8;
	}

	if (!GetSubNodeValue(dataElement, "level_limit", m_other_cfg.level_limit) || m_other_cfg.level_limit <= 0 || m_other_cfg.level_limit > MAX_ROLE_LEVEL)
	{
		return -9;
	}

	if (!GetSubNodeValue(dataElement, "refresh_time", m_other_cfg.refresh_opponent_cd_s) || m_other_cfg.refresh_opponent_cd_s <= 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(dataElement, "refresh", m_other_cfg.manual_refresh_times_limit) || m_other_cfg.manual_refresh_times_limit <= 0)
	{
		return -1010;
	}

	if (!GetSubNodeValue(dataElement, "rank_value", m_other_cfg.match_factor_range) || m_other_cfg.match_factor_range < 0)
	{
		return -11;
	}

	if (!GetSubNodeValue(dataElement, "rank_wave_threshold", m_other_cfg.match_wave_threshold) || m_other_cfg.match_wave_threshold < 0)
	{
		return -12;
	}

	if (!GetSubNodeValue(dataElement, "rank_wave_value_1", m_other_cfg.boundry_rank_wave_value) || m_other_cfg.boundry_rank_wave_value < 0)
	{
		return -14;
	}

	if (!GetSubNodeValue(dataElement, "reduce_score", m_other_cfg.season_reduce_score) || m_other_cfg.season_reduce_score < 0)
	{
		return -15;
	}

	if (!GetSubNodeValue(dataElement, "max_reset_score", m_other_cfg.max_season_initial_score) || m_other_cfg.max_season_initial_score < m_other_cfg.min_score)
	{
		return -16;
	}

	if (!GetSubNodeValue(dataElement, "accept_round", m_other_cfg.allow_surrender_round) || m_other_cfg.allow_surrender_round < 0)
	{
		return -17;
	}

	return 0;
}

int CloudArenaConfig::InitFloatFactorCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "refresh_value", m_float_factor_cfg.refresh_value))
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "fail_value", m_float_factor_cfg.fail_value))
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "win_value", m_float_factor_cfg.win_value))
	{
		return -3;
	}

	return 0;
}

int CloudArenaConfig::InitMatchScoreFactorCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_seq = -1;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0 || seq >= CLOUD_ARENA_NEW_OPPONENT_NUM || seq != last_seq + 1)
		{
			return -1;
		}
		last_seq = seq;

		int position_factor = 0;
		if (!GetSubNodeValue(dataElement, "position_coefficient", position_factor))
		{
			return -2;
		}
		m_match_score_factor_cfg.position_factor_list[seq] = position_factor;

		if (seq > 0 && m_match_score_factor_cfg.position_factor_list[seq] > m_match_score_factor_cfg.position_factor_list[seq - 1])
		{
			return -3;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int CloudArenaConfig::InitBuffGroupCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_group = 0;
	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "buff_group", group_id) || (last_group + 1 != group_id && last_group != group_id))
		{
			return -1;
		}
		last_group = group_id;

		int buff_id = 0;
		if (!GetSubNodeValue(dataElement, "buff_id", buff_id) || buff_id <= 0)
		{
			return -2;
		}

		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate < 0)
		{
			return -3;
		}

		CloudArenaBuffGroupCfg* group_cfg = this->MutableBuffGroupCfg(group_id);
		if (NULL == group_cfg)
		{
			if (m_buff_group_num >= ARRAY_ITEM_COUNT(m_buff_group_list))
			{
				return -44;
			}
			group_cfg = &m_buff_group_list[m_buff_group_num++];
			group_cfg->group_id = group_id;
		}

		if (group_cfg->buff_num >= ARRAY_ITEM_COUNT(group_cfg->buff_list))
		{
			return -55;
		}
		group_cfg->buff_list[group_cfg->buff_num].buff_id = buff_id;
		group_cfg->buff_list[group_cfg->buff_num].rate = rate;
		group_cfg->buff_num += 1;

		dataElement = dataElement->NextSiblingElement();
	}

	if (NULL == this->GetBuffGroupCfg(1))
	{
		return -999;
	}
	return 0;
}

int CloudArenaConfig::InitRobotCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_robot_id = 0;
	while (dataElement)
	{
		if (m_robot_cfg_num >= ARRAY_ITEM_COUNT(m_robot_cfg_list))
		{
			return -1000;
		}

		CloudArenaRobotCfg& cfg = m_robot_cfg_list[m_robot_cfg_num];
		if (!GetSubNodeValue(dataElement, "id", cfg.id ) || cfg.id != last_robot_id + 1)
		{
			return -1;
		}
		last_robot_id = cfg.id;

		if (!GetSubNodeValue(dataElement, "score", cfg.score) || cfg.score < m_other_cfg.initial_score)
		{
			return -2;
		}

		for (int i = 0; i < CLOUD_ARENA_ROBOT_ATTR_ADJUST_GROUP_NUM; ++i)
		{
			if (!GetSubNodeValue(dataElement, STRING_SPRINTF("attr_adjust_group_%d", i + 1).c_str(), cfg.attr_adjust_group_list[i]) 
				|| NULL == this->GetRobotAttrAdjustCfg(cfg.attr_adjust_group_list[i]))
			{
				return -5;
			}
		}	

		if (!GetSubNodeValue(dataElement, "pets_num", cfg.pet_helper_num) || cfg.pet_helper_num < 0 || cfg.pet_helper_num > 4)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "level_start", cfg.level_start) || cfg.level_start < 30 || cfg.level_start > MAX_ROLE_LEVEL)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "level_diff", cfg.world_level_diff))
		{
			return -8;
		}

		m_robot_cfg_num += 1;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int CloudArenaConfig::InitRobotAttrAdjustCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_group_id = 0;
	while (dataElement)
	{
		// 数量超过上限
		if (m_robot_attr_adjust_cfg_num >= ARRAY_LENGTH(m_robot_attr_adjust_list))
		{
			return -888;
		}

		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "attr_adjust_group", group_id) || group_id < 0 || group_id != last_group_id + 1)
		{
			return -1;
		}
		last_group_id = group_id;

		CloudArenaRobotAttrAdjustCfg& cfg = m_robot_attr_adjust_list[m_robot_attr_adjust_cfg_num];
		cfg.group_id = group_id;

		static int ADJUST_MIN = -10 * CLOUD_ARENA_ROBOT_ATTR_ADJUST_PERCENT;
		static int ADJUST_MAX = 10 * CLOUD_ARENA_ROBOT_ATTR_ADJUST_PERCENT;

		if (!GetSubNodeValue(dataElement, "role_gongji_adjust", cfg.role_attack_adjust) || cfg.role_attack_adjust <= ADJUST_MIN || cfg.role_attack_adjust > ADJUST_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "role_jingshen_adjust", cfg.role_mental_adjust) || cfg.role_mental_adjust <= ADJUST_MIN || cfg.role_mental_adjust > ADJUST_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "role_huifu_adjust", cfg.role_recover_adjust) || cfg.role_recover_adjust <= ADJUST_MIN || cfg.role_recover_adjust > ADJUST_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "role_maxhp_adjust", cfg.role_maxhp_adjust) || cfg.role_maxhp_adjust <= ADJUST_MIN || cfg.role_maxhp_adjust > ADJUST_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "pet_gongji_adjust", cfg.pet_attack_adjust) || cfg.pet_attack_adjust <= ADJUST_MIN || cfg.pet_attack_adjust > ADJUST_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "pet_jingshen_adjust", cfg.pet_mental_adjust) || cfg.pet_mental_adjust <= ADJUST_MIN || cfg.pet_mental_adjust > ADJUST_MAX)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "pet_huifu_adjust", cfg.pet_recover_adjust) || cfg.pet_recover_adjust <= ADJUST_MIN || cfg.pet_recover_adjust > ADJUST_MAX)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "pet_maxhp_adjust", cfg.pet_maxhp_adjust) || cfg.pet_maxhp_adjust <= ADJUST_MIN || cfg.pet_maxhp_adjust > ADJUST_MAX)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "partner_gongji_adjust", cfg.partner_attack_adjust) || cfg.partner_attack_adjust <= ADJUST_MIN || cfg.partner_attack_adjust > ADJUST_MAX)
		{
			return -10;
		}

		if (!GetSubNodeValue(dataElement, "partner_jingshen_adjust", cfg.partner_mental_adjust) || cfg.partner_mental_adjust <= ADJUST_MIN || cfg.partner_mental_adjust > ADJUST_MAX)
		{
			return -11;
		}

		if (!GetSubNodeValue(dataElement, "partner_huifu_adjust", cfg.partner_recover_adjust) || cfg.partner_recover_adjust <= ADJUST_MIN || cfg.partner_recover_adjust > ADJUST_MAX)
		{
			return -12;
		}

		if (!GetSubNodeValue(dataElement, "partner_maxhp_adjust", cfg.partner_maxhp_adjust) || cfg.partner_maxhp_adjust <= ADJUST_MIN || cfg.partner_maxhp_adjust > ADJUST_MAX)
		{
			return -13;
		}

		m_robot_attr_adjust_cfg_num += 1;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int CloudArenaConfig::InitRankingCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_rank_id = -1;
	int last_score = 0;
	while (dataElement)
	{
		if (m_rank_cfg_num >= MAX_RANK_CFG_NUM) return -1;

		CloudArenaRankCfg& cfg = m_rank_cfg_list[m_rank_cfg_num++];
		if (!GetSubNodeValue(dataElement, "ranking", cfg.rank_id) || cfg.rank_id != last_rank_id + 1)
		{
			return -2;
		}
		last_rank_id = cfg.rank_id;

		if (!GetSubNodeValue(dataElement, "score", cfg.rank_score) || cfg.rank_score <= last_score)
		{
			return -3;
		}
		last_score = cfg.rank_score;

		if (!GetSubNodeValue(dataElement, "rank", cfg.rank_dan_type) || cfg.rank_dan_type <= CLOUD_ARENA_DAN_TYPE_INVALID || cfg.rank_dan_type >= CLOUD_ARENA_DAN_TYPE_MAX)
		{
			return -33;
		}

		TiXmlElement* rankRewardElement = dataElement->FirstChildElement("ranking_reward_list");
		if (NULL != rankRewardElement)
		{
			cfg.rank_reward_num = 0;
			TiXmlElement* rewardElement = rankRewardElement->FirstChildElement("ranking_reward");
			while (rewardElement)
			{
				if (cfg.rank_reward_num >= ARRAY_ITEM_COUNT(cfg.rank_reward_list))
				{
					return -66;
				}
				if (!cfg.rank_reward_list[cfg.rank_reward_num].ReadConfig(rewardElement))
				{
					return -5;
				}
				cfg.rank_reward_num += 1;
				rewardElement = rewardElement->NextSiblingElement();
			}
		}	

		if (!GetSubNodeValue(dataElement, "ranking_reward_money", cfg.rank_reward_arena_money) || cfg.rank_reward_arena_money <= 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "day_reward_1", cfg.day_reward_arena_money) || cfg.day_reward_arena_money <= 0)
		{
			return -7;
		}
		
		if (!GetSubNodeValue(dataElement, "day_reward_2", cfg.day_reward_bind_coin) || cfg.day_reward_bind_coin <= 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "score_level", cfg.score_level) || cfg.score_level <= 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "reduce_score_min", cfg.season_min_score) || cfg.season_min_score < m_other_cfg.initial_score)
		{
			return -10;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	for (int i = 1; i < m_rank_cfg_num; ++i)
	{
		if (m_rank_cfg_list[i].rank_score <= m_rank_cfg_list[i - 1].rank_score)
		{
			return -100;
		}

		if (m_rank_cfg_list[i].season_min_score < m_rank_cfg_list[i - 1].season_min_score)
		{
			return -101;
		}
	}

	return 0;
}

int CloudArenaConfig::InitScoreCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_seq = -1;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != last_seq + 1)
		{
			return -1;
		}
		last_seq = seq;

		if (m_score_cfg_num >= ARRAY_ITEM_COUNT(m_score_cfg_list))
		{
			return -111;
		}

		CloudArenaScoreCfg& cfg = m_score_cfg_list[seq];
		
		if (!GetSubNodeValue(dataElement, "win_price", cfg.win_add_score) || cfg.win_add_score <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "win_money", cfg.win_add_money) || cfg.win_add_money <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "lose_money", cfg.lose_add_money) || cfg.lose_add_money <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "drawn_price", cfg.draw_add_score) || cfg.draw_add_score < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "drawn_money", cfg.draw_add_money) || cfg.draw_add_money < 0)
		{
			return -6;
		}

		m_score_cfg_num = seq + 1;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int CloudArenaConfig::InitSeasonRewardCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	
	m_season_reward_cfg_num = 0;
	int last_max_rank = 0;
	while (dataElement)
	{
		if (m_season_reward_cfg_num >= ARRAY_ITEM_COUNT(m_season_reward_cfg_list))
		{
			return -1;
		}

		CloudArenaSeasonRewardCfg& cfg = m_season_reward_cfg_list[m_season_reward_cfg_num++];

		if (!GetSubNodeValue(dataElement, "min_rank_pos", cfg.min_rank) || cfg.min_rank <= 0 || last_max_rank + 1 != cfg.min_rank)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "max_rank_pos", cfg.max_rank) || cfg.max_rank < cfg.min_rank)
		{
			return -3;
		}
		last_max_rank = cfg.max_rank;

		// 服务端排名从0开始
		cfg.min_rank -= 1;
		cfg.max_rank -= 1;

		if (!GetSubNodeValue(dataElement, "title", cfg.title_id) || cfg.title_id < 0)
		{
			return -33;
		}
		if (cfg.title_id > 0)
		{
			CheckResourceCenter::Instance().GetTitleCheck()->Add(cfg.title_id, __FUNCTION__);
		}

		TiXmlElement* rankRewardElement = dataElement->FirstChildElement("ranking_reward_item_list");
		if (NULL != rankRewardElement)
		{
			cfg.reward_num = 0;
			TiXmlElement* rewardElement = rankRewardElement->FirstChildElement("ranking_reward_item");
			while (rewardElement)
			{
				if (cfg.reward_num >= ARRAY_ITEM_COUNT(cfg.reward_list))
				{
					return -66;
				}
				if (!cfg.reward_list[cfg.reward_num].ReadConfig(rewardElement))
				{
					return -5;
				}
				cfg.reward_num += 1;
				rewardElement = rewardElement->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int CloudArenaConfig::InitAchievementCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");

	int last_achieve_level = -1;
	int last_achieve_type = -1;
	while (dataElement)
	{
		int achieve_type = 0;
		if (!GetSubNodeValue(dataElement, "achieve_type", achieve_type) 
			|| achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX)
		{
			return -1;
		}
		if (last_achieve_type != achieve_type)
		{
			last_achieve_type = achieve_type;
			last_achieve_level = -1;
			m_achieve_cfg_list[achieve_type].reserve(15);
		}

		int ach_level = 0;
		if (!GetSubNodeValue(dataElement, "ach_level", ach_level) || ach_level != last_achieve_level + 1)
		{
			return -2;
		}
		last_achieve_level = ach_level;

		CloudArenaAchievementCfg cfg;
		
		if (!GetSubNodeValue(dataElement, "ach_param1", cfg.progress_need) || cfg.progress_need <= 0)
		{
			return -3;
		}

		int is_hide = 0;
		if (!GetSubNodeValue(dataElement, "hide", is_hide) || is_hide < 0 || is_hide > 1)
		{
			return -33;
		}
		cfg.is_hide = (is_hide > 0);

		if (!GetSubNodeValue(dataElement, "money_num", cfg.reward_arena_coin) || cfg.reward_arena_coin < 0 || (cfg.reward_arena_coin == 0 && !cfg.is_hide))
		{
			return -4;
		}

		TiXmlElement* rankRewardElement = dataElement->FirstChildElement("item_list_list");
		if (NULL != rankRewardElement)
		{
			cfg.reward_num = 0;
			TiXmlElement* rewardElement = rankRewardElement->FirstChildElement("item_list");
			while (rewardElement)
			{
				if (cfg.reward_num >= ARRAY_ITEM_COUNT(cfg.reward_list))
				{
					return -66;
				}
				if (!cfg.reward_list[cfg.reward_num].ReadConfig(rewardElement))
				{
					return -5;
				}
				cfg.reward_num += 1;
				rewardElement = rewardElement->NextSiblingElement();
			}
		}
		m_achieve_cfg_list[achieve_type].push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	for (int type = 0; type < CLOUD_ARENA_ACHIEVEMENT_MAX; ++type)
	{
		if (m_achieve_cfg_list[type].empty())
		{
			return -999;
		}
	}

	return 0;
}

int CloudArenaConfig::InitWorldLevelAttrAdjustGroupHash(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	int last_group_index = -1;
	while (dataElement)
	{
		CloudArenaWorldLevelAttrAdjustGroupHash cfg;
		if (!GetSubNodeValue(dataElement, "level_min", cfg.level_min) || cfg.level_min <= 0 || cfg.level_max > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "level_max", cfg.level_max) || cfg.level_max < cfg.level_min || cfg.level_max > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "attr_adjust_group", cfg.group_index) || cfg.group_index <= 0 || cfg.group_index > CLOUD_ARENA_ROBOT_ATTR_ADJUST_GROUP_NUM)
		{
			return -3;
		}

		cfg.group_index -= 1;

		if (cfg.group_index != last_group_index + 1)
		{
			return -111;
		}

		if (!m_world_level_attr_adjust_group_hash.empty())
		{
			if (cfg.level_min != m_world_level_attr_adjust_group_hash[m_world_level_attr_adjust_group_hash.size() - 1].level_max + 1)
			{
				return -222;
			}
		}

		m_world_level_attr_adjust_group_hash.push_back(cfg);


		dataElement = dataElement->NextSiblingElement();
	}

	if (!m_world_level_attr_adjust_group_hash.empty())
	{
		if (1 != m_world_level_attr_adjust_group_hash[0].level_min)
		{
			return -333;
		}

		if (MAX_ROLE_LEVEL != m_world_level_attr_adjust_group_hash[m_world_level_attr_adjust_group_hash.size() - 1].level_max)
		{
			return -444;
		}
	}

	return 0;
}

CloudArenaBuffGroupCfg* CloudArenaConfig::MutableBuffGroupCfg(int group_id)
{
	for (int i = 0; i < m_buff_group_num; ++i)
	{
		if (group_id == m_buff_group_list[i].group_id)
		{
			return &m_buff_group_list[i];
		}
	}

	return NULL;
}

const CloudArenaBuffGroupCfg* CloudArenaConfig::GetBuffGroupCfg(int group_id)
{
	return this->MutableBuffGroupCfg(group_id);
}

bool BinarySearchByRobotId(const CloudArenaRobotCfg* cfg_list, int cfg_list_size, int robot_id, ARG_OUT int* out_index)
{
	if (NULL == cfg_list) return false;

	int low_index = 0;
	int high_index = cfg_list_size - 1;

	while (low_index <= high_index)
	{
		int mid_index = (low_index + high_index) / 2;
		if (robot_id == cfg_list[mid_index].id)
		{
			if (NULL != out_index)
			{
				*out_index = mid_index;
				return true;
			}
		}
		else if (robot_id < cfg_list[mid_index].id)
		{
			high_index = mid_index - 1;
		}
		else
		{
			low_index = mid_index + 1;
		}
	}

	return false;
}

const CloudArenaRobotCfg* CloudArenaConfig::GetRobotCfg(int id)
{
	int index = 0;
	if (BinarySearchByRobotId(m_robot_cfg_list, m_robot_cfg_num, id, &index))
	{
		return &m_robot_cfg_list[index];
	}

	return NULL;
}

bool BinarySearchByGroupId(const CloudArenaRobotAttrAdjustCfg* cfg_list, int cfg_list_size, int group_id, ARG_OUT int* out_index)
{
	if (NULL == cfg_list) return false;

	int low_index = 0;
	int high_index = cfg_list_size - 1;

	while (low_index <= high_index)
	{
		int mid_index = (low_index + high_index) / 2;
		if (group_id == cfg_list[mid_index].group_id)
		{
			if (NULL != out_index)
			{
				*out_index = mid_index;
				return true;
			}
		}
		else if (group_id < cfg_list[mid_index].group_id)
		{
			high_index = mid_index - 1;
		}
		else
		{
			low_index = mid_index + 1;
		}
	}

	return false;
}

const CloudArenaRobotAttrAdjustCfg* CloudArenaConfig::GetRobotAttrAdjustCfg(int group_id)
{
	int index = 0;
	if (BinarySearchByGroupId(m_robot_attr_adjust_list, m_robot_attr_adjust_cfg_num, group_id, &index))
	{
		return &m_robot_attr_adjust_list[index];
	}

	return NULL;
}

const CloudArenaRankCfg* CloudArenaConfig::GetRankCfg(int score)
{
	if (score < m_rank_cfg_list[0].rank_score) return &m_rank_cfg_list[0];
	
	int low = 0;
	int high = m_rank_cfg_num - 1;
	int result = -1;
	while (low <= high)
	{
		int mid = (low + high) / 2;
		if (m_rank_cfg_list[mid].rank_score <= score)
		{
			low = mid + 1;
			result = mid;
		}
		else
		{
			high = mid - 1;
		}
	}

	if (result != -1)
	{
		return &m_rank_cfg_list[result];
	}

	return NULL;
}

const CloudArenaRankCfg* CloudArenaConfig::GetRankCfgByRankId(int rank_id)
{
	int low = 0;
	int high = m_rank_cfg_num - 1;
	while (low <= high)
	{
		int mid = (low + high) / 2;
		if (m_rank_cfg_list[mid].rank_id == rank_id)
		{
			return &m_rank_cfg_list[mid];
		}
		else if (m_rank_cfg_list[mid].rank_id > rank_id)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}

	return NULL;
}

const CloudArenaScoreCfg* CloudArenaConfig::GetScoreCfg(int target_seq)
{
	if (target_seq < 0 || target_seq >= m_score_cfg_num) return NULL;

	return &m_score_cfg_list[target_seq];
}

void CloudArenaConfig::GetScoreAndArenaCoinReward(int target_seq, bool is_win, bool is_draw, ARG_OUT int* modify_score, ARG_OUT int* modify_arena_coin)
{
	const CloudArenaScoreCfg* cfg = this->GetScoreCfg(target_seq);
	if (NULL == cfg) return;

	if (is_win)
	{
		if (NULL != modify_arena_coin) *modify_arena_coin = cfg->win_add_money;
		if (NULL != modify_score) *modify_score = cfg->win_add_score;
	}
	else if (is_draw)
	{
		if (NULL != modify_arena_coin) *modify_arena_coin = cfg->draw_add_money;
		if (NULL != modify_score) *modify_score = cfg->draw_add_score;
	}
	else
	{
		if (NULL != modify_arena_coin) *modify_arena_coin = cfg->lose_add_money;
		if (NULL != modify_score) *modify_score = cfg->lose_add_score;
	}
}

int CloudArenaConfig::GetDanTypeByScore(int score)
{
	const CloudArenaRankCfg* rank_cfg = this->GetRankCfg(score);
	if (NULL == rank_cfg)
	{
		return CLOUD_ARENA_DAN_TYPE_NEWBIE;
	}

	return rank_cfg->rank_dan_type;
}

int CloudArenaConfig::GetDanIdByScore(int score)
{
	const CloudArenaRankCfg* rank_cfg = this->GetRankCfg(score);
	if (NULL == rank_cfg)
	{
		return 0;
	}

	return rank_cfg->rank_id;
}

int CloudArenaConfig::GetSeasonReduceToScore(int last_season_score)
{
	int season_min_score = m_other_cfg.initial_score;
	const CloudArenaRankCfg* rank_cfg = this->GetRankCfg(last_season_score);
	if (NULL != rank_cfg)
	{
		season_min_score = rank_cfg->season_min_score;
	}

	int ret_score = GetMax(season_min_score, last_season_score - m_other_cfg.season_reduce_score);
	ret_score = GetMin(ret_score, m_other_cfg.max_season_initial_score);
	return ret_score;
}

bool CloudArenaConfig::IsHighestDanId(int dan_id)
{
	assert(m_rank_cfg_num > 0);
	return dan_id == m_rank_cfg_list[m_rank_cfg_num - 1].rank_id;
}

const CloudArenaSeasonRewardCfg* CloudArenaConfig::GetSeasonRewardCfg(int rank)
{
	for (int i = 0; i < m_season_reward_cfg_num; ++i)
	{
		if (rank >= m_season_reward_cfg_list[i].min_rank && rank <= m_season_reward_cfg_list[i].max_rank)
		{
			return &m_season_reward_cfg_list[i];
		}
	}

	return NULL;
}

const CloudArenaAchievementCfg* CloudArenaConfig::GetAchievementCfg(int achieve_type, int achieve_level)
{
	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX) return NULL;
	if (achieve_level < 0 || achieve_level >= static_cast<int>(m_achieve_cfg_list[achieve_type].size())) return NULL;

	return &m_achieve_cfg_list[achieve_type][achieve_level];
}

int CloudArenaConfig::GetRobotAttrAdjustGroupIndexByWorldLevel(int world_level)
{
	for (int i = 0; i < static_cast<int>(m_world_level_attr_adjust_group_hash.size()); ++i)
	{
		if (world_level >= m_world_level_attr_adjust_group_hash[i].level_min &&
			world_level <= m_world_level_attr_adjust_group_hash[i].level_max)
		{
			return i;
		}
	}

	return 0;
}

