#ifndef __ACTIVE_SYSTEM_HPP__
#define __ACTIVE_SYSTEM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activesystemdef.hpp"
#include "gameworld/protocol/msgother.h"

class RoleModuleManager;
class ActiveSystem
{
public:
	ActiveSystem();
	~ActiveSystem();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const ActiveSystemParam & param,
		const ActiveDegreeBaseParam & week_base_param,
		const ActiveDegreeTaskParam & week_task_param, 
		const ActiveTurntableParam & turntable_param);
	void GetInitParam(ActiveSystemParam * param,
		ActiveDegreeBaseParam * week_base_param,
		ActiveDegreeTaskParam * week_task_param,
		ActiveTurntableParam * turntable_param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();
	void OnWeekChange();
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void ActiveSystemOp(Protocol::CSActiveSystemReq *req);

	//�һؽ���
	bool OnFindReward(int find_type, int count);
	int GetCanFindCountByType(int find_type)const;

	void RoleLevelUp(int cur_level);
	void AddActiveDegree(int type, int count, const char* reason_str = "NULL");
	void OnFetchActiveReward(int index);
	void OnFetchActiveExtraReward(int type);
	void SendActiveInfo();
	void SendSigleActiveInfo(int active_id);
	void GmAddActiveDegree(int add_num);
	bool IsStartedActive(int type);

	//�����Ծ����
	void OnFetchSpecialActiveReward(int special_active_type);
	void OnSendSpecialActiveInfo();

	//�ܳ�
	//�ܳ�-������¼
	void AddWeekDegreeParam(int active_degree_type, int param);
	//�ܳ�-������ȡ
	void FetchWeekDegreeReward(int seq);
	void SendWeekInfo();
	//������
	void WeekDegreeDailyReset();
	void WeekDegreeWeekReset();
	
	void GmAddWeekActiveDegree(int add_num);
	void GmAddTurntableTime(int add_num);

	void OnCompleteTurntableTask(int task_type, int param);
	void OnLuckTurntable();
	void SendTurntableInfo();

	void OnAddActiveCheckGiveGuessCoin(int add_num);

	int GetActiveNum() { return m_param.integral_active; }
private:
	void CheckGiveGuessCoin();

	RoleModuleManager * m_module_mgr;
	ActiveSystemParam m_param;

	ActiveDegreeBaseParam m_week_base_param;
	ActiveDegreeTaskParam m_week_task_param;

	ActiveTurntableParam m_turntable_param;

	std::map<int, int> m_find_reward_map;		//�һؽ���ӳ�� key:find_type second:ACTIVE_SYSTEM_TYPE
};



#endif