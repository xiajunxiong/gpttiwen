#ifndef __RA_TRUN_TABLE_PARAM_HPP__
#define __RA_TRUN_TABLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2051 开服活动 - 转盘活动  //////////////////////////////////////////

static const int RA_TURN_TABLE_MAX_NUM = 8;

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RATrunTableParam
{
	RATrunTableParam() { this->Reset(); }

	void Reset()
	{
		ra_turn_table_begin_timestamp = 0;
		ra_has_free_turn_table_num = 0;
		ra_free_turn_table_after_timestamp = 0;
		kill_boss_num = 0;
		turn_table_index = -1;
		before_use_turn_table_type = 0;
	}

	unsigned int ra_turn_table_begin_timestamp;
	int ra_has_free_turn_table_num;							// 免费转盘次数
	unsigned int ra_free_turn_table_after_timestamp;		// 免费次数抽取后下一次的时间
	int kill_boss_num;										// 击杀BOSS数量
	int turn_table_index;									// 上一次抽取的道具index
	int before_use_turn_table_type;							// 上一次使用的转盘类型
};

UNSTD_STATIC_CHECK(sizeof(RATrunTableParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TRUN_TABLE_PARAM_HPP__
