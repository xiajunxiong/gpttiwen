#ifndef __MSG_RA_DUAN_YANG_ZI_XUAN_HPP__
#define __MSG_RA_DUAN_YANG_ZI_XUAN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

//////////////////////////////////////////// 2200  ����  ������ѡ  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRADuanYanZiXuanInfo						//<! 5100 ������ѡ��Ϣ
	{
	public:
		SCRADuanYanZiXuanInfo();
		MessageHeader header;

		int is_buy_reward;			//�Ƿ��������
		unsigned int fetch_reward_flag;
		unsigned char select_reward_seq[DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM];		//32 ��ȡ��־����seq
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
