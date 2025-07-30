#ifndef __MSG_GOTO_SCHOOL_HPP__
#define __MSG_GOTO_SCHOOL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/tuzikuaipaodef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	enum GOTO_SCHOOL_REQ_TYPE
	{
		GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT,		//打怪，p1 怪物组id
		GOTO_SCHOOL_REQ_TYPE_REQ_SUCC,		// 成功 发奖励
		GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT_SUCC,		// 打怪 发奖励
	};

	class CSGotoSchoolReq			//!< 5150 
	{
	public:
		CSGotoSchoolReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class SCGotoSchoolInfo			//!< 5151
	{
	public:
		SCGotoSchoolInfo();
		MessageHeader header;
		
		int get_reward_times;		//获取奖励次数
	};	
}

#pragma pack(pop)

#endif