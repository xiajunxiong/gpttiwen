#ifndef __ROLE_CRYSTAL_FB_DEF_H__
#define __ROLE_CRYSTAL_FB_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

const static int CRYSTAL_FB_MAX_LAYER = 6;

const static int CRYSTAL_FB_XI_YOU_MAX_LEVEL = 52;		//稀有副本要记录次数。超过要改数据库
const static int CRYSTAL_FB_XI_LIMIT_ITEM_MAX = 100;		//最大限制物品数量
const static int CRYSTAL_FB_FIRST_FLAG = 5;				// 最大记录 32*5 - 1个副本首通
UNSTD_STATIC_CHECK(CRYSTAL_FB_FIRST_FLAG >= (CRYSTAL_FB_XI_YOU_MAX_LEVEL / 32) + 1);//检测错误，修改需要改动数据库

static const int CRYSTAL_FB_NORMAL_NUM = 30;
static const int CRYSTAL_FB_XI_YOU_NUM = 30;//稀有副本录像数

struct CrystalFBTicketData
{
	enum TICKET_TYPE
	{
		TICKET_TYPE_ENERGY = 1,
		TICKET_TYPE_ITEM = 2,
	};

	CrystalFBTicketData()
	{
		this->Reset();
	}

	void Reset()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		ticket_type = 0;
		reserve_sh = 0;
		consume_energy = 0;
		consume_item_id = 0;
		consume_item_num = 0;
		consume_item_is_bind = 0;
	}

	void ResetTicketInfo()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		ticket_type = 0;
		reserve_sh = 0;
		consume_energy = 0;
		consume_item_id = 0;
		consume_item_num = 0;
		consume_item_is_bind = 0;
	}

	char has_give_ticket;		//!< 是否给了门票
	char has_get_boss_reward;	//!< 是否获得了Boss奖励
	char ticket_type;			//!< 门票类型 1:体力 2:物品
	char reserve_sh;			//!< 保留字
	short consume_energy;		//!< 消耗的体力数
	ItemID consume_item_id;		//!< 消耗的物品ID
	short consume_item_num;		//!< 消耗的物品数量
	short consume_item_is_bind;	//!< 消耗的物品是否绑定
};

struct CrystalFBNormalRecord
{
	CrystalFBNormalRecord()
	{
		this->Reset();
	}

	void Reset()
	{
		fb_seq = -1;
		times = 0;
	}

	short fb_seq;
	short times;
};

struct RoleCrystalParam
{
	RoleCrystalParam()
	{
		this->Reset();
	}

	void Reset()
	{
		pu_tong_jing_ying_play_times = 0;
		memset(xi_you_fb_play_times, 0, sizeof(xi_you_fb_play_times));
		memset(frist_pass_flag, 0, sizeof(frist_pass_flag));

		role_enter_energy = 0;
		next_energy_reply_tamp = 0;
		frist_fun_open = 0;
		change_flag = 0;
		buy_energy_times = 0;

		for (int i = 0; i < CRYSTAL_FB_NORMAL_NUM; ++i)
		{
			normal_fb_record_list[i].Reset();
			normal_fb_buy_times_list[i].Reset();
		}

		new_jing_ying_play_times = 0;
		new_xi_you_play_times = 0;
		use_xi_you_play_times = 0;
		memset(frist_pass_reward, 0, sizeof(frist_pass_reward));
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_syn_xi_you_play_times = 0;
		ondaychange_flag = 0;
		reserve_sh = 0;
		get_xi_you_play_times = 0;
		last_full_energy_timestamp = 0;
		overflow_energy = 0;
	}

	void DayReset()
	{
		pu_tong_jing_ying_play_times = 0;

		for (int i = 0; i < ARRAY_ITEM_COUNT(xi_you_fb_play_times); ++i)
		{
			xi_you_fb_play_times[i] = 0;
		}

		for (int i = 0; i < CRYSTAL_FB_NORMAL_NUM; ++i)
		{
			normal_fb_record_list[i].Reset();
			normal_fb_buy_times_list[i].Reset();
		}

		new_jing_ying_play_times = 0;
		new_xi_you_play_times = 0;
		use_xi_you_play_times = 0;
		get_xi_you_play_times = 0;
		ondaychange_flag = 0;
	}

	int pu_tong_jing_ying_play_times;
	unsigned char xi_you_fb_play_times[CRYSTAL_FB_XI_YOU_MAX_LEVEL];
	unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];

	int role_enter_energy;
	unsigned int next_energy_reply_tamp;
	int frist_fun_open;
	short change_flag;
	short buy_energy_times;

	CrystalFBTicketData fb_ticket_data;

	CrystalFBNormalRecord normal_fb_record_list[CRYSTAL_FB_NORMAL_NUM];			//挑战次数
	CrystalFBNormalRecord normal_fb_buy_times_list[CRYSTAL_FB_NORMAL_NUM];		//额外购买次数

	int new_jing_ying_play_times;
	int new_xi_you_play_times;				 //有多少次
	int use_xi_you_play_times;				 //使用了多少次

	unsigned int frist_pass_reward[CRYSTAL_FB_FIRST_FLAG];
	long long reserve_ll[2];

	unsigned int last_full_energy_timestamp;	// 玩家最后一次满体力的时间戳
	int overflow_energy;						// 溢出体力

	char is_syn_xi_you_play_times;			//老号稀有次数同步标识 0:未同步 1:已同步
	char ondaychange_flag;
	short reserve_sh;
	int get_xi_you_play_times;				//稀有副本当天获得奖励次数(精英试炼)
};

struct DrawCrystalPoker
{
	DrawCrystalPoker() { this->Reset(); }

	bool Serialize(TLVSerializer2 & outstream) const ;
	void Reset()
	{
		index = -1;
		role_uid = 0;
		memset(role_name, 0, sizeof(role_name));
		reward_info.Reset();
	}

	int index;
	int role_uid;
	GameName role_name;
	ItemDrawInfo reward_info;
};


typedef char RoleCrystalParamHex[sizeof(RoleCrystalParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCrystalParamHex) < 2048);

#pragma pack(pop)

#endif // !__SCHOOL_TASK_DEF_H__
