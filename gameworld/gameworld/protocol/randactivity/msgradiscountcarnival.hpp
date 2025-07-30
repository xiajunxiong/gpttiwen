#ifndef __MSG_RA_DISCOUNT_CARNIVAL_HPP__
#define __MSG_RA_DISCOUNT_CARNIVAL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/radiscountcarnivalparam.hpp"

//////////////////////////////////////////// 2069  ����  �ۿۿ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_DISCOUNT_CARNIVAL_OPERA_TYPE
	{
		RA_DISCOUNT_CARNIVAL_OPERA_TYPE_INFO = 0,		//��Ϣ�·�	
		RA_DISCOUNT_CARNIVAL_OPERA_TYPE_BUY_ITEM = 1,	//�̵깺����� param_1 ����seq   param_2 ʹ��ȯID
	};

	struct SCRADiscountCarnivalInfo        // 3030 �ۿۿ���Ϣ
	{
	public:
		SCRADiscountCarnivalInfo();
		MessageHeader header;

		unsigned char buy_count[DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM];		//������� DISCOUNT_CARNIVAL_COMMODITY_MAX_NUM 16  
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_DISCOUNT_CARNIVAL_HPP__


