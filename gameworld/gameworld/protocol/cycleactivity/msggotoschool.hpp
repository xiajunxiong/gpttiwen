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
		GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT,		//��֣�p1 ������id
		GOTO_SCHOOL_REQ_TYPE_REQ_SUCC,		// �ɹ� ������
		GOTO_SCHOOL_REQ_TYPE_REQ_FIGHT_SUCC,		// ��� ������
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
		
		int get_reward_times;		//��ȡ��������
	};	
}

#pragma pack(pop)

#endif