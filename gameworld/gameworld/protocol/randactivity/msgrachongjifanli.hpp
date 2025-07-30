#ifndef __MSG_RA_CHONG_JI_FAN_LI_HPP__
#define __MSG_RA_CHONG_JI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rachongjifanliparam.hpp"

//////////////////////////////////////////// 2209  随机活动  宠技返利  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	//3000协议转接
	enum RA_CHONG_JI_FAN_LI_REQ_TYPE
	{
		RA_CHONG_JI_FAN_LI_REQ_TYPE_INFO = 0,		// 请求信息
		RA_CHONG_JI_FAN_LI_REQ_TYPE_FETCH = 1,		// 领取奖励 P1:seq
	};

	class SCRAChongJiFanLiInfo						//<! 3187 随机活动-宠技返利信息下发
	{
	public:
		SCRAChongJiFanLiInfo();
		MessageHeader header;

		char is_day_meet_flag;			//当天是否已经满足条件过 0:还未 1:已满足
		char reserve_ch;
		short reserve_sh;
		int meet_cond_day_num;				//满足完成条件天数
		int day_chongzhi_num;				//当天充值的元宝数量
		BitMap<MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM> reward_fetch_flag;  //奖励领取 标识
	};
	
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
