#include "randactivitymibaofangdianconfig.hpp"

#include "servercommon/roleactivity/ranationaldayparam.hpp"
#include "world.h"
#include "global/randactivity/randactivitymanager.hpp"

RandActivityMiBaoFangDianConfig::RandActivityMiBaoFangDianConfig()
{
}

RandActivityMiBaoFangDianConfig::~RandActivityMiBaoFangDianConfig()
{
	std::vector<RAMiBaoFangDianSeverSectionShopCfg>().swap(m_shop_section_cfg);
}

bool RandActivityMiBaoFangDianConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("mibaofangdian", InitShopCfg);

	return true;
}

const RAMiBaoFangDianShopCfg * RandActivityMiBaoFangDianConfig::GetShopCfg(RandActivityManager * ramgr, int seq, int cur_use_open_day) const
{
	if (NULL == ramgr || seq < 0 || seq >= MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM) return NULL;

	//int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	int act_real_open_day = cur_use_open_day;
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day && m_shop_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				const RAMiBaoFangDianSectionShopCfg & node_cfg = m_shop_section_cfg[i].section_list[k];
				if (node_cfg.start_time <= act_open_day && node_cfg.end_time >= act_open_day && seq < (int)node_cfg.shop_list.size())
				{
					return &node_cfg.shop_list[seq];
				}
			}
		}
	}
	return NULL;
}

void RandActivityMiBaoFangDianConfig::GetPhase(RandActivityManager * ramgr, int & use_open_day, int & cur_phase)const
{
	if (NULL == ramgr) return;

	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	int use_activity_open_day = act_real_open_day;
	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_start <= act_real_open_day &&  act_real_open_day <= m_shop_section_cfg[i].section_end)
		{
			for (int k = 0; k < (int)m_shop_section_cfg[i].section_list.size(); k++)
			{
				const RAMiBaoFangDianSectionShopCfg & node_cfg = m_shop_section_cfg[i].section_list[k];
				if (node_cfg.start_time <= act_open_day && node_cfg.end_time >= act_open_day)
				{
					int phase_act_open_day = act_open_day - node_cfg.start_time;
					time_t today_zero_timestamp = GetZeroTime(EngineAdapter::Instance().Time());
					time_t phase_start_zero_timestamp  = today_zero_timestamp -  phase_act_open_day * SECOND_PER_DAY;
					use_activity_open_day = this->GetOpenDayByTimestamp(ramgr, phase_start_zero_timestamp);		//计算出当前阶段开始时的开服天数
					cur_phase = node_cfg.phase;
					break;	
				}
			}
		}
	}

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	if (activity_begin_time < open_server_timestamp)	//活动开启在开服前
	{
		use_activity_open_day = GetMin(use_activity_open_day, this->GetCurRealOpenDay(ramgr));	
	}

	if (use_activity_open_day <= 0)
	{
		use_activity_open_day = 1;
	}

	use_open_day = use_activity_open_day;
	return;
}

int RandActivityMiBaoFangDianConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityMiBaoFangDianConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver_1", m_other_cfg.cfg_ver) || m_other_cfg.cfg_ver <= 0) return -1;

	return 0;
}

int RandActivityMiBaoFangDianConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int seq = 0;
	int last_phase = -1;
	int phase = 0;
	bool is_first = false;
	while (NULL != data_element)
	{
		bool has_server_section_cfg = true;
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_server_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_server_section_cfg = false;
		}

		if (!has_server_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_shop_section_cfg.size() > 0)
		{
			int pre_index = m_shop_section_cfg.size() - 1;
			if (section_start != m_shop_section_cfg[pre_index].section_start ||
				section_end != m_shop_section_cfg[pre_index].section_end)
			{
				if (section_start < m_shop_section_cfg[pre_index].section_end)
				{
					return -777;
				}

				m_shop_section_cfg.push_back(RAMiBaoFangDianSeverSectionShopCfg());
				last_phase = -1;
			}
		}
		if (m_shop_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_shop_section_cfg.push_back(RAMiBaoFangDianSeverSectionShopCfg());
			is_first = true;
		}
		RAMiBaoFangDianSeverSectionShopCfg & server_node_cfg = m_shop_section_cfg[m_shop_section_cfg.size() - 1];
		server_node_cfg.section_start = section_start;
		server_node_cfg.section_end = section_end;

		int start_time = 0, end_time = 0;
		bool has_section_cfg = true;
		if (!GetSubNodeValue(data_element, "start_time", start_time))
		{
			has_section_cfg = false;
		}
		if (!GetSubNodeValue(data_element, "end_time", end_time))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			start_time = 1;
			end_time = INT_MAX;
		}

		if (0 == end_time) // 0 代表无穷
		{
			end_time = INT_MAX;
		}

		if (server_node_cfg.section_list.size() > 0)
		{
			int pre_index = server_node_cfg.section_list.size() - 1;
			if (start_time != server_node_cfg.section_list[pre_index].start_time ||
				end_time != server_node_cfg.section_list[pre_index].end_time)
			{
				if (start_time < server_node_cfg.section_list[pre_index].end_time)
				{
					return -666;
				}

				server_node_cfg.section_list.push_back(RAMiBaoFangDianSectionShopCfg());
				seq = 0;
				last_phase = phase;
			}
		}
		if (server_node_cfg.section_list.size() == 0)
		{
			if (start_time != 1)
			{
				return -555;
			}
			server_node_cfg.section_list.push_back(RAMiBaoFangDianSectionShopCfg());
			seq = 0;
		}

		RAMiBaoFangDianSectionShopCfg & node_cfg = server_node_cfg.section_list[server_node_cfg.section_list.size() - 1];
		node_cfg.start_time = start_time;
		node_cfg.end_time = end_time;
		long long phase_time_key = this->GetKeyBySection(node_cfg.start_time, node_cfg.end_time);

		if (!GetSubNodeValue(data_element, "phase", node_cfg.phase) || node_cfg.phase <= 0) return -22;
		phase = node_cfg.phase;
		if (-1 != last_phase)
		{
			if (phase != last_phase + 1)
			{
				return -33;
			}
		}
		else
		{
			if (phase != 1)		//阶段固定从1开始
			{
				return -44;
			}
		}
		RAMiBaoFangDianShopCfg shop_cfg;

		if (!GetSubNodeValue(data_element, "index", shop_cfg.index) || shop_cfg.index < 0 || shop_cfg.index >= MAX_RA_MI_BAO_FANG_DIAN_SHOP_ITEM_NUM) return -1;
		if (!GetSubNodeValue(data_element, "seq", shop_cfg.seq) || shop_cfg.seq < 0 || shop_cfg.seq >= MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM) return -2;
		if (!GetSubNodeValue(data_element, "limit_type", shop_cfg.limit_type)) return -3;
		if (!GetSubNodeValue(data_element, "buy_times", shop_cfg.buy_times)) return -4;
		if (!GetSubNodeValue(data_element, "today_times", shop_cfg.today_times)) return -5;

		switch (shop_cfg.limit_type)
		{
		case ACTVITY_BUY_LIMIT_TYPE_ALL:
		{
			if (shop_cfg.buy_times <= 0) return -6;
		}
		break;
		case ACTVITY_BUY_LIMIT_TYPE_DAY:
		{
			if (shop_cfg.today_times <= 0) return -7;
		}
		break;
		default:
			return -8;
		}

		if (!GetSubNodeValue(data_element, "price_type", shop_cfg.price_type) || shop_cfg.price_type < RAMiBaoFangDianShopCfg::PRICE_TYPE_MIN ||
			shop_cfg.price_type >= RAMiBaoFangDianShopCfg::PRICE_TYPE_MAX) return -9;
		if (!GetSubNodeValue(data_element, "price", shop_cfg.price) || shop_cfg.price <= 0) return -10;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)
		{
			return -100;
		}

		if (!shop_cfg.reward_item.ReadConfig(reward_element))
		{
			return -101;
		}

		if (seq++ != shop_cfg.seq)
		{
			return -66;
		}
		long long section_key =  this->GetKeyBySection(server_node_cfg.section_start, server_node_cfg.section_end);
		if (is_first)
		{
			m_shop_phase_map[section_key].insert(phase_time_key);
		}
		else
		{
			if(m_shop_phase_map.empty()) return -400;

			std::map<long long, std::set<long long> >::const_iterator it = m_shop_phase_map.find(section_key);
			if (it != m_shop_phase_map.end())
			{
				m_shop_phase_map[section_key].insert(phase_time_key);
			}
			else
			{
				it = m_shop_phase_map.begin();
				if(it->second.empty()) return -401;

				bool is_has = false;
				for (std::set<long long>::const_iterator phase_time_it = it->second.begin(); phase_time_it != it->second.end(); ++phase_time_it)
				{
					if (phase_time_key == *phase_time_it)
					{
						is_has = true;
					}
				}
				if (!is_has)
				{
					return -402;
				}
			}
		}
		is_first = false;

		node_cfg.shop_list.push_back(shop_cfg);
		data_element = data_element->NextSiblingElement();
	}

	if(m_shop_phase_map.empty()) return -610;

	int count = m_shop_phase_map.begin()->second.size();

	for (int i = 0; i < (int)m_shop_section_cfg.size(); i++)
	{
		if (m_shop_section_cfg[i].section_list.size() != count)
		{
			return -611;
		}
	}
	
	return 0;
}

long long RandActivityMiBaoFangDianConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivityMiBaoFangDianConfig::GetSectionByKey(long long key, int & section_start, int & section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	section_start = un_key.section[0];
	section_end = un_key.section[1];
}
