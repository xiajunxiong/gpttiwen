#ifndef __MSG_RA_TIGER_LUCKY_MONEY_HPP__
#define __MSG_RA_TIGER_LUCKY_MONEY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratigerluckymoneyparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{

	class SCRATigerLuckyMoneyInfo		//!< 3146 ����-����ѹ��
	{
	public:
		SCRATigerLuckyMoneyInfo();
		MessageHeader header;

		// ѹ��Ǯ�ۼ�����
		int lucky_money_num;

		// ѹ��Ǯ�Ƿ����ȡ,0-������ȡ
		short is_can_receive;

		// ��ҵ��յ�¼ʱ�ȼ�
		short role_level;

		// ÿ�������Ƿ���� [0-RATLM_TASK_TYPE_MAX(9)]
		BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_status;

		// ÿ���������Ƿ�����ȡ[0-RATLM_TASK_TYPE_MAX(9)]
		BitMap<RATLM::RATLM_TASK_TYPE_MAX> task_reward_status;

		// ÿ��������� [0-RATLM_TASK_TYPE_MAX(9)]
		int task_extent[RATLM::RATLM_TASK_TYPE_MAX];
	};

}


#pragma pack(pop)

#endif