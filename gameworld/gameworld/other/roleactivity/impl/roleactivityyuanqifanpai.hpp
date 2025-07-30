#ifndef __ROLE_ACTIVITY_YUAN_QI_FAN_PAI_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_FAN_PAI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

class RoleActivityYuanQiFanPai : public RoleActivity
{
public:
	RoleActivityYuanQiFanPai(int type);
	virtual ~RoleActivityYuanQiFanPai();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();


	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void Lottery();				// 翻牌
	void ReceiveTimes(int seq);	// 领取翻牌次数奖励
	void Exchange(int seq);		// 兑换
	void FetchDayReward();		// 领取每日奖励
private:
	void RASendInfo();

	RAYuanQiFanPaiParam m_param;
};






#endif