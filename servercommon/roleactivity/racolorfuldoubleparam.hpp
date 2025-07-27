#ifndef __RA_COLORFUL_DOUBLE_PARAM_HPP__
#define __RA_COLORFUL_DOUBLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//����ӱ�
//RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE
/*
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"

	{
		RoleActivityColorfulDouble * role_activity = static_cast<RoleActivityColorfulDouble *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAColorfulDouble::xxxxx, 1);
		}
	}
*/
namespace RAColorfulDouble
{
	//��������
	static const int BOX_COUNT = 10;
	//ÿ��������
	static const int DAY_TASK_COUNT = 20;

	
	//��������
	enum TaskType
	{
		TSAK_TYPE_LOGIN = 1,		//��¼				���յ�½
		TSAK_TYPE_YUN_ZE_TERRITORY,	//���󻭾�	2147	̽��X��
		TSAK_TYPE_ZAI_ZHAN_YUN_ZE,	//��ս����	2150	��������X��
		TSAK_TYPE_YUN_ZE_ER_SAN_SHI,//���������2153	���X��
		TSAK_TYPE_YUN_ZE_BLOOMING_RECORD,//���󷱻�¼2148	���X����
		TSAK_TYPE_MO_ZU_ZAI_NA_LI,	//ħ��������2151	����X��ħ��

		TSAK_TYPE_MAX,
	};

};

#pragma pack(push, 4)

struct RAColorfulDoubleParam
{
	RAColorfulDoubleParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;

		today_free_fetch_flag = 0;
		today_task_finish_flag = 0;

		act_box_rewaed_flag = 0;
		fetch_box_rewaed_flag = 0;

		act_task_finish_num = 0;

		now_day_id = 0;
		hold_ll = 0;

		memset(task_param_list, 0, sizeof(task_param_list));
	}

	unsigned int ra_begin_timestamp;
	int today_free_fetch_flag;		//������ѽ�����ȡ���
	UNSTD_STATIC_CHECK(30 > RAColorfulDouble::DAY_TASK_COUNT)
	int today_task_finish_flag;		//����������ɱ��[����index]

	UNSTD_STATIC_CHECK(30 > RAColorfulDouble::BOX_COUNT)//��0λ�ճ���
	int act_box_rewaed_flag;		//���佱��������[����seq]
	int fetch_box_rewaed_flag;		//���佱�����ű��[����seq]

	int act_task_finish_num;		//����������
	unsigned int now_day_id;		//����day_id
	int hold_ll;

	//�������[��������]
	int task_param_list[RAColorfulDouble::TSAK_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAColorfulDoubleParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif