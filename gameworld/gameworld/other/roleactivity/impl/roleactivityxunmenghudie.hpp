#ifndef __ROLE_ACTIVITY_XUNMENG_HUDIE_HPP__
#define __ROLE_ACTIVITY_XUNMENG_HUDIE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raxunmenghudieparam.hpp"

class RoleActivityXunMengHuDie : public RoleActivity
{
public:
	RoleActivityXunMengHuDie(int type);
	virtual ~RoleActivityXunMengHuDie();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnComplete(bool is_win);

public:
	void RASendInfo();
	void RAGrab(ObjID obj_id, int is_win);
	void OnJoinActivity();

	RAXunMengHuDieParam m_param;
};

#endif	//__ROLE_ACTIVITY_QIYUAN_KONGMINGDENG_HPP__
