#ifndef __ANECDOTE_TASK_HPP__
#define __ANECDOTE_TASK_HPP__

#include "servercommon/anecdotetaskdef.hpp"
#include "servercommon/taskdef.h"
#include "gamedef.h"

class RoleModuleManager;
class AnecdoteTask		// ��������
{
public:
	AnecdoteTask();
	~AnecdoteTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Update(unsigned int now_second);

	void OnTalkToNpc(int npc_id);
	void OnCommitItem(int task_id, int item_id, int num);
	void OnCommitItemGroup(TaskID task_id, int item_num, ARG_IN const ItemContainer* item_list);	
	void OnChallengeAnecdoteSucc(int anecdote_type);

	void Init(RoleModuleManager* mgr, const AnecdoteTaskParam& param, const AnecdoteTaskCommonParam& common_param, const SpecialAnecdoteParam& special_param);
	void GetInitParam(AnecdoteTaskParam* out_param, AnecdoteTaskCommonParam* out_common_param, SpecialAnecdoteParam* out_special_param);

	void OnLogin(); // ��¼ʱ�Զ���������δ�ύ�����񲢷��ʼ�����
	void OnRoleLevelUp(); // ����ʱ�жϣ���ȡ�ɽӵ���������
	void OnRoleClaimReward(int anecdote_task_type);
	void OnRoleFetchBigEventReward(int reward_seq);

	void OnFinishTask(int anecdote_task_type); // ���淨���е���
	void OnFailTask(int anecdote_task_type);   // ���淨���е���

	void OnBattleFail(int monster_group_id);   // ս��ʧ�� ��ս�����Զ�����ã�
	void OnBattleSucc(int monster_group_id);   // ս���ɹ� ��ս�����Զ�����ã�

	void AcceptTask(int task_id);
	void CommitTask(int task_id);

	void GmResetAnecdote(int anecdote_type);

	void OnProgressChange(int task_id, int old_value, int new_value);
	void SendAllInfo();

	void SpecialSet(int req_type, int value);

	bool IsFinishTask(int anecdote_task_type);
	bool IsAcceptOrFinish(int anecdote_task_type);		//�����Ƿ��ѽӻ����

	inline const AnecdoteTaskCommonParam& GetCommonParam() { return m_common_param; }
	inline AnecdoteTaskCommonParam& GetMutableCommonParam() { return m_common_param; }

	void CheckSpecialTask(int task_type, int task_idx);
	void StarSpecialTask(int task_type, int task_idx);
	void GrantSpecialReward(int task_type, int task_idx);
	void SendSpecialAnecdoteTaskInfo();
	void SendSpecialAnecdoteTaskSingleInfo(int task_type);
private:
	void GiveFinalReward(int anecdote_task_type, bool is_by_mail);	// ��������������
	void SendTaskInfoChangeNotice(int anecdote_task_type);
	void SendTaskCompleteNotice(int anecdote_task_type, int task_id);

	void SetTianJueDuiIndex(int index) { m_common_param.tianxiajuedui = index; }

	void CheckAutoGiveReward();
	void CheckAutoAcceptTask();
	bool CheckTaskStatus(int task_id);
	int CountFinishAnedoteTaskNum() const;
	void CheckBigEvent();
	void CheckBigEventExpireAutoGiveMail();
	void SendBigEventRewardMail();
	void GetUnfetchBigEventRewardSeqs(ARG_OUT std::vector<int>& seqs);
	bool HasFetchBigEventReward(int seq);
	bool HasUnfetchBigEventReward();

	void SendTaskInfo();
	void SendSpecialInfo();
	void SendBigEventInfo();
	bool IsBigEventOpenNow();

	RoleModuleManager* m_mgr;

	AnecdoteTaskParam m_param;
	AnecdoteTaskCommonParam m_common_param;
	SpecialAnecdoteParam m_special_param;
};

#endif