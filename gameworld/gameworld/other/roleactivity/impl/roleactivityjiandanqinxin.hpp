#ifndef __ROLE_ACTIVITY_JIAN_DAN_QIN_XIN_HPP__
#define __ROLE_ACTIVITY_JIAN_DAN_QIN_XIN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

class RoleActivityJianDanQinXin : public RoleActivity
{
public:
	RoleActivityJianDanQinXin(int type);
	virtual ~RoleActivityJianDanQinXin();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual bool IsFinishBonusCond(int layer_type = 0);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	void OnRAJianDanQinXinReward(int monster_group_id);
public:
	void SendRAJianDanQinXinInfo();
	void OnRAJianDanQinXinBattle(int monster_group_id);

	RAJianDanQinXinParam m_param;
};

#endif	//__ROLE_ACTIVITY_JIAN_DAN_QIN_XIN_HPP__
