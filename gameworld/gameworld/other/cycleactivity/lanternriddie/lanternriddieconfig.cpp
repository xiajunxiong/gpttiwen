#include "lanternriddieconfig.hpp"
#include "servercommon/servercommon.h"
#include <set>
#include "npc/npcpool.h"

CycleActLanternRiddleConfig::CycleActLanternRiddleConfig():m_total_rate(0)
{
}

CycleActLanternRiddleConfig::~CycleActLanternRiddleConfig()
{
	for (std::map<int, std::vector<ItemConfigData> >::iterator it = m_last_reward_cfg.begin(); it != m_last_reward_cfg.end(); it++)
	{
		std::vector<ItemConfigData>().swap(it->second);
	}
	std::vector<int>().swap(m_quetion_vec_cfg);
	std::vector<int>().swap(m_npc_cfg);
	std::vector<CALRCardCfg>().swap(m_card_cfg);
	std::vector<int>().swap(m_word_cfg);
}

bool CycleActLanternRiddleConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("question", InitQuertionCfg);
	LOAD_CONFIG_2("answer", InitAnswerCfg);
	LOAD_CONFIG_2("result", InitResultRewardCfg);
	LOAD_CONFIG_2("reward_show", InitLastRewardCfg);
	LOAD_CONFIG_2("lantern", InitNPCCfg);
	LOAD_CONFIG_2("sp_card", InitCardCfg);
	LOAD_CONFIG_2("word", InitWordCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);
	
	return true;
}

int CycleActLanternRiddleConfig::RandQuestionCfg(std::set<int>& used_questions) const
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

	return t_qusetion_vec[0];	//如果全部被用过了,或者一个都没有用过,就直接随机一个
}

const CALRQuestionResultCfg * CycleActLanternRiddleConfig::GetAnswerResultCfg(int question, int answer, int & result) const
{
	std::map<int, int>::const_iterator it_question = m_question_cfg.find(question);
	if (m_question_cfg.end() == it_question)  return NULL;

	std::map<int, int>::const_iterator it_answer = m_answer_cfg.find(it_question->second);
	if (m_answer_cfg.end() == it_answer)  return NULL;

	result = it_answer->second == answer ? CALRQuestionResultCfg::CORRECT : CALRQuestionResultCfg::WRONG;

	std::map<int, CALRQuestionResultCfg>::const_iterator it_reward = m_result_reward_cfg.find(result);
	if (m_result_reward_cfg.end() == it_reward) return NULL;

	return &it_reward->second;
}

const CALRQuestionResultCfg * CycleActLanternRiddleConfig::GetLastSatisfactionCfg() const
{
	std::map<int, CALRQuestionResultCfg>::const_iterator it_reward = m_result_reward_cfg.find(CALRQuestionResultCfg::CORRECT);
	if (m_result_reward_cfg.end() != it_reward) return &it_reward->second;

	return NULL;
}

const std::vector<ItemConfigData>* CycleActLanternRiddleConfig::GetLastRewardCfg(int satisfaction) const
{
	std::map<int, std::vector<ItemConfigData> >::const_iterator iter = m_last_reward_cfg.find(satisfaction);
	if (m_last_reward_cfg.end() != iter)
	{
		return &iter->second;
	}

	return NULL;
}

int CycleActLanternRiddleConfig::GetRandNPCCfg(int last_npc_seq) const
{
	int npc = RandomNum(m_npc_cfg.size());

	while (last_npc_seq == npc && m_npc_cfg.size() > 1)
	{
		npc = RandomNum(m_npc_cfg.size());
	}
	return npc;
}

int CycleActLanternRiddleConfig::GetTrueAnswer(int question)const
{
	std::map<int, int>::const_iterator it_question = m_question_cfg.find(question);
	if (m_question_cfg.end() != it_question)
	{
		std::map<int, int>::const_iterator it_answer = m_answer_cfg.find(it_question->second);
		if (m_answer_cfg.end() != it_answer) return it_answer->second;
	}

	return -1;
}

int CycleActLanternRiddleConfig::RandCardCfg()const
{
	int rand_rate = RandomNum(m_total_rate);
	
	for (int i = 0; i < (int)m_card_cfg.size(); ++i)
	{
		if (m_card_cfg[i].rate >= rand_rate)
		{
			return m_card_cfg[i].card_type;
		}
	}

	return -1;
}

int CycleActLanternRiddleConfig::RandWordCfg() const
{
	return RandomNum(m_word_cfg.size()) + 1;
}

int CycleActLanternRiddleConfig::InitQuertionCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		int question_id = 0, answer_group = 0;

		if (!GetSubNodeValue(data_element, "question_id", question_id) || question_id < 0)
		{
			errormsg = STRING_SPRINTF("read failed or question_id < 0");
			return -1;
		}
		if (!GetSubNodeValue(data_element, "answer", answer_group) || answer_group < 0)
		{
			errormsg = STRING_SPRINTF("read failed or answer < 0");
			return -2;
		}
		m_question_cfg[question_id] = answer_group;
		m_quetion_vec_cfg.push_back(question_id);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleActLanternRiddleConfig::InitAnswerCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		int group_id = 0, answer_true = 0;

		if (!GetSubNodeValue(data_element, "group_id", group_id) || group_id < 0) return -1;
		if (!GetSubNodeValue(data_element, "answer_true", answer_true) || answer_true < 0) return -2;

		m_answer_cfg[group_id] = answer_true;

		data_element = data_element->NextSiblingElement();
	}


	return 0;
}

int CycleActLanternRiddleConfig::InitResultRewardCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		CALRQuestionResultCfg qr_cfg;
		int result = 0;

		if (!GetSubNodeValue(data_element, "result", result) || (result != CALRQuestionResultCfg::WRONG && result != CALRQuestionResultCfg::CORRECT))
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

int CycleActLanternRiddleConfig::InitLastRewardCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
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

int CycleActLanternRiddleConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "answer_time", m_other_cfg.answer_time) || m_other_cfg.answer_time <= 0) return -1;
	if (!GetSubNodeValue(data_element, "carrot_times", m_other_cfg.question_num) || m_other_cfg.question_num <= 0) return -2;
	if (!GetSubNodeValue(data_element, "satisfaction", m_other_cfg.satisfaction) || m_other_cfg.satisfaction < 0 ) return -3;
	if (!GetSubNodeValue(data_element, "member", m_other_cfg.member) || m_other_cfg.member <= 0 || m_other_cfg.member > MAX_TEAM_MEMBER_NUM)
	{
		errormsg = STRING_SPRINTF("[member] <= 0 or [member] > %d", MAX_TEAM_MEMBER_NUM);
		return -4;
	}
	if (!GetSubNodeValue(data_element, "monster_guoup", m_other_cfg.monster_group) || m_other_cfg.monster_group < 0) return -5;
	if (!GetSubNodeValue(data_element, "fail_satisfaction", m_other_cfg.fail_satisfaction) || m_other_cfg.fail_satisfaction < 0) return -6;

	return 0;
}

int CycleActLanternRiddleConfig::InitNPCCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int lantern = 0;
		if (!GetSubNodeValue(data_element, "lantern", lantern) || lantern < 0) return -1;
		m_npc_cfg.push_back(lantern);

		data_element = data_element->NextSiblingElement();
	}

	if (m_npc_cfg.empty()) return -2;

	return 0;
}

int CycleActLanternRiddleConfig::InitCardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		CALRCardCfg card_cfg;
		int rate = 0;

		if (!GetSubNodeValue(data_element, "type", card_cfg.card_type) || card_cfg.card_type <= 0 || card_cfg.card_type >= CALRCardCfg::CARD_TYPE_MAX) return -1;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -2;
		m_total_rate += rate;
		card_cfg.rate = m_total_rate;

		m_card_cfg.push_back(card_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int CycleActLanternRiddleConfig::InitWordCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int word_id = 0;

		if (!GetSubNodeValue(data_element, "word_id", word_id) || word_id <= 0) return-1;

		m_word_cfg.push_back(word_id);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
