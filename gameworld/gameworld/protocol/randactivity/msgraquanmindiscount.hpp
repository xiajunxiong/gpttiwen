#ifndef __MSG_RA_QUAN_MIN_DISCOUNT_HPP__
#define __MSG_RA_QUAN_MIN_DISCOUNT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"

//////////////////////////////////////////// 2078  ����  ȫ���Ź�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_QUAN_MIN_DISCOUNT_OPERA_TYPE
	{
		RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_GET_INFO = 0,		//��ȡ��Ϣ
		RA_QUAN_MIN_DISCOUNT_OPERA_TYPE_BUY_ITEM = 1,		//������� param1 ����seq
	};

	struct QuanMinDiscountCommodity
	{
		QuanMinDiscountCommodity() { this->Reset(); }

		void Reset()
		{
			discount = 0;
			buy_count = 0;
			all_buy_count = 0;
		}
		char discount;
		unsigned char buy_count;				// ��ҹ������
		short all_buy_count;					// ����Ʒһ���������
	};

	struct SCRAQuanMinDiscountInfo			//<! 3036 ȫ���Ź���Ϣ
	{
	public:
		SCRAQuanMinDiscountInfo();
		MessageHeader header;

		int count;
		QuanMinDiscountCommodity commodity_info[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	};


}

#pragma pack(pop)

#endif  // __MSG_RA_QUAN_MIN_DISCOUNT_HPP__
