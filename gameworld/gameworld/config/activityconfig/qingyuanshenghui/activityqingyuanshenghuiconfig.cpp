#include "activityqingyuanshenghuiconfig.hpp"

ActivityQingYuanShengHuiConfig::ActivityQingYuanShengHuiConfig()
{
}

ActivityQingYuanShengHuiConfig::~ActivityQingYuanShengHuiConfig()
{
}

bool ActivityQingYuanShengHuiConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitGroupRewardCfg);
	LOAD_CONFIG("game_link", InitPhaseCfg);
	LOAD_CONFIG("zeus_and_roxanne", InitQuestionCfg);
	

	return true;
}

const QingYuanShengHuiGroupRewardItemCfg * ActivityQingYuanShengHuiConfig::GetGatherReward()
{
	return this->GetRewardByGroupId(m_other_cfg.gather_reward_group_id);
}

const QingYuanShengHuiOtherCfg & ActivityQingYuanShengHuiConfig::GetQingYuanShengHuiOtherCfg()
{
	return m_other_cfg;
}

const QingYuanShengHuiPhaseCfg * ActivityQingYuanShengHuiConfig::GetQingYuanShengHuiPhaseCfg(int phase)
{	
	return MapValuesConstPtr(m_phase_cfg, phase);
}

const QingYuanShengHuiQuestionCfg * ActivityQingYuanShengHuiConfig::GetQingYuanShengHuiQuestionCfg(int question_id)
{
	return MapValuesConstPtr(m_question_cfg, question_id);
}

int ActivityQingYuanShengHuiConfig::GetRandQuestionId(std::set<int> exclude_list)
{
	std::vector<int> rand_list;
	for (std::map<int, QingYuanShengHuiQuestionCfg>::iterator iter = m_question_cfg.begin(); iter != m_question_cfg.end(); ++iter)
	{
		int question_id = iter->first;
		if (exclude_list.find(iter->first) != exclude_list.end())
		{
			continue;
		}

		rand_list.push_back(question_id);
	}

	if (rand_list.empty())
	{
		return 0;
	}

	return rand_list[rand() % (int)rand_list.size()];
}

const QingYuanShengHuiGroupRewardItemCfg * ActivityQingYuanShengHuiConfig::GetRewardByGroupId(int reward_group_id)
{
	const QingYuanShengHuiGroupRewardCfg * cfg = MapValuesConstPtr(m_group_reward_cfg, reward_group_id);
	if (!cfg || cfg->rate_count <= 0)
	{
		return NULL;
	}

	int r = ::RandomNum(cfg->rate_count);
	for (int i = 0; i < (int)cfg->group_list.size(); ++i)
	{
		const QingYuanShengHuiGroupRewardItemCfg & curr = cfg->group_list[i];
		if (r < curr.rate)
		{
			return &curr;
		}

		r -= curr.rate;
	}

	return NULL;
}

int ActivityQingYuanShengHuiConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "wish_reward_group", m_other_cfg.qi_yuan_reward_group_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "eatreward_time", m_other_cfg.gather_interval))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "eatreward_group", m_other_cfg.gather_reward_group_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "scene_reward_time", m_other_cfg.scene_reward_interval))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "scene_reward_group", m_other_cfg.scene_reward_group_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "orderreward_group", m_other_cfg.yu_yue_reward_group_id))
	{
		return -__LINE__;
	}

	for (int i = 0; i < 3; ++i)
	{
		std::string s = STRING_SPRINTF("warreward_%d", i);

		if (ReadItemConfigDataList(root_element, s, m_other_cfg.m_battle_reward[i]) != 0)
		{ 
			ret_line;
		}		
	}

	if (!GetSubNodeValue(root_element, "question_amount", m_other_cfg.max_question_num))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "anwser_time", m_other_cfg.question_interval))
	{
		return -__LINE__;
	}


	return 0;
}

int ActivityQingYuanShengHuiConfig::InitGroupRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", group_id))
		{
			ret_line;
		}

		QingYuanShengHuiGroupRewardCfg & cfg = m_group_reward_cfg[group_id];

		QingYuanShengHuiGroupRewardItemCfg item_cfg;


		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "buff", item_cfg.buff_id))
		{
			ret_line;
		}
		
		if (item_cfg.buff_id == 0)
		{
			ItemConfigData tmp;
			if (ReadItemConfigData(root_element, "reward", tmp, false) != 0)
			{
				ret_line;
			}

			item_cfg.rewards.push_back(tmp);
		}

		cfg.rate_count += item_cfg.rate;
		cfg.group_list.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityQingYuanShengHuiConfig::InitPhaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		QingYuanShengHuiPhaseCfg cfg;
		if (!GetSubNodeValue(root_element, "link_type", cfg.phase))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "last_time", cfg.phase_length_of_time))
		{
			ret_line;
		}
		m_phase_cfg[cfg.phase] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityQingYuanShengHuiConfig::InitQuestionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		QingYuanShengHuiQuestionCfg cfg;
		if (!GetSubNodeValue(root_element, "question_id", cfg.question_id))
		{
			ret_line;
		}

		if (ReadItemConfigDataList(root_element, "trueanswer", cfg.same_answer_rewards) != 0)
		{
			ret_line;
		}

		if (ReadItemConfigDataList(root_element, "wronganswer", cfg.diff_answer_rewards) != 0)
		{
			ret_line;
		}

		m_question_cfg[cfg.question_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
