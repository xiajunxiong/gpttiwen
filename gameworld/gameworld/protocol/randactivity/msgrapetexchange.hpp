#ifndef __MSG_RA_PET_EXCHANGE_HPP__
#define __MSG_RA_PET_EXCHANGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapetexchangeparam.hpp"

//////////////////////////////////////////// 2072  ����  ����һ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_PET_EXCHANGE_OPERA_TYPE
	{
		RA_PET_EXCHANGE_OPERA_TYPE_GET_INFO = 0,		//��ȡ��Ϣ
		RA_PET_EXCHANGE_OPERA_TYPE_EXCHANGE = 1,		//�һ� param1:seq
		RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM = 2,		//�̵깺������ param1:seq
		RA_PET_EXCHANGE_OPERA_TYPE_BUY_ITEM_CHECK = 3,	//�̵깺��ֱ����Ʒ��� param1:seq
	};


	class SCRAActivityPetExchangeInfo			//!< 1234 ����һ���·�
	{
	public:
		SCRAActivityPetExchangeInfo();
		MessageHeader header;

		unsigned short ra_pet_exchange_times[MAX_RA_PET_NUM];			//��Ѷһ�����
		unsigned short ra_pet_exchange_today_times[MAX_RA_PET_NUM];		//ÿ���Ѷһ�����
		int ra_shop_buy_times[MAX_RA_PET_EXCHANGE_SHOP_ITEM_NUM];		//�̵��ѹ������(ÿ��������)
	};

	class SCRAPetExchangeSingleInfo				//!< 3192 ����-����һ�������Ϣ�·�
	{
	public:
		SCRAPetExchangeSingleInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			INFO_TYPE_EXCHANGE = 0,			//�һ�		param1:��Ѷһ����� param2:ÿ���Ѷһ�����
			INFO_TYPE_BUY_ITEM = 1,			//�̵깺��	param1:�ѹ������
		};

		short type;
		short seq;
		int param1;
		int param2;
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_PET_EXCHANGE_HPP__
