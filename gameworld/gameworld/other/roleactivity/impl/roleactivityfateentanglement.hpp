#ifndef __ROLE_ACTIVITY_FATE_ENTANGLEMENT_HPP__ 
#define __ROLE_ACTIVITY_FATE_ENTANGLEMENT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "protocol/randactivity/msgrafateentanglement.hpp"
#include "servercommon//roleactivity/rafateentanglementparam.hpp"

class RoleActivityFateEntanglement : public RoleActivity
{
public:
	RoleActivityFateEntanglement(int type);
	virtual ~RoleActivityFateEntanglement();

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

	RAFateEntanglementParam m_param;
};


#endif //  __ROLE_ACTIVITY_FATE_ENTANGLEMENT_HPP__
