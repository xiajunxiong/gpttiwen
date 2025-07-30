#include "rachongjifanliconfig.hpp"
#include "servercommon/roleactivity/rachongjifanliparam.hpp"

RandActivityChongJiFanLiConfig::RandActivityChongJiFanLiConfig()
{
}

RandActivityChongJiFanLiConfig::~RandActivityChongJiFanLiConfig()
{
	std::vector<RAChongJiFanLiSectionRewardCfg>().swap(m_section_reward_cfg);
}

bool RandActivityChongJiFanLiConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG_2("reward", InitRewardCfg);

	return true;
}

const RAChongJiFanLiRewardCfg * RandActivityChongJiFanLiConfig::GetRewardCfg(RandActivityManager * ramgr, int seq) const
{
	if (NULL == ramgr || seq < 0 || seq >= MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI);
	for (int i = 0; i < (int)m_section_reward_cfg.size(); ++i)
	{
		if (m_section_reward_cfg[i].section_start <= act_real_open_day && m_section_reward_cfg[i].section_end >= act_real_open_day)
		{
			const RAChongJiFanLiSectionRewardCfg & node_list = m_section_reward_cfg[i];
			if (seq < (int)node_list.reward_list.size())
			{
				return &node_list.reward_list[seq];
			}
		}
	}

	return NULL;
}

const RAChongJiFanLiRewardCfg * RandActivityChongJiFanLiConfig::GetRewardCfgByDay(RandActivityManager * ramgr, int day) const
{
	if (NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI);
	for (int i = 0; i < (int)m_section_reward_cfg.size(); ++i)
	{
		if (m_section_reward_cfg[i].section_start <= act_real_open_day && m_section_reward_cfg[i].section_end >= act_real_open_day)
		{
			const RAChongJiFanLiSectionRewardCfg & node_list = m_section_reward_cfg[i];
			for (std::vector<RAChongJiFanLiRewardCfg>::const_reverse_iterator it = node_list.reward_list.rbegin(); it != node_list.reward_list.rend(); ++it)
			{
				if (day >= it->day)
				{
					return &(*it);
				}
			}
		}
	}

	return NULL;
}

const RAChongJiFanLiSectionRewardCfg * RandActivityChongJiFanLiConfig::GetRewardSectionCfg(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return NULL;

	int act_real_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI);
	for (int i = 0; i < (int)m_section_reward_cfg.size(); ++i)
	{
		if (m_section_reward_cfg[i].section_start <= act_real_open_day && m_section_reward_cfg[i].section_end >= act_real_open_day)
		{
			return &m_section_reward_cfg[i];
		}
	}

	return NULL;
}

int RandActivityChongJiFanLiConfig::InitRewardCfg(TiXmlElement * RootElement, std::string error_msg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int last_day = 0;
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

		if (m_section_reward_cfg.size() > 0)
		{
			int pre_index = m_section_reward_cfg.size() - 1;
			if (section_start != m_section_reward_cfg[pre_index].section_start ||
				section_end != m_section_reward_cfg[pre_index].section_end)
			{
				if (section_start < m_section_reward_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_section_reward_cfg.push_back(RAChongJiFanLiSectionRewardCfg());
				last_day = 0;
			}
		}
		if (m_section_reward_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_reward_cfg.push_back(RAChongJiFanLiSectionRewardCfg());
			last_day = 0;
		}

		RAChongJiFanLiSectionRewardCfg & node_cfg = m_section_reward_cfg[m_section_reward_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != (int)node_cfg.reward_list.size() || seq >= MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM) 
		{
			error_msg = STRING_SPRINTF("section[start:%d, end:%d], seq[%d] != %d or >= %d", section_start, section_end, seq, 
				(int)node_cfg.reward_list.size(), MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM);
			return -1;
		}
		RAChongJiFanLiRewardCfg reward_cfg;
		if (!GetSubNodeValue(data_element, "day", reward_cfg.day) || reward_cfg.day <= 0 || last_day >= reward_cfg.day) 
		{
			error_msg = STRING_SPRINTF("section[start:%d, end:%d], day[%d] <= 0", section_start, section_end, reward_cfg.day);
			return -2;
		}
		last_day = reward_cfg.day;
		TiXmlElement * reward2_item_list_element = data_element->FirstChildElement("reward2_item_list");
		if (NULL != reward2_item_list_element) 
		{
			TiXmlElement * reward2_element = reward2_item_list_element->FirstChildElement("reward2_item");
			if (NULL != reward2_element)
			{
				while (NULL != reward2_element)
				{
					ItemConfigData item;
					if (!item.ReadConfig(reward2_element))
					{
						error_msg = STRING_SPRINTF("section[start:%d, end:%d] seq[%d], [reward2_item_list] has item invaild[item_id:%d]", section_start, section_end,
							seq, item.item_id);
						return -111;
					}

					reward_cfg.common_reward_list.push_back(item);
					reward2_element = reward2_element->NextSiblingElement();
				}
			}
		}

		TiXmlElement * reward1_element = data_element->FirstChildElement("reward1_item");
		if (NULL != reward1_element)
		{
			int item_id = 0;
			if (!GetSubNodeValue(reward1_element, "item_id", item_id))
			{
				error_msg = STRING_SPRINTF("section[start:%d, end:%d] seq[%d], [reward1_item] not find[item_id]", section_start, section_end, seq);
				return -120;
			}
			if (item_id > 0)
			{
				if (!reward_cfg.grand_prize.ReadConfig(reward1_element))
				{
					error_msg = STRING_SPRINTF("section[start:%d, end:%d] seq[%d], [reward1_item] item_id[%d]", section_start, section_end, seq, item_id);
					return -121;
				}
			}
			else
			{
				if (reward_cfg.common_reward_list.empty())
				{
					error_msg = STRING_SPRINTF("section[start:%d, end:%d] seq[%d], at least one reward", section_start, section_end, seq, item_id);
					return -122;
				}
			}
		}

		if (!GetSubNodeValue(data_element, "price", reward_cfg.need_chong_num) || reward_cfg.need_chong_num <= 0)
		{
			error_msg = STRING_SPRINTF("section[start:%d, end:%d] seq[%d], price[%d] <= 0", section_start, section_end, seq, reward_cfg.need_chong_num);
			return -3;
		}

		node_cfg.reward_list.push_back(reward_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
