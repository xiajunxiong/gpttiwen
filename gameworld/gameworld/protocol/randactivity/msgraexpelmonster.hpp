#ifndef __MSG_RA_EXPEL_MONSTER_HPP__
#define __MSG_RA_EXPEL_MONSTER_HPP__

#include "servercommon/userprotocal/msgheader.h"

//////////////////////////////////////////// 2055 随机活动  驱逐妖兽  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_EXPEL_MONSTER_OPERA_TYPE
	{
		RA_EXPEL_MONSTER_OPERA_TYPE_BUY = 0,						// 购买商品 param_1 :商品序号
 
	};

	struct SCRAExpelMonsterInfo			//<! 3015 驱除妖兽信息
	{
	public:
		SCRAExpelMonsterInfo();
		MessageHeader header;

		int today_times;				//今天驱除妖兽数量
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_EXPEL_MONSTER_HPP__
