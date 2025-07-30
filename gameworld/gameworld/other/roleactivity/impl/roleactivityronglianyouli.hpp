#ifndef __ROLE_ACTIVITY_RONG_LIAN_YOU_LI_HPP__
#define __ROLE_ACTIVITY_RONG_LIAN_YOU_LI_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "protocol/randactivity/msgraronglianyouli.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

class RoleActivityRongLianYouLi : public RoleActivity
{
public:
	RoleActivityRongLianYouLi(int type);
	virtual ~RoleActivityRongLianYouLi();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void AddTaskScore(int purple = 0, int orange = 0, int red = 0);		// �����������

private:
	void RASendInfo();
	void ReceiveTaskReward(int seq);	// ��ȡ������ɽ���
	void ReceiveTaskNumReward();		// ��ȡ���������������
	void ReissueAllReward();			// ����ȫ��δ��ȡ����

	RARongLianYouLiParam m_param;
};




#endif