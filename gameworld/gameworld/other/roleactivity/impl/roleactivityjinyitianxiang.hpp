#ifndef __ROLE_ACTIVITY_JIN_YI_TIAN_XIANG_HPP__
#define __ROLE_ACTIVITY_JIN_YI_TIAN_XIANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"

class RoleActivityJinYiTianXiang: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_JIN_YI_TIAN_XIANG;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG;

	RoleActivityJinYiTianXiang(int type);
	virtual ~RoleActivityJinYiTianXiang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int Draw();

	int OnBuyItemCheck(long long ARG_OUT & unique_key) const;
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyItemByMoneyType();

	int OnBuyGiftItem(ARG_OUT int * price);

	bool OnUseItem(int add_draw_value);
private:
	void SendInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放
	void AddDrawValue(int add_draw_value, const char * reason);

	RAJinYiTianXiangParam m_param;
};

#endif	
