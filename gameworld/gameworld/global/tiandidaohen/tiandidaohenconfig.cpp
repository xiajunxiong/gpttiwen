#include "tiandidaohenconfig.hpp"

TianDiDaoHenConfig::TianDiDaoHenConfig()
{
}

TianDiDaoHenConfig::~TianDiDaoHenConfig()
{
}

bool TianDiDaoHenConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("collect_group", InitCollectCfg);
	LOAD_CONFIG("reward", InitGroupRewardCfg);

	return true;
}

int TianDiDaoHenConfig::GetRandSeq()
{
	if (m_collection_cfg.list_cfg.empty())
	{
		return 0;
	}

	return rand() % m_collection_cfg.list_cfg.size();
}

const TianDiDaoHenCollectionSeqCfg * TianDiDaoHenConfig::GetTianDiDaoHenCollectionSeqCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_collection_cfg.list_cfg.size())
	{
		return NULL;
	}

	return &m_collection_cfg.list_cfg[seq];
}

const TianDiDaoHenRewardGroupCfg::ItemCfg * TianDiDaoHenConfig::GetRandReward(int group_id)
{
	const TianDiDaoHenRewardGroupCfg * cfg = MapValuesConstPtr(m_reward_cfg, group_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	int r = RandomNum(cfg->rate_count);
	for (int i = 0; i < (int)cfg->list_cfg.size(); ++i)
	{
		const TianDiDaoHenRewardGroupCfg::ItemCfg & curr = cfg->list_cfg[i];
		if (r < curr.rate)
		{
			return &curr;
		}
		r -= curr.rate;
	}

	return NULL;
}

ItemConfigData * TianDiDaoHenConfig::GetRandDropItem()
{
	int r = RandomNum(10000);
	if (r>= m_other_cfg.get_item_rate)
	{
		return NULL;
	}

	if (m_other_cfg.get_item_list.empty())
	{
		return NULL;
	}
	
	int r2 = RandomNum((int)m_other_cfg.get_item_list.size());
	return &m_other_cfg.get_item_list[r2];
}

const TianDiDaoHenOtherCfg & TianDiDaoHenConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int TianDiDaoHenConfig::InitCollectCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		TianDiDaoHenCollectionSeqCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq != next_seq++)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "final_reward_group_id", cfg.final_reward_group_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "final_reward_group_id_2", cfg.final_reward_group_id_2))
		{
			return -__LINE__;
		}

		TianDiDaoHenCollectionSeqCfg::ItemCfg item_cfg;
		for (int i = 0; i < 4; ++i)
		{
			std::string item_str = STRING_SPRINTF("item_%d", i + 1);
			std::string num_str = STRING_SPRINTF("num_%d", i + 1);
			std::string group_id_str = STRING_SPRINTF("reward_group_id_%d", i + 1);

			if (!GetSubNodeValue(root_element, item_str, item_cfg.need_item_id) )
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, num_str, item_cfg.need_item_num))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, group_id_str, item_cfg.reward_group_id))
			{
				return -__LINE__;
			}

			cfg.item_cfg.push_back(item_cfg);
		}

		m_collection_cfg.list_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TianDiDaoHenConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "rate", m_other_cfg.get_item_rate) || m_other_cfg.get_item_rate < 0)
	{
		return -__LINE__;
	}

	for (int i = 0; i < 4; ++i)
	{
		std::string item_str = STRING_SPRINTF("item_list_%d", i + 1);

		TiXmlElement * item_element = root_element->FirstChildElement(item_str.c_str());
		ItemConfigData item;
		if (!item.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		m_other_cfg.get_item_list.push_back(item);
	}

	if (!GetSubNodeValue(root_element, "open_time", m_other_cfg.begin_day) || m_other_cfg.begin_day <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "open_days", m_other_cfg.open_during_day) || m_other_cfg.open_during_day <= 0)
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "relax_days", m_other_cfg.close_during_day) || m_other_cfg.close_during_day <= 0)
	{
		return -__LINE__;
	}

	return 0;
}

int TianDiDaoHenConfig::InitGroupRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int next_seq = 0;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", group_id) )
		{
			return -__LINE__;
		}

		TianDiDaoHenRewardGroupCfg & cfg = m_reward_cfg[group_id];

		TianDiDaoHenRewardGroupCfg::ItemCfg item_cfg;
		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("item_list");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				item_cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		cfg.rate_count += item_cfg.rate;
		cfg.list_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
