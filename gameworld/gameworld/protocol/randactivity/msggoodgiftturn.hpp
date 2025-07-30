#ifndef __MSG_GOOD_GIFT_TURN_HPP__
#define __MSG_GOOD_GIFT_TURN_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э�� 3000
	enum RA_GOOD_GIFT_TURN_REQ_TYPE
	{
		RA_GOOD_GIFT_TURN_REQ_TYPE_INFO = 0,			// ������Ϣ�·�
		RA_GOOD_GIFT_TURN_REQ_TYPE_FETCH = 1,			// ������ȡ����
	};

	class SCRAGoodGiftTurnInfo				//!< 3070 ����-����ת����Ϣ�·�
	{
	public:
		SCRAGoodGiftTurnInfo();
		MessageHeader header;

		short fetch_num;					// ʣ����ȡ����
		short fetch_flag;					// ��ȡ������ʶ(������) 
	};

	class SCRAGoodGiftTurnRewardInfo		//!< 3071 ����-����ת����ȡ��Ϣ
	{
	public:
		SCRAGoodGiftTurnRewardInfo();
		MessageHeader header;

		int index;						//��Ӧ���õ�seq1
	};
}

#pragma pack(pop)

#endif	// __MSG_GOOD_GIFT_TURN_HPP__