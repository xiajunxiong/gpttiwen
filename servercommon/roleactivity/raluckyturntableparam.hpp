#ifndef __RA_LUCKY_TURN_TABLE_PARAM_HPP__
#define __RA_LUCKY_TURN_TABLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2076 开服活动 - 幸运转盘  //////////////////////////////////////////

static const int MAX_RA_LUCKY_TURNTABLE_TYPE_NUM = 20;
static const int MAX_RA_LUCKY_TURNTABLE_REWARD_NUM = 8;		//一个转盘最多显示8个
enum RA_LUCKY_TURNTABLE_TYPE
{
	RA_LUCKY_TURNTABLE_TYPE_SCHOOL_TASK = 1,				// 1  学院任务（门派任务）
	RA_LUCKY_TURNTABLE_TYPE_ZHEN_ZHONG = 2,					// 2  真中的研究(三界除魔)
	RA_LUCKY_TURNTABLE_TYPE_DEMON_CAVE = 3,					// 3  魔窟（万妖谷)
	RA_LUCKY_TURNTABLE_TYPEARRESTING_THIEVES = 4,			// 4  追缉盗贼
	RA_LUCKY_TURNTABLE_TYPE_WST_QUESTION = 5,				// 5  万事通的考验 / 神算子的考验
	RA_LUCKY_TURNTABLE_TYPE_ESCORT = 6,						// 6  运镖
	RA_LUCKY_TURNTABLE_TYPE_BOUNTY_TASK = 7,				// 7  赏金任务
	RA_LUCKY_TURNTABLE_TYPE_TRANSACTION_TASK = 8,			// 8  购买任务 - 丝绸之路
	RA_LUCKY_TURNTABLE_TYPE_JINGJICHANG = 9,				// 9  参与竞技场
	RA_LUCKY_TURNTABLE_TYPE_DREAM_NORES_CHALLENGE = 10,		// 10 通关/扫荡梦渊笔录
	RA_LUCKY_TURNTABLE_TYPE_REALM_BREAK_SOUL_FIGHT = 11,	// 11 境界-心灵之境通关
	RA_LUCKY_TURNTABLE_TYPE_CRYSTAL_JING_YING = 12,			// 12 精英试炼通关

	RA_LUCKY_TURNTABLE_TYPE_MAX,
};
UNSTD_STATIC_CHECK(RA_LUCKY_TURNTABLE_TYPE_MAX <= MAX_RA_LUCKY_TURNTABLE_TYPE_NUM)

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

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
	short ra_lucky_turntable_last_num;									//剩余次数
	short ra_lucky_turntable_last_index;								//上次抽取的index
	char ra_lucky_turntable_schedule[MAX_RA_LUCKY_TURNTABLE_TYPE_NUM];	//各个条件任务的进度
	BitMap<MAX_RA_LUCKY_TURNTABLE_TYPE_NUM> ra_lucky_turntable_flag;	//各个条件任务领取标识
};

UNSTD_STATIC_CHECK(MAX_RA_LUCKY_TURNTABLE_TYPE_NUM == 20);

UNSTD_STATIC_CHECK(sizeof(RALuckyTurnTableParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LUCKY_TURN_TABLE_PARAM_HPP__
