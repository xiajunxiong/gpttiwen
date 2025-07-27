#ifndef __RA_TIAN_MING_DIVINATION_PARAM_HPP__
#define __RA_TIAN_MING_DIVINATION_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2077 开服活动 - 天命卜卦  //////////////////////////////////////////

static const int RAND_ACTIVITY_TIANMING_DIVINATION_REWARD_MAX_SEQ = 22;						// 天命卜卦奖励最大奖池下标
static const int RAND_ACTIVITY_TIANMING_DIVINATION_TOTAL_MAX_ADD_LOT_TIMES = 54;			// 天命卜卦最大加注次数
static const int RAND_ACTIVITY_TIANMING_DIVINATION_EVERY_LOT_MAX_ADD_TIMES = 9;				// 天命卜卦每个签筒最大加注次数
static const int RAND_ACTIVITY_SECTION_MAX_COUNT = 20;

static const int RA_TIANMING_LOT_COUNT = 6;								// 可加注标签数量
static const int RA_TIANMING_REWARD_HISTORY_COUNT = 20;					// 奖励历史记录数量

struct RATianMingRewardHistoryItem
{
	RATianMingRewardHistoryItem() { this->Reset(); }

	void Reset()
	{
		seq = 0;
		add_lot = 0;
		reserve_sh = 0;
	}

	char seq;															// 奖池下标
	char add_lot;														// 获奖加注倍数
	short reserve_sh;
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RATianMingDivinationParam
{
	RATianMingDivinationParam() { this->Reset(); }
	void Reset()
	{
		ra_tianming_join_timestamp = 0;

		for (int i = 0; i < RA_TIANMING_LOT_COUNT; ++i)
		{
			ra_tianming_add_lot_times_list[i] = 1;
		}
		reserve_ch = 0;
		ra_tianming_reward_history_list_cur_index = 0;
		ra_tianming_free_chou_times = 0;
		for (int i = 0; i < RA_TIANMING_REWARD_HISTORY_COUNT; ++i)
		{
			ra_tianming_reward_history_list[i].Reset();
		}
		ra_tianming_today_chou_tims = 0;
	}

	unsigned int ra_tianming_join_timestamp;									// 玩家参与活动时间
	char ra_tianming_add_lot_times_list[RA_TIANMING_LOT_COUNT];					// 竹签加注次数列表
	char ra_tianming_reward_history_list_cur_index;								// 奖励历史记录列表中最旧奖励记录的下标
	char reserve_ch;
	short reserve_sh;
	short ra_tianming_free_chou_times;											// 天命卜卦活动 - 今天免费抽奖次数
	RATianMingRewardHistoryItem ra_tianming_reward_history_list[RA_TIANMING_REWARD_HISTORY_COUNT];	// 奖励历史记录
	unsigned int ra_tianming_today_chou_tims;
};

UNSTD_STATIC_CHECK(sizeof(RATianMingDivinationParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_TIAN_MING_DIVINATION_PARAM_HPP__
