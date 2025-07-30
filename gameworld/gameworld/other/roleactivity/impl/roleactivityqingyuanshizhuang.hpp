#ifndef __ROLE_ACTIVITY_QING_YUAN_SHI_ZHUANG_HPP__
#define __ROLE_ACTIVITY_QING_YUAN_SHI_ZHUANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raqingyuanactivityparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityQingYuanShiZhuang : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_QING_YUAN_SHIZHUANG;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG;

	RoleActivityQingYuanShiZhuang(int type);
	virtual ~RoleActivityQingYuanShiZhuang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAClose();

	void OnJieHun(long long qingyuan_uid);

	int OnBuyItemCheck(int seq_index, long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int seq_index, ARG_OUT int * price);

	void RASendInfo();
	void OnLiHunBuFa(int buy_mark);
public:
	void RAFetchReward(int seq_index);
	void RAShop(int seq_index);
	void BuyNotice();

	RAQingYuanShiZhuangParam m_param;
};

#endif	//__ROLE_ACTIVITY_QING_YUAN_SHI_ZHUANG_HPP__
