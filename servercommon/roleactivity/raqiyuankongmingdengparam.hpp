#ifndef __RA_QIYUAN_KONGMINGDENG_PARAM_HPP__
#define __RA_QIYUAN_KONGMINGDENG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ��Ը������ //////////////////////////////////////////
static const int MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT = 10;


enum RA_QIYUAN_KONGMINGDENG_TASK_TYPE
{
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_LOGIN = 1,						// ÿ�յ�½	
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_ACTIVE = 2,					// ��Ծ�ﵽ X ��
	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_VITALITY_HELPS = 3,			// Ԫ������ X ��

	RA_QIYUAN_KONGMINGDENG_TASK_TYPE_MAX
};


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////


struct RAQiYuanKongMingDengParam
{
	RAQiYuanKongMingDengParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		huozhong = 0;
		now_day_id = 0;

		complete_flag = 0;
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;
	int huozhong;				// ��õĻ�������, �����
	int now_day_id;
	
	int complete_flag;					// ���������� ��λ���
	unsigned short task_list[MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT];		// �������
};

UNSTD_STATIC_CHECK(sizeof(RAQiYuanKongMingDengParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_QIYUAN_KONGMINGDENG_PARAM_HPP__
