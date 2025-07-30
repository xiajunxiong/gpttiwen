#ifndef __ROLE_ACTIVITY_JIXING_GAOZHAO_HPP__
#define __ROLE_ACTIVITY_JIXING_GAOZHAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rajixinggaozhaoparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityJiXingGaoZhao : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_JIXING_GAOZHAO;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO;

	RoleActivityJiXingGaoZhao(int type);
	virtual ~RoleActivityJiXingGaoZhao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnAddRandTimes(int times) { m_param.rand_times += times; }
	void ClearRandTimes() { m_param.rand_times = 0; }

	int OnBuyGiftItem(int shop_index, ARG_OUT int * price);

	int OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	const RAJiXingGaoZhaoParam * GetParam() { return &m_param; }
public:
	void RASendInfo();
	void RAShop(int shop_index);
	void RAFetchDrawReward(int index);

	RAJiXingGaoZhaoParam m_param;
};

#endif	//__ROLE_ACTIVITY_JIXING_GAOZHAO_HPP__
