#include "activityworldteamarenaconfig.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/struct/moneyotherdef.h"

ActivityWorldTeamArenaConfig::~ActivityWorldTeamArenaConfig()
{
	std::vector<ActivityWorldTeamArenaLevelRankRewardCfg>().swap(m_rank_award_cfg);
	std::vector<Posi>().swap(m_born_list);
}

bool ActivityWorldTeamArenaConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	
	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("participation_reward", InitParticicpationCfg);
	LOAD_CONFIG_2("rank_reward", InitRankAwardCfg);
	LOAD_CONFIG_2("random_entrance", InitBornPosiCfg);

	return true;
}

const ActivityWorldTeamArenaParRewardCfg * ActivityWorldTeamArenaConfig::GetParRewardCfg(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	for (std::map<ParamSection, ActivityWorldTeamArenaParRewardCfg>::iterator it = m_particicpation_map.begin(); it != m_particicpation_map.end(); it++)
	{
		if (it->first.section_min <= level && it->first.section_max >= level)
		{
			return &it->second;
		}
	}

	return NULL;
}

const ActivityWorldTeamArenaRankRewardCfg * ActivityWorldTeamArenaConfig::GetRankRewardCfg(int rank, int role_level)
{
	for (int i = 0; i < (int)m_rank_award_cfg.size(); ++i)
	{
		const ActivityWorldTeamArenaLevelRankRewardCfg & curr = m_rank_award_cfg[i];
		if (curr.min_role_level <= role_level && role_level <= curr.max_role_level)
		{
			for (std::map<ParamSection, ActivityWorldTeamArenaRankRewardCfg>::const_iterator it = curr.rank_reward.begin(); it != curr.rank_reward.end(); it++)
			{
				if (it->first.section_min <= rank && it->first.section_max >= rank)
				{
					return &it->second;
				}
			}
		}
	}

	return NULL;
}

Posi ActivityWorldTeamArenaConfig::GetRandBornPosi()
{
	if(m_born_list.empty()) return Posi(0, 0);

	int rand_index = RandomNum((int)m_born_list.size());

	return m_born_list[rand_index];
}

int ActivityWorldTeamArenaConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "level", m_other_cfg.need_level) || m_other_cfg.need_level > MAX_ROLE_LEVEL)
	{
		errormsg = STRING_SPRINTF("[level=%d] error, is not find data or > %d?", m_other_cfg.need_level, MAX_ROLE_LEVEL);
		return -1;
	}

	if (!GetSubNodeValue(data_element, "map", m_other_cfg.scene_id))
	{
		errormsg = STRING_SPRINTF("[map=%d] error, is not find data?", m_other_cfg.scene_id);
		return -2;
	}
	CheckResourceCenter::Instance().GetSceneCheck()->Add(m_other_cfg.scene_id, __FUNCTION__);
	if (!GetSubNodeValue(data_element, "max_fail_num", m_other_cfg.max_fail_num) || m_other_cfg.max_fail_num <= 0)
	{
		errormsg = STRING_SPRINTF("[max_fail_num=%d] error, is not find data or <= 0?", m_other_cfg.max_fail_num);
		return -3;
	}
	if (!GetSubNodeValue(data_element, "max_rounds", m_other_cfg.max_rounds) || m_other_cfg.max_rounds <= 0)
	{
		errormsg = STRING_SPRINTF("[max_rounds=%d] error, is not find data or <= 0?", m_other_cfg.max_rounds);
		return -4;
	}
	if (!GetSubNodeValue(data_element, "win_score", m_other_cfg.win_score) || m_other_cfg.win_score <= 0)
	{
		errormsg = STRING_SPRINTF("[win_score=%d] error, is not find data or <= 0?", m_other_cfg.win_score);
		return -5;
	}
	if (!GetSubNodeValue(data_element, "lose_score", m_other_cfg.lose_score) || m_other_cfg.lose_score < 0)
	{
		errormsg = STRING_SPRINTF("[lose_score=%d] error, is not find data or < 0?", m_other_cfg.lose_score);
		return -6;
	}
	if (!GetSubNodeValue(data_element, "draw_score", m_other_cfg.draw_score) || m_other_cfg.draw_score < 0)
	{
		errormsg = STRING_SPRINTF("[draw_score=%d] error, is not find data or < 0?", m_other_cfg.draw_score);
		return -7;
	}
	if (!GetSubNodeValue(data_element, "refresh_time", m_other_cfg.refresh_time) || m_other_cfg.refresh_time <= 0)
	{
		errormsg = STRING_SPRINTF("[refresh_time=%d] error, is not find data or <= 0?", m_other_cfg.refresh_time);
		return -8;
	}
	if (!GetSubNodeValue(data_element, "countdown", m_other_cfg.countdown) || m_other_cfg.countdown <= 0)
	{
		errormsg = STRING_SPRINTF("[countdown=%d] error, is not find data or <= 0?", m_other_cfg.countdown);
		return -9;
	}
	if (!GetSubNodeValue(data_element, "registration_type", m_other_cfg.registration_type) || m_other_cfg.registration_type < 0 || 
		m_other_cfg.registration_type >= MONEY_TYPE_MAX)
	{
		errormsg = STRING_SPRINTF("[registration_type=%d] error, is not find data or < 0 or >= ?", m_other_cfg.registration_type, MONEY_TYPE_MAX);
		return -10;
	}
	if (!GetSubNodeValue(data_element, "registration_price", m_other_cfg.registration_price) || m_other_cfg.registration_price <= 0)
	{
		errormsg = STRING_SPRINTF("[registration_price=%d] error, is not find data or <= 0?", m_other_cfg.registration_price);
		return -11;
	}
	if (!GetSubNodeValue(data_element, "battle_protect", m_other_cfg.battle_protect) || m_other_cfg.battle_protect <= 0)
	{
		errormsg = STRING_SPRINTF("[battle_protect=%d] error, is not find data or <= 0?", m_other_cfg.battle_protect);
		return -12;
	}

	return 0;
}

int ActivityWorldTeamArenaConfig::InitParticicpationCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ParamSection section; 
		ActivityWorldTeamArenaParRewardCfg tmp_con;
		if (!GetSubNodeValue(root_element, "level_min", section.section_min) || section.section_min <= 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d] error, is not find data or <= 0?", section.section_min);
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level_max", section.section_max) || section.section_max < section.section_min || section.section_max > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("[level_max=%d] error, is not find data or <= [level_min:%d] or > %d?", section.section_max, section.section_min, MAX_ROLE_LEVEL);
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, is not find data[coin:%d] or < 0?", section.section_min, section.section_max, tmp_con.coin);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.exp) || tmp_con.exp < 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, is not find data[exp:%d] or < 0?", section.section_min, section.section_max, tmp_con.exp);
			return -4;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("join_reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("join_reward");
			if (NULL == award_node) 
			{
				errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, is not find data[join_reward_list]?", section.section_min, section.section_max,
					tmp_con.exp);
				return -100;
			}
			while (NULL != award_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(award_node))
				{	
					errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, is not find data[join_reward_list] or item[id:%d,num:%d] error?", section.section_min, 
						section.section_max, item.item_id, item.num);
					return -200;
				}
				tmp_con.reward_list.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}

		m_particicpation_map[section] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityWorldTeamArenaConfig::InitRankAwardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_section_max = 0;
	int last_max_role_level = 0;
	int last_min_role_level = 0;
	while (NULL != root_element)
	{
		int min_role_level = 0;
		if (!GetSubNodeValue(root_element, "level_min", min_role_level) || min_role_level <= 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d] error, is not find data or <= 0?", min_role_level);
			return -1001;
		}

		int max_role_level = 0;
		if (!GetSubNodeValue(root_element, "level_max", max_role_level) || max_role_level < min_role_level)
		{
			errormsg = STRING_SPRINTF("[level_max=%d] error, is not find data or <= 0?", max_role_level);
			return -1001;
		}

		if (last_max_role_level != max_role_level || last_min_role_level != min_role_level)
		{
			if (min_role_level <= last_max_role_level)
			{
				errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, last_level[min:%d,max%d]?", min_role_level, max_role_level, 
					last_min_role_level, last_max_role_level);
				return -1002;
			}

			last_min_role_level = min_role_level;
			last_max_role_level = max_role_level;
			last_section_max = 0;
			m_rank_award_cfg.push_back(ActivityWorldTeamArenaLevelRankRewardCfg());
		}

		ActivityWorldTeamArenaLevelRankRewardCfg & rank_reward = m_rank_award_cfg.back();
		rank_reward.min_role_level = min_role_level;
		rank_reward.max_role_level = max_role_level;

		ParamSection rank_sec;
		ActivityWorldTeamArenaRankRewardCfg tmp_con;
		if (!GetSubNodeValue(root_element, "rank_min", rank_sec.section_min) || rank_sec.section_min <= 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [rank_min:%d] not find data or <= 0?", min_role_level, max_role_level, rank_sec.section_min);
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rank_max", rank_sec.section_max) || rank_sec.section_max < rank_sec.section_min)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [rank_max:%d] not find data or < [rank_min:%d]?", min_role_level, max_role_level, 
				rank_sec.section_max, rank_sec.section_min);
			return -2;
		}
		if (0 != last_section_max && last_section_max + 1 != rank_sec.section_min)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, rank_min[min:%d, max:%d] last_rank_max[%d] cur_min != last_max + 1?", min_role_level,
				max_role_level, rank_sec.section_min, rank_sec.section_max, last_section_max);
			return -3;
		}
		last_section_max = rank_sec.section_max;

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [coin:%d] not find data or < 0?", min_role_level, max_role_level, tmp_con.coin);
			return -4;
		}

		if (!GetSubNodeValue(root_element, "title", tmp_con.title_id))
		{
			errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [title_id:%d] not find data?", min_role_level, max_role_level, tmp_con.title_id);
			return -5;
		}
		if (0 != tmp_con.title_id)
		{
			CheckResourceCenter::Instance().GetTitleCheck()->Add(tmp_con.title_id, __FUNCTION__);

			//首个等级区间
			if (1 == m_rank_award_cfg.size())
			{
				m_rank_reward_title_list_cfg[rank_sec.section_max] = tmp_con.title_id;
			}
			else
			{	
				std::map<int, int>::const_iterator it = m_rank_reward_title_list_cfg.find(rank_sec.section_max);
				if (it == m_rank_reward_title_list_cfg.end() || it ->second != tmp_con.title_id)
				{
					errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [title_id:%d] not consistent?", min_role_level, max_role_level, tmp_con.title_id);
					return -6;
				}
			}
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) 
			{
				errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, [reward_list] not find data or < 0?", min_role_level, max_role_level);
				return -100;
			}
			while (NULL != award_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(award_node))
				{
					errormsg = STRING_SPRINTF("[level_min=%d, level_max=%d] error, is not find data[reward_list] or item[id:%d,num:%d] error?", min_role_level, 
						max_role_level, item.item_id, item.num);
					return -200;
				}
				tmp_con.reward_list.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}

		rank_reward.rank_reward[rank_sec] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityWorldTeamArenaConfig::InitBornPosiCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		Posi born_pos;
		if (!GetSubNodeValue(root_element, "x", born_pos.x) || born_pos.x < 0)
		{
			errormsg = STRING_SPRINTF("[x=%d] error, not find data or < 0?", born_pos.x);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "y", born_pos.y) || born_pos.y < 0)
		{
			errormsg = STRING_SPRINTF("[y=%d] error, not find data or < 0?", born_pos.y);
			return -2;
		}

		m_born_list.push_back(born_pos);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
