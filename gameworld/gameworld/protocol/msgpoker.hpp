#ifndef __MSGPOKER_HPP__
#define __MSGPOKER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push,4)

namespace Protocol
{
	struct CSPokerReq			//!< 2260 副本翻牌请求
	{
		CSPokerReq();
		MessageHeader header;

		enum ReqOperType		// 请求类型
		{
			POKER_REQ_MI_YI,				//!< 密医 / 试炼之地 / 远征魔冢
			POKER_REQ_WILD_BOSS,			//!< 野外boss
			POKER_REQ_SHAN_HAI,				//!< 山海降妖
		};

		enum ReqReason
		{
			POKER_REQ_SELF = 0,				// 玩家主动翻牌
			POKER_REQ_SYSTEM = 1,			// 系统帮忙翻牌
		};

		int oper_type;

		int role_uid;
		int index;			// [1,5]
		int reason;			// ReqReason

	};
}

#pragma pack(pop)

#endif // !__MSGPOKER_HPP__

