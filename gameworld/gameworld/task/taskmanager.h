#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

#define task_role m_mgr->GetRole()
#define m_nowTime EngineAdapter::Instance().Time()

#include "gamedef.h"
#include "servercommon/taskdef.h"
#include "taskpool.h"

#include <set>
#include <map>

enum TASK_MANAGER_GIVE_UP_TYPE
{
	TASK_MANAGER_GIVE_UP_TYPE_DEFAULT,
	TASK_MANAGER_GIVE_UP_TYPE_UPDATE,
	TASK_MANAGER_GIVE_UP_TYPE_GUILD_TASK,
};

class RoleModuleManager;
class TaskManager
{
public:
	TaskManager();
	~TaskManager();
	enum ADV_TYPE
	{
		ADV_TYPE_DEFEAT = 0,//默认
		ADV_TYPE_TALK = 1,	//对话
	};

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_mgr; }

	void Init(RoleModuleManager * role_module_manager, const TaskParam & task_param, const TaskTraceParam & task_trace_param, const TaskWenXiangLouFbParam & task_wenxianglou);
	void GetTaskInitParam(TaskParam *param, TaskTraceParam *trace_param, TaskWenXiangLouFbParam *task_wenxianglou_param);
	void InitEnd();
	const TaskParam& GetTaskParam() const { return m_task_param; }

	void FinishAllPreTaskForMainQuest();
	
	void Update(time_t now_second);
	void OnTaskManagerDayChange();

	bool CanAcceptTask(TaskID task_id, bool notify = false, ARG_OUT unsigned int* wait_timestamp = NULL);	// 只检测自身条件，不检测组队状态
	bool CanCommitTask(TaskID task_id, bool notify = false, bool is_from_protocol = false);	// 只检测自身条件，不检测组队状态
	bool CanCommitTaskTeam(TaskID task_id, bool notify = false);// 检测队伍任务完成状态

	// 基本操作
	void AcceptTask(TaskID task_id);		// 接受任务
	void CommitTask(TaskID task_id);		// 提交任务
	void GiveupTask(TaskID task_id);		// 放弃任务
	void EraseTask(TaskID task_id);			// 直接删掉任务（用于每日刷新的任务需求，要把前一天的删掉）
	void DemandTask(TaskID task_id);		// 查询任务
	void SetupTrace(int task_num, unsigned int * param_list);	// 设置任务追踪
	void TaskSkipBattle(TaskID task_id);

	bool CheckTaskByType(int task_type, int opera_type, int param);//根据任务类型查询任务操作

	// 任务执行条件
	void SkipTask(TaskID task_id);															// 跳过任务

	void OnCommitMoney(TaskID task_id);											// 提交货币
	void OnTalkWithNPC(TaskID task_id, UInt16 npc_id);										// 与 NPC 交谈
	void OnKillMonster(int monster_id);														// 消灭一只怪物
	void OnPassFB(int fb_type, int count = 1);																// 通关某个副本
	void OnPassCrystalFB(int crystal_fb_seq);
	void OnEnterScene(int scene_id);														// 进入某个场景
	void OnLoadProgress(int gather_id);														// 采集某个对象
	void OnGiveProperty(TaskID task_id, int item_id, int num);								// 给予一种道具 - 必须一次性提交
	void OnGiveProGroup(TaskID task_id, const std::set<ItemContainer> & com_item_list);		// 给予一组道具 - 必须一次性提交
	bool OnCommitProperty(TaskID task_id, int item_id, int num);							// 提交一种道具（提交任务时触发）
	void OnUseProperty(int item_id, int num);												// 使用一个道具
	void OnBuyProperty(int item_id, int num);												// 购买一个道具
	void OnPlayGame(int game_type, int param1, int param2);									// 玩某种小游戏
	void OnPlayCutscene(int cutscene_id);													// 播放剧情动画
	void OnPlayMedalTaskVideo(int video_id);												// 播放试炼任务动画
	void OnTrapCreature(int pet_id);														// 捕捉一只宠物
	void OnGetACreature(int pet_id);														// 获得一只宠物
	void OnActivePartner(int partner_id);													// 激活一个伙伴
	void OnPartnerUpLevel(int partner_id, int old_level, int new_level);					// 伙伴升级
	void OnLearnSkill();																	// 学习任意技能
	void OnVictBattle(int group_id);														// 战斗胜利 / 击败怪物组
	void OnJoinBattle(int group_id);														// 参与战斗
	void OnOpenAChest(int chest_id);														// 打开一个宝箱
	void OnItemChange(int item_id);															// 道具数量改变
	void PlayColosseum();																	// 挑战古神宝库
	void OnTransactionTask();																// 丝绸之路完成
	void OnPetRelease();																	// 宠物放生
	void OnPetCompose();																	// 宠物合成
	void OnEnterFB(int fb_type, int seq);
	void OnPassCourageChallenge();															// 锢魔之塔通关
	void OnPassDreamNotesChallenge(int level);												// 梦渊笔录通关
	void OnFinishBountyTask(int count);														// 完成赏金任务
	void OnFinsihResearchTask(int count);													// 完成三界除魔
	void OnPassMaterial(int chapter_id);													// 通关玉虚境
	void OnPetMonopolyCatchPet();			
	void OnPassLunHuiWangChuan();															// 通关轮回忘川
	void OnPassKillMiYiBoss(int seq);														// 击杀远征BOSS
	void OnAddPrestige(int village_id, int level);											// 村庄声望等级提升
	void OnCollectionTuJianActive();														// 村庄声望等级提升
	void OnFaBaoLevelChange();																// 村庄声望等级提升
	void OnFinishAnedoteGame(short anedote_type);											// 完成奇闻
	void OnGodPrintWish();																	// 祈神
	void OnOpenOrCloseYinYangEye(int open_mark);											// 开关阴阳眼
	void OnElementRestraintGame(bool is_win);												// 元素克制
	void OnDestructionFaZhen();																// 破坏法阵
	void OnFixFaZhen();																		// 修复法阵
	void OnXiuXing();																		// 修行
	void OnJoinGuild();																		// 加入家族
	void OnCommitTreasureTask(int count);													// 完成九州秘宝
	void OnFinishTreasureTask();															// 完成一天的九州秘宝
	void OnFinishBountyToday();																// 完成一天的赏金任务
	void OnRoleFightWildBoss();															
	void OnRoleRealmBreakFight(int fight_seq);												// 通关心灵之境
	void OnRoleStrParnter(int partner_id);													// 强化伙伴
	void OnRoleParnterBreak(int partner_id);												// 伙伴晋升(只是小格子)
	void OnCommitPet(int task_id, int pet_index);

	bool CheckTeamForTask(TaskID task_id, bool notify = true);		// 在任务战前检查一次队伍状态 - 注意，不检测玩家身上是否有对应任务
	bool CheckRoleHasTask(TaskID task_id);							// 检测玩家身上是否有对应任务

	void OnSetPartnerUpgradeTaskBuyFlag() { m_task_param.is_partner_upgrade_task_buy_flag = 1; }
	bool IsPartnerUpgradeTaskBuyFlag() { return 0 != m_task_param.is_partner_upgrade_task_buy_flag; }

	void SendSummaryTaskList();
	void SendWaitingTaskList();

	void OnRoleLevelUp();		// 玩家等级提升，计算当前等级新增可接任务和淘汰任务
	void OnProfPromote();		// 玩家职业进阶，计算当前职阶新增可接任务和淘汰任务
	void OnTeamConvert();		// 玩家队伍变更，计算当前身份下的已接任务的放弃状态

	void OnTaskBattleFinish(int group_id, int battle_mode);

	// GM 命令
	void GmTaskManagerOrder(int op_type, int param1, int param2);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool OnAcceptTask(TaskID task_id, bool notify = true);		// 全队执行逻辑 - 接取任务
	void OnCommitTask(TaskID task_id);		// 全队执行逻辑 - 提交任务
	void OnGiveupTask(TaskID task_id, int type = TASK_MANAGER_GIVE_UP_TYPE_DEFAULT);		// 全队执行逻辑 - 放弃任务

	void ExecuteATask(TaskID task_id, int prog_num);	// 用于有指定任务 ID 的任务，例如 与 NPC 交谈、提交道具、提交道具组 - 此函数不会实际扣除道具
	void ExecuteATask(int condition, int p1, int p2);	// 用于不指定任务 ID 的任务，允许玩家一个操作同时满足所有对应任务
	
	// =========================================================== 奇遇任务 =========================================================== //
	void AdventureTaskTriggerJudge(int adv_type = ADV_TYPE_DEFEAT);
	bool OnUseItemToTriggerAdventureTask(int item_id);
	//------
	bool HasMainTask();
	TaskID GetCurMainTaskID();
	void OnRoleLogin();						//玩家上线自动接取任务
	void AcceptPatrolTask();							//接取巡逻任务  
	void AddPatrolTaskProgress();
	void SendPatrolTaskInfo();

	//---------------宠物飞升任务操作---------------------
	void ClearPetFeiShengTask();
	std::vector<int>  GetPetFeiShengTask();
	bool HasPetFeiShengTask();

	// =================================================== 飞升任务 =========================================
	void FlyUpTask(int task_id);//获取飞升任务
	void OnRoleFlyUpFinish();	//角色完成飞升
	void OnPartnerSoarFinish();
	void OnSendPartnerSoarInfo();

	int GetCoinByLevel(int _role_level);					//根据等级获取经验
	void CommitPatrolTask();							//领取奖励 任务提交

	//---------------家族任务操作---------------------
	void ClearRoleGuildTask();
	bool HasRoleGuildTask();

	TaskWenXiangLouFbParam & GetWenXiangLouParam() { return m_wenxianglou_param; }

	void GmResetTask(TaskID task_id);
	void GmClearAllTask();
private:
	void TaskDataToProtocol(TaskInfoPro * out_info, const TaskInfo & t_idx);		// 数据库数据 -> 协议信息 转换器

	enum ACCEPTABLE_LIST_RECALC_REASON
	{
		ACB_RECALC_INITIALIZES = 0,		// 初始化
		ACB_RECALC_ON_LEVEL_UP = 1,		// 等级提升
		ACB_RECALC_TASK_COMMIT = 2,		// 任务提交 param1: task_id
		ACB_RECALC_PROF_CHANGE = 3,		// 职业进阶
		ACB_RECALC_TASK_TYPE = 4,		// 对单个任务类型 param1: task_type
		ACB_RECALC_ON_DAY_CHANGE = 5,	// 切天时判断等待列表中有哪些任务可接
	};

	// 升级、完成任务、改变职业、
	bool CheckTaskIsLegal(TaskID task_id);			// 检查已接列表里的任务是否合法
	void CalcAcceptableTask(int op_type, int param1 = 0);
	void CalcAcceptableTaskRemoveIllegalTask(std::set<TaskID>& delete_list);
	void CalcAcceptableTaskOnInit();
	void CalcAcceptableTaskOnLevelUp(std::set<TaskID>& delete_list, std::set<TaskID>& change_list);
	void CalcAcceptableTaskOnTaskCommit(int task_id, std::set<TaskID>& change_list);
	void CalcAcceptableTaskOnProfChange(std::set<TaskID>& delete_list, std::set<TaskID>& change_list);
	void CalcAcceptableTaskOnRecalcTaskType(int task_type, std::set<TaskID>& delete_list, std::set<TaskID>& change_list);
	void CalcAcceptableTaskOnDayChange(std::set<TaskID>& delete_list, std::set<TaskID>& change_list);

	bool IsNoShowAtAcceptableList(const TaskInfomaConfig* task_cfg, ARG_OUT std::set<TaskID>* acceptable_list, ARG_OUT std::set<TaskID>* change_list);
	void SendSingleTaskDelete(int task_id, int reason);
	void SendTaskDeleteNotify(const std::set<TaskID> & task_id_list, int reason);
	void SendSingleTaskInfo(int task_id, int to_list_type);
	void SendChangeTaskList(const std::set<TaskID> & task_id_list, int to_list_type);
	void RefreshTaskProgNum(int task_id, bool is_notify);		// 此函数在接受任务的时候执行一次，请注意某些任务条件不是从任务接受起开始计数
	void RefreshTaskCanGiveUp(int label);
	void SendTaskCanGiveUpList(const std::set<ItemContainer> & can_give_up_list);
	void CheckAutoCommit(TaskID task_id, int commit_npc);
	void SendTaskTraceListInfo();
	void DoDelayCommit();

	void GmPreTaskForceFinish(TaskID task_id);
	void SetAllPreTaskFinish(TaskID task_id);
	void GmResetTaskImpl(TaskID task_id);
	void ExecuteATaskForMeetRequireCondition(int condition, int p1, int p2, const TaskInfomaConfig * task_cfg, TaskInfo& p_task);
	void ExecuteATaskForCondition(int condition, int p1, int p2, const TaskInfomaConfig * task_cfg, TaskInfo& p_task);

	bool IsTaskNeedWaiting(const TaskInfomaConfig* task_cfg, ARG_OUT unsigned int* wait_timestamp = NULL);
	void OnDayChangeResetWaitingMap();
	void CalcWaitingMapOnInit();

	static bool IsTaskTypeAcceptIgnoreLevelLimit(int task_type);

	RoleModuleManager * m_mgr;
	time_t				task_expire_check_time;

	TaskParam			m_task_param;
	TaskTraceParam		m_trace_param;

	TaskWenXiangLouFbParam	m_wenxianglou_param;

	std::set<TaskID>	m_acceptable_set;
	std::map<TaskID, unsigned int>	m_waiting_map;
	std::map<TaskID, UInt16> m_old_give_up_state;
	std::set<TaskID>	m_auto_commit_set;
	std::set<TaskID>	m_can_commit_set;
	
};

#endif