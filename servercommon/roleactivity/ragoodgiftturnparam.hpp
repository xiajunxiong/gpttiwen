#ifndef __RA_GOOD_GIFT_TURN_PARAM_HPP__
#define __RA_GOOD_GIFT_TURN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_GOOD_GIFT_TURN_NUM = 5;			//ת�̸��������
UNSTD_STATIC_CHECK(MAX_GOOD_GIFT_TURN_NUM <= (int)(BIT_COUNT_OF_BYTE * sizeof(short)));

struct RAGoodGiftTurnParam
{
	RAGoodGiftTurnParam() {  this->Reset(); }

	void Reset()
	{
		ra_begin_time = 0;
		fetch_num = 0;
		fetch_flag = 0;
		last_add_fetch_num_day = 0;
	}

	void ActReset()
	{
		fetch_num = 0;
		fetch_flag = 0;
		last_add_fetch_num_day = 0;
	}

	unsigned int ra_begin_time;
	short fetch_num;					// ʣ����ȡ����
	short fetch_flag;					// ��ȡ������ʶ(������) 
	int last_add_fetch_num_day;			// �ϴ���ȡ��ʶʱ�����(���������)
};

UNSTD_STATIC_CHECK(sizeof(RAGoodGiftTurnParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif