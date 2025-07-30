#ifndef __RESEARCH_TASK_H__
#define __RESEARCH_TASK_H__

#include "servercommon/researchtaskdef.h"

#include <map>

class RoleModuleManager;
class ResearchTask
{
public:
	ResearchTask();
	~ResearchTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const ResearchTaskParam & param);
	void GetInitParam(ResearchTaskParam * param);

	void Update(time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResearchTaskRoleLogin();

	int GetYesterdayUnfinishTimes() { return m_param.residual_reward_back_times; }
	bool OnResearchTaskFindReward(int consume_type, int count);
	bool IsHaveResearchTask() { return 0 != m_param.task_info.task_id; }

	void OnRoleEnterTeam();					// ������飺��Ҽ�����������ӳ�������������ͬ������
	void OnRoleRejoinTeam();
	void OnTeamLeaderChange();				// TODO ��leader��ͬ�� Ҫ��Ҫͬ������
	void OnRoleLeaveTeam();					// �뿪���飺����뿪�����ֱ��ȡ������

	void OnGainTaskInfo(ResearchTaskInfo * task_con);			// ��ȡ������Ϣ
	void OnSyncTaskInfo(const ResearchTaskInfo & task_con);		// ͬ��������Ϣ

	void OnAcceptResearchTask();
	void OnCommitResearchTask();

	void OnOneKeyFinishZhenZhong();				// һ�����
	void OnDefeatMonsterGroup();		// �������й�
	void TeamLeaderSyncTaskAfterBattle();
	int GetTodayFinishCount(){ return m_param.today_finish_times; }			// ��ȡ������ɵĴ���
	bool HasFinishAllToday() const;
	void OnEnterScene(int scene_id);
	void SendResearchTaskInfo(bool is_new = false);

	bool IsCanOneClick();	// �Ƿ����һ�����
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	void GmResearchTaskHandle(int operator_type, int param);

private:
	void CheckCanGainAward(bool _is_onekey_finish = false);		// ���������
	bool RandTaskInfoOnce(int team_average_level, ResearchTaskInfo * out_con);	// ��ȡһ���������

	RoleModuleManager * m_mgr;
	ResearchTaskParam m_param;
};


#endif // !__SCHOOL_TASK_H__
