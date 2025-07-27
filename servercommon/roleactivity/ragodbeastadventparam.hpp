#ifndef __RA_GOD_BEAST_ADVENT_PARAM_HPP__
#define __RA_GOD_BEAST_ADVENT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_BITMAP_GOD_BEAST_ADVENT = 32;			// bitmap��
static const int MAX_TASK_NUM_GOD_BEAST_ADVENT = 25;		// �����������ֵ
static const int MAX_REWARD_GROUP_GOD_BEAST_ADVENT = 20;	// ���߶һ��������ֵ

///////////////////////////////////////// ���������� ////////////////////////////////////////////////

struct RAGodBeastAdventBuyParam
{
	RAGodBeastAdventBuyParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cycle_flag = 0;
		reward_group = 0;
		activity_seq = 0;
	}

	unsigned int ra_begin_timestamp;		// ���ʼʱ���
	int cycle_flag;							// �����flag
	int reward_group;						// ��ǰ����������
	int activity_seq;						// ���������,�����ͻ��˵�
};


///////////////////////////////////////// �������� //////////////////////////////////////////////////
struct RAGodBeastAdventParam
{
	RAGodBeastAdventParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		zhigou_flag = 0;
		reserve_ch = 0;
		cycle_flag = 0;
		cur_like = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(buy_limit, 0, sizeof(buy_limit));
	}

	void DayReset()
	{
		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
	}

	void CycleReset()
	{
		zhigou_flag = 0;
		cur_like = 0;

		today_task_finish.Reset();
		is_receive_reward.Reset();
		memset(today_task_count, 0, sizeof(today_task_count));
		memset(buy_limit, 0, sizeof(buy_limit));
	}

	unsigned int ra_begin_timestamp;

	char zhigou_flag;			// �Ƿ����˻��Ʊ
	char reserve_ch;
	short cycle_flag;			// ����flag

	unsigned int cur_like;			// ��ǰ�øж�

	///////// ������� //////////
	BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> today_task_finish;		// ÿ�������Ƿ����
	BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> is_receive_reward;		// ÿ���������Ƿ�����ȡ
	int today_task_count[MAX_TASK_NUM_GOD_BEAST_ADVENT];		// ÿ���������

	///////// �һ���� /////////
	short buy_limit[MAX_REWARD_GROUP_GOD_BEAST_ADVENT];			// �޹����߶һ�����
};
UNSTD_STATIC_CHECK(sizeof(RAGodBeastAdventParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif