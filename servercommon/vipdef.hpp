#ifndef __VIP_DEF_HPP__
#define __VIP_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"

static const int MAX_VIP_LEVEL = 15;						// vip等级限制

#pragma pack(push, 4)

struct VipParam
{
	VipParam() { this->Reset(); }

	void Reset()
	{
		vip_level = 0;
		vip_exp = 0;
	}

	int vip_level;
	int vip_exp;
};

typedef char VipParamHex[sizeof(VipParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(VipParamHex) < 128);

#pragma pack(pop)

#endif  // __MENTALITY_DEF_HPP__

