#ifndef __MSG_RA_HAPPY_UP_HPP__
#define __MSG_RA_HAPPY_UP_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahappyupparam.hpp"

//////////////////////////////////////////// 2060  ����  ��������|ӡ�Ƕһ�  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_HAPPY_UP_OPERA_TYPE
	{
		RA_HAPPY_UP_OPERA_TYPE_INFO = 0,				//��Ϣ�·�	
		RA_HAPPY_UP_OPERA_TYPE_EXCHANGE_ITEM = 1,		//�̵�һ����� param_1:seq   param_2:count
	};


	struct SCRAHappyUpInfo								// 3017 ����������Ϣ�·�
	{
	public:
		SCRAHappyUpInfo();
		MessageHeader header;

		int count;															//��ǰ������ӵ�еĸ���			
		short happy_up_exchange_count[MAX_HAPPY_UP_ITEM_SEQ_NUM];			//��Ҷһ��������ߵ�����
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__
