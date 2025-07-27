#ifndef __MAP_UNLOCK_DEF_H__
#define __MAP_UNLOCK_DEF_H__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int SCENE_ENTER_RECORD_LIST_NUM_MAX = 157;		// �����б��������
static const int ECTYPE_PASS_RECORD_LIST_NUM_MAX = 30;		// �����б��������
static const int IRREGULAR_ID_LIST_NUM_MAX = 2;				// ������ID�б��������
static const int IRREGULAR_ID_BEGIN = 1;					// ������ID �� 1 ��ʼ

struct MapUnlockParam
{
	MapUnlockParam() { this->Reset(); }

	void Reset()
	{
		memset(scene_enter_flag, 0, sizeof(scene_enter_flag));
		memset(ectype_pass_flag, 0, sizeof(ectype_pass_flag));
		memset(ectype_enter_flag, 0, sizeof(ectype_enter_flag));
	}

	long long scene_enter_flag[SCENE_ENTER_RECORD_LIST_NUM_MAX];		// �����Ƿ��������
	long long ectype_pass_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];		// �����Ƿ���ͨ�ع�
	long long ectype_enter_flag[ECTYPE_PASS_RECORD_LIST_NUM_MAX];		// �����Ƿ��������
};
typedef char MapUnlockParamHex[sizeof(MapUnlockParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MapUnlockParamHex) < 4096);

#pragma pack(pop)
#endif