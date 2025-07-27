#ifndef __RA_COLLECT_WORD_PARAM_HPP__
#define __RA_COLLECT_WORD_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2068 ���� - ���ֻ  //////////////////////////////////////////

static const int RA_COLLECT_WORD_MAX_ENTRY_NUM = 20;  //���ֺ��������Ŀ�������ñ��������ܳ����������
static const int RA_COLLECT_WORD_MAX_CONSUME_ITEM = 5;  //���ֺ���һ���Ʒ ��Ҫ�����Ʒ����
static const int RA_COLLECT_WORD_MAX_DAY_GIFT_REWARD_NUM = 5;	//��������ÿ��������ĵ�����

UNSTD_STATIC_CHECK(RA_COLLECT_WORD_MAX_ENTRY_NUM % 2 == 0);

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RACollectWordParam
{
	RACollectWordParam() { this->Reset(); }
	void Reset()
	{	
		ra_collect_word_begin_timestamp = 0;
		ra_collect_word_day_gift_is_fetch = 0;
		reserve_sh = 0;
		for (int i = 0; i < ARRAY_LENGTH(ra_collect_word_exchange_num); ++i)
		{
			ra_collect_word_exchange_num[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(ra_collect_word_total_exchange_num); ++i)
		{
			ra_collect_word_total_exchange_num[i] = 0;
		}
	}

	unsigned int ra_collect_word_begin_timestamp;
	short ra_collect_word_day_gift_is_fetch;									//��������ÿ��������  0:δ��ȡ 1:����ȡ
	short reserve_sh;															//����
	unsigned short ra_collect_word_exchange_num[RA_COLLECT_WORD_MAX_ENTRY_NUM]; //��¼ÿ����Ŀ�Ķһ�����
	unsigned short ra_collect_word_total_exchange_num[RA_COLLECT_WORD_MAX_ENTRY_NUM]; //
};

UNSTD_STATIC_CHECK(sizeof(RACollectWordParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_COLLECT_WORD_PARAM_HPP__
