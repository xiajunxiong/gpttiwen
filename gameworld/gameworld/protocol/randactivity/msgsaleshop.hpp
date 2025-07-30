#ifndef __MSG_RA_SALE_SHOP_HPP__
#define __MSG_RA_SALE_SHOP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasaleshopparam.hpp"

//////////////////////////////////////////// 2059  ����  �����̵�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_SALE_SHOP_OPERA_TYPE
	{
		SALE_SHOP_TYPE_INFO = 0,				//��Ϣ�·�	param1
		SALE_SHOP_TYPE_BUY						//�������  param1 param 2 param 3
	};


	struct SCRASaleShopInfo						// 3016 �����̵���Ϣ�·�
	{
	public:
		SCRASaleShopInfo();
		MessageHeader header;

		int count;															//��ǰ������ӵ�еĸ���			
		short sale_shop_buy_count[MAX_SALE_SHOP_TYPE_SEQ_NUM];				//��ҵ�����Ŀ�и������ߵ��ѹ�������
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SALE_SHOP_HPP__
