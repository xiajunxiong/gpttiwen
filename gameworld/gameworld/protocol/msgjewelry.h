#ifndef __MSG_JEWELRY_H__
#define __MSG_JEWELRY_H__

#include "gamedef.h"
#include "servercommon/workshopdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "equipment/jewelry.h"

#pragma pack(push, 4) 

namespace Protocol
{
	class CSJewelryHandleReq
	{
	public:
		CSJewelryHandleReq();		//!< 1740： 灵饰 系统操作请求
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_ALL_INFO_REQ = 0,		//!< 请求所有 灵饰 信息
			OPERA_TYPE_WEAR_JEWELRY = 1,		//!< 请求装备灵饰
			OPERA_TYPE_TAKE_OFF_ONE = 2,		//!< 请求脱下灵饰
		};

		short op_type;		//!< 操作类型

		short param;		//!< 额外参数：0 - 无； 1 - 背包格子ID； 2 - 灵饰装备格ID；
	};

	class SCJewelryAllInform
	{
	public:
		SCJewelryAllInform();		//!< 1741： 装备灵饰信息发送
		MessageHeader header;

		int jewelry_count;		//!< 灵饰 总数
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
	};

	class SCJewelryInfoAlter
	{
	public:
		SCJewelryInfoAlter();		//!< 1742： 单个灵饰信息发送
		MessageHeader header;

		JewelryPro jewelry_info;
	};
}

#pragma pack(pop)

#endif


