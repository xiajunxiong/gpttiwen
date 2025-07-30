#ifndef __MSG_RA_HUN_QI_FAN_LI_HPP__
#define __MSG_RA_HUN_QI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//////////////////////////////////////// 魂器返利 ///////////////////////////////////////////////////
	//!<3000 随机活动请求操作
	enum RA_HUN_QI_FAN_LI_REQ_TYPE						//对应 3000协议的 opera_type
	{
		RA_HUN_QI_FAN_LI_REQ_TYPE_INFO = 0,					// 获取活动信息
		RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY = 1,				// 抽奖
		RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY_COUNT_REWARD = 2,	// 领取抽奖次数奖励, param1-seq
	};

	class SCRAHunQiFanLiInfo							//<! 5220	 随机活动-魂器返利信息
	{
	public:
		SCRAHunQiFanLiInfo();
		MessageHeader header;

		uint32_t m_last_lottery_count;	// 玩家剩余抽奖次数
		int m_total_lottery_count;		// 玩家总抽奖次数
		int m_today_recharge;			// 当日充值金额
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_recharge_reward_flag;		// 充值奖励领取flag
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_lottery_reward_flag;		// 抽奖次数奖励领取flag
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_cur_rotary_table_state;	// 当前轮盘状态, 已抽到过的奖励置为1,全抽完后刷新
	};


	//////////////////////////////////////// 天衣耀世 ///////////////////////////////////////////////////
	//!<3000 随机活动请求操作
	enum RA_TIAN_YI_YAO_SHI_REQ_TYPE						//对应 3000协议的 opera_type
	{
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_INFO = 0,			// 获取活动信息
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_LOTTERY = 1,		// 抽奖
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_TEN_LOTTERY = 2,	// 十连抽
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_BUY = 3,			// 购买抽奖道具礼包 p1-seq
	};

	class SCRATianYiYaoShiInfo				//<! 5221	 随机活动-天衣耀世信息
	{
	public:
		SCRATianYiYaoShiInfo();
		MessageHeader header;

		uint32_t cur_log_tail;		// 当前日志尾部

		uint16_t m_buy_count[MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE];		// 购买礼包次数
		RATianYiYaoShiLOG m_lottery_log[MAX_RA_TIAN_YI_YAO_SHI_LOG];	// 抽奖日志
	};



}

#pragma pack(pop)

#endif  
