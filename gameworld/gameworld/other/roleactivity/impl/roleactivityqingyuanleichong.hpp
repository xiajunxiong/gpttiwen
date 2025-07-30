#ifndef __ROLE_ACTIVITY_QING_YUAN_LEI_CHONG_HPP__
#define __ROLE_ACTIVITY_QING_YUAN_LEI_CHONG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raqingyuanactivityparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityQingYuanLeiChong : public RoleActivity
{
public:
	RoleActivityQingYuanLeiChong(int type);
	virtual ~RoleActivityQingYuanLeiChong();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void Update(time_t now_second);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnAddChongZhi(long long chongzhi);

	void OnJieHun(long long qingyuan_id);
	void OnRewardMail();
	void OnLiHunBuFa(int chongzhi_gold, bool is_offline = false);

	unsigned int GetLiHunTimes() { return m_param.lihun_refresh_timestamp; }
public:
	void RASendInfo();
	void RAFetchReward(int seq_index);
	void OnNotice();

	RAQingYuanLeiChongParam m_param;
};

#endif	//__ROLE_ACTIVITY_QING_YUAN_LEI_CHONG_HPP__
