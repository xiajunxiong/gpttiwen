#ifndef __REALM_BREAK_DEF_HPP__
#define __REALM_BREAK_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_REALM_BREAK_ITEM_NUM = 50;

struct RealmBreakItem
{
	RealmBreakItem() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		is_bind = 0;
		num = 0;
	}

	ItemID item_id;
	short is_bind;
	int num;
};

struct RealmBreakParam
{
	RealmBreakParam() { this->Reset(); }

	void Reset()
	{
		is_old_user_flag = 0;
		is_change_ver_reset_flag = 0;
		cur_seq = 0;
		tuna_num = 0;
		break_exp = 0;
		cumulation_user_exp_count = 0;
		user_cumulation_pratice_time = 0;
		cumulation_user_coin_count = 0;
		last_rand_item_timestamp = 0;
		item_count = 0;
		cur_fight_seq = 0;
		for (int i = 0; i < MAX_REALM_BREAK_ITEM_NUM; ++i)
		{
			cumulation_item_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		last_get_fix_item_timestamp = 0;
		is_reissue = 0;
		memset(reserve_ch, 0, sizeof(reserve_ch));
	}
	void FetchReset()
	{
		cumulation_user_exp_count = 0;
		cumulation_user_coin_count = 0;
		user_cumulation_pratice_time = 0;
		for (int i = 0; i < item_count && i < MAX_REALM_BREAK_ITEM_NUM; ++i)
		{
			cumulation_item_list[i].Reset();
		}
		item_count = 0;
	}

	char is_old_user_flag;							//老玩家第一次登陆不算 1:需要计算
	char is_change_ver_reset_flag;					//需求版本变更重置标识 0:未重置 1:已重置
	short cur_seq;									//当前阶数(对应配置seq) 玩家初始为1阶	
	int tuna_num;									//当前阶段已使用吐纳次数 修改为每天重置(升阶不重置)
	unsigned int break_exp;							//当前灵力值
	unsigned int cumulation_user_exp_count;			//玩家累积的待领取经验值
	unsigned int user_cumulation_pratice_time;		//玩家累积修炼的时间
	unsigned int cumulation_user_coin_count;		//玩家累积的待领取铜币
	unsigned int last_rand_item_timestamp;			//玩家上次随机道具时间戳
	short item_count;
	short cur_fight_seq;
	RealmBreakItem cumulation_item_list[MAX_REALM_BREAK_ITEM_NUM];	//玩家累积的待领取道具列表
	long long reserve_ll[3];

	unsigned int last_get_fix_item_timestamp;		//玩家上次获得固定道具奖励时间戳

	char is_reissue;		// 是否补发过改命丹
	char reserve_ch[3];
};

typedef char RealmBreakParamHex[sizeof(RealmBreakParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RealmBreakParamHex) < 1024);


#pragma pack(pop)

#endif