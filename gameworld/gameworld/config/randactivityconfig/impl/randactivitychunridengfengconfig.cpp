#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitychunridengfengconfig.hpp"

RandActivityChunRiDengFengConfig::RandActivityChunRiDengFengConfig()
{
}

RandActivityChunRiDengFengConfig::~RandActivityChunRiDengFengConfig()
{
	std::vector<RAChunRiDengFengSectionCfg>().swap(m_section_cfg);
}

bool RandActivityChunRiDengFengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("spring_climb", InitLayerCfg);

	return true;
}


const RAChunRiDengFengOtherCfg & RandActivityChunRiDengFengConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RAChunRiDengFengLayerCfg * RandActivityChunRiDengFengConfig::GetLayerCfg(RandActivityManager * ramgr, int layer) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG);
	for (std::vector<RAChunRiDengFengSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		const RAChunRiDengFengSectionCfg & curr = *it;

		if (curr.section_start <= act_real_open_day && curr.section_end >= act_real_open_day)
		{
			std::map<int, RAChunRiDengFengLayerCfg> ::const_iterator find_iter = curr.layer_cfg.find(layer);
			if (find_iter != curr.layer_cfg.end())
			{
				return &find_iter->second;
			}

			break;
		}
	}

	return NULL;
}

bool RandActivityChunRiDengFengConfig::CanGetLastLayerItem(RandActivityManager * ramgr, unsigned short get_flag) const
{
	const RAChunRiDengFengLayerCfg * layer_cfg = this->GetLayerCfg(ramgr, RA_CHUN_RI_DENG_FENG_MAX_LEVEL);
	if (layer_cfg)
	{
		bool is_succ = false;
		const RAChunRiDengFengLayerCfg::ItemCfg * item_cfg = layer_cfg->GetRandItem(&is_succ, get_flag);
		return item_cfg && is_succ;
	}

	return false;
}

int RandActivityChunRiDengFengConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}


	if (!GetSubNodeValue(data_element, "item_id_1", m_other_cfg.explore_comsume_item_id))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(data_element, "item_num_1", m_other_cfg.explore_comsume_num))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(data_element, "explore_1", m_other_cfg.change_base_layer_explore_times))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(data_element, "initial", m_other_cfg.base_layer))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(data_element, "explore_2", m_other_cfg.goto_last_layer_explore_times) || m_other_cfg.goto_last_layer_explore_times <= 0)
	{
		return -__LINE__;
	}

	return 0;
}

int RandActivityChunRiDengFengConfig::InitLayerCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
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

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
			    section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_section_cfg.push_back(RAChunRiDengFengSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAChunRiDengFengSectionCfg());
		}

		RAChunRiDengFengSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int layer = 0;
		if (!GetSubNodeValue(data_element, "layer", layer) || layer < 0 || layer > RA_CHUN_RI_DENG_FENG_MAX_LEVEL) return -__LINE__;

		RAChunRiDengFengLayerCfg & layer_cfg = node_cfg.layer_cfg[layer];
		layer_cfg.layer = layer;

		RAChunRiDengFengLayerCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate) || item_cfg.rate <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != (int)layer_cfg.reward.size()) return -__LINE__;

		if (!GetSubNodeValue(data_element, "into_next", item_cfg.is_into_next)) return -__LINE__;
		if (!item_cfg.is_into_next)
		{
			TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
			if (!item_cfg.reward.ReadConfig(reward_element))   return -__LINE__;

			if (layer == RA_CHUN_RI_DENG_FENG_MAX_LEVEL)
			{
				layer_cfg.last_layer_item_num += 1;
			}

			if (layer_cfg.last_layer_item_num >= RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM) return - __LINE__;
		}

		layer_cfg.rate_count += item_cfg.rate;
		layer_cfg.reward.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

const RAChunRiDengFengLayerCfg::ItemCfg * RAChunRiDengFengLayerCfg::GetRandItem(bool * is_succ, unsigned short get_flag) const
{
	if (is_succ != NULL)
	{
		*is_succ = true;
	}

	int temp_rate_count = rate_count;
	for (int i = 0; i < RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM && i < (int)reward.size(); ++i)
	{
		if (get_flag & (1 << i))
		{
			temp_rate_count -= reward[i].rate;
		}
	}

	if (temp_rate_count <= 0)
	{
		return NULL;
	}

	int r = RandomNum(temp_rate_count);
	for (int i = 0; i < (int)reward.size(); ++i)
	{
		if (get_flag & (1 << i))
		{
			continue;
		}

		if (r < reward[i].rate)
		{
			if (is_succ != NULL)
			{
				*is_succ = true;
			}

			return &reward[i];
		}

		r -= reward[i].rate;
	}

	return NULL;
}
