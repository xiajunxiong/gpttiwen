#ifndef __ROLE_ACT_CHAO_ZHI_GIFT_HPP__
#define __ROLE_ACT_CHAO_ZHI_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rachaozhigiftdef.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleACTChaoZhiGift : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_CHAO_ZHI_GIFT;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT;

	RoleACTChaoZhiGift(int type);
	virtual ~RoleACTChaoZhiGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();

	virtual void OnLoginCheck();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnRAClose();

	//直购
	int BuyItemCheck(int cfg_ver, long long ARG_OUT & unique_key);
	int BuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(ARG_OUT int * price);

	void SendInfo();

	//领取
	void FetchGift(int gift_index);

	//补发
	void CheckGive(unsigned int act_begin_itme);

public:
	RoleRAChaoZhiGiftParam m_param;
};

#endif