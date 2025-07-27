#ifndef __SURFACE_DEF_HPP__
#define __SURFACE_DEF_HPP__
#include "servercommon/servercommon.h"

const static int SURFACE_MAX_NUM = 600;
const static int SURFACE_SUIT_MAX_NUM = 20;

#pragma pack(push, 4)

struct SurfaceParamData
{
	SurfaceParamData() { this->Reset(); }
	void Reset()
	{
		level = 0;
		reserve_sh = 0;
		end_time = 0;
	}
	short level;
	short reserve_sh;
	unsigned int end_time;
};

struct SurfaceParam
{
	SurfaceParam() { this->Reset(); }

	void Reset()
	{
		memset(wear_surface, 0, sizeof(wear_surface));
		for (int i = 0; i < SURFACE_MAX_NUM; ++i)
		{
			surface_list[i].Reset();
		}
	}

	short wear_surface[SURFACE_TYPE_MAX];
	SurfaceParamData surface_list[SURFACE_MAX_NUM];
};

typedef char SurfaceListParamHex[sizeof(SurfaceParam::wear_surface) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SurfaceListParamHex) < 64);


typedef char SurfaceParamHex[sizeof(SurfaceParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SurfaceParamHex) < 10240);

#pragma pack(pop)

#endif // !__Surface_DEF_HPP__
