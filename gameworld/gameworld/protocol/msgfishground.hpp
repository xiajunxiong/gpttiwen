#ifndef __MSG_FISH_GROUND_HPP__
#define __MSG_FISH_GROUND_HPP__

#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSFishGroundReq		//!< 1315 µöÓãÇëÇó
	{
	public:
		CSFishGroundReq();
		MessageHeader header;

		enum FISH_GROUND_REQ_TYPE
		{
			FISH_GROUND_REQ_TYPE_START = 0,		//¿ªÊ¼µöÓã
			FISH_GROUND_REQ_TYPE_END = 1,		//½áÊøµöÓã(µöµ½Óã)
			FISH_GROUND_REQ_TYPE_STOP = 2,		//Í£Ö¹µöÓã
		};

		int op_type;
		int param1;
	};

	class SCFishingNotify		//!< 1316 µöÓã¹ã²¥
	{
	public:
		SCFishingNotify();
		MessageHeader header;

		int role_obj_id;
	};

	class SCStopFishingNotify		//!< 1317 ½áÊøµöÓã
	{
	public:
		SCStopFishingNotify();
		MessageHeader header;

		enum STOP_REASON_TYPE
		{
			STOP_REASON_TYPE_FAIL = 0,				//µöÓãÊ§°Ü
			STOP_REASON_TYPE_SUCC = 1,				//µöÓã³É¹¦ param1: item_id param2:num
			STOP_REASON_TYPE_NOT_ENOUGH_TIME = 2,	//µöÓãÊ§°Ü,ËùÐèÊ±¼ä²»×ã

			STOP_REASON_TYPE_MAX
		};

		int role_obj_id;
		int stop_reason;
		int param1;
		int param2;
	};

}

#pragma pack(pop)



#endif	//__MSG_FISH_GROUND_HPP__