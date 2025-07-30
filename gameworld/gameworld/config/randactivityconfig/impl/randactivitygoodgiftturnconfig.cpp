#include "randactivitygoodgiftturnconfig.hpp"

RandActivityGoodGiftTurnConfig::RandActivityGoodGiftTurnConfig()
{
}

RandActivityGoodGiftTurnConfig::~RandActivityGoodGiftTurnConfig()
{
	std::vector<RAGoodGiftTurnSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityGoodGiftTurnConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("gift", InitRewardCfg);

	return true;
}

const RAGoodGiftTurnCfg * RandActivityGoodGiftTurnConfig::GetRandReward(RandActivityManager * ramgr, short fetch_flag)const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
	for (int i = 0; i < (int)m_reward_section_cfg.size(); i++)
	{
		if (m_reward_section_cfg[i].section_start <= act_real_open_day && m_reward_section_cfg[i].section_end >= act_real_open_day)
		{	
			int rate_count = m_reward_section_cfg[i].rate_count;
			std::vector<RAGoodGiftTurnCfg> can_rand_list;
			for (int k = 0; k < MAX_GOOD_GIFT_TURN_NUM; k++)
			{
				const RAGoodGiftTurnCfg & node_cfg = m_reward_section_cfg[i].node[k];
				if (fetch_flag & (1 << node_cfg.seq1))
				{
					rate_count -= node_cfg.rate;
				}
				else
				{
					can_rand_list.push_back(node_cfg);
				}
			}
			if (rate_count <= 0 || can_rand_list.empty())
			{
				return NULL;
			}
			int rand_rate = rand() % rate_count;
			for (int index = 0; index < (int)can_rand_list.size(); index++)
			{
				if (rand_rate <= can_rand_list[index].rate)
				{
					return &m_reward_section_cfg[i].node[can_rand_list[index].seq1];
				}
				rand_rate -= can_rand_list[index].rate;
			}
		}
	}
	return NULL;
}

int RandActivityGoodGiftTurnConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "reward_frequency", m_other_cfg.reward_frequency) || m_other_cfg.reward_frequency <= 0) return -1;
	if (!GetSubNodeValue(data_element, "reward_frequency_total", m_other_cfg.reward_frequency_total) || m_other_cfg.reward_frequency_total < m_other_cfg.reward_frequency ||
		m_other_cfg.reward_frequency_total > MAX_GOOD_GIFT_TURN_NUM) return -2;

	return 0;
}

int RandActivityGoodGiftTurnConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int seq = 0;
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

				m_reward_section_cfg.push_back(RAGoodGiftTurnSectionCfg());
				seq = 0;
			}
		}
		if (m_reward_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_reward_section_cfg.push_back(RAGoodGiftTurnSectionCfg());
			seq = 0;
		}

		RAGoodGiftTurnSectionCfg & node_cfg = m_reward_section_cfg[m_reward_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAGoodGiftTurnCfg turn_cfg;
		if (!GetSubNodeValue(data_element, "seq1", turn_cfg.seq1) || turn_cfg.seq1 < 0 || turn_cfg.seq1 >= MAX_GOOD_GIFT_TURN_NUM) return -1;
		if (!GetSubNodeValue(data_element, "weight", turn_cfg.rate) || turn_cfg.rate <= 0) return -2;

		TiXmlElement * reward_list_element = data_element->FirstChildElement("reward_list");
		if (NULL == reward_list_element)
		{
			return -100;
		}
		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
		while (reward_element != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -101;
			}
			turn_cfg.item_list.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		if(seq++ >= MAX_GOOD_GIFT_TURN_NUM) return -3;

		node_cfg.rate_count += turn_cfg.rate;
		node_cfg.node[turn_cfg.seq1] = turn_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
