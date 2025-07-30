#ifndef __MI_YI_HPP__
#define __MI_YI_HPP__

#include "servercommon/miyidef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class MiYi
{
public:
	MiYi();
	~MiYi();

	void Init(RoleModuleManager * mgr, const MiYiParam & param);
	void GetInitParam(MiYiParam * param);

	void OnWeekChange();
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	bool IsFirstKill(int seq,int which_monster);
	bool IsWeekKilled(int seq, int which_monster);			// 是否杀过
	bool IsWeekKilled(int monster_group_id);				// 是否杀过
	void SetKill(int seq, int which_monster);
	bool CheckLayerFinish(int seq);
	int GetCurLayerFinishNum(int seq);

	void OnLogin();
	void AddPassCount();
	void GmSetWeeklyPassCount(int pass_count);
	void GmSetDayPassCount(int pass_count);
	
	int GetWeekPassCount() { return m_param.weekly_pass_count; }
	int GetDayPassCount() { return m_param.day_pass_count; }
	bool CanGetXiaYiZhi();
	void SendInfo();
	void SendMiyiInfo();

	void FetchWeekFirstKillReward(int scene_id, int monster_npc_id);
	void SaoDang(int scene_id);

	void* operator new(size_t c);
	void operator delete(void* m);
private:
	RoleModuleManager * m_role_module_mgr;

	MiYiParam m_param;
};

#endif