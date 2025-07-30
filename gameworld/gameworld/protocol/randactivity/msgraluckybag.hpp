#ifndef __MSG_RA_LUCKY_BAG_HPP__
#define __MSG_RA_LUCKY_BAG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckybagparam.hpp"

//////////////////////////////////////////// 2062  ����  ����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_LUCKY_BAG_OPERA_TYPE
	{
		RA_LUCKY_BAG_OPERA_TYPE_GET_INFO = 0,				//��ȡ��Ϣ
		RA_LUCKY_BAG_OPERA_TYPE_FETCH_DAY_REWARD = 1,		//��ȡÿ�ս���
	};

	struct SCRALuckyBagInfo						//<! 3020 ������Ϣ
	{
	public:
		SCRALuckyBagInfo();
		MessageHeader header;

		int ra_lucky_bag_buy_flag;				//����ÿ����ĵ�λ
		int ra_lucky_bag_is_fetch_reward;		//�Ƿ���ȡÿ�ս���
		unsigned int batch_buy_end_time;		//һ���������ʱ��
		unsigned int batch_buy_last_time;		//һ�������ϴ���ȡ����ʱ��
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
