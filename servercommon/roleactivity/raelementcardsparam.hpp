#ifndef __RA_ELEMENT_CARDS_PARAM_HPP__
#define __RA_ELEMENT_CARDS_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2082 随机活动 - 元素翻牌  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAElementCardsParam
{
	RAElementCardsParam() { this->Reset(); }
	void Reset()
	{
		ra_element_cards_begin_timestamp = 0;
		ra_element_cards_count = 0;
		ra_element_cards_last_seq = -1;
		ra_element_cards_day_id = 0;
	}

	unsigned int ra_element_cards_begin_timestamp;
	short ra_element_cards_count;
	short ra_element_cards_last_seq;
	unsigned int ra_element_cards_day_id;
};

UNSTD_STATIC_CHECK(sizeof(RAElementCardsParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_ELEMENT_CARDS_PARAM_HPP__
