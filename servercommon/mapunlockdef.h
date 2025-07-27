#ifndef __MAP_UNLOCK_DEF_H__
#define __MAP_UNLOCK_DEF_H__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int SCENE_ENTER_RECORD_LIST_NUM_MAX = 157;		// 场景列表最大数量
static const int ECTYPE_PASS_RECORD_LIST_NUM_MAX = 30;		// 副本列表最大数量
static const int IRREGULAR_ID_LIST_NUM_MAX = 2;				// 不规则ID列表最大数量
static const int IRREGULAR_ID_BEGIN = 1;					// 不规则ID 从 1 开始

struct MapUnlockParam
{
	MapUnlockParam() { this->Reset(); }

	void Reset()
	{
		memset(scene_enter_flag, 0, sizeof(scene_enter_flag));
		memset(ectype_pass_flag, 0, sizeof(ectype_pass_flag));
		memset(ectype_enter_flag, 0, sizeof(ectype_enter_flag));
	}

	long long scene_enter_flag[SCENE_ENTER_RECORD_LIST_NUM_MAX];		// 场景是否曾进入过
	long long ectype_pass_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];		// 副本是否曾通关过
	long long ectype_enter_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];		// 副本是否曾进入过
};
typedef char MapUnlockParamHex[sizeof(MapUnlockParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MapUnlockParamHex) < 4096);

#pragma pack(pop)
#endif