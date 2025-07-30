#ifndef __ROLE_ACTIVITY_HAPPY_LOOK_HPP__
#define __ROLE_ACTIVITY_HAPPY_LOOK_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahappylookparam.hpp"

class RoleActivityHappyLook : public RoleActivity
{
public:
	RoleActivityHappyLook(int type);
	virtual ~RoleActivityHappyLook();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void RASendInfo();
	void OnStartGame(int index);					// 客户端通知服务器 游戏开始
	void OnEndGame(int is_succ);					// 客户端通知服务器 游戏成功完成
	void OnFetchReward();

	RAHappyLookParam m_param;
};

#endif	//__ROLE_ACTIVITY_HAPPY_LOOK_HPP__
