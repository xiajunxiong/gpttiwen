#ifndef __MSG_RA_TIME_LIMIT_STORE_HPP__
#define __MSG_RA_TIME_LIMIT_STORE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratimelimitstoreparam.hpp"

//////////////////////////////////////////// 2049 ����  ��ʱ�̵�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_TIME_LIMIT_STORE_OPERA_TYPE
	{
		RA_TIME_LIMIT_STORE_OPERA_TYPE_GET_INFO = 0,			// ��ȡ��Ϣ
		RA_TIME_LIMIT_STORE_OPERA_TYPE_BUY = 1,					// ���� param_1: �������Ʒ���  param_2: ��������� 					
	};

	class SCRandActivityTimeLimitStoreInfo						//!<3001  ��ȡ��Ϣ
	{
	public:
		SCRandActivityTimeLimitStoreInfo();
		MessageHeader header;

		unsigned short ra_time_limit_store_buy_count[RA_TIME_LIMIT_STORE_MAX_NUM];		//!< RA_TIME_LIMIT_STORE_MAX_NUM:12
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TIME_LIMIT_STORE_HPP__
