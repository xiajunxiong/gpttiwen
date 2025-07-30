#ifndef __ROLE_ACTIVITY_LAO_DONG_HAO_LI_HPP__
#define __ROLE_ACTIVITY_LAO_DONG_HAO_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"

class RoleActivityLaoDongHaoLi: public RoleActivity
{
public:
	RoleActivityLaoDongHaoLi(int type);
	virtual ~RoleActivityLaoDongHaoLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	void RATianJiangHaoLiAddReceiveCount();
	int GetGtherCount() { return m_param.ra_tian_jiang_hao_li_gather_count; }
public:
	void SendRATianJiangHaoLiInfo();


	RALaoDongHaoLiParam m_param;
};

#endif	//__ROLE_ACTIVITY_TIAN_JIANG_HAO_LI_HPP__
