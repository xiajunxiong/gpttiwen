#ifndef __ROLE_ACTIVITY_YUAN_QI_FAN_PAI_HPP__
#define __ROLE_ACTIVITY_YUAN_QI_FAN_PAI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"

class RoleActivityYuanQiFanPai : public RoleActivity
{
public:
	RoleActivityYuanQiFanPai(int type);
	virtual ~RoleActivityYuanQiFanPai();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();


	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void Lottery();				// ����
	void ReceiveTimes(int seq);	// ��ȡ���ƴ�������
	void Exchange(int seq);		// �һ�
	void FetchDayReward();		// ��ȡÿ�ս���
private:
	void RASendInfo();

	RAYuanQiFanPaiParam m_param;
};






#endif