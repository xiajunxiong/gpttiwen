#ifndef __ROLE_ACTIVITY_ZHUI_YUE_SHANG_DIAN_HPP__
#define __ROLE_ACTIVITY_ZHUI_YUE_SHANG_DIAN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityzhuiyueshangdianparam.hpp"
#include "protocol/randactivity/msgrazhuiyueshangdian.hpp"

struct RoleActivityZhuiYueShangDianDrawItem;

class RoleActivityZhuiYueShangDian : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN;			//金龙聚宝
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN;

	RoleActivityZhuiYueShangDian(int type);
	virtual ~RoleActivityZhuiYueShangDian();

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

	int SetReward(int index,int reward_seq);
	int DrwardReward(int times);
	int DrawHelper(int & draw_get_item);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyItemByMoneyType(int seq);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);
private:
	void SendInfo();
	void SendRecordInfo();
	void GiveUnFetchReward();  //活动结束未领取的奖励自动发放
	bool IsAllSelect();
	void AddRecord(const RoleActivityZhuiYueShangDianDrawItem & draw_item);
	BaoDiInfoItem * GetBaoDiInfoItem(int bao_di_id);
	void SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list);

	RAZhuiYueShangDianParam m_param;
};

#endif	

