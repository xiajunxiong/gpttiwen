#ifndef __MSG_GIFT_HPP__
#define __MSG_GIFT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum GIFT_REQ_TYPE
	{
		GIFT_REQ_TYPE_GIVE,				//赠予		 p1 role_id p2 item_id p3 num
		GIFT_REQ_TYPE_FETCH,			//领取		 p1 p2 4052协议里的uuid
		GIFT_REQ_TYPE_FLOWER,			//送花		 p1 role_id p2_item_id p3 num p4 is_auto_buy 
		GIFT_REQ_TYPE_BACK_TO_KISS,		//送花回吻   p1 role_id
	};

	class CSGiftReq		//!< 	4050  
	{
	public:
		CSGiftReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
		int p3;
		int p4;
		char str[COMMON_SAVE_GIVE_GIFT_STR_NUM];		
	};

	class SCGiftRoleInfo		//!< 	4051
	{
	public:
		SCGiftRoleInfo();
		MessageHeader header;

		struct ItemInfo			//这种类型物品赠送给某个玩家多少次
		{
			int role_id;			
			short type;
			unsigned short num;
		};

		int count;
		ItemInfo give_info[100];		//数组 赠送信息
	};

	enum SC_GET_GIFT_INFO_TYPE
	{
		SC_GET_GIFT_INFO_TYPE_DEFAULT,
		SC_GET_GIFT_INFO_TYPE_UPDATE,
		SC_GET_GIFT_INFO_TYPE_DEL,
	};

	class SCGetGiftInfo //!< 	4052  
	{
	public:
		SCGetGiftInfo();
		MessageHeader header;

		int type;  //0 默认，1更新，2删除
		int count;
		CommonSaveGiveGiftData gift_info[200];		//数组 
	};


}

#pragma pack(pop)

#endif