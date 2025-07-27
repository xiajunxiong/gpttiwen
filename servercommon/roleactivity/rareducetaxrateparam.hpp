#ifndef __RA_REDUCE_TAX_RATE_PARAM_HPP__
#define __RA_REDUCE_TAX_RATE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2063 随机活动 - 税率优惠  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAReduceTaxRateParam
{
	RAReduceTaxRateParam() { this->Reset(); }

	void Reset()
	{
		ra_reduce_tax_rate_begin_timestamp = 0;

	}

	unsigned int ra_reduce_tax_rate_begin_timestamp;
};

UNSTD_STATIC_CHECK(sizeof(RAReduceTaxRateParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_REDUCE_TAX_RATE_PARAM_HPP__
