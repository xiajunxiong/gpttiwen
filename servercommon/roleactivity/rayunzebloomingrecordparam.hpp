#ifndef __RA_YUN_ZE_BLOOMING_RECORD_PARAM_HPP__
#define __RA_YUN_ZE_BLOOMING_RECORD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM = 5;		// ����ȡ��������

struct RAYunZeBloomingRecordParam
{
	RAYunZeBloomingRecordParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
		challenge_end_timestamp = 0;
		cur_integral = 0;
		is_available_integral = 0;

		memset(reward_list, 0, sizeof(reward_list));
	}

	void DayReset()
	{
		times = 0;
	}

	unsigned int ra_begin_timestamp;
	short times;						// ����ս����
	short cur_integral;					// ��ǰ��һ���
	int challenge_end_timestamp;		// ��ս����ʱ���
	int is_available_integral;			// �����Ƿ���ã�1-���ã�0-������
	int reward_list[YUNZE_BLOOMING_RECORD_MAX_REWARD_NUM];					// �����Ƿ�����ȡ��0-δ��ȡ��1-����ȡ
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeBloomingRecordParam) <= sizeof(RoleActivityRoleData));



#pragma pack(pop)

#endif 
