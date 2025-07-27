#ifndef __RA_CHAO_ZHI_GIFT_DEF_HPP__
#define __RA_CHAO_ZHI_GIFT_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

namespace RAChaoZhiGift
{
	static const int GIFT_MAX = 10;
};

//BUY_TIME_TYPE_CHAO_ZHI_GIFT = 52,					// 超值献礼
//RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT =2171
#pragma pack(push, 4)
//////////////////  个人数据  /////////////////////////////////
struct RoleRAChaoZhiGiftParam
{
	RoleRAChaoZhiGiftParam()
	{
		this->Reset();
	}
	void Reset()
	{
		ra_begin_timestamp = 0;
		hold_ll = 0;
		gift_act_flag = 0;
		memset(gift_info_list, 0, sizeof(gift_info_list));
	}

	unsigned int ra_begin_timestamp;
	long long hold_ll;

	int gift_act_flag;		//礼包激活标志

	struct GiftInfo
	{
		GiftInfo()
		{
			this->Reset();
		}
		void Reset()
		{
			fetch_num = 0;
			hold_int = 0;
		}

		int fetch_num;		//领取数量 0: 未领取
		int hold_int;
	};

	GiftInfo gift_info_list[RAChaoZhiGift::GIFT_MAX];
};

#pragma pack(pop)
#endif