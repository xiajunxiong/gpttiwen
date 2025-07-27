#ifndef __RA_DISCOUNT_STORE_PARAM_HPP__
#define __RA_DISCOUNT_STORE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2052 ����� - �ۿ��̵�  //////////////////////////////////////////

static const int RA_DISCOUNT_STORE_MAX_NUM = 30;

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RADiscountStoreParam
{
	RADiscountStoreParam() { this->Reset(); }

	void Reset()
	{
		ra_discount_store_begin_timestamp = 0;
		memset(ra_discount_store_buy_count, 0, sizeof(ra_discount_store_buy_count));
	}

	unsigned int ra_discount_store_begin_timestamp;
	unsigned short ra_discount_store_buy_count[RA_DISCOUNT_STORE_MAX_NUM];
	UNSTD_STATIC_CHECK(30 == RA_DISCOUNT_STORE_MAX_NUM);
};

UNSTD_STATIC_CHECK(sizeof(RADiscountStoreParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
