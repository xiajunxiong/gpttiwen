#ifndef __DREAM_NOTES_CHALLENGE_DEF_HPP__
#define __DREAM_NOTES_CHALLENGE_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_DREAM_NOTES_REISSUE_MAIL_NUM = 7;			// 梦渊笔录多日未登陆补发邮件数最大值

struct DreamNotesChallengeTicketData
{
	DreamNotesChallengeTicketData() { this->Reset(); }

	void Reset()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		consume_energy = 0;
		reserve_int = 0;
	}

	void ResetTicketInfo()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		consume_energy = 0;
		reserve_int = 0;
	}

	char has_give_ticket;		//!< 是否给了门票
	char has_get_boss_reward;	//!< 是否获得了Boss奖励
	short consume_energy;		//!< 消耗的体力数
	int reserve_int;	
};


struct DreamNotesChallengeParam
{
	DreamNotesChallengeParam() { this->Reset(); }

	void Reset()
	{
		pass_level = 0;
		remaining_times = 0;
		pass_new_level_timestamp = 0;
		is_set_times = 0;
		reserve_ch = 0;
		buy_times = 0;
		use_item_add_times = 0;
		reserve_sh = 0;
		ticket_data.Reset();
	}

	short pass_level;							// 当前通关过的最高关卡
	short remaining_times;						// 今天剩余次数
	unsigned int pass_new_level_timestamp;		// 通过新关卡的时间戳
	char is_set_times;							// 0:未设置 1:已设置
	char reserve_ch;
	short buy_times;							// 购买的扫荡次数
	short use_item_add_times;					// 使用道具增加的次数
	short reserve_sh;
	DreamNotesChallengeTicketData ticket_data;	
};

typedef char DreamNotesChallengeParamHex[sizeof(DreamNotesChallengeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DreamNotesChallengeParamHex) < 256);

#pragma pack(pop)

#endif