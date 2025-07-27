#ifndef __RA_HAPPY_UP_PARAM_HPP__
#define __RA_HAPPY_UP_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2060 随机活动 - 欣欣向荣/印记兑换  //////////////////////////////////////////

const static int MAX_HAPPY_UP_ITEM_SEQ_NUM = 24;
enum HAPPY_UP_LIMIT_TYPE
{
	HAPPY_UP_LIMIT_TYPE_BEGIN = 0,			//无效
	HAPPY_UP_LIMIT_TYPE_BUY_COUNT = 1,		//总限兑
	HAPPY_UP_LIMIT_TYPE_TODAY = 2,			//每日限兑

	HAPPY_UP_LIMIT_TYPE_MAX,
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAHappyUpParam
{
	RAHappyUpParam() { this->Reset(); }
	void Reset()
	{
		ra_happy_up_begin_timestamp = 0;
		memset(ra_happy_up_exchange_count, 0, sizeof(ra_happy_up_exchange_count));
	}

	unsigned int ra_happy_up_begin_timestamp;
	short ra_happy_up_exchange_count[MAX_HAPPY_UP_ITEM_SEQ_NUM];				//玩家兑换各个道具的数量
};

UNSTD_STATIC_CHECK(sizeof(RAHappyUpParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_HAPPY_UP_PARAM_HPP__
