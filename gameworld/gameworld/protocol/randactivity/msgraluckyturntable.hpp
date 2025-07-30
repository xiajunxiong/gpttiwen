#ifndef __MSG_RA_LUCKY_TURN_TABLE_HPP__
#define __MSG_RA_LUCKY_TURN_TABLE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"

//////////////////////////////////////////// 2076  随机活动  幸运转盘  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_LUCKY_TURNTABLE_REQ_TYPE
	{
		RA_LUCKY_TURNTABLE_REQ_TYPE_INFO = 0,			//请求信息下发	
		RA_LUCKY_TURNTABLE_REQ_TYPE_START = 1,			//请求转盘启动(抽取奖励)
		RA_LUCKY_TURNTABLE_REQ_TYPE_GIVE = 2,			//发放转盘奖励(等待客户端转盘结束回调) param_1 - seq
		RA_LUCKY_TURNTABLE_REQ_TYPE_SKIP = 3,			//请求转动抽取奖励直接给予玩家(跳过动画)
	};

	struct SCRALuckyTurntableInfo			//!< 3032 转盘信息下发
	{
		SCRALuckyTurntableInfo();
		MessageHeader header;

		int last_num;											//剩余次数
		BitMap<MAX_RA_LUCKY_TURNTABLE_TYPE_NUM> flag;			//各个条件任务领取标识
		char schedule[MAX_RA_LUCKY_TURNTABLE_TYPE_NUM];			//各个条件任务的进度
	};

	struct SCRALuckyTurntableReward			//!< 3033 幸运转盘奖励
	{
		SCRALuckyTurntableReward();
		MessageHeader header;

		enum LUCKY_TURN_TABLE_TYPE
		{
			LUCKY_TURN_TABLE_TYPE_START = 0,			//正常播放动画转动下发
			LUCKY_TURN_TABLE_TYPE_SKIP = 1,				//跳过动画
		};

		short index;							//抽取到的index
		short type;	
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_TURN_TABLE_HPP__



