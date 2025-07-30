#ifndef __ROLE_ACTIVITY_JIN_LONG_JU_BAO_HPP__
#define __ROLE_ACTIVITY_JIN_LONG_JU_BAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "protocol/randactivity/msgrajinlongjubao.hpp"

class RoleActivityJinLongJuBao : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_JIN_LONG_JU_BAO;			//金龙聚宝
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO;

	RoleActivityJinLongJuBao(int type);
	virtual ~RoleActivityJinLongJuBao();

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

	int FetchTimesReward(int reward_seq);
	int SetReward(int index, int reward_seq);
	int DrwardReward(int times);
	int DrawHelper(Protocol::SCRAJinLongJuBaoDrawRetItem & item);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

private:
	void SendInfo(int reason = 0);
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放
	bool IsAllSelect();
	bool IsAllGet();
	bool CheckNextTimes();		//检测这轮自选奖励是否全部获得，是的话，新开始一轮
	void ResetReward();
	void AddRecord(const std::vector<ItemConfigData> & rewards);
	BaoDiInfoItem * GetBaoDiInfoItem(int bao_di_id);
	void SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list);

	RAJinLongJuBaoParam m_param;
};

#endif	

