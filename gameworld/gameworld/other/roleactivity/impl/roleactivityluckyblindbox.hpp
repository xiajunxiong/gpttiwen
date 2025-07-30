#ifndef __ROLE_ACTIVITY_LUCKY_BLIND_BOX_HPP__
#define __ROLE_ACTIVITY_LUCKY_BLIND_BOX_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raluckyblindboxparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityLuckyBlindBox : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_LUCKY_BLIND_BOX;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX;

	RoleActivityLuckyBlindBox(int type);
	virtual ~RoleActivityLuckyBlindBox();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void Update(time_t now_second);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnAddChongZhi(long long chongzhi);

	void RandBox(int box_x, int box_y);
	void OnAchiment(int seq);
	void BuyBox(Protocol::CSRandActivityOperaReq * raor);
	
	int OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver, int seq);
	int OnCmdBuy(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int seq, int ARG_OUT * price);
private:
	void RASendInfo();
	void CheckBox();
	RALuckyBlindBoxParam m_param;
};

#endif