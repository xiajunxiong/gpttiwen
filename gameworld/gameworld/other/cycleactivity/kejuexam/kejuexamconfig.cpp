#include "kejuexamconfig.hpp"

CycleKeJuExamConfig::CycleKeJuExamConfig():m_total_rate(0)
{
}

CycleKeJuExamConfig::~CycleKeJuExamConfig()
{
	std::vector<int>().swap(m_quetion_vec_cfg);
	for (std::map<int, std::vector<ItemConfigData> >::iterator it = m_last_reward_cfg.begin(); it != m_last_reward_cfg.end(); it++)
	{
		std::vector<ItemConfigData>().swap(it->second);
	}
	std::vector<int>().swap(m_npc_cfg);
	std::vector<KeJuExamCardCfg>().swap(m_card_cfg);
	for (std::map<int, std::vector<std::pair<int, int> > >::iterator it = m_word_cfg.begin(); it != m_word_cfg.end(); it++)
	{
		std::vector<std::pair<int, int> >().swap(it->second);
	}

	std::vector<int>().swap(m_order_group_list_cfg);
	for (std::map<int, std::vector<std::pair<int, int> > >::iterator it = m_order_cfg.begin(); it != m_order_cfg.end(); it++)
	{
		std::vector<std::pair<int, int> >().swap(it->second);
	}
}

bool CycleKeJuExamConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("question", InitQuertionCfg);
	LOAD_CONFIG_2("result", InitResultRewardCfg);
	LOAD_CONFIG_2("reward_show", InitLastRewardCfg);
	LOAD_CONFIG_2("npc", InitNPCCfg);
	LOAD_CONFIG_2("sp_card", InitCardCfg);
	LOAD_CONFIG_2("word", InitWordCfg);
	LOAD_CONFIG_2("order", InitOrderCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

int CycleKeJuExamConfig::RandQuestionCfg(std::set<int>& used_questions) const
{
	std::vector<int> t_qusetion_vec = m_quetion_vec_cfg;
	RandomShuffle(&t_qusetion_vec[0], t_qusetion_vec.size());

	for (int i = 0; i < (int)t_qusetion_vec.size(); ++i)
	{
		int question_id = t_qusetion_vec[i];
		std::set<int>::const_iterator iter = used_questions.find(question_id);
		if (used_questions.end() == iter)
		{
			return question_id;
		}
	}

	return t_qusetion_vec[0];	//如果全部被用过了,或者一个都没有用过,就直接选第一个
}

const KeJuExamQuestionResultCfg * CycleKeJuExamConfig::GetAnswerResultCfg(int question, int answer, int & result) const
{
	std::map<int, int>::const_iterator it = m_question_cfg.find(question);
	if (m_question_cfg.end() == it)  return NULL;

	result = it->second == answer ? KeJuExamQuestionResultCfg::CORRECT : KeJuExamQuestionResultCfg::WRONG;

	std::map<int, KeJuExamQuestionResultCfg>::const_iterator it_reward = m_result_reward_cfg.find(result);
	if (m_result_reward_cfg.end() == it_reward) return NULL;

	return &it_reward->second;
}

const std::vector<ItemConfigData>* CycleKeJuExamConfig::GetLastRewardCfg(int satisfaction) const
{
	std::map<int, std::vector<ItemConfigData> >::const_iterator iter = m_last_reward_cfg.find(satisfaction);
	if (m_last_reward_cfg.end() != iter)
	{
		return &iter->second;
	}

	return NULL;
}

int CycleKeJuExamConfig::GetRandNPCCfg(int last_npc_seq) const
{
	std::vector<int> temp_npc_list = m_npc_cfg;

	RandomShuffle(&temp_npc_list[0], (int)temp_npc_list.size());

	return temp_npc_list[0] == last_npc_seq ? temp_npc_list[1] : temp_npc_list[0];
}

int CycleKeJuExamConfig::GetTrueAnswer(int question) const
{
	std::map<int, int>::const_iterator it = m_question_cfg.find(question);
	if (it != m_question_cfg.end())
	{
		return it->second;
	}

	return -1;
}

int CycleKeJuExamConfig::RandCardCfg() const
{
	int rand_rate = RandomNum(m_total_rate);

	for (int i = 0; i < (int)m_card_cfg.size(); ++i)
	{
		if (m_card_cfg[i].rate >= rand_rate)
		{
			return m_card_cfg[i].card_type;
		}
	}
	
	return m_card_cfg[m_card_cfg.size() - 1].card_type;		// 找不到就是配置有问题, 但是兼容一下吧
}

int CycleKeJuExamConfig::RandWordCfg(int word_type, int word_id) const
{
	std::map<int, std::vector<std::pair<int, int> > >::const_iterator it = m_word_cfg.find(word_type);
	if (it == m_word_cfg.end()) return -1;

	switch (word_type)
	{
	case WORD_TYPE_ANSWER:
	case WORD_TYPE_SUGGEST:
		{
			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (it->second[i].first == word_id)
				{
					return it->second[i].second;
				}
			}		
		}
		break;
	case WORD_TYPE_PRAISE:
	case WORD_TYPE_SEEK_HELP:
		{
			int seq = RandomNum((int)it->second.size());
			return it->second[seq].second;
		}
		break;
	default:
		return -2;
	}

	return -3;
}

int CycleKeJuExamConfig::GetOrderCfg(int & group, int & order) const
{
	///////////////////第一次获取/////////////////////
	if (group == 0 || order == 0)
	{
		int temp_group = m_order_group_list_cfg[RandomNum((int)m_order_group_list_cfg.size())];

		std::map<int, std::vector<std::pair<int, int> > >::const_iterator it = m_order_cfg.find(temp_group);
		if (it == m_order_cfg.end()) return -1;

		group = temp_group;
		order = it->second[0].first;
		return it->second[0].second;
	}

/////////////////////////正常流程////////////////////////
	{
		std::map<int, std::vector<std::pair<int, int> > >::const_iterator it = m_order_cfg.find(group);
		if (it == m_order_cfg.end()) return -2;

		int temp_order = order + 1;
		for (size_t i = 0; i < it->second.size(); i++)
		{
			if (it->second[i].first == temp_order)
			{
				order = temp_order;
				return it->second[i].second;
			}
		}
	}

////////////////////////当前组用完,重新随机组///////////////////
	{
		int temp_group = m_order_group_list_cfg[RandomNum((int)m_order_group_list_cfg.size())];

		std::map<int, std::vector<std::pair<int, int> > >::const_iterator it = m_order_cfg.find(temp_group);
		if (it == m_order_cfg.end()) return -1;

		group = temp_group;
		order = it->second[0].first;
		return it->second[0].second;
	}
}


int CycleKeJuExamConfig::InitQuertionCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int question_id = -1, answer_true = 0;

		if (!GetSubNodeValue(data_element, "question_id", question_id) || question_id < 0)
		{
			errormsg = STRING_SPRINTF("read failed or question_id < 0");
			return -1;
		}
		if (!GetSubNodeValue(data_element, "answer_true", answer_true) || answer_true <= 0)
		{
			errormsg = STRING_SPRINTF("read failed or answer <= 0");
			return -2;
		}

		m_question_cfg[question_id] = answer_true;
		m_quetion_vec_cfg.push_back(question_id);

		data_element = data_element->NextSiblingElement();
	}

	if (m_quetion_vec_cfg.empty()) return -3;

	return 0;
}

int CycleKeJuExamConfig::InitResultRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		KeJuExamQuestionResultCfg qr_cfg;
		int result = 0;

		if (!GetSubNodeValue(data_element, "result", result) || (result != KeJuExamQuestionResultCfg::WRONG && result != KeJuExamQuestionResultCfg::CORRECT))
		{
			errormsg = STRING_SPRINTF("read failed or [result] != 0 or [result] != 1");
			return -1;
		}

		if (!GetSubNodeValue(data_element, "satisfaction_up", qr_cfg.satisfaction_up) || qr_cfg.satisfaction_up < 0) return -2;

		TiXmlElement * item_list_element = data_element->FirstChildElement("answer_reward_list");
		if (NULL == item_list_element) return -__LINE__;

		TiXmlElement * item_element = item_list_element->FirstChildElement("answer_reward");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element)) return -3;

			qr_cfg.reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_result_reward_cfg[result] = qr_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleKeJuExamConfig::InitLastRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int satisfaction = 0;
		std::vector<ItemConfigData> reward_list;

		if (!GetSubNodeValue(data_element, "satisfaction", satisfaction) || satisfaction < 0) return -1;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("final_reward_list");
		if (NULL == reward_list_element) return -545;

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("final_reward");
		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			reward_list.push_back(item);
			reward_element = reward_element->NextSiblingElement();
		}
		if (reward_list.empty()) return -3;

		m_last_reward_cfg[satisfaction] = reward_list;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleKeJuExamConfig::InitNPCCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int npc = -1;
		if (!GetSubNodeValue(data_element, "npc", npc) || npc < 0) return -1;
		m_npc_cfg.push_back(npc);

		data_element = data_element->NextSiblingElement();
	}

	if (m_npc_cfg.size() <= 1) return -2;

	return 0;
}

int CycleKeJuExamConfig::InitCardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		KeJuExamCardCfg card_cfg;
		int rate = 0;

		if (!GetSubNodeValue(data_element, "type", card_cfg.card_type) || card_cfg.card_type <= 0 || card_cfg.card_type >= KeJuExamCardCfg::CARD_TYPE_MAX) return -1;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -2;
		m_total_rate += rate;
		card_cfg.rate = m_total_rate;

		m_card_cfg.push_back(card_cfg);

		data_element = data_element->NextSiblingElement();
	}

	if (m_card_cfg.empty()) return -3;

	return 0;
}

int CycleKeJuExamConfig::InitWordCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int word_type = -1;
		if (!GetSubNodeValue(data_element, "word_type", word_type) || word_type < 0 || word_type >= WORD_TYPE_MAX) return -1;

		std::pair<int, int> id_index;
		if (!GetSubNodeValue(data_element, "word_id", id_index.first) || id_index.first < 0) return -2;
		if (!GetSubNodeValue(data_element, "index", id_index.second) || id_index.second < 0) return -3;

		std::map<int, std::vector<std::pair<int, int> > >::iterator it = m_word_cfg.find(word_type);
		if (it == m_word_cfg.end())
		{
			std::vector < std::pair<int, int> > temp_vec;
			temp_vec.push_back(id_index);
			m_word_cfg[word_type] = temp_vec;
		}
		else
		{
			it->second.push_back(id_index);
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleKeJuExamConfig::InitOrderCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int group = 0;
		if (!GetSubNodeValue(data_element, "group", group) || group < 0) return -1;

		std::pair<int, int> order_posi;
		if (!GetSubNodeValue(data_element, "order", order_posi.first) || order_posi.first <= 0) return -2;
		if (!GetSubNodeValue(data_element, "position", order_posi.second) || order_posi.second < 0) return -3;

		std::map<int, std::vector<std::pair<int, int> > >::iterator it = m_order_cfg.find(group);
		if (it == m_order_cfg.end())
		{
			std::vector < std::pair<int, int> > temp_vec;
			temp_vec.push_back(order_posi);
			m_order_cfg[group] = temp_vec;
		}
		else
		{
			it->second.push_back(order_posi);
		}

		{
			bool is_same = false;
			for (size_t i = 0; i < m_order_group_list_cfg.size(); i++)
			{
				if (m_order_group_list_cfg[i] == group) is_same = true;
			}

			if (!is_same) m_order_group_list_cfg.push_back(group);
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleKeJuExamConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "answer_time", m_other_cfg.answer_time) || m_other_cfg.answer_time <= 0) return -1;
	if (!GetSubNodeValue(data_element, "carrot_times", m_other_cfg.question_num) || m_other_cfg.question_num <= 0) return -2;
	if (!GetSubNodeValue(data_element, "satisfaction", m_other_cfg.max_satisfaction) || m_other_cfg.max_satisfaction < 0) return -3;
	if (!GetSubNodeValue(data_element, "member", m_other_cfg.min_member) || m_other_cfg.min_member <= 0 || m_other_cfg.min_member > MAX_TEAM_MEMBER_NUM)
	{
		errormsg = STRING_SPRINTF("[member] <= 0 or [member] > %d", MAX_TEAM_MEMBER_NUM);
		return -4;
	}
	if (!GetSubNodeValue(data_element, "monster_guoup", m_other_cfg.monster_group) || m_other_cfg.monster_group < 0) return -5;
	if (!GetSubNodeValue(data_element, "fail_satisfaction", m_other_cfg.fail_satisfaction) || m_other_cfg.fail_satisfaction < 0) return -6;

	return 0;
}
