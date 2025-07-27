#ifndef __RA_QI_XI_TE_HUI_PARAM_HPP__
#define __RA_QI_XI_TE_HUI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2209 随机活动 - 七夕特惠  //////////////////////////////////////////

static const int RA_QI_XI_TE_HUI_MAX_BASE_NUM = 32;				//一个时间阶段最多特惠礼包数量
static const int RA_QI_XI_TE_HUI_MAX_PHASE_NUM = 32;			//一个时间阶段最多阶段奖励数量

UNSTD_STATIC_CHECK(RA_QI_XI_TE_HUI_MAX_BASE_NUM % 4 == 0);

#pragma pack(push, 4)

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAQiXiTeHuiParam
{
	RAQiXiTeHuiParam() { this->Reset(); }
	void Reset()
	{
		ra_qi_xi_te_hui_begin_timestamp = 0;
		reserve_ll = 0;
		is_init_open_interface_flag = 0;
		reserve_ch = 0;
		activity_buy_count = 0;
		phase_reward_flag.Reset();
		for (int i = 0; i < ARRAY_LENGTH(today_buy_times); ++i)
		{
			today_buy_times[i] = 0;
		}

		for (int i = 0; i < ARRAY_LENGTH(activity_buy_times); ++i)
		{
			activity_buy_times[i] = 0;
		}
	}

	unsigned int ra_qi_xi_te_hui_begin_timestamp;
	long long reserve_ll;
	char is_init_open_interface_flag;									//是否初始打开过界面 1:已打开
	char reserve_ch;
	short activity_buy_count;											//活动期间购买的总次数
	BitMap<RA_QI_XI_TE_HUI_MAX_PHASE_NUM> phase_reward_flag;			//阶段奖励领取标识
	short today_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//每日限购
	short activity_buy_times[RA_QI_XI_TE_HUI_MAX_BASE_NUM];				//活动限购
};

UNSTD_STATIC_CHECK(sizeof(RAQiXiTeHuiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_QI_XI_TE_HUI_PARAM_HPP__
