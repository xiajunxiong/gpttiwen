#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitycaishenjubaoconfig.hpp"

RandActivityCaiShenJuBaoConfig::RandActivityCaiShenJuBaoConfig()
{
}

RandActivityCaiShenJuBaoConfig::~RandActivityCaiShenJuBaoConfig()
{
	std::vector<RACaiShenJuBaoSectionCfg>().swap(m_section_cfg);
}

bool RandActivityCaiShenJuBaoConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("cornucopia_configure", InitBuyCfg);

	return true;
}

const RACaiShenJuBaoItemCfg * RandActivityCaiShenJuBaoConfig::GetRACaiShenJuBaoItemCfg(RandActivityManager * ramgr,int seq) const
{
	if (seq < 0 || seq >= RA_CAI_SHEN_JU_BAO_MAX_ITEM) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO);
	for (std::vector<RACaiShenJuBaoSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			if (seq >= (int)it->item.size())
			{
				return NULL;
			}

			return &it->item[seq];
		}
	}

	return NULL;
}

int RandActivityCaiShenJuBaoConfig::GetRACaiShenJuBaoItemNum(RandActivityManager * ramgr)const
{
	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO);
	for (std::vector<RACaiShenJuBaoSectionCfg>::const_iterator it = m_section_cfg.begin(); it != m_section_cfg.end(); it++)
	{
		if (it->section_start <= act_real_open_day && it->section_end >= act_real_open_day)
		{
			return (int)it->item.size();
		}
	}

	return 0;
}

int RandActivityCaiShenJuBaoConfig::InitBuyCfg(TiXmlElement * RootElement)
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
				m_section_cfg.push_back(RACaiShenJuBaoSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RACaiShenJuBaoSectionCfg());
		}

		RACaiShenJuBaoSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != next_seq || seq >= RA_CAI_SHEN_JU_BAO_MAX_ITEM) return -1;
		RACaiShenJuBaoItemCfg item_cfg;
		item_cfg.seq = seq;
		next_seq += 1;

		if (!GetSubNodeValue(data_element, "limit_type", item_cfg.limit_type)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_times", item_cfg.limit_buy_times) || item_cfg.limit_buy_times < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "today_times", item_cfg.limit_buy_times_day) || item_cfg.limit_buy_times_day < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_num", item_cfg.add_gold) || item_cfg.add_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_give", item_cfg.extra_add_gold) || item_cfg.extra_add_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi_gold) || item_cfg.need_chong_zhi_gold < 0) return -__LINE__;

		node_cfg.item.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
