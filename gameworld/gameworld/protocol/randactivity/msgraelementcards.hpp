#ifndef __MSG_RA_ELEMENT_CARDS_HPP__
#define __MSG_RA_ELEMENT_CARDS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "protocol/randactivity/msgradiscountstore.hpp"

//////////////////////////////////////////// 2082 ����� --  Ԫ�ط���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_ELEMENT_CARDS_OP_TYPE
	{
		RA_ELEMENT_CARDS_OP_TYPE_INFO = 0,						// ��Ϣ�·�
		RA_ELEMENT_CARDS_OP_TYPE_FLOP = 1,						// ����
	};

	struct SCRAElementCardsInfo									//!<��3040 Ԫ�ط��������·�
	{
	public:
		SCRAElementCardsInfo();
		MessageHeader header;

		short flop_seq;
		short flop_count;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ELEMENT_CARDS_HPP__
