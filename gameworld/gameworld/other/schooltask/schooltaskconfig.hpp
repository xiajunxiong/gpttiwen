#ifndef __SCHOLL_TASK_CONFIG_HPP__
#define __SCHOLL_TASK_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>
#include <algorithm>

struct SchoolTaskOtherCfg
{
	SchoolTaskOtherCfg()
	{
		daily_num = 0;
		quick_finish = 0;
		power_reward = 0;
	}
	int daily_num;
	int quick_finish;
	int power_reward;
};
static const int SCHOOL_TASK_REWARD_MAX_COUNT = 5;

struct SchoolTaskCfg
{
	SchoolTaskCfg():task_id(0), min_level(0), max_level(0), condition(0), c_param1(0), c_param2(0), target_scene(0), reputation_city(0), reputation_num(0)
	{}

	int task_id;
	int min_level;
	int max_level;
	int condition;
	int c_param1;
	int c_param2;
	int target_scene;
	int reputation_city;
	int reputation_num;
};

struct SchoolTaskRewardCfg
{
	SchoolTaskRewardCfg()
	{
		rate = 0;
	}

	int rate;
	ItemConfigData reward;
};

static const int GROUP_NUM_MAX = 5;

struct SchoolTaskrewardFindCfg
{
	SchoolTaskrewardFindCfg() : exp(0), coin(0) 
	{
		memset(reward_group,0,sizeof(reward_group));
	}

	int exp;
	int coin;
	int reward_group_cnt;
	int reward_group[GROUP_NUM_MAX];
};

struct SchoolTaskLevelRewardCfg
{
	SchoolTaskLevelRewardCfg() : level(0), exp(0), coin(0), reward_group_cnt(0)
	{
		memset(reward_group_list, 0, sizeof(reward_group_list));
	}

	int level;
	int exp;
	int coin;
	int reward_group_cnt;
	int reward_group_list[GROUP_NUM_MAX];
};

class SchoolTaskConfig : public ILogicConfig
{
public:
	SchoolTaskConfig();
	virtual ~SchoolTaskConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const SchoolTaskOtherCfg& GetOtherCfg() { return m_other_cfg; }
	const SchoolTaskCfg* GetTaskCfg(int task_id);
	bool GetTaskList(int level, int *task_list);
	void RandTaskListForServer(int *task_list);

	const ItemConfigData* RandItemReward(int group);
	const SchoolTaskLevelRewardCfg* GetLevelReward(int level);

	bool GetFindReward(int level, int type, int *coin, int *exp, std::vector<ItemConfigData>* item);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitRewardFindCfg(TiXmlElement *RootElement);
	int InitLevelRewardCfg(TiXmlElement *RootElement);

	SchoolTaskOtherCfg m_other_cfg;
	std::map<int, SchoolTaskCfg> m_task_map;
	std::map<int, std::vector<SchoolTaskCfg> > m_task_by_level;

	std::map<int, std::vector<SchoolTaskRewardCfg> > m_reward_map;
	std::map<int, std::map<int, SchoolTaskrewardFindCfg> > m_reward_find_map;			// key1 最小等级*1000 + 最大等级   key2找回类型

	SchoolTaskLevelRewardCfg m_level_reward_cfg_list[MAX_ROLE_LEVEL + 1];
};

#endif