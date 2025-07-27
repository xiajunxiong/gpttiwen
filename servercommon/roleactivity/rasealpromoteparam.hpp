#ifndef __RA_SEAL_PROMOTE_PARAM_HPP__
#define __RA_SEAL_PROMOTE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2065 随机活动 - 封印提升  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASealPromoteParam
{
	RASealPromoteParam() { this->Reset(); }
	void Reset()
	{
		ra_seal_promote_begin_timestamp = 0;
	}

	unsigned int ra_seal_promote_begin_timestamp;
};

UNSTD_STATIC_CHECK(sizeof(RASealPromoteParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SEAL_PROMOTE_PARAM_HPP__
