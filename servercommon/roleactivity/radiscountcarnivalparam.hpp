#ifndef __RA_DISCOUNT_CARNIVAL_PARAM_HPP__
#define __RA_DISCOUNT_CARNIVAL_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2069 ���� - �ۿۿ�  //////////////////////////////////////////

static const int DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM = 16;			//��Ʒ�������

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RADiscountCarnivalParam
{
	RADiscountCarnivalParam() { this->Reset(); }
	void Reset()
	{
		ra_discount_carnival_begin_timestamp = 0;
		ra_discount_carnival_buy_flag.Reset();
		memset(ra_discount_carnival_buy_count, 0, sizeof(ra_discount_carnival_buy_count));
	}

	unsigned int ra_discount_carnival_begin_timestamp;
	BitMap<DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM> ra_discount_carnival_buy_flag;				//�����־(���� �������湺�����)
	unsigned char ra_discount_carnival_buy_count[DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM];		//��Ʒ�������
};

UNSTD_STATIC_CHECK(sizeof(RADiscountCarnivalParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_CARNIVAL_PARAM_HPP__
