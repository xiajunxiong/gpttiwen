#ifndef __MSG_RA_CHUN_RI_DENG_FENG_HPP__
#define __MSG_RA_CHUN_RI_DENG_FENG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2085  随机活动  定制宝箱  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAChunRiDengFengInfo						//<! 3611 春日登峰
	{
	public:
		SCRAChunRiDengFengInfo();
		MessageHeader header;

		unsigned int layer;				//当前层数
		unsigned int explore_total_times;		//探索总次数							
		unsigned short explore_times;		//没到达第七层得次数，满足配置表得次数直接到第七层
		unsigned short get_item_flag;		//第7层道具只可获得一次，不可重复获得，记录第七层获取什么道具	
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
