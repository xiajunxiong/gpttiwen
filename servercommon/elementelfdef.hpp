#ifndef __ELEMENT_ELF_DEF_HPP__
#define __ELEMENT_ELF_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

struct ElementElfParam
{
	ElementElfParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		today_showup_times = 0;
		reserve_sh = 0;
	}

	long long reserve_ll[4];
	short today_showup_times;
	short reserve_sh;
};

typedef char ElementElfParamHex[sizeof(ElementElfParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ElementElfParamHex) < 128);

#pragma pack(pop)

#endif