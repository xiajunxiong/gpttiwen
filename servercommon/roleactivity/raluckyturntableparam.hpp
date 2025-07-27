#ifndef __RA_LUCKY_TURN_TABLE_PARAM_HPP__
#define __RA_LUCKY_TURN_TABLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2076 ����� - ����ת��  //////////////////////////////////////////

static const int MAX_RA_LUCKY_TURNTABLE_TYPE_NUM = 20;
static const int MAX_RA_LUCKY_TURNTABLE_REWARD_NUM = 8;		//һ��ת�������ʾ8��
enum RA_LUCKY_TURNTABLE_TYPE
{
	RA_LUCKY_TURNTABLE_TYPE_SCHOOL_TASK = 1,				// 1  ѧԺ������������
	RA_LUCKY_TURNTABLE_TYPE_ZHEN_ZHONG = 2,					// 2  ���е��о�(�����ħ)
	RA_LUCKY_TURNTABLE_TYPE_DEMON_CAVE = 3,					// 3  ħ�ߣ�������)
	RA_LUCKY_TURNTABLE_TYPEARRESTING_THIEVES = 4,			// 4  ׷������
	RA_LUCKY_TURNTABLE_TYPE_WST_QUESTION = 5,				// 5  ����ͨ�Ŀ��� / �����ӵĿ���
	RA_LUCKY_TURNTABLE_TYPE_ESCORT = 6,						// 6  ����
	RA_LUCKY_TURNTABLE_TYPE_BOUNTY_TASK = 7,				// 7  �ͽ�����
	RA_LUCKY_TURNTABLE_TYPE_TRANSACTION_TASK = 8,			// 8  �������� - ˿��֮·
	RA_LUCKY_TURNTABLE_TYPE_JINGJICHANG = 9,				// 9  ���뾺����
	RA_LUCKY_TURNTABLE_TYPE_DREAM_NORES_CHALLENGE = 10,		// 10 ͨ��/ɨ����Ԩ��¼
	RA_LUCKY_TURNTABLE_TYPE_REALM_BREAK_SOUL_FIGHT = 11,	// 11 ����-����֮��ͨ��
	RA_LUCKY_TURNTABLE_TYPE_CRYSTAL_JING_YING = 12,			// 12 ��Ӣ����ͨ��

	RA_LUCKY_TURNTABLE_TYPE_MAX,
};
UNSTD_STATIC_CHECK(RA_LUCKY_TURNTABLE_TYPE_MAX <= MAX_RA_LUCKY_TURNTABLE_TYPE_NUM)

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RALuckyTurnTableParam
{
	RALuckyTurnTableParam() { this->Reset(); }

	void Reset()
	{
		ra_lucky_turntable_begin_timestamp = 0;
		ra_lucky_turntable_last_num = 0;
		ra_lucky_turntable_last_index = -1;
		memset(ra_lucky_turntable_schedule, 0, sizeof(ra_lucky_turntable_schedule));
		ra_lucky_turntable_flag.Reset();
	}

	unsigned int ra_lucky_turntable_begin_timestamp;
	short ra_lucky_turntable_last_num;									//ʣ�����
	short ra_lucky_turntable_last_index;								//�ϴγ�ȡ��index
	char ra_lucky_turntable_schedule[MAX_RA_LUCKY_TURNTABLE_TYPE_NUM];	//������������Ľ���
	BitMap<MAX_RA_LUCKY_TURNTABLE_TYPE_NUM> ra_lucky_turntable_flag;	//��������������ȡ��ʶ
};

UNSTD_STATIC_CHECK(MAX_RA_LUCKY_TURNTABLE_TYPE_NUM == 20);

UNSTD_STATIC_CHECK(sizeof(RALuckyTurnTableParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_TURN_TABLE_PARAM_HPP__
