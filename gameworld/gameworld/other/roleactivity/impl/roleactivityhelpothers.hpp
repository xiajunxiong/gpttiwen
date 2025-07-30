#ifndef __ROLE_ACTIVITY_HELP_OTHERS_HPP__
#define __ROLE_ACTIVITY_HELP_OTHERS_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahelpothersparam.hpp"
#include "protocol/randactivity/msgrahelpothers.hpp"

class RoleActivityHelpOthers : public RoleActivity
{
public:
	RoleActivityHelpOthers(int type);
	virtual ~RoleActivityHelpOthers();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void AddTaskProgress(int num);

private:
	void SendInfo();
	void OnStart(); 
	void OnAccept();
	void OnDelivery(int column, int index);
	void OnTalkNPC();
	void OnCommitPet(int pet_index);
	void OnBattleFinish();

	void OnCommitTask();	

	RAHelpOthersParam m_param;
};


#endif
