#ifndef __RA_SANBAI_CHOU_PARAM_HPP__
#define __RA_SANBAI_CHOU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 三百抽皮肤 //////////////////////////////////////////

static const int MAX_RA_SANBAI_CHOU_TYPE_NUM = 10;			// 抽奖等级区间不能超过10个
static const int MAX_RA_SANBAI_CHOU_DRAW_NUM = 10;			// 十连抽

struct RASanBaiChouTypeParam
{
	RASanBaiChouTypeParam() : times(0), select_seq(-1) 
	{
		memset(pool, -1, sizeof(pool));
	}

	void Reset()
	{
		times = 0;
		select_seq = -1;
		memset(pool, -1, sizeof(pool));
	}

	short times;
	short select_seq;
	short pool[MAX_RA_SANBAI_CHOU_DRAW_NUM];
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RASanBaiChouParam
{
	RASanBaiChouParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		baodi_total_times = 0;

		reward_item_seq = -1;
		
		for (int i = 0; i < MAX_RA_SANBAI_CHOU_TYPE_NUM; i++)
		{
			select[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	int draw_times;													// 总共抽奖次数
	short baodi_total_times;										// 没有抽到珍稀奖励的累积次数，用来判断是否给低保	
	short reward_item_seq;											// 最终选择的奖励

	RASanBaiChouTypeParam select[MAX_RA_SANBAI_CHOU_TYPE_NUM];		// 类型参数
};

UNSTD_STATIC_CHECK(sizeof(RASanBaiChouParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SANBAI_CHOU_PARAM_HPP__
