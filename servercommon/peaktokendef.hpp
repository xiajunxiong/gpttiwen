#ifndef __PEAK_TOKEN_DEF__
#define __PEAK_TOKEN_DEF__

#include "servercommon/servercommon.h"

///////////////////////天下至尊令///////////////////

#pragma pack(push, 4)
struct RolePeakTokenParam
{
	RolePeakTokenParam()
	{
		this->Reset();
	}
	void Reset()
	{
		today_use_times = 0;
	}

	int today_use_times;	//今日使用次数

};
typedef char RolePeakTokenParamHex[sizeof(RolePeakTokenParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RolePeakTokenParamHex) < 256);

#pragma pack(pop)

#endif