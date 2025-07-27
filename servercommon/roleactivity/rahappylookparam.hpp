#ifndef __RA_HAPPY_LOOK_PARAM_HPP__
#define __RA_HAPPY_LOOK_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// �����ַ��� //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAHappyLookParam
{
	RAHappyLookParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
		challenge_index = -1;
		challenge_is_succ = -1;
		challenge_end_timestamp = 0;
	}

	void ResetQuency()
	{
		challenge_index = -1;
		challenge_is_succ = -1;
		challenge_end_timestamp = 0;
	}

	unsigned int ra_begin_timestamp;
	short ra_frequency_num;										// ����ս����

	char challenge_index;										// ��սindex
	char challenge_is_succ;										// �Ƿ���ս�ɹ�
	int challenge_end_timestamp;								// ��սʧ��ʱ���
};

UNSTD_STATIC_CHECK(sizeof(RAHappyLookParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_HAPPY_LOOK_PARAM_HPP__
