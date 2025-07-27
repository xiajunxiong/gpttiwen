#ifndef __RA_TIME_LIMIT_STORE_PARAM_HPP__
#define __RA_TIME_LIMIT_STORE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2049 ����  ��ʱ�̵�  //////////////////////////////////////////

//-----------------------------����-��ʱ�̵�------------------------------------
static const int RA_TIME_LIMIT_STORE_MAX_NUM = 12;

enum RA_TIME_LIMIT_STORE_MONEY_TYPE
{
	RA_TIME_LIMIT_STORE_MONEY_TYPE_GOLD = 0,					//Ԫ��
	RA_TIME_LIMIT_STORE_MONEY_TYPE_IMMORTAL_COIN = 1,			//�ɱ�
	RA_TIME_LIMIT_STORE_MONEY_TYPE_COIN = 2,					//ͭ��
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RATimeLimitStoreParam
{
	RATimeLimitStoreParam() { this->Reset(); }

	void Reset()
	{
		ra_time_limit_store_begin_timestamp = 0;
		memset(ra_time_limit_store_buy_count, 0, sizeof(ra_time_limit_store_buy_count));
	}

	unsigned int ra_time_limit_store_begin_timestamp;
	unsigned short ra_time_limit_store_buy_count[RA_TIME_LIMIT_STORE_MAX_NUM];
	UNSTD_STATIC_CHECK(12 == RA_TIME_LIMIT_STORE_MAX_NUM);
};

UNSTD_STATIC_CHECK(sizeof(RATimeLimitStoreParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TIME_LIMIT_STORE_PARAM_HPP__
