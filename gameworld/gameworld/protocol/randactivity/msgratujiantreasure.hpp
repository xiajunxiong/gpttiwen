#ifndef __MSG_RA_TU_JIAN_TREASURE_HPP__
#define __MSG_RA_TU_JIAN_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratujiantreasureparam.hpp"

//////////////////////////////////////////// 2080  随机活动  图鉴夺宝  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// 由协议  3000
	enum RA_TU_JIAN_TREASURE_OPERA_TYPE
	{
		RA_TU_JIAN_TREASURE_OPERA_TYPE_INFO = 0,		//信息下发	
		RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH_DAY = 1,	//领取天数奖励	param1:seq
		RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH = 2,		//领取奖励	param1:seq
	};


	struct RATuJianTreasureInfo
	{
		RATuJianTreasureInfo() { this->Reset(); }
		void Reset()
		{
			seq = 0;
			is_fetch = 0;
			count = 0;
		}

		char seq;
		char is_fetch;						// 1:已领取 0:未领取
		short count;
	};

	struct SCRATuJianTreasureInfo			//!<　3038 图鉴夺宝数据下发
	{
	public:
		SCRATuJianTreasureInfo();
		MessageHeader header;

		unsigned int end_timestamp;										//玩家活动结束时间
		int login_day;
		BitMap<RA_TU_JIAN_TREASURE_LOGIN_MAXNUM> day_fetch_flag;		//天数领取标志 RA_TU_JIAN_TREASURE_LOGIN_MAXNUM 32
		int count;
		RATuJianTreasureInfo info[RA_TU_JIAN_TREASURE_MAXNUM];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TU_JIAN_TREASURE_HPP__


