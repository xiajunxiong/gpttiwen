#include "activesystemconfig.hpp"

#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "protocol/msgrolejoinactivityreward.hpp"

ActiveSystemConfig::ActiveSystemConfig()
{
}

ActiveSystemConfig::~ActiveSystemConfig()
{
	std::vector<ActiveSystemLevelGroupCfg>().swap(m_active_level_group_cfg);
	std::vector<TurntableTaskCfg>().swap(m_turntable_week_task_cfg);
}

bool ActiveSystemConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("active", InitActiveSystemCfg);
	LOAD_CONFIG("active_reward", InitActiveLevelGroupCfg);
	LOAD_CONFIG("linkage_act", InitActiveRelatedCfg);

	LOAD_CONFIG("week_activity", InitWeekActiveDegreeTaskCfg);
	LOAD_CONFIG("week_activity_reward", InitWeekActiveDegreeRewardfg);

	LOAD_CONFIG("turntable_task", InitTurntableTaskCfg);
	LOAD_CONFIG("active_turntable", InitTurntableLevelCfg);
	LOAD_CONFIG("turntable_reward_group", InitTurntableRewardCfg);
	LOAD_CONFIG("one_click_list", InitOneClickListCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const ActiveSystemCfg * ActiveSystemConfig::GetActiveSystemCfg(int type, int level)
{
	if (type <= ACTIVE_SYSTEM_TYPE_BEGIN || type >= ACTIVE_SYSTEM_TYPE_MAX) return NULL;

	ActiveSystemListCfg &node = m_active_system_cfg[type];
	for (std::vector<ActiveSystemCfg>::iterator it = node.m_reward_vec.begin(); it != node.m_reward_vec.end(); it++)
	{
		if (it->reward_cfg.level_min <= level && it->reward_cfg.level_max >= level)
		{
			return &(*it);
		}
	}

	return NULL;
}

const ActiveSystemSpecialCfg * ActiveSystemConfig::GetSpecialActiveCfg(int type, int level)
{
	if (type <= ACTIVE_SYSTEM_TYPE_BEGIN || type >= ACTIVE_SYSTEM_TYPE_MAX) return NULL;

	ActiveSystemSpecialListCfg &node = m_special_active_cfg[type];
	for (std::vector<ActiveSystemSpecialCfg>::iterator it = node.m_reward_vec.begin(); it != node.m_reward_vec.end(); it++)
	{
		if (it->reward_cfg.level_min <= level && it->reward_cfg.level_max >= level)
		{
			return &(*it);
		}
	}

	return NULL;
}

const ActiveSystemRewardCfg * ActiveSystemConfig::GetActiveSystemRewardCfg(int level, int index)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL || index < 0 || index >= ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM) return NULL;

	for (std::vector<ActiveSystemLevelGroupCfg>::const_iterator it = m_active_level_group_cfg.begin(); it != m_active_level_group_cfg.end(); it++)
	{
		if(it->level_min <= level && it->level_max >= level)
			return &it->node_cfg[index];
	}
	return NULL;
}

void ActiveSystemConfig::GetActiveSystemRelatedCfgNum(RandActivityManager* ramgr, int seq, int count, const ItemConfigData * reward_num, std::vector<ItemConfigData> * reward_list)
{
	if(NULL == reward_list) return;
	for (int i = 0; i < count; i++)
	{
		(*reward_list).push_back(reward_num[i]);
	}
	if(seq <= 0 || seq > ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM) return;

	for (std::map<int, ActiveSystemRelatedActCfg>::iterator it = m_active_related_cfg.begin(); it != m_active_related_cfg.end(); it++)
	{
		if (NULL == ramgr || !ramgr->IsRandActivityOpen(it->first)) continue;

		if(it->second.node_cfg[seq - 1].count <= 0) continue;

		for (int i = 0; i < it->second.node_cfg[seq - 1].count && i < ACTIVE_SYSTEM_REWARD_MAX_NUM; i++)
		{
			(*reward_list).push_back(it->second.node_cfg[seq - 1].reward[i]);
		}
	}

	return;
}

const WeekActiveDegreeTaskCfg * ActiveSystemConfig::GetWeekActiveDegreeTaskCfg(int task_type) const
{
	if (ACTIVE_DEGREE_TYPE_BEGIN >= task_type
		|| ACTIVE_DEGREE_TYPE_MAX <= task_type)
	{
		return NULL;
	}
	if (ACTIVE_DEGREE_TYPE_BEGIN == m_week_degree_task_cfg[task_type].task_type)
	{
		return NULL;
	}
	return &m_week_degree_task_cfg[task_type];
}

const WeekActiveDegreeRewardCfg * ActiveSystemConfig::GetWeekActiveDegreeRewardCfg(int role_level, int reward_seq) const
{
	if (reward_seq <= 0 ||
		reward_seq > WEEKDAY_COUNT ||
		reward_seq > MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM ||
		role_level <= 0 || role_level > MAX_ROLE_LEVEL)
		return NULL;

	for (size_t i = 0; i < m_week_degree_reward_level_cfg.size(); i++)
	{
		if (role_level <= m_week_degree_reward_level_cfg[i].level_max)
		{
			return &m_week_degree_reward_level_cfg[i].week_degree_reward_cfg[reward_seq - 1];
		}
	}

	return NULL;
}

const TurntableTaskCfg * ActiveSystemConfig::GetTurntableActivityCfg(int activity_type)
{
	std::map<int, TurntableTaskCfg>::iterator it = m_turntable_day_task_cfg.find(activity_type);
	if (it == m_turntable_day_task_cfg.end())
		return NULL;

	return &it->second;
}

const TurntableRewardCfg * ActiveSystemConfig::GetTurntableRewardCfg(int role_leve)
{
	std::map<short, short>::iterator level_it = m_turntable_level_cfg.upper_bound(role_leve);
	if (level_it == m_turntable_level_cfg.begin())
		return NULL;

	level_it--;

	std::map<short, std::map<int, TurntableRewardCfg> >::iterator group_it = m_turntable_reward_cfg.find(level_it->second);
	if (group_it == m_turntable_reward_cfg.end())
		return NULL;

	int rate = RandomNum(10000);
	std::map<int, TurntableRewardCfg>::iterator reward_it = group_it->second.upper_bound(rate);
	if (reward_it == group_it->second.end())
		return NULL;

	return &reward_it->second;
}

int ActiveSystemConfig::GetOneClickConsumeGold(int activie_type) const
{
	std::map<int, int>::const_iterator it = m_one_click_list_cfg.find(activie_type);
	if (it != m_one_click_list_cfg.end()) 
		return it->second;

	return -1;
}

int ActiveSystemConfig::InitActiveSystemCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int type = 0, need_count = 0, is_special = 0;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -1;
		if (!GetSubNodeValue(root_element, "need_count", need_count) || need_count < 0) return -2;
		if (!GetSubNodeValue(root_element, "is_special", is_special) || is_special < 0) return -3;

		if (0 != is_special)
		{
			if (need_count == 0)
			{
				return -50;
			}
			if (type <= ACTIVITY_SYSTEM_SPECIAL_TYPE_BEGIN || type >= ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX)
			{
				return -51;
			}
			ActiveSystemSpecialListCfg & list_cfg = m_special_active_cfg[type];
			ActiveSystemSpecialCfg node_cfg;
			node_cfg.special_type = type;
			node_cfg.need_count = need_count;
			int ret = this->InitActiveCommonCfg(root_element, node_cfg.reward_cfg);
			if (0 != ret)
			{
				return ret;
			}

			list_cfg.m_reward_vec.push_back(node_cfg);
		}
		else
		{
			//type与need_count都为0,则是客户端展示类型,不会给予奖励
			if (0 == need_count)
			{
				if (ACTIVE_SYSTEM_TYPE_BEGIN != type)
				{
					return -60;
				}
			}
			else
			{
				if (type <= ACTIVE_SYSTEM_TYPE_BEGIN || type >= ACTIVE_SYSTEM_TYPE_MAX)
				{
					return -61;
				}

				ActiveSystemListCfg & list_cfg = m_active_system_cfg[type];
				ActiveSystemCfg node_cfg;
				node_cfg.type = type;
				node_cfg.need_count = need_count;

				int ret = this->InitActiveCommonCfg(root_element, node_cfg.reward_cfg);
				if (0 != ret)
				{
					return ret;
				}

				if (!GetSubNodeValue(root_element, "integral_active", node_cfg.integral_active) || node_cfg.integral_active < 0) return -81;
				if (!GetSubNodeValue(root_element, "is_every_time", node_cfg.is_every_time) || node_cfg.is_every_time < 0) return -82;

				list_cfg.m_reward_vec.push_back(node_cfg);
			}
		}
	
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActiveSystemConfig::InitActiveLevelGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "level_min", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "level_max", max_level) || max_level <= min_level || max_level > MAX_ROLE_LEVEL) return -2;

		if (m_active_level_group_cfg.size() > 0)
		{
			if (m_active_level_group_cfg[m_active_level_group_cfg.size() - 1].level_min != min_level &&
				m_active_level_group_cfg[m_active_level_group_cfg.size() - 1].level_max != max_level)
				m_active_level_group_cfg.push_back(ActiveSystemLevelGroupCfg());
		}
		if (m_active_level_group_cfg.size() == 0)
		{
			m_active_level_group_cfg.push_back(ActiveSystemLevelGroupCfg());
		}
		
		ActiveSystemLevelGroupCfg & cfg = m_active_level_group_cfg[m_active_level_group_cfg.size() - 1];
		cfg.level_min = min_level;
		cfg.level_max = max_level;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0 || seq > ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM) return -3;

		ActiveSystemRewardCfg &node_cfg = cfg.node_cfg[seq - 1];
		node_cfg.seq = seq;

		if (!GetSubNodeValue(root_element, "integral_active", node_cfg.integral_active) || node_cfg.integral_active <= 0 ) return -4;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin)) return -5;
		if (!GetSubNodeValue(root_element, "gold", node_cfg.gold)) return -6;
		if (!GetSubNodeValue(root_element, "power", node_cfg.power) || node_cfg.power < 0) return -7;
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0) return -8;

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;
				if (node_cfg.count >= ACTIVE_SYSTEM_REWARD_MAX_NUM) return -101;

				node_cfg.reward[node_cfg.count++] = item;
				item_node = item_node->NextSiblingElement();
			}
		}
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActiveSystemConfig::InitActiveRelatedCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		ActiveSystemRelatedActCfg cfg;
		static int act_type = 0;
		if (!GetSubNodeValue(root_element, "act_type", act_type) || act_type <= RAND_ACTIVITY_TYPE_BEGIN || act_type >= RAND_ACTIVITY_TYPE_END) return -1;
		std::map<int, ActiveSystemRelatedActCfg>::iterator it= m_active_related_cfg.find(act_type);
		if (it == m_active_related_cfg.end())
		{
			m_active_related_cfg[act_type] = cfg;
		}

		ActiveSystemRelatedCfg node_cfg;

		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq <= 0 || node_cfg.seq > ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM) return -1;
		
		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;
				if (node_cfg.count > ACTIVE_SYSTEM_REWARD_MAX_NUM) return -101;

				node_cfg.reward[node_cfg.count++] = item;
				item_node = item_node->NextSiblingElement();
			}
		}
		m_active_related_cfg[act_type].node_cfg[node_cfg.seq - 1] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActiveSystemConfig::InitActiveCommonCfg(TiXmlElement * root_element, ActiveSystemCommonCfg & node_cfg)
{
	if (NULL == root_element)
	{
		return -1000;
	}

	if (!GetSubNodeValue(root_element, "coin", node_cfg.reward_coin) || node_cfg.reward_coin < 0) return -21;
	if (!GetSubNodeValue(root_element, "strength", node_cfg.reward_energy) || node_cfg.reward_energy < 0) return -22;
	if (!GetSubNodeValue(root_element, "level_min", node_cfg.level_min) || node_cfg.level_min < 0) return -23;
	if (!GetSubNodeValue(root_element, "level_max", node_cfg.level_max) || node_cfg.level_max < 0) return -24;
	if (!GetSubNodeValue(root_element, "exp", node_cfg.reward_exp) || node_cfg.reward_exp < 0) return -25;
	if (!GetSubNodeValue(root_element, "open_day", node_cfg.open_day) || node_cfg.open_day < 0) return -26;

	TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
	if (NULL != list_node)
	{
		TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -100;
			if (node_cfg.reward_num >= ARRAY_ITEM_COUNT(node_cfg.reward_list)) return -101;

			node_cfg.reward_list[node_cfg.reward_num++] = item;
			item_node = item_node->NextSiblingElement();
		}
	}

	return 0;
}

int ActiveSystemConfig::InitWeekActiveDegreeTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int task_type = ACTIVE_DEGREE_TYPE_BEGIN;
		if (!GetSubNodeValue(root_element, "type", task_type)
			|| ACTIVE_DEGREE_TYPE_BEGIN >= task_type
			|| ACTIVE_DEGREE_TYPE_MAX <= task_type)
		{
			return -1;
		}
		WeekActiveDegreeTaskCfg &task_info = m_week_degree_task_cfg[task_type];
		if (ACTIVE_DEGREE_TYPE_BEGIN != task_info.task_type)
		{
			return -2;
		}
		else
		{
			task_info.task_type = task_type;
		}

		if (!GetSubNodeValue(root_element, "need_count", task_info.task_param))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "integral_active", task_info.week_active_degree)
			|| 0 >= task_info.week_active_degree)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActiveSystemConfig::InitWeekActiveDegreeRewardfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int level_max = 0;
		if (!GetSubNodeValue(root_element, "level_max", level_max) || level_max <= 0 || level_max > MAX_ROLE_LEVEL) return -102;

		if (m_week_degree_reward_level_cfg.empty())
		{
			m_week_degree_reward_level_cfg.push_back(WeekActiveDegreeRewardLevelCfg());
		}
		else
		{
			if (m_week_degree_reward_level_cfg[m_week_degree_reward_level_cfg.size() - 1].level_max > level_max)
			{
				return -103;
			}

			if (m_week_degree_reward_level_cfg[m_week_degree_reward_level_cfg.size() - 1].level_max < level_max)
			{
				m_week_degree_reward_level_cfg.push_back(WeekActiveDegreeRewardLevelCfg());
			}
		}

		WeekActiveDegreeRewardLevelCfg& cfg = m_week_degree_reward_level_cfg[m_week_degree_reward_level_cfg.size() - 1];
		cfg.level_max = level_max;

		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0 || seq > WEEKDAY_COUNT || seq > MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM)
		{
			return -1;
		}

		WeekActiveDegreeRewardCfg &cfg_info = cfg.week_degree_reward_cfg[seq - 1];
		cfg_info.reward_index = seq - 1;

		if (!GetSubNodeValue(root_element, "integral_active", cfg_info.need_degree)
			|| 0 > cfg_info.need_degree)
		{
			return -2;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL == list_node)
		{
			return -100;
		}

		TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node))
			{
				return -101;
			}
			cfg_info.reward_item_vec.push_back(item);

			item_node = item_node->NextSiblingElement();
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActiveSystemConfig::InitTurntableTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TurntableTaskCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -1;

		if (!GetSubNodeValue(root_element, "turntable_num", cfg.turntable_num) || cfg.turntable_num < 0)
			return -2;

		int task_type = 0;
		if (!GetSubNodeValue(root_element, "task_type", task_type) || task_type <= 0 || task_type > ACTIVE_TURNTABLE_TYPE_MAX)
			return -3;

		if (!GetSubNodeValue(root_element, "type_param", cfg.param) || cfg.param < 0)
			return -4;

		switch (task_type)
		{
		case ACTIVE_TURNTABLE_TYPE_JOIN_ACTIVITY:
			{
				m_turntable_day_task_cfg[cfg.param] = cfg;
			}
			break;
		case ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE:
			{
				m_turntable_week_task_cfg.push_back(cfg);
			}
			break;
		default:
			return -1000;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActiveSystemConfig::InitTurntableLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		short min_level = 0;
		if (!GetSubNodeValue(root_element, "min_level_turn", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL)
			return -1;

		short reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || reward_group < 0)
			return -2;

		m_turntable_level_cfg[min_level] = reward_group;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActiveSystemConfig::InitTurntableRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_group = 1, last_rate = 0;
	while (NULL != root_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || reward_group <= 0)
			return -1;

		if (reward_group != last_group)
		{
			if (last_rate != 10000)
				return -1000;

			last_group = reward_group;
			last_rate = 0;
		}

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0 || rate > 10000)
			return -2;

		last_rate += rate;
		if (last_rate > 10000)
			return -1001;

		TurntableRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
			return -3;

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL == item_element)
			return -1002;

		if (!cfg.item.ReadConfig(item_element))
			return -5;

		m_turntable_reward_cfg[reward_group][last_rate] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActiveSystemConfig::InitOneClickListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -544;

	while (root_element)
	{
		std::pair<int, int> cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.first) || cfg.first < 0 || cfg.first >= Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_MAX) return -1;
		if (!GetSubNodeValue(root_element, "cost_gold", cfg.second) || cfg.second < 0) return -2;

		m_one_click_list_cfg.insert(cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActiveSystemConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -544;

	if (!GetSubNodeValue(root_element, "one_click_level", m_other_cfg.one_click_level) || m_other_cfg.one_click_level < 0 || m_other_cfg.one_click_level > MAX_ROLE_LEVEL) return -1;
	if (!GetSubNodeValue(root_element, "one_click_open_time", m_other_cfg.one_click_open_time) || m_other_cfg.one_click_open_time < 0) return -2;

	return 0;
}
