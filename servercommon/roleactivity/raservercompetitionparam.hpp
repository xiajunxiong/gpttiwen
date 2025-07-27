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

	SEVER_COMPETITION_RANK_TYPE_ROLE_CAP = SEVER_COMPETITION_RANK_TYPE_BEGIN,				// 角色评分
	SEVER_COMPETITION_RANK_TYPE_PET_CAP = 1,				// 宠物评分
	SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP = 2,			// 伙伴评分
	SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP = 3,				// 装备评分
	SEVER_COMPETITION_RANK_TYPE_COLOSSEUM = 4,				// 竞技场排名
	SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE = 5,		// 锢魔之塔排名
	SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP = 6,			// 综合评分排行榜

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
	char is_give_rand_activity_reward_flag;			// 该排行类型活动排名奖励结算发放标识 0:未发放 1:已发放
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

	RAServerCompetitionRankItem rank_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];		//各个排行榜结束时间戳
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
	char is_fetch_rank_end_reward_flag;											// 是否经过该排行榜结束标识 0:未经历过 1:经历过
	int max_rank_value;															// 活动期间所达到的该排行类型的最大值(用于领取奖励判断)
	BitMap<MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM> reward_fetch_flag;			// 奖励领取标识
};
UNSTD_STATIC_CHECK(MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM == 32);

//全服比拼
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
	RARoleRankTypeItem reserve_rank_list[5];	//保留

	char is_fetch_day_gift_flag;		// 是否领取每日礼包 0:未领取 1:已领取
	char reserve_ch;
	short reserve_sh;
};
UNSTD_STATIC_CHECK(sizeof(RARoleServerCompetitionParam) <= sizeof(RoleActivityRoleData));
UNSTD_STATIC_CHECK(MAX_SERVER_COMPETITION_RANK_TYPE_NUM == 10);

#pragma pack(pop)

#endif