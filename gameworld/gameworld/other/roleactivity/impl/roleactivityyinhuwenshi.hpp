#ifndef __ROLE_ACTIVITY_YINHU_WENSHI_HPP__
#define __ROLE_ACTIVITY_YINHU_WENSHI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rayinhuactivityparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityYinHuWenShi : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_YINHU_WENSHI;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI;

	RoleActivityYinHuWenShi(int type);
	virtual ~RoleActivityYinHuWenShi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();

	int OnBuyGiftItem(ARG_OUT int * price);
	int OnBuyItemCheck(long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

public:
	void RASendInfo();
	void RAFetchReward(int index);

	RAYinHuWenShiParam m_param;
};

#endif	//__ROLE_ACTIVITY_YINHU_WENSHI_HPP__
