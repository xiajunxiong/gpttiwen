#ifndef __RA_SHAN_HAI_QUESTION_PARAM_HPP__
#define __RA_SHAN_HAI_QUESTION_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2140 ���� - ɽ���ʴ�  //////////////////////////////////////////

#pragma pack(push,4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  ///////////////////////////////////////////////////////

struct RAShanHaiQuestionParam
{
	RAShanHaiQuestionParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		times = 0;
		question_id = 0;
		reward_seq = 0;
	}

	unsigned int ra_begin_timestamp;
	short times;		//�����ѳ齱����
	short question_id;
	int reward_seq;
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiQuestionParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif
