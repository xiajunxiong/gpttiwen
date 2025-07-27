#ifndef __RA_ACTIVITY_HUN_QI_FAN_LI_PARAM_HPP__
#define __RA_ACTIVITY_HUN_QI_FAN_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


#pragma pack(push,4)

////////////////////////////////// 魂器返利 ///////////////////////////////////////
static const int MAX_RA_HUN_QI_FAN_LI_BITMIP = 32;

struct RAHunQiFanLiParam
{
	RAHunQiFanLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		m_last_lottery_count = 0;
		m_today_recharge = 0;
		m_total_lottery_count = 0;
		m_recharge_reward_flag.Reset();
		m_lottery_reward_flag.Reset();
		m_cur_rotary_table_state.Reset();
	}

	void DayReset()
	{
		m_today_recharge = 0;
		m_recharge_reward_flag.Reset();
	}

	unsigned int ra_begin_timestamp;

	uint32_t m_last_lottery_count;	// 玩家剩余抽奖次数
	int m_total_lottery_count;		// 玩家总抽奖次数
	int m_today_recharge;			// 当日充值金额
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_recharge_reward_flag;		// 充值奖励领取flag
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_lottery_reward_flag;		// 抽奖次数奖励领取flag
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_cur_rotary_table_state;	// 当前轮盘状态, 已抽到过的奖励置为1,全抽完后刷新
};
UNSTD_STATIC_CHECK(sizeof(RAHunQiFanLiParam) <= sizeof(RoleActivityRoleData));

////////////////////////////////// 天衣耀世 ///////////////////////////////////////

static const int MAX_RA_TIAN_YI_YAO_SHI_LOG = 50;	// 记录50条抽奖日志
static const int MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE = 10;// 商城最大购买类型

enum RA_TIAN_YI_YAO_SHI_BUY_LIMIT
{
	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_TOTAL = 1,	// 总限购
	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_TODAY = 2,	// 每日限购

	RA_TIAN_YI_YAO_SHI_BUY_LIMIT_MAX,
};

// 天衣耀世日志
struct RATianYiYaoShiLOG
{
	RATianYiYaoShiLOG() { this->Reset(); }

	void Reset()
	{
		lottery_timestamp = 0;
		item_id = 0;
		num = 0;
	}

	uint32_t lottery_timestamp;		// 抽奖时的时间戳
	ItemID item_id;
	uint16_t num;
};

struct RATianYiYaoShiParam
{
	RATianYiYaoShiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		cur_log_tail = 0;
		memset(m_buy_count, 0, sizeof(m_buy_count));

		for (int i = 0; i < MAX_RA_TIAN_YI_YAO_SHI_LOG; i++)
		{
			m_lottery_log[i].Reset();
		}
	}


	unsigned int ra_begin_timestamp;

	uint32_t cur_log_tail;		// 当前日志尾部(下一条要被替换的index)

	uint16_t m_buy_count[MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE];		// 购买礼包次数
	RATianYiYaoShiLOG m_lottery_log[MAX_RA_TIAN_YI_YAO_SHI_LOG];	// 抽奖日志
};
UNSTD_STATIC_CHECK(sizeof(RATianYiYaoShiParam) <= sizeof(RoleActivityRoleData));


#pragma pack(pop)
#endif