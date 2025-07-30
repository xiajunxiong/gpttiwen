#ifndef __MSGPOKER_HPP__
#define __MSGPOKER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push,4)

namespace Protocol
{
	struct CSPokerReq			//!< 2260 ������������
	{
		CSPokerReq();
		MessageHeader header;

		enum ReqOperType		// ��������
		{
			POKER_REQ_MI_YI,				//!< ��ҽ / ����֮�� / Զ��ħڣ
			POKER_REQ_WILD_BOSS,			//!< Ұ��boss
			POKER_REQ_SHAN_HAI,				//!< ɽ������
		};

		enum ReqReason
		{
			POKER_REQ_SELF = 0,				// �����������
			POKER_REQ_SYSTEM = 1,			// ϵͳ��æ����
		};

		int oper_type;

		int role_uid;
		int index;			// [1,5]
		int reason;			// ReqReason

	};
}

#pragma pack(pop)

#endif // !__MSGPOKER_HPP__

