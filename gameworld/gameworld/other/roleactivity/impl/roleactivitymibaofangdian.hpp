#ifndef __ROLE_ACTIVITY_MI_BAO_FANG_DIAN_HPP__
#define __ROLE_ACTIVITY_MI_BAO_FANG_DIAN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ranationaldayparam.hpp"

class RoleActivityMiBaoFangDian : public RoleActivity
{
public:
	RoleActivityMiBaoFangDian(int type);
	virtual ~RoleActivityMiBaoFangDian();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	/*virtual void OnRAOpenOnlineSend();*/
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	void SendInfo(bool is_recal_phase = false);
	void SendSingleInfo(short seq, short index);
	void BuyItem(int seq);
	void SetOpenFlag();
	void CheckPhase();

	RAMiBaoFangDianParam m_param;
};

#endif