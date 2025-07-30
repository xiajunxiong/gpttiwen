#include "randactivitymazesummerconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include <set>

RandActivityMazeSummerConfig::RandActivityMazeSummerConfig()
{
}

RandActivityMazeSummerConfig::~RandActivityMazeSummerConfig()
{
}

bool RandActivityMazeSummerConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("maze_summer", InitMazeSummerCfg);
	LOAD_RA_CONFIG("event_group", InitEventGroupCfg);
	LOAD_RA_CONFIG("maze_reward_group", InitMazeRewardGroupCfg);
	LOAD_RA_CONFIG("maze_shop_group", InitMazeShopGroupCfg);
	LOAD_RA_CONFIG("maze_task", InitMazeTaskCfg);

	return true;
}

const RAMazeSummerMapListCfg * RandActivityMazeSummerConfig::GetMazeSummerMapListCfg(int open_day) const
{
	for (std::map<long long, RAMazeSummerMapListCfg>::const_iterator it = m_map_info.begin(); it != m_map_info.end(); it++)
	{
		if (it->second.section_start <= open_day && it->second.section_end >= open_day)
		{
			return &it->second;
		}
	}
	return NULL;
}

const RAMazeSummerMapCfg * RandActivityMazeSummerConfig::GetMazeSummerMapCfgByDayAndGrid(int open_day, int grid) const
{
	for (std::map<long long, RAMazeSummerMapListCfg>::const_iterator it = m_map_info.begin(); it != m_map_info.end(); it++)
	{
		if (it->second.section_start <= open_day && it->second.section_end >= open_day)
		{
			for (int i = 0; i < MAX_MAZE_SUMMER_POS_X;i++)
			{
				for (int j = 0; j < MAX_MAZE_SUMMER_POS_Y; j++)
				{
					if (it->second.map_info[i][j].grid == grid)
					{
						return &it->second.map_info[i][j];
					}
				}
			}
		}
	}
	return NULL;
}

const RAMazeSummerEventGroupCfg * RandActivityMazeSummerConfig::GetMazeSummerEventInfoCfg(int event_group, bool has_shop/* = false*/) const
{
	std::map<int, RAMazeSummerEventGroupListCfg>::const_iterator it = m_event_group.find(event_group);
	if (it != m_event_group.end())
	{
		int weight = 0;
		std::vector<const RAMazeSummerEventGroupCfg*> new_vec;
		for (std::vector<RAMazeSummerEventGroupCfg>::const_iterator new_it = it->second.m_list.begin(); new_it != it->second.m_list.end(); new_it++)
		{
			if (has_shop)
			{
				if (new_it->event_type == RA_MAZE_SUMMER_MAP_TYPE_SHOP)
				{
					continue;
				}
			}
			new_vec.push_back(&(*new_it));
			weight += new_it->rate;
		}

		if (weight <= 0)weight = 1;
		int rand_num = rand() % weight;
		
		for (std::vector<const RAMazeSummerEventGroupCfg*>::const_iterator itor = new_vec.begin(); itor != new_vec.end(); itor++)
		{
			if ((*itor)->rate >= rand_num)
			{
				return &(*(*itor));
			}
			rand_num -= (*itor)->rate;
		}
	}
	return NULL;
}

const RAMazeSummerShopGroupCfg * RandActivityMazeSummerConfig::GetMazeSummerShopGroupCfg(int shop_group) const
{
	std::map<int, RAMazeSummerShopGroupCfg>::const_iterator it = m_shop_group.find(shop_group);
	if (it != m_shop_group.end())
	{
		return &it->second;
	}
	return NULL;
}

const RAMazeSummerTaskListCfg * RandActivityMazeSummerConfig::GetMazeSummerTaskListCfg(int open_day) const
{
	for (std::map<long long, RAMazeSummerTaskListCfg>::const_iterator it = m_task_map.begin(); it != m_task_map.end(); it++)
	{
		if (it->second.section_start <= open_day && it->second.section_end >= open_day)
		{
			return &it->second;
		}
	}
	return NULL;
}

const RAMazeSummerRewardGroupCfg * RandActivityMazeSummerConfig::GetMazeSummerRewardCfg(int reward_group) const
{
	std::map<int, RAMazeSummerRewardGroupListCfg>::const_iterator it = m_reward_group.find(reward_group);
	if (it != m_reward_group.end())
	{
		int rand_num = rand() % it->second.weight;
		for (std::vector<RAMazeSummerRewardGroupCfg>::const_iterator itor = it->second.m_list.begin(); itor != it->second.m_list.end(); itor++)
		{
			if (itor->rate >= rand_num)
			{
				return &(*itor);
			}
			rand_num -= itor->rate;
		}
	}
	return NULL;
}

const RAMazeSummerTaskCfg * RandActivityMazeSummerConfig::GetMazeSummerTaskCfgByType(int type) const
{
	std::map<int, RAMazeSummerTaskCfg>::const_iterator it = m_task_type_map.find(type);
	if (it != m_task_type_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const RAMazeSummerShopCfg * RandActivityMazeSummerConfig::GetShopItemDetailCfg(int shop_id, int seq) const
{
	std::map<int, RAMazeSummerShopGroupCfg>::const_iterator it = m_shop_group.find(shop_id);
	if (it != m_shop_group.end())
	{
		for (std::vector<RAMazeSummerShopCfg>::const_iterator itor = it->second.m_reward_vec.begin(); itor != it->second.m_reward_vec.end(); itor++)
		{
			if (itor->shop_type == shop_id && itor->index == seq)
			{
				return &(*itor);
			}
		}
	}
	return NULL;
}

int RandActivityMazeSummerConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL != data_element)
	{
		if (!GetSubNodeValue(data_element, "consume_item", m_other_cfg.consume_item) || m_other_cfg.consume_item < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "first_num", m_other_cfg.first_num) || m_other_cfg.first_num < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "sec_num", m_other_cfg.sec_num) || m_other_cfg.sec_num < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "third_num", m_other_cfg.third_num) || m_other_cfg.third_num < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "four_more_num", m_other_cfg.four_more_num) || m_other_cfg.four_more_num < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "shop_item_num_down", m_other_cfg.shop_item_num_down) || m_other_cfg.shop_item_num_down < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "shop_item_num_up", m_other_cfg.shop_item_num_up) || m_other_cfg.shop_item_num_up < 0)
		{
			return -7;
		}
	}

	return 0;
}

int RandActivityMazeSummerConfig::InitMazeSummerCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start < 0)
		{
			return -1;
		}
		int section_end = 0;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0)
		{
			return -2;
		}

		if (section_end <= 0)
		{
			section_end = MAX_INT;
		}
		long long key = this->GetKeyBy(section_start, section_end);
		RAMazeSummerMapListCfg & node_list = m_map_info[key];
		node_list.section_start = section_start;
		node_list.section_end = section_end;

		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0)
		{
			return -4;
		}
		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0)
		{
			return -5;
		}
		int grid = 0;
		if (!GetSubNodeValue(data_element, "grid", grid) || grid < 0 || grid >= MAX_MAZE_SUMMER_POS_X * MAX_MAZE_SUMMER_POS_Y)
		{
			return -6;
		}

		int pos_x = grid % MAX_MAZE_SUMMER_POS_X;
		int pos_y = grid / MAX_MAZE_SUMMER_POS_X;
		if (pos_x < 0 || pos_x >= MAX_MAZE_SUMMER_POS_X || pos_y < 0 || pos_y >= MAX_MAZE_SUMMER_POS_Y)
		{
			return -7;
		}
		RAMazeSummerMapCfg &node = node_list.map_info[pos_x][pos_y];
		node.type = type;
		node.seq = seq;
		node.grid = grid;

		int event_group = 0;
		if (!GetSubNodeValue(data_element, "event_group", node.event_group) || node.event_group < 0)
		{
			return -7;
		}

		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", node.reward_group) || node.reward_group < 0)
		{
			return -8;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMazeSummerConfig::InitEventGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int event_group = 0;
		if (!GetSubNodeValue(data_element, "event_group", event_group) || event_group < 0)
		{
			return -1;
		}
		RAMazeSummerEventGroupListCfg &node_list = m_event_group[event_group];
		RAMazeSummerEventGroupCfg node;
		node.event_group = event_group;

		if (!GetSubNodeValue(data_element, "event_type", node.event_type) || node.event_type < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "param_1", node.param_1) || node.param_1 < 0 || node.param_1 >= MAX_UINT16)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "rate", node.rate) || node.rate < 0)
		{
			return -4;
		}

		node_list.weight += node.rate;
		node_list.m_list.push_back(node);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMazeSummerConfig::InitMazeRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", reward_group) || reward_group < 0)
		{
			return -1;
		}
		RAMazeSummerRewardGroupListCfg &node_list = m_reward_group[reward_group];
		RAMazeSummerRewardGroupCfg node;
		node.reward_group = reward_group;

		if (!GetSubNodeValue(data_element, "exp", node.exp) || node.exp < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "coin", node.coin) || node.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "rate", node.rate) || node.rate < 0)
		{
			return -4;
		}

		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != reward_item_list_element)
		{
			TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward_item");
			while (NULL != reward_item_element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(reward_item_element))
				{
					return -5;
				}
				node.m_reward_vec.push_back(reward_node);
				reward_item_element = reward_item_element->NextSiblingElement();
			}
		}

		node_list.weight += node.rate;
		node_list.m_list.push_back(node);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMazeSummerConfig::InitMazeShopGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int shop_type = 0;
		if (!GetSubNodeValue(data_element, "shop_type", shop_type) || shop_type < 0)
		{
			return -1;
		}
		RAMazeSummerShopGroupCfg &node_list = m_shop_group[shop_type];
		RAMazeSummerShopCfg node;
		node.shop_type = shop_type;

		if (!GetSubNodeValue(data_element, "index", node.index) || node.index < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "buy_times", node.buy_times) || node.buy_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "money_type", node.money_type) || (node.money_type != MONEY_TYPE_GOLD && node.money_type != MONEY_TYPE_COIN_BIND))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "price", node.price) || node.price < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "rate", node.rate) || node.rate < 0)
		{
			return -6;
		}

		TiXmlElement * reward_item_element = data_element->FirstChildElement("reward_item");
		if (NULL != reward_item_element)
		{
			if (!node.shop_info.ReadConfig(reward_item_element))
			{
				return -5;
			}
		}

		node_list.weight += node.rate;
		node_list.m_reward_vec.push_back(node);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMazeSummerConfig::InitMazeTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (data_element != NULL)
	{
		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start < 0)
		{
			return -1;
		}
		int section_end = 0;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0)
		{
			return -2;
		}

		if (section_end <= 0)
		{
			section_end = MAX_INT;
		}
		long long key = this->GetKeyBy(section_start, section_end);
		RAMazeSummerTaskListCfg & node_list = m_task_map[key];
		node_list.section_start = section_start;
		node_list.section_end = section_end;

		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0 || type >= MAX_INT16)
		{
			return -4;
		}
		int task_type = 0;
		if (!GetSubNodeValue(data_element, "task_type", task_type) || task_type < RAMAZE_SUMMER_TASK_TYPE_0 || task_type >= RAMAZE_SUMMER_TASK_TYPE_MAX)
		{
			return -5;
		}
		int task = 0;
		if (!GetSubNodeValue(data_element, "task", task) || task < RAMAZE_SUMMER_TASK_TYPE_0 || task >= RAMAZE_SUMMER_TASK_TYPE_MAX)
		{
			return -6;
		}

		RAMazeSummerTaskCfg node;
		node.type = type;
		node.task_type = task_type;
		node.task = task;

		if (!GetSubNodeValue(data_element, "param_1", node.param_1) || node.param_1 < 0)
		{
			return -7;
		}

		if (!GetSubNodeValue(data_element, "param_2", node.param_2) || node.param_2 < 0)
		{
			return -8;
		}

		TiXmlElement * reward_item_element = data_element->FirstChildElement("reward_item");
		if (NULL != reward_item_element)
		{
			if (!node.reward.ReadConfig(reward_item_element))
			{
				return -5;
			}
		}

		node_list.m_task_list.push_back(node);
		RAMazeSummerTaskCfg &node_cfg = m_task_type_map[type];
		node_cfg = node;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityMazeSummerConfig::GetKeyBy(int section_start, int section_end)
{
	KeyAndSection key_section;
	key_section.section[SECTION_TYPE_START] = section_start;
	key_section.section[SECTION_TYPE_END] = section_end;
	return key_section.key;
}

void RandActivityMazeSummerConfig::GetSectionByKey(long long key, int * section_start, int * section_end)
{
	KeyAndSection key_section;
	key_section.key = key;
	if (section_start != NULL)*section_start = key_section.section[SECTION_TYPE_START];
	if (section_end != NULL)*section_end = key_section.section[SECTION_TYPE_END];
}
