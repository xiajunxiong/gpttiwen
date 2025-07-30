#ifndef __MSG_RA_JIAN_DAN_QIN_XIN_HPP__
#define __MSG_RA_JIAN_DAN_QIN_XIN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

//////////////////////////////////////////// 2073  随机活动  剑胆琴心  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_JIAN_DAN_QIN_XIN_OPERA_TYPE
	{
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_INFO = 0,			//剑胆琴心请求信息下发	
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_REQ = 1,		//剑胆琴心挑战关卡    需要param1
		RA_JIAN_DAN_QIN_XIN_OPERA_TYPE_BATTLE_RESULT = 2,	//战斗结果    服务端内部调用 
	};

	struct SCRAJianDanQinxinInfo		//!< 3027 剑胆琴心信息下发
	{
		SCRAJianDanQinxinInfo();
		MessageHeader header;

		int open_day;											//活动开启第几天
		BitMap<MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM> flag;		//前七个为低级难度,后七个为高难度(根据配置seq1)
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_JIAN_DAN_QIN_XIN_HPP__
