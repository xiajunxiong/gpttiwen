#ifndef __RA_LUCKY_DISCOUNT_PARAM_HPP__
#define __RA_LUCKY_DISCOUNT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2071 ���� - �����ۿ�  //////////////////////////////////////////

static const int LUCKY_DISCOUNT_COMMODITY_MAX_NUM = 24;
static const int LUCKY_DISCOUNT_BEGIN_DISCOUNT = 10;			//Ĭ�ϳ�ʼ�ۿ� 10�ۣ������ۣ�

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RALuckyDiscountParam
{
	RALuckyDiscountParam() { this->Reset(); }
	void Reset()
	{
		ra_lucky_discount_begin_timestamp = 0;
		ra_lucky_discount_discount = LUCKY_DISCOUNT_BEGIN_DISCOUNT;
		memset(ra_lucky_discount_buy_count, 0, sizeof(ra_lucky_discount_buy_count));
		memset(ra_lucky_discount_today_buy_count, 0, sizeof(ra_lucky_discount_today_buy_count));
	}

	unsigned int ra_lucky_discount_begin_timestamp;
	int ra_lucky_discount_discount;													//�����ۿ�
	char ra_lucky_discount_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//�������
	char ra_lucky_discount_today_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//�������
};

UNSTD_STATIC_CHECK(sizeof(RALuckyDiscountParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_DISCOUNT_PARAM_HPP__
