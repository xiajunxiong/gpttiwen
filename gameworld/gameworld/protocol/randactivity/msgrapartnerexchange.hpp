#ifndef __MSG_RA_PARTNER_EXCHANGE_HPP__
#define __MSG_RA_PARTNER_EXCHANGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"

//////////////////////////////////////////// 2070  ����  ���һ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_PARTNER_EXCHANGE_OPERA_TYPE
	{
		RA_PARTNER_EXCHANGE_OPERA_TYPE_GET_INFO = 0,		//��ȡ��Ϣ
		RA_PARTNER_EXCHANGE_OPERA_TYPE_EXCHANGE = 1,		//��ȡÿ�ս���
	};

	class SCRAActivityPartnerExchangeInfo			//!< 1233: ���һ���·�
	{
	public:
		SCRAActivityPartnerExchangeInfo();
		MessageHeader header;

		unsigned short ra_partner_exchange_times[MAX_RA_PARTNER_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_PARTNER_EXCHANGE_HPP__
