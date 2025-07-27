#ifndef __RA_TIGER_LUCKY_MONEY_PARAM_HPP__
#define __RA_TIGER_LUCKY_MONEY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RATLM
{
	enum LUCKY_MONEY_TYPE		// 压岁钱奖励类型
	{
		LUCKY_MONEY_TYPE_GOLD = 0,	// 元宝
		LUCKY_MONEY_TYPE_ITEM = 1,	// 道具
		LUCKY_MONEY_TYPE_JADE = 2,	// 灵玉

		LUCKY_MONEY_TYPE_MAX,
	};

	enum RATLM_TASK_TYPE			// 完成任务类型
	{
		RATLM_TASK_TYPE_JING_JI_CHANG = 0,			//!< 竞技场
		RATLM_TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 穹顶之争
		RATLM_TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< 五方擂台
		RATLM_TASK_TYPE_XIAN_SHI_HUO_DONG = 3,		//!< 任意限时活动	[魔将入侵,家族答题,以一敌百,百鬼夜行,巨龙来袭,迷宫竞速,家族荣誉战,幻界战场,首席竞选,家族大乱斗,竞技对抗赛]
		RATLM_TASK_TYPE_CONSUME_TI_LI = 4,			//!< 使用体力
		RATLM_TASK_TYPE_JING_YING_SHI_LIAN = 5,		//!< 精英试炼
		RATLM_TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< 野外首领
		RATLM_TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< 远征魔冢
		RATLM_TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< 北斗七星
		RATLM_TASK_TYPE_ONLINE_TIME = 9,			//!< 在线时长
		RATLM_TASK_TYPE_WAN_LING_HUA_BEN = 10,		//!< 万灵话本

		RATLM_TASK_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(RATLM_TASK_TYPE_MAX <= 32);

};

#pragma pack(push, 4)

struct RATigerLuckyMoneyParam
{
	RATigerLuckyMoneyParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_online_time_timestamp = 0;
		is_can_receive = 0;
		lucky_money_num = 0;
		role_level = 0;
		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}
	
	void DayReset()
	{
		ra_online_time_timestamp = 0;
		task_status.Reset();
		task_reward_status.Reset();
		memset(task_extent, 0, sizeof(task_extent));
	}

	unsigned int ra_begin_timestamp;

	// 压岁钱累计数量
	int lucky_money_num;

	// 压岁钱是否可领取,0-不可领取
	short is_can_receive;

	// 玩家当日登录时等级
	short role_level;

	// 每日任务是否完成 [0-RATLM_TASK_TYPE_MAX(9)]
	BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_status;

	// 每日任务奖励是否已领取[0-RATLM_TASK_TYPE_MAX(9)]
	BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_reward_status;

	// 每日任务进度 [0-RATLM_TASK_TYPE_MAX(9)]
	int task_extent[RATLM::RATLM_TASK_TYPE_MAX];

	// 玩家在线时长任务时间戳
	unsigned int ra_online_time_timestamp;

};
UNSTD_STATIC_CHECK(sizeof(RATigerLuckyMoneyParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif