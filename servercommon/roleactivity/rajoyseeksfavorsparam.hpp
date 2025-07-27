#ifndef __RA_JOY_SEEKS_FAVORS_PARAM_HPP__
#define __RA_JOY_SEEKS_FAVORS_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2067 随机活动 - 欢乐寻宠  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAJoySeeksFavorsParam
{
	RAJoySeeksFavorsParam() { this->Reset(); }
	void Reset()
	{
		ra_joy_seeks_favors_begin_timestamp = 0;
		ra_joy_seeks_favors_role_level = 0;
	}

	unsigned int ra_joy_seeks_favors_begin_timestamp;
	int ra_joy_seeks_favors_role_level;//活动开启时玩家等级
};

UNSTD_STATIC_CHECK(sizeof(RAJoySeeksFavorsParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_JOY_SEEKS_FAVORS_PARAM_HPP__
