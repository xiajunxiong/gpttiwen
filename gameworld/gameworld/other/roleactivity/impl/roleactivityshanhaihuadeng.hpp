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
	
	void OnStartGame();						//开始游戏
	void OnCatchLantern(int type);			//捕抓花灯
	void OnFetchTaskReward(int task_id);	//领取任务奖励
	void OnForceClose();					//游戏中强制关闭
	void OnFlowAway(int type);				//花灯流走

	void OnRefreshLight(unsigned int now_second);
	
	void OnFinishGame();					//完成游戏

	void SendAllInfo();
	void SendGameInfo(int notice_type);
	void SendLightInfo();

	RAShanHaiHuaDengParam m_param;
	unsigned int next_refresh_light_timestamp;					//下次刷新花灯的时间戳
	short m_light_list[MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM];	//游戏中此时刷出的各个类型花灯数量
	//std::map<int, int> m_light_count_map;					//花灯数量key:type
};

#endif // __ROLE_ACTIVITY_SHANHAI_HUADENG_HPP__