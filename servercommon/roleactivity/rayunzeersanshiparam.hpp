#ifndef __RA_YUN_ZE_ER_SAN_SHI_PARAM_HPP__
#define __RA_YUN_ZE_ER_SAN_SHI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YUNZE_ER_SAN_SHI_MAX_TASK_NUM = 10;

enum RECEIVE_STATUS
{
	RECEIVE_STATUS_EXIST = 0,	// δ��ȡ
	RECEIVE_STATUS_LACK = 1,	// ����ȡ
};

struct RAYunZeErSanShiParam
{
	RAYunZeErSanShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
		is_receive_reward = 0;
		is_turntable_reward = 0;
		task_id = 0;

		memset(task_list, 0, sizeof(task_list));
	}

	void DayReset()
	{
		times = 0;
		is_receive_reward = 0;
		is_turntable_reward = 0;

		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	short times;
	short is_receive_reward;			//�����Ƿ���ȡ��ת�̽����� 0-δ��ȡ��1-����ȡ
	short is_turntable_reward;			// �Ƿ����ת�� 1-���ԣ�0-������
	short task_id;
	int task_list[YUNZE_ER_SAN_SHI_MAX_TASK_NUM];		//�����������id
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeErSanShiParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif