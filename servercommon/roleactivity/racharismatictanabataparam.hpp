#ifndef __RA_CHARISMATIC_TANABATA_PARAM_HPP__
#define __RA_CHARISMATIC_TANABATA_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

const static int MAX_CHARIS_FLAG = 32;//最大奖励数量

struct RACharismaticTanabataParam
{
	RACharismaticTanabataParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		admiration_value = 0;
		admiration_map.Reset();
		charm_value = 0;
		charm_map.Reset();
		begin_time = 0;
	}

	int open_day;

	int admiration_value;//爱慕值
	BitMap<MAX_CHARIS_FLAG> admiration_map;
	int charm_value;//魅力值
	BitMap<MAX_CHARIS_FLAG> charm_map;

	unsigned int begin_time;
};

UNSTD_STATIC_CHECK(sizeof(RACharismaticTanabataParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
