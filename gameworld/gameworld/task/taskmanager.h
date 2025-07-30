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
		ADV_TYPE_DEFEAT = 0,//Ĭ��
		ADV_TYPE_TALK = 1,	//�Ի�
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

	bool CanAcceptTask(TaskID task_id, bool notify = false, ARG_OUT unsigned int* wait_timestamp = NULL);	// ֻ���������������������״̬
	bool CanCommitTask(TaskID task_id, bool notify = false, bool is_from_protocol = false);	// ֻ���������������������״̬
	bool CanCommitTaskTeam(TaskID task_id, bool notify = false);// �������������״̬

	// ��������
	void AcceptTask(TaskID task_id);		// ��������
	void CommitTask(TaskID task_id);		// �ύ����
	void GiveupTask(TaskID task_id);		// ��������
	void EraseTask(TaskID task_id);			// ֱ��ɾ����������ÿ��ˢ�µ���������Ҫ��ǰһ���ɾ����
	void DemandTask(TaskID task_id);		// ��ѯ����
	void SetupTrace(int task_num, unsigned int * param_list);	// ��������׷��
	void TaskSkipBattle(TaskID task_id);

	bool CheckTaskByType(int task_type, int opera_type, int param);//�����������Ͳ�ѯ�������

	// ����ִ������
	void SkipTask(TaskID task_id);															// ��������

	void OnCommitMoney(TaskID task_id);											// �ύ����
	void OnTalkWithNPC(TaskID task_id, UInt16 npc_id);										// �� NPC ��̸
	void OnKillMonster(int monster_id);														// ����һֻ����
	void OnPassFB(int fb_type, int count = 1);																// ͨ��ĳ������
	void OnPassCrystalFB(int crystal_fb_seq);
	void OnEnterScene(int scene_id);														// ����ĳ������
	void OnLoadProgress(int gather_id);														// �ɼ�ĳ������
	void OnGiveProperty(TaskID task_id, int item_id, int num);								// ����һ�ֵ��� - ����һ�����ύ
	void OnGiveProGroup(TaskID task_id, const std::set<ItemContainer> & com_item_list);		// ����һ����� - ����һ�����ύ
	bool OnCommitProperty(TaskID task_id, int item_id, int num);							// �ύһ�ֵ��ߣ��ύ����ʱ������
	void OnUseProperty(int item_id, int num);												// ʹ��һ������
	void OnBuyProperty(int item_id, int num);												// ����һ������
	void OnPlayGame(int game_type, int param1, int param2);									// ��ĳ��С��Ϸ
	void OnPlayCutscene(int cutscene_id);													// ���ž��鶯��
	void OnPlayMedalTaskVideo(int video_id);												// �����������񶯻�
	void OnTrapCreature(int pet_id);														// ��׽һֻ����
	void OnGetACreature(int pet_id);														// ���һֻ����
	void OnActivePartner(int partner_id);													// ����һ�����
	void OnPartnerUpLevel(int partner_id, int old_level, int new_level);					// �������
	void OnLearnSkill();																	// ѧϰ���⼼��
	void OnVictBattle(int group_id);														// ս��ʤ�� / ���ܹ�����
	void OnJoinBattle(int group_id);														// ����ս��
	void OnOpenAChest(int chest_id);														// ��һ������
	void OnItemChange(int item_id);															// ���������ı�
	void PlayColosseum();																	// ��ս���񱦿�
	void OnTransactionTask();																// ˿��֮·���
	void OnPetRelease();																	// �������
	void OnPetCompose();																	// ����ϳ�
	void OnEnterFB(int fb_type, int seq);
	void OnPassCourageChallenge();															// ��ħ֮��ͨ��
	void OnPassDreamNotesChallenge(int level);												// ��Ԩ��¼ͨ��
	void OnFinishBountyTask(int count);														// ����ͽ�����
	void OnFinsihResearchTask(int count);													// ��������ħ
	void OnPassMaterial(int chapter_id);													// ͨ�����龳
	void OnPetMonopolyCatchPet();			
	void OnPassLunHuiWangChuan();															// ͨ���ֻ�����
	void OnPassKillMiYiBoss(int seq);														// ��ɱԶ��BOSS
	void OnAddPrestige(int village_id, int level);											// ��ׯ�����ȼ�����
	void OnCollectionTuJianActive();														// ��ׯ�����ȼ�����
	void OnFaBaoLevelChange();																// ��ׯ�����ȼ�����
	void OnFinishAnedoteGame(short anedote_type);											// �������
	void OnGodPrintWish();																	// ����
	void OnOpenOrCloseYinYangEye(int open_mark);											// ����������
	void OnElementRestraintGame(bool is_win);												// Ԫ�ؿ���
	void OnDestructionFaZhen();																// �ƻ�����
	void OnFixFaZhen();																		// �޸�����
	void OnXiuXing();																		// ����
	void OnJoinGuild();																		// �������
	void OnCommitTreasureTask(int count);													// ��ɾ����ر�
	void OnFinishTreasureTask();															// ���һ��ľ����ر�
	void OnFinishBountyToday();																// ���һ����ͽ�����
	void OnRoleFightWildBoss();															
	void OnRoleRealmBreakFight(int fight_seq);												// ͨ������֮��
	void OnRoleStrParnter(int partner_id);													// ǿ�����
	void OnRoleParnterBreak(int partner_id);												// ������(ֻ��С����)
	void OnCommitPet(int task_id, int pet_index);

	bool CheckTeamForTask(TaskID task_id, bool notify = true);		// ������սǰ���һ�ζ���״̬ - ע�⣬�������������Ƿ��ж�Ӧ����
	bool CheckRoleHasTask(TaskID task_id);							// �����������Ƿ��ж�Ӧ����

	void OnSetPartnerUpgradeTaskBuyFlag() { m_task_param.is_partner_upgrade_task_buy_flag = 1; }
	bool IsPartnerUpgradeTaskBuyFlag() { return 0 != m_task_param.is_partner_upgrade_task_buy_flag; }

	void SendSummaryTaskList();
	void SendWaitingTaskList();

	void OnRoleLevelUp();		// ��ҵȼ����������㵱ǰ�ȼ������ɽ��������̭����
	void OnProfPromote();		// ���ְҵ���ף����㵱ǰְ�������ɽ��������̭����
	void OnTeamConvert();		// ��Ҷ����������㵱ǰ����µ��ѽ�����ķ���״̬

	void OnTaskBattleFinish(int group_id, int battle_mode);

	// GM ����
	void GmTaskManagerOrder(int op_type, int param1, int param2);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool OnAcceptTask(TaskID task_id, bool notify = true);		// ȫ��ִ���߼� - ��ȡ����
	void OnCommitTask(TaskID task_id);		// ȫ��ִ���߼� - �ύ����
	void OnGiveupTask(TaskID task_id, int type = TASK_MANAGER_GIVE_UP_TYPE_DEFAULT);		// ȫ��ִ���߼� - ��������

	void ExecuteATask(TaskID task_id, int prog_num);	// ������ָ������ ID ���������� �� NPC ��̸���ύ���ߡ��ύ������ - �˺�������ʵ�ʿ۳�����
	void ExecuteATask(int condition, int p1, int p2);	// ���ڲ�ָ������ ID �������������һ������ͬʱ�������ж�Ӧ����
	
	// =========================================================== �������� =========================================================== //
	void AdventureTaskTriggerJudge(int adv_type = ADV_TYPE_DEFEAT);
	bool OnUseItemToTriggerAdventureTask(int item_id);
	//------
	bool HasMainTask();
	TaskID GetCurMainTaskID();
	void OnRoleLogin();						//��������Զ���ȡ����
	void AcceptPatrolTask();							//��ȡѲ������  
	void AddPatrolTaskProgress();
	void SendPatrolTaskInfo();

	//---------------��������������---------------------
	void ClearPetFeiShengTask();
	std::vector<int>  GetPetFeiShengTask();
	bool HasPetFeiShengTask();

	// =================================================== �������� =========================================
	void FlyUpTask(int task_id);//��ȡ��������
	void OnRoleFlyUpFinish();	//��ɫ��ɷ���
	void OnPartnerSoarFinish();
	void OnSendPartnerSoarInfo();

	int GetCoinByLevel(int _role_level);					//���ݵȼ���ȡ����
	void CommitPatrolTask();							//��ȡ���� �����ύ

	//---------------�����������---------------------
	void ClearRoleGuildTask();
	bool HasRoleGuildTask();

	TaskWenXiangLouFbParam & GetWenXiangLouParam() { return m_wenxianglou_param; }

	void GmResetTask(TaskID task_id);
	void GmClearAllTask();
private:
	void TaskDataToProtocol(TaskInfoPro * out_info, const TaskInfo & t_idx);		// ���ݿ����� -> Э����Ϣ ת����

	enum ACCEPTABLE_LIST_RECALC_REASON
	{
		ACB_RECALC_INITIALIZES = 0,		// ��ʼ��
		ACB_RECALC_ON_LEVEL_UP = 1,		// �ȼ�����
		ACB_RECALC_TASK_COMMIT = 2,		// �����ύ param1: task_id
		ACB_RECALC_PROF_CHANGE = 3,		// ְҵ����
		ACB_RECALC_TASK_TYPE = 4,		// �Ե����������� param1: task_type
		ACB_RECALC_ON_DAY_CHANGE = 5,	// ����ʱ�жϵȴ��б�������Щ����ɽ�
	};

	// ������������񡢸ı�ְҵ��
	bool CheckTaskIsLegal(TaskID task_id);			// ����ѽ��б���������Ƿ�Ϸ�
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
	void RefreshTaskProgNum(int task_id, bool is_notify);		// �˺����ڽ��������ʱ��ִ��һ�Σ���ע��ĳЩ�����������Ǵ����������ʼ����
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