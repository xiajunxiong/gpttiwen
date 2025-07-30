#ifndef __MSG_OFFLINE_EXP_H__
#define __MSG_OFFLINE_EXP_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCOfflineExpAddition
	{
	public:
		SCOfflineExpAddition();		//!< 1755 - 离线经验累积信息
		MessageHeader header;

		int offline_min;			//!< 离线时长 - 单位：分钟（m）
		unsigned int exp_add;		//!< 累积经验
	};

	class CSOfflineExpReceives
	{
	public:
		CSOfflineExpReceives();		//!< 1756 - 离线经验领取奖励
		MessageHeader header;
	};
}

#pragma pack(pop)
#endif