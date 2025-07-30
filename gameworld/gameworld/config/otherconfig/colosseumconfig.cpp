#include "colosseumconfig.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "gameworld/config/logicconfigmanager.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/struct/moneyotherdef.h"

ColosseumConfig::ColosseumConfig() : m_robot_level_days_cfg_num(0), m_robot_attr_adjust_cfg_num(0), m_total_rate(0)
{
}

ColosseumConfig::~ColosseumConfig()
{
	std::vector<ColosseumRefreshCfg>().swap(m_refresh_cfg);
	std::vector<ColosseumHistoryRankRewardCfg>().swap(m_history_rank_reward_cfg_vec);
	std::vector<ColossumBattleRewardCfg>().swap(m_battle_reward_cfg);
}

bool ColosseumConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("arena", InitColosseumCfg);
	LOAD_CONFIG("reward", InitRankAwardCfg);
	LOAD_CONFIG("robot_cfg", InitRobotLevelCfg);
	LOAD_CONFIG("buff_group", InitBuffCfg);
	LOAD_CONFIG("rank_section", InitRefreshCfg);
	LOAD_CONFIG("attr_adjust_group", InitAttrAdjustGroupCfg);
	LOAD_CONFIG("arena_achievement", InitArenaAchievementCfg);
	LOAD_CONFIG("history_rank_reward", InitHistoryRankRewardCfg);
	LOAD_CONFIG("battle_reward", InitBattleRewardCfg);

	return true;
}

void ColosseumConfig::GetRankAwardListSeq(std::set<int> & out_list)
{
	out_list.clear();
	for (std::map<int, ColosseumAwardCfg>::iterator loop = m_rank_award_cfg.begin(); loop != m_rank_award_cfg.end(); ++loop)
	{
		out_list.insert(loop->first);
	}
}

const ColosseumCfg * ColosseumConfig::GetColosseumCfg(int level) const
{
	for (std::map<int, ColosseumCfg>::const_iterator it = m_colosseum_cfg.begin();it != m_colosseum_cfg.end();++it)
	{
		if (level >= (it->first / 1000) && level <= (it->first % 1000))
		{
			return &it->second;
		}
	}
	return NULL;
}

const ColosseumAwardCfg * ColosseumConfig::GetAwardCfg(int award_seq) const
{
	std::map<int, ColosseumAwardCfg>::const_iterator finder_it = m_rank_award_cfg.find(award_seq);
	if (m_rank_award_cfg.end() != finder_it) return & finder_it->second;

	return NULL;
}

const ColosseumAwardCfg * ColosseumConfig::GetRandRewardCfg(int rank) const
{
	for (std::map<int, ColosseumAwardCfg>::const_iterator iter = m_rank_award_cfg.begin(); iter != m_rank_award_cfg.end(); ++iter)
	{
		const ColosseumAwardCfg & cfg = iter->second;
		if (cfg.min_rank <= rank && rank <= cfg.max_rank)
		{
			return &cfg;
		}
	}

	return NULL;
}

bool BinarySearchByRank(const ColosseumRobotLevelCfg* cfg_list, int cfg_list_size, int rank, ARG_OUT int* out_index);
int ColosseumConfig::RandRobotLevelByRank(int open_server_days, int rank) const
{
	for (int i = 0; i < m_robot_level_days_cfg_num; ++i)
	{
		if (open_server_days <= m_robot_days_level_cfg_list[i].open_days)
		{
			const ColosseumRobotDaysLevelCfg& cfg = m_robot_days_level_cfg_list[i];
			int index = 0;
			if (BinarySearchByRank(cfg.robot_level_cfg_list, cfg.robot_level_cfg_num, rank, &index))
			{
				return RandomNum(cfg.robot_level_cfg_list[index].level_min, cfg.robot_level_cfg_list[index].level_max + 1);
			}
		}
	}

	const FunOpenCfg* fun_open_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_COLOSSEUM);
	if (NULL != fun_open_cfg)
	{
		return fun_open_cfg->limit_level;
	}

	return 18;
}

const ColosseumRefreshCfg * ColosseumConfig::GetRefreshCfg(int rank)
{
	for (std::vector<ColosseumRefreshCfg> ::reverse_iterator iter = m_refresh_cfg.rbegin(); iter != m_refresh_cfg.rend(); ++iter)
	{
		if (rank >= iter->rank)
		{
			return &*iter;
		}
	}

	return NULL;
}

void ColosseumConfig::GetRandDefenderBuffId(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_buff_list, int server_open_day)
{
	if (NULL == out_num || NULL == out_buff_list)
	{
		return;
	}

	*out_num = 0;
	if (server_open_day < m_other_cfg.buff_open_server_days) return; // 未到服务器开启天数 不开启

	std::set<int> already_used_buff;
	for (int i = 0; i < max_num && *out_num < max_num; ++i) // 最多执行max_num次
	{
		ColosseumBuffCfg buff_temp;
		buff_temp.rate_count = 0;
		for (size_t k = 0; k < m_buff_cfg.buff_list.size(); ++k)
		{
			if (already_used_buff.find(m_buff_cfg.buff_list[k].buff_id) != already_used_buff.end())
			{
				continue;
			}
			buff_temp.buff_list.push_back(m_buff_cfg.buff_list[k]);
			buff_temp.rate_count += m_buff_cfg.buff_list[k].rate;
		}

		int buff_id = this->GetRandDefenderBuffId(buff_temp);
		if (0 == buff_id)
		{
			break;
		}

		out_buff_list[*out_num] = buff_id;
		*out_num += 1;
		already_used_buff.insert(buff_id);
	}
}

int ColosseumConfig::GetRandDefenderBuffId(const ColosseumBuffCfg& cfg)
{
	if (cfg.rate_count != 0)
	{
		int r = RandomNum(cfg.rate_count);
		for (int i = 0; i < (int)cfg.buff_list.size(); ++i)
		{
			if (r < cfg.buff_list[i].rate)
			{
				return cfg.buff_list[i].buff_id;
			}

			r -= cfg.buff_list[i].rate;
		}
	}

	return 0;
}

int ColosseumConfig::GetMaxHistoryRank()
{
	assert(!m_history_rank_reward_cfg_vec.empty());

	return m_history_rank_reward_cfg_vec[m_history_rank_reward_cfg_vec.size() - 1].rank_tail;
}

int ColosseumConfig::GetTitleIdByRank(int rank)
{
	if (rank == 1)
	{
		return m_other_cfg.top_1_title_id;
	}


	if (1 < rank && rank <= 10)
	{
		return m_other_cfg.top_10_title_id;
	}

	return 0;
}

int ColosseumConfig::GetRobotShowLevelByRankAndOpenServerDays(int rank, int open_server_days)
{
	const ColosseumRobotLevelCfg* cfg = this->GetRobotLevelCfgByRankAndOpenServerDays(rank, open_server_days);
	if (NULL != cfg)
	{
		return cfg->show_level;
	}

	const FunOpenCfg* fun_open_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_COLOSSEUM);
	if (NULL != fun_open_cfg)
	{
		return fun_open_cfg->limit_level;
	}

	return 18;
}

bool BinarySearchByRank(const ColosseumRobotLevelCfg* cfg_list, int cfg_list_size, int rank, ARG_OUT int* out_index)
{
	if (NULL == cfg_list) return false;

	int low_index = 0;
	int high_index = cfg_list_size - 1;

	while (low_index <= high_index)
	{
		int mid_index = (low_index + high_index) / 2;
		if (cfg_list[mid_index].rank_min <= rank && rank <= cfg_list[mid_index].rank_max)
		{
			if (NULL != out_index)
			{
				*out_index = mid_index;
				return true;
			}
		}
		else if (rank < cfg_list[mid_index].rank_min)
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

const ColosseumRobotLevelCfg* ColosseumConfig::GetRobotLevelCfgByRankAndOpenServerDays(int rank, int open_server_days)
{
	for (int i = 0; i < m_robot_level_days_cfg_num; ++i)
	{
		if (open_server_days <= m_robot_days_level_cfg_list[i].open_days)
		{
			const ColosseumRobotDaysLevelCfg& cfg = m_robot_days_level_cfg_list[i];
			int index = 0;
			if (BinarySearchByRank(cfg.robot_level_cfg_list, cfg.robot_level_cfg_num, rank, &index))
			{
				return &cfg.robot_level_cfg_list[index];
			}
		}
	}

	return NULL;
}

bool BinarySearchByGroupId(const ColosseumRobotAttrAdjustCfg* cfg_list, int cfg_list_size, int group_id, ARG_OUT int* out_index)
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

const ColosseumRobotAttrAdjustCfg* ColosseumConfig::GetRobotAttrAdjustCfg(int group_id)
{
	int index = 0;
	if (BinarySearchByGroupId(m_robot_attr_adjust_list, m_robot_attr_adjust_cfg_num, group_id, &index))
	{
		return &m_robot_attr_adjust_list[index];
	}

	return NULL;
}

const ColosseumAchievementCfg * ColosseumConfig::GetAchievementCfg(int sn)
{
	std::map<int, ColosseumAchievementCfg>::iterator it = m_achievement_map.find(sn);
	if (it != m_achievement_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const ColossumBattleRewardCfg * ColosseumConfig::RandBattleRewardCfg()
{
	int real_rate = RandomNum(m_total_rate);

	for (size_t i = 0; i < m_battle_reward_cfg.size(); i++)
	{
		if (real_rate < m_battle_reward_cfg[i].rate)
		{
			return &m_battle_reward_cfg[i];
		}
	}

	return NULL;
}

int ColosseumConfig::CalcHistoryRankReward(int old_rank, int change_rank_num)
{
	if (-1 == old_rank)
	{
		old_rank = this->GetMaxHistoryRank();
	}

	int tmp_rank = old_rank;
	int tmp_change_num = change_rank_num;
	int ret_num = 0;
	for (int i = (int)m_history_rank_reward_cfg_vec.size() - 1; i >= 0 && tmp_change_num > 0; --i)
	{
		if (tmp_rank > m_history_rank_reward_cfg_vec[i].rank_tail)
		{
			break;
		}

		if (tmp_rank < m_history_rank_reward_cfg_vec[i].rank_head)
		{
			continue;
		}

		if (tmp_rank - m_history_rank_reward_cfg_vec[i].rank_head + 1 <= tmp_change_num)
		{
			ret_num += (tmp_rank - m_history_rank_reward_cfg_vec[i].rank_head + 1) * m_history_rank_reward_cfg_vec[i].reward_score_num;
			tmp_change_num -= (tmp_rank - m_history_rank_reward_cfg_vec[i].rank_head + 1);
			tmp_rank = m_history_rank_reward_cfg_vec[i].rank_head - 1;
		}
		else
		{
			ret_num += tmp_change_num * m_history_rank_reward_cfg_vec[i].reward_score_num;
			tmp_rank -= tmp_change_num;
			tmp_change_num = 0;
		}
	}

	return ret_num;
}

int ColosseumConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		if (!GetSubNodeValue(dataElement, "challenge_times", m_other_cfg.challenge_times) || m_other_cfg.challenge_times < 0) return -1;
		if (!GetSubNodeValue(dataElement, "reward_pool", m_other_cfg.reward_pool_coin) || m_other_cfg.reward_pool_coin <= 0) return -4;
		if (!GetSubNodeValue(dataElement, "copy_percent", m_other_cfg.copy_attr_perc) || m_other_cfg.copy_attr_perc <= 0) return -5;
		if (!GetSubNodeValue(dataElement, "min_num", m_other_cfg.min_pool_num) || m_other_cfg.min_pool_num < 0) return -6;
		if (!GetSubNodeValue(dataElement, "min_coin", m_other_cfg.bottom_pool_coin) || m_other_cfg.bottom_pool_coin < 0) return -7;
		if (!GetSubNodeValue(dataElement, "refresh_time", m_other_cfg.client_refresh_interval_s) || m_other_cfg.client_refresh_interval_s < 3) return -8;
		if (!GetSubNodeValue(dataElement, "integral_pool", m_other_cfg.max_colosseum_pool_credits) || m_other_cfg.max_colosseum_pool_credits < 0) return -9;
		if (!GetSubNodeValue(dataElement, "integral_coin", m_other_cfg.min_colosseum_pool_credits) || m_other_cfg.min_colosseum_pool_credits < 0 || m_other_cfg.min_colosseum_pool_credits>m_other_cfg.max_colosseum_pool_credits) return -10;
		if (!GetSubNodeValue(dataElement, "end_round", m_other_cfg.battle_max_round) || m_other_cfg.battle_max_round < 0) return -11;
		if (!GetSubNodeValue(dataElement, "top_one_title", m_other_cfg.top_1_title_id)) return -12;
		if (!GetSubNodeValue(dataElement, "top_ten_title", m_other_cfg.top_10_title_id)) return -13;
		if (!GetSubNodeValue(dataElement, "refresh_intervar", m_other_cfg.refresh_intervar)) return -14;
		if (!GetSubNodeValue(dataElement, "buff_open_time", m_other_cfg.buff_open_server_days)) return -15;
		if (!GetSubNodeValue(dataElement, "battle_reward", m_other_cfg.battle_reward) || m_other_cfg.battle_reward < 0) return -16;
		if (!GetSubNodeValue(dataElement, "raiding_battle", m_other_cfg.raiding_battle) || m_other_cfg.raiding_battle < 0 || m_other_cfg.raiding_battle > 100) return -17;
		if (!GetSubNodeValue(dataElement, "open_level", m_other_cfg.open_level) || m_other_cfg.open_level < 0 || m_other_cfg.open_level > MAX_ROLE_LEVEL) return -18;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitColosseumCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		int min_level;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL)return -111;
		int max_level;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < 0 || max_level > MAX_ROLE_LEVEL)return -222;

		if (min_level > max_level) return -333;

		ColosseumCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "exp", node_cfg.exp) || node_cfg.exp < 0)return -1;
		if (!GetSubNodeValue(dataElement, "bind_coin", node_cfg.bind_coin) || node_cfg.bind_coin < 0)return -2;

		m_colosseum_cfg[min_level * 1000 + max_level] = node_cfg;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitRankAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_max_rank = 0;
	while (dataElement)
	{
		int award_seq; ColosseumAwardCfg tmp_cac;
		if (!GetSubNodeValue(dataElement, "seq", award_seq) || award_seq < 0 || award_seq >= BIT_COUNT_OF_LONG_64)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "min_rank", tmp_cac.min_rank) || tmp_cac.min_rank <= last_max_rank)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "max_rank", tmp_cac.max_rank) || tmp_cac.max_rank < tmp_cac.min_rank)
		{
			return -3;
		}
		last_max_rank = tmp_cac.max_rank;
		if (!GetSubNodeValue(dataElement, "bind_coin", tmp_cac.award_coin) || tmp_cac.award_coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "rank_price", tmp_cac.colosseum_credits) || tmp_cac.colosseum_credits < 0)
		{
			return -6;
		}

		TiXmlElement * award_list_node = dataElement->FirstChildElement("reward_list");
		if (NULL != award_list_node)		// 允许列表为空
		{
			TiXmlElement * award_node = award_list_node->FirstChildElement("reward");
			if (NULL == award_node) return -200;

			while (NULL != award_node)
			{
				if ((int)tmp_cac.award_list.size() >= CHALLENGE_FIELD_RANK_AWARD_MAX_NUM) return -6;

				ItemConfigData reward;
				if (!reward.ReadConfig(award_node))
				{
					return -7;
				}

				tmp_cac.award_list.push_back(reward);
				award_node = award_node->NextSiblingElement();
			}
		}

		m_rank_award_cfg[award_seq] = tmp_cac;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitRobotLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_rank_min = 0;
	int last_rank_max = 0;
	int last_days = -1;

	while (dataElement)
	{
		int days = -1;
		if (!GetSubNodeValue(dataElement, "time", days) || (0 != days && days < last_days))
		{
			return -111;
		}
		if (last_days != days)
		{
			last_rank_min = 0;
			last_rank_max = 0;
			if (m_robot_level_days_cfg_num >= MAX_DAYS_NUM)
			{
				return -112;
			}
			
			m_robot_level_days_cfg_num += 1;
		}
		last_days = days;

		ColosseumRobotDaysLevelCfg& cfg = m_robot_days_level_cfg_list[m_robot_level_days_cfg_num - 1];
		if (0 == days)
		{
			cfg.open_days = MAX_INT;
		}
		else
		{
			cfg.open_days = days;
		}

		int rank_min = 0;
		if (!GetSubNodeValue(dataElement, "rank_min", rank_min) || rank_min != last_rank_max + 1)
		{
			return -1;
		}
		last_rank_min = rank_min;

		int rank_max = 0;
		if (!GetSubNodeValue(dataElement, "rank_max", rank_max) || rank_max < rank_min)
		{
			return -2;
		}
		last_rank_max = rank_max;

		int level_max = 0;
		if (!GetSubNodeValue(dataElement, "level_max", level_max) || level_max < 0 || level_max >= MAX_ROLE_LEVEL)
		{
			return -3;
		}

		int level_min = 0;
		if (!GetSubNodeValue(dataElement, "level_min", level_min) || level_min < 0 || level_min >= MAX_ROLE_LEVEL || level_min > level_max)
		{
			return -4;
		}

		int level_show = 0;
		if (!GetSubNodeValue(dataElement, "level_show", level_show) || level_show <= 0 || level_show > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		int attr_adjust_group = 0;
		if (!GetSubNodeValue(dataElement, "attr_adjust_group", attr_adjust_group) || attr_adjust_group < 0)
		{
			return -6;
		}

		int buff_group = 0;
		if (!GetSubNodeValue(dataElement, "buff_group", buff_group) || buff_group < 0)
		{
			return -7;
		}

		if (cfg.robot_level_cfg_num >= MAX_COLOSSEUM_ROBOT_LEVEL_CFG_NUM) return -999;

		static ColosseumRobotLevelCfg lv_cfg;
		lv_cfg.level_max = level_max;
		lv_cfg.level_min = level_min;
		lv_cfg.rank_max = rank_max;
		lv_cfg.rank_min = rank_min;
		lv_cfg.show_level = level_show;
		lv_cfg.attr_adjust_group = attr_adjust_group;
		lv_cfg.buff_group = buff_group;

		cfg.robot_level_cfg_list[cfg.robot_level_cfg_num++] = lv_cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitBuffCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;



	while (dataElement)
	{
		ColosseumBuffCfg::ItemCfg cfg;


		if (!GetSubNodeValue(dataElement, "buff_id", cfg.buff_id) || cfg.buff_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "rate", cfg.rate) || cfg.rate <= 0)
		{
			return -2;
		}

		m_buff_cfg.rate_count += cfg.rate;
		m_buff_cfg.buff_list.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	if (m_buff_cfg.rate_count <= 0)
	{
		return -998;
	}

	return 0;
}

int ColosseumConfig::InitRefreshCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;


	int last_rank = 0;
	while (dataElement)
	{
		ColosseumRefreshCfg cfg;
		if (!GetSubNodeValue(dataElement, "rank", cfg.rank) || cfg.rank <= last_rank)
		{
			return -1;
		}
		last_rank = cfg.rank;

		int last_refresh_num = 0;
		for (int i = 1; i <= CHALLENGE_FIELD_OPPONENT_MAX; ++i)
		{
			char str[1024] = { 0 };
			SNPRINTF(str, sizeof(str), "position_%d", i);
			ColosseumRefreshCfg::ItemCfg item_cfg;

			int num = 0;
			if (!GetSubNodeValue(dataElement, str, num) || num <= last_refresh_num)
			{
				return -111;
			}
			last_refresh_num = num;
			cfg.refresh_num.push_back(num / 100.0);
		}
		m_refresh_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitAttrAdjustGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_group_id = 0;
	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "attr_adjust_group", group_id) || group_id < 0 || group_id != last_group_id + 1)
		{
			return -1;
		}
		last_group_id = group_id;

		ColosseumRobotAttrAdjustCfg& cfg = m_robot_attr_adjust_list[m_robot_attr_adjust_cfg_num];
		cfg.group_id = group_id;

		static int ADJUST_MIN = -COLOSSEUM_ROBOT_ATTR_ADJUST_PERCENT;
		static int ADJUST_MAX = COLOSSEUM_ROBOT_ATTR_ADJUST_PERCENT;

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

int ColosseumConfig::InitArenaAchievementCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_sn = 0;
	while (dataElement)
	{
		int sn = 0;
		if (!GetSubNodeValue(dataElement, "sn", sn) || sn >= MAX_COLOSSEUM_TASK_NUM)
		{
			return -1;
		}
		last_sn = sn;

		ColosseumAchievementCfg& cfg = m_achievement_map[sn];
		cfg.sn = sn;

		if (!GetSubNodeValue(dataElement, "ach_group", cfg.ach_group))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "ach_param1", cfg.ach_param1) || cfg.ach_param1 < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "progress_way", cfg.progress_way) || cfg.progress_way < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "achieve_type", cfg.achieve_type) || cfg.achieve_type < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "price_type", cfg.price_type) || cfg.price_type < MONEY_TYPE_GOLD || cfg.price_type >= MONEY_TYPE_MAX)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "price_num", cfg.price_num) || cfg.price_num < 0)
		{
			return -7;
		}
		TiXmlElement * item_list_Element = dataElement->FirstChildElement("item_list_list");
		if (item_list_Element != NULL)
		{
			TiXmlElement * item_Element = item_list_Element->FirstChildElement("item_list");
			while(NULL != item_Element)
			{
				ItemConfigData node;
				if (!node.ReadConfig(item_Element))
				{
					return -6;
				}
				cfg.item_list.push_back(node);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ColosseumConfig::InitHistoryRankRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		ColosseumHistoryRankRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "rank_high", cfg.rank_head) || cfg.rank_head < 1)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "rank_low", cfg.rank_tail) || cfg.rank_tail < cfg.rank_head)
		{
			return -2;
		}

		if (!m_history_rank_reward_cfg_vec.empty())
		{
			int last_idx = m_history_rank_reward_cfg_vec.size() - 1;
			if (cfg.rank_head != m_history_rank_reward_cfg_vec[last_idx].rank_tail + 1)
			{
				return -22;
			}

			if (cfg.reward_score_num >= m_history_rank_reward_cfg_vec[last_idx].reward_score_num)
			{
				return -23;
			}
		}

		if (!GetSubNodeValue(dataElement, "reward", cfg.reward_score_num) || cfg.reward_score_num <= 0)
		{
			return -3;
		}

		m_history_rank_reward_cfg_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	for (size_t i = 0; i < m_history_rank_reward_cfg_vec.size(); ++i)
	{
		// 服务器排名从0开始
		m_history_rank_reward_cfg_vec[i].rank_head -= 1;
		m_history_rank_reward_cfg_vec[i].rank_tail -= 1;
	}

	return 0;
}

int ColosseumConfig::InitBattleRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -544;

	while (dataElement)
	{
		ColossumBattleRewardCfg reward_cfg;

		if (!GetSubNodeValue(dataElement, "battle_price", reward_cfg.battle_price) || reward_cfg.battle_price < 0) return -101;
		if (!GetSubNodeValue(dataElement, "integral", reward_cfg.integral) || reward_cfg.integral < 0) return -1;
		if (!GetSubNodeValue(dataElement, "bind_coin", reward_cfg.bind_coin) || reward_cfg.bind_coin < 0) return -2;

		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate < 0) return -3;
		m_total_rate += rate;
		reward_cfg.rate = m_total_rate;

		TiXmlElement* reward_list_element = dataElement->FirstChildElement("item_list_list");
		if (NULL != reward_list_element)
		{ 
			TiXmlElement* reward_element = reward_list_element->FirstChildElement("item_list");
			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_element)) return -4;

				reward_cfg.reward_item_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_battle_reward_cfg.push_back(reward_cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	if (m_battle_reward_cfg.empty()) return -5;

	return 0;
}
