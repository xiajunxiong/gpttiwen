#ifndef __ROLE_ACTIVITY_XIAN_SHI_LEI_CHONG_HPP__
#define __ROLE_ACTIVITY_XIAN_SHI_LEI_CHONG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raxianshileichongparam.hpp"

class RoleActivityXianShiLeiChong : public RoleActivity
{
public:
	RoleActivityXianShiLeiChong(int type);
	virtual ~RoleActivityXianShiLeiChong();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnAddChongZhi(long long chongzhi);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

public:
	void CalcChongZhiInfo();
	void GetReward(int _req);
	void ReplacementReward(bool is_day_change = false);
	void SendInfo();

	RAXianShiLeiChongParam  m_param;
};

#endif	//__ROLE_ACTIVITY_XIAN_SHI_LEI_CHONG_HPP__
