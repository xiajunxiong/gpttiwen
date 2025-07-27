#ifndef __RA_HORCRXU_TIANCHENG_PARAM_HPP__
#define __RA_HORCRXU_TIANCHENG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ������� //////////////////////////////////////////
static const int MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX = 20;				// �̳���������
static const int MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX = 15;			// �һ���������

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
struct RandActivityHorcruxTianChengParam
{
	RandActivityHorcruxTianChengParam() { this->Reset(); }

	void Reset()
	{
		draw_times = 0;
		jackpot_type = -2;
		item_id = 0;
		jackpot_num = 0;
	}
	
	short draw_times;
	short jackpot_type;
	unsigned short item_id;
	unsigned int jackpot_num;
};


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

const static int MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM = 64;//������ɵ�ʱ�����������������

struct RAHorcruxTianChengParam
{
	RAHorcruxTianChengParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		memset(shop_list, 0, sizeof(shop_list));
		memset(exchange_list, 0, sizeof(exchange_list));

		consume_fetch_mark = 0;
		consume_gold = 0;

		rand_times = 0;
		floor_bitmap.Reset();
	}

	unsigned int ra_begin_timestamp;

	short shop_list[MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX];
	short exchange_list[MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX];
	
	short consume_fetch_mark;
	int consume_gold;

	int rand_times;
	BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> floor_bitmap;
};

UNSTD_STATIC_CHECK(sizeof(RAHorcruxTianChengParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_HORCRXU_TIANCHENG_PARAM_HPP__
