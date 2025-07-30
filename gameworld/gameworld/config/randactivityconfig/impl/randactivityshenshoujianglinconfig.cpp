#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityshenshoujianglinconfig.hpp"

RandActivityShenShouJiangLinConfig::RandActivityShenShouJiangLinConfig()
{
}

RandActivityShenShouJiangLinConfig::~RandActivityShenShouJiangLinConfig()
{
	std::vector<RAShenShouJiangLinSectionCfg>().swap(m_section_cfg);
	std::vector<RAShenShouJiangLinOtherSectionCfg>().swap(m_other_section_cfg);
	std::vector<RAShenShouJiangLinBuySectionCfg>().swap(m_buy_section_cfg);
}

bool RandActivityShenShouJiangLinConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitDrawCfg);
	LOAD_RA_CONFIG("jackpot_configure", InitOtherCfg);
	LOAD_RA_CONFIG("pet_befall_configure", InitBuyCfg);

	return true;
}

int RandActivityShenShouJiangLinConfig::GetConfigVer(RandActivityManager * ramgr) const
{
	return this->GetOtherCfg(ramgr).cfg_ver;
}

const RAShenShouJiangLinOtherCfg & RandActivityShenShouJiangLinConfig::GetOtherCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	for (std::vector<RAShenShouJiangLinOtherSectionCfg>::const_iterator it = m_other_section_cfg.begin(); m_other_section_cfg.end() != it; ++it)
	{
		if (it->section_end >= act_real_open_day && act_real_open_day >= it->section_start)
		{
			return it->other_cfg;
		}

	}
	return m_other_section_cfg.rbegin()->other_cfg;
}

const RAShenShouJiangLinItemCfg * RandActivityShenShouJiangLinConfig::GetRandReward(RandActivityManager * ramgr) const
{
	const RAShenShouJiangLinSectionCfg * section_cfg = this->GetRAShenShouJiangLinSectionCfg(ramgr);
	if (!section_cfg)
	{
		return NULL;
	}

	int r = RandomNum(section_cfg->rate_count);
	for (int i = 0; i < (int)section_cfg->item.size(); ++i)
	{
		if (r < section_cfg->item[i].rate)
		{
			return &section_cfg->item[i];
		}

		r -= section_cfg->item[i].rate;
	}

	return NULL;
}

const RAShenShouJiangLinBuyItemCfg * RandActivityShenShouJiangLinConfig::GetRAShenShouJiangLinBuyItemCfg(RandActivityManager * ramgr, int seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	for (std::vector<RAShenShouJiangLinBuySectionCfg>::const_iterator it = m_buy_section_cfg.begin(); it != m_buy_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if (0 <= seq && seq < (int)it->item.size())
			{
				return &it->item[seq];
			}

			break;
		}
	}

	return NULL;
}

const RAShenShouJiangLinSectionCfg * RandActivityShenShouJiangLinConfig::GetRAShenShouJiangLinSectionCfg(RandActivityManager * ramgr) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	for (std::vector<RAShenShouJiangLinSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return &(*it);
		}
	}

	return NULL;
}

std::vector<const RAShenShouJiangLinItemCfg *> RandActivityShenShouJiangLinConfig::GetRAShenShouJiangLinBaoDiCfgList(RandActivityManager * ramgr, std::set<int> & bao_di_id_list) const
{
	std::vector<const RAShenShouJiangLinItemCfg *> ret;
	const RAShenShouJiangLinSectionCfg * section_cfg = this->GetRAShenShouJiangLinSectionCfg(ramgr);
	if (!section_cfg)
	{
		return ret;
	}

	for (int i = 0; i < (int)section_cfg->item.size(); ++i)
	{
		const RAShenShouJiangLinItemCfg & tmp_cfg = section_cfg->item[i];

		if (tmp_cfg.bao_di_times > 0)
		{
			ret.push_back(&tmp_cfg);
			bao_di_id_list.insert(tmp_cfg.bao_di_id);
		}
	}

	return ret;
}

int RandActivityShenShouJiangLinConfig::InitDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
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

				next_seq = 0;
				m_section_cfg.push_back(RAShenShouJiangLinSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAShenShouJiangLinSectionCfg());
		}

		RAShenShouJiangLinSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAShenShouJiangLinItemCfg item_cfg;
		
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq) return -__LINE__; 
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate) || item_cfg.rate < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_num", item_cfg.gold_num) || item_cfg.gold_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_per", item_cfg.pool_get_reward_percent) || item_cfg.pool_get_reward_percent < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "hearsay", item_cfg.broadcast)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "bao_di_id", item_cfg.bao_di_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "guarantee_times", item_cfg.bao_di_times)) return -__LINE__;

		if (item_cfg.bao_di_times > 0 && (item_cfg.bao_di_id <= 0 || item_cfg.bao_di_id > BAO_DI_INFO_ITEM_MAX_NUM))
		{
			return -__LINE__;
		}

		int reward_type = 0;
		if (!GetSubNodeValue(data_element, "reward_type", reward_type))return -__LINE__;
		if (reward_type == 0)
		{
			if (!item_cfg.reward.ReadConfig(data_element->FirstChildElement("reward_item"))) return -__LINE__;
		}
		
		next_seq += 1;
		node_cfg.rate_count += item_cfg.rate;
		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	for (int i = 0; i < (int)m_section_cfg.size(); ++i)
	{
		if (m_section_cfg[i].rate_count <= 0)
		{
			return -__LINE__;
		}
	}

	return 0;
}

int RandActivityShenShouJiangLinConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int last_section = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_other_section_cfg.push_back(RAShenShouJiangLinOtherSectionCfg(section_start, section_end));
			last_section = section_start;
		}

		RAShenShouJiangLinOtherCfg &m_other_cfg = m_other_section_cfg.rbegin()->other_cfg;

		if (!GetSubNodeValue(data_element, "jackpot_item_id", m_other_cfg.pool_item_id) || ITEMPOOL->GetItem(m_other_cfg.pool_item_id) == NULL) return -__LINE__;
		if (!GetSubNodeValue(data_element, "jackpot_item_num", m_other_cfg.pool_item_init_num) || m_other_cfg.pool_item_init_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "jackpot_gold_num", m_other_cfg.pool_gold_init_num) || m_other_cfg.pool_gold_init_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "all_draw_times", m_other_cfg.pool_add_item_draw_items) || m_other_cfg.pool_add_item_draw_items <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "add_item_num", m_other_cfg.pool_add_item_num) || m_other_cfg.pool_add_item_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "add_gold", m_other_cfg.pool_add_gold_num) || m_other_cfg.pool_add_gold_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_id", m_other_cfg.draw_comsume_item_id) || ITEMPOOL->GetItem(m_other_cfg.draw_comsume_item_id) == NULL) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_num", m_other_cfg.draw_comsume_num) || m_other_cfg.draw_comsume_num <= 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityShenShouJiangLinConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_seq = 0;
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

		if (m_buy_section_cfg.size() > 0)
		{
			int pre_index = m_buy_section_cfg.size() - 1;
			if (section_start != m_buy_section_cfg[pre_index].section_start ||
			    section_end != m_buy_section_cfg[pre_index].section_end)
			{
				if (section_start < m_buy_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				next_seq = 0;
				m_buy_section_cfg.push_back(RAShenShouJiangLinBuySectionCfg());
			}
		}
		if (m_buy_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_buy_section_cfg.push_back(RAShenShouJiangLinBuySectionCfg());
		}

		RAShenShouJiangLinBuySectionCfg & node_cfg = m_buy_section_cfg[m_buy_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAShenShouJiangLinBuyItemCfg item_cfg;

		if (!GetSubNodeValue(data_element, "type", item_cfg.type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq != next_seq || item_cfg.seq >= RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi) || item_cfg.need_chong_zhi < 0) return -__LINE__;

		TiXmlElement * item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward_item");
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

		next_seq += 1;
		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
