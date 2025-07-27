#ifndef __RA_PARTNER_EXCHANGE_PARAM_HPP__
#define __RA_PARTNER_EXCHANGE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2069 ����� - ���һ�  //////////////////////////////////////////

const static int MAX_RA_PARTNER_NUM = 10;			// ���һ�����

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAPartnerExchangeParam
{
	RAPartnerExchangeParam() { this->Reset(); }
	void Reset()
	{
		ra_partner_exchange_begin_timestamp = 0;
		memset(ra_partner_exchange_times, 0, sizeof(ra_partner_exchange_times));
	}

	unsigned int ra_partner_exchange_begin_timestamp;
	unsigned short ra_partner_exchange_times[MAX_RA_PARTNER_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAPartnerExchangeParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_PARTNER_EXCHANGE_PARAM_HPP__
