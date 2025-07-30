#include "wstquestionconfig.hpp"

WstQuestionConfig::WstQuestionConfig()
{
}

WstQuestionConfig::~WstQuestionConfig()
{
	for (std::map<int, std::vector<WSTQuestionBankCfg> >::iterator it = m_question_bank_group_cfg.begin(); it != m_question_bank_group_cfg.end(); it++)
	{
		std::vector<WSTQuestionBankCfg>().swap(it->second);
	}

	std::vector<WstQuestionFindCfg>().swap(m_wqfcfg_v);
}
bool WstQuestionConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("question_bank", InitQuestionBankCfg);
	LOAD_CONFIG("reward_find", InitFindRewardCfg);
	return true;
}

const WSTQuestionRewardCfg * WstQuestionConfig::GetRewardCfg(int level)
{
	std::map<int, WSTQuestionRewardCfg>::const_iterator it = m_reward_cfg.find(level);
	if (it == m_reward_cfg.end()) return NULL;
	return &it->second;

	/*
	for (std::map<int, WSTQuestionRewardCfg>::const_iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); ++it)
	{
		if (level <= it->first)
		{
			return &it->second;
		}
	}
	return NULL;
	*/
}

const WSTQuestionBankCfg * WstQuestionConfig::GetQuestionBankCfg(int id)
{
	std::map<int, WSTQuestionBankCfg>::const_iterator it = m_question_bank_cfg.find(id);
	if (it == m_question_bank_cfg.end()) return NULL;
	return &it->second;
}

const WSTQuestionBankCfg * WstQuestionConfig::GetQuestionBankByGroupCfg(int group_id)
{
	std::map<int, std::vector<WSTQuestionBankCfg> >::const_iterator it = m_question_bank_group_cfg.find(group_id);
	if (it == m_question_bank_group_cfg.end()) return NULL;
	int index = rand() % (it->second.size());
	return &it->second[index];
}

bool WstQuestionConfig::GetFindReward(int level, int type, int * coin, int * exp)
{
	for (std::map<int, std::map<int, WstQuestionFindCfg> >::const_iterator level_it = m_find_reward_map.begin(), level_end = m_find_reward_map.end();
		 level_end != level_it; ++level_it)
	{
		int min_level = level_it->first / 1000;
		int max_level = level_it->first % 1000;

		if (level >= min_level && level <= max_level)
		{
			std::map<int, WstQuestionFindCfg>::const_iterator it = level_it->second.find(type);
			if (it == level_it->second.end())
			{
				return false;
			}
			else
			{
				if (NULL != coin) *coin = it->second.coin;
				if (NULL != exp) *exp = it->second.exp;

				return true;
			}
		}
	}
	return false;
}

const int WstQuestionConfig::GetRewardGroup(int _role_level)
{
	for (std::vector<WstQuestionFindCfg>::iterator it = m_wqfcfg_v.begin(); it != m_wqfcfg_v.end(); ++it)
	{
		if (_role_level < it->min_level || _role_level > it->max_level) continue;
		return it->reward_group;
	}
	return -1000;
}

const int WstQuestionConfig::GetFindType(int _role_level)
{
	for (std::vector<WstQuestionFindCfg>::iterator it = m_wqfcfg_v.begin();it != m_wqfcfg_v.end();++it)
	{
		if (_role_level < it->min_level || _role_level > it->max_level) continue;
		return it->find_type;
	}

	return -1;
}

int WstQuestionConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "coin_reduction_ratio", m_other_cfg.coin_reduction_ratio))return -1;
		if (!GetSubNodeValue(root_element, "exp_reduction_ratio", m_other_cfg.exp_reduction_ratio))return -2;
		if (!GetSubNodeValue(root_element, "question_num", m_other_cfg.question_num))return -3;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int WstQuestionConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	int last_level = 0;
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "role_level", level) || level != last_level + 1 || level > MAX_ROLE_LEVEL)return -1;
		last_level = level;
		WSTQuestionRewardCfg& node = m_reward_cfg[level];
		node.level = level;

		if (!GetSubNodeValue(root_element, "exp", node.exp) || node.exp < 0) return -2;
		if (!GetSubNodeValue(root_element, "coin", node.coin) || node.coin < 0) return -3;
		if (!GetSubNodeValue(root_element, "exp_buy", node.exp_buy) || node.exp_buy < 0) return -4;
		if (!GetSubNodeValue(root_element, "gold", node.gold) || node.gold < 0) return -5;
		if (!GetSubNodeValue(root_element, "num", node.num) || node.num < 0) return -6;

		root_element = root_element->NextSiblingElement();
	}

	if (last_level != MAX_ROLE_LEVEL) return -1000;

	return 0;
}

int WstQuestionConfig::InitQuestionBankCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	int last_question_id = 0;
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0)return -1;
		
		int question_id = 0;
		if (!GetSubNodeValue(root_element, "question_id", question_id) || question_id != last_question_id + 1 || question_id >= MAX_UINT16)return -3;
		WSTQuestionBankCfg& node = m_question_bank_cfg[question_id];
		node.group_id = group_id;
		node.question_id = question_id;
		last_question_id = question_id;
		if (!GetSubNodeValue(root_element, "answer_id", node.answer_id) || node.answer_id < 1 || node.answer_id > 4) return -3;

		m_question_bank_group_cfg[group_id].push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WstQuestionConfig::InitFindRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		WstQuestionFindCfg node_cfg;
		if (!GetSubNodeValue(root_element, "find_type", node_cfg.find_type) || node_cfg.find_type < 0 || node_cfg.find_type > 1)return -1;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level)) return -2;
		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level)) return -3;

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp)) return -4;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin)) return -5;
		if (!GetSubNodeValue(root_element, "reward_group", node_cfg.reward_group) || (node_cfg.reward_group < 0 && node_cfg.reward_group != -1)) return -5;
		
		m_find_reward_map[node_cfg.min_level * 1000 + node_cfg.max_level][node_cfg.find_type] = node_cfg;
		m_wqfcfg_v.push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
