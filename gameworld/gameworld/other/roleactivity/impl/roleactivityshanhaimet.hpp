#ifndef __ROLE_ACTIVITY_SHAN_HAI_MET_HPP__
#define __ROLE_ACTIVITY_SHAN_HAI_MET_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityShanHaiMet : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHAN_HAI_MET;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET;


	RoleActivityShanHaiMet(int type);
	virtual ~RoleActivityShanHaiMet();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//ֱ��
	int OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(ARG_OUT int * price);

	//�����¼
	void AddTaskParam(int task_type, int task_param);


	//������Ϣ
	void SendBaseInfo();
	//������Ϣ
	void SendTaskParam();
	//��������ı�
	void SendTaskParamChange(int task_type, int task_param);
public:
	//��������
	//ͨ������
	void FetchNormal(const int fetch_index);
	//δ��ɽ���
	void FetchUnfinish(const int fetch_index);
	//���⽱��
	void FetchExtra(const int fetch_index);

	void CheckToDayTaskFinish(const int now_act_open_day);

	RAShanHaiMetParam m_param;
};

#endif
