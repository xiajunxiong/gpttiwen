#ifndef __ROLE_ACTIVITY_COLORFUL_LANTERN_HPP__
#define __ROLE_ACTIVITY_COLORFUL_LANTERN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

class RoleActivityColorfulLantern : public RoleActivity
{
public:
	RoleActivityColorfulLantern(int type);
	virtual ~RoleActivityColorfulLantern();

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

	RAColorfulLanternParam m_param;
	unsigned int next_refresh_light_timestamp;					//�´�ˢ�»��Ƶ�ʱ���
	short m_light_list[MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM];	//��Ϸ�д�ʱˢ���ĸ������ͻ�������
	//std::map<int, int> m_light_count_map;					//��������key:type
};

#endif // __ROLE_ACTIVITY_COLORFUL_LANTERN_HPP__