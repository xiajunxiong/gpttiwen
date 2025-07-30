#ifndef __ROLE_ACTIVITY_YUN_ZE_BLOOMING_RECORD_HPP__
#define __ROLE_ACTIVITY_YUN_ZE_BLOOMING_RECORD_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayunzebloomingrecordparam.hpp"
#include "protocol/randactivity/msgrayunzebloomingrecord.hpp"

class RoleActivityYunZeBloomingRecord : public RoleActivity
{
public:
	RoleActivityYunZeBloomingRecord(int type);
	virtual ~RoleActivityYunZeBloomingRecord();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnLoginSendInfo();
	virtual void Update(time_t now_second);


private:
	void RASendInfo();
	void OnStart();
	void RecordIntegral(int integral);
	void SendReward(int reward_id);


	RAYunZeBloomingRecordParam m_param;
};


#endif