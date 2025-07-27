#ifndef __RA_LITTLE_YEAR_GIFT_PARAM_HPP__
#define __RA_LITTLE_YEAR_GIFT_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

const static int MAX_LITTLE_YEAR_GIFT_LOG_TIMES = 20;	//最大记录种类

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RALittleYearGiftParam
{
	RALittleYearGiftParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		thread_id = 0;
		has_get_reward = 0;
	}

	void DayReset()
	{
		thread_id = 0;
		has_get_reward = 0;
	}

	int open_day;
	unsigned int begin_time;
	int thread_id;
	int has_get_reward;
};

#pragma pack(pop)


#endif	//__RA_LITTLE_YEAR_GIFT_PARAM_HPP__
