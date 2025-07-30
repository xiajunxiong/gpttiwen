#ifndef __MSG_TIAN_MING_HPP__
#define __MSG_TIAN_MING_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/tianmingparam.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCTianMingInfo							//!< 天命系统信息 4015
	{
	public:
		SCTianMingInfo();
		MessageHeader		header;

		int level;									// 天命系统总等级
		int exp;									// 天命系统的经验

		int sys_type_level[TIAN_MING_ADD_ATTR_SYS_TYPE_MAX];	// 每个分项加成当前最高等级
	};
}

#pragma pack(pop)

#endif // __MSG_TIAN_MING_HPP__

