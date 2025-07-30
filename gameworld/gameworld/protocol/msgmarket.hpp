#ifndef __MSG_MARKET_HPP__
#define __MSG_MARKET_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/marketmanagerparamhpp.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSMarketReq			//!< 2545 �г���������
	{
	public:
		CSMarketReq();
		MessageHeader header;

		enum MARKET_REQ_TYPE
		{
			MARKET_REQ_TYPE_INFO = 0,				//������Ϣ�·�
			MARKET_REQ_TYPE_BUY_ITEM = 1,			//���������	param1:item_id param2:num
			MARKET_REQ_TYPE_SALE_ITEM = 2,			//������������	param1:column  param2:index param3:num
			MARKET_REQ_TYPE_SALE_FU_SHI = 3,		//������۷�ʯ	param1:index
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	class SCMarketAllInfo		//!< 2546 �г�������Ϣ�·�
	{
	public:
		SCMarketAllInfo();
		MessageHeader header;

		int count;
		MarketItemSimpleInfo item_list[MAX_MARKET_ITEM_NUM];
	};

	class SCMarketSingleInfo	//!< 2547 �г�������Ϣ�·�
	{
	public:
		SCMarketSingleInfo();
		MessageHeader header;

		MarketItemSimpleInfo item_info;
	};
	
}

#pragma pack(pop)

#endif	//__MSG_MARKET_HPP__