#ifndef __ROLE_ACTIVITY_GIVE_SECRET_KEY_HPP__
#define __ROLE_ACTIVITY_GIVE_SECRET_KEY_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ragivesecretkeyparam.hpp"

class RoleActivityGiveSecretKey : public RoleActivity
{
public:
	RoleActivityGiveSecretKey(int type);
	virtual ~RoleActivityGiveSecretKey();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	void SetFristPassCrystalFb(int fb_seq);
	bool HasFristPassCrystalFb(int fb_seq);
public:
	void SendInfo();
	void GetReward(int seq);

	RAGiveSecretKeyParam m_param;
};

#endif	//__ROLE_ACTIVITY_GIVE_SECRET_KEY_HPP__
