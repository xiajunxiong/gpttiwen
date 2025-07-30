#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityjinlongjubaoconfig.hpp"

RandActivityJinLongJuBaoConfig::RandActivityJinLongJuBaoConfig()
{
}

RandActivityJinLongJuBaoConfig::~RandActivityJinLongJuBaoConfig()
{
	std::vector<RandActivityJinLongJuBaoSectionCfg>().swap(m_section_cfg);
	std::vector<RandActivityJinLongJuBaoTimesRewardSectionCfg>().swap(m_times_reward_section_cfg);
	std::vector<RandActivityJinLongJuBaBuyItemSectionCfg>().swap(m_buy_section_cfg);
}

bool RandActivityJinLongJuBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherfg);
	LOAD_RA_CONFIG("basis_configuration", InitRandActivityJinLongJuBaoCfg);
	LOAD_RA_CONFIG("reward_configuration", InitRandActivityJinLongJuBaoTimesRewardCfg);
	LOAD_RA_CONFIG("reward_group", InitRandActivityJinLongJuBaoRewardGroupItemCfg);
	LOAD_RA_CONFIG("shop_configuration", InitRandActivityJinLongJuBaBuyItemCfg);

	return true;
}

const RandActivityJinLongJuBaoOtherCfg & RandActivityJinLongJuBaoConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

const RandActivityJinLongJuBaoCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaoCfg(RandActivityManager * ramgr, int reward_type) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO);
	for (std::vector<RandActivityJinLongJuBaoSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return MapValuesConstPtr(it->item_list, reward_type);
		}
	}

	return NULL;
}

const RandActivityJinLongJuBaoRewardGroupItemCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaoRewardGroupItemCfg(RandActivityManager * ramgr, int reward_type, int reward_seq) const
{
	const RandActivityJinLongJuBaoCfg * cfg = GetRandActivityJinLongJuBaoCfg(ramgr, reward_type);
	if (!cfg)
	{
		return NULL;
	}

	const RandActivityJinLongJuBaoRewardGroupCfg * reward_cfg = MapValuesConstPtr(m_reward_group_cfg, cfg->reward_group_id);
	if (!reward_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(reward_cfg->item_list, reward_seq);
}

const RandActivityJinLongJuBaoRewardGroupCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaoRewardGroupCfg(RandActivityManager * ramgr, int reward_type) const
{

	const RandActivityJinLongJuBaoCfg * cfg = GetRandActivityJinLongJuBaoCfg(ramgr, reward_type);
	if (!cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(m_reward_group_cfg, cfg->reward_group_id);
}

const RandActivityJinLongJuBaBuyItemCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaBuyItemCfg(RandActivityManager * ramgr, int buy_seq) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO);
	for (std::vector<RandActivityJinLongJuBaBuyItemSectionCfg>::const_iterator it = m_buy_section_cfg.begin(); it != m_buy_section_cfg.end(); it++)
	{
		const RandActivityJinLongJuBaBuyItemSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return MapValuesConstPtr(curr.item_list, buy_seq);
		}
	}

	return NULL;
}

const RandActivityJinLongJuBaoTimesRewardCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaoTimesRewardCfg(RandActivityManager * ramgr, int reward_seq) const
{
	const RandActivityJinLongJuBaoTimesRewardSectionCfg * section_cfg = this->GetRandActivityJinLongJuBaoTimesRewardSectionCfg(ramgr);
	if (!section_cfg)
	{
		return NULL;
	}

	return MapValuesConstPtr(section_cfg->item_list, reward_seq);
}

const RandActivityJinLongJuBaoTimesRewardSectionCfg * RandActivityJinLongJuBaoConfig::GetRandActivityJinLongJuBaoTimesRewardSectionCfg(RandActivityManager * ramgr, time_t time) const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO);
	if (time > 0)
	{
		act_real_open_day = this->GetOpenDayByTimestamp(ramgr, (unsigned int)time);
	}

	for (std::vector<RandActivityJinLongJuBaoTimesRewardSectionCfg>::const_iterator it = m_times_reward_section_cfg.begin(); it != m_times_reward_section_cfg.end(); it++)
	{
		const RandActivityJinLongJuBaoTimesRewardSectionCfg & curr = *it;
		if (curr.section_start <= act_real_open_day && act_real_open_day <= curr.section_end)
		{
			return &curr;
		}
	}

	return NULL;
}

int RandActivityJinLongJuBaoConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityJinLongJuBaoConfig::InitOtherfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.draw_need_item_id)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "num", m_other_cfg.draw_need_item_num)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver)) return -__LINE__;

	return 0;
}

int RandActivityJinLongJuBaoConfig::InitRandActivityJinLongJuBaoCfg(TiXmlElement * RootElement)
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

				m_section_cfg.push_back(RandActivityJinLongJuBaoSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RandActivityJinLongJuBaoSectionCfg());
		}

		RandActivityJinLongJuBaoSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityJinLongJuBaoCfg item_cfg;
		if (!GetSubNodeValue(data_element, "jackpot_type", item_cfg.type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_group", item_cfg.reward_group_id)) return -__LINE__;

		node_cfg.item_list[item_cfg.type] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinLongJuBaoConfig::InitRandActivityJinLongJuBaoTimesRewardCfg(TiXmlElement * RootElement)
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
		std::vector<RandActivityJinLongJuBaoTimesRewardSectionCfg> & section_cfg_ref = m_times_reward_section_cfg;
		if (section_cfg_ref.size() > 0)
		{
			int pre_index = section_cfg_ref.size() - 1;
			if (section_start != section_cfg_ref[pre_index].section_start ||
				section_end != section_cfg_ref[pre_index].section_end)
			{
				if (section_start < section_cfg_ref[pre_index].section_end)
				{
					return -888;
				}

				section_cfg_ref.push_back(RandActivityJinLongJuBaoTimesRewardSectionCfg());
			}
		}
		if (section_cfg_ref.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg_ref.push_back(RandActivityJinLongJuBaoTimesRewardSectionCfg());
		}

		RandActivityJinLongJuBaoTimesRewardSectionCfg & node_cfg = section_cfg_ref[section_cfg_ref.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityJinLongJuBaoTimesRewardCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "times", item_cfg.times)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinLongJuBaoConfig::InitRandActivityJinLongJuBaoRewardGroupItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group_id)) return -__LINE__;
		RandActivityJinLongJuBaoRewardGroupCfg & cfg = m_reward_group_cfg[reward_group_id];
		cfg.reward_group_id = reward_group_id;

		RandActivityJinLongJuBaoRewardGroupItemCfg item_cfg;
		item_cfg.reward_group_id = reward_group_id;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.reward_seq)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "rate", item_cfg.rate)) return -__LINE__;
		if (ReadItemConfigData2(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "bao_di_id", item_cfg.bao_di_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "guarantee_times", item_cfg.bao_di_times)) return -__LINE__;

		if (item_cfg.bao_di_times > 0 && (item_cfg.bao_di_id <= 0 || item_cfg.bao_di_id > BAO_DI_INFO_ITEM_MAX_NUM))
		{
			return -__LINE__;
		}

		cfg.item_list[item_cfg.reward_seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJinLongJuBaoConfig::InitRandActivityJinLongJuBaBuyItemCfg(TiXmlElement * RootElement)
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
		std::vector<RandActivityJinLongJuBaBuyItemSectionCfg> & section_cfg_ref = m_buy_section_cfg;
		if (section_cfg_ref.size() > 0)
		{
			int pre_index = section_cfg_ref.size() - 1;
			if (section_start != section_cfg_ref[pre_index].section_start ||
				section_end != section_cfg_ref[pre_index].section_end)
			{
				if (section_start < section_cfg_ref[pre_index].section_end)
				{
					return -888;
				}

				section_cfg_ref.push_back(RandActivityJinLongJuBaBuyItemSectionCfg());
			}
		}
		if (section_cfg_ref.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			section_cfg_ref.push_back(RandActivityJinLongJuBaBuyItemSectionCfg());
		}

		RandActivityJinLongJuBaBuyItemSectionCfg & node_cfg = section_cfg_ref[section_cfg_ref.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RandActivityJinLongJuBaBuyItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq >= JIN_LONG_JU_BAO_MAX_BUY_ITEM) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "limit_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "day_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "price", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;
		if (ReadItemConfigDataList(data_element, "reward_item", item_cfg.rewards) != 0) return -__LINE__;

		node_cfg.item_list[item_cfg.seq] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
