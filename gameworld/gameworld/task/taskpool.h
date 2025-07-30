#ifndef __TASKPOOL_HPP__
#define __TASKPOOL_HPP__

#include "gamedef.h"
#include "servercommon/taskdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/bosschapterdef.hpp"
#include "servercommon/anecdotetaskdef.hpp"
#include "servercommon/prestigedef.hpp"

#include <map>
#include <set>
#include <vector>

static const int TASK_PREDECESSOR_COUNT_MAX = 3;		//!< 单个任务的前置任务数量上限
static const int TASK_REWARD_LIST_COUNT_MAX = 5;		//!< 单个任务能获得的道具奖励的最大种数
static const int TASK_LIMIT_DROP_ITEM_NUM_MAX = 5;		//!< 单个任务最多的限制掉落物品数量

struct TaskInfomaConfig		// 任务信息头文件 - 通用
{
	TaskInfomaConfig(int t_id = 0)
	{
		task_id = t_id;
		task_ver = 0;
		task_type = TASK_TYPE_MAIN_QUEST;
		team_limit_type = TASK_TEAM_UNCONFINED;
		task_param1 = 0;
		task_param2 = 0;
		task_param3 = 0;

		level_min = 0;
		level_max = 0;
		prof_limit_min = PROF_TYPE_INVALID;
		prof_limit_max = PROF_TYPE_INVALID;
		pre_task_id = 0;
		pre_task_id2 = 0;
		nex_task_id = 0;
		nex_task_id2 = 0;
		accept_npc = 0;

		task_condition = TASK_NEED_TO_DO_NOTHING;
		timer_limit = 0;
		param1 = 0;
		param2 = 0;
		commit_npc = 0;
		can_auto_commit = false;
		can_auto_catch = false;

		is_share = 0;
		memset(giveup, 0, sizeof(giveup));

		reward_allot = ALLOT_TYPE_COMMON;

		is_add_robot = false;
		can_skip_battle = false;
		skip_battle_cost_coin = 0;

		limit_drop_count = 0;
		memset(limit_drop_item, 0, sizeof(limit_drop_item));

		battle_mode = 0;

		is_skip = false;
		skip_cost_type = 0;
		skip_cost_num = 0;
		open_server_day = 0;
	}

	///////////////////// 基本信息 /////////////////////
	int task_id;				// 任务ID
	int task_ver;				// 任务版本
	int task_type;				// 任务类型
	int team_limit_type;		// 组队限制
	int task_param1;
	int task_param2;
	int task_param3;

	///////////////////// 接取条件 /////////////////////
	int level_min;				// 最低等级
	int level_max;				// 最高等级
	int prof_limit_min;			// 职业限制：左值
	int prof_limit_max;			// 职业限制：右值
	int pre_task_id;			// 前置任务： 0 - 无需前置
	int pre_task_id2;			// 前置任务2
	int nex_task_id;			// 后置任务： 0 - 无需后置
	int nex_task_id2;			// 后置任务2
	int accept_npc;				// 接取任务的 NPC
	int open_server_day;		// 开服天数

	///////////////////// 任务目标 /////////////////////
	int task_condition;			// 完成条件
	int timer_limit;			// 任务限时 - 0 表示 不限时。单位：秒(s)
	int param1;					// 目标 NPC / 怪物组 / 道具ID / 拾取物ID
	int param2;					// 数量 / 时间长度(s)
	int commit_npc;				// 提交任务的 NPC
	bool can_auto_commit;		// 是否可以自动提交
	bool can_auto_catch;		// 是否可以自动接取

	///////////////////// 特殊参数 /////////////////////
	int is_share;				// 是否共享给全队
	int giveup[GIVEUP_TYPE_MAX];// 任务放弃行为类型

	int limit_drop_count;
	ItemID limit_drop_item[TASK_LIMIT_DROP_ITEM_NUM_MAX];	// 接任务后开放掉落物品ID（完成任务后关闭掉落）

	bool is_add_robot;			// 是否加机器人
	bool can_skip_battle;		// 是否可跳过战斗(将视为战胜了怪物组)
	int skip_battle_cost_coin;	// 跳过战斗铜币耗费

	int battle_mode;			// 任务战斗完之后触发相关战斗模式处理

	bool is_skip;
	int skip_cost_type;
	int skip_cost_num;

	///////////////////// 任务奖励 /////////////////////
	// virtual : 每类任务获得奖励的逻辑不同，应分情况讨论
	int reward_allot;			// 奖励分配类型

	///////////////////// 成员函数 /////////////////////

	//////////////////// 运算符重载 ////////////////////
	bool operator == (const TaskInfomaConfig & cobj) const
	{
		return (this->task_id == cobj.task_id);
	}

	bool operator < (const TaskInfomaConfig & cobj) const
	{
		return (this->task_id < cobj.task_id);
	}
};

// ———————————————————————————————————————————————————————————— 任务奖励 ———————————————————————————————————————————————————————————— //
struct CommonTaskReward		// 通常任务奖励容器
{
	CommonTaskReward() : allot_min(0), allot_max(0), coin(0), immortal(0), gold(0), exp(0), pet_exp(0), repu_city(0), repu_numb(0), power(0),
		reward_count(0) { memset(reward_list, 0, sizeof(reward_list)); }

	int allot_min;		// 闭区间左值 - 当奖励类型为 0 时，这个参数没啥用
	int allot_max;		// 闭区间右值 - 当奖励类型为 0 时，这个参数没啥用

	int coin;			// 金币报酬
	int immortal;		// 仙币报酬 - 其实就是 绑元
	int gold;			// 元宝报酬
	int exp;			// 经验值
	int pet_exp;		// 宠物经验 - 加给出战的宠物，如果没有出战的宠物，就不加

	int repu_city;		// 声望所在地
	int repu_numb;		// 声望数值
	int power;			// 增加秘境体力

	int reward_count;	// 奖励数量
	ItemConfigData reward_list[TASK_REWARD_LIST_COUNT_MAX];	// 奖励物品
};

// ———————————————————————————————————————————————————————————— 其它容器 ———————————————————————————————————————————————————————————— //
struct DefeatMonsGroups
{
	DefeatMonsGroups() : need_item(0), drop_id(0) {}

	int need_item;
	int drop_id;

	std::set<int> drop_monster_list;
};

struct AdventruesOthers
{
	AdventruesOthers() : limit_times_daily(0), basic_trigger_rate(0), talk_rate(0), task_level_limit(0){}

	int limit_times_daily;		// 每日允许触发的次数
	int basic_trigger_rate;		// 基本触发概率：百分比（%）
	int talk_rate;				// 对话触发概率
	int task_level_limit;		// 等级限制
};

struct AdvTaskActivates
{
	AdvTaskActivates() : prob(0), mission_group(0), partner_id(0), partner_pro(0){}

	int prob;
	int mission_group;
	int partner_id;
	int partner_pro;
};

struct AdvTaskGroup
{
	AdvTaskGroup() : seq(0), activation_task_id(0) , active_probability_10000(0){}

	int seq;
	int activation_task_id;
	int active_probability_10000;
};

struct AdvTaskGroupList
{
	AdvTaskGroupList() : weight(0){}

	int weight;
	std::vector<AdvTaskGroup> m_task_list;
};

struct BossChapterBossCfg
{
	static const int REWARD_NUM = 5;

	BossChapterBossCfg() : monster_group_id(0), is_elite(false), related_task_id(0), reward_num(0) {}

	int monster_group_id;
	bool is_elite;
	int related_task_id;

	int reward_num;
	ItemConfigData reward_list[REWARD_NUM];
};

struct BossChapterChapterRewardCfg
{
	static const int REWARD_NUM = 5;

	BossChapterChapterRewardCfg() : reward_num(0) {}

	int reward_num;
	ItemConfigData reward_list[REWARD_NUM];
};

struct BountyTaskOtherCfg
{
	BountyTaskOtherCfg() : level_limit(0), tasknum_per_day(0), skip_first_task_level(0),
		skip_first_task_task_id(0), first_task_level(0){}

	int level_limit;
	int tasknum_per_day;

	int skip_first_task_level;
	int skip_first_task_task_id;

	int first_task_level;	// 首次门派任务特殊逻辑触发等级限制
};

struct BountyTaskTurnTableCfg
{
	BountyTaskTurnTableCfg() : reward_group_id(0), reward_num(0), total_rate(0), min_level(0), max_level(0), first_reward_index(0)
	{
		memset(rate_list, 0, sizeof(rate_list));
		memset(is_hearsay, 0, sizeof(is_hearsay));
	}

	static const int TABLE_REWARD_NUM = 12;

	int reward_group_id;
	int reward_num;
	ItemConfigData reward_list[TABLE_REWARD_NUM];
	int rate_list[TABLE_REWARD_NUM];
	int total_rate;
	int min_level;
	int max_level;
	int first_reward_index;
	int is_hearsay[TABLE_REWARD_NUM];
};

struct BountyTaskSimpleCfg
{
	BountyTaskSimpleCfg() : is_first_task(false), task_id(0), min_level(0), max_level(0) {}

	bool is_first_task;
	int task_id;
	int min_level;
	int max_level;
};

struct BountyTaskFindCfg
{
	BountyTaskFindCfg():seq(0),find_type(0), min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}
	
	int seq;
	int find_type;
	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct BountyTaskFindRewardCfg
{
	BountyTaskFindRewardCfg():group_id(0), rate(0)
	{}

	int group_id;
	ItemConfigData reward;
	int rate;
};

struct BountyTaskFinishRewardCfg
{
	BountyTaskFinishRewardCfg():min_level(0), max_level(0)
	{}

	int min_level;
	int max_level;
	std::vector<ItemConfigData> reward_list;
};

struct TaskOtherCfg
{
	TaskOtherCfg() : realive_guide_monster_group(0), anecdotes_recommend_task_id(0), /*award_surface_point_task_id(0),
		award_surface_seq(0),*/ career_rewards(0), partner_upgrade_task_id(0)
	{}

	int realive_guide_monster_group;
	int anecdotes_recommend_task_id;		//推荐任务ID特殊判断奇闻
	//int award_surface_point_task_id;		//奖励幻化点的任务ID(变身卡)
	//int award_surface_seq;					//奖励幻化卡索引
	int career_rewards;						//特殊奖励逻辑的任务id
	int partner_upgrade_task_id;			//伙伴晋升任务ID
};

struct AnecdoteInitialTaskCfg
{
	AnecdoteInitialTaskCfg() : task_id(0), level_limit(0) {}

	int task_id;
	int level_limit;
};

struct AnecdoteRewardCfg
{
	AnecdoteRewardCfg() : is_valid(false), anecdote_type(0), coin_bind(0), exp(0), gold(0), reward_count(0) {}

	bool is_valid;
	int anecdote_type;
	int coin_bind;
	int exp;
	int gold;

	int reward_count;
	ItemConfigData reward_list[TASK_REWARD_LIST_COUNT_MAX];
};

struct AnecdoteMonsterGroupCfg
{
	AnecdoteMonsterGroupCfg() : anecdote_task_type(0), npc_id(0), min_level(0), 
		max_level(0), monster_group_id(0)
	{

	}

	int anecdote_task_type;
	int npc_id;
	int min_level;
	int max_level;
	int monster_group_id;
};

struct AnecdoteOtherCfg
{
	AnecdoteOtherCfg() : big_event_open_duration_hours(0), big_event_open_level(0), big_event_version(0) {}

	int big_event_open_duration_hours;
	int big_event_open_level;
	int big_event_version;
};

struct AnecdoteBigEventRewardCfg
{
	static const int MAX_REWARD_NUM = 5;

	AnecdoteBigEventRewardCfg() : seq(0), complete_anecdote_num(0), item_num(0) {}

	int seq;
	int complete_anecdote_num;
	int item_num;
	ItemConfigData item_list[MAX_REWARD_NUM];
};

struct PrestigeTaskVillageCfg
{
	PrestigeTaskVillageCfg() : is_valid(0), village_id(0), unlock_level(0), reward_num(0), finish_all_reward_group(0){}

	bool IsValid() const { return is_valid; }

	bool is_valid;
	int village_id;
	int unlock_level;
	int reward_num;
	ItemConfigData reward_list[TASK_REWARD_LIST_COUNT_MAX];

	int finish_all_reward_group;
};

struct PrestigeTaskCfg
{
	PrestigeTaskCfg() : village_id(0), task_group(0), task_id(0), prestige_reward_group(0), is_activedegree(false) {}

	int village_id;
	int task_group;
	TaskID task_id;
	short prestige_reward_group;
	bool is_activedegree;
};

struct PrestigeTaskPrestigeGroupCfg
{
	struct Cfg
	{
		int role_level;
		int add_prestige_num;
	};

	int group_id;
	std::vector<Cfg> level_prestige_vec;
};

struct PrestigeTaskStarCfg
{
	std::vector<ItemConfigData> reward_item;
};

struct PrestigeTaskOtherCfg
{
	PrestigeTaskOtherCfg() : reset_times(0), use_item(0) {}

	int reset_times;
	ItemID use_item;
};

struct AnecdoteSpecialCfg
{
	AnecdoteSpecialCfg() :special_type(0), monster_seq(0), npc_id(0), monster_id(0) {}

	int special_type;
	int	monster_seq;
	int	npc_id;
	int monster_id;
	std::vector<ItemConfigData> reward_vec;
};

struct TeamChallengeRewardCfg
{
	TeamChallengeRewardCfg() :task_id(0), coin_bind(0), exp(0), pet_exp(0), immortal(0), gold(0) {}

	int task_id;
	int coin_bind;
	int	exp;
	int	pet_exp;
	int	immortal;
	int	gold;
	std::vector<ItemConfigData> reward_vec;
};

class Role;

#define TASKPOOL TaskPool::Instance()

class TaskPool
{
public:
	typedef std::map<int, BossChapterBossCfg> BossChapterBossCfgMap;
	typedef std::vector<int> ChapterBossList;
	typedef std::vector<BountyTaskSimpleCfg> BountyTaskList;
	typedef std::map<TaskID, TaskID> NextBountyTaskMap;
	typedef std::map<ItemID, TaskID> LimitDropItemMap;
	typedef std::map<int, AnecdoteMonsterGroupCfg> AnecdoteMonsterGroupCfgMap;
	typedef std::map<int, PrestigeTaskPrestigeGroupCfg> PrestigeGroupCfgMap;
	typedef std::map<int, PrestigeTaskCfg> PrestigeTaskCfgMap;
	typedef std::vector<int> PrestigeTaskGroupVec;

public:
	TaskPool();
	~TaskPool();

	static TaskPool * Instance();

	bool Init(const std::string &dir, std::string *err);
	static bool Reload(const std::string &config_dir, std::string *err);

	bool IsMonsterGroupsMember(int monster_group, int monster_id);
	const std::vector<int>* GetMonsterGroupMonsterList(int monster_group);
	bool IsItemGroupsMember(int item_group, int item_id);
	const std::set<ItemID> * GetItemGroupList(int item_group);
	bool IsInRightOrder(int monster_group, int monster_id, int order);

	void GetPrevTaskList(TaskID task_id, std::set<TaskID> & result_list);
	void GetNextTaskList(TaskID task_id, std::set<TaskID> & result_list);
	const std::set<TaskID>* GetTaskListAtLv(UInt16 min_lv);
	const std::set<TaskID>* GetTaskListWithBaseProf(int base_prof_type);
	const std::set<TaskID>* GetTaskListAtTaskType(int task_type);

	const TaskInfomaConfig * GetTaskCfgByID(int p_task_id);
	const CommonTaskReward * GetCommonTaskReward(int p_task_id, Role * p_role);		// 获取通常任务报酬
	const DefeatMonsGroups * GetDefeatMonsGroups(int group_id);
	const AdventruesOthers & GetAdvTaskOtherCfg() { return m_adv_task_other; }

	int RandAdventureTask(int level, std::set<int>* unlocked_partner_set = NULL);
	int GetAdventureTaskByUsingItem(int item_id, int level);
	int GetAdventureTaskTimeLimit(int task_id);
	int GetAdventureTaskByGroupID(int group_id);

	const BountyTaskOtherCfg& GetBountyTaskOtherCfg() { return m_bounty_task_other_cfg; }
	TaskID RandomBountyTask(int role_lv, int accepted_task_num, ARG_IN TaskID* accepted_task_list);

	const BossChapterBossCfg* GetBossChapterBossCfg(int monster_group_id);
	const ChapterBossList* GetChapterBossList(int chapter);
	const BossChapterChapterRewardCfg* GetBossChapterChapterRewardCfg(int chapter);
	int RandomBountyTaskTurnTableReward(int role_level, bool is_first);
	const BountyTaskTurnTableCfg* GetBountyTaskTurnTableCfg(int role_level);
	int GetBountyTaskGroupId(int role_level);
	const BountyTaskTurnTableCfg* GetBountyTaskTurnTableCfgByGroupId(int group_id);
	TaskID GetNextBountyTaskID(TaskID task_id);
	TaskID GetFirstBountyTaskID(int prof_type);
	const BountyTaskFindRewardCfg * GetBountyTaskFindRewardCfgByGroupId(int group_id);
	const BountyTaskFindCfg * GetBountyTaskFindCfg(int level, int find_type);
	const int GetFindType(int _role_level);
	int GetFirstTaskCfg(int seq)const;
	const std::vector<ItemConfigData>* GetBountyTaskFinishRewardCfg(int role_level) const;	// 获取赏金任务完成奖励

	TaskID GetLimitDropItemTask(ItemID item_id);

	const TaskOtherCfg& GetTaskOtherCfg() const { return m_task_other_cfg; }

	int GetAnecdoteTaskType(int task_id);
	const AnecdoteInitialTaskCfg* GetAnecdoteTaskInitialCfg(int anecdote_task_type);
	const AnecdoteRewardCfg* GetAnecdoteFinalReward(int anecdote_task_type);
	const AnecdoteMonsterGroupCfg* GetAnecdoteMonsterGroupCfg(int monster_group_id);

	TaskID GetRobotMissionBeginTask() const { return m_robot_mission_begin_task; };

	bool IsConditionNeedItem(ItemID item_id) const;

	const PrestigeTaskCfg* GetPrestigeTaskCfg(int task_id);
	const PrestigeTaskVillageCfg* GetPrestigeTaskVillageCfg(int village_id);
	int RandPrestigeTaskGroup(int village_id);
	int FillPrestigeTaskList(int village_id, int task_group, int task_list_size, ARG_OUT UInt16* task_list);

	int GetPrestigeTaskReward(int role_level, int reward_group);
	int GetPrestigeTaskEndReward(int role_level, int reward_group);
	bool IsPrestigeTaskAddActiveDegree(int task_id);
	const PrestigeTaskStarCfg * GetPrestigeTaskStarCfg(int star);
	int GetPrestigeTaskStar(bool is_rate_2 = false);
	const PrestigeTaskOtherCfg * GetPrestigeTaskOtherCfg() { return &m_prestige_task_other_cfg; }

	const AnecdoteOtherCfg& GetAnecdoteOthetCfg() const { return m_anecdote_other_cfg; }
	const AnecdoteBigEventRewardCfg* GetAnecdoteBigEventRewardCfg(int seq, int version) const;
	const std::vector<AnecdoteBigEventRewardCfg>* GetAnecdoteBigEventRewardCfgVec(int version) const;

	const std::vector<ItemConfigData>* GetSpecialRewardCfg(int job)const;

	const AnecdoteSpecialCfg * GetAnecdoteSpecialCfg(int task_type, int monster_seq);

	const TeamChallengeRewardCfg * GetTeamChallengeRewardCfg(int task_id);
private:
	int InitDefeatsGroupCfg(TiXmlElement * RootElement);		// 消灭组配置
	int InitMonsterGroupCfg(TiXmlElement * RootElement);		// 怪物组配置
	int InitItemOpeGroupCfg(TiXmlElement * RootElement);		// 道具组配置
	int InitCommonRewardCfg(TiXmlElement * RootElement);		// 通常任务奖励
	int InitAllTaskInforCfg(TiXmlElement * RootElement);		// 所有任务配置
	int InitOtherCfg(TiXmlElement * RootElement);				// 其他配置
	int InitSpecialRewardsCfg(TiXmlElement * RootElement);		// 根据other表任务id发放不同的的职业对应的奖励

	int InitTeamChallengeRewardCfg(TiXmlElement* RootElement);	// 

	int InitAdventureGetCfg(TiXmlElement * RootElement);		// 奇遇任务激活方式配置
	int InitAdvTaskOtherCfg(TiXmlElement * RootElement);		// 奇遇任务其它配置
	int InitAdvTaskIDGroupCfg(TiXmlElement * RootElement);		// 奇遇任务组

	int InitBossChapterBossCfg(TiXmlElement* RootElement);		// Boss章节Boss配置
	int InitBossChapterChapterCfg(TiXmlElement* RootElement);	// Boss章节章节配置

	int InitBountyTaskOtherCfg(TiXmlElement* RootElement);
	int InitBountyTaskLevelLimitCfg(TiXmlElement* RootElement);
	int InitBountyTaskTurnTableCfg(TiXmlElement* RootElement);
	int InitBountyTaskTurnTableGroupCfg(TiXmlElement* RootElement);
	int InitNextBountyTaskCfg(TiXmlElement* RootElement);
	int InitFirstBountyTaskCfg(TiXmlElement* RootElement);
	int InitBountyTaskFindCfg(TiXmlElement* RootElement);
	int InitBountyTaskFindRewardCfg(TiXmlElement* RootElement);
	int InitBountyFirstTaskCfg(TiXmlElement* RootElement);
	int InitAnecdoteTaskCfg(TiXmlElement* RootElement);
	int InitAnecdoteTaskFinalRewardCfg(TiXmlElement* RootElement);
	int InitAnecdoteTaskMonsterGroupCfg(TiXmlElement* RootElement);
	int InitAnecdoteOtherCfg(TiXmlElement* RootElement);
	int InitAnecdoteBigEventRwardCfg(TiXmlElement* RootElement);
	int InitAnecdoteSpecialCfg(TiXmlElement* RootElement);

	int InitConditionItemCfg(TiXmlElement* RootElement);
	int InitBountyTaskFinishRewardCfg(TiXmlElement* RootElement);

	int InitPrestigeTaskVillageCfg(TiXmlElement* RootElement);
	int InitPrestigeTaskPrestigeGroupCfg(TiXmlElement* RootElement, PrestigeGroupCfgMap& map);
	int InitPrestigeTaskCfg(TiXmlElement* RootElement);
	int InitPrestigeTaskStarCfg(TiXmlElement* RootElement);
	int InitPrestigeTaskOtherCfg(TiXmlElement* RootElement);

	void InsertPrevTask(TaskID prec_task, TaskID task_id);		// 插入前置新增任务映射
	void InsertNextTask(TaskID prec_task, TaskID task_id);		// 插入后置新增任务映射
	void InsertCuLvTask(UInt16 min_level, TaskID task_id);		// 插入等级新增任务映射
	void InsertProfTask(int base_prof_type, TaskID task_id);	// 插入职业进阶任务映射
	void InsertTaskType(int task_type, TaskID task_id);			// 插入任务类型集合映射


	bool CheckMainTask(std::string& error_msg);
	int CheckPrevNextTask();
	void SetPetFeiShengFirstTask();
	
	const PrestigeTaskPrestigeGroupCfg* GetPrestigeRewardGroupCfg(int reward_group, const PrestigeGroupCfgMap& map);
	bool CheckPrestigeTaskNumInSameGroup(int village, int group, int max_num);

	UInt32 m_record_task;										// 需要写入任务完成数据记录的任务数量
	TaskInfomaConfig *m_task_list[MAX_UINT16];					// 所有任务

	std::map<int, std::vector<int> > m_monster_group;			// 怪物组配置
	std::map<int, std::set<ItemID> > m_item_group;				// 道具组配置

	std::map<TaskID, std::vector<CommonTaskReward> > m_common_task_reward_cfg;	// 通常任务奖励配置

	// —————————————————————————————————————————————————————————————————————————————————————————————————
	struct KeyPair
	{
		KeyPair(UInt16 p_main = 0, UInt16 p_seco = 0)
		{
			principal = p_main;
			secondary = p_seco;
		}

		UInt16 principal;		// 主要大类
		UInt16 secondary;		// 次要小类

		bool operator < (const KeyPair & cobj) const
		{
			if (this->principal != cobj.principal) return this->principal < cobj.principal;
			if (this->secondary != cobj.secondary) return this->secondary < cobj.secondary;

			return false;
		}

		bool operator == (const KeyPair & cobj) const
		{
			return ((this->principal == cobj.principal) && (this->secondary == cobj.secondary));
		}
	};

	std::map<TaskID, std::set<TaskID> > m_prev_task;			// 前置任务 任务链映射
	std::map<TaskID, std::set<TaskID> > m_next_task;			// 后置任务 任务链映射
	std::set<TaskID>* m_acpb_task[MAX_ROLE_LEVEL + 1];			// 当前等级新增的任务映射
	std::set<TaskID>* m_prof_task[PROFESSION_TYPE_NUM];			// 职业base_type所专属的任务
	std::set<TaskID>* m_task_type_list[TASK_TYPE_MAX];			// 任务类型的任务ID集合
	std::map<int, DefeatMonsGroups> m_defeat_mon_group;			// 消灭组ID

	AdventruesOthers m_adv_task_other;
	std::map<ParamSection, std::vector<AdvTaskActivates> > m_adv_task_active_map;	// 奇遇任务激活条件：键 - 等级区间；
	std::vector<AdvTaskActivates>* m_level_adv_task_list[MAX_ROLE_LEVEL + 1];
	std::map<ItemID, std::map<ParamSection, TaskID> > m_item_act_task_map;	// 道具激活任务对应配置：键 - 道具ID、等级区间；值 - 任务ID；
	std::map<TaskID, int> m_adv_task_time_limit;				// 奇遇任务时限：键 - 任务组；值 - 时间限制（分：min）
	std::map<int, AdvTaskGroupList> m_adv_task_group_map;			// 奇遇任务组

	BossChapterBossCfgMap m_boss_chapter_boss_cfg_map;
	ChapterBossList m_boss_chapter_chapter_boss_list[MAX_BOSS_CHAPTER];
	BossChapterChapterRewardCfg m_boss_chapter_chapter_reward_cfg_list[MAX_BOSS_CHAPTER];

	BountyTaskOtherCfg m_bounty_task_other_cfg;
	BountyTaskList m_all_bounty_task_list;
	std::vector<BountyTaskTurnTableCfg> m_bounty_turn_table_cfg_list;
	NextBountyTaskMap m_next_bounty_task_map;
	TaskID m_first_bounty_task_list[PROFESSION_TYPE_NUM];
	std::map<int, int> m_bounty_first_task_cfg;		// key-seq,value-任务ID
	std::vector<BountyTaskFinishRewardCfg> m_bounty_task_finish_reward_cfg;	// 赏金任务十环完成奖励

	std::map<int ,std::vector<BountyTaskFindRewardCfg> > m_bounty_task_find_reward_cfg;	// 找回奖励组：键 - group_id
	std::map<int, std::map<int, BountyTaskFindCfg> > m_bounty_task_find_cfg;			// 奖励找回表：键 - 等级区间、找回类型
	//std::map<int, BountyTaskFindCfg> m_seq_to_btfcfg;
	std::vector<BountyTaskFindCfg> m_btfcfg_v;

	LimitDropItemMap m_limit_drop_item_task_hash;

	TaskOtherCfg m_task_other_cfg;

	AnecdoteInitialTaskCfg m_anecdote_initial_task_cfg_list[ANECDOTE_TASK_TYPE_MAX];
	std::map<int, int> m_task_anecdote_type_hash; // key: taskID, value: anecdote_task_type
	AnecdoteRewardCfg m_anecdote_reward_cfg[ANECDOTE_TASK_TYPE_MAX];
	AnecdoteMonsterGroupCfgMap m_anecdote_monster_group_cfg_map;

	//主线机器人初始任务
	ItemID m_robot_mission_begin_task;

	std::set<ItemID> m_condition_item_set;

	PrestigeTaskVillageCfg m_prestige_task_village_cfg_list[PRESTIGE_VILLAGE_MAX_NUM];
	PrestigeGroupCfgMap m_prestige_group_cfg_map;
	PrestigeGroupCfgMap m_end_prestige_group_cfg_map;
	PrestigeTaskCfgMap m_prestige_task_cfg_map;
	PrestigeTaskGroupVec m_prestige_task_group_vec[PRESTIGE_VILLAGE_MAX_NUM];
	std::vector<PrestigeTaskStarCfg> m_prestige_task_star_cfg;
	std::map<int, int> m_prestige_task_star_rate_1_map;
	std::map<int, int> m_prestige_task_star_rate_2_map;
	PrestigeTaskOtherCfg m_prestige_task_other_cfg;

	AnecdoteOtherCfg m_anecdote_other_cfg;
	std::vector<AnecdoteBigEventRewardCfg> m_anecdote_big_event_reward_cfg_vec[ANECDOTE_BIG_EVENT_REWARD_VERSION + 1];

	std::map<int, int> m_pet_fei_sheng_first_task_id;

	std::map<int, std::vector<ItemConfigData> > m_special_rewards_cfg;	// 特殊奖励,key-职业id

	std::vector<AnecdoteSpecialCfg> special_anecdote_cfg;

	std::map<int, TeamChallengeRewardCfg> team_challenge_reward_map;
};

#endif // __TASKPOOL_HPP__


