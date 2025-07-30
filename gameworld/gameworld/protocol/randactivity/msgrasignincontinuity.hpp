#ifndef __MSG_RA_SIGN_IN_CONTINUITY_HPP__
#define __MSG_RA_SIGN_IN_CONTINUITY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasignincontinuityparam.hpp"

//////////////////////////////////////////// 2083  ����  ���Ǻ���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	enum RA_SIGN_IN_CONTINUITY_OP_TYPE
	{
		RA_SIGN_IN_CONTINUITY_OP_TYPE_INFO = 0,				// ��Ϣ�·�
		RA_SIGN_IN_CONTINUITY_OP_TYPE_RECEIVE,				// ��ȡ���� param1-seq
	};


	struct SCRASignInContinuityInfo			//!<��3041 ���Ǻ��������·�
	{
	public:
		SCRASignInContinuityInfo();
		MessageHeader header;

		int login_day;
		BitMap<INT_TYPE_BIT> receive_flag;
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_SIGN_IN_CONTINUITY_HPP__
