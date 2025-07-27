#ifndef __RA_SERVER_COMPETITION_PARAM_HPP__
#define __RA_SERVER_COMPETITION_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int MAX_SERVER_COMPETITION_RANK_TYPE_NUM = 10;
static const int MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM = 32;

enum SEVER_COMPETITION_RANK_TYPE
{
	SEVER_COMPETITION_RANK_TYPE_BEGIN = 0,

	SEVER_COMPETITION_RANK_TYPE_ROLE_CAP = SEVER_COMPETITION_RANK_TYPE_BEGIN,				// ��ɫ����
	SEVER_COMPETITION_RANK_TYPE_PET_CAP = 1,				// ��������
	SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP = 2,			// �������
	SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP = 3,				// װ������
	SEVER_COMPETITION_RANK_TYPE_COLOSSEUM = 4,				// ����������
	SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE = 5,		// ��ħ֮������
	SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP = 6,			// �ۺ��������а�

	SEVER_COMPETITION_RANK_TYPE_MAX
};
UNSTD_STATIC_CHECK(MAX_SERVER_COMPETITION_RANK_TYPE_NUM >= SEVER_COMPETITION_RANK_TYPE_MAX);

struct RAServerCompetitionRankItem
{
	RAServerCompetitionRankItem() { this->Reset(); }

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));

		reserve_sh = 0;
		reserve_ch = 0;
		is_give_rand_activity_reward_flag = 0;
		rank_end_timestamp = 0;
	}

	long long reserve_ll[2];

	short reserve_sh;
	char reserve_ch;
	char is_give_rand_activity_reward_flag;			// ���������ͻ�����������㷢�ű�ʶ 0:δ���� 1:�ѷ���
	unsigned int rank_end_timestamp;
};

struct RAServerCompetitionParam
{
	RAServerCompetitionParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < ARRAY_LENGTH(rank_list); i++)
		{
			rank_list[i].Reset();
		}
	}

	RAServerCompetitionRankItem rank_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];		//�������а����ʱ���
};


struct RARoleRankTypeItem
{
	RARoleRankTypeItem() { this->Reset(); }
	
	void Reset()
	{
		reserve_sh = 0;
		reserve_ch = 0;
		is_fetch_rank_end_reward_flag = 0;
		max_rank_value = 0;
		reward_fetch_flag.Reset();
	}
	
	short reserve_sh;
	char reserve_ch;
	char is_fetch_rank_end_reward_flag;											// �Ƿ񾭹������а������ʶ 0:δ������ 1:������
	int max_rank_value;															// ��ڼ����ﵽ�ĸ��������͵����ֵ(������ȡ�����ж�)
	BitMap<MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM> reward_fetch_flag;			// ������ȡ��ʶ
};
UNSTD_STATIC_CHECK(MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM == 32);

//ȫ����ƴ
struct RARoleServerCompetitionParam
{
	RARoleServerCompetitionParam() { this->Reset(); }

	void Reset()
	{
		ra_server_competition_begin_timestamp = 0;
		ra_activity_begin_time_timestamp = 0;

		for (int i = 0; i < ARRAY_LENGTH(rank_info_list); i++)
		{
			rank_info_list[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(reserve_rank_list); i++)
		{
			reserve_rank_list[i].Reset();
		}
		is_fetch_day_gift_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
	}

	unsigned int ra_server_competition_begin_timestamp;
	unsigned int ra_activity_begin_time_timestamp;

	RARoleRankTypeItem rank_info_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];
	RARoleRankTypeItem reserve_rank_list[5];	//����

	char is_fetch_day_gift_flag;		// �Ƿ���ȡÿ����� 0:δ��ȡ 1:����ȡ
	char reserve_ch;
	short reserve_sh;
};
UNSTD_STATIC_CHECK(sizeof(RARoleServerCompetitionParam) <= sizeof(RoleActivityRoleData));
UNSTD_STATIC_CHECK(MAX_SERVER_COMPETITION_RANK_TYPE_NUM == 10);

#pragma pack(pop)

#endif