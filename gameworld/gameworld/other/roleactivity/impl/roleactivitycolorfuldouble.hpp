#ifndef __ROLE_ACTIVITY_COLORFUL_DOUBLE_HPP__
#define __ROLE_ACTIVITY_COLORFUL_DOUBLE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"
#include "servercommon/rolecommonsavedef.h"


class RoleActivityColorfulDouble : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_COLORFUL_DOUBLE;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE;


	RoleActivityColorfulDouble(int type);
	virtual ~RoleActivityColorfulDouble();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnLoginSendInfo();
	virtual void OnLoginCheck();

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void OnRAClose();

	//ֱ��
	int OnBuyItemCheck(long long ARG_OUT & unique_key, int cfg_ver, int box_seq);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int box_seq, int ARG_OUT * price);

	//�����¼
	void AddTaskParam(int task_type, int task_param, unsigned int now_dayid = 0);

	void CheckGive(unsigned int act_begin_itme);

	//������Ϣ
	void SendBaseInfo();
	//������Ϣ
	void SendTaskParam();
	//��������ı�
	void SendTaskParamChange(int task_type, int task_param);
public:
	//��������
	void OnActBoxReq(int cfg_ver,int box_seq);
	//����佱��s
	int ActBoxReward(int box_seq);
	//��ȡ���佱��
	void FetchBoxReaard(int box_seq);

	//��ȡ��ѽ���
	void FetchFreeReward();

	//�������������
	void ChackTaskFinish(int task_type);

	RAColorfulDoubleParam m_param;
};



#endif