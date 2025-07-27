#ifndef __RA_SIGN_IN_CONTINUITY_PARAM_HPP__
#define __RA_SIGN_IN_CONTINUITY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2083 开服活动 - 连登好礼  //////////////////////////////////////////

static const int RA_SIGN_IN_CONTINUITY_TYPE_MAXNUM = 32;		//连登好礼最大类型

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASignInContinuityParam
{
	RASignInContinuityParam() { this->Reset(); }
	void Reset()
	{
		ra_sign_in_continuity_begin_timestamp = 0;
		ra_sign_in_continuity_login_day = 0;
		ra_sign_in_continuity_receive_flag.Reset();
		ra_sign_in_continuity_day_id = 0;
	}

	unsigned int ra_sign_in_continuity_begin_timestamp;
	int ra_sign_in_continuity_login_day;
	BitMap<RA_SIGN_IN_CONTINUITY_TYPE_MAXNUM> ra_sign_in_continuity_receive_flag;
	unsigned int ra_sign_in_continuity_day_id;
};

UNSTD_STATIC_CHECK(sizeof(RASignInContinuityParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SIGN_IN_CONTINUITY_PARAM_HPP__
