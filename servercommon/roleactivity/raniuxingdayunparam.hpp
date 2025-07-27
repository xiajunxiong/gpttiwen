#ifndef __RA_NIU_XING_DA_YUN_PARAM_HPP__
#define __RA_NIU_XING_DA_YUN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RANiuXingDaYunParam
{
	RANiuXingDaYunParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		today_reward = 0;
		type_id = 0;
		begin_time = 0;
	}

	int open_day;
	int today_reward;
	int type_id;
	unsigned int begin_time;
};

UNSTD_STATIC_CHECK(sizeof(RANiuXingDaYunParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
