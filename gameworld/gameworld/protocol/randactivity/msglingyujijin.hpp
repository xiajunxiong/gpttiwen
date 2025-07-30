#ifndef __MSG_RA_LING_YU_JI_JIN_HPP__
#define __MSG_RA_LING_YU_JI_JIN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRALingYuJiJinInfo						//<! 3900 灵玉基金信息
	{
	public:
		SCRALingYuJiJinInfo();
		MessageHeader header;

		int active_flag;				//激活那个阶段的标志
		int fetch_flag[RA_LING_YU_JI_JIN_MAX_PHASE];		//每个阶段领取奖励标志
	};

}

#pragma pack(pop)

#endif  
