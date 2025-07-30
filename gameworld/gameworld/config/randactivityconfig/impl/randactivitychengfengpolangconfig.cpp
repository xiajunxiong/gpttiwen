#include "randactivitychengfengpolangconfig.hpp"

#include <algorithm>
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

RandActivityChengFengPoLangConfig::RandActivityChengFengPoLangConfig()
{
}

RandActivityChengFengPoLangConfig::~RandActivityChengFengPoLangConfig()
{
	std::vector<int>().swap(m_question_cfg);
	std::vector<CFPLRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityChengFengPoLangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("task", InitBaseCfg);
	LOAD_RA_CONFIG("reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityChengFengPoLangConfig::RandQuestionCfg(std::vector<int>& used_question) const
{
	std::vector<int> temp_vec = m_question_cfg;

	RandomShuffle(&temp_vec[0], (int)temp_vec.size());

	if(!used_question.empty())
	{
		for (size_t i = 0; i < temp_vec.size(); i++)
		{
			if (used_question.end() == std::find(used_question.begin(), used_question.end(), temp_vec[i]))
			{
				return temp_vec[i];
			}
		}
	}

	return temp_vec[0];		// 全都被用了,开摆
}

const std::vector<ItemConfigData>* RandActivityChengFengPoLangConfig::GetResultCfg(RandActivityManager* ramgr, int question, int answer, ARG_OUT bool & is_true) const
{
	if (NULL == ramgr) return NULL;

	std::map<int, std::pair<int, int> >::const_iterator it = m_answer_cfg.find(question);
	if (it == m_answer_cfg.end()) return NULL;

	if (it->second.first != answer)
	{
		is_true = false;
		return NULL;
	}

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			std::map<int, std::vector<ItemConfigData> >::const_iterator reward_it = m_reward_section_cfg[i].reward_list.find(it->second.second);
			if (reward_it == m_reward_section_cfg[i].reward_list.end()) return NULL;

			is_true = true;
			return &reward_it->second;
		}
	}

	return NULL;
}

int RandActivityChengFengPoLangConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		int problem_id = 0, correct = 0, reward_group = 0;

		if (!GetSubNodeValue(data_element, "problem_id", problem_id) || problem_id <= 0) return -1;
		if (!GetSubNodeValue(data_element, "correct", correct) || correct < 0) return -2;
		if (!GetSubNodeValue(data_element, "item_id", reward_group) || reward_group < 0) return -3;	// 运营的字段是item_id,实际上是奖励组....

		m_question_cfg.push_back(problem_id);
		m_answer_cfg[problem_id] = std::make_pair(correct, reward_group);

		data_element = data_element->NextSiblingElement();
	}

	if (m_question_cfg.size() > MAX_CHENG_FENG_PO_LANG_QUESTION) return -1313;

	return 0;
}

int RandActivityChengFengPoLangConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_reward_section_cfg.push_back(CFPLRewardSectionCfg());
			}
		}

		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_reward_section_cfg.push_back(CFPLRewardSectionCfg());
		}

		CFPLRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int reward_group = 0;

		if (!GetSubNodeValue(data_element, "item_id", reward_group) || reward_group < 0) return -1;		// 运营的字段是item_id,实际上是奖励组....

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -99;
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return-88;

		std::vector<ItemConfigData> reward_list;

		while (NULL != reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.reward_list[reward_group] = reward_list;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityChengFengPoLangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "frequency", m_other_cfg.frequency) || m_other_cfg.frequency < 0) return -1;
	if (!GetSubNodeValue(data_element, "time", m_other_cfg.time) || m_other_cfg.time <= 0) return -2;

	return 0;
}
