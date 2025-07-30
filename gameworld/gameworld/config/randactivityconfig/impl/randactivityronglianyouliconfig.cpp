#include "randactivityronglianyouliconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

RandActivityRongLianYouLiConfig::RandActivityRongLianYouLiConfig()
{

}


RandActivityRongLianYouLiConfig::~RandActivityRongLianYouLiConfig()
{
	std::vector<RARongLianYouLiRewardSectionCfg>().swap(m_reward_section_cfg);
	std::vector<RARongLianYouLiStageRewardSectionCfg>().swap(m_stage_reward_section_cfg);
}

bool RandActivityRongLianYouLiConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitRewardCfg);
	LOAD_RA_CONFIG("phase_reward", InitStageRewardCfg);

	return true;
}

const std::vector<RARongLianYouLiRewardCfg> * RandActivityRongLianYouLiConfig::GetRewardCfg(RandActivityManager * ramgr) const
{
	if (ramgr == NULL) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);

	for (int i = 0; i < (int)m_reward_section_cfg.size(); ++i)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			return &m_reward_section_cfg[i].reward;
		}
	}

	return NULL;
}

const RARongLianYouLiStageRewardCfg * RandActivityRongLianYouLiConfig::GetRandStageRewardCfg(RandActivityManager * ramgr, int task_num) const
{
	if (NULL == ramgr || task_num < 0) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);

	for (int i = 0; i < (int)m_stage_reward_section_cfg.size(); ++i)
	{
		if (m_stage_reward_section_cfg[i].section_start <= act_real_open_day && m_stage_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; j < (int)m_stage_reward_section_cfg[i].reward_group.size(); ++j)
			{
				if (m_stage_reward_section_cfg[i].reward_group[j].task_num == task_num)
				{
					int rate_rand = RandomNum(m_stage_reward_section_cfg[i].reward_group[j].total_rate);

					for (int x = 0; x < (int)m_stage_reward_section_cfg[i].reward_group[j].reward_lsit.size(); ++x)
					{
						if (rate_rand < m_stage_reward_section_cfg[i].reward_group[j].reward_lsit[x].rate)
						{
							return &m_stage_reward_section_cfg[i].reward_group[j].reward_lsit[x];
						}
					}
				}
			}
		}
	}

	return NULL;
}

const std::vector<int> RandActivityRongLianYouLiConfig::GetTaskNumCfg(RandActivityManager * ramgr) const
{
	std::vector<int> task_num_list;

	if (NULL == ramgr) return task_num_list;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);

	for (int i = 0; i < (int)m_stage_reward_section_cfg.size(); ++i)
	{
		if (m_stage_reward_section_cfg[i].section_start <= act_real_open_day && m_stage_reward_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int j = 0; j < (int)m_stage_reward_section_cfg[i].reward_group.size(); ++j)
			{
				if (task_num_list.empty())
				{
					task_num_list.push_back(m_stage_reward_section_cfg[i].reward_group[j].task_num);
				}
				else
				{
					if (m_stage_reward_section_cfg[i].reward_group[j].task_num == task_num_list[task_num_list.size() - 1])
					{
						continue;
					}

					task_num_list.push_back(m_stage_reward_section_cfg[i].reward_group[j].task_num);
				}
			}
		}
	}

	return task_num_list;
}


int RandActivityRongLianYouLiConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
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

		if (m_reward_section_cfg.size() > 0)
		{
			int pre_index = m_reward_section_cfg.size() - 1;
			if (section_start != m_reward_section_cfg[pre_index].section_start ||
				section_end != m_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_reward_section_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_reward_section_cfg.push_back(RARongLianYouLiRewardSectionCfg());
			}
		}
		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_section_cfg.push_back(RARongLianYouLiRewardSectionCfg());
		}

		RARongLianYouLiRewardSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RARongLianYouLiRewardCfg reward_cfg;

		if (!GetSubNodeValue(data_element, "seq", reward_cfg.seq) || reward_cfg.seq < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "attribute_type", reward_cfg.attribute_type) || reward_cfg.attribute_type < 0 || reward_cfg.attribute_type >= RARongLianYouLi::RONG_LIAN_TYPE_MAX) return -__LINE__;
		if (!GetSubNodeValue(data_element, "parameter", reward_cfg.parameter) || reward_cfg.parameter < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_times", reward_cfg.reward_times) || reward_cfg.reward_times < 0) return -__LINE__;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_element) return -__LINE__;

		if (!reward_cfg.reward.ReadConfig(reward_element))
		{
			return -__LINE__;
		}

		node_cfg.reward.push_back(reward_cfg);

		if ((int)node_cfg.reward.size() > RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM)
		{
			return -__LINE__;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityRongLianYouLiConfig::InitStageRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
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

		if (m_stage_reward_section_cfg.size() > 0)
		{
			int pre_index = m_stage_reward_section_cfg.size() - 1;
			if (section_start != m_stage_reward_section_cfg[pre_index].section_start ||
				section_end != m_stage_reward_section_cfg[pre_index].section_end)
			{
				if (section_start < m_stage_reward_section_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_stage_reward_section_cfg.push_back(RARongLianYouLiStageRewardSectionCfg());
			}
		}
		if (m_stage_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_stage_reward_section_cfg.push_back(RARongLianYouLiStageRewardSectionCfg());
		}

		RARongLianYouLiStageRewardSectionCfg & node_cfg = m_stage_reward_section_cfg[m_stage_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int reward_group = 0, task_num = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "task_num", task_num) || task_num < 0) return -__LINE__;

		if (node_cfg.reward_group.empty())
		{
			node_cfg.reward_group.push_back(RARongLianYouLiStageRewardGroupCfg());
		}
		else
		{
			int pre_index = node_cfg.reward_group.size() - 1;
			if (node_cfg.reward_group[pre_index].reward_group != reward_group || node_cfg.reward_group[pre_index].task_num != task_num)
			{
				if (!(node_cfg.reward_group[pre_index].reward_group != reward_group && node_cfg.reward_group[pre_index].task_num != task_num))
				{
					return -__LINE__;
				}
				else
				{
					if (node_cfg.reward_group[pre_index].task_num > task_num)
					{
						return -__LINE__;
					}
					node_cfg.reward_group.push_back(RARongLianYouLiStageRewardGroupCfg());
				}
			}
		}

		if ((int)node_cfg.reward_group.size() > RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP)
		{
			return -__LINE__;
		}

		RARongLianYouLiStageRewardGroupCfg & ndoe_node_cfg = node_cfg.reward_group[node_cfg.reward_group.size() - 1];
		ndoe_node_cfg.reward_group = reward_group;
		ndoe_node_cfg.task_num = task_num;

		RARongLianYouLiStageRewardCfg reward_cfg;

		int rate = -1;
		if (!GetSubNodeValue(data_element, "rate", rate) || rate < 0) return -__LINE__;
		ndoe_node_cfg.total_rate += rate;
		reward_cfg.rate = ndoe_node_cfg.total_rate;

		std::vector<ItemConfigData> reward_list;

		TiXmlElement * regular_item_element = data_element->FirstChildElement("regular_item");
		if(NULL != regular_item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(regular_item_element))
			{
				return -__LINE__;
			}

			reward_list.push_back(item);
		}

		TiXmlElement * Rand_item_list_element = data_element->FirstChildElement("Rand_item_list");
		if (NULL != Rand_item_list_element)
		{
			TiXmlElement * Rand_item_element = Rand_item_list_element->FirstChildElement("Rand_item");
			if (NULL == Rand_item_element) return -__LINE__;

			while (NULL != Rand_item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(Rand_item_element))
				{
					return -__LINE__;
				}

				reward_list.push_back(item);

				Rand_item_element = Rand_item_element->NextSiblingElement();
			}
		}

		if (reward_list.empty()) return -__LINE__;

		reward_cfg.reawrd = reward_list;
		ndoe_node_cfg.reward_lsit.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
