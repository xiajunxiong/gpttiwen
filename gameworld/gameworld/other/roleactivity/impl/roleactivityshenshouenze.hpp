#ifndef __ROLE_ACTIVITY_SHEN_SHOU_EN_ZE_HPP__
#define __ROLE_ACTIVITY_SHEN_SHOU_EN_ZE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityShenShouEnZe : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHEN_SHOU_EN_ZE;			//金龙聚宝
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE;

	RoleActivityShenShouEnZe(int type);
	virtual ~RoleActivityShenShouEnZe();

	void * operator new(size_t c);
	void operator delete(void * m);

	virtual void Init(RoleModuleManager * mgr, const RoleActivityRoleData & p);
	virtual void GetInitParam(RoleActivityRoleData * p);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRAClose();
	virtual void OnLoginCheck();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor);

	int FetchTimesReward();
	int SetReward(int reward_seq);
	int ClearSetReward();
	int DrwardReward(int times);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyItemByMoneyType(int seq);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);
private:
	void SendInfo();
	void SendRecordInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放

	void AddRecord(const std::vector<ItemConfigData> & rewards,bool is_broadcast);

	RAShenShouEnZeParam m_param;
};

#endif	

