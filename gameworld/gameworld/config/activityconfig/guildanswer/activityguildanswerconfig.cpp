#include "activityguildanswerconfig.hpp"
#include "world.h"

ActivityGuildAnswerConfig::ActivityGuildAnswerConfig()
{
}

ActivityGuildAnswerConfig::~ActivityGuildAnswerConfig()
{
	for (std::map<int, std::vector<GuildAnswerItemRand> >::iterator it = m_item_rand_group.begin(); it != m_item_rand_group.end(); it++)
	{
		std::vector<GuildAnswerItemRand>().swap(it->second);
	}
	for (std::map<int, std::vector<GuildAnswerFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<GuildAnswerFindRewardGroupCfg>().swap(it->second);
	}
}

bool ActivityGuildAnswerConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitOtherInformaCfg)
	LOAD_CONFIG("question_bank", InitQuestionListCfg)
	LOAD_CONFIG("item_reward", InitRandAwardGroupList)
	LOAD_CONFIG("right_reward", InitRightAnswerAward)
	LOAD_CONFIG("wrong_reward", InitWrongAnswerAward)
	LOAD_CONFIG("reward_find_group", InitGuildAnswerFindRewardGroupCfg)
	LOAD_CONFIG("reward_find", InitGuildAnswerFindCfg)

	return true;
}

const GuildAnswerReward * ActivityGuildAnswerConfig::GetAnswerReward(int level, bool is_correct)
{
	if (is_correct)
	{
		std::map<int, GuildAnswerReward>::iterator award_it = m_right_reward_map.find(level);
		if (m_right_reward_map.end() != award_it) return &award_it->second;
	}
	else
	{
		std::map<int, GuildAnswerReward>::iterator award_it = m_wrong_reward_map.find(level);
		if (m_wrong_reward_map.end() != award_it) return &award_it->second;
	}

	return NULL;
}

const ItemConfigData * ActivityGuildAnswerConfig::RandItemAward(int group_id)
{
	std::map<int, std::vector<GuildAnswerItemRand> >::iterator group_it = m_item_rand_group.find(group_id);
	if (m_item_rand_group.end() == group_it) return NULL;

	int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::vector<GuildAnswerItemRand>::iterator loop_it = group_it->second.begin(); loop_it != group_it->second.end(); ++loop_it)
	{
		if (rand_prob < loop_it->prob) return & loop_it->reward_item;
		rand_prob -= loop_it->prob;
	}

	return NULL;
}

int ActivityGuildAnswerConfig::GetRightAnswer(int question_id)
{
	std::map<int, int>::iterator question_it = m_question_answer_map.find(question_id);
	if (m_question_answer_map.end() != question_it) return question_it->second;

	return 0;
}

bool ActivityGuildAnswerConfig::RandQuestionList(std::vector<int> & out_list)
{
	out_list.clear();

	int question_list_size = (int)m_question_answer_map.size();
	if (question_list_size < m_guild_answer_other_cfg.question_numb) return false;

	std::set<int> tmp_insert_list;
	while ((int)out_list.size() < m_guild_answer_other_cfg.question_numb)
	{
		std::map<int, int>::iterator insert_it = m_question_answer_map.begin();

		int rand_idx = rand() % question_list_size;
		std::advance(insert_it, rand_idx);

		if (m_question_answer_map.end() != insert_it && tmp_insert_list.end() == tmp_insert_list.find(insert_it->first))
		{
			tmp_insert_list.insert(insert_it->first);
			out_list.push_back(insert_it->first);
		}
	}

	return true;
}

const GuildAnswerFindCfg * ActivityGuildAnswerConfig::GetFindCfg(int find_type, int role_level) const
{
	if(find_type < 0 || find_type >= RB_TYPE_MAX) return NULL;

	const GuildAnswerFindGroupCfg & node_cfg = m_find_cfg[find_type];
	for (int i = 0; i < (int)node_cfg.node_list.size(); i++)
	{
		if (node_cfg.node_list[i].min_level <= role_level && node_cfg.node_list[i].max_level >= role_level)
		{
			return &node_cfg.node_list[i];
		}
	}
	return NULL;
}

const GuildAnswerFindRewardGroupCfg * ActivityGuildAnswerConfig::GetFindRewardGroupCfg(int group_id) const
{
	std::map<int, std::vector<GuildAnswerFindRewardGroupCfg> >::const_iterator it = m_find_reward_group_map.find(group_id);
	if(it == m_find_reward_group_map.end()) return NULL;

	int rate = rand() & RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (int i = 0; i < (int)it->second.size();i++)
	{
		if (rate < it->second[i].rate)
		{
			return &it->second[i];
		}
		rate -= it->second[i].rate;
	}

	return NULL;
}

int ActivityGuildAnswerConfig::InitOtherInformaCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "question_time", m_guild_answer_other_cfg.question_time) || m_guild_answer_other_cfg.question_time <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "question_num", m_guild_answer_other_cfg.question_numb) || m_guild_answer_other_cfg.question_numb <= 0)
	{
		return -2;
	}
	
	if (!GetSubNodeValue(root_element, "scene_id", m_guild_answer_other_cfg.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(m_guild_answer_other_cfg.scene_id))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "waiting_time", m_guild_answer_other_cfg.ready_time) || m_guild_answer_other_cfg.ready_time < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "min_level", m_guild_answer_other_cfg.min_level) || m_guild_answer_other_cfg.min_level <= 0 || m_guild_answer_other_cfg.min_level > MAX_ROLE_LEVEL)
	{
		return -5;
	}

	return 0;
}

int ActivityGuildAnswerConfig::InitQuestionListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int question_id = 0, answer_id = 0;
		if (!GetSubNodeValue(root_element, "question_id", question_id) || m_question_answer_map.end() != m_question_answer_map.find(question_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "answer_id", answer_id))
		{
			return -2;
		}

		m_question_answer_map[question_id] = answer_id;
		root_element = root_element->NextSiblingElement();
	}

	// 如果题库的数量少于需要答题的数量，报错
	if ((int)m_question_answer_map.size() < m_guild_answer_other_cfg.question_numb) return -500;

	return 0;
}

int ActivityGuildAnswerConfig::InitRandAwardGroupList(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0; GuildAnswerItemRand tmp_con;
		if (!GetSubNodeValue(root_element, "group", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.prob) || tmp_con.prob <= 0 || tmp_con.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward_item");
		if (NULL == item_node) return -100;

		if (!tmp_con.reward_item.ReadConfig(item_node))
		{
			return -3;
		}

		std::map<int, std::vector<GuildAnswerItemRand> >::iterator group_it = m_item_rand_group.find(group_id);
		if (m_item_rand_group.end() != group_it)
		{
			group_it->second.push_back(tmp_con);
		}
		else
		{
			std::vector<GuildAnswerItemRand> tmp_list; tmp_list.push_back(tmp_con);
			m_item_rand_group[group_id] = tmp_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	// 检查每组的概率是否超过 10000
	for (std::map<int, std::vector<GuildAnswerItemRand> >::iterator check_it = m_item_rand_group.begin(); check_it != m_item_rand_group.end(); ++check_it)
	{
		int rand_total = 0;
		for (std::vector<GuildAnswerItemRand>::iterator loop_it = check_it->second.begin(); loop_it != check_it->second.end(); ++loop_it)
		{
			rand_total += loop_it->prob;
		}

		if (rand_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -(2000 + check_it->first);
	}

	return 0;
}

int ActivityGuildAnswerConfig::InitRightAnswerAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level = 0; GuildAnswerReward tmp_con;
		if (!GetSubNodeValue(root_element, "role_level", level) || level <= 0 || level > MAX_ROLE_LEVEL || m_right_reward_map.end() != m_right_reward_map.find(level))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.exp) || tmp_con.exp < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "gongxian", tmp_con.contribution) || tmp_con.contribution < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "family_coin", tmp_con.family_coin) || tmp_con.family_coin < 0)
		{
			return -5;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("item_group");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("node");
			if (NULL == item_node) return -100;

			while (NULL != item_node)
			{
				if (tmp_con.list_count >= GUILD_ANSWER_REWARD_ITEM_MAX) return -150;
				if (!GetNodeValue(item_node, tmp_con.reward_group_list[tmp_con.list_count]) || m_item_rand_group.end() == m_item_rand_group.find(tmp_con.reward_group_list[tmp_con.list_count]))
				{
					return -6;
				}

				++tmp_con.list_count;
				item_node = item_node->NextSiblingElement();
			}
		}

		m_right_reward_map[level] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildAnswerConfig::InitWrongAnswerAward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level = 0; GuildAnswerReward tmp_con;
		if (!GetSubNodeValue(root_element, "role_level", level) || level <= 0 || level > MAX_ROLE_LEVEL || m_wrong_reward_map.end() != m_wrong_reward_map.find(level))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.exp) || tmp_con.exp < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin) || tmp_con.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "gongxian", tmp_con.contribution) || tmp_con.contribution < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "family_coin", tmp_con.family_coin) || tmp_con.family_coin < 0)
		{
			return -5;
		}

		/*TiXmlElement * list_node = root_element->FirstChildElement("item_group");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("node");
			if (NULL == item_node) return -100;

			while (NULL != item_node)
			{
				if (tmp_con.list_count >= GUILD_ANSWER_REWARD_ITEM_MAX) return -150;
				if (!GetNodeValue(item_node, tmp_con.reward_group_list[tmp_con.list_count]) || m_item_rand_group.end() == m_item_rand_group.find(tmp_con.reward_group_list[tmp_con.list_count]))
				{
					return -6;
				}

				++tmp_con.list_count;
				item_node = item_node->NextSiblingElement();
			}
		}*/

		m_wrong_reward_map[level] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityGuildAnswerConfig::InitGuildAnswerFindRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id < 0)
		{
			return -1;
		}
		GuildAnswerFindRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0)
		{
			return -2;
		}
		TiXmlElement * reward_node = root_element->FirstChildElement("reward");
		if(NULL == reward_node) return -222;
		
		if (!node_cfg.reward.ReadConfig(reward_node))
		{
			return -3;
		}

		m_find_reward_group_map[group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityGuildAnswerConfig::InitGuildAnswerFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int find_type = 0;
		if (!GetSubNodeValue(root_element, "find_type", find_type) || find_type < 0 || find_type >= RB_TYPE_MAX)
		{
			return -1;
		}
		GuildAnswerFindGroupCfg & group_cfg = m_find_cfg[find_type];
		GuildAnswerFindCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "family_gongxian", node_cfg.family_gongxian) || node_cfg.family_gongxian < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "group_id", node_cfg.group_id) || (node_cfg.group_id != -1 && m_find_reward_group_map.end() == m_find_reward_group_map.find(node_cfg.group_id)))
		{
			return -6;
		}

		group_cfg.node_list.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
