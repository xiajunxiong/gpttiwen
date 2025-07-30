#ifndef __MSG_RA_SPORTS_HPP__
#define __MSG_RA_SPORTS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasportsparam.hpp"

//////////////////////////////////////////// 2079  随机活动  竞技场排名奖励  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_SPORTS_OPERA_TYPE
	{
		RA_SPORTS_OPERA_TYPE_INFO = 0,				//获取信息	
		RA_SPORTS_OPERA_TYPE_GET_REWARD = 1,		//获取奖励 param_1 : 奖励序号
	};


	class SCSportsHistroyInfo			// 新服竞技活动下发 
	{
	public:
		SCSportsHistroyInfo();
		MessageHeader header;

		short ra_sports_histroy_top;				//-1是未上榜,排名
		unsigned short ra_sports_histroy_reward;	//二进制，是否领奖
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__


