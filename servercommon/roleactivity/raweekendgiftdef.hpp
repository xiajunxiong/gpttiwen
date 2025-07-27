#ifndef __RA_WEEKEND_GIFT_DEF_HPP__
#define __RA_WEEKEND_GIFT_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
//RAND_ACTIVITY_TYPE_WEEKEND_GIFT = 2170,			// ��ĩ���
//BUY_TIME_TYPE_WEEKEND_GIFT = 51,					// ��ĩ��� param1: gift_index

namespace RAWeekendGift
{
	static const int ROLE_GIFT_MAX = 5;

};
#pragma pack(push, 4)
//////////////////  ��������  /////////////////////////////////
struct RoleRAWeekendGiftParam
{

	RoleRAWeekendGiftParam()
	{
		this->Reset();
	}
	void Reset()
	{
		ra_begin_timestamp = 0;
		last_chack_day_id = 0;
		hold_ll = 0;

		for (int i = 0; i < RAWeekendGift::ROLE_GIFT_MAX; i++)
		{
			gift_list[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	unsigned int last_chack_day_id;

	long long hold_ll;
	
	struct GiftItem
	{
		GiftItem()
		{
			this->Reset();
		}
		void Reset()
		{
			out_time_s = 0;
			gift_index = 0;
			buy_times = 0;

			hold_int = 0;
		}
		
		const bool IsValid() const { return 0 != out_time_s; };
		unsigned int out_time_s;//ʧЧʱ��

		int gift_index;		//�������
		int buy_times;		//�ѹ������

		int hold_int;
	};
	GiftItem gift_list[RAWeekendGift::ROLE_GIFT_MAX];

	static const bool SortFunnc(const GiftItem & aa, const GiftItem & bb)
	{
		if (aa.out_time_s != bb.out_time_s)
		{
			return aa.out_time_s > bb.out_time_s;
		}
		return aa.gift_index > bb.gift_index;
	}
};

UNSTD_STATIC_CHECK(sizeof(RoleRAWeekendGiftParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)
#endif