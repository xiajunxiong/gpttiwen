#include "marriageconfig.hpp"
#include "item/itempool.h"

MarriageConfig::MarriageConfig()
{
	std::vector<MarriageTaskFbCfg>().swap(m_task_fb_cfg);
	std::map<int, std::vector<MarriageTaskQuestionCfg> >().swap(m_question_cfg);
}

MarriageConfig::~MarriageConfig()
{
	std::vector<MarriageTaskFbCfg>().swap(m_task_fb_cfg);

	for (std::map<int, std::vector<MarriageTaskQuestionCfg> >::iterator it = m_question_cfg.begin(); it != m_question_cfg.end(); it++)
	{
		std::vector<MarriageTaskQuestionCfg>().swap(it->second);
	}
}

bool MarriageConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("marriage_type", InitMarryCfg);

	LOAD_CONFIG("love_daily", InitMarryTaskCfg);
	LOAD_CONFIG("question", InitMarryQuestionCfg);
	LOAD_CONFIG("monster", InitMarryFbCfg);

	return true;
}

const MarriageOtherCfg & MarriageConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const MarriageMarryCfg * MarriageConfig::GetMarriageMarryCfg(int marry_type)
{
	if (m_marry_cfg.find(marry_type) != m_marry_cfg.end())
	{
		return &m_marry_cfg[marry_type];
	}

	return NULL;
}

const MarriageTaskQuestionCfg * MarriageConfig::GetTaskDuetQuestionCfg(int question_id)
{
	const std::vector<MarriageTaskQuestionCfg> & question = m_question_cfg[QUESTION_TYPE_DUET];
	
	int index = question_id - 1;
	if (index < 0 || index >= (int)question.size())
	{
		return NULL;
	}


	return &question[index];
}

const MarriageTaskQuestionCfg * MarriageConfig::GetTaskSingleQuestionCfg(int question_id)
{
	const std::vector<MarriageTaskQuestionCfg> & question = m_question_cfg[QUESTION_TYPE_SINGLE];

	int index = question_id - 1;
	if (index < 0 || index >= (int)question.size())
	{
		return NULL;
	}


	return &question[index];
}

int MarriageConfig::GetTaskRandQuestion(int type,const std::set<int> & already_have_question)
{
	const std::vector<MarriageTaskQuestionCfg> & question = m_question_cfg[QUESTION_TYPE_SINGLE];
	
	if (question.size() > already_have_question.size())
	{
		int r = rand() % (question.size() - already_have_question.size());
		for (int i = 0; i < (int)question.size(); ++i)
		{
			const MarriageTaskQuestionCfg & question_cfg = question[i];
			if (already_have_question.count(question_cfg.question_id) != 0)
			{
				continue;
			}

			r -= 1;

			if (r < 0)
			{
				return question_cfg.question_id;
			}
		}
	}

	return 0;
}

const MarriageTaskFbCfg * MarriageConfig::GetMarriageTaskFbCfg(int wave)
{
	if (wave < 0 || wave >= (int)m_task_fb_cfg.size())
	{
		return NULL;
	}

	return &m_task_fb_cfg[wave];
}

const MarriageTaskRewardCfg * MarriageConfig::GetMarriageTaskRewardCfg(int task_type)
{
	std::map<int, MarriageTaskRewardCfg>::iterator iter = m_marry_task_cfg.find(task_type);
	if (iter != m_marry_task_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

int MarriageConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "marriage_level", m_other_cfg.need_role_level) || m_other_cfg.need_role_level >= MAX_ROLE_LEVEL)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "marriage_intimacy", m_other_cfg.need_intimacy) || m_other_cfg.need_intimacy < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "divorce_days", m_other_cfg.can_divorce_time) || m_other_cfg.can_divorce_time < 0)
	{
		return -3;
	}
	m_other_cfg.can_divorce_time = m_other_cfg.can_divorce_time * 24 * 3600;

	if (!GetSubNodeValue(root_element, "confession_days", m_other_cfg.force_divorce_time) || m_other_cfg.force_divorce_time < 0)
	{
		return -4;
	}
	m_other_cfg.force_divorce_time = m_other_cfg.force_divorce_time * 24 * 3600;

	if (!GetSubNodeValue(root_element, "confession_days", m_other_cfg.force_divorce_time) || m_other_cfg.force_divorce_time < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "declaration", m_other_cfg.marriage_seek_need_coin))
	{
		return -6;
	}

	return 0;
}

int MarriageConfig::InitMarryCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		MarriageMarryCfg cfg;

		if (!GetSubNodeValue(root_element, "type", cfg.marry_type))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "item_id", cfg.item_id) ) //TODO:gfp || ITEMPOOL->GetItem(cfg.item_id) == NULL
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "item_num", cfg.need_item_num) || cfg.need_item_num < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin_num", cfg.need_coin) || cfg.need_coin < 0)
		{
			return -5;
		}

		if (cfg.item_id == 0 && cfg.need_coin == 0)
		{
			return -99;
		}

		m_marry_cfg[cfg.marry_type] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MarriageConfig::InitMarryQuestionCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	std::map<int, int>next_question_id;
	while (NULL != root_element)
	{
		int question_type = 0;

		if (!GetSubNodeValue(root_element, "question_type", question_type))
		{
			return -1;
		}

		std::vector<MarriageTaskQuestionCfg> & questions = m_question_cfg[question_type];

		MarriageTaskQuestionCfg cfg;

		if (!GetSubNodeValue(root_element, "question_id", cfg.question_id) || cfg.question_id != next_question_id[question_type] + 1)
		{
			return -2;
		}
		next_question_id[question_type] += 1;

		if (!GetSubNodeValue(root_element, "answer_id", cfg.answer))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.coin) || cfg.coin < 0)
		{
			return -4;
		}

		questions.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MarriageConfig::InitMarryTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int task_type = 0;
		if (!GetSubNodeValue(root_element, "act_id", task_type))
		{
			return -1;
		}

		MarriageTaskRewardCfg & cfg = m_marry_task_cfg[task_type];

		TiXmlElement * reward_list_element = root_element->FirstChildElement("item_list");

		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("item");

			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}
				cfg.reward_list.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}



		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MarriageConfig::InitMarryFbCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_wave = 1;
	while (NULL != root_element)
	{
		MarriageTaskFbCfg cfg;

		int wave = 0;
		if (!GetSubNodeValue(root_element, "num", wave) || wave != next_wave)
		{
			return -1;
		}
		next_wave += 1;

		if (!GetSubNodeValue(root_element, "monster_group", cfg.monster_id))
		{
			return -2;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");

			while (reward_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -104;
				}
				cfg.rewards.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_task_fb_cfg.push_back(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}


