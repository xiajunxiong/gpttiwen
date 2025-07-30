#ifndef __MSG_GATHER_FB_H__
#define __MSG_GATHER_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolegatherfbdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCGatherFbInfo				//!<2510
	{
	public:
		SCGatherFbInfo();
		MessageHeader		header;

		int gather_power;		//ÓµÓÐ»îÁ¦
	};

	class SCGatherFbGatherListInfo			//!< 2513
	{
	public:
		SCGatherFbGatherListInfo();
		MessageHeader header;

		int list_num;
		ItemPairCon gather_list[GATHER_LIST_ITEM_TYPE_MAX];
	};
}

#pragma pack(pop)
#endif