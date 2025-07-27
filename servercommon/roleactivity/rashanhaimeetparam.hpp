#ifndef __RA_SHAN_HAI_MEET_PARAM_HPP__
#define __RA_SHAN_HAI_MEET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2140 ɽ�����  //////////////////////////////////////////

enum RA_SHAN_HAI_MEET_ACT_TYPE
{
	RA_SHAN_HAI_MEET_ACT_TYPE_0 = 0,							// 2122 ɽ���ر�	����x��
	RA_SHAN_HAI_MEET_ACT_TYPE_1,								// 2134 ������		���x��
	RA_SHAN_HAI_MEET_ACT_TYPE_2,								// 2124 ¶������	����x��boss
	RA_SHAN_HAI_MEET_ACT_TYPE_3,								// 2142 ɽ���ʴ�	����x���ʴ�
	RA_SHAN_HAI_MEET_ACT_TYPE_4,								// 2140 ɽ������	����x��
	RA_SHAN_HAI_MEET_ACT_TYPE_5,								// 2122 ɽ���ر�	����x��
	RA_SHAN_HAI_MEET_ACT_TYPE_6,								// 2125 ɽ������	����x��
	RA_SHAN_HAI_MEET_ACT_TYPE_7,								// 2156 ɽ���ٱ�	����x��
	RA_SHAN_HAI_MEET_ACT_TYPE_8,								// 2156 ɽ���ٱ�	����x��

	RA_SHAN_HAI_MEET_ACT_TYPE_END,
};

static const int RA_SHAN_HAI_MEET_ACT_TYPE_MAX = 16;			//������������
UNSTD_STATIC_CHECK(RA_SHAN_HAI_MEET_ACT_TYPE_END <= RA_SHAN_HAI_MEET_ACT_TYPE_MAX);
static const int RA_SHAN_HAI_MEET_TASK_ID_MAX = 32;				//��������
static const int RA_SHAN_HAI_MEET_POKER_MAX_INDEX = 6;			//��������±�

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAShanHaiMeetParam
{
	RAShanHaiMeetParam() { this->Reset(); }
	void Reset()
	{
		begin_timestamp = 0;
		is_buy = 0;
		sh_server = 0;
		poker_count = 0;

		memset(finish_count, 0, sizeof(finish_count));
		fetch_flag.Reset();
		poker_idx_flag.Reset();
		memset(poker_item_seq, -1, sizeof(poker_item_seq));
	}

	unsigned int	begin_timestamp;
	char			is_buy;												// 1:�ѹ��� 0:δ����
	char			sh_server;
	short			poker_count;										// �ɷ��ƴ���(����)

	int				finish_count[RA_SHAN_HAI_MEET_ACT_TYPE_MAX];
	BitMap<RA_SHAN_HAI_MEET_TASK_ID_MAX> fetch_flag;

	BitMap<RA_SHAN_HAI_MEET_POKER_MAX_INDEX> poker_idx_flag;
	int poker_item_seq[RA_SHAN_HAI_MEET_POKER_MAX_INDEX];				//���ƽ�������(���ͻ���չʾ��)
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiMeetParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SHAN_HAI_MEET_PARAM_HPP__
