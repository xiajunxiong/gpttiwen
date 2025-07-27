#ifndef __SCHOOL_TASK_DEF_H__
#define __SCHOOL_TASK_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "bitmapfunction.hpp"

#pragma once
#pragma pack(push, 4)

struct SchoolTaskData
{
	SchoolTaskData() { this->Reset(); }
	void Reset()
	{
		task_id = 0;
		progress = 0;
	}

	int task_id;
	int progress;
};

static const int SCHOOL_TASK_NUM = 8;
static const int MAX_SCHOOL_TASK_NUM = 200;

enum SCHOOL_TASK_STATUS
{
	SCHOOL_TASK_STATUS_DEFAULT = 0,		// 未领取
	SCHOOL_TASK_STATUS_ONGOING,			// 进行中
	SCHOOL_TASK_STATUS_FINISH,			// 已完成
};

struct SchoolTaskParam
{
	SchoolTaskParam() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < SCHOOL_TASK_NUM; i++)
		{
			task_status[i] = SCHOOL_TASK_STATUS_DEFAULT;
			task_list[i].Reset();
		}
		reward_find_count = 0;
	}

	char task_status[SCHOOL_TASK_NUM];
	SchoolTaskData task_list[SCHOOL_TASK_NUM];
	int reward_find_count;
};

typedef char SchoolTaskParamHex[sizeof(SchoolTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SchoolTaskParamHex) < 512);

#pragma pack(pop)

#endif // !__SCHOOL_TASK_DEF_H__
