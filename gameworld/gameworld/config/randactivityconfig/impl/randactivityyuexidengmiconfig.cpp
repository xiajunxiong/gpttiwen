#include "randactivityyuexidengmiconfig.hpp"

RandActivityYueXiDengMiConfig::RandActivityYueXiDengMiConfig():m_question_total_rate(0)
{
}

RandActivityYueXiDengMiConfig::~RandActivityYueXiDengMiConfig()
{
}

bool RandActivityYueXiDengMiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("basis_configuration", InitBaseCfg);
	LOAD_RA_CONFIG_2("lantern_riddle_configuration", InitLanternRiddleCfg);
	LOAD_RA_CONFIG_2("other", InitOtherCfg);

	return true;
}

int RandActivityYueXiDengMiConfig::RandQuestion() const
{
	int real_rate = RandomNum(m_question_total_rate);

	for (size_t i = 0; i < m_question_cfg.size(); i++)
	{
		if (real_rate < m_question_cfg[i].rate)
		{
			return m_question_cfg[i].lantern_riddle_id;
		}
	}

	return 0;
}

bool RandActivityYueXiDengMiConfig::IsCorrectAnswer(int question_id, const char * answer) const
{
	if (NULL == answer) return false;

	for (size_t i = 0; i < m_question_cfg.size(); i++)
	{
		if (question_id == m_question_cfg[i].lantern_riddle_id)
		{
			if (0 == strcmp(answer, m_question_cfg[i].answer.c_str())) {
				return true;
			}
			else {
				return false;
			}
		}
	}

	return false;
}

const std::vector<ItemConfigData>* RandActivityYueXiDengMiConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI);

	for (size_t i = 0; i < m_basic_section_cfg.size(); i++)
	{
		if (m_basic_section_cfg[i].IsInclude(act_real_open_day))
		{
			int real_rate = RandomNum(m_basic_section_cfg[i].total_rate);

			for (size_t j = 0; j < m_basic_section_cfg[i].section_contents.size(); j++)
			{
				if (real_rate < m_basic_section_cfg[i].section_contents[j].rate)
				{
					return &m_basic_section_cfg[i].section_contents[j].reward_list;
				}
			}
		}
	}

	return NULL;
}

int RandActivityYueXiDengMiConfig::InitBaseCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_basic_section_cfg.size() > 0)
		{
			int pre_index = m_basic_section_cfg.size() - 1;
			if (section_start != m_basic_section_cfg[pre_index].section_start ||
				section_end != m_basic_section_cfg[pre_index].section_end)
			{
				if (section_start < m_basic_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_basic_section_cfg.push_back(RAYueXiDengMiBaseSectionCfg<std::vector<RAYueXIDengMiBaseCfg> >());
			}
		}

		if (m_basic_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_basic_section_cfg.push_back(RAYueXiDengMiBaseSectionCfg<std::vector<RAYueXIDengMiBaseCfg> >());
		}

		RAYueXiDengMiBaseSectionCfg<std::vector<RAYueXIDengMiBaseCfg> > & node_cfg = m_basic_section_cfg[m_basic_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAYueXIDengMiBaseCfg cfg;

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -1;
		node_cfg.total_rate += rate;
		cfg.rate = node_cfg.total_rate;

		TiXmlElement* reward_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -101;

		TiXmlElement* reward_element = reward_list_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -102;

		while (reward_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_element)) return -2;

			cfg.reward_list.push_back(item);

			reward_element = reward_element->NextSiblingElement();
		}

		node_cfg.section_contents.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYueXiDengMiConfig::InitLanternRiddleCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		RAYueXiDengMiQuestionCfg cfg;

		if (!GetSubNodeValue(data_element, "lantern_riddle_id", cfg.lantern_riddle_id) || cfg.lantern_riddle_id <= 0)
		{
			err = STRING_SPRINTF("lantern_riddle_id[%d] <= 0", cfg.lantern_riddle_id);
			return -1;
		}

		if (!GetSubNodeValue(data_element, "answer", cfg.answer) || cfg.answer.empty())
		{
			return -2;
		}

		int rate = 0;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -3;
		m_question_total_rate += rate;
		cfg.rate = m_question_total_rate;

		m_question_cfg.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityYueXiDengMiConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "day_answer_up", m_other_cfg.day_answer_limit) || m_other_cfg.day_answer_limit < 0)
	{
		return -1;
	}

	return 0;
}
