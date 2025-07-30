#ifndef __ROLE_ACTIVITY_SHAN_HAI_QUESTION_HPP__ 
#define __ROLE_ACTIVITY_SHAN_HAI_QUESTION_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaiquestionparam.hpp"
#include "global/randactivity/randactivityimpl/randactivityshanhaiquestion.hpp"

class RandActivityShanHaiQuestion;
class RoleActivityShanHaiQuestion : public RoleActivity
{
public:
	RoleActivityShanHaiQuestion(int type);
	virtual ~RoleActivityShanHaiQuestion();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void RandQuestion();
	void SendActivityStatusInfo(bool is_open);
	void SendInfo();
private:
	void DecideReward(int question_id,int answer_id);	
	int RandReward();	
	int RandRewardGroup(int reward_group);
	
	//下发奖励时调用
	void OnDecideReward();

	RAShanHaiQuestionParam m_param;
};




#endif
