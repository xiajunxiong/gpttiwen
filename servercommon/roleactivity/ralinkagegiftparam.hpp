#ifndef __RA_LINKAGE_GIFT_PARAM_HPP__
#define __RA_LINKAGE_GIFT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2075 随机活动 - 联动送礼  //////////////////////////////////////////

enum LINKAGE_GIFT_FETCH_TYPE
{
	LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH = 0,		// 未达成不可领取
	LINKAGE_GIFT_FETCH_TYPE_CAN_FETCH = 1,			// 已达成未领取
	LINKAGE_GIFT_FETCH_TYPE_ALREADY_FETCH = 2,		// 已领取
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RALinkageGiftParam
{
	RALinkageGiftParam() { this->Reset(); }
	void Reset()
	{
		ra_linkage_gfit_can_receive_timestamp = 0;
		ra_linkage_gift_begin_timestamp = 0;
		ra_linkage_gfit_fetch_flag = LINKAGE_GIFT_FETCH_TYPE_CAN_NOT_FETCH;
		ra_linkage_gift_open_activity_level_seq = 0;
	}

	unsigned int ra_linkage_gfit_can_receive_timestamp;					//可领取时间戳
	unsigned int ra_linkage_gift_begin_timestamp;
	int ra_linkage_gfit_fetch_flag;										//0 未达成不可领取,1已达成未领取,2已领取
	int ra_linkage_gift_open_activity_level_seq;						//活动开启时玩家的等级对应配置的seq
};

UNSTD_STATIC_CHECK(sizeof(RALinkageGiftParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_LINKAGE_GIFT_PARAM_HPP__
