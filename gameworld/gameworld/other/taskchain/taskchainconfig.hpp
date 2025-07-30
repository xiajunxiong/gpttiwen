#ifndef __TASK_CHAIN_CONFIG_HPP__
#define __TASK_CHAIN_CONFIG_HPP__
#pragma once

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <vector>
#include <map>
#include <algorithm>
#include <set>

enum TASK_CHAIN_TYPE
{
	TASK_CHAIN_TYPE_MIN = 0,		//			param1		param2				param3				param4				param5				param6				param7				param8
	TASK_CHAIN_TYPE_TALK,			// 交谈		npc_id		|reputation_city	|reputation_num
	TASK_CHAIN_TYPE_ITEM,			// 交货		npc_id		|item_id			|num				|drop_scene			|drop_rate			|has_drop			|reputation_city	|reputation_num
	TASK_CHAIN_TYPE_FIGHT,			// 交手		monster_id	|scene_id			|x*10000+y			|monster_group_id	|jump_cost_type		|jump_cost_num		|reputation_city	|reputation_num
	TASK_CHAIN_TYPE_PET,			// 交宠		npc_id		|pet_item			|pet_id				|reputation_city	|reputation_num
	TASK_CHAIN_TYPE_WAIT,			// 等待		npc_id		|item_id			|time_limit			|pay_npc_id			|reputation_city	|reputation_num
	TASK_CHAIN_TYPE_MAX,
};

struct TaskChainOtherCfg
{
	TaskChainOtherCfg() : level(0), max_times(0), space_times(0){}

	int level;
	int max_times;
	int space_times;
};

struct TaskChainCfg
{
	TaskChainCfg() : task_type(0), param1(0), param2(0), param3(0), param4(0), can_skip(false), reward_double(0),price_type(0),price(0), time_limit(0), coin(0), rate(0) {}

	int task_type;
	int param1;
	int param2;
	int param3;
	int param4;
	bool can_skip;
	int reward_double;
	int price_type;
	int price;
	int time_limit;
	int coin;
	int rate;
};

struct TaskChainSceneCfg
{
	TaskChainSceneCfg() : scene_id(0), rate(0), pos_group_id(0), reputation_city(0), reputation_num(0) {}

	int scene_id;
	int rate;
	int pos_group_id;
	int reputation_city;
	int reputation_num;
};

struct TaskChainMonsterCfg
{
	TaskChainMonsterCfg() : monster_id(0), rate(0) {}

	int monster_id;
	int rate;
};

struct TaskChainNpcCfg
{
	TaskChainNpcCfg() : npc_id(0), rate(0), reputation_city(0), reputation_num(0){}

	int npc_id;
	int rate;
	int reputation_city;
	int reputation_num;
};

struct TaskChainItemCfg
{
	TaskChainItemCfg() : rate(0), is_specialway(0){}

	int rate;
	int is_specialway;
	ItemConfigData item;
};

struct TaskChainExpItemCfg
{
	TaskChainExpItemCfg() : min_level(0), max_level(0), min_num(0), max_num(0), exp(0), coin(0),
		coin_rate(0) , item_rate(0) {}

	int min_level;
	int max_level;

	int min_num;
	int max_num;

	int exp;
	int coin;

	int coin_rate;
	int item_rate;

	ItemConfigData item;
};

struct TaskChainItemDropSceneCfg
{
	TaskChainItemDropSceneCfg() : scene_id(0), item_rate(0), rate(0){}

	int scene_id;
	int item_rate;
	int rate;
};

struct TaskChainPetCfg
{
	TaskChainPetCfg() : pet_id(0), item_id(0), rate(0) {}

	int pet_id;
	int item_id;
	int rate;
};

struct TaskChainLevelTimesCfg
{
	TaskChainLevelTimesCfg() : min_level(0), max_level(0), completion_times(0)
	{}

	int min_level;
	int max_level;
	int completion_times;
};

struct TaskChainLevelTimes
{
	TaskChainLevelTimes() : index(-1), completion_times(0)
	{}

	int index;
	int completion_times;
};

struct TaskChainLoopRewardCfg
{
	TaskChainLoopRewardCfg() : is_remind(0)
	{}

	std::vector<int> reward_group;
	int is_remind;			// 0:不弹窗
};

class TaskChainConfig : public ILogicConfig
{
public:
	TaskChainConfig();
	virtual ~TaskChainConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TaskChainOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const TaskChainCfg* RandTaskType(int level,int per_task_type, short pre_wait_count = 0, int cur_completion_times = -1) const;
	const TaskChainSceneCfg* RandScene(int group_id)const;
	const Posi * RandScenePosi(int group_id)const;
	const TaskChainMonsterCfg* RandMonster(int group_id)const;
	const TaskChainNpcCfg* RandNpc(int group_id, int pre_task_npc_id)const;
	const TaskChainItemCfg* RandItem(int group_id)const;
	const TaskChainItemCfg* RandWaitItem(int group_id)const;
	const TaskChainExpItemCfg * GetExp(int level, int times)const;
	const TaskChainItemDropSceneCfg* RandItemDropScene(int group_id)const;
	const TaskChainPetCfg * RandPet(int group_id)const;
	bool GetTaskReward(int role_level, int finish_times, std::vector<ItemConfigData> & out_list);
	const TaskChainCfg* GetTaskByTaskID(int level,int task_type);

	const std::map<int, std::vector<TaskChainItemCfg> >* GetWaitItemMap() { return &m_task_chain_wait_item_map; }

	int GetTurnTableRewardGroup(int completion_times, int level);
	int GetRankTurnTableReward(int group_id);
	const ItemConfigData * GetTurnTableReward(int completion_times, int level);
	int GetMaxCompletionTimesByLevel(int level);
	bool HasCompletionTimes(int completion_times);
	int GetNextCompletionTimes(int completion_times, int max_completion_times_by_level);
	TaskChainLevelTimes GetStageByCompetionTimes(int completion_times) const;
	const TaskChainLevelTimesCfg * GetCompletionTimesByIndex(int index);
	int GetMaxCompletionTimes();
	bool IsCanAcceptWaitTask(int cur_completion_times)const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskChainCfg(TiXmlElement *RootElement);
	int InitTaskChainSceneCfg(TiXmlElement *RootElement);
	int InitTaskChainScenePosiCfg(TiXmlElement *RootElement);
	int InitTaskChainMonsterCfg(TiXmlElement *RootElement);
	int InitTaskChainNpcCfg(TiXmlElement *RootElement);
	int InitTaskChainItemCfg(TiXmlElement *RootElement);
	int InitTaskChainWaitItemCfg(TiXmlElement *RootElement);
	int InitTaskChainExpCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitTaskRewardListCfg(TiXmlElement *RootElement);
	int InitTaskItemDropSceneCfg(TiXmlElement *RootElement);
	int InitTaskChainPetCfg(TiXmlElement *RootElement);
	int InitTaskChainTurnTableCfg(TiXmlElement *RootElement);
	int InitTaskChainLevelTimesCfg(TiXmlElement *RootElement);

	TaskChainOtherCfg m_other_cfg;

	std::map<int, int> m_task_chain_total_rate;
	std::map<int, std::vector<TaskChainCfg> > m_task_chain_map;			// key 1 等级

	std::map<int, int> m_task_chain_scene_total_rate;
	std::map<int, std::vector<TaskChainSceneCfg> > m_task_chain_scene_map;

	std::map<int, int> m_task_chain_scene_pos_total_rate;
	std::map<int, std::map<int, Posi> > m_task_chain_scene_pos_map;

	std::map<int, int> m_task_chain_monster_total_rate;
	std::map<int, std::vector<TaskChainMonsterCfg> > m_task_chain_monster_map;

	std::map<int, int> m_task_chain_npc_total_rate;
	std::map<int, std::vector<TaskChainNpcCfg> > m_task_chain_npc_map;

	std::map<int, int> m_task_chain_item_total_rate;
	std::map<int, std::vector<TaskChainItemCfg> > m_task_chain_item_map;

	std::map<int, int> m_task_chain_wait_item_total_rate;
	std::map<int, std::vector<TaskChainItemCfg> > m_task_chain_wait_item_map;

	std::map<int, std::vector<TaskChainItemCfg> > m_reward_group_list;			// 奖励随机组：键 - 组ID
	std::map<ParamSection, std::map<int, TaskChainLoopRewardCfg> > m_reward_list;	// 环数奖励表：键 - 等级区间、完成环数、环奖励信息

	std::map<int, std::map<int, TaskChainExpItemCfg> > m_task_chain_exp_map;

	std::map<int, int> m_task_chain_item_drop_scene_rate;
	std::map<int, std::vector<TaskChainItemDropSceneCfg> > m_task_chain_item_drop_scene_map;

	std::map<int, int> m_task_chain_pet_rate;
	std::map<int, std::vector<TaskChainPetCfg> > m_task_chain_pet_map;

	std::map<int, std::map<int, ItemConfigData> > m_task_chain_turn_table_cfg;				// 转盘奖励 key1:completion_times	 key2: min_level * 1000 + max_level  second:reward_group

	std::vector<TaskChainLevelTimesCfg> m_task_chain_level_times_cfg;			// 等级对应环数
	std::set<int> m_task_chain_level_set;										// 环数
};

#endif // !__TASK_CHAIN_CONFIG_HPP__

