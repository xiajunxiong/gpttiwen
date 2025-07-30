#ifndef __ROLE_ACTIVITY_YUE_XI_LEI_CHONG_HPP__
#define __ROLE_ACTIVITY_YUE_XI_LEI_CHONG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityYueXiLeiChong : public RoleActivity
{
public:
	RoleActivityYueXiLeiChong(int type);
	virtual ~RoleActivityYueXiLeiChong();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnAddChongZhi(long long chongzhi);
	virtual void OnRAClose();
private:
	void ReceiveReward(int seq);
	// 活动结束补发未领取奖励
	void GiveUnFetchReward();

	void RASendInfo();
private:
	RAYueXiLeiChongParam m_param;
};

#endif