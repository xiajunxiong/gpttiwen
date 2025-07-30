#ifndef __ROLE_ACTIVITY_SHAN_HAI_BAO_XIANG_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_BAO_XIANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "servercommon/roleactivity/rashanhaibaoxiangparam.hpp"

class RoleActivityShanHaiBaoXiang : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHAN_HAI_BAO_XIANG;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG;

	RoleActivityShanHaiBaoXiang(int type);
	virtual ~RoleActivityShanHaiBaoXiang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual bool IsFinishBonusCond(int type = 0);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);		//ʹ�õ��߻�ȡ,��ѡ�ļ���,ֱ�Ӹ�

	int OnBuyItemCheck(int seq, int param1, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
public:
	void SendInfo();
	void ReceiveFreeReward();

	RAShanHaiBaoXiangParam m_param;
};

#endif	//__ROLE_ACTIVITY_SHAN_HAI_BAO_XIANG_HPP__
