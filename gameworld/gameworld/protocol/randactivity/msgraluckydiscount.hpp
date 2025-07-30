#ifndef __MSG_RA_LUCKY_DISCOUNT_HPP__
#define __MSG_RA_LUCKY_DISCOUNT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckydiscountparam.hpp"

//////////////////////////////////////////// 2071  ����  �����ۿ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_LUCKY_DISCOUNT_OPERA_TYPE
	{
		RA_LUCKY_DISCOUNT_OPERA_TYPE_INFO = 0,					//��Ϣ�·�	
		RA_LUCKY_DISCOUNT_OPERA_TYPE_GET_DISCOUNT = 1,			//��ȡ�ۿ�
		RA_LUCKY_DISCOUNT_OPERA_TYPE_BUY_ITEM = 2,				//������� param_1-����seq param_2-num
	};

	struct SCRALuckyDiscountInfo        // 3031 �����ۿ���Ϣ
	{
	public:
		SCRALuckyDiscountInfo();
		MessageHeader header;

		int this_discount;											//�˴��ۿ�
		char bug_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];			//������� LUCKY_DISCOUNT_COMMODITY_MAX_NUM 24 
		char today_buy_count[LUCKY_DISCOUNT_COMMODITY_MAX_NUM];				//ÿ�չ������
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_DISCOUNT_HPP__


