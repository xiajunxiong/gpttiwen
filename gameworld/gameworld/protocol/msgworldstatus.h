#ifndef __MSG_WORLD_STATUS_HPP__
#define __MSG_WORLD_STATUS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/global/worldstatusparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCWorldStatusHiddenInfo		//!< 4750
	{
	public:
		SCWorldStatusHiddenInfo();
		MessageHeader header;

		unsigned int hidden_open_server_time; //隐藏服开启时间
	};
}

#pragma pack(pop)

#endif

