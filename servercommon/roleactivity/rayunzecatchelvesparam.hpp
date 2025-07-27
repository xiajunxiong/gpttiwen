#ifndef __RA_YUN_ZE_CATCH_ELVES_PARAM_HPP__
#define __RA_YUN_ZE_CATCH_ELVES_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

struct RAYunZeCatchElvesParam
{
	RAYunZeCatchElvesParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
	}

	unsigned int ra_begin_timestamp;
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeCatchElvesParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)

#endif