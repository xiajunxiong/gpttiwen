#ifndef __ROLE_ACTIVITY_YUAN_QI_SPEED_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_SPEED_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayuanqispeedparam.hpp"
#include "protocol/randactivity/msgrayuanqispeed.hpp"

class RoleActivityYuanQiSpeed : public RoleActivity
{
public:
	RoleActivityYuanQiSpeed(int type);
	virtual ~RoleActivityYuanQiSpeed();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void Update(time_t now_second);
 	virtual void OnLoginSendInfo();
 	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
 	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
 	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void StarPlay(int seq);//开始游戏
	void Achiment(bool is_true);//是否胜利到达
	void GetFirstReward(int seq);
private:
	void EndPlay(bool is_win);
	void RASendInfo();
	void RASendPlayInfo(int game_type);
	void OnJoinActivity();

	RAYuanQiSpeedParam m_param;
};

#endif