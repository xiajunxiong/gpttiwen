#ifndef __RA_HAPPY_UP_PARAM_HPP__
#define __RA_HAPPY_UP_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2060 ���� - ��������/ӡ�Ƕһ�  //////////////////////////////////////////

const static int MAX_HAPPY_UP_ITEM_SEQ_NUM = 24;
enum HAPPY_UP_LIMIT_TYPE
{
	HAPPY_UP_LIMIT_TYPE_BEGIN = 0,			//��Ч
	HAPPY_UP_LIMIT_TYPE_BUY_COUNT = 1,		//���޶�
	HAPPY_UP_LIMIT_TYPE_TODAY = 2,			//ÿ���޶�

	HAPPY_UP_LIMIT_TYPE_MAX,
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAHappyUpParam
{
	RAHappyUpParam() { this->Reset(); }
	void Reset()
	{
		ra_happy_up_begin_timestamp = 0;
		memset(ra_happy_up_exchange_count, 0, sizeof(ra_happy_up_exchange_count));
	}

	unsigned int ra_happy_up_begin_timestamp;
	short ra_happy_up_exchange_count[MAX_HAPPY_UP_ITEM_SEQ_NUM];				//��Ҷһ��������ߵ�����
};

UNSTD_STATIC_CHECK(sizeof(RAHappyUpParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_HAPPY_UP_PARAM_HPP__
