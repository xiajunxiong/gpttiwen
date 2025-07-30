#include "randactivityhunqifanliconfig.hpp"

RandActivityHunQiFanLiConfig::RandActivityHunQiFanLiConfig()
{
}

RandActivityHunQiFanLiConfig::~RandActivityHunQiFanLiConfig()
{
	std::vector<RAHunQiFanLiBasicSectionCfg>().swap(m_basic_section_cfg);
	std::vector<RAHunQiFanLiPhaseSectionCfg>().swap(m_phase_reward_cfg);
	std::vector<RAHunQiFanLiChongZhiSectionCfg>().swap(m_chong_zhi_cfg);
}

bool RandActivityHunQiFanLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("gift_configure", InitBasisCfg);
	LOAD_RA_CONFIG_2("phase_reward", InitPhaseRewardCfg);
	LOAD_RA_CONFIG_2("recharge_configure", InitChongZhiCfg);

	return true;
}

const ItemConfigData * RandActivityHunQiFanLiConfig::RandRewardCfg(RandActivityManager * ramgr, BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP>& used_flag, int* turntable_length) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);
	
	int total_rate = 0;
	int cur_section = -1;	
	std::vector<std::pair<int, RAHunQiFanLiBasicCfg> > t_reward_vec;
	for (size_t i = 0; i < m_basic_section_cfg.size(); i++)
	{
		if (m_basic_section_cfg[i].IsInclude(act_real_open_day))
		{		
			for (size_t j = 0; j < m_basic_section_cfg[i].basic_cfg.size(); j++)
			{
				if(used_flag.IsBitSet(j)) continue;

				total_rate += m_basic_section_cfg[i].basic_cfg[j].rate;
				t_reward_vec.push_back(std::make_pair(j, RAHunQiFanLiBasicCfg(total_rate, m_basic_section_cfg[i].basic_cfg[j].reward)));
			}

			cur_section = i;
			if (NULL != turntable_length) *turntable_length = (int)m_basic_section_cfg[i].basic_cfg.size();
		}
	}

	if (total_rate == 0 || cur_section < 0 || cur_section >= (int)m_basic_section_cfg.size() || t_reward_vec.empty()) return NULL;

	int last_rate = RandomNum(total_rate);

	for (size_t i = 0; i < t_reward_vec.size(); i++)
	{
		if (t_reward_vec[i].second.rate > last_rate)
		{
			used_flag.SetBit(t_reward_vec[i].first);
			return &m_basic_section_cfg[cur_section].basic_cfg[t_reward_vec[i].first].reward;	// t_reward_vec是一个临时变量,不可传出指针
		}
	}

	return NULL;
}

const std::pair<int, ItemConfigData> * RandActivityHunQiFanLiConfig::GetPhaseRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);

	for (size_t i = 0; i < m_phase_reward_cfg.size(); i++)
	{
		if (m_phase_reward_cfg[i].IsInclude(act_real_open_day))
		{
			if (seq < 0 || seq >= (int)m_phase_reward_cfg[i].reward_limit_cfg.size()) return NULL;
			return &m_phase_reward_cfg[i].reward_limit_cfg[seq];
		}
	}

	return NULL;
}

const std::vector<std::pair<int, int> >* RandActivityHunQiFanLiConfig::GetChongZhiCfg(RandActivityManager * ramgr) const
{
	if (NULL == ramgr) return NULL;
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);

	for (size_t i = 0; i < m_chong_zhi_cfg.size(); i++)
	{
		if (m_chong_zhi_cfg[i].IsInclude(act_real_open_day))
		{
			return &m_chong_zhi_cfg[i].chong_zhi_cfg;
		}
	}

	return NULL;
}

int RandActivityHunQiFanLiConfig::InitBasisCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_basic_section_cfg.size() > 0)
		{
			int pre_index = m_basic_section_cfg.size() - 1;
			if (section_start != m_basic_section_cfg[pre_index].section_start ||
				section_end != m_basic_section_cfg[pre_index].section_end)
			{
				if (section_start < m_basic_section_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_basic_section_cfg.push_back(RAHunQiFanLiBasicSectionCfg());
			}
		}

		if (m_basic_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_basic_section_cfg.push_back(RAHunQiFanLiBasicSectionCfg());
		}

		RAHunQiFanLiBasicSectionCfg & node_cfg = m_basic_section_cfg[m_basic_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAHunQiFanLiBasicCfg baisc_cfg;

		if (!GetSubNodeValue(data_element, "rate", baisc_cfg.rate) || baisc_cfg.rate < 0) return -1;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward1_item");
		if (NULL == reward_element) return -101;

		if (!baisc_cfg.reward.ReadConfig(reward_element)) return -2;

		node_cfg.basic_cfg.push_back(baisc_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHunQiFanLiConfig::InitPhaseRewardCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_phase_reward_cfg.size() > 0)
		{
			int pre_index = m_phase_reward_cfg.size() - 1;
			if (section_start != m_phase_reward_cfg[pre_index].section_start ||
				section_end != m_phase_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_phase_reward_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_phase_reward_cfg.push_back(RAHunQiFanLiPhaseSectionCfg());
			}
		}

		if (m_phase_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_phase_reward_cfg.push_back(RAHunQiFanLiPhaseSectionCfg());
		}

		RAHunQiFanLiPhaseSectionCfg & node_cfg = m_phase_reward_cfg[m_phase_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		std::pair<int, ItemConfigData> reward_cfg;

		if (!GetSubNodeValue(data_element, "number", reward_cfg.first) || reward_cfg.first < 0) return -1;

		TiXmlElement* reward_element = data_element->FirstChildElement("reward1_item");
		if (NULL == reward_element) return -101;

		if (!reward_cfg.second.ReadConfig(reward_element)) return -2;

		node_cfg.reward_limit_cfg.push_back(reward_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityHunQiFanLiConfig::InitChongZhiCfg(TiXmlElement * RootElement, std::string & err)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
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

		if (m_chong_zhi_cfg.size() > 0)
		{
			int pre_index = m_chong_zhi_cfg.size() - 1;
			if (section_start != m_chong_zhi_cfg[pre_index].section_start ||
				section_end != m_chong_zhi_cfg[pre_index].section_end)
			{
				if (section_start < m_chong_zhi_cfg[pre_index].section_end)
				{
					return -100;
				}
				m_chong_zhi_cfg.push_back(RAHunQiFanLiChongZhiSectionCfg());
			}
		}

		if (m_chong_zhi_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -200;
			}
			m_chong_zhi_cfg.push_back(RAHunQiFanLiChongZhiSectionCfg());
		}

		RAHunQiFanLiChongZhiSectionCfg & node_cfg = m_chong_zhi_cfg[m_chong_zhi_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		std::pair<int, int> chongzhi_cfg;

		if (!GetSubNodeValue(data_element, "price", chongzhi_cfg.first) || chongzhi_cfg.first <= 0) return -1;
		if (!GetSubNodeValue(data_element, "number", chongzhi_cfg.second) || chongzhi_cfg.second < 0) return -2;

		if (!node_cfg.chong_zhi_cfg.empty())
		{
			if (node_cfg.chong_zhi_cfg[node_cfg.chong_zhi_cfg.size() - 1].first >= chongzhi_cfg.first)
			{
				err = STRING_SPRINTF("front price[%d] >= behind price[%d]", node_cfg.chong_zhi_cfg[node_cfg.chong_zhi_cfg.size() - 1].first, chongzhi_cfg.first);
				return -102;
			}
		}

		node_cfg.chong_zhi_cfg.push_back(chongzhi_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
