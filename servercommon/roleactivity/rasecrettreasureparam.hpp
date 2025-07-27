#ifndef __RA_SECRET_TREASURE_PARAM_HPP__
#define __RA_SECRET_TREASURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_SECRET_TREASURE_SHOP_ITEM_NUM = 100;				// ����۵�����Ŀ���ֵ
static const int MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM = 25;			// �׶γ��۵�����Ŀ���ֵ

struct RASecretTreasureParam
{
	RASecretTreasureParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		phase = 0;
		is_open_today_flag = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short phase;															//�׶�
	short is_open_today_flag;												//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
	short today_buy_times[MAX_SECRET_TREASURE_PHASE_SHOP_ITEM_NUM];			//ÿ���޹�
	short activity_buy_times[MAX_SECRET_TREASURE_SHOP_ITEM_NUM];			//��޹�
};
UNSTD_STATIC_CHECK(sizeof(RASecretTreasureParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif	//__RA_SECRET_TREASURE_PARAM_HPP__