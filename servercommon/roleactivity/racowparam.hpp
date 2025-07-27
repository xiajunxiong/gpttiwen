#ifndef __RA_COW_PARAM_HPP__
#define __RA_COW_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

const static int MAX_COW_LOG_TIMES = 20;	//最大记录种类
const static int MAX_MAP_COUNT = 2;		//地图数
const static int MAX_ONE_MAP_INDEX = 23;//一张地图最大格子数
const static int SHOW_TAMP = 5;			//展示时间最大间隔

const int MAX_COST_ITEM_COUNT = 5;//最大消耗道具种类--牛运当头

union KeyChangeUn
{
	int section[2];
	long long key;
};

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RACowLuckyExchangeParam
{
	RACowLuckyExchangeParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		initmacy = 0;
	}

	int open_day;
	unsigned int begin_time;
	int initmacy;
};

UNSTD_STATIC_CHECK(sizeof(RACowLuckyExchangeParam) <= sizeof(RoleActivityRoleData));

struct RACowStoreParam
{
	RACowStoreParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	void DayReset()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;
	short today_buy_times[MAX_COW_LOG_TIMES];//今天购买次数
	short activity_buy_times[MAX_COW_LOG_TIMES];//活动购买次数
};

UNSTD_STATIC_CHECK(sizeof(RACowStoreParam) <= sizeof(RoleActivityRoleData));

struct RACowExchangeParam
{
	RACowExchangeParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		m_unlock_integral = 0;
		m_jifen = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(activity_buy_times, 0, sizeof(activity_buy_times));
	}

	void DayReset()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;
	int m_unlock_integral;//总积分
	int m_jifen;//现有积分
	short today_buy_times[MAX_COW_LOG_TIMES];//今天购买次数
	short activity_buy_times[MAX_COW_LOG_TIMES];//活动购买次数
};

UNSTD_STATIC_CHECK(sizeof(RACowExchangeParam) <= sizeof(RoleActivityRoleData));

struct RAMonpolyGiftParam
{
	RAMonpolyGiftParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		m_show_tamp = 0;
		now_map_id = 0;
		now_map_index = 0;
		has_play_special_plan = 0;
		question_id = 0;
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	void OnDayChnage()
	{
		memset(today_buy_times, 0, sizeof(today_buy_times));
	}

	int open_day;
	unsigned int begin_time;

	unsigned int m_show_tamp;
	int now_map_id;		// 地图id
	int now_map_index;	// 现在在地图的哪一个点
	int has_play_special_plan;//特殊玩法是否执行过了（奖励是否领取过了）
	int question_id;

	short today_buy_times[MAX_COW_LOG_TIMES];//今天购买次数
};

UNSTD_STATIC_CHECK(sizeof(RAMonpolyGiftParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
