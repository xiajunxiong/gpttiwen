#include "schooltaskconfig.hpp"
#include "gameworld/checkresourcecenter.hpp"
#include "servercommon/schooltaskdef.h"
#include "gameworld/checkresourcecenter.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/prestigedef.hpp"

SchoolTaskConfig::SchoolTaskConfig()
{
}

SchoolTaskConfig::~SchoolTaskConfig()
{
	for (std::map<int, std::vector<SchoolTaskCfg> >::iterator it = m_task_by_level.begin(); it != m_task_by_level.end(); it++)
	{
		std::vector<SchoolTaskCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<SchoolTaskRewardCfg> >::iterator it = m_reward_map.begin(); it != m_reward_map.end(); it++)
	{
		std::vector<SchoolTaskRewardCfg>().swap(it->second);
	}
}

bool SchoolTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_CONFIG("reward_find", InitRewardFindCfg);
	LOAD_CONFIG("task", InitTaskCfg);
	LOAD_CONFIG("mission_reward", InitLevelRewardCfg);

	return true;
}

const SchoolTaskCfg * SchoolTaskConfig::GetTaskCfg(int task_id)
{
	std::map<int, SchoolTaskCfg>::const_iterator it = m_task_map.find(task_id);
	if (it == m_task_map.end()) return NULL;
	return &it->second;
}

bool SchoolTaskConfig::GetTaskList(int level, int * task_list)
{
	std::vector<SchoolTaskCfg> ret_list;

	// 取出能接的所以任务
	for (std::map<int, std::vector<SchoolTaskCfg> >::const_iterator it = m_task_by_level.begin(), end = m_task_by_level.end(); it != end; ++it)
	{
		int min_level = it->first / 1000;
		int max_level = it->first % 1000;
		if (level <= max_level && level >= min_level)
		{
			ret_list.insert(ret_list.begin(), it->second.begin(), it->second.end());
		}
	}
	if (ret_list.size() <= 0) return false;

	// 随机一下
	std::random_shuffle(ret_list.begin(), ret_list.end());

	// 取出几个
	int task_count = 0;
	for (std::vector<SchoolTaskCfg>::const_iterator it = ret_list.begin(), end = ret_list.end(); it != end; ++it)
	{
		task_list[task_count++] = it->task_id;
		if (task_count >= m_other_cfg.daily_num) break;
	}

	return true;
}

void SchoolTaskConfig::RandTaskListForServer(int *task_list)
{
	UNSTD_STATIC_CHECK(MAX_SCHOOL_TASK_NUM > 0);
	DynamicArray<int, MAX_SCHOOL_TASK_NUM> all_task_list;

	// 取出能接的所以任务
	std::map<int, SchoolTaskCfg>::iterator it = m_task_map.begin();
	for (; it != m_task_map.end(); ++it)
	{
		all_task_list.PushBack(it->second.task_id);
	}
	if (all_task_list.Count() <= 0) return;

	// 计算下一天的零点的时间，用这个时间作为随机数种子
	unsigned int seed = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	seed = seed + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	seed = seed % 13131;

	// 随机一下
	RandomShuffle(&all_task_list[0], 0, all_task_list.Count(), seed);

	// 取出几个
	int task_count = 0;
	for (int i = 0; i < all_task_list.Count() && i < SCHOOL_TASK_NUM; ++i)
	{
		task_list[task_count++] = all_task_list[i];
		if (task_count >= m_other_cfg.daily_num) break;
	}
}

static int rand_num;
bool FindIf(SchoolTaskRewardCfg cfg)
{
	return rand_num < cfg.rate;
}

const ItemConfigData * SchoolTaskConfig::RandItemReward(int group)
{
	std::map<int, std::vector<SchoolTaskRewardCfg> >::const_iterator group_it = m_reward_map.find(group);
	if (group_it == m_reward_map.end()) return NULL;
	rand_num = RandomNum(RAND_UNIVERSAL_RATIO_BASE_NUM);
	std::vector<SchoolTaskRewardCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindIf);
	if (it == group_it->second.end()) return NULL;
	return &it->reward;
}

const SchoolTaskLevelRewardCfg* SchoolTaskConfig::GetLevelReward(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	return &m_level_reward_cfg_list[level];
}

bool SchoolTaskConfig::GetFindReward(int level, int type, int * coin, int * exp, std::vector<ItemConfigData> * item)
{
	for (std::map<int, std::map<int, SchoolTaskrewardFindCfg> >::const_iterator level_it = m_reward_find_map.begin(), level_end = m_reward_find_map.end();
		 level_end != level_it; ++level_it)
	{
		int min_level = level_it->first / 1000;
		int max_level = level_it->first % 1000;

		if (level >= min_level && level <= max_level)
		{
			std::map<int, SchoolTaskrewardFindCfg>::const_iterator it = level_it->second.find(type);
			if (it == level_it->second.end())
			{
				return false;
			}
			else
			{
				if (NULL != coin) *coin = it->second.coin;
				if (NULL != exp) *exp = it->second.exp;
				if (NULL != item)
				{
					for (int i = 0; i < it->second.reward_group_cnt; ++i)
					{
						ItemConfigData temp_item;
						const ItemConfigData * reward = LOGIC_CONFIG->GetSchoolTaskConfig()->RandItemReward(it->second.reward_group[i]);
						if (NULL != reward)	
						{
							temp_item = *reward;
							(*item).push_back(temp_item);
						}
					}
				}
				return true;
			}
		}
	}

	return false;
}

int SchoolTaskConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "daily_num", m_other_cfg.daily_num) || m_other_cfg.daily_num >= SCHOOL_TASK_NUM) return -1;
		if (!GetSubNodeValue(root_element, "quick_finish", m_other_cfg.quick_finish)) return -2;
		if (!GetSubNodeValue(root_element, "reward_power", m_other_cfg.power_reward) || m_other_cfg.power_reward < 0) return -4;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int SchoolTaskConfig::InitTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		static SchoolTaskCfg node_cfg;
		if (!GetSubNodeValue(root_element, "task_id", node_cfg.task_id)) return -1;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < 0 || node_cfg.max_level > MAX_ROLE_LEVEL) return -3;
		if (!GetSubNodeValue(root_element, "condition", node_cfg.condition)) return -4;
		if (!GetSubNodeValue(root_element, "c_param1", node_cfg.c_param1)) return -5;
		if (!GetSubNodeValue(root_element, "c_param2", node_cfg.c_param2)) return -6;
		if (!GetSubNodeValue(root_element, "target_scene", node_cfg.target_scene)) return -7;
		if (!GetSubNodeValue(root_element, "reputation_city", node_cfg.reputation_city) || node_cfg.reputation_city < 0 || node_cfg.reputation_city >= PRESTIGE_VILLAGE_MAX_NUM) return -8;
		if (!GetSubNodeValue(root_element, "reputation_num", node_cfg.reputation_num) || node_cfg.reputation_num < 0) return -9;

		m_task_map[node_cfg.task_id] = node_cfg;
		int level_key = node_cfg.min_level * 1000 + node_cfg.max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_task_by_level[level_key].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SchoolTaskConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	std::map<int, int > group_rate;

	while (NULL != root_element)
	{
		SchoolTaskRewardCfg node_cfg;
		int group_id;
		if (!GetSubNodeValue(root_element, "group_id", group_id)) return -3;
		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate)) return -2;
		group_rate[group_id] += rate;
		if (group_rate[group_id] > RAND_UNIVERSAL_RATIO_BASE_NUM) return -999;
		node_cfg.rate = group_rate[group_id];
		TiXmlElement *reward_element = root_element->FirstChildElement("reward");
		if (!node_cfg.reward.ReadConfig(reward_element)) return -1;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.reward.item_id, node_cfg.reward.item_id, node_cfg.reward.num, __FUNCTION__);

		m_reward_map[group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SchoolTaskConfig::InitRewardFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		SchoolTaskrewardFindCfg node_cfg;
		int find_type;
		if (!GetSubNodeValue(root_element, "find_type", find_type)) return -1;
		int min_level;
		int max_level;
		if (!GetSubNodeValue(root_element, "min_level", min_level)) return -2;
		if (!GetSubNodeValue(root_element, "max_level", max_level)) return -3;

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp)) return -4;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin)) return -5;

		TiXmlElement* reward_element = root_element->FirstChildElement("reward_group");
		if (NULL == reward_element)
		{
			return -6;
		}

		node_cfg.reward_group_cnt = 0;
		TiXmlElement* node_element = reward_element->FirstChildElement("node");
		while (NULL != node_element)
		{
			if (!GetNodeValue(node_element, node_cfg.reward_group[node_cfg.reward_group_cnt]))
			{
				break;
			}
			node_cfg.reward_group_cnt++;
			node_element = node_element->NextSiblingElement();
		}

		m_reward_find_map[min_level * 1000 + max_level][find_type] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int SchoolTaskConfig::InitLevelRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	int last_level = 0;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level != last_level + 1 || level > MAX_ROLE_LEVEL || level <= 0)
		{
			return -1;
		}
		last_level = level;

		SchoolTaskLevelRewardCfg& cfg = m_level_reward_cfg_list[level];
		cfg.level = level;

		if (!GetSubNodeValue(root_element, "exp", cfg.exp) || cfg.exp < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.coin) || cfg.coin < 0)
		{
			return -2;
		}

		TiXmlElement* reward_element = root_element->FirstChildElement("reward_group");
		if (NULL == reward_element)
		{
			return -3;
		}

		cfg.reward_group_cnt = 0;
		TiXmlElement* node_element = reward_element->FirstChildElement("node");
		while (NULL != node_element)
		{
			if (!GetNodeValue(node_element, cfg.reward_group_list[cfg.reward_group_cnt]))
			{
				break;
			}
			cfg.reward_group_cnt += 1;
			node_element = node_element->NextSiblingElement();
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
