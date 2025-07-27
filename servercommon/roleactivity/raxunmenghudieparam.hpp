#ifndef __RA_XUNMENG_HUDIE_PARAM_HPP__
#define __RA_XUNMENG_HUDIE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 寻梦蝴蝶 //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////


struct RAXunMengHuDieParam
{
	RAXunMengHuDieParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
	}

	unsigned int ra_begin_timestamp;
	int ra_frequency_num;				// 每日次数
};

UNSTD_STATIC_CHECK(sizeof(RAXunMengHuDieParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_XUNMENG_HUDIE_PARAM_HPP__
