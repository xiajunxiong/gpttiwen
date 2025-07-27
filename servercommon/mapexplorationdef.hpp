#ifndef __MAP_EXPLORATION_DEF_H__
#define __MAP_EXPLORATION_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"
#pragma pack(push, 4)

static const int MAX_MAP_EXPLORATION_NUM = 512;			//最大探索数量

struct MapExplorationParam
{
	MapExplorationParam()
	{
		this->Reset();
	}

	void Reset()
	{
		m_map.Reset();
	}

	BitMap<MAX_MAP_EXPLORATION_NUM> m_map;
};
typedef char MapExplorationParamHex[sizeof(MapExplorationParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MapExplorationParamHex) < 256);
#pragma pack(pop)

#endif
