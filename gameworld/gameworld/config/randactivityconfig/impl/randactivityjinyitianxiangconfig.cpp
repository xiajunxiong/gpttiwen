#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityjinyitianxiangconfig.hpp"

RandActivityJinYiTianXiangConfig::RandActivityJinYiTianXiangConfig()
{
}

RandActivityJinYiTianXiangConfig::~RandActivityJinYiTianXiangConfig()
{
}

bool RandActivityJinYiTianXiangConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("reward", InitDrawCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("buy", InitDrawBuyCfg);
	LOAD_RA_CONFIG("draw", InitDrawConsumeCfg);

	return true;
}

const RAJinYiTianXiangOtherCfg & RandActivityJinYiTianXiangConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RAJinYiTianXiangItemCfg * RandActivityJinYiTianXiangConfig::GetRandRewardCfg(RandActivityManager * ramgr, unsigned int exclude_flag) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG);

	RAJinYiTianXiangSectionCfg cfg;
	const RAJinYiTianXiangSectionCfg * cfg_ptr = NULL;
	cfg.rate_count = 0;

	if (m_section_list_cfg.empty())
	{
		return NULL;
	}

	const std::vector<RAJinYiTianXiangSectionCfg>  & section_cfg = m_section_list_cfg.rbegin()->second.sections_cfg;
	for (std::vector<RAJinYiTianXiangSectionCfg>::const_iterator it = section_cfg.begin(); it != section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			cfg = *it;
			cfg_ptr = &(*it);
			break;
		}
	}

	if (!cfg_ptr)
	{
		return NULL;
	}

	int rate_count = 0;
	for (int i = 0; i < (int)cfg.item_list.size(); ++i)
	{
		RAJinYiTianXiangItemCfg & curr = cfg.item_list[i];
		if (::IsSetBit(exclude_flag,curr.seq))
		{
			curr.rate = 0;
		}

		rate_count += curr.rate;
	}

	int r = RandomNum(rate_count);
	for (int i = 0; i < (int)cfg.item_list.size() && i < (int)cfg_ptr->item_list.size(); ++i)
	{
		RAJinYiTianXiangItemCfg & curr = cfg.item_list[i];
		if (r < curr.rate)
		{
			return &cfg_ptr->item_list[i];
		}

		r -= curr.rate;
	}

	return NULL;
}

const RAJinYiTianXiangItemCfg * RandActivityJinYiTianXiangConfig::GetRandRewardCfgByMarkAndTime(RandActivityManager * ramgr, int mark, time_t activity_open_time, unsigned int exclude_flag) const
{
	const RAJinYiTianXiangDrawSectionCfgList * list_cfg = MapValuesConstPtr(m_section_list_cfg, mark);
	if (!list_cfg)
	{
		return NULL;
	}

	int act_real_open_day = this->GetOpenDayByTimestamp(ramgr, (unsigned int)activity_open_time);

	RAJinYiTianXiangSectionCfg cfg;
	const RAJinYiTianXiangSectionCfg * cfg_ptr = NULL;
	cfg.rate_count = 0;

	const std::vector<RAJinYiTianXiangSectionCfg> & section_cfg = list_cfg->sections_cfg;
	for (std::vector<RAJinYiTianXiangSectionCfg>::const_iterator it = section_cfg.begin(); it != section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			cfg = *it;
			cfg_ptr = &(*it);
			break;
		}
	}

	if (!cfg_ptr)
	{
		return NULL;
	}

	int rate_count = 0;
	for (int i = 0; i < (int)cfg.item_list.size(); ++i)
	{
		RAJinYiTianXiangItemCfg & curr = cfg.item_list[i];
		if (::IsSetBit(exclude_flag, curr.seq))
		{
			curr.rate = 0;
		}

		rate_count += curr.rate;
	}

	int r = RandomNum(rate_count);
	for (int i = 0; i < (int)cfg.item_list.size() && i < (int)cfg_ptr->item_list.size(); ++i)
	{
		RAJinYiTianXiangItemCfg & curr = cfg.item_list[i];
		if (r < curr.rate)
		{
			return &cfg_ptr->item_list[i];
		}

		r -= curr.rate;
	}

	return NULL;
}

const RAJinYiTianXiangBuyCfg * RandActivityJinYiTianXiangConfig::GetRAJinYiTianXiangBuyCfg(int buy_seq) const
{
	return MapValuesConstPtr(m_buy_cfg, buy_seq);
}

const RAJinYiTianXiangConsumeCfg * RandActivityJinYiTianXiangConfig::GetDrawConsumeCfg(int draw_times) const
{
	return MapValuesConstPtr(m_consume_cfg, draw_times);
}

int RandActivityJinYiTianXiangConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityJinYiTianXiangConfig::InitDrawBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAJinYiTianXiangBuyCfg cfg;

		if (!GetSubNodeValue(data_element, "seq", cfg.buy_seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", cfg.need_chong_zhi_gold)) return -__LINE__;

		if (!GetSubNodeValue(data_element, "cfg_or", cfg.is_need_chong_zhi)) return -__LINE__;

		if (cfg.is_need_chong_zhi)
		{
			if (!GetSubNodeValue(data_element, "price", cfg.need_chong_zhi_gold)) return -__LINE__;
		}

		if (!GetSubNodeValue(data_element, "price_type", cfg.money_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", cfg.money_value)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "draw_buy", cfg.add_draw_value)) return -__LINE__;

		m_buy_cfg[cfg.buy_seq] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinYiTianXiangConfig::InitDrawConsumeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAJinYiTianXiangConsumeCfg cfg;

		if (!GetSubNodeValue(data_element, "draw", cfg.draw_times)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "draw_consume", cfg.draw_consume_value)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", cfg.buy_seq)) return -__LINE__;

		m_consume_cfg[cfg.draw_times] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinYiTianXiangConfig::InitDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
	while (NULL != data_element)
	{
		int mark = 0;
		if (!GetSubNodeValue(data_element, "mark", mark)) ret_line;

		if (m_section_list_cfg.find(mark) == m_section_list_cfg.end())
		{
			next_seq = 0;
		}

		RAJinYiTianXiangDrawSectionCfgList & section_list_cfg = m_section_list_cfg[mark];
		std::vector<RAJinYiTianXiangSectionCfg> & section_cfg = section_list_cfg.sections_cfg;
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

			if (section_cfg.size() > 0)
			{
				int pre_index = section_cfg.size() - 1;
				if (section_start != section_cfg[pre_index].section_start ||
					section_end != section_cfg[pre_index].section_end)
				{
					if (section_start < section_cfg[pre_index].section_end)
					{
						return -888;
					}

					next_seq = 0;
					section_cfg.push_back(RAJinYiTianXiangSectionCfg());
				}
			}
			if (section_cfg.size() == 0)
			{
				if (section_start != 1)
				{
					return -11111;
				}
				section_cfg.push_back(RAJinYiTianXiangSectionCfg());
			}

			RAJinYiTianXiangSectionCfg & node_cfg = section_cfg[section_cfg.size() - 1];
			node_cfg.section_start = section_start;
			node_cfg.section_end = section_end;

			RAJinYiTianXiangItemCfg item_cfg;
			ItemConfigData t;

			if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq) return -__LINE__;
			if (!GetSubNodeValue(data_element, "rate", item_cfg.rate)) return -__LINE__;
			if (!GetSubNodeValue(data_element, "hearsay", item_cfg.is_broadcast)) return -__LINE__;
			if (ReadItemConfigData(data_element, "reward_item", t) != 0)return -__LINE__;
			item_cfg.rewards.push_back(t);
			next_seq += 1;

			node_cfg.rate_count += item_cfg.rate;
			node_cfg.item_list.push_back(item_cfg);
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinYiTianXiangConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "mark", m_other_cfg.mark)) return -__LINE__;

	return 0;
}
