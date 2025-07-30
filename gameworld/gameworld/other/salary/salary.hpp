#ifndef __SALARY_HPP__
#define __SALARY_HPP__

#include "servercommon/salarydef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "protocol/msgother.h"

class Role;
class RoleModuleManager;
struct CaptainSalaryCfg;
struct ChivalrousSalaryCfg;
struct TeamItem;
class Salary
{
public:
	Salary();
	~Salary();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const SalaryParam& param);
	void GetParam(SalaryParam* out_param);
	void CheckSalaryVersion();

	void OnWeekChange(bool type);
	void OnWeekChange();
	void SendDaySaveSalary(unsigned int old_dayid,unsigned int now_dayid);
	void SendTodaySalary();

	void OnBattleStartStatSalaryInfo(Protocol::SCSalaryBattleStatInfo & info, int monster_group_id);
	int CalculateChivalrousByMonsterGroupID(int monster_group_id);
	int CalculateChivalrousByTeamType(int team_type, const TeamItem& ti);

	int OnRoleWinBattle(int battle_mode, int monster_group_id, bool is_stat_calc_salary = false);
	int OnRoleClearFB(int fb_type, int param = 0, bool is_stat_calc_salary = false);

	int PreCalculateChivalrousOnWinBattle(int battle_mode, int monster_group_id, const TeamItem& ti); // 查询目标队伍可以加的侠义值
	int PreCalculateChivalrousOnClearFB(int fb_type, int param, const TeamItem& ti, int param1 = 0);
	int PreCalculateTeamUserCurBattleGetChivSalary(int monster_group_id, const TeamItem& ti);				//战斗之前预先计算玩家所能获得的侠义值(仅限任务类型)

	void OnRoleWinTaskMonsterGroup(int monster_group_id);
	void OnRolePlayDemonCave(int beat_wave);
	void OnRoleClearFBOneLayer(int fb_type, int param);
	void OnRoleKillWildBoss();
	void OnRoleKillMonsterGroup(int monster_group_id);
	
	void OnRoleTaskMonsterSucc(int task_id, int reward_type, int task_type);	//任务完成
	bool IsTaskMonsterGroup(int monster_group_id, int reward_type, int * task_id = NULL);		
	int CalcTeamUserCurBattleGetChivSalary(int monster_group_id);				//战斗之前预先计算玩家所能获得的侠义值(仅限任务类型)

	void SendAllInfo(int salary_type);
	void GmAddSalary(Int64 count);
	void GMChangeSalaryProgress(int reward_type,int param,int param2, int times);
	inline int GetHistoryChivalrous() const { return m_param.history_chivalrous; }
	int GetWeekTotalChivalrous();
	void OnFetchHistoryChivalrousReward(int reward_seq);
	void SendHistoryChivalrousInfo();
private:
	void SendUpdateInfo(int salary_type, int seq);
	void AddSalary(const CaptainSalaryCfg* cfg, int times = 1);
	void AddSalary(const ChivalrousSalaryCfg* cfg, int times = 1);

	SalaryParam m_param;
	RoleModuleManager* m_module_mgr;
};

#endif