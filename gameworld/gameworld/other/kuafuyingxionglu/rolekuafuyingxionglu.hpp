#ifndef __ROLE_KUA_FU_YING_XIONG_LU_HPP__
#define __ROLE_KUA_FU_YING_XIONG_LU_HPP__

#include "servercommon/struct/global/kuafuyingxiongludef.hpp"

class RoleModuleManager;
class RoleKuaFuYingXiongLu
{
public:
	RoleKuaFuYingXiongLu();
	~RoleKuaFuYingXiongLu();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const RoleKuaFuYingXiongParam & param);
	void GetInitParam(RoleKuaFuYingXiongParam * param);

	void OnRoleLogin();
	void OnUpdate(unsigned int now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnCheckInitActivity();

	void OnFetchTaskReward(int rank_type, int seq);

	void SendAllInfo();
	void SendSingleRankInfo(int rank_type, KFYXLRoleRankTypeItem & rank_item);
	void SendSingleTaskInfo(int rank_type, int seq, KuaFuYingXiongLuTaskInfo & task_info);
	void SendRoleRankValue();

	//�ȸ��߼�
	void OnReloadConfig();
	//���������Ϣ���
	void OnRelatedInfoChange(int task_type, int param1, int add_value = 0);


private:

	bool IsInvalidRankType(int rank_type) { return rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN || rank_type >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; }
	bool IsInvalidTaskType(int task_type) { return task_type <= KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID || task_type >= KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX; }
	bool IsInvalidTaskSeq(int task_index) { return task_index < 0 || task_index >= MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM; }

	bool IsAddTypeByTaskType(int task_type) const;
	int GetValueByRankType(int rank_type) const;

	//���������������,�鿴�Ƿ���Ҫ�������ʼ��
	void OnCheckAllTaskData(bool is_init = false);
	//������Ϣ���
	bool OnTaskInfoChange(int rank_type, int task_index, KuaFuYingXiongLuTaskInfo & task_info, int param1, int task_param1, int add_value = 0);
	//���ʱ��,�鿴�Ƿ���Ҫ����
	void OnCheckTime();

	RoleModuleManager * m_role_module_mgr;
	RoleKuaFuYingXiongParam m_param;
};


#endif
