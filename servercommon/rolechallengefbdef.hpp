#ifndef __ROLE_CHALLENGE_FB_DEF_HPP__
#define __ROLE_CHALLENGE_FB_DEF_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int PARAM_1_LIST_COUNT = 5;				// ����1 �б���
static const int ITEM_REWARD_LIST_COUNT = 5;			// ���߽����������
static const int CHALLENGE_FB_ID_MAX = 10;				// ��󸱱�ID

enum CHALLENGE_FB_PLAY_TYPE
{
	CF_TYPE_BASICAL_PLAY_TYPE = 0,			// ��һ������
	CF_TYPE_KILL_APPO_MON_NUM = 1,			// ��ɱָ�������Ĺ���
	CF_TYPE_KILL_MON_IN_ORDER = 2,			// ��˳���ɱ����

	CF_TYPE_MAX
};

struct ChallengeFbParam
{
	ChallengeFbParam() { this->Reset(); }

	void Reset()
	{
		memset(week_highest_layer, 0, sizeof(week_highest_layer));
		reserve_ush = 0;
	}

	UInt8 week_highest_layer[CHALLENGE_FB_ID_MAX];
	UInt16 reserve_ush;
};
typedef char ChallengeFbParamHex[sizeof(ChallengeFbParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ChallengeFbParamHex) < 128);

#pragma pack(pop)
#endif