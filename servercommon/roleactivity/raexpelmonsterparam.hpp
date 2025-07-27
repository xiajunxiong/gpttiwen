#ifndef __RA_EXPEL_MONSTER_PARAM_HPP__
#define __RA_EXPEL_MONSTER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2056 随机活动 - 驱除妖兽  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAExpelMonsterParam
{
	RAExpelMonsterParam() { this->Reset(); }

	void Reset()
	{
		ra_expel_monster_begin_timestamp = 0;
		ra_expel_monster_num = 0;
		ra_expel_role_level = 0;
	}

	unsigned int ra_expel_monster_begin_timestamp;
	int ra_expel_monster_num;										// 驱除妖兽数量
	int ra_expel_role_level;
};

UNSTD_STATIC_CHECK(sizeof(RAExpelMonsterParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_EXPEL_MONSTER_PARAM_HPP__
