#ifndef __ROLE_GUILD_TASK_HPP__
#define __ROLE_GUILD_TASK_HPP__

#include "servercommon/roleguildtaskdef.hpp"

class RoleModuleManager;



class RoleGuildTask
{
public:
	RoleGuildTask();
	~RoleGuildTask();

	void Init(RoleModuleManager * mgr, const RoleGuildTaskParam & param);
	void GetInitParam(RoleGuildTaskParam * param);
	void Update(time_t now_second);
	void UpdateRefresh(time_t now_second);
	void OnDayChange();

	void OnFinishTask(int task_id);
	void SendInfo();
	void SendChangeInfo(int reason,int task_index);
	int GetFinishTimes() const;

	void OnGiveUpTask(int task_id, int type);

	void * operator new(size_t c);
	void operator delete(void * m);

public:
	int ReqStartTask(int task_index);
	int ReqGiveUpTask(int task_index);
	int ReqBattle();
	int ReqSolicit();
	int ReqGive(int role_id);
	int ReqRefresh();
	int ReqMailNotice(bool is_all, int role_id);
	int ReqFetch();
private:
	RoleGuildTaskItem * GetCurrRoleGuildTaskItem();
	RoleGuildTaskItem * GetRoleGuildTaskItem(int task_index);

	RoleModuleManager * m_role_module_mgr;
	RoleGuildTaskParam m_param;

	time_t m_last_req_time;
};

#endif