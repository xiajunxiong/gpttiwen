#ifndef __RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__
#define __RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 魔族在哪里 //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAMoZuZaiNaLiParam
{
	RAMoZuZaiNaLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
	}

	unsigned int ra_begin_timestamp;
	int ra_frequency_num;								// 驱魔次数
};

UNSTD_STATIC_CHECK(sizeof(RAMoZuZaiNaLiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__
