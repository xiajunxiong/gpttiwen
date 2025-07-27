#ifndef __RA_PARTNER_EXCHANGE_PARAM_HPP__
#define __RA_PARTNER_EXCHANGE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2069 开服活动 - 伙伴兑换  //////////////////////////////////////////

const static int MAX_RA_PARTNER_NUM = 10;			// 最多兑换种类

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

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
