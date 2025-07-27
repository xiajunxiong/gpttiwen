#ifndef __RA_ZONG_XIANG_PARAM_HPP__
#define __RA_ZONG_XIANG_PARAM_HPP__


#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

////////////////////////////////////////////  粽香满城 个人数据  //////////////////////////////////////////////////////
const static int MAX_ZONG_XIANG_MAN_CHENG_FLAG = 32;
struct RAZongXiangManChengParam
{
	RAZongXiangManChengParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ra_begin_timestamp = 0;
		light_type = 0;
		m_flag_map.Reset();
	}

	void DayReset()
	{
		light_type = 0;
		m_flag_map.Reset();
	}


	unsigned int ra_begin_timestamp;
	
	int light_type;
	BitMap<MAX_ZONG_XIANG_MAN_CHENG_FLAG> m_flag_map;
};

////////////////////////////////////////////  咸甜之争 个人数据  //////////////////////////////////////////////////////

struct RASaltySweetBattleParam
{
	RASaltySweetBattleParam()
	{
		this->Reset();
	}

	void Reset()
	{
		vote_type = 0;
		reward_flag = 0;
		ra_begin_timestamp = 0;
	}

	int vote_type;
	int reward_flag;
	unsigned int ra_begin_timestamp;
};

///////////////////////////////////////////  咸甜之争 服务器数据 /////////////////////////////////////////////////////

enum SALTY_SWEET_MOD
{
	SALTY_SWEET_MOD_VOTE = 0,//投票
	SALTY_SWEET_MOD_COUNTING = 1,//计票
	SALTY_SWEET_MOD_RESULT = 2,//结果
};

enum SALTY_SWEET_TYPE
{
	SALTY_SWEET_TYPE_NO = 0,
	SALTY_SWEET_TYPE_SALTY = 1,//咸
	SALTY_SWEET_TYPE_SWEET = 2,//甜
};

struct RASaltySweetBattleServerParam
{
	RASaltySweetBattleServerParam() { this->Reset(); }

	void Reset()
	{
		pre_win_type = SALTY_SWEET_TYPE_NO;
		salty_value = 0;
		sweet_value = 0;
		status_mod = SALTY_SWEET_MOD_RESULT;
		add_value_tamp = 0;
		activity_end_tamp = 0;
	}

	void NextReset()
	{
		salty_value = 0;
		sweet_value = 0;
		status_mod = SALTY_SWEET_MOD_RESULT;
		activity_end_tamp = 0;
		add_value_tamp = 0;
	}

	int pre_win_type;

	int salty_value;
	int sweet_value;
	int status_mod;
	unsigned int activity_end_tamp;
	unsigned int add_value_tamp;
};

UNSTD_STATIC_CHECK(sizeof(RASaltySweetBattleServerParam) <= sizeof(RandActivityData));

#pragma pack(pop)
#endif