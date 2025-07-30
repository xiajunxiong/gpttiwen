#ifndef __WAN_LING_FB_HPP__
#define __WAN_LING_FB_HPP__

#include "servercommon/wanlingfbdef.hpp"
class RoleModuleManager;
class RoleWanLingFB
{
public:
	RoleWanLingFB();
	~RoleWanLingFB();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const WanLingFBParam & param);
	void GetInitParam(WanLingFBParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	//��ɫ����
	void OnReq(int req_type, int param_1, int param_2);

	//�ɽ��븱��
	bool CanEnterFB(int fb_index);

	//ͨ�ؽ���
	void FBPass(int fb_index);
	int GetFBPassRewardTimes() { return m_param.day_fetch_pass_reward; }
	//ս������
	void FBBattlePass(int fb_index);
	bool HasRewardTimes() const;

	bool IsCanOneClick();	// �Ƿ����һ�����
	bool OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

	void SendInfo();
private:
	void SetFBFetchFlag(int fb_index, int set_type);

	//��������
	//�������
	void FinishTask();
	//��������ս��
	void TaskFight();

	RoleModuleManager * m_module_mgr;

	WanLingFBParam m_param;
};

#endif