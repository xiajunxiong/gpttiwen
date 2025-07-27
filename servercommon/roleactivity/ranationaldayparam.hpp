#ifndef __RA_NATIONAL_DAY_PARAM_HPP__
#define __RA_NATIONAL_DAY_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM = 30;		//觅宝坊店阶段内出售商品数量最大值
static const int MAX_RA_MI_BAO_FANG_DIAN_SHOP_ITEM_NUM = 70;			//觅宝坊店活动内出售商品数量最大值
//static const int MAX_RA_MI_BAO_FANG_DIAN_PHASE_NUM = 2;					//觅宝坊店阶段数量最大值


//觅宝坊店
struct RAMiBaoFangDianParam
{
	RAMiBaoFangDianParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		is_open_today_flag = 0;
		phase = 0;
		cur_use_open_day = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(today_buy_times, 0, sizeof(today_buy_times));
		memset(cur_phase_activity_buy_times, 0, sizeof(cur_phase_activity_buy_times));
	}

	unsigned int ra_begin_timestamp;
	short is_open_today_flag;																//今天是否打开过界面 0:没打开 1：打开过
	short phase;																			//角色阶段记录
	int cur_use_open_day;																	//使用该天数来确定开服区间
	long long reserve_ll[3];
	short today_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];						//每日限购
	short cur_phase_activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_PHASE_SHOP_ITEM_NUM];		//当前阶段的活动限购	
	//short activity_buy_times[MAX_RA_MI_BAO_FANG_DIAN_SHOP_ITEM_NUM];				//活动限购 不再展示上一阶段的购买
};
UNSTD_STATIC_CHECK(sizeof(RAMiBaoFangDianParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif