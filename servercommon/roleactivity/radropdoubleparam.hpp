#ifndef __RA_DROP_DOUBLE_PARAM_HPP__
#define __RA_DROP_DOUBLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2066 开服活动 - 掉落翻倍  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RADropDoubleParam
{
	RADropDoubleParam() { this->Reset(); }
	void Reset()
	{
		ra_drop_double_begin_timestamp = 0;
	}

	unsigned int ra_drop_double_begin_timestamp;

};

UNSTD_STATIC_CHECK(sizeof(RADropDoubleParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DROP_DOUBLE_PARAM_HPP__
