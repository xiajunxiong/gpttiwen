#ifndef __RA_YUN_ZE_CUTE_PET_PARAM_HPP__
#define __RA_YUN_ZE_CUTE_PET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RAYZCutePet
{
	//ÿ��������
	static const int DAY_TASK_COUNT_MAX = 10;
	//��������ʱ��
	static const int ACTIVITY_DAY_MAX = 12;
	//���������ս����
	static const int MONSTER_MAX_NUM = 10;

	//��������
	enum TaskType
	{
		TASK_TYPE_BEGIN,

		TASK_TYPE_LOGIN = 1,				// ��¼
		TASK_TYPE_YZ_TERRITORY = 2,			// ���󻭾�
		TASK_TYPE_YZ_ZAI_ZHAN = 3,			// ��ս����
		TASK_TYPE_YZ_ER_SAN_SHI = 4,		// ���������
		TASK_TYPE_YZ_BLOOMING_RECORD = 5,	// ���󷱻�¼
		TASK_TYPE_MO_ZU_ZAI_NA_LI = 6,		// ħ��������
		TASK_TYPE_ACTIVE = 7,				// ��Ծ��

		TASK_TYPE_MAX,
	};


};



#pragma pack(push, 4)

struct RAYunZeCutePetParam
{
	RAYunZeCutePetParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		pet_combat_num = 0;
		ra_login_timestamp = 0;

		memset(today_fraction, 0, sizeof(today_fraction));
		memset(task_param_list, 0, sizeof(task_param_list));
		task_status.Reset();
		challenge_status.Reset();
	}

	void DayReset()
	{

		memset(task_param_list, 0, sizeof(task_param_list));
		task_status.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned int ra_login_timestamp;
	int pet_combat_num;					// ����ս����

	//��ʷÿ�ջ��ս��[������ʱ��--12]
	int today_fraction[RAYZCutePet::ACTIVITY_DAY_MAX];

	//�������[��������], ÿ������÷�
	int task_param_list[RAYZCutePet::TASK_TYPE_MAX];

	//�������״̬
	BitMap<RAYZCutePet::DAY_TASK_COUNT_MAX> task_status;

	//�ȳ���ս״̬[1-10]
	BitMap<RAYZCutePet::MONSTER_MAX_NUM+1> challenge_status;
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeCutePetParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif