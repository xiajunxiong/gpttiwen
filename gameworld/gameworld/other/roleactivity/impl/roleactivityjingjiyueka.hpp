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
	short task_info_index;		//已接取的任务列表下标
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

	bool GetSpecialTypeCount(int task_type, int & count);	//获取一些特殊任务类型的模块进度
	void OnCheckInitTypeProgress();			//初始化一些特殊任务类型的进度
	void OnCalcMap();						//重新组成映射
	void OnCheckLevel(const char * reason);	//检测角色等级
	void OnCheckFetchStatusReissue();		//补发玩家未领取的奖励
	void OnCheckTaskStatus(int task_type);	//检测任务状况
	void OnSeasonStart(const char * reason);//赛季开始
	void OnSeasonEnd(const char * reason);	//赛季结束
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
	std::map<int, std::vector<RAJingJiYueKaTaskSimpleInfo> > m_task_type_map;		//任务类型映射	key:task_type
};

#endif	//__ROLE_ACTIVITY_JING_JI_YUE_KA_HPP__