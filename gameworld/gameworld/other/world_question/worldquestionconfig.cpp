#include "worldquestionconfig.hpp"
#include "engineadapter.h"

WorldQuestionConfig::WorldQuestionConfig()
{
}

WorldQuestionConfig::~WorldQuestionConfig()
{
	for (std::map<int, std::vector<WorldQuestionBank> >::iterator it = m_question_bank_cfg.begin(); it != m_question_bank_cfg.end(); it++)
	{
		std::vector<WorldQuestionBank>().swap(it->second);
	}

	for (std::map<int, std::vector<WorldQuestionRewardGroup> >::iterator it = m_question_reward_group_cfg.begin(); it != m_question_reward_group_cfg.end(); it++)
	{
		std::vector<WorldQuestionRewardGroup>().swap(it->second);
	}
}

bool WorldQuestionConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("question_bank", InitQuestionBankCfg);
	LOAD_CONFIG("reward", InitQuestionRewardCfg);
	LOAD_CONFIG("reward_group", InitQuestionRewardGroupCfg);
	LOAD_CONFIG("answer_group", InitAnswerGroupCfg);
	return true;
}

bool WorldQuestionConfig::RandQuestionCdg(int group_id, WorldQuestionBank * now_question)
{
	std::map<int, std::vector<WorldQuestionBank> >::const_iterator it = m_question_bank_cfg.find(group_id);
	if (it != m_question_bank_cfg.end())
	{
		//取出该题组中的题目
		std::vector<WorldQuestionBank> rand_vec(it->second);
		if (rand_vec.empty())
		{
			return false;
		}
		//计算下一天零点的时间,用这个时间来做随机数种子
		unsigned int seed = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		seed += EngineAdapter::Instance().NextDayInterval(0, 0, 0);
		seed %= 13131;

		// 随机一下
		RandomShuffle(&rand_vec[0], 0, rand_vec.size(), seed);
		if (now_question != NULL)
		{
			*now_question = rand_vec[0];
			return true;
		}
	}
	return false;
}

const WorldQuestionBank * WorldQuestionConfig::GetQuestion(int question_id)
{
	for (std::map<int, std::vector<WorldQuestionBank> >::const_iterator it = m_question_bank_cfg.begin();it != m_question_bank_cfg.end();it++)
	{
		for (std::vector<WorldQuestionBank>::const_iterator itor = it->second.begin();itor != it->second.end();itor++)
		{
			if (itor->question_id == question_id)
			{
				return &(*itor);
			}
		}
	}
	return NULL;
}

const WorldQuestionReward * WorldQuestionConfig::GetReward(int level)
{
	std::map<int, WorldQuestionReward>::const_iterator it = m_reward_cfg.find(level);
	if (it != m_reward_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}
static int temp_num;

bool FindType(const WorldQuestionRewardGroup &cfg)
{
	return temp_num < cfg.rate;
}

const WorldQuestionRewardGroup * WorldQuestionConfig::RandRewardGroup(int group_id)
{
	std::map<int, std::vector<WorldQuestionRewardGroup> >::const_iterator it = m_question_reward_group_cfg.find(group_id);
	std::map<int, int>::iterator weight_it2 = m_reward_group_weight.find(group_id);
	if (it != m_question_reward_group_cfg.end() && weight_it2 != m_reward_group_weight.end())
	{
		temp_num = RandomNum(weight_it2->second);
		std::vector<WorldQuestionRewardGroup>::const_iterator it3 = std::find_if(it->second.begin(), it->second.end(), FindType);
		if (it3 != it->second.end())
		{
			return &(*it3);
		}
	}
	return NULL;
}

const WorldAnswerGroup * WorldQuestionConfig::GetAnswerGroupCfg(int group_id)
{
	std::map<int, WorldAnswerGroup>::iterator it = m_answer_group_map.find(group_id);
	if (it != m_answer_group_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int WorldQuestionConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "start_time", m_other_cfg.start_time))return -1;
		if (!GetSubNodeValue(root_element, "end_time", m_other_cfg.end_time))return -2;
		if (!GetSubNodeValue(root_element, "question_time", m_other_cfg.question_time))return -3;
		if (!GetSubNodeValue(root_element, "answer_time", m_other_cfg.answer_time))return -4;
		if (!GetSubNodeValue(root_element, "refresh_time", m_other_cfg.refresh_time))return -5;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int WorldQuestionConfig::InitQuestionBankCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int question_groups = 0;

		if (!GetSubNodeValue(root_element, "question_groups", question_groups) || question_groups <= 0)return -1;

		WorldQuestionBank node_cfg;
		node_cfg.question_groups = question_groups;

		if (!GetSubNodeValue(root_element, "question_id", node_cfg.question_id) || node_cfg.question_id <= 0)return -2;
		std::string data_string;
		if (!GetSubNodeValue(root_element, "question", data_string))return -3;

		int question_hash = GetStringHash(data_string.length(), data_string.c_str());//问题哈希值
		node_cfg.question = question_hash;

		if (!GetSubNodeValue(root_element, "answer", node_cfg.answer))return -4;

		m_question_bank_cfg[question_groups].push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WorldQuestionConfig::InitQuestionRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int role_level = 0;

		if (!GetSubNodeValue(root_element, "role_level", role_level) || role_level <= 0)return -1;

		WorldQuestionReward &node_cfg = m_reward_cfg[role_level];
		node_cfg.role_level = role_level;

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)return -2;

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)return -3;

		//if (!GetSubNodeValue(root_element, "world_question_score", node_cfg.world_question_score) || node_cfg.world_question_score < 0)return -4;

		TiXmlElement *  item_element = root_element->FirstChildElement("item_group");
		if (NULL != item_element)
		{
			TiXmlElement *node_element = item_element->FirstChildElement("node");
			while (node_element && node_element->FirstChild())
			{
				int node = 0;
				if (!GetNodeValue(node_element, node))
				{
					return -18;
				}

				node_cfg.item_list.push_back(node);
				node_element = node_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0; 
}

int WorldQuestionConfig::InitQuestionRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int group = 0;

		if (!GetSubNodeValue(root_element, "group", group) || group <= 0)return -1;

		WorldQuestionRewardGroup node_cfg;
		node_cfg.group = group;

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate < 0)return -2;

		m_reward_group_weight[group] += rate;
		node_cfg.rate = m_reward_group_weight[group];
		TiXmlElement *node_element = root_element->FirstChildElement("reward_item");
		if (node_element == NULL)return -3;
		
		node_cfg.reward_item.ReadConfig(node_element);

		m_question_reward_group_cfg[group].push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}
	return 0; 
}

int WorldQuestionConfig::InitAnswerGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int group_id = 0;

		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0)return -1;
		int answer_true = 0;
		if (!GetSubNodeValue(root_element, "answer_true", answer_true) || answer_true <= 0)return -2;

		std::string data_string;
		WorldAnswerGroup & node = m_answer_group_map[group_id];
		node.answer_true = answer_true;
		for (int i = 0; i < 4; i++)
		{
			std::string reader_mate = STRING_SPRINTF("answer%d", i + 1);
			data_string.clear();

			if (!GetSubNodeValue(root_element, reader_mate, data_string))
			{
				return -(10 * i + 3);
			}
			int answer_hash = GetStringHash(data_string.length(), data_string.c_str());//答案哈希值
			node.answer_id[i] = answer_hash;
		}
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
