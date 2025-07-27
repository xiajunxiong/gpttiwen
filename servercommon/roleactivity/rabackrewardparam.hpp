#ifndef __RA_BACK_REWARD_PARAM_HPP__
#define __RA_BACK_REWARD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

////////////////////////////////////////////  �ع���� //////////////////////////////////////////////////////

static const int MAX_RA_BACK_REWARD_DAY = 8;					// �ع齱����������
static const int MAX_RA_BACK_REWARD_TASK = 64;					// �ع齱����������

enum RA_BACK_REWARD_TYPE
{
	RA_BACK_REWARD_TYPE_BEGIN = 0,
	RA_BACK_REWARD_TYPE_PET_INTENSI,				// 1 ����ǿ������
	RA_BACK_REWARD_TYPE_PARTNER_UPGRADE,			// 2 ������
	RA_BACK_REWARD_TYPE_PASS_FB,					// 3 ͨ����ͨ�ؾ�
	RA_BACK_REWARD_TYPE_PASS_LHWC,					// 4 ͨ���ֻ�����
	RA_BACK_REWARD_TYPE_PURPLE_TOKEN,				// 5 ������ɫ��������
	RA_BACK_REWARD_TYPE_BIG_DIPPER,					// 6 ���뱱������
	RA_BACK_REWARD_TYPE_WEAR_EQUIPNUM,				// 7 ��鴩�����񿨵�����
	RA_BACK_REWARD_TYPE_PASS_ELITEFB,				// 8 ͨ�ؾ�Ӣ�ؾ�
	RA_BACK_REWARD_TYPE_PET_INTENSI_LEVEL,			// 9 ����ǿ���ȼ�

	RA_BACK_REWARD_TYPE_MAX
};

enum RA_BACK_REWARD_FLAG_TYPE
{
	RA_BACK_REWARD_FLAG_TYPE_NOT_FINISH = 0,		// δ���
	RA_BACK_REWARD_FLAG_TYPE_FINISH,				// �����
	RA_BACK_REWARD_FLAG_TYPE_FETCH,					// ����ȡ

	RA_BACK_REWARD_FLAG_TYPE_MAX
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
struct RABackRewardTaskData
{
	char flag;
	unsigned char param;
};


struct RABackRewardParam
{
	RABackRewardParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		huigui_timestamp = 0;
		memset(day_flag, 0, sizeof(day_flag));
		memset(task_list, 0, sizeof(task_list));
	}

	unsigned int ra_begin_timestamp;					// �Ƿ���һ�β���ʱ���
	unsigned int huigui_timestamp;						// �ع���ʼʱ���

	char day_flag[MAX_RA_BACK_REWARD_DAY];
	RABackRewardTaskData task_list[MAX_RA_BACK_REWARD_TASK];
};

UNSTD_STATIC_CHECK(sizeof(RABackRewardParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_BACK_REWARD_PARAM_HPP__
