#ifndef __MSG_RA_GOD_BEAST_ADVENT_HPP__
#define __MSG_RA_GOD_BEAST_ADVENT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	class SCGodBeastAdventInfo				//!<3160 随机活动-异兽临世基础信息
	{
	public:
		SCGodBeastAdventInfo();
		MessageHeader header;

		short zhigou_flag;			// 是否购买了活动门票
		short reserve_sh;

		unsigned int cur_like;			// 当前好感度

		///////// 任务相关 //////////
		BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> today_task_finish;		// 每日任务是否完成
		BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> is_receive_reward;		// 每日任务奖励是否已领取
		int today_task_count[MAX_TASK_NUM_GOD_BEAST_ADVENT];		// 每日任务进度

		///////// 兑换相关 /////////
		short buy_day_limit[MAX_REWARD_GROUP_GOD_BEAST_ADVENT];			// 限购道具兑换次数
	};

	class SCGodBeastAdventOpenInfo			//!<3162 随机活动-异兽临世开启状态信息
	{
	public:
		SCGodBeastAdventOpenInfo();
		MessageHeader header;

		int open_flag;		// 开启标记
		int seq;			// 期数索引
	};

	class SCGodBeastAdventBuyInfo		//!<3163 随机活动-异兽临世全服限购道具数量信息
	{
	public:
		SCGodBeastAdventBuyInfo();
		MessageHeader header;

		int last_buy_limit;		// 剩余可兑换数量
	};

	class SCGodBeastAdventNotifyInfo			//3169 随机活动-异兽临世好感度变动通知
	{
	public:
		SCGodBeastAdventNotifyInfo();
		MessageHeader header;

		int add_like;
	};

}

#pragma pack(pop)

#endif