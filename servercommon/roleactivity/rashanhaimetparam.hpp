#ifndef __RA_SHAN_HAI_MET_PARAM_HPP__
#define __RA_SHAN_HAI_MET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//ɽ������
//RAND_ACTIVITY_TYPE_SHAN_HAI_MET

//��������� 
static const int SHAN_HAI_MET_DAY_MAX = 10;
/*
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"

{//ɽ������
RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
if (NULL != role_activity)
{
role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_SHANHAI_QUESTION, 1);
}
}

*/
namespace RAShanHaiMet
{
	//��������
	enum TaskType
	{
		TSAK_TYPE_SUMMER_LI_LIAN = 0,	//2124 �������� p:����������	�ĳ� 2155
		TSAK_TYPE_COLORFUL_LANTERN,		//2125 ��ʺӵ� p:��ɻ����  �ĳ� 2160
		TSAK_TYPE_HAPPY_LOOK,			//2134 �����ַ��� p:�������
		TSAK_TYPE_SHANHAI_QUESTION,		//2142 ɽ���ʴ� p:�������
		TSAK_TYPE_INTEGRAL_DRAW,		//2140 ����齱(ɽ�����£� p:�齱����
		TSAK_TYPE_LOTUS_MI_BAO_ACT_ROW,	//2122 �����ٱ� p:��������		�ĳ� 2156
		TSAK_TYPE_LOTUS_MI_BAO_ACT_PAGE,//2122 �����ٱ� p:����ҳ��		�ĳ� 2156

		TSAK_TYPE_MAX,
	};
	//������  ȫ����Ϊ�������
	// ������ɻ��ǰ���ÿ��ͳ��
	// ���������Ϊ��ڼ��ܴ���
	////�������--ÿ��
	//static const int DAY_RESET_TASK_[] =
	//{
	//	TSAK_TYPE_SUMMER_LI_LIAN,
	//	TSAK_TYPE_COLORFUL_LANTERN,
	//	TSAK_TYPE_HAPPY_LOOK,
	//	TSAK_TYPE_SHANHAI_QUESTION,
	//	TSAK_TYPE_INTEGRAL_DRAW,
	//};
	//static const int DAY_RESET_TASK_CONUT = ARRAY_ITEM_COUNT(DAY_RESET_TASK_);
};

#pragma pack(push, 4)

//////////////////  ��������  /////////////////////////////////
struct RAShanHaiMetParam
{
	RAShanHaiMetParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;

		act_extra_rewaed_flag = 0;
		day_task_finish_flag = 0;
		fetch_reward_flag = 0;
		fetch_extra_rewaed_flag = 0;

		today_task_finish = 0;
		hold_ll = 0;

		memset(task_param_list, 0, sizeof(task_param_list));
	}
	unsigned int ra_begin_timestamp;

	int act_extra_rewaed_flag;	//���⽱��������

	int day_task_finish_flag;	//������ɱ��(ֻ����ȡ�Ϳ���ʱУ��)
	int fetch_reward_flag;		//�������ű��[�����index]
	int fetch_extra_rewaed_flag;//���⽱�����ű��[�����index]

	int today_task_finish;		//����������ɱ��

	long long hold_ll;

	//�������[��������]
	int task_param_list[RAShanHaiMet::TSAK_TYPE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiMetParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SHAN_HAI_MET_PARAM_HPP__
