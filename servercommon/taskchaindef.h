#ifndef __TASK_CHAIN_DEF_H__
#define __TASK_CHAIN_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int TASK_CHAIN_PARAM_COUNT = 10;		// ��⣬���ģ�
UNSTD_STATIC_CHECK(10 == TASK_CHAIN_PARAM_COUNT);

static const int TASK_CHAIN_RATE_MAX = 10000;		//����Ȩ�ر�-��ֱ�

struct TaskChainParam
{
	TaskChainParam() { this->Reset(); }
	void Reset()
	{
		pass_count = 0;
		task_type = 0;
		memset(task_param, 0, sizeof(task_param));
		task_count = 0;
		next_refresh_timestamp = 0LL;
		task_limit_timestamp = 0LL;
		has_fetch_turn_table_reward = 0;
		turn_table_group_id = 0;
		accept_task_level = 0;	
		pre_wait_task = 0;
		next_completion_times = 0;
		turn_table_completion_times_index = -1;
		is_version_change_process_flag = 0;
		is_version_change_week_reset_flag = 0;
		version_change_completion_times = 0;
		task_pass_count = 0;
		cur_task_max_count = 0;
	}

	void ClearTask()
	{
		task_type = 0;
		memset(task_param, 0, sizeof(task_param));
		can_skip = 0;
		reward_double = 0;
		price_type = 0;
		price = 0;
		task_limit_timestamp = 0LL;
	}

	int			pass_count;		// ���ִε�������ȣ�
	int			task_type;
	int			task_param[TASK_CHAIN_PARAM_COUNT];
	short		can_skip;
	short		reward_double;
	int			price_type;
	int			price;
	int			task_count;					// �׶Σ�
	long long	next_refresh_timestamp;		// �´�ˢ�������Լ��ܴ�����ʱ���
	long long	task_limit_timestamp;		// ��������ʱ�� - 0������
	short		has_fetch_turn_table_reward;	// �����Ƿ���ȡ��ת�� 0:δ��ȡ 1:����ȡ(λ���㣩
	short		turn_table_group_id;			// ����ת������Ľ�����ID
	short		accept_task_level;				// ��ȡÿ�׶�������ʱ�ĵȼ�
	short       pre_wait_task;					// ����������
	short		next_completion_times;			// ��һ���׶εĻ���Ŀ��
	short		turn_table_completion_times_index;	// ��ʱ�����ת�̽�����Ӧ����һλ����Ӧ��ȡλ���㣩
	char		is_version_change_process_flag;		// �İ�����ʶ  0:δ���� 1:�Ѵ��� 2:�°����ݸĶ�
	char		is_version_change_week_reset_flag;	// �İ���Ƿ��������ˢ�� 0:�� 1:��
	short		version_change_completion_times;	// �İ�����ݴ���,��ʱӦ���·���������ɵĻ���
	short		task_pass_count;				// ���׶ε��������
	short		cur_task_max_count;				// ���׶ε������
};
typedef char TaskChainParamHex[sizeof(TaskChainParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskChainParamHex) < 256);

#pragma pack(pop)
#endif // !__TASK_CHAIN_DEF_H__