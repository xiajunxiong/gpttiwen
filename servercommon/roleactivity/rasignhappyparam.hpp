#ifndef __RA_SIGN_HAPPY_PARAM_HPP__
#define __RA_SIGN_HAPPY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 登录送好礼 //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RASignHappyParam
{
	RASignHappyParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		login_day = 0;
		is_add_day = 0;
		fetch_mark = 0;
	}

	unsigned int ra_begin_timestamp;
	short login_day;
	short is_add_day;
	int fetch_mark;

};

UNSTD_STATIC_CHECK(sizeof(RASignHappyParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SIGN_HAPPY_PARAM_HPP__
