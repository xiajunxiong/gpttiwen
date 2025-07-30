#ifndef __ROLE_ACTIVITY_GOD_TOKEN_HPP__
#define __ROLE_ACTIVITY_GOD_TOKEN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ragodtokenparam.hpp"

struct GodTokenItemData
{
	GodTokenItemData() : is_bind_num(0), no_bind_num(0) {}

	int is_bind_num;
	int no_bind_num;
};

class RoleActivityGodToken : public RoleActivity
{
public:
	RoleActivityGodToken(int type);
	virtual ~RoleActivityGodToken();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnWeekChange();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRAOpenOnlineSend();
	virtual void OnRoleLevelUp(int old_level, int to_level);

	void OnRAGodTokenAddCount(int type, int count = 1, int param = 0);			//ÿ������
	void OnRAGodTokenWeekAddCount(int type, int count = 1, int param = 0);		//ÿ������

	bool OnRAGodTokenBuyToken(bool is_gm = false);												//����ս��
	int OnBuyGiftItem(ARG_OUT int * price);

	void OnRAGodTokenCaclTimeSeq();
	void CheckRoleGodTokenIsOpen();

	bool IsRoleraGodTokenOpen() { return m_param.ra_god_token_is_open == 1 ? true : false; }
	void OnReSetRAGodToken(bool is_gm = false);

	void OnRAGodTokenPatchLevelReward();					//�����������رղ���û��ȡ�ĵȼ������Լ�����������ڻ�����Ʒ֮ǰ��
	void OnRAGodTokenReclaimItem();							//�����������رջ�����Ʒ����������֮��
private:
	void SendRAGodTokenInfo();
	void OnTimeSeqChange();

	//����
	bool IsRAGodTokenTaskType(int task_type, int type);
	void OnRAGodTokenRefreshTask(int task_type);

	//�ȼ�����
	void OnRAGodTokenFetchCommonReward(int seq);
	void OnRAGodTokenFetchSeniorReward(int seq);

	void OnRAGodTokenExchange(int seq);

	//����
	void OnRAGodTokenBuyExp(int count);
	bool OnRAGodTokenAddExp(int add_exp);

	//�������
	void OnRAGodTokenFetchSurplusReward();
	void OnRAGodTokenPatchSurplusSeniorReward();			//�ʼ�������������ĸ߼�����

	//������
	void OnRAGodTokenFetchDailyReward(int seq); //(����)
	void OnRAGodTokenFetchWeeklyReward(int seq); //(����)
	void CheckDailyReward(int type);
	void CheckWeekReward(int type);

	void AddWeeklyTask();
	void ChangeWeelyTask();

	RAGodTokenParam m_param;
	unsigned int ra_god_token_cacl_time;

	int m_mail_reclaim_num;
};

#endif	//__ROLE_ACTIVITY_GOD_TOKEN_HPP__
