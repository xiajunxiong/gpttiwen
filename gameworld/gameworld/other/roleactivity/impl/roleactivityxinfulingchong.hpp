#ifndef __ROLE_ACTIVITY_XIN_FU_LING_CHONG_HPP__
#define __ROLE_ACTIVITY_XIN_FU_LING_CHONG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raxinfulingchongparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RandActivityXinFuLingChongConfig;
class RoleActivityXinFuLingChong : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_XIN_FU_LING_CHONG;			//ÐÂ·þÁé³è
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG;

	RoleActivityXinFuLingChong(int type);
	virtual ~RoleActivityXinFuLingChong();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnCmdBuyItem(long long unique_id, int chong_zhi_gold);

	int OnBuyGiftItem(ARG_OUT int * price);

private:
	void SendAllInfo();
	void BuyItem();
	void OnBuyItemCheck(const RandActivityXinFuLingChongConfig * ra_config);

	RAXinFuLingChongParam m_param;
};



#endif