#ifndef __ROLE_ACTIVITY_HORCRUX_TIANCHENG_HPP__
#define __ROLE_ACTIVITY_HORCRUX_TIANCHENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityHorcruxTianCheng : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_HORCRUX_TIANCHENG;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG;

	RoleActivityHorcruxTianCheng(int type);
	virtual ~RoleActivityHorcruxTianCheng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnConsumeGold(long long consume_gold);
	void OnAddRandTimes(int times) { m_param.rand_times += times; }
	void ClearRandTimes() { m_param.rand_times = 0; }
	int OnBuyGiftItem(int shop_index, ARG_OUT int * price);

	int OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	const RAHorcruxTianChengParam * GetParam() { return &m_param; }
public:
	void RASendInfo();
	void RAExchange(int exchange_index);
	void RAShop(int shop_index);
	void RAFetchConsumeReward(int seq);
	void CheckConsumeReward();

	RAHorcruxTianChengParam m_param;
};

#endif	//__ROLE_ACTIVITY_HORCRUX_TIANCHENG_HPP__
