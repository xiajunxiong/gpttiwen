#ifndef __ROLE_ACTIVITY_SHANHAI_HUADENG_HPP__
#define __ROLE_ACTIVITY_SHANHAI_HUADENG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rashanhaihuadengparam.hpp"

class RoleActivityShanHaiHuaDeng : public RoleActivity
{
public:
	RoleActivityShanHaiHuaDeng(int type);
	virtual ~RoleActivityShanHaiHuaDeng();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

private:
	
	void OnStartGame();						//��ʼ��Ϸ
	void OnCatchLantern(int type);			//��ץ����
	void OnFetchTaskReward(int task_id);	//��ȡ������
	void OnForceClose();					//��Ϸ��ǿ�ƹر�
	void OnFlowAway(int type);				//��������

	void OnRefreshLight(unsigned int now_second);
	
	void OnFinishGame();					//�����Ϸ

	void SendAllInfo();
	void SendGameInfo(int notice_type);
	void SendLightInfo();

	RAShanHaiHuaDengParam m_param;
	unsigned int next_refresh_light_timestamp;					//�´�ˢ�»��Ƶ�ʱ���
	short m_light_list[MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM];	//��Ϸ�д�ʱˢ���ĸ������ͻ�������
	//std::map<int, int> m_light_count_map;					//��������key:type
};

#endif // __ROLE_ACTIVITY_SHANHAI_HUADENG_HPP__