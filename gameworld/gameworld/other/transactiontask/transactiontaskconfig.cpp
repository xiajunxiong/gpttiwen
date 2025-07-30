#include "transactiontaskconfig.hpp"
#include "servercommon/taskdef.h"
#include "servercommon/transactiontaskdef.hpp"
#include "servercommon/commondata.hpp"
#include <set>

TransactionTaskConfig::TransactionTaskConfig()
{
}

TransactionTaskConfig::~TransactionTaskConfig()
{
	std::vector<TransactionTaskLevelGroupCfg>().swap(m_task_level_group_cfg);
	std::vector<TransactionTaskRewardFindCfg>().swap(m_rfcfg_v);
	for (std::map<int, std::vector<TransactionTaskFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<TransactionTaskFindRewardGroupCfg>().swap(it->second);
	}
}

bool TransactionTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	
	LOAD_CONFIG("level", InitTaskLevelGroupCfg);
	LOAD_CONFIG("reward_group", InitTaskRewardGroupCfg);
	LOAD_CONFIG("all_reward", InitTaskRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_find_group", InitTaskFindRewardGroupCfg);
	LOAD_CONFIG("reward_find", InitTaskRewardFindCfg);

	return true;
}

bool TransactionTaskConfig::GetRewardListCfg(int level, int* list, short *type, short *version)
{
	//创建记录已经选择过的奖励列表
	bool flag[MAX_GROUP_ID_NUM][MAX_GROUP_SEQ_NUM] = {0};

	//创建容器来暂存奖励组,用来随机
	std::vector<TransactionTaskSingleCfg> find_list;

	int count = 0;
	int rate = 0;
	int weight = 0;
	int group_id = 0;
	int cur_count = 0;
	// 1.从等级段中取出对应的等级组
	for (int i = 0; i < (int)m_task_level_group_cfg.size(); i++)
	{
		if (level <= m_task_level_group_cfg[i].max_level && level >= m_task_level_group_cfg[i].min_level)
		{
			*type = m_task_level_group_cfg[i].type;
			*version = m_task_level_group_cfg[i].version_reset;
			//2.循环指定次数获取奖励
			for (int j = 0; j < TRANSACTION_TASK_REWARD_MAX_NUM; j++)
			{
				//3.找到奖励组列表
				if(m_task_level_group_cfg[i].group_id[j] <= 0  || m_task_level_group_cfg[i].group_id[j] > MAX_GROUP_ID_NUM) continue;

				group_id = m_task_level_group_cfg[i].group_id[j] - 1;
				cur_count = MAX_TRANSACTION_TASK_GROUP_RATE_COUNT_NUM;

				//4.清空并导入奖励组
				std::vector<TransactionTaskSingleCfg>().swap(find_list);
				for (int k = 0; k < MAX_GROUP_SEQ_NUM; k++)
				{
					if(m_group_cfg[group_id][k].seq < 0) break;

					find_list.push_back(m_group_cfg[group_id][k]);
				}
		
				//5. 计算当前该奖励组总权重
				for (int y = 0; y < MAX_GROUP_SEQ_NUM && y < (int)find_list.size(); y++)
				{
					if (!flag[group_id][y]) continue;
					
					cur_count -= find_list[y].rate;
					find_list[y].rate = 0;
				}
				if(count >= TRANSACTION_TASK_REWARD_MAX_NUM) break;

				//6.从奖励组中抽取一个并记录
				rate = rand() % cur_count;
				weight = 0;
				for (int z = 0; z < (int)find_list.size(); z++)
				{
					weight += find_list[z].rate;
					if (weight >= rate)
					{	
						if(flag[group_id][z]) continue;

						list[count++] = find_list[z].seq;
						flag[group_id][z] = true;
						break;
					}
				}
			}
		}
	}
	if(count != TRANSACTION_TASK_REWARD_MAX_NUM) return false;

	return true;
}

int TransactionTaskConfig::GetGroupByIndex(int type, int index)
{
	if(index < 0 || index >= TRANSACTION_TASK_REWARD_MAX_NUM) return 0;

	for (int i = 0; i < (int)m_task_level_group_cfg.size(); i++)
	{
		if (m_task_level_group_cfg[i].type == type)
		{
			return m_task_level_group_cfg[i].group_id[index];
		}
	}

	return 0;
}

const TransactionTaskSingleCfg * TransactionTaskConfig::GetTaskSingleRewardCfg(int group_id, int seq)
{
	if(group_id <= 0 || seq < 0 || group_id > MAX_GROUP_ID_NUM || seq > MAX_GROUP_SEQ_NUM || m_group_cfg[group_id - 1][seq].seq < 0) return NULL;

	return &m_group_cfg[group_id - 1][seq];
}



const TransactionTaskRewardCfg * TransactionTaskConfig::GetTaskRewardCfg(int level)
{
	std::map<int, TransactionTaskRewardCfg>::const_iterator it = m_task_reward_map.find(level);
	if(it != m_task_reward_map.end()) return &it->second;

	return NULL;
}

const TransactionTaskLevelGroupCfg * TransactionTaskConfig::GetTaskLevelGroupCfg(int level)
{
	for (int i = 0; i < (int)m_task_level_group_cfg.size(); i++)
	{
		if (level <= m_task_level_group_cfg[i].max_level && level >= m_task_level_group_cfg[i].min_level)
		{
			return &m_task_level_group_cfg[i];
		}
	}

	return NULL;
}

const int TransactionTaskConfig::GetRewardType(int _role_level)
{
	/*for (std::map<int, RewardFindCfg>::iterator it = m_seq_to_rfcfg.begin(); it != m_seq_to_rfcfg.end(); ++it)
	{
		if (_role_level < it->second.min_level || _role_level > it->second.max_level) continue;
		return &it->second;
	}*/

	for (std::vector<TransactionTaskRewardFindCfg>::iterator it = m_rfcfg_v.begin(); it != m_rfcfg_v.end(); ++it)
	{
		if (_role_level < it->min_level || _role_level > it->max_level) continue;
		return it->find_type;
	}

	return -1;
}

const TransactionTaskRewardFindCfg * TransactionTaskConfig::GetFindRewardCfg(int find_type, int level)
{
	for (std::map<int, std::map<int, TransactionTaskRewardFindCfg> >::iterator it = m_transaction_task_find_cfg.begin(); it != m_transaction_task_find_cfg.end(); it++)
	{
		if (it->first / 1000 <= level && it->first % 1000 >= level)
		{
			std::map<int, TransactionTaskRewardFindCfg>::iterator find_it = it->second.find(find_type);
			if (find_it != it->second.end())
			{
				return &find_it->second;
			}
		}
	}

	return NULL;
}

const TransactionTaskFindRewardGroupCfg * TransactionTaskConfig::GetFindRewardGroupCfg(int group_id)
{
	std::map<int, std::vector<TransactionTaskFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.find(group_id);
	if(it == m_find_reward_group_map.end()) return NULL;

	int rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (rate < it->second[i].rate)
		{
			return &it->second[i];
		}
		rate -= it->second[i].rate;
	}


	return NULL;
}

int TransactionTaskConfig::InitTaskLevelGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		int min_level = 0, max_level = 0, type = 0;
		if (!GetSubNodeValue(root_element, "level_min", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "level_max", max_level) || max_level <= min_level || max_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "type", type) || type < 0) return -3;

		if (m_task_level_group_cfg.size() > 0)
		{
			if (m_task_level_group_cfg[m_task_level_group_cfg.size() - 1].min_level != min_level &&
				m_task_level_group_cfg[m_task_level_group_cfg.size() - 1].max_level != max_level && 
				m_task_level_group_cfg[m_task_level_group_cfg.size() - 1].type != type)
				m_task_level_group_cfg.push_back(TransactionTaskLevelGroupCfg());
		}
		if (m_task_level_group_cfg.size() == 0)
		{
			m_task_level_group_cfg.push_back(TransactionTaskLevelGroupCfg());
		}
		
		TransactionTaskLevelGroupCfg & cfg = m_task_level_group_cfg[m_task_level_group_cfg.size() - 1];
		cfg.min_level = min_level;
		cfg.max_level = max_level;
		cfg.type = type;

		int seq = 0, group_id = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0 || seq > TRANSACTION_TASK_REWARD_MAX_NUM) return -4;
		if (!GetSubNodeValue(root_element, "reward_group", group_id) || group_id <= 0 || group_id > MAX_GROUP_ID_NUM) return -5;
		if (!GetSubNodeValue(root_element, "reset", cfg.version_reset) || cfg.version_reset < 0) return -6;

		cfg.group_id[seq - 1] = group_id;
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TransactionTaskConfig::InitTaskRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		static int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0 || group_id > MAX_GROUP_ID_NUM) return -1;

		TransactionTaskSingleCfg node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq > MAX_GROUP_SEQ_NUM) return -2;

		TiXmlElement *need_element = root_element->FirstChildElement("need_item");
		if(NULL != need_element)
		{
			if (!node_cfg.need_item.ReadConfig(need_element)) 
			return -3;
		}

		//不会同时出现2个都没奖励
		if (!GetSubNodeValue(root_element, "money_num", node_cfg.money_num) || node_cfg.money_num < 0) return -4;
		TiXmlElement* reward_element = root_element->FirstChildElement("reward");
		if (NULL != reward_element)
		{
			bool is_has_item_reward = true;
			if (!node_cfg.reward.ReadConfig(reward_element))
			{
				is_has_item_reward = false;
			}
			if (node_cfg.money_num <= 0 && !is_has_item_reward)
			{
				return -5;
			}
			node_cfg.is_has_item_reward = is_has_item_reward;
		}

		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0) return -6;

		m_group_cfg[group_id - 1][node_cfg.seq] = node_cfg;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TransactionTaskConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TransactionTaskOtherCfg node_cfg;

		if (!GetSubNodeValue(root_element, "random", node_cfg.random) || node_cfg.random <= 0 || node_cfg.random > TRANSACTION_TASK_REWARD_MAX_NUM) return -1;
		if (!GetSubNodeValue(root_element, "task_id", node_cfg.task_id) || node_cfg.task_id <= 0) return -2;

		m_other_cfg = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TransactionTaskConfig::InitTaskRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TransactionTaskRewardCfg node_cfg;

		if (!GetSubNodeValue(root_element, "level", node_cfg.level) || node_cfg.level <= 0 || node_cfg.level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp)) return -2;
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0) return -3;

		TiXmlElement * list_node = root_element->FirstChildElement("item_list_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("item_list");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				node_cfg.task_reward.push_back(item);
				node_cfg.count++;

				item_node = item_node->NextSiblingElement();
			}
		}
		m_task_reward_map[node_cfg.level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TransactionTaskConfig::InitTaskRewardFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		TransactionTaskRewardFindCfg rf_cfg;
		if (!GetSubNodeValue(root_element, "seq", rf_cfg.seq) || rf_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(root_element, "min_level", rf_cfg.min_level) || rf_cfg.min_level < 0 || rf_cfg.min_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "max_level", rf_cfg.max_level) || rf_cfg.max_level > MAX_ROLE_LEVEL || rf_cfg.max_level < rf_cfg.min_level) return -3;
		if (!GetSubNodeValue(root_element, "find_type", rf_cfg.find_type) || rf_cfg.find_type < RB_TYPE_COIN || rf_cfg.find_type >= RB_TYPE_MAX) return -4;
		if (!GetSubNodeValue(root_element, "group_id", rf_cfg.group_id) || (rf_cfg.group_id > 0 && m_find_reward_group_map.end() == m_find_reward_group_map.find(rf_cfg.group_id))) return -5;	
		if (!GetSubNodeValue(root_element, "exp", rf_cfg.exp) || rf_cfg.exp < 0) return -6;
		if (!GetSubNodeValue(root_element, "coin", rf_cfg.coin) || rf_cfg.coin < 0) return -7;
		if (!GetSubNodeValue(root_element, "family_gongxian", rf_cfg.family_gongxian) || rf_cfg.family_gongxian < 0) return -8;

		int level_key = rf_cfg.min_level * 1000 + rf_cfg.max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		m_transaction_task_find_cfg[level_key][rf_cfg.find_type] = rf_cfg;

		m_rfcfg_v.push_back(rf_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int TransactionTaskConfig::InitTaskFindRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int group_id;
		TransactionTaskFindRewardGroupCfg tmp_con;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.rate) || tmp_con.rate <= 0 || tmp_con.rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * list_node = root_element->FirstChildElement("reward_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -100;

				tmp_con.reward_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}

		m_find_reward_group_map[group_id].push_back(tmp_con);
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<TransactionTaskFindRewardGroupCfg> >::iterator group_it = m_find_reward_group_map.begin(); group_it != m_find_reward_group_map.end(); ++group_it)
	{
		int rate_total = 0;
		for (std::vector<TransactionTaskFindRewardGroupCfg>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
		{
			rate_total += item_it->rate;
		}

		if (rate_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -500;
	}
	
	return 0;
}
