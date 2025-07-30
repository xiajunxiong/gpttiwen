#ifndef __MSG_RA_SHEN_SHOU_JIANG_LIN_HPP__
#define __MSG_RA_SHEN_SHOU_JIANG_LIN_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRAShenShouJiangLinInfo						//<! 3580 ���޽��٣�������Ϣ
	{
	public:
		SCRAShenShouJiangLinInfo();
		MessageHeader header;

		int pool_gold_num;
		int pool_item_num;
	};

	struct SCRAShenShouJiangLinDrawResult			 //<! 3581 ���޽��ٽ��
	{
	public:
		SCRAShenShouJiangLinDrawResult();
		MessageHeader header;

		struct Item
		{
			int item_id;
			short num;
			short is_bind;
		};

		int seq;
		int gold_num;
		int item_count;
		Item item[5];
		
	};

	struct SCRAShenShouJiangLinBuyInfo			 //<! 3582 ���޽��ٹ�����Ϣ
	{
	public:
		SCRAShenShouJiangLinBuyInfo();
		MessageHeader header;

		unsigned short ra_day_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
		unsigned short ra_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];				//ÿ����Ʒ�޹�����
	};
}

#pragma pack(pop)

#endif  
