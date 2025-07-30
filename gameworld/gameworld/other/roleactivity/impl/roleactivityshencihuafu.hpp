#ifndef __ROLE_ACTIVITY_SHEN_CI_HUA_FU_HPP__
#define __ROLE_ACTIVITY_SHEN_CI_HUA_FU_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/randactivityshencihuafuparam.hpp"
#include "protocol/randactivity/msgrashencihuafu.hpp"

class RoleActivityShenCiHuaFu : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHEN_CI_HUA_FU;			//�����۱�
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU;

	RoleActivityShenCiHuaFu(int type);
	virtual ~RoleActivityShenCiHuaFu();

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

	int SetReward(const int(&select_reward_seq_list)[SHEN_CI_HUA_FU_SELECT_NUM_SELECT]);
	int DrwardReward(int times);
	int DrawHelper(Protocol::SCRAShenCiHuaFuRetItem & item);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyItemByMoneyType(int seq);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);

	int ExchangeShop(int seq);
private:
	void SendInfo(int reason = 0);
	bool IsAllSelect();
	bool IsAllGet();
	bool CheckNextTimes();		//���������ѡ�����Ƿ�ȫ����ã��ǵĻ����¿�ʼһ��
	void ResetReward();
	void GiveUnFetchReward();  //�����δ��ȡ�Ľ����Զ�����

	RAShenCiHuaFuParam m_param;
};

#endif	

