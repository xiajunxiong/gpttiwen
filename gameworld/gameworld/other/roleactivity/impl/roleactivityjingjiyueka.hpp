#ifndef __ROLE_ACTIVITY_JING_JI_YUE_KA_HPP__
#define __ROLE_ACTIVITY_JING_JI_YUE_KA_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rajingjiyuekaparam.hpp"
#include "servercommon/rolecommonsavedef.h"

struct RAJingJiYueKaTaskSimpleInfo
{
	RAJingJiYueKaTaskSimpleInfo() { this->Reset(); }
	RAJingJiYueKaTaskSimpleInfo(short _task_group_type, short _task_info_index) : task_group_type(_task_group_type), task_info_index(_task_info_index) {}

	void Reset()
	{
		task_group_type = -1;
		task_info_index = -1;
	}

	short task_group_type;
	short task_info_index;		//�ѽ�ȡ�������б��±�
};


class RoleActivityJingJiYueKa : public RoleActivity
{
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA;
public:
	RoleActivityJingJiYueKa(int type);
	virtual ~RoleActivityJingJiYueKa();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnRAClose();
	virtual void Update(time_t now_second);
	virtual void OnRoleLevelUp(int old_level, int to_level);
	virtual void OnConsumeGold(long long consume_gold);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnWeekChange();

	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnAddTaskTypeProgress(int task_type, int count);

	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);

	void SendAllInfo();
public:

	bool GetSpecialTypeCount(int task_type, int & count);	//��ȡһЩ�����������͵�ģ�����
	void OnCheckInitTypeProgress();			//��ʼ��һЩ�����������͵Ľ���
	void OnCalcMap();						//�������ӳ��
	void OnCheckLevel(const char * reason);	//����ɫ�ȼ�
	void OnCheckFetchStatusReissue();		//�������δ��ȡ�Ľ���
	void OnCheckTaskStatus(int task_type);	//�������״��
	void OnSeasonStart(const char * reason);//������ʼ
	void OnSeasonEnd(const char * reason);	//��������
	bool IsSpecialType(int task_type);

	void OnFetchCommonReward(int card_level);
	void OnFetchSeniorReward(int card_level);
	void OnBuyLevel(int buy_level);
	void OnRefreshDayTask(int task_index);
	void OnBuyCmdCheck();

	void OnSendNoticeInfo(int notice_type, int param1);
	void OnSendRefreshInfo(int task_index, const RAJingJiYueKaTaskInfo & task_info);
	void OnSendSingleTypeInfo(int task_type);
	void OnSendTaskListInfo(int task_group_type);

	RAJingJiYueKaParam m_param;
	std::map<int, std::vector<RAJingJiYueKaTaskSimpleInfo> > m_task_type_map;		//��������ӳ��	key:task_type
};

#endif	//__ROLE_ACTIVITY_JING_JI_YUE_KA_HPP__