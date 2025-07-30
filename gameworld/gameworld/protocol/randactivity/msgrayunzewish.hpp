#ifndef __MSG_RA_YUN_ZE_WISH_HPP__
#define __MSG_RA_YUN_ZE_WISH_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzewishparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRAYunZeWishInfo				//!< 3132 ����-������Ը ��Ϣ�·�
	{
	public:
		SCRAYunZeWishInfo();
		MessageHeader header;

		unsigned int next_refresh_timestamp;		// �´�ˢ�½�������ʱ���
		int is_can_lottery;							// �Ƿ�ɳ齱��0-���ܣ�1-����
		BitMap<YZ_WISH_MAX_REWARD_NUM> is_lock_reward;		// [0-4] ��������, YZ_WISH_MAX_REWARD_NUM = 5
	};

}


#pragma pack(pop)

#endif
