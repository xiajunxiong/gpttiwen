#ifndef __ROLE_ACTIVITY_QIONG_LOU_YU_YAN_HPP__
#define __ROLE_ACTIVITY_QIONG_LOU_YU_YAN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raqionglouyuyanparam.hpp"
class RandActivityQiongLouYuYan;
class RoleActivityQiongLouYuYan : public RoleActivity
{
public:
	RoleActivityQiongLouYuYan(int type);
	virtual ~RoleActivityQiongLouYuYan();
	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLogin();
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
private:
	void CheckBanquetEndTamp(RandActivityQiongLouYuYan * rand_activity);
	void SendRoleInfo();
	void GetReward(int seq);
	void PreparationItem(int item_id);
	void EatFood();//³ÔÏ¯
	RAQiongLouYuYanRoleParam m_param;
};


#endif
