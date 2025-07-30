#include "randactivityshanhaiquestionconfig.hpp"
#include "servercommon/struct/itemlistparam.h"

RandActivityShanHaiQuestionConfig::RandActivityShanHaiQuestionConfig():m_total_rate(0)
{

}

RandActivityShanHaiQuestionConfig::~RandActivityShanHaiQuestionConfig()
{
	std::vector<BasisCfg>().swap(m_basis_cfg);
	std::vector<RewardGroupCfg>().swap(m_reward_group_cfg);
}

bool RandActivityShanHaiQuestionConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configuration", InitBasisCfg);
	LOAD_RA_CONFIG("reward_group", InitRewardCfg);
	LOAD_RA_CONFIG("question_bank", InitQuestionCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityShanHaiQuestionConfig::InitBasisCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	while (NULL != data_element)
	{
		bool has_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}
		int suc_reward_group = 0, fault_reward_group = 0;
		if (!GetSubNodeValue(data_element, "suc_reward_group", suc_reward_group) || suc_reward_group <= 0) return -1;
		if (!GetSubNodeValue(data_element, "fault_reward_group", fault_reward_group) || fault_reward_group <= 0) return -2;

		if (m_basis_cfg.size() > 0)
		{
			int pre_index = m_basis_cfg.size() - 1;
			if (section_start != m_basis_cfg[pre_index].section_start ||
				section_end != m_basis_cfg[pre_index].section_end)
			{
				if (section_start < m_basis_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_basis_cfg.push_back(BasisCfg());
			}
		}
		if (m_basis_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_basis_cfg.push_back(BasisCfg());
		}

		BasisCfg &node_cfg = m_basis_cfg[m_basis_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;
		node_cfg.suc_reward_group = suc_reward_group;
		node_cfg.fault_reward_group = fault_reward_group;

		int question_cfg = 0;
		if (!GetSubNodeValue(data_element, "question_id", question_cfg)) return -3;

		node_cfg.question_id.push_back(question_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiQuestionConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	while (NULL != data_element)
	{
		RewardGroupCfg t_reward_gropup;

		if (!GetSubNodeValue(data_element, "type", t_reward_gropup.seq) || t_reward_gropup.seq <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "reward_group", t_reward_gropup.reward_group) || t_reward_gropup.reward_group <= 0)
		{
			return -2;
		}

		if (t_reward_gropup.reward_group != m_basis_cfg[0].fault_reward_group)
		{
			TiXmlElement * reward_item_element = data_element->FirstChildElement("reward_item_list");
			if (NULL == reward_item_element)
			{
				return __LINE__;
			}

			TiXmlElement * reward_node = reward_item_element->FirstChildElement("reward_item");
			if (NULL == reward_node)
			{
				return __LINE__;
			}

			while (NULL != reward_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(reward_node))
				{
					return -100;
				}

				t_reward_gropup.reward_item.push_back(item);
				reward_node = reward_node->NextSiblingElement();
			}
		}

		if (!GetSubNodeValue(data_element, "exp", t_reward_gropup.exp) || t_reward_gropup.exp < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "coin", t_reward_gropup.coin) || t_reward_gropup.coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "rate", t_reward_gropup.rate) || t_reward_gropup.rate < 0)
		{
			return -5;
		}

		m_reward_group_cfg.push_back(t_reward_gropup);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityShanHaiQuestionConfig::InitQuestionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	while (NULL != data_element)
	{
		QuestionBankCfg t_question_bank;

		if (!GetSubNodeValue(data_element, "question_id", t_question_bank.question_id) || t_question_bank.question_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element,"answer_id",t_question_bank.anwser_id)||t_question_bank.anwser_id<=0)
		{
			return -2; 
		}

		if (!GetSubNodeValue(data_element, "rate", t_question_bank.rate) || t_question_bank.rate < 0)
		{
			return -3;
		}
		m_total_rate += t_question_bank.rate;
		t_question_bank.rate_section = m_total_rate-1;

		m_question_bank_cfg[t_question_bank.question_id] = t_question_bank;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}



int RandActivityShanHaiQuestionConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -__LINE__;
	}

	while (NULL != data_element)
	{
		if (!GetSubNodeValue(data_element, "start_times", m_other_cfg.start_times) || m_other_cfg.start_times < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "end_times", m_other_cfg.end_times) || m_other_cfg.end_times < 0 || m_other_cfg.start_times >= m_other_cfg.end_times)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "ans_times", m_other_cfg.ans_times) || m_other_cfg.ans_times < 0)
		{
			return -3;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}


bool RandActivityShanHaiQuestionConfig::IsSucAnswer(int question_id, int answer_id)const
{
	std::map<int, QuestionBankCfg>::const_iterator it = m_question_bank_cfg.find(question_id);
	if (m_question_bank_cfg.end() == it) return false;

	if (answer_id == it->second.anwser_id)
	{
		return true;
	}
	return false;
}

const RewardGroupCfg * RandActivityShanHaiQuestionConfig::GetRewardGroupCfg(int reward_group)const
{
	if (reward_group <= 0) return NULL;

	std::vector<int> rand_group;
	for (int i = 0; i < m_reward_group_cfg.size(); ++i)
	{
		if (m_reward_group_cfg[i].reward_group == reward_group)
		{
			rand_group.push_back(m_reward_group_cfg[i].seq);
		}
	}

	if (rand_group.size() == 0) return NULL;
	int rand_reward = RandomNum(rand_group.size());

	for (int i = 0; i < m_reward_group_cfg.size(); ++i)
	{
		if (rand_group[rand_reward] == m_reward_group_cfg[i].seq)
		{
			return &m_reward_group_cfg[i];
		}
	}

	return NULL;
}

const RewardGroupCfg * RandActivityShanHaiQuestionConfig::GetRewardGroupBySeqCfg(int reward_seq)const
{
	if (reward_seq <= 0)return NULL;

	for (int i = 0; i < m_reward_group_cfg.size(); ++i)
	{
		if (reward_seq == m_reward_group_cfg[i].seq)
		{
			return &m_reward_group_cfg[i];
		}
	}

	return NULL;
}

const QuestionBankCfg * RandActivityShanHaiQuestionConfig::GetQuestionBankCfg(int question_id)const
{
	std::map<int, QuestionBankCfg>::const_iterator it = m_question_bank_cfg.find(question_id);
	if (m_question_bank_cfg.end() == it) return NULL;

	return &it->second;
}

const QuestionBankCfg * RandActivityShanHaiQuestionConfig::GetRandQuestionCfg(int rand_rate)const
{
	if (rand_rate < 0 || rand_rate >= m_total_rate) return NULL;

	for (std::map<int, QuestionBankCfg>::const_iterator it = m_question_bank_cfg.begin(); it != m_question_bank_cfg.end(); ++it)
	{
		if (rand_rate < it->second.rate_section)
		{
			return &it->second;
		}
	}

	return NULL;
}

int RandActivityShanHaiQuestionConfig::GetBasisSucCfg(int rand_open_day)const
{
	if (rand_open_day <= 0) return -1;

	for (int i = 0; i < m_basis_cfg.size(); ++i)
	{
		if (rand_open_day >= m_basis_cfg[i].section_start && rand_open_day <= m_basis_cfg[i].section_end)
		{
			return m_basis_cfg[i].suc_reward_group;
		}
	}

	return -2;
}

int RandActivityShanHaiQuestionConfig::GetBasisFaultCfg(int rand_open_day)const
{
	if (rand_open_day <= 0) return -1;

	for (int i = 0; i < m_basis_cfg.size(); ++i)
	{
		if (rand_open_day >= m_basis_cfg[i].section_start && rand_open_day <= m_basis_cfg[i].section_end)
		{
			return m_basis_cfg[i].fault_reward_group;
		}
	}

	return -2;
}