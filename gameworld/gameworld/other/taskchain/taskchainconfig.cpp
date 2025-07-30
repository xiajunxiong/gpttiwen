#include "taskchainconfig.hpp"
#include "gameworld/checkresourcecenter.hpp"
#include "gameworld/item/itembase.h"
#include "gameworld/item/itempool.h"
#include "servercommon/struct/itemlistparam.h"

TaskChainConfig::TaskChainConfig()
{
}

TaskChainConfig::~TaskChainConfig()
{
	for (std::map<int, std::vector<TaskChainCfg> >::iterator it = m_task_chain_map.begin(); it != m_task_chain_map.end(); it++){
		std::vector<TaskChainCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainSceneCfg> >::iterator it = m_task_chain_scene_map.begin(); it != m_task_chain_scene_map.end(); it++) {
		std::vector<TaskChainSceneCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainMonsterCfg> >::iterator it = m_task_chain_monster_map.begin(); it != m_task_chain_monster_map.end(); it++) {
		std::vector<TaskChainMonsterCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainNpcCfg> >::iterator it = m_task_chain_npc_map.begin(); it != m_task_chain_npc_map.end(); it++) {
		std::vector<TaskChainNpcCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_task_chain_item_map.begin(); it != m_task_chain_item_map.end(); it++) {
		std::vector<TaskChainItemCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_task_chain_wait_item_map.begin(); it != m_task_chain_wait_item_map.end(); it++) {
		std::vector<TaskChainItemCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_reward_group_list.begin(); it != m_reward_group_list.end(); it++) {
		std::vector<TaskChainItemCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainItemDropSceneCfg> >::iterator it = m_task_chain_item_drop_scene_map.begin(); it != m_task_chain_item_drop_scene_map.end(); it++) {
		std::vector<TaskChainItemDropSceneCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<TaskChainPetCfg> >::iterator it = m_task_chain_pet_map.begin(); it != m_task_chain_pet_map.end(); it++) {
		std::vector<TaskChainPetCfg>().swap(it->second);
	}

	std::vector<TaskChainLevelTimesCfg>().swap(m_task_chain_level_times_cfg);
}

bool TaskChainConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("task_chain_config", InitTaskChainCfg);
	LOAD_CONFIG("scene_group", InitTaskChainSceneCfg);
	LOAD_CONFIG("coordinate_group", InitTaskChainScenePosiCfg);
	LOAD_CONFIG("monster_group", InitTaskChainMonsterCfg);
	LOAD_CONFIG("npc_group", InitTaskChainNpcCfg);
	LOAD_CONFIG("item_group", InitTaskChainItemCfg);
	LOAD_CONFIG("task_item_group", InitTaskChainWaitItemCfg);
	LOAD_CONFIG("exp_config", InitTaskChainExpCfg);
	LOAD_CONFIG("rewar_group", InitRewardGroupCfg);
	LOAD_CONFIG("reward_config", InitTaskRewardListCfg);
	LOAD_CONFIG("scene_drop", InitTaskItemDropSceneCfg);
	LOAD_CONFIG("pet_group", InitTaskChainPetCfg);
	LOAD_CONFIG("level_times", InitTaskChainLevelTimesCfg);
	LOAD_CONFIG("turn_table", InitTaskChainTurnTableCfg);

	return true;
}

static int temp_num;
bool FindType(const TaskChainCfg *cfg)
{
	bool ret = temp_num < cfg->rate;
	temp_num -= cfg->rate;
	return ret;
}

const TaskChainCfg * TaskChainConfig::RandTaskType(int level,int pre_task_type, short pre_wait_count, int cur_completion_times)const
{
	for (std::map<int, std::vector<TaskChainCfg> >::const_iterator level_it = m_task_chain_map.begin(), level_end = m_task_chain_map.end(); 
		 level_it != level_end; ++level_it)
	{
		int min_level = level_it->first / 1000;
		int max_level = level_it->first % 1000;
		if (level >= min_level && level <= max_level)
		{
			int level_key = level_it->first;
			std::map<int, int>::const_iterator rate_it = m_task_chain_total_rate.find(level_key);
			if (rate_it == m_task_chain_total_rate.end()) return NULL;
			//检测是否有上一次任务信息
			int rate = rate_it->second;
			std::vector<const TaskChainCfg*> new_task_vec;
			
			{
				for (std::vector<TaskChainCfg>::const_iterator rate_vec = level_it->second.begin();rate_vec != level_it->second.end(); rate_vec++)
				{
					if (pre_task_type == TASK_CHAIN_TYPE_FIGHT)
					{
						//如果前一个任务是打怪，那么剔除代送任务类型
						if (rate_vec->task_type == TASK_CHAIN_TYPE_WAIT)
						{
							rate -= rate_vec->rate;
							continue;
						}
					}
					//检查代送类型任务间隔，+2的理由：是因为需要间隔X环才能接到也就是X+1，但是服务端在接到这个任务的时候就标记成1了，少了0和1，这2次
					//每阶段前X环不能接取代送类型任务
					if (rate_vec->task_type == TASK_CHAIN_TYPE_WAIT)
					{
						if ((-1 != cur_completion_times && !this->IsCanAcceptWaitTask(cur_completion_times)) || (pre_wait_count != 0 && pre_wait_count <= m_other_cfg.space_times + 2))
						{
							rate -= rate_vec->rate;
							continue;
						}
					}
					

					if (rate_vec->task_type == pre_task_type) //减去前一个任务的权重不随机
					{
						rate -= rate_vec->rate;
					}
					else
					{
						new_task_vec.push_back(&(*rate_vec));//把过滤之后的任务配置重组
					}
				}
			}
			temp_num = RandomNum(rate);
			std::vector<const TaskChainCfg*>::const_iterator it = std::find_if(new_task_vec.begin(), new_task_vec.end(), FindType);
			if (it != new_task_vec.end())
			{
				return *it;
			}

			break;
		}
	}

	return NULL;
}

bool FindScene(TaskChainSceneCfg cfg)
{
	return temp_num < cfg.rate;
}

const TaskChainSceneCfg * TaskChainConfig::RandScene(int group_id) const
{
	std::map<int, std::vector<TaskChainSceneCfg> >::const_iterator group_it = m_task_chain_scene_map.find(group_id);
	if (group_it == m_task_chain_scene_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_scene_total_rate.find(group_id);
	if (rate_it == m_task_chain_scene_total_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainSceneCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindScene);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}

const Posi * TaskChainConfig::RandScenePosi(int group_id) const
{
	std::map<int, std::map<int, Posi> >::const_iterator group_it = m_task_chain_scene_pos_map.find(group_id);
	if (group_it == m_task_chain_scene_pos_map.end())
		return NULL;

	std::map<int, int>::const_iterator rate_it = m_task_chain_scene_pos_total_rate.find(group_id);
	if (rate_it == m_task_chain_scene_pos_total_rate.end())
		return NULL;

	temp_num = RandomNum(rate_it->second);

	std::map<int, Posi>::const_iterator it = group_it->second.upper_bound(temp_num);
	if (it == group_it->second.end())
		return NULL;

	return &it->second;
}

bool FindMonster(TaskChainMonsterCfg cfg)
{
	return temp_num < cfg.rate;
}

const TaskChainMonsterCfg * TaskChainConfig::RandMonster(int group_id) const
{
	std::map<int, std::vector<TaskChainMonsterCfg> >::const_iterator group_it = m_task_chain_monster_map.find(group_id);
	if (group_it == m_task_chain_monster_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_monster_total_rate.find(group_id);
	if (rate_it == m_task_chain_monster_total_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainMonsterCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindMonster);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}


bool FindNpc(const TaskChainNpcCfg *cfg)
{
	bool ret = temp_num < cfg->rate;
	temp_num -= cfg->rate;
	return ret;
}

const TaskChainNpcCfg * TaskChainConfig::RandNpc(int group_id, int pre_task_npc_id) const
{
	std::map<int, std::vector<TaskChainNpcCfg> >::const_iterator group_it = m_task_chain_npc_map.find(group_id);
	if (group_it == m_task_chain_npc_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_npc_total_rate.find(group_id);
	if (rate_it == m_task_chain_npc_total_rate.end()) return NULL;

	//检测是否有上一次任务信息
	int rate = rate_it->second;
	std::vector<const TaskChainNpcCfg*> new_npc_vec;
	
	{
		for (std::vector<TaskChainNpcCfg>::const_iterator rate_vec = group_it->second.begin(); rate_vec != group_it->second.end(); rate_vec++)
		{
			if (rate_vec->npc_id == pre_task_npc_id) //减去前一个任务的权重不随机
			{
				rate -= rate_vec->rate;
			}
			else
			{
				new_npc_vec.push_back(&(*rate_vec));//把过滤之后的任务配置重组
			}
		}
	}

	temp_num = RandomNum(rate);

	std::vector<const TaskChainNpcCfg*>::const_iterator it = std::find_if(new_npc_vec.begin(), new_npc_vec.end(), FindNpc);
	if (it == new_npc_vec.end()) return NULL;

	return *it;
}

bool FindItem(TaskChainItemCfg cfg)
{
	return temp_num < cfg.rate;
}

const TaskChainItemCfg * TaskChainConfig::RandItem(int group_id) const
{
	std::map<int, std::vector<TaskChainItemCfg> >::const_iterator group_it = m_task_chain_item_map.find(group_id);
	if (group_it == m_task_chain_item_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_item_total_rate.find(group_id);
	if (rate_it == m_task_chain_item_total_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainItemCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindItem);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}

const TaskChainItemCfg * TaskChainConfig::RandWaitItem(int group_id) const
{
	std::map<int, std::vector<TaskChainItemCfg> >::const_iterator group_it = m_task_chain_wait_item_map.find(group_id);
	if (group_it == m_task_chain_wait_item_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_wait_item_total_rate.find(group_id);
	if (rate_it == m_task_chain_wait_item_total_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainItemCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindItem);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}

bool FindReward(TaskChainItemCfg cfg)
{
	return temp_num < cfg.rate;
}
/*
const ItemConfigData * TaskChainConfig::RandReward(int level, int times) const
{
	std::map<int, std::map<int, std::vector<TaskChainItemCfg> > >::const_iterator level_it = m_task_chain_reward_map.find(level);
	if (level_it == m_task_chain_reward_map.end()) return NULL;

	std::map<int, std::vector<TaskChainItemCfg> >::const_iterator times_it = level_it->second.find(times);
	if (times_it == level_it->second.end()) return NULL;

	std::vector<TaskChainItemCfg>::const_iterator it = std::find_if(times_it->second.begin(), times_it->second.end(), FindReward);
	if (it == times_it->second.end()) return NULL;

	return &it->item;
}
*/
const TaskChainExpItemCfg * TaskChainConfig::GetExp(int level, int times) const
{
	for (std::map<int, std::map<int, TaskChainExpItemCfg> >::const_iterator level_it = m_task_chain_exp_map.begin(), level_end = m_task_chain_exp_map.end();
		 level_it != level_end; ++level_it)
	{
		if (level <= level_it->first)
		{
			for (std::map<int, TaskChainExpItemCfg>::const_iterator it = level_it->second.begin(), end = level_it->second.end(); end != it; ++it)
			{
				if (times <= it->first)
				{
					return &it->second;
				}
			}

			break;
		}
	}

	return NULL;
}

bool FindItemDropScene(TaskChainItemDropSceneCfg cfg)
{
	return temp_num < cfg.rate;
}

const TaskChainItemDropSceneCfg * TaskChainConfig::RandItemDropScene(int group_id) const
{
	std::map<int, std::vector<TaskChainItemDropSceneCfg> >::const_iterator group_it = m_task_chain_item_drop_scene_map.find(group_id);
	if (group_it == m_task_chain_item_drop_scene_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_item_drop_scene_rate.find(group_id);
	if (rate_it == m_task_chain_item_drop_scene_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainItemDropSceneCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindItemDropScene);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}

bool FindPet(TaskChainPetCfg cfg)
{
	return temp_num < cfg.rate;
}

const TaskChainPetCfg * TaskChainConfig::RandPet(int group_id) const
{
	std::map<int, std::vector<TaskChainPetCfg> >::const_iterator group_it = m_task_chain_pet_map.find(group_id);
	if (group_it == m_task_chain_pet_map.end()) return NULL;
	std::map<int, int>::const_iterator rate_it = m_task_chain_pet_rate.find(group_id);
	if (rate_it == m_task_chain_pet_rate.end()) return NULL;

	temp_num = RandomNum(rate_it->second);

	std::vector<TaskChainPetCfg>::const_iterator it = std::find_if(group_it->second.begin(), group_it->second.end(), FindPet);
	if (it == group_it->second.end()) return NULL;

	return &(*it);
}

bool TaskChainConfig::GetTaskReward(int role_level, int finish_times, std::vector<ItemConfigData> & out_list)
{
	out_list.clear();
	bool ret = false;
	for (std::map<ParamSection, std::map<int, TaskChainLoopRewardCfg> >::iterator level_it = m_reward_list.begin(); level_it != m_reward_list.end(); ++level_it)
	{
		if (role_level < level_it->first.section_min || role_level > level_it->first.section_max) continue;

		std::map<int, TaskChainLoopRewardCfg >::iterator times_it = level_it->second.find(finish_times);
		if (level_it->second.end() == times_it) continue;

		ret = (0 != times_it->second.is_remind) ? true : false;
		for (std::vector<int>::iterator group_it = times_it->second.reward_group.begin(); group_it != times_it->second.reward_group.end(); ++group_it)
		{
			std::map<int, std::vector<TaskChainItemCfg> >::iterator reward_group_it = m_reward_group_list.find(*group_it);
			if (m_reward_group_list.end() == reward_group_it) continue;

			int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			for (std::vector<TaskChainItemCfg>::iterator item_it = reward_group_it->second.begin(); item_it != reward_group_it->second.end(); ++item_it)
			{
				if (rand_prob < item_it->rate)
				{
					out_list.push_back(item_it->item);
					break;
				}

				rand_prob -= item_it->rate;
			}
		}
	}
	return ret;
}

const TaskChainCfg* TaskChainConfig::GetTaskByTaskID(int level, int task_type)
{
	for (std::map<int, std::vector<TaskChainCfg> >::const_iterator level_it = m_task_chain_map.begin(), level_end = m_task_chain_map.end();
		level_it != level_end; ++level_it)
	{
		int min_level = level_it->first / 1000;
		int max_level = level_it->first % 1000;
		if (level >= min_level && level <= max_level)
		{
			for (std::vector<TaskChainCfg>::const_iterator rate_vec = level_it->second.begin(); rate_vec != level_it->second.end(); rate_vec++)
			{
				if (rate_vec->task_type == task_type)
				{
					return &(*rate_vec);
				}
			}
		}
	}

	return NULL;
}

int TaskChainConfig::GetTurnTableRewardGroup(int completion_times, int level)
{
// 	std::map<int, std::map<int, int> >::iterator times_it = m_task_chain_turn_table_cfg.find(completion_times);
// 	if(m_task_chain_turn_table_cfg.end() == times_it) return 0;
// 
// 	for (std::map<int, int>::iterator it = times_it->second.begin(); it != times_it->second.end(); it++)
// 	{	
// 		if (it->first / 1000 <= level && it->first % 1000 >= level)
// 		{
// 			return it->second;
// 		}
// 	}
	return 0;
}

int TaskChainConfig::GetRankTurnTableReward(int group_id)
{
	std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_reward_group_list.find(group_id);
	if (it != m_reward_group_list.end())
	{
		int rand_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (int i = 0 ; i < (int)it->second.size(); ++i)
		{
			if (rand_num < it->second[i].rate)
			{
				return i;
			}

			rand_num -= it->second[i].rate;
		}
	}
	return -1;
}

const ItemConfigData * TaskChainConfig::GetTurnTableReward(int completion_times, int level)
{
// 	if(index < 0) return NULL;
// 
// 	std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_reward_group_list.find(group_id);
// 	if (it != m_reward_group_list.end())
// 	{
// 		if(index >= (int)it->second.size()) return NULL;
// 
// 		return &it->second[index].item;
// 	}

	std::map<int, std::map<int, ItemConfigData> >::iterator times_it = m_task_chain_turn_table_cfg.find(completion_times);
	if (m_task_chain_turn_table_cfg.end() == times_it) return NULL;

	for (std::map<int, ItemConfigData>::iterator it = times_it->second.begin(); it != times_it->second.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
		{
			return &it->second;
		}
	}

	return NULL;
}

int TaskChainConfig::GetMaxCompletionTimesByLevel(int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return 0;

	for (int i = 0; i < (int)m_task_chain_level_times_cfg.size(); i++)
	{
		if (level >= m_task_chain_level_times_cfg[i].min_level && level <= m_task_chain_level_times_cfg[i].max_level)
		{
			return m_task_chain_level_times_cfg[i].completion_times;
		}
	}

	return 0;
}

bool TaskChainConfig::HasCompletionTimes(int completion_times)
{
	std::set<int>::iterator it = m_task_chain_level_set.find(completion_times);
	if(it != m_task_chain_level_set.end()) return true;

	return false;
}

int TaskChainConfig::GetNextCompletionTimes(int completion_times, int max_completion_times_by_level)
{
	int next_completion_times = completion_times;

	for (std::set<int>::iterator it = m_task_chain_level_set.begin(); it != m_task_chain_level_set.end(); it++)
	{
		if (max_completion_times_by_level >= *it && completion_times < *it)
		{
			next_completion_times = *it;
			break;
		}
	}

	return next_completion_times;
}

TaskChainLevelTimes TaskChainConfig::GetStageByCompetionTimes(int completion_times) const
{
	TaskChainLevelTimes info;
	for (int i = 0; i < (int)m_task_chain_level_times_cfg.size(); i++)
	{
		if (completion_times >= m_task_chain_level_times_cfg[i].completion_times)
		{
			info.index = i;
			info.completion_times = m_task_chain_level_times_cfg[i].completion_times;
		}
	}

	return info;
}

const TaskChainLevelTimesCfg * TaskChainConfig::GetCompletionTimesByIndex(int index)
{
	if(index < 0 || index >= (int)m_task_chain_level_times_cfg.size()) return NULL;

	return &m_task_chain_level_times_cfg[index];
}

int TaskChainConfig::GetMaxCompletionTimes()
{
	int completion_times = 0;

	for (std::set<int>::iterator it = m_task_chain_level_set.begin(); it != m_task_chain_level_set.end(); it++)
	{
		completion_times = GetMax(completion_times, *it);
	}

	return completion_times;
}

bool TaskChainConfig::IsCanAcceptWaitTask(int cur_completion_times)const
{
	TaskChainLevelTimes info = this->GetStageByCompetionTimes(cur_completion_times);
	if (cur_completion_times < info.completion_times + m_other_cfg.space_times)
	{
		return false;
	}

	return true;
}

int TaskChainConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "level", m_other_cfg.level))return -1;
		if (!GetSubNodeValue(root_element, "weekly_num", m_other_cfg.max_times))return -2;
		if (!GetSubNodeValue(root_element, "space_times", m_other_cfg.space_times))return -3;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int min_level;
		int max_level;
		if (!GetSubNodeValue(root_element, "min_level", min_level))return -1;
		if (!GetSubNodeValue(root_element, "max_level", max_level))return -2;
			
		TaskChainCfg node_cfg;
		if (!GetSubNodeValue(root_element, "task_type", node_cfg.task_type))return -3;
		if (!GetSubNodeValue(root_element, "param_1", node_cfg.param1))return -4;
		if (!GetSubNodeValue(root_element, "param_2", node_cfg.param2))return -5;
		if (!GetSubNodeValue(root_element, "param_3", node_cfg.param3))return -6;
		if (!GetSubNodeValue(root_element, "param_4", node_cfg.param4))return -7;
		if (!GetSubNodeValue(root_element, "time_limit", node_cfg.time_limit))return -8;
		int rate ;
		if (!GetSubNodeValue(root_element, "rate", rate))return -9;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin))return -10;
		int can_skip = 0;
		if (!GetSubNodeValue(root_element, "can_skip", can_skip))return -11;
		node_cfg.can_skip = (can_skip == 1);
		if (!GetSubNodeValue(root_element, "reward_double", node_cfg.reward_double))return -11;
		if (!GetSubNodeValue(root_element, "price_type", node_cfg.price_type) || node_cfg.price_type < 0 || node_cfg.price_type > 9)return -12;
		if (!GetSubNodeValue(root_element, "price", node_cfg.price))return -13;
		m_task_chain_total_rate[min_level * 1000 + max_level] += rate;
		node_cfg.rate = rate;

		m_task_chain_map[min_level * 1000 + max_level].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int scene_group_id;
		if (!GetSubNodeValue(root_element, "scene_group_id", scene_group_id))return -2;

		TaskChainSceneCfg node_cfg;
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id))return -3;
		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -4;

		if (!GetSubNodeValue(root_element, "pos_group_id", node_cfg.pos_group_id)) return -7;
		if (!GetSubNodeValue(root_element, "reputation_city", node_cfg.reputation_city))return -5;
		if (!GetSubNodeValue(root_element, "reputation_num", node_cfg.reputation_num))return -6;

		m_task_chain_scene_total_rate[scene_group_id] += rate;
		node_cfg.rate = m_task_chain_scene_total_rate[scene_group_id];

		m_task_chain_scene_map[scene_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainScenePosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
			return -1;

		Posi pos;
		if (!GetSubNodeValue(root_element, "x", pos.x))
			return -2;
		if (!GetSubNodeValue(root_element, "y", pos.y))
			return -3;

		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))
			return -4;

		rate += m_task_chain_scene_pos_total_rate[group_id];

		m_task_chain_scene_pos_total_rate[group_id] = rate;
		m_task_chain_scene_pos_map[group_id][rate] = pos;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int monster_group_id;
		if (!GetSubNodeValue(root_element, "monster_group_id", monster_group_id))return -2;

		TaskChainMonsterCfg node_cfg;
		if (!GetSubNodeValue(root_element, "monster_id", node_cfg.monster_id))return -3;
		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -6;
		m_task_chain_monster_total_rate[monster_group_id] += rate;
		node_cfg.rate = m_task_chain_monster_total_rate[monster_group_id];

		m_task_chain_monster_map[monster_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainNpcCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int npc_group_id;
		if (!GetSubNodeValue(root_element, "npc_group_id", npc_group_id))return -2;

		TaskChainNpcCfg node_cfg;
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id))return -3;
		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -4;

		if (!GetSubNodeValue(root_element, "reputation_city", node_cfg.reputation_city))return -5;
		if (!GetSubNodeValue(root_element, "reputation_num", node_cfg.reputation_num))return -6;
		m_task_chain_npc_total_rate[npc_group_id] += rate;
		node_cfg.rate = rate;

		m_task_chain_npc_map[npc_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int item_group_id;
		if (!GetSubNodeValue(root_element, "item_group_id", item_group_id))return -2;

		TaskChainItemCfg node_cfg;
		if (!GetSubNodeValue(root_element, "is_specialway", node_cfg.is_specialway) || node_cfg.is_specialway < 0 || node_cfg.is_specialway > 1)
		{
			return -3;
		}
		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		if(!node_cfg.item.ReadConfigNoCheck(item_element))return -3;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.item.item_id, node_cfg.item.item_id, node_cfg.item.num, __FUNCTION__);
		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -6;
		m_task_chain_item_total_rate[item_group_id] += rate;
		node_cfg.rate = m_task_chain_item_total_rate[item_group_id];

		m_task_chain_item_map[item_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainWaitItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int item_group_id;
		if (!GetSubNodeValue(root_element, "task_item_group", item_group_id))return -2;

		TaskChainItemCfg node_cfg;
		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		if (!node_cfg.item.ReadConfigNoCheck(item_element))return -3;
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(node_cfg.item.item_id, node_cfg.item.item_id, node_cfg.item.num, __FUNCTION__);

		const ItemBase * item_base = ITEMPOOL->GetItem(node_cfg.item.item_id);
		if (item_base == NULL || item_base->GetItemColumn() != ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL)
		{
			return -4;
		}

		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -4;
		m_task_chain_wait_item_total_rate[item_group_id] += rate;
		node_cfg.rate = m_task_chain_wait_item_total_rate[item_group_id];

		m_task_chain_wait_item_map[item_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int last_level = 0;
	while (NULL != root_element)
	{
		int min_level;
		int max_level;
		if (!GetSubNodeValue(root_element, "min_level", min_level))return -1;
		if (!GetSubNodeValue(root_element, "max_level", max_level))return -2;
		if (last_level > max_level) return -99;
		last_level = max_level;

		int min_num;
		int max_num;
		if (!GetSubNodeValue(root_element, "min_num", min_num))return -3;
		if (!GetSubNodeValue(root_element, "max_num", max_num))return -4;

		int exp;
		if (!GetSubNodeValue(root_element, "exp", exp))return -5;
		int coin;
		if (!GetSubNodeValue(root_element, "coin", coin))return -6;
		int coin_rate;
		if (!GetSubNodeValue(root_element, "coin_rate", coin_rate))return -7;
		int item_rate;
		if (!GetSubNodeValue(root_element, "item_rate", item_rate))return -8;

		TaskChainExpItemCfg &node_cfg = m_task_chain_exp_map[max_level][max_num];
		node_cfg.min_level = min_level;
		node_cfg.max_level = max_level;
		node_cfg.min_num = min_num;
		node_cfg.max_num = max_num;
		node_cfg.exp = exp;
		node_cfg.coin = coin;
		node_cfg.coin_rate = coin_rate;
		node_cfg.item_rate = item_rate;

		TiXmlElement *item_element = root_element->FirstChildElement("reward");
		if (!node_cfg.item.ReadConfigNoCheck(item_element))return -9;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TaskChainConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int group_id = 0; TaskChainItemCfg tmp_con;
		if (!GetSubNodeValue(root_element, "reward_group", group_id) || group_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.rate) || tmp_con.rate <= 0 || tmp_con.rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL == item_node) return -2000;		// 如果这个位置没有道具，那将毫无意义；

		if (!tmp_con.item.ReadConfig(item_node))
		{
			return -4;
		}

		std::map<int, std::vector<TaskChainItemCfg> >::iterator group_it = m_reward_group_list.find(group_id);
		if (m_reward_group_list.end() != group_it)
		{
			group_it->second.push_back(tmp_con);
		}
		else
		{
			std::vector<TaskChainItemCfg> tmp_vec; tmp_vec.push_back(tmp_con);
			m_reward_group_list[group_id] = tmp_vec;
		}

		root_element = root_element->NextSiblingElement();
	}

	// 检查概率总和是否超过 10000
	for (std::map<int, std::vector<TaskChainItemCfg> >::iterator group_it = m_reward_group_list.begin(); group_it != m_reward_group_list.end(); ++group_it)
	{
		int rate_total = 0;
		for (std::vector<TaskChainItemCfg>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
		{
			rate_total += item_it->rate;
		}

		if (rate_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -5000;
	}
	
	return 0;
}

int TaskChainConfig::InitTaskRewardListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		ParamSection level_section; int finish_times = 0; TaskChainLoopRewardCfg node;
		if (!GetSubNodeValue(root_element, "min_level", level_section.section_min) || level_section.section_min <= 0 || level_section.section_min > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_section.section_max) || level_section.section_max < level_section.section_min || level_section.section_max > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "completion_times", finish_times) || finish_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "is_remind", node.is_remind) || node.is_remind < 0)
		{
			return -4;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_group");
		if (NULL == list_node) return -2000;

		TiXmlElement * data_node = list_node->FirstChildElement("node");
		while (NULL != data_node)
		{
			int data_reader = 0;
			if (!GetNodeValue(data_node, data_reader) || m_reward_group_list.end() == m_reward_group_list.find(data_reader))
			{
				return -8000;
			}

			node.reward_group.push_back(data_reader);
			data_node = data_node->NextSiblingElement();
		}

		m_reward_list[level_section][finish_times] = node;
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int TaskChainConfig::InitTaskItemDropSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int group;
		if (!GetSubNodeValue(root_element, "group", group))return -2;

		TaskChainItemDropSceneCfg node_cfg;
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id))return -3;

		if (!GetSubNodeValue(root_element, "item_rate", node_cfg.item_rate))return -4;

		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -5;
		m_task_chain_item_drop_scene_rate[group] += rate;
		node_cfg.rate = m_task_chain_item_drop_scene_rate[group];

		m_task_chain_item_drop_scene_map[group].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainPetCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int group;
		if (!GetSubNodeValue(root_element, "group", group))return -2;

		TaskChainPetCfg node_cfg;
		if (!GetSubNodeValue(root_element, "pet_id", node_cfg.pet_id))return -3;

		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id))return -4;

		int rate;
		if (!GetSubNodeValue(root_element, "rate", rate))return -5;
		m_task_chain_pet_rate[group] += rate;
		node_cfg.rate = m_task_chain_pet_rate[group];

		m_task_chain_pet_map[group].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainTurnTableCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int completion_times = 0;
		if (!GetSubNodeValue(root_element, "completion_times", completion_times) || m_task_chain_level_set.end() == m_task_chain_level_set.find(completion_times))return -1;

		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level_turn", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL)return -2;
		if (!GetSubNodeValue(root_element, "max_level_turn", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)return -3;

// 		int reward_group = 0;
// 		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || reward_group <= 0)return -4;
// 
// 		std::map<int, std::vector<TaskChainItemCfg> >::iterator it = m_reward_group_list.find(reward_group);
// 		if(it == m_reward_group_list.end()) return -5;

		TiXmlElement *item_element = root_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -4;

		ItemConfigData item;
		if (!item.ReadConfig(item_element))
			return -5;

		int level_key = min_level * 1000 + max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_task_chain_turn_table_cfg[completion_times][level_key] = item;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TaskChainConfig::InitTaskChainLevelTimesCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TaskChainLevelTimesCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level <= 0 || node_cfg.min_level > MAX_ROLE_LEVEL)return -1;
		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)return -2;
		if (!GetSubNodeValue(root_element, "completion_times", node_cfg.completion_times) || node_cfg.completion_times <= 0)return -3;
		
		m_task_chain_level_times_cfg.push_back(node_cfg);
		m_task_chain_level_set.insert(node_cfg.completion_times);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
