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

static const int TASK_PREDECESSOR_COUNT_MAX = 3;		//!< ���������ǰ��������������
static const int TASK_REWARD_LIST_COUNT_MAX = 5;		//!< ���������ܻ�õĵ��߽������������
static const int TASK_LIMIT_DROP_ITEM_NUM_MAX = 5;		//!< ���������������Ƶ�����Ʒ����

struct TaskInfomaConfig		// ������Ϣͷ�ļ� - ͨ��
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

	///////////////////// ������Ϣ /////////////////////
	int task_id;				// ����ID
	int task_ver;				// ����汾
	int task_type;				// ��������
	int team_limit_type;		// �������
	int task_param1;
	int task_param2;
	int task_param3;

	///////////////////// ��ȡ���� /////////////////////
	int level_min;				// ��͵ȼ�
	int level_max;				// ��ߵȼ�
	int prof_limit_min;			// ְҵ���ƣ���ֵ
	int prof_limit_max;			// ְҵ���ƣ���ֵ
	int pre_task_id;			// ǰ������ 0 - ����ǰ��
	int pre_task_id2;			// ǰ������2
	int nex_task_id;			// �������� 0 - �������
	int nex_task_id2;			// ��������2
	int accept_npc;				// ��ȡ����� NPC
	int open_server_day;		// ��������

	///////////////////// ����Ŀ�� /////////////////////
	int task_condition;			// �������
	int timer_limit;			// ������ʱ - 0 ��ʾ ����ʱ����λ����(s)
	int param1;					// Ŀ�� NPC / ������ / ����ID / ʰȡ��ID
	int param2;					// ���� / ʱ�䳤��(s)
	int commit_npc;				// �ύ����� NPC
	bool can_auto_commit;		// �Ƿ�����Զ��ύ
	bool can_auto_catch;		// �Ƿ�����Զ���ȡ

	///////////////////// ������� /////////////////////
	int is_share;				// �Ƿ����ȫ��
	int giveup[GIVEUP_TYPE_MAX];// ���������Ϊ����

	int limit_drop_count;
	ItemID limit_drop_item[TASK_LIMIT_DROP_ITEM_NUM_MAX];	// ������󿪷ŵ�����ƷID����������رյ��䣩

	bool is_add_robot;			// �Ƿ�ӻ�����
	bool can_skip_battle;		// �Ƿ������ս��(����Ϊսʤ�˹�����)
	int skip_battle_cost_coin;	// ����ս��ͭ�Һķ�

	int battle_mode;			// ����ս����֮�󴥷����ս��ģʽ����

	bool is_skip;
	int skip_cost_type;
	int skip_cost_num;

	///////////////////// ������ /////////////////////
	// virtual : ÿ�������ý������߼���ͬ��Ӧ���������
	int reward_allot;			// ������������

	///////////////////// ��Ա���� /////////////////////

	//////////////////// ��������� ////////////////////
	bool operator == (const TaskInfomaConfig & cobj) const
	{
		return (this->task_id == cobj.task_id);
	}

	bool operator < (const TaskInfomaConfig & cobj) const
	{
		return (this->task_id < cobj.task_id);
	}
};

// ������������������������������������������������������������������������������������������������������������������������ ������ ������������������������������������������������������������������������������������������������������������������������ //
struct CommonTaskReward		// ͨ������������
{
	CommonTaskReward() : allot_min(0), allot_max(0), coin(0), immortal(0), gold(0), exp(0), pet_exp(0), repu_city(0), repu_numb(0), power(0),
		reward_count(0) { memset(reward_list, 0, sizeof(reward_list)); }

	int allot_min;		// ��������ֵ - ����������Ϊ 0 ʱ���������ûɶ��
	int allot_max;		// ��������ֵ - ����������Ϊ 0 ʱ���������ûɶ��

	int coin;			// ��ұ���
	int immortal;		// �ɱұ��� - ��ʵ���� ��Ԫ
	int gold;			// Ԫ������
	int exp;			// ����ֵ
	int pet_exp;		// ���ﾭ�� - �Ӹ���ս�ĳ�����û�г�ս�ĳ���Ͳ���

	int repu_city;		// �������ڵ�
	int repu_numb;		// ������ֵ
	int power;			// �����ؾ�����

	int reward_count;	// ��������
	ItemConfigData reward_list[TASK_REWARD_LIST_COUNT_MAX];	// ������Ʒ
};

// ������������������������������������������������������������������������������������������������������������������������ �������� ������������������������������������������������������������������������������������������������������������������������ //
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

	int limit_times_daily;		// ÿ���������Ĵ���
	int basic_trigger_rate;		// �����������ʣ��ٷֱȣ�%��
	int talk_rate;				// �Ի���������
	int task_level_limit;		// �ȼ�����
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

	int first_task_level;	// �״��������������߼������ȼ�����
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
	int anecdotes_recommend_task_id;		//�Ƽ�����ID�����ж�����
	//int award_surface_point_task_id;		//�����û��������ID(����)
	//int award_surface_seq;					//�����û�������
	int career_rewards;						//���⽱���߼�������id
	int partner_upgrade_task_id;			//����������ID
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
	const CommonTaskReward * GetCommonTaskReward(int p_task_id, Role * p_role);		// ��ȡͨ�����񱨳�
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
	const std::vector<ItemConfigData>* GetBountyTaskFinishRewardCfg(int role_level) const;	// ��ȡ�ͽ�������ɽ���

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
	int InitDefeatsGroupCfg(TiXmlElement * RootElement);		// ����������
	int InitMonsterGroupCfg(TiXmlElement * RootElement);		// ����������
	int InitItemOpeGroupCfg(TiXmlElement * RootElement);		// ����������
	int InitCommonRewardCfg(TiXmlElement * RootElement);		// ͨ��������
	int InitAllTaskInforCfg(TiXmlElement * RootElement);		// ������������
	int InitOtherCfg(TiXmlElement * RootElement);				// ��������
	int InitSpecialRewardsCfg(TiXmlElement * RootElement);		// ����other������id���Ų�ͬ�ĵ�ְҵ��Ӧ�Ľ���

	int InitTeamChallengeRewardCfg(TiXmlElement* RootElement);	// 

	int InitAdventureGetCfg(TiXmlElement * RootElement);		// �������񼤻ʽ����
	int InitAdvTaskOtherCfg(TiXmlElement * RootElement);		// ����������������
	int InitAdvTaskIDGroupCfg(TiXmlElement * RootElement);		// ����������

	int InitBossChapterBossCfg(TiXmlElement* RootElement);		// Boss�½�Boss����
	int InitBossChapterChapterCfg(TiXmlElement* RootElement);	// Boss�½��½�����

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

	void InsertPrevTask(TaskID prec_task, TaskID task_id);		// ����ǰ����������ӳ��
	void InsertNextTask(TaskID prec_task, TaskID task_id);		// ���������������ӳ��
	void InsertCuLvTask(UInt16 min_level, TaskID task_id);		// ����ȼ���������ӳ��
	void InsertProfTask(int base_prof_type, TaskID task_id);	// ����ְҵ��������ӳ��
	void InsertTaskType(int task_type, TaskID task_id);			// �����������ͼ���ӳ��


	bool CheckMainTask(std::string& error_msg);
	int CheckPrevNextTask();
	void SetPetFeiShengFirstTask();
	
	const PrestigeTaskPrestigeGroupCfg* GetPrestigeRewardGroupCfg(int reward_group, const PrestigeGroupCfgMap& map);
	bool CheckPrestigeTaskNumInSameGroup(int village, int group, int max_num);

	UInt32 m_record_task;										// ��Ҫд������������ݼ�¼����������
	TaskInfomaConfig *m_task_list[MAX_UINT16];					// ��������

	std::map<int, std::vector<int> > m_monster_group;			// ����������
	std::map<int, std::set<ItemID> > m_item_group;				// ����������

	std::map<TaskID, std::vector<CommonTaskReward> > m_common_task_reward_cfg;	// ͨ������������

	// ��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	struct KeyPair
	{
		KeyPair(UInt16 p_main = 0, UInt16 p_seco = 0)
		{
			principal = p_main;
			secondary = p_seco;
		}

		UInt16 principal;		// ��Ҫ����
		UInt16 secondary;		// ��ҪС��

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

	std::map<TaskID, std::set<TaskID> > m_prev_task;			// ǰ������ ������ӳ��
	std::map<TaskID, std::set<TaskID> > m_next_task;			// �������� ������ӳ��
	std::set<TaskID>* m_acpb_task[MAX_ROLE_LEVEL + 1];			// ��ǰ�ȼ�����������ӳ��
	std::set<TaskID>* m_prof_task[PROFESSION_TYPE_NUM];			// ְҵbase_type��ר��������
	std::set<TaskID>* m_task_type_list[TASK_TYPE_MAX];			// �������͵�����ID����
	std::map<int, DefeatMonsGroups> m_defeat_mon_group;			// ������ID

	AdventruesOthers m_adv_task_other;
	std::map<ParamSection, std::vector<AdvTaskActivates> > m_adv_task_active_map;	// �������񼤻��������� - �ȼ����䣻
	std::vector<AdvTaskActivates>* m_level_adv_task_list[MAX_ROLE_LEVEL + 1];
	std::map<ItemID, std::map<ParamSection, TaskID> > m_item_act_task_map;	// ���߼��������Ӧ���ã��� - ����ID���ȼ����䣻ֵ - ����ID��
	std::map<TaskID, int> m_adv_task_time_limit;				// ��������ʱ�ޣ��� - �����飻ֵ - ʱ�����ƣ��֣�min��
	std::map<int, AdvTaskGroupList> m_adv_task_group_map;			// ����������

	BossChapterBossCfgMap m_boss_chapter_boss_cfg_map;
	ChapterBossList m_boss_chapter_chapter_boss_list[MAX_BOSS_CHAPTER];
	BossChapterChapterRewardCfg m_boss_chapter_chapter_reward_cfg_list[MAX_BOSS_CHAPTER];

	BountyTaskOtherCfg m_bounty_task_other_cfg;
	BountyTaskList m_all_bounty_task_list;
	std::vector<BountyTaskTurnTableCfg> m_bounty_turn_table_cfg_list;
	NextBountyTaskMap m_next_bounty_task_map;
	TaskID m_first_bounty_task_list[PROFESSION_TYPE_NUM];
	std::map<int, int> m_bounty_first_task_cfg;		// key-seq,value-����ID
	std::vector<BountyTaskFinishRewardCfg> m_bounty_task_finish_reward_cfg;	// �ͽ�����ʮ����ɽ���

	std::map<int ,std::vector<BountyTaskFindRewardCfg> > m_bounty_task_find_reward_cfg;	// �һؽ����飺�� - group_id
	std::map<int, std::map<int, BountyTaskFindCfg> > m_bounty_task_find_cfg;			// �����һر��� - �ȼ����䡢�һ�����
	//std::map<int, BountyTaskFindCfg> m_seq_to_btfcfg;
	std::vector<BountyTaskFindCfg> m_btfcfg_v;

	LimitDropItemMap m_limit_drop_item_task_hash;

	TaskOtherCfg m_task_other_cfg;

	AnecdoteInitialTaskCfg m_anecdote_initial_task_cfg_list[ANECDOTE_TASK_TYPE_MAX];
	std::map<int, int> m_task_anecdote_type_hash; // key: taskID, value: anecdote_task_type
	AnecdoteRewardCfg m_anecdote_reward_cfg[ANECDOTE_TASK_TYPE_MAX];
	AnecdoteMonsterGroupCfgMap m_anecdote_monster_group_cfg_map;

	//���߻����˳�ʼ����
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

	std::map<int, std::vector<ItemConfigData> > m_special_rewards_cfg;	// ���⽱��,key-ְҵid

	std::vector<AnecdoteSpecialCfg> special_anecdote_cfg;

	std::map<int, TeamChallengeRewardCfg> team_challenge_reward_map;
};

#endif // __TASKPOOL_HPP__


