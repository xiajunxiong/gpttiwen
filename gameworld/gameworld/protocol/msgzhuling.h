#ifndef __MSG_ZHU_LING_HPP__
#define __MSG_ZHU_LING_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ZHU_LING_REQ_TYPE
	{
		ZHU_LING_REQ_TYPE_UP_LEVEL,			//升级		p1 item_src p2 index,  p3 column,  p4 slot_index  |  消耗 p5  item_src  p6 index, p7  column   p8 slot_index
		ZHU_LING_REQ_TYPE_PUT_ON,			//穿上		p1 item_src p2 index,  p3 column,  p4 slot_index  |  消耗 p5  item_src  p6 index, p7  column   p8 slot_index
		ZHU_LING_REQ_TYPE_TAKE_OFF,			//脱下		p1 item_src p2 index,  p3 column,  p4 slot_index  |  消耗 p5  item_src  p6 index, p7  column   p8 slot_index
	};

	class CSZhuLingReq					//!< 3850
	{
	public:
		CSZhuLingReq();
		MessageHeader		header;

		int req_type;
		short param1;
		short param2;
		short param3;
		short param4;
		short param5;
		short param6;
		short param7;
		short param8;
	};

	class SCZhuLingChangeInfo				//!< 3851			升级，穿戴，脱下 操作后下发
	{
	public:
		SCZhuLingChangeInfo();
		MessageHeader		header;

		short req_type;
		unsigned short item_src;
		unsigned short column;
		unsigned short index;
		unsigned short slot_index;
		unsigned short item_id;
		unsigned short level;

		unsigned short consume_item_src;
		unsigned short consume_column;
		unsigned short consume_index;
		unsigned short consume_slot_index;
	};

	class SCZhuLingDecomposeInfo				//!< 3852			分解物品信息
	{
	public:
		SCZhuLingDecomposeInfo();
		MessageHeader		header;

		struct Item 
		{
			int item_id;
			int num;
		};

		int fen_chen_num;		//粉尘
		int count;
		Item item_list[100];		//获得物品
	};

	class CSZhuLingDecomposeReq				//!< 3853			分解铸灵请求
	{
	public:
		CSZhuLingDecomposeReq();
		MessageHeader		header;

		struct Item
		{
			short column;
			short index;
			int num;
		};

		Item item_list[100];  //不用补齐100个
	};

	class CSZhuLingUpLevelReq				//!< 3854			铸灵升级请求
	{
	public:
		CSZhuLingUpLevelReq();
		MessageHeader		header;

		short item_src; 
		short index;
		short  column;
		short slot_index;
		int buy_item_num;
		unsigned short consume_list[100];  //不用补齐100个
	};


	class SCZhuLingPutOnSuccNotice				//!< 3855			铸灵穿戴请求成功返回
	{
	public:
		SCZhuLingPutOnSuccNotice();
		MessageHeader		header;


		unsigned char p1;
		unsigned char p2;
		unsigned char p3;
		unsigned char p4;
	};

}

#pragma pack(pop)

#endif 

