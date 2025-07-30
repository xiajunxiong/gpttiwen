#ifndef __MSG_RA_TRADE_MARKET_RETURN_GOLD_HPP__
#define __MSG_RA_TRADE_MARKET_RETURN_GOLD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratrademarketreturngoldparam.hpp"

//////////////////////////////////////////// 2102 ����  �����������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE
	{
		RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_INFO = 0,		//��ȡ��Ϣ	
		RA_TRADE_MARKET_RETURN_GOLD_OPERA_TYPE_CALIM_GOLD = 1,	//��ȡ����Ԫ��
	};

	class SCRATradeMarketReturnGoldInfo			// 3063 �������������Ϣ�·� 
	{
	public:
		SCRATradeMarketReturnGoldInfo();
		MessageHeader header;

		long long total_buy_item_consume_gold;	//!< �ܼƹ�����Ʒ����Ԫ����
		long long total_calaimable_gold;		//!< �ܼƷ���Ԫ����
		long long remain_calaimable_gold;		//!< ʣ�����ȡԪ����
	};

	class SCRATradeMarketReturnHotItemInfo : public IMessageSerializer	//!< 3064 ������������Ʒ�б�
	{
	public:
		SCRATradeMarketReturnHotItemInfo();
		virtual ~SCRATradeMarketReturnHotItemInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);

		ItemDataWrapper hot_item_list[6];				//!< ������ƷID�б� ����6���Ļ�item_idΪ0
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__


