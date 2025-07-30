#ifndef __MSG_RA_DISCOUNT_STORE_HPP__
#define __MSG_RA_DISCOUNT_STORE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radiscountstoreparam.hpp"

//////////////////////////////////////////// 2052 ����� --  �ۿ��̵�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_DISCOUNT_STORE_OPERA_TYPE
	{
		RA_DISCOUNT_STORE_OPERA_TYPE_GET_INFO = 0,						// ��ȡ��Ϣ
		RA_DISCOUNT_STORE_OPERA_TYPE_BUY = 1,							// תת�� param_1: ��� param_2: ����	
	};


	class SCRandActivityDiscountStoreInfo									//!<3005  ��ȡ��Ϣ
	{
	public:
		SCRandActivityDiscountStoreInfo();
		MessageHeader header;

		unsigned short ra_discount_store_buy_count[RA_DISCOUNT_STORE_MAX_NUM];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TIME_LIMIT_STORE_HPP__
